/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/crashrpt.cpp
// Purpose:     helpers for structured exception handling (SEH)
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
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

#include "wx/dynload.h"

#include "wx/palmos/crashrpt.h"

#include "wx/palmos/wrapwin.h"
#include "wx/palmos/private.h"

#ifndef wxUSE_DBGHELP
    #define wxUSE_DBGHELP 0
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
    }

    ~BusyCursor()
    {
    }

private:
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
}

void wxCrashReportImpl::Output(const wxChar *format, ...)
{
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
    return false;
}

/* static */ BasicType
wxCrashReportImpl::GetBasicType(DWORD64 modBase, DWORD typeIndex)
{
    return BASICTYPE_NOTYPE;
}

/* static */ wxString
wxCrashReportImpl::FormatSimpleValue(BasicType bt,
                                     DWORD64 length,
                                     PVOID pAddress)
{
    wxString s;

    return s;
}

/* static */
wxString wxCrashReportImpl::GetSymbolName(DWORD64 modBase, DWORD dwTypeIndex)
{
    wxString s;

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

    return s;
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

    return s;
}

// return the string containing the symbol of the given symbol
/* static */ wxString
wxCrashReportImpl::FormatAnyValue(PSYMBOL_INFO pSym, void *pVariable)
{
    wxString s;

    return s;
}

// display contents and type of the given variable
/* static */ wxString
wxCrashReportImpl::FormatSymbol(PSYMBOL_INFO pSym, STACKFRAME *sf)
{
    wxString s;

    return s;
}

void
wxCrashReportImpl::OutputSymbol(PSYMBOL_INFO pSymInfo, STACKFRAME  *sf)
{
}

// callback for SymEnumSymbols()
/* static */
BOOL CALLBACK
wxCrashReportImpl::EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,
                                            ULONG         WXUNUSED(SymbolSize),
                                            PVOID         UserContext)
{
    return false;
}

HANDLE
wxCrashReportImpl::OutputBasicContext(EXCEPTION_RECORD *pExceptionRecord,
                                    CONTEXT *pCtx)
{
    return ::GetModuleHandle(szFaultingModule);
}

void wxCrashReportImpl::OutputStack(const CONTEXT *pCtx, int flags)
{
}

void wxCrashReportImpl::OutputGlobals(HANDLE hModule)
{
}

#endif // wxUSE_MINIDUMP

bool wxCrashReportImpl::BindDbgHelpFunctions(const wxDynamicLibrary& dllDbgHelp)
{
    return false;
}

#if !wxUSE_MINIDUMP

/* static */
wxString wxCrashReportImpl::GetExceptionString(DWORD dwCode)
{
    wxString s;

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
    return false;
}

// ----------------------------------------------------------------------------
// wxCrashReport
// ----------------------------------------------------------------------------

/* static */
void wxCrashReport::SetFileName(const wxChar *filename)
{
}

/* static */
const wxChar *wxCrashReport::GetFileName()
{
    return NULL;
}

/* static */
bool wxCrashReport::Generate(int flags)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxApp::OnFatalException() support
// ----------------------------------------------------------------------------

bool wxHandleFatalExceptions(bool doit)
{
    return true;
}

extern unsigned long wxGlobalSEHandler(EXCEPTION_POINTERS *pExcPtrs)
{
    return EXCEPTION_EXECUTE_HANDLER;
}

#else // !wxUSE_ON_FATAL_EXCEPTION

bool wxHandleFatalExceptions(bool WXUNUSED(doit))
{
    return false;
}

#endif // wxUSE_ON_FATAL_EXCEPTION/!wxUSE_ON_FATAL_EXCEPTION

