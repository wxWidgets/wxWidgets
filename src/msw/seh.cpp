/////////////////////////////////////////////////////////////////////////////
// Name:        msw/seh.cpp
// Purpose:     helpers for structured exception handling (SEH)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.07.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    The code in this file is heavily based on Matt Pietrek's column from
    the 2002 issue of MSDN Magazine.
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ON_FATAL_EXCEPTION

#ifndef WX_PRECOMP
#endif  //WX_PRECOMP

#include "wx/datetime.h"
#include "wx/dynload.h"

#include "wx/msw/seh.h"

#include <windows.h>
#include <imagehlp.h>
#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// types of imagehlp.h functions
// ----------------------------------------------------------------------------

typedef DWORD (WINAPI *SymSetOptions_t)(DWORD);
typedef BOOL (WINAPI *SymInitialize_t)(HANDLE, LPSTR, BOOL);
typedef BOOL (WINAPI *StackWalk_t)(DWORD, HANDLE, HANDLE, LPSTACKFRAME,
                                   LPVOID, PREAD_PROCESS_MEMORY_ROUTINE,
                                   PFUNCTION_TABLE_ACCESS_ROUTINE,
                                   PGET_MODULE_BASE_ROUTINE,
                                   PTRANSLATE_ADDRESS_ROUTINE);
typedef BOOL (WINAPI *SymFromAddr_t)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
typedef LPVOID (WINAPI *SymFunctionTableAccess_t)(HANDLE, DWORD);
typedef DWORD (WINAPI *SymGetModuleBase_t)(HANDLE, DWORD);
typedef BOOL (WINAPI *SymGetLineFromAddr_t)(HANDLE, DWORD,
                                            PDWORD, PIMAGEHLP_LINE);

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// the real crash report generator
class wxSEHReportImpl
{
public:
    wxSEHReportImpl(const wxChar *filename);

    bool Generate();

    ~wxSEHReportImpl()
    {
        if ( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle(m_hFile);
        }
    }

private:
    // formatted output to m_hFile
    void Output(const wxChar *format, ...);

    // translate exception code to its symbolic name
    static wxString GetExceptionString(DWORD dwCode);

    // load all the functions we need from dbghelp.dll, return true if all ok
    bool ResolveSymFunctions(const wxDynamicLibrary& dllDbgHelp);

    // map address to module (and also section:offset), retunr true if ok
    static bool GetLogicalAddress(PVOID addr,
                                  PTSTR szModule,
                                  DWORD len,
                                  DWORD& section,
                                  DWORD& offset);


    // show the general information about exception which should be always
    // available
    bool OutputBasicContext(EXCEPTION_RECORD *pExceptionRecord, CONTEXT *pCtx);

    // output the call stack (pCtx may be modified, make copy before call!)
    void OutputStack(CONTEXT *pCtx);


    // the handle of the report file
    HANDLE m_hFile;

    // dynamically loaded dbghelp.dll functions
    #define DECLARE_SYM_FUNCTION(func) func ## _t func

    DECLARE_SYM_FUNCTION(SymSetOptions);
    DECLARE_SYM_FUNCTION(SymInitialize);
    DECLARE_SYM_FUNCTION(StackWalk);
    DECLARE_SYM_FUNCTION(SymFromAddr);
    DECLARE_SYM_FUNCTION(SymFunctionTableAccess);
    DECLARE_SYM_FUNCTION(SymGetModuleBase);
    DECLARE_SYM_FUNCTION(SymGetLineFromAddr);
};

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// global pointer to exception information, only valid inside OnFatalException
extern WXDLLIMPEXP_BASE EXCEPTION_POINTERS *wxGlobalSEInformation = NULL;


// flag telling us whether the application wants to handle exceptions at all
static bool gs_handleExceptions = false;

// the file name where the report about exception is written
static wxChar gs_reportFilename[MAX_PATH];

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSEHReport
// ----------------------------------------------------------------------------

wxSEHReportImpl::wxSEHReportImpl(const wxChar *filename)
{
    m_hFile = ::CreateFile
                (
                    filename,
                    GENERIC_WRITE,
                    0,                          // no sharing
                    NULL,                       // default security
                    CREATE_ALWAYS,
                    FILE_FLAG_WRITE_THROUGH,
                    NULL                        // no template file
                );
}

void wxSEHReportImpl::Output(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    DWORD cbWritten;

    wxString s = wxString::FormatV(format, argptr);
    ::WriteFile(m_hFile, s, s.length() * sizeof(wxChar), &cbWritten, 0);

    va_end(argptr);
}

bool
wxSEHReportImpl::GetLogicalAddress(PVOID addr,
                                   PTSTR szModule,
                                   DWORD len,
                                   DWORD& section,
                                   DWORD& offset)
{
    MEMORY_BASIC_INFORMATION mbi;

    if ( !::VirtualQuery(addr, &mbi, sizeof(mbi)) )
        return false;

    DWORD hMod = (DWORD)mbi.AllocationBase;

    if ( !::GetModuleFileName((HMODULE)hMod, szModule, len) )
        return false;

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

    DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    const DWORD nSections = pNtHdr->FileHeader.NumberOfSections;
    for ( DWORD i = 0; i < nSections; i++, pSection++ )
    {
        DWORD sectionStart = pSection->VirtualAddress;
        DWORD sectionEnd = sectionStart
                    + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section?
        if ( (rva >= sectionStart) && (rva <= sectionEnd) )
        {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i + 1;
            offset = rva - sectionStart;

            return true;
        }
    }

    // failed to map to logical address...
    return false;
}

bool
wxSEHReportImpl::OutputBasicContext(EXCEPTION_RECORD *pExceptionRecord,
                                    CONTEXT *pCtx)
{
    // First print information about the type of fault
    const DWORD dwCode = pExceptionRecord->ExceptionCode;
    Output(_T("Exception code: %s (%#08x)\r\n"),
           GetExceptionString(dwCode).c_str(), dwCode);

    // Now print information about where the fault occured
    TCHAR szFaultingModule[MAX_PATH];
    DWORD section,
          offset;
    void * const pExceptionAddress = pExceptionRecord->ExceptionAddress;
    if ( !GetLogicalAddress(pExceptionAddress,
                            szFaultingModule,
                            WXSIZEOF(szFaultingModule),
                            section, offset) )
    {
        section =
        offset = 0;

        wxStrcpy(szFaultingModule, _T("<< unknown >>"));
    }

    Output(_T("Fault address:  %08x %02x:%08x %s\r\n"),
           pExceptionAddress, section, offset, szFaultingModule);

    // Show the registers
#ifdef _M_IX86
    Output( _T("\r\nRegisters:\r\n") );

    Output(_T("EAX: %08x EBX: %08x ECX: %08x EDX: %08x ESI: %08x EDI: %08x\r\n"),
            pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi);

    Output(_T("CS:EIP: %04x:%08x SS:ESP: %04x:%08x  EBP: %08x\r\n"),
           pCtx->SegCs, pCtx->Eip, pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    Output(_T("DS: %04x  ES: %04x  FS: %04x  GS: %04x\r\n"),
           pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs);
    Output(_T("Flags: %08x\r\n"), pCtx->EFlags );
#endif // _M_IX86

    return true;
}

void wxSEHReportImpl::OutputStack(CONTEXT *pCtx)
{
    Output(_T("\r\nCall stack:\r\n"));

    Output(_T("Address   Frame     Function            SourceFile\r\n"));

    DWORD dwMachineType = 0;

    STACKFRAME sf;
    wxZeroMemory(sf);

#ifdef _M_IX86
    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pCtx->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pCtx->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pCtx->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
#endif // _M_IX86

    const HANDLE hProcess = GetCurrentProcess();
    const HANDLE hThread = GetCurrentThread();

    for ( ;; )
    {
        // Get the next stack frame
        if ( !StackWalk(dwMachineType,
                        hProcess,
                        hThread,
                        &sf,
                        pCtx,
                        0,
                        SymFunctionTableAccess,
                        SymGetModuleBase,
                        0) )
        {
            break;
        }

        // Basic sanity check to make sure the frame is OK.
        if ( !sf.AddrFrame.Offset )
            break;

        Output(_T("%08x  %08x  "), sf.AddrPC.Offset, sf.AddrFrame.Offset);

        // Get the name of the function for this stack frame entry
        BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLen = 1024;
        DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                        // relative to the start of the symbol

        if ( SymFromAddr(hProcess, sf.AddrPC.Offset,
                         &symDisplacement,pSymbol) )
        {
            Output(_T("%hs() + %#08I64x"), pSymbol->Name, symDisplacement);
        }
        else    // No symbol found.  Print out the logical address instead.
        {
            TCHAR szModule[MAX_PATH];
            DWORD section,
                  offset;

            if ( !GetLogicalAddress((PVOID)sf.AddrPC.Offset,
                                    szModule, sizeof(szModule),
                                    section, offset) )
            {
                szModule[0] = _T('\0');
                section =
                offset = 0;
            }

            Output(_T("%04x:%08x %s"), section, offset, szModule);
        }

        // Get the source line for this stack frame entry
        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;
        if ( SymGetLineFromAddr(hProcess, sf.AddrPC.Offset,
                                &dwLineDisplacement, &lineInfo ))
        {
            Output(_T("  %s line %u"), lineInfo.FileName, lineInfo.LineNumber); 
        }

        Output(_T("\r\n"));
    }
}

bool wxSEHReportImpl::ResolveSymFunctions(const wxDynamicLibrary& dllDbgHelp)
{
    #define LOAD_SYM_FUNCTION(name)                                           \
        name = (name ## _t) dllDbgHelp.GetSymbol(#name);                      \
        if ( !name )                                                          \
        {                                                                     \
            Output(_T("\r\nFunction ") __XFILE__(#name)                       \
                   _T("() not found.\r\n"));                                  \
            return false;                                                     \
        }

    LOAD_SYM_FUNCTION(SymSetOptions);
    LOAD_SYM_FUNCTION(SymInitialize);
    LOAD_SYM_FUNCTION(StackWalk);
    LOAD_SYM_FUNCTION(SymFromAddr);
    LOAD_SYM_FUNCTION(SymFunctionTableAccess);
    LOAD_SYM_FUNCTION(SymGetModuleBase);
    LOAD_SYM_FUNCTION(SymGetLineFromAddr);

    #undef LOAD_SYM_FUNCTION

    return true;
}

bool wxSEHReportImpl::Generate()
{
    if ( m_hFile == INVALID_HANDLE_VALUE )
        return false;

    PEXCEPTION_RECORD pExceptionRecord = wxGlobalSEInformation->ExceptionRecord;
    PCONTEXT pCtx = wxGlobalSEInformation->ContextRecord;

    if ( !OutputBasicContext(pExceptionRecord, pCtx) )
        return false;

    // for everything else we need dbghelp.dll
    wxDynamicLibrary dllDbgHelp(_T("dbghelp.dll"), wxDL_VERBATIM);
    if ( dllDbgHelp.IsLoaded() )
    {
        if ( ResolveSymFunctions(dllDbgHelp) )
        {
            SymSetOptions(SYMOPT_DEFERRED_LOADS);

            // Initialize DbgHelp
            if ( SymInitialize(GetCurrentProcess(), NULL, TRUE /* invade */) )
            {
                CONTEXT ctxCopy = *pCtx;

                OutputStack(&ctxCopy);

                return true;
            }
        }
        else
        {
            Output(_T("Please update your dbghelp.dll version, "
                      "at least version 6.0 is needed!\r\n"));
        }
    }
    else
    {
        Output(_T("Please install dbghelp.dll available free of charge ")
               _T("from Microsoft to get more detailed crash information!"));
    }

    Output(_T("\r\nLatest dbghelp.dll is available at "
              "http://www.microsoft.com/whdc/ddk/debugging/\r\n"));

    return true;
}

/* static */
wxString wxSEHReportImpl::GetExceptionString(DWORD dwCode)
{
    wxString s;

    #define CASE_EXCEPTION( x ) case EXCEPTION_##x: s = _T(#x); break

    switch ( dwCode )
    {
        CASE_EXCEPTION(ACCESS_VIOLATION);
        CASE_EXCEPTION(DATATYPE_MISALIGNMENT);
        CASE_EXCEPTION(BREAKPOINT);
        CASE_EXCEPTION(SINGLE_STEP);
        CASE_EXCEPTION(ARRAY_BOUNDS_EXCEEDED);
        CASE_EXCEPTION(FLT_DENORMAL_OPERAND);
        CASE_EXCEPTION(FLT_DIVIDE_BY_ZERO);
        CASE_EXCEPTION(FLT_INEXACT_RESULT);
        CASE_EXCEPTION(FLT_INVALID_OPERATION);
        CASE_EXCEPTION(FLT_OVERFLOW);
        CASE_EXCEPTION(FLT_STACK_CHECK);
        CASE_EXCEPTION(FLT_UNDERFLOW);
        CASE_EXCEPTION(INT_DIVIDE_BY_ZERO);
        CASE_EXCEPTION(INT_OVERFLOW);
        CASE_EXCEPTION(PRIV_INSTRUCTION);
        CASE_EXCEPTION(IN_PAGE_ERROR);
        CASE_EXCEPTION(ILLEGAL_INSTRUCTION);
        CASE_EXCEPTION(NONCONTINUABLE_EXCEPTION);
        CASE_EXCEPTION(STACK_OVERFLOW);
        CASE_EXCEPTION(INVALID_DISPOSITION);
        CASE_EXCEPTION(GUARD_PAGE);
        CASE_EXCEPTION(INVALID_HANDLE);

        default:
            // unknown exception, ask NTDLL for the name
            if ( !::FormatMessage
                    (
                     FORMAT_MESSAGE_IGNORE_INSERTS |
                     FORMAT_MESSAGE_FROM_HMODULE,
                     ::GetModuleHandle(_T("NTDLL.DLL")),
                     dwCode,
                     0,
                     wxStringBuffer(s, 1024),
                     1024,
                     0
                    ) )
            {
                s = _T("UNKNOWN_EXCEPTION");
            }
    }

    #undef CASE_EXCEPTION

    return s;
}

// ----------------------------------------------------------------------------
// wxSEHReport
// ----------------------------------------------------------------------------

/* static */
void wxSEHReport::SetFileName(const wxChar *filename)
{
    wxStrncpy(gs_reportFilename, filename, WXSIZEOF(gs_reportFilename) - 1);
    gs_reportFilename[WXSIZEOF(gs_reportFilename) - 1] = _T('\0');
}

/* static */
const wxChar *wxSEHReport::GetFileName()
{
    return gs_reportFilename;
}

/* static */
bool wxSEHReport::Generate()
{
    wxSEHReportImpl impl(gs_reportFilename);

    return impl.Generate();
}

// ----------------------------------------------------------------------------
// wxApp::OnFatalException() support
// ----------------------------------------------------------------------------

bool wxHandleFatalExceptions(bool doit)
{
    // assume this can only be called from the main thread
    gs_handleExceptions = doit;

    if ( doit )
    {
        // try to find a place where we can put out report file later
        if ( !::GetTempPath
                (
                    WXSIZEOF(gs_reportFilename),
                    gs_reportFilename
                ) )
        {
            wxLogLastError(_T("GetTempPath"));

            // when all else fails...
            wxStrcpy(gs_reportFilename, _T("c:\\"));
        }

        // use PID and date to make the report file name more unique
        wxString fname = wxString::Format
                         (
                            _T("%s_%s_%lu.rpt"),
                            wxTheApp ? wxTheApp->GetAppName().c_str()
                                     : _T("wxwindows"),
                            wxDateTime::Now().Format(_T("%Y%m%d")).c_str(),
                            ::GetCurrentProcessId()
                         );

        wxStrncat(gs_reportFilename, fname,
                  WXSIZEOF(gs_reportFilename) - strlen(gs_reportFilename) - 1);
    }

    return true;
}

extern unsigned long wxGlobalSEHandler(EXCEPTION_POINTERS *pExcPtrs)
{
    if ( gs_handleExceptions && wxTheApp )
    {
        // store the pointer to exception info
        wxGlobalSEInformation = pExcPtrs;

        // give the user a chance to do something special about this
        wxTheApp->OnFatalException();

        wxGlobalSEInformation = NULL;

        // this will execute our handler and terminate the process
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

#else // !wxUSE_ON_FATAL_EXCEPTION

bool wxHandleFatalExceptions(bool WXUNUSED(doit))
{
    wxFAIL_MSG(_T("set wxUSE_ON_FATAL_EXCEPTION to 1 to use this function"));

    return false;
}

#endif // wxUSE_ON_FATAL_EXCEPTION/!wxUSE_ON_FATAL_EXCEPTION

