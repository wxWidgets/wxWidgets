/////////////////////////////////////////////////////////////////////////////
// Name:        msw/crashrpt.cpp
// Purpose:     helpers for structured exception handling (SEH)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.07.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    The code generating the crash reports in this file is heavily based on
    Matt Pietrek's column from the March 2002 issue of MSDN Magazine. Note
    that this code is not currently used by default, however. In any case,
    all bugs are my alone.
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

/*
   We have two possibilities here: one, a priori more interesting, is to
   generate the crash report ourselves and include the values of all the
   variables in the dump. Unfortunately my code to do it doesn't work in
   "real life" situations i.e. it works in small examples but invariably
   gets confused by something in big programs which makes quite useless.

   The other possibility is to let dbghelp.dll to do the work for us and
   analyze its results later using a debugger with knowledge about crash
   dumps, such as (free!) WinDbg. This also has another advantage of not
   needing to ship the .pdb file (containing debug info) to the user. So
   this is the default now, but I keep the old code just in case, and if
   you really want you can still use it.
 */
#define wxUSE_MINIDUMP 1

#if !wxUSE_MINIDUMP
    #include "wx/longlong.h"
#endif // wxUSE_MINIDUMP

#include "wx/datetime.h"

#include "wx/dynlib.h"

#include "wx/msw/crashrpt.h"

#include "wx/msw/wrapwin.h"
#include <imagehlp.h>
#include "wx/msw/private.h"

// we need to determine whether we have the declarations for the function in
// debughlp.dll version 5.81 (at least) and we check for DBHLPAPI to test this
//
// reasons:
//  - VC6 version of imagehlp.h doesn't define it
//  - VC7 one does
//  - testing for compiler version doesn't work as you can install and use
//    the new SDK headers with VC6
//
// in any case, the user may override by defining wxUSE_DBGHELP himself
#ifndef wxUSE_DBGHELP
    #ifdef DBHLPAPI
        #define wxUSE_DBGHELP 1
    #else
        #define wxUSE_DBGHELP 0
    #endif
#endif

#if wxUSE_DBGHELP

// ----------------------------------------------------------------------------
// types of imagehlp.h functions
// ----------------------------------------------------------------------------

#if wxUSE_MINIDUMP

typedef BOOL (WINAPI *MiniDumpWriteDump_t)(HANDLE, DWORD, HANDLE,
                                           MINIDUMP_TYPE,
                                           CONST PMINIDUMP_EXCEPTION_INFORMATION,
                                           CONST PMINIDUMP_USER_STREAM_INFORMATION,
                                           CONST PMINIDUMP_CALLBACK_INFORMATION);
#else // !wxUSE_MINIDUMP
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
typedef BOOL (WINAPI *SymSetContext_t)(HANDLE, PIMAGEHLP_STACK_FRAME,
                                       PIMAGEHLP_CONTEXT);
typedef BOOL (WINAPI *SymEnumSymbols_t)(HANDLE, ULONG64, PCSTR,
                                        PSYM_ENUMERATESYMBOLS_CALLBACK, PVOID);
typedef BOOL (WINAPI *SymGetTypeInfo_t)(HANDLE, DWORD64, ULONG,
                                        IMAGEHLP_SYMBOL_TYPE_INFO, PVOID);
#endif // wxUSE_MINIDUMP

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#if !wxUSE_MINIDUMP

// Stolen from CVCONST.H in the DIA 2.0 SDK
enum BasicType
{
    BASICTYPE_NOTYPE = 0,
    BASICTYPE_VOID = 1,
    BASICTYPE_CHAR = 2,
    BASICTYPE_WCHAR = 3,
    BASICTYPE_INT = 6,
    BASICTYPE_UINT = 7,
    BASICTYPE_FLOAT = 8,
    BASICTYPE_BCD = 9,
    BASICTYPE_BOOL = 10,
    BASICTYPE_LONG = 13,
    BASICTYPE_ULONG = 14,
    BASICTYPE_CURRENCY = 25,
    BASICTYPE_DATE = 26,
    BASICTYPE_VARIANT = 27,
    BASICTYPE_COMPLEX = 28,
    BASICTYPE_BIT = 29,
    BASICTYPE_BSTR = 30,
    BASICTYPE_HRESULT = 31
};

// Same as above
enum SymbolTag
{
    SYMBOL_TAG_NULL,
    SYMBOL_TAG_FUNCTION = 5,
    SYMBOL_TAG_DATA = 7,
    SYMBOL_TAG_PUBLIC = 10,             // appears in .DBGs
    SYMBOL_TAG_UDT,
    SYMBOL_TAG_ENUM,
    SYMBOL_TAG_FUNCTION_TYPE,
    SYMBOL_TAG_POINTER_TYPE,
    SYMBOL_TAG_ARRAY_TYPE,
    SYMBOL_TAG_BASE_TYPE,
    SYMBOL_TAG_TYPEDEF,
    SYMBOL_TAG_BASECLASS
};

#endif // wxUSE_MINIDUMP

#endif // wxUSE_DBGHELP

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// low level wxBusyCursor replacement: we use Win32 API directly here instead
// of going through wxWidgets calls as this could be dangerous
class BusyCursor
{
public:
    BusyCursor()
    {
        HCURSOR hcursorBusy = ::LoadCursor(NULL, IDC_WAIT);
        m_hcursorOld = ::SetCursor(hcursorBusy);
    }

    ~BusyCursor()
    {
        if ( m_hcursorOld )
        {
            ::SetCursor(m_hcursorOld);
        }
    }

private:
    HCURSOR m_hcursorOld;
};

// the real crash report generator
class wxCrashReportImpl
{
public:
    wxCrashReportImpl(const wxChar *filename);

    bool Generate(int flags);

    ~wxCrashReportImpl()
    {
        if ( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle(m_hFile);
        }
    }

private:

    // formatted output to m_hFile
    void Output(const wxChar *format, ...);

    // output end of line
    void OutputEndl() { Output(_T("\r\n")); }

#if wxUSE_DBGHELP

#if !wxUSE_MINIDUMP
    // translate exception code to its symbolic name
    static wxString GetExceptionString(DWORD dwCode);

    // return the type from "type index"
    static BasicType GetBasicType(DWORD64 modBase, DWORD typeIndex);

    // return the name for the type index
    static wxString GetSymbolName(DWORD64 modBase, DWORD dwTypeIndex);

    // return the string representation of the variable value
    static wxString FormatSimpleValue(BasicType bt,
                                      DWORD64 length,
                                      PVOID pAddress);

    // return string representation of a struct field (which may itself be a
    // struct, of course)
    static wxString FormatField(DWORD64 modBase,
                                DWORD dwTypeIndex,
                                void *pVariable,
                                unsigned level);

    // show the name and value of the given symbol
    static wxString FormatSymbol(PSYMBOL_INFO pSym, STACKFRAME *sf);

    // show value described by SYMBOL_INFO located at pVariable
    static wxString FormatAnyValue(PSYMBOL_INFO pSym, void *pVariable);

    // show value of possibly complex (user-defined) type
    static wxString FormatUDT(DWORD64 modBase,
                              DWORD dwTypeIndex,
                              void *pVariable,
                              unsigned level = 0);

    // outputs information about the given symbol
    void OutputSymbol(PSYMBOL_INFO pSymInfo, STACKFRAME *sf);

    // map address to module (and also section:offset), retunr true if ok
    static bool GetLogicalAddress(PVOID addr,
                                  PTSTR szModule,
                                  DWORD len,
                                  DWORD& section,
                                  DWORD& offset);

    // callback used with SymEnumSymbols() to process all variables
    static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,
                                                  ULONG         SymbolSize,
                                                  PVOID         UserContext);


    // show the general information about exception which should be always
    // available
    //
    // returns the module of the handle where the crash occured
    HANDLE OutputBasicContext(EXCEPTION_RECORD *pExceptionRecord, CONTEXT *pCtx);

    // output the call stack and local variables values
    void OutputStack(const CONTEXT *pCtx, int flags);

    // output the global variables values
    void OutputGlobals(HANDLE hModuleCrash);


    // the current stack frame (may be NULL)
    STACKFRAME *m_sfCurrent;
#endif // !wxUSE_MINIDUMP

    // load all the functions we need from dbghelp.dll, return true if all ok
    bool BindDbgHelpFunctions(const wxDynamicLibrary& dllDbgHelp);


    // dynamically loaded dbghelp.dll functions
    #define DECLARE_SYM_FUNCTION(func) static func ## _t func

#if wxUSE_MINIDUMP
    DECLARE_SYM_FUNCTION(MiniDumpWriteDump);
#else // !wxUSE_MINIDUMP
    DECLARE_SYM_FUNCTION(SymSetOptions);
    DECLARE_SYM_FUNCTION(SymInitialize);
    DECLARE_SYM_FUNCTION(StackWalk);
    DECLARE_SYM_FUNCTION(SymFromAddr);
    DECLARE_SYM_FUNCTION(SymFunctionTableAccess);
    DECLARE_SYM_FUNCTION(SymGetModuleBase);
    DECLARE_SYM_FUNCTION(SymGetLineFromAddr);
    DECLARE_SYM_FUNCTION(SymSetContext);
    DECLARE_SYM_FUNCTION(SymEnumSymbols);
    DECLARE_SYM_FUNCTION(SymGetTypeInfo);
#endif // wxUSE_MINIDUMP/!wxUSE_MINIDUMP

    #undef DECLARE_SYM_FUNCTION
#endif // wxUSE_DBGHELP

    // the handle of the report file
    HANDLE m_hFile;
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

#if wxUSE_DBGHELP

#define DEFINE_SYM_FUNCTION(func) func ## _t wxCrashReportImpl::func = 0

#if wxUSE_MINIDUMP
DEFINE_SYM_FUNCTION(MiniDumpWriteDump);
#else // !wxUSE_MINIDUMP
DEFINE_SYM_FUNCTION(SymSetOptions);
DEFINE_SYM_FUNCTION(SymInitialize);
DEFINE_SYM_FUNCTION(StackWalk);
DEFINE_SYM_FUNCTION(SymFromAddr);
DEFINE_SYM_FUNCTION(SymFunctionTableAccess);
DEFINE_SYM_FUNCTION(SymGetModuleBase);
DEFINE_SYM_FUNCTION(SymGetLineFromAddr);
DEFINE_SYM_FUNCTION(SymSetContext);
DEFINE_SYM_FUNCTION(SymEnumSymbols);
DEFINE_SYM_FUNCTION(SymGetTypeInfo);
#endif // wxUSE_MINIDUMP/!wxUSE_MINIDUMP

#undef DEFINE_SYM_FUNCTION

#endif // wxUSE_DBGHELP

// ----------------------------------------------------------------------------
// wxCrashReportImpl
// ----------------------------------------------------------------------------

wxCrashReportImpl::wxCrashReportImpl(const wxChar *filename)
{
#if wxUSE_DBGHELP && !wxUSE_MINIDUMP
    m_sfCurrent = NULL;
#endif // wxUSE_DBGHELP

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

void wxCrashReportImpl::Output(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    DWORD cbWritten;

    wxString s = wxString::FormatV(format, argptr);
    ::WriteFile(m_hFile, s, s.length() * sizeof(wxChar), &cbWritten, 0);

    va_end(argptr);
}

#if wxUSE_DBGHELP

#if !wxUSE_MINIDUMP

bool
wxCrashReportImpl::GetLogicalAddress(PVOID addr,
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

/* static */ BasicType
wxCrashReportImpl::GetBasicType(DWORD64 modBase, DWORD typeIndex)
{
    const HANDLE hProcess = GetCurrentProcess();

    // try the index we have
    BasicType bt;
    if ( SymGetTypeInfo(hProcess, modBase, typeIndex, TI_GET_BASETYPE, &bt) )
    {
        return bt;
    }

    // if failed, try to get the "real" typeid first
    DWORD typeId;
    if ( SymGetTypeInfo(hProcess, modBase, typeIndex, TI_GET_TYPEID, &typeId)
            &&
         (typeId != typeIndex &&
            SymGetTypeInfo(hProcess, modBase, typeId, TI_GET_BASETYPE, &bt)) )
    {
        return bt;
    }

    return BASICTYPE_NOTYPE;
}

/* static */ wxString
wxCrashReportImpl::FormatSimpleValue(BasicType bt,
                                     DWORD64 length,
                                     PVOID pAddress)
{
    wxString s;
    s.reserve(256);

    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if ( length == 1 )
    {
        s.Printf(_T("%#04x"), *(PBYTE)pAddress);
    }
    else if ( length == 2 )
    {
        s.Printf(_T("%#06x"), *(PWORD)pAddress);
    }
    else if ( length == 4 )
    {
        bool handled = false;

        if ( bt == BASICTYPE_FLOAT )
        {
            s.Printf(_T("%f"), *(PFLOAT)pAddress);

            handled = true;
        }
        else if ( bt == BASICTYPE_CHAR )
        {
            static const size_t NUM_CHARS = 32;

            const char * const pc = *(PSTR *)pAddress;
            if ( !::IsBadStringPtrA(pc, NUM_CHARS) )
            {
                s << _T('"') << wxString(pc, wxConvLibc, NUM_CHARS) << _T('"');

                handled = true;
            }
        }

        if ( !handled )
        {
            // treat just as an opaque DWORD
            s.Printf(_T("%#x"), *(PDWORD)pAddress);
        }
    }
    else if ( length == 8 )
    {
        if ( bt == BASICTYPE_FLOAT )
        {
            s.Printf(_T("%lf"), *(double *)pAddress);
        }
        else // opaque 64 bit value
        {
            s.Printf(_T("%#" wxLongLongFmtSpec _T("x")), *(PDWORD *)pAddress);
        }
    }

    return s;
}

/* static */
wxString wxCrashReportImpl::GetSymbolName(DWORD64 modBase, DWORD dwTypeIndex)
{
    wxString s;

    WCHAR *pwszTypeName;
    if ( SymGetTypeInfo
         (
            GetCurrentProcess(),
            modBase,
            dwTypeIndex,
            TI_GET_SYMNAME,
            &pwszTypeName
         ) )
    {
        s = wxConvCurrent->cWC2WX(pwszTypeName);

        ::LocalFree(pwszTypeName);
    }

    return s;
}

// this is called for the struct members/base classes
wxString
wxCrashReportImpl::FormatField(DWORD64 modBase,
                               DWORD dwTypeIndex,
                               void *pVariable,
                               unsigned level)
{
    wxString s;

    // avoid infinite recursion
    if ( level > 10 )
    {
        return s;
    }

    const HANDLE hProcess = GetCurrentProcess();

    DWORD dwTag = 0;
    SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_SYMTAG, &dwTag);

    switch ( dwTag )
    {
        case SYMBOL_TAG_UDT:
        case SYMBOL_TAG_BASECLASS:
            s = FormatUDT(modBase, dwTypeIndex, pVariable, level);
            break;

        case SYMBOL_TAG_FUNCTION:
            // don't show
            break;

        default:
            // try to treat all the rest as data even though it's not clear if
            // it's really a good idea...

            // Get the offset of the child member, relative to its parent
            DWORD dwMemberOffset = 0;
            SymGetTypeInfo(hProcess, modBase, dwTypeIndex,
                           TI_GET_OFFSET, &dwMemberOffset);

            // Get the real "TypeId" of the child.  We need this for the
            // SymGetTypeInfo(TI_GET_LENGTH) call below.
            DWORD typeId;
            if ( !SymGetTypeInfo(hProcess, modBase, dwTypeIndex,
                                 TI_GET_TYPEID, &typeId) )
            {
                typeId = dwTypeIndex;
            }

            // Get the size of the child member
            ULONG64 size;
            SymGetTypeInfo(hProcess, modBase, typeId, TI_GET_LENGTH, &size);

            // Calculate the address of the member
            DWORD_PTR dwFinalOffset = (DWORD_PTR)pVariable + dwMemberOffset;

            BasicType basicType = GetBasicType(modBase, dwTypeIndex);

            s = FormatSimpleValue(basicType, size, (PVOID)dwFinalOffset);
            break;

    }

    if ( s.empty() )
    {
        // don't show if no value -- what for?
        return s;
    }

    return wxString(_T('\t'), level + 1) +
                GetSymbolName(modBase, dwTypeIndex) +
                    _T(" = ") + s + _T("\r\n");
}

// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.
wxString
wxCrashReportImpl::FormatUDT(DWORD64 modBase,
                             DWORD dwTypeIndex,
                             void *pVariable,
                             unsigned level)
{
    wxString s;
    s.reserve(512);
    s = GetSymbolName(modBase, dwTypeIndex) + _T(" {\r\n");

    const HANDLE hProcess = GetCurrentProcess();

    // Determine how many children this type has.
    DWORD dwChildrenCount = 0;
    SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT,
                   &dwChildrenCount);

    // Prepare to get an array of "TypeIds", representing each of the children.
    TI_FINDCHILDREN_PARAMS *children = (TI_FINDCHILDREN_PARAMS *)
        malloc(sizeof(TI_FINDCHILDREN_PARAMS) +
                    (dwChildrenCount - 1)*sizeof(ULONG));
    if ( !children )
        return s;

    children->Count = dwChildrenCount;
    children->Start = 0;

    // Get the array of TypeIds, one for each child type
    if ( !SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN,
                         children) )
    {
        return s;
    }

    // Iterate through all children
    for ( unsigned i = 0; i < dwChildrenCount; i++ )
    {
        s += FormatField(modBase, children->ChildId[i], pVariable, level + 1);
    }

    free(children);

    s << wxString(_T('\t'), level + 1) << _T('}');

    return s;
}

// return the string containing the symbol of the given symbol
/* static */ wxString
wxCrashReportImpl::FormatAnyValue(PSYMBOL_INFO pSym, void *pVariable)
{
    DWORD dwTag = 0;
    SymGetTypeInfo(GetCurrentProcess(), pSym->ModBase, pSym->TypeIndex,
                   TI_GET_SYMTAG, &dwTag);

    wxString s;
    switch ( dwTag )
    {
        case SYMBOL_TAG_FUNCTION:
            break;

        case SYMBOL_TAG_UDT:
        case SYMBOL_TAG_BASECLASS:
            // show UDT recursively
            s = FormatUDT(pSym->ModBase, pSym->TypeIndex, pVariable);
            break;

        default:
            // variable of simple type (but could be array which we don't
            // handle correctly yet...), format it using its type and size
            BasicType bt = GetBasicType(pSym->ModBase, pSym->TypeIndex);

            s = FormatSimpleValue(bt, pSym->Size, pVariable);
            break;

    }

    return s;
}

// display contents and type of the given variable
/* static */ wxString
wxCrashReportImpl::FormatSymbol(PSYMBOL_INFO pSym, STACKFRAME *sf)
{
    wxString s;

    if ( pSym->Tag == SYMBOL_TAG_FUNCTION )
    {
        // If it's a function, don't do anything.
        return s;
    }

    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
    {
        // Don't try to report register variable
        return s;
    }

    s.reserve(512);

    // Indicate if the variable is a local or parameter
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        s += _T("\t[param] ");
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        s += _T("\t[local] ");

    // Will point to the variable's data in memory
    DWORD_PTR pVariable = 0;

    if ( (pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE) && sf )
    {
        pVariable = sf->AddrFrame.Offset;
        pVariable += (DWORD_PTR)pSym->Address;
    }
    else // It must be a global variable
    {
        pVariable = (DWORD_PTR)pSym->Address;
    }

    s << wxString(pSym->Name, wxConvLibc)
      << _T(" = ")
      << FormatAnyValue(pSym, (PVOID)pVariable);

    return s;
}

void
wxCrashReportImpl::OutputSymbol(PSYMBOL_INFO pSymInfo, STACKFRAME  *sf)
{
    wxString s = FormatSymbol(pSymInfo, sf);
    if ( !s.empty() )
    {
        Output(_T("%s\r\n"), s.c_str());
    }
    //else: not an interesting symbol
}

// callback for SymEnumSymbols()
/* static */
BOOL CALLBACK
wxCrashReportImpl::EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,
                                            ULONG         WXUNUSED(SymbolSize),
                                            PVOID         UserContext)
{
    wxCrashReportImpl *self = (wxCrashReportImpl *)UserContext;

    __try
    {
        self->OutputSymbol(pSymInfo, self->m_sfCurrent);
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        self->Output(_T("Can't process symbol %hs\r\n"), pSymInfo->Name);
    }

    // continue with enumeration
    return true;
}

HANDLE
wxCrashReportImpl::OutputBasicContext(EXCEPTION_RECORD *pExceptionRecord,
                                    CONTEXT *pCtx)
{
    // First print information about the type of fault
    const DWORD dwCode = pExceptionRecord->ExceptionCode;
    Output(_T("Exception code: %s (%#10x)\r\n"),
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

#ifdef _M_IX86
    // Show the registers
    Output( _T("\r\nRegisters:\r\n") );

    Output(_T("EAX: %08x EBX: %08x ECX: %08x EDX: %08x ESI: %08x EDI: %08x\r\n"),
            pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi);

    Output(_T("CS:EIP: %04x:%08x SS:ESP: %04x:%08x  EBP: %08x\r\n"),
           pCtx->SegCs, pCtx->Eip, pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    Output(_T("DS: %04x  ES: %04x  FS: %04x  GS: %04x\r\n"),
           pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs);
    Output(_T("Flags: %08x\r\n"), pCtx->EFlags );
#endif // _M_IX86

    return ::GetModuleHandle(szFaultingModule);
}

void wxCrashReportImpl::OutputStack(const CONTEXT *pCtx, int flags)
{
    enum
    {
        Output_Stack,
        Output_Locals,
        Output_Max
#ifndef _M_IX86
        // can't show locals under other architectures
            = Output_Locals
#endif
    };

    for ( int step = 0; step < Output_Max; step++ )
    {
        // don't do things we're not asked for
        if ( (step == Output_Stack) && !(flags & wxCRASH_REPORT_STACK) ||
                (step == Output_Locals) && !(flags & wxCRASH_REPORT_LOCALS) )
        {
            continue;
        }

        // the context is going to be modified below so make a copy
        CONTEXT ctx = *pCtx;

        Output(_T("\r\n%s\r\n")
               _T(" # Address   Frame     Function            SourceFile\r\n"),
               step == Output_Stack ? _T("Call stack") : _T("Local variables"));

        DWORD dwMachineType = 0;

        STACKFRAME sf;
        wxZeroMemory(sf);

#ifdef _M_IX86
        // Initialize the STACKFRAME structure for the first call.  This is
        // only necessary for Intel CPUs, and isn't mentioned in the
        // documentation.
        sf.AddrPC.Offset       = ctx.Eip;
        sf.AddrPC.Mode         = AddrModeFlat;
        sf.AddrStack.Offset    = ctx.Esp;
        sf.AddrStack.Mode      = AddrModeFlat;
        sf.AddrFrame.Offset    = ctx.Ebp;
        sf.AddrFrame.Mode      = AddrModeFlat;

        dwMachineType = IMAGE_FILE_MACHINE_I386;
#endif // _M_IX86

        const HANDLE hProcess = GetCurrentProcess();
        const HANDLE hThread = GetCurrentThread();

        // first show just the call stack
        int frame = 0;
        for ( ;; )
        {
            // Get the next stack frame
            if ( !StackWalk(dwMachineType,
                            hProcess,
                            hThread,
                            &sf,
                            &ctx,
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

            Output(_T("%2d %08x  %08x  "),
                   frame++, sf.AddrPC.Offset, sf.AddrFrame.Offset);

            // Get the name of the function for this stack frame entry
            BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
            pSymbol->SizeOfStruct = sizeof(symbolBuffer);
            pSymbol->MaxNameLen = 1024;

            // Displacement of the input address, relative to the start of the
            // symbol
            DWORD64 symDisplacement = 0;

            if ( SymFromAddr(hProcess, sf.AddrPC.Offset,
                             &symDisplacement,pSymbol) )
            {
                Output(_T("%hs()+%#") wxLongLongFmtSpec _T("x"),
                       pSymbol->Name, symDisplacement);
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
                Output(_T("  %s line %u"),
                       lineInfo.FileName, lineInfo.LineNumber);
            }

            OutputEndl();


#ifdef _M_IX86
            // on the second iteration also show the local variables and
            // parameters
            if ( step == Output_Locals )
            {
                // Use SymSetContext to get just the locals/params for this
                // frame
                IMAGEHLP_STACK_FRAME imagehlpStackFrame;
                imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
                SymSetContext(hProcess, &imagehlpStackFrame, 0);

                // Enumerate the locals/parameters
                m_sfCurrent = &sf;
                SymEnumSymbols(hProcess, 0, 0, EnumerateSymbolsCallback, this);

                OutputEndl();
            }
#endif // _M_IX86
        }
    }
}

void wxCrashReportImpl::OutputGlobals(HANDLE hModule)
{
#ifdef _M_IX86
    Output(_T("\r\nGlobal variables:\r\n"));

    m_sfCurrent = NULL;
    SymEnumSymbols(::GetCurrentProcess(), (DWORD64)hModule, NULL,
                   EnumerateSymbolsCallback, this);
#endif // _M_IX86
}

#endif // wxUSE_MINIDUMP

bool wxCrashReportImpl::BindDbgHelpFunctions(const wxDynamicLibrary& dllDbgHelp)
{
    #define LOAD_SYM_FUNCTION(name)                                           \
        name = (name ## _t) dllDbgHelp.GetSymbol(_T(#name));                  \
        if ( !name )                                                          \
        {                                                                     \
            Output(_T("\r\nFunction ") _T(#name)                              \
                   _T("() not found.\r\n"));                                  \
            return false;                                                     \
        }

#if wxUSE_MINIDUMP
    LOAD_SYM_FUNCTION(MiniDumpWriteDump);
#else // !wxUSE_MINIDUMP
    LOAD_SYM_FUNCTION(SymSetOptions);
    LOAD_SYM_FUNCTION(SymInitialize);
    LOAD_SYM_FUNCTION(StackWalk);
    LOAD_SYM_FUNCTION(SymFromAddr);
    LOAD_SYM_FUNCTION(SymFunctionTableAccess);
    LOAD_SYM_FUNCTION(SymGetModuleBase);
    LOAD_SYM_FUNCTION(SymGetLineFromAddr);
    LOAD_SYM_FUNCTION(SymSetContext);
    LOAD_SYM_FUNCTION(SymEnumSymbols);
    LOAD_SYM_FUNCTION(SymGetTypeInfo);
#endif // wxUSE_MINIDUMP/!wxUSE_MINIDUMP

    #undef LOAD_SYM_FUNCTION

    return true;
}

#if !wxUSE_MINIDUMP

/* static */
wxString wxCrashReportImpl::GetExceptionString(DWORD dwCode)
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

#endif // !wxUSE_MINIDUMP

#endif // wxUSE_DBGHELP

bool wxCrashReportImpl::Generate(
#if wxUSE_DBGHELP
    int flags
#else
    int WXUNUSED(flags)
#endif
)
{
    if ( m_hFile == INVALID_HANDLE_VALUE )
        return false;

#if wxUSE_DBGHELP
    if ( !wxGlobalSEInformation )
        return false;

#if !wxUSE_MINIDUMP
    PEXCEPTION_RECORD pExceptionRecord = wxGlobalSEInformation->ExceptionRecord;
    PCONTEXT pCtx = wxGlobalSEInformation->ContextRecord;

    if ( !pExceptionRecord || !pCtx )
        return false;

    HANDLE hModuleCrash = OutputBasicContext(pExceptionRecord, pCtx);
#endif // !wxUSE_MINIDUMP

    // show to the user that we're doing something...
    BusyCursor busyCursor;

    // user-specified crash report flags override those specified by the
    // programmer
    TCHAR envFlags[64];
    DWORD dwLen = ::GetEnvironmentVariable
                    (
                        _T("WX_CRASH_FLAGS"),
                        envFlags,
                        WXSIZEOF(envFlags)
                    );

    int flagsEnv;
    if ( dwLen && dwLen < WXSIZEOF(envFlags) &&
            wxSscanf(envFlags, _T("%d"), &flagsEnv) == 1 )
    {
        flags = flagsEnv;
    }

    // for everything else we need dbghelp.dll
    wxDynamicLibrary dllDbgHelp(_T("dbghelp.dll"), wxDL_VERBATIM);
    if ( dllDbgHelp.IsLoaded() )
    {
        if ( BindDbgHelpFunctions(dllDbgHelp) )
        {
#if wxUSE_MINIDUMP
            MINIDUMP_EXCEPTION_INFORMATION minidumpExcInfo;

            minidumpExcInfo.ThreadId = ::GetCurrentThreadId();
            minidumpExcInfo.ExceptionPointers = wxGlobalSEInformation;
            minidumpExcInfo.ClientPointers = FALSE; // in our own address space

            // do generate the dump
            MINIDUMP_TYPE dumpFlags;
            if ( flags & wxCRASH_REPORT_LOCALS )
            {
                // the only way to get local variables is to dump the entire
                // process memory space -- but this makes for huge (dozens or
                // even hundreds of Mb) files
                dumpFlags = MiniDumpWithFullMemory;
            }
            else if ( flags & wxCRASH_REPORT_GLOBALS )
            {
                // MiniDumpWriteDump() has the option for dumping just the data
                // segment which contains all globals -- exactly what we need
                dumpFlags = MiniDumpWithDataSegs;
            }
            else // minimal dump
            {
                dumpFlags = MiniDumpNormal;
            }

            if ( !MiniDumpWriteDump
                  (
                    ::GetCurrentProcess(),
                    ::GetCurrentProcessId(),
                    m_hFile,                    // file to write to
                    dumpFlags,                  // kind of dump to craete
                    &minidumpExcInfo,
                    NULL,                       // no extra user-defined data
                    NULL                        // no callbacks
                  ) )
            {
                Output(_T("MiniDumpWriteDump() failed."));

                return false;
            }

            return true;
#else // !wxUSE_MINIDUMP
            SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);

            // Initialize DbgHelp
            if ( ::SymInitialize(GetCurrentProcess(), NULL, TRUE /* invade */) )
            {
                OutputStack(pCtx, flags);

                if ( hModuleCrash && (flags & wxCRASH_REPORT_GLOBALS) )
                {
                    OutputGlobals(hModuleCrash);
                }

                return true;
            }
#endif // !wxUSE_MINIDUMP
        }
        else
        {
            Output(_T("\r\nPlease update your dbghelp.dll version, ")
                   _T("at least version 5.1 is needed!\r\n")
                   _T("(if you already have a new version, please ")
                   _T("put it in the same directory where the program is.)\r\n"));
        }
    }
    else // failed to load dbghelp.dll
    {
        Output(_T("Please install dbghelp.dll available free of charge ")
               _T("from Microsoft to get more detailed crash information!"));
    }

    Output(_T("\r\nLatest dbghelp.dll is available at ")
           _T("http://www.microsoft.com/whdc/ddk/debugging/\r\n"));

#else // !wxUSE_DBGHELP
    Output(_T("Support for crash report generation was not included ")
           _T("in this wxWidgets version."));
#endif // wxUSE_DBGHELP/!wxUSE_DBGHELP

    return false;
}

// ----------------------------------------------------------------------------
// wxCrashReport
// ----------------------------------------------------------------------------

/* static */
void wxCrashReport::SetFileName(const wxChar *filename)
{
    wxStrncpy(gs_reportFilename, filename, WXSIZEOF(gs_reportFilename) - 1);
    gs_reportFilename[WXSIZEOF(gs_reportFilename) - 1] = _T('\0');
}

/* static */
const wxChar *wxCrashReport::GetFileName()
{
    return gs_reportFilename;
}

/* static */
bool wxCrashReport::Generate(int flags)
{
    wxCrashReportImpl impl(gs_reportFilename);

    return impl.Generate(flags);
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
#if wxUSE_MINIDUMP
                            _T("%s_%s_%lu.dmp"),
#else // !wxUSE_MINIDUMP
                            _T("%s_%s_%lu.rpt"),
#endif // wxUSE_MINIDUMP/!wxUSE_MINIDUMP
                            wxTheApp ? wxTheApp->GetAppName().c_str()
                                     : _T("wxwindows"),
                            wxDateTime::Now().Format(_T("%Y%m%d")).c_str(),
                            ::GetCurrentProcessId()
                         );

        wxStrncat(gs_reportFilename, fname,
                  WXSIZEOF(gs_reportFilename) - wxStrlen(gs_reportFilename) - 1);
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
        __try
        {
            wxTheApp->OnFatalException();
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
            // nothing to do here, just ignore the exception inside the
            // exception handler
            ;
        }

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

