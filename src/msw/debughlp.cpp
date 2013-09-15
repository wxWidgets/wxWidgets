/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/debughlp.cpp
// Purpose:     various Win32 debug helpers
// Author:      Vadim Zeitlin
// Modified by: Suzumizaki-kimitaka 2013-04-10
// Created:     2005-01-08 (extracted from crashrpt.cpp)
// Copyright:   (c) 2003-2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/msw/debughlp.h"

#if wxUSE_DBGHELP && wxUSE_DYNLIB_CLASS

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// to prevent recursion which could result from corrupted data we limit
// ourselves to that many levels of embedded fields inside structs
static const unsigned MAX_DUMP_DEPTH = 20;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// error message from Init()
static wxString gs_errMsg;

// ============================================================================
// wxDbgHelpDLL implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static members
// ----------------------------------------------------------------------------

#define DEFINE_SYM_FUNCTION(func, name) \
    wxDbgHelpDLL::func ## _t wxDbgHelpDLL::func = 0

wxDO_FOR_ALL_SYM_FUNCS(DEFINE_SYM_FUNCTION);

#undef DEFINE_SYM_FUNCTION

// ----------------------------------------------------------------------------
// initialization methods
// ----------------------------------------------------------------------------

// load all function we need from the DLL

/* static */
bool wxDbgHelpDLL::BindDbgHelpFunctions(const wxDynamicLibrary& dllDbgHelp)
{
    #define LOAD_SYM_FUNCTION(func, name)                                     \
        wxDbgHelpDLL::func = (wxDbgHelpDLL::func ## _t)                       \
            dllDbgHelp.GetSymbol(wxT(#name));                                 \
        if ( !wxDbgHelpDLL::func )                                            \
        {                                                                     \
            gs_errMsg += wxT("Function ") wxT(#name) wxT("() not found.\n");  \
            return false;                                                     \
        }

    wxDO_FOR_ALL_SYM_FUNCS_REQUIRED(LOAD_SYM_FUNCTION);

    #undef LOAD_SYM_FUNCTION

    #define LOAD_SYM_FUNCTION_OPTIONAL(func, name)                            \
        if ( dllDbgHelp.HasSymbol(wxT(#name)) )                               \
        {                                                                     \
            wxDbgHelpDLL::func = (wxDbgHelpDLL::func ## _t)                   \
                dllDbgHelp.GetSymbol(wxT(#name));                             \
        }

    wxDO_FOR_ALL_SYM_FUNCS_OPTIONAL(LOAD_SYM_FUNCTION_OPTIONAL);

    #undef LOAD_SYM_FUNCTION_CAN_FAIL

    return true;
}

// called by Init() if we hadn't done this before
/* static */
bool wxDbgHelpDLL::DoInit()
{
    wxDynamicLibrary dllDbgHelp(wxT("dbghelp.dll"), wxDL_VERBATIM);
    if ( dllDbgHelp.IsLoaded() )
    {
        if ( BindDbgHelpFunctions(dllDbgHelp) )
        {
            // turn on default options
            DWORD options = wxDbgHelpDLL::SymGetOptions();

            options |= SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_DEBUG;

            wxDbgHelpDLL::SymSetOptions(options);

            dllDbgHelp.Detach();
            return true;
        }

        gs_errMsg += wxT("\nPlease update your dbghelp.dll version, ")
                     wxT("at least version 5.1 is needed!\n")
                     wxT("(if you already have a new version, please ")
                     wxT("put it in the same directory where the program is.)\n");
    }
    else // failed to load dbghelp.dll
    {
        gs_errMsg += wxT("Please install dbghelp.dll available free of charge ")
                     wxT("from Microsoft to get more detailed crash information!");
    }

    gs_errMsg += wxT("\nLatest dbghelp.dll is available at ")
                 wxT("http://www.microsoft.com/whdc/ddk/debugging/\n");

    return false;
}

/* static */
bool wxDbgHelpDLL::Init()
{
    // this flag is -1 until Init() is called for the first time, then it's set
    // to either false or true depending on whether we could load the functions
    static int s_loaded = -1;

    if ( s_loaded == -1 )
    {
        s_loaded = DoInit();
    }

    return s_loaded != 0;
}

// ----------------------------------------------------------------------------
// error handling
// ----------------------------------------------------------------------------

/* static */
const wxString& wxDbgHelpDLL::GetErrorMessage()
{
    return gs_errMsg;
}

/* static */
void wxDbgHelpDLL::LogError(const wxChar *func)
{
    ::OutputDebugString(wxString::Format(wxT("dbghelp: %s() failed: %s\r\n"),
                        func, wxSysErrorMsg(::GetLastError())).t_str());
}

// ----------------------------------------------------------------------------
// data dumping
// ----------------------------------------------------------------------------

static inline
bool
DoGetTypeInfo(DWORD64 base, ULONG ti, IMAGEHLP_SYMBOL_TYPE_INFO type, void *rc)
{
    static HANDLE s_hProcess = ::GetCurrentProcess();

    return wxDbgHelpDLL::SymGetTypeInfo
                         (
                            s_hProcess,
                            base,
                            ti,
                            type,
                            rc
                         ) != 0;
}

static inline
bool
DoGetTypeInfo(wxPSYMBOL_INFO pSym, IMAGEHLP_SYMBOL_TYPE_INFO type, void *rc)
{
    return DoGetTypeInfo(pSym->ModBase, pSym->TypeIndex, type, rc);
}

static inline
wxDbgHelpDLL::BasicType GetBasicType(wxPSYMBOL_INFO pSym)
{
    wxDbgHelpDLL::BasicType bt;
    return DoGetTypeInfo(pSym, TI_GET_BASETYPE, &bt)
            ? bt
            : wxDbgHelpDLL::BASICTYPE_NOTYPE;
}

/* static */
wxString wxDbgHelpDLL::GetSymbolName(wxPSYMBOL_INFO pSym)
{
    wxString s;

    WCHAR *pwszTypeName;
    if ( SymGetTypeInfo
         (
            GetCurrentProcess(),
            pSym->ModBase,
            pSym->TypeIndex,
            TI_GET_SYMNAME,
            &pwszTypeName
         ) )
    {
        s = wxConvCurrent->cWC2WX(pwszTypeName);

        ::LocalFree(pwszTypeName);
    }

    return s;
}

/* static */ wxString
wxDbgHelpDLL::DumpBaseType(BasicType bt, DWORD64 length, PVOID pAddress)
{
    if ( !pAddress )
    {
        return wxT("null");
    }

    if ( ::IsBadReadPtr(pAddress, length) != 0 )
    {
        return wxT("BAD");
    }


    wxString s;
    s.reserve(256);

    if ( length == 1 )
    {
        const BYTE b = *(PBYTE)pAddress;

        if ( bt == BASICTYPE_BOOL )
            s = b ? wxT("true") : wxT("false");
        else
            s.Printf(wxT("%#04x"), b);
    }
    else if ( length == 2 )
    {
        s.Printf(bt == BASICTYPE_UINT ? wxT("%#06x") : wxT("%d"),
                 *(PWORD)pAddress);
    }
    else if ( length == 4 )
    {
        bool handled = false;

        if ( bt == BASICTYPE_FLOAT )
        {
            s.Printf(wxT("%f"), *(PFLOAT)pAddress);

            handled = true;
        }
        else if ( bt == BASICTYPE_CHAR )
        {
            // don't take more than 32 characters of a string
            static const size_t NUM_CHARS = 64;

            const char *pc = *(PSTR *)pAddress;
            if ( ::IsBadStringPtrA(pc, NUM_CHARS) == 0 )
            {
                s += wxT('"');
                for ( size_t n = 0; n < NUM_CHARS && *pc; n++, pc++ )
                {
                    s += *pc;
                }
                s += wxT('"');

                handled = true;
            }
        }

        if ( !handled )
        {
            // treat just as an opaque DWORD
            s.Printf(wxT("%#x"), *(PDWORD)pAddress);
        }
    }
    else if ( length == 8 )
    {
        if ( bt == BASICTYPE_FLOAT )
        {
            s.Printf(wxT("%lf"), *(double *)pAddress);
        }
        else // opaque 64 bit value
        {
            s.Printf("%#" wxLongLongFmtSpec "x", *(wxLongLong_t *)pAddress);
        }
    }

    return s;
}

wxString
wxDbgHelpDLL::DumpField(wxPSYMBOL_INFO pSym, void *pVariable, unsigned level)
{
    wxString s;

    // avoid infinite recursion
    if ( level > MAX_DUMP_DEPTH )
    {
        return s;
    }

    SymbolTag tag = SYMBOL_TAG_NULL;
    if ( !DoGetTypeInfo(pSym, TI_GET_SYMTAG, &tag) )
    {
        return s;
    }

    switch ( tag )
    {
        case SYMBOL_TAG_UDT:
        case SYMBOL_TAG_BASE_CLASS:
            s = DumpUDT(pSym, pVariable, level);
            break;

        case SYMBOL_TAG_DATA:
            if ( !pVariable )
            {
                s = wxT("NULL");
            }
            else // valid location
            {
                wxDbgHelpDLL::DataKind kind;
                if ( !DoGetTypeInfo(pSym, TI_GET_DATAKIND, &kind) ||
                        kind != DATA_MEMBER )
                {
                    // maybe it's a static member? we're not interested in them...
                    break;
                }

                // get the offset of the child member, relative to its parent
                DWORD ofs = 0;
                if ( !DoGetTypeInfo(pSym, TI_GET_OFFSET, &ofs) )
                    break;

                pVariable = (void *)((DWORD_PTR)pVariable + ofs);


                // now pass to the type representing the type of this member
                wxSYMBOL_INFO sym = *pSym;
                if ( !DoGetTypeInfo(pSym, TI_GET_TYPEID, &sym.TypeIndex) )
                    break;

                ULONG64 size;
                DoGetTypeInfo(&sym, TI_GET_LENGTH, &size);

                switch ( DereferenceSymbol(&sym, &pVariable) )
                {
                    case SYMBOL_TAG_BASE_TYPE:
                        {
                            BasicType bt = GetBasicType(&sym);
                            if ( bt )
                            {
                                s = DumpBaseType(bt, size, pVariable);
                            }
                        }
                        break;

                    case SYMBOL_TAG_UDT:
                    case SYMBOL_TAG_BASE_CLASS:
                        s = DumpUDT(&sym, pVariable, level);
                        break;

                    default:
                        // Suppress gcc warnings about unhandled enum values.
                        break;
                }
            }

            if ( !s.empty() )
            {
                s = GetSymbolName(pSym) + wxT(" = ") + s;
            }
            break;

        default:
            // Suppress gcc warnings about unhandled enum values, don't assert
            // to avoid problems during fatal crash generation.
            break;
    }

    if ( !s.empty() )
    {
        s = wxString(wxT('\t'), level + 1) + s + wxT('\n');
    }

    return s;
}

/* static */ wxString
wxDbgHelpDLL::DumpUDT(wxPSYMBOL_INFO pSym, void *pVariable, unsigned level)
{
    wxString s;

    // we have to limit the depth of UDT dumping as otherwise we get in
    // infinite loops trying to dump linked lists... 10 levels seems quite
    // reasonable, full information is in minidump file anyhow
    if ( level > 10 )
        return s;

    s.reserve(512);
    s = GetSymbolName(pSym);

#if !wxUSE_STD_STRING
    // special handling for ubiquitous wxString: although the code below works
    // for it as well, it shows the wxStringBase class and takes 4 lines
    // instead of only one as this branch
    if ( s == wxT("wxString") )
    {
        wxString *ps = (wxString *)pVariable;

        // we can't just dump wxString directly as it could be corrupted or
        // invalid and it could also be locked for writing (i.e. if we're
        // between GetWriteBuf() and UngetWriteBuf() calls) and assert when we
        // try to access it contents using public methods, so instead use our
        // knowledge of its internals
        const wxChar *p = NULL;
        if ( !::IsBadReadPtr(ps, sizeof(wxString)) )
        {
            p = ps->data();
            wxStringData *data = (wxStringData *)p - 1;
            if ( ::IsBadReadPtr(data, sizeof(wxStringData)) ||
                    ::IsBadReadPtr(p, sizeof(wxChar *)*data->nAllocLength) )
            {
                p = NULL; // don't touch this pointer with 10 feet pole
            }
        }

        s << wxT("(\"") << (p ? p : wxT("???")) << wxT(")\"");
    }
    else // any other UDT
#endif // !wxUSE_STD_STRING
    {
        // Determine how many children this type has.
        DWORD dwChildrenCount = 0;
        DoGetTypeInfo(pSym, TI_GET_CHILDRENCOUNT, &dwChildrenCount);

        // Prepare to get an array of "TypeIds", representing each of the children.
        TI_FINDCHILDREN_PARAMS *children = (TI_FINDCHILDREN_PARAMS *)
            malloc(sizeof(TI_FINDCHILDREN_PARAMS) +
                        (dwChildrenCount - 1)*sizeof(ULONG));
        if ( !children )
            return s;

        children->Count = dwChildrenCount;
        children->Start = 0;

        // Get the array of TypeIds, one for each child type
        if ( !DoGetTypeInfo(pSym, TI_FINDCHILDREN, children) )
        {
            free(children);
            return s;
        }

        s << wxT(" {\n");

        // Iterate through all children
        wxSYMBOL_INFO sym;
        wxZeroMemory(sym);
        sym.ModBase = pSym->ModBase;
        for ( unsigned i = 0; i < dwChildrenCount; i++ )
        {
            sym.TypeIndex = children->ChildId[i];

            // children here are in lexicographic sense, i.e. we get all our nested
            // classes and not only our member fields, but we can't get the values
            // for the members of the nested classes, of course!
            DWORD nested;
            if ( DoGetTypeInfo(&sym, TI_GET_NESTED, &nested) && nested )
                continue;

            // avoid infinite recursion: this does seem to happen sometimes with
            // complex typedefs...
            if ( sym.TypeIndex == pSym->TypeIndex )
                continue;

            s += DumpField(&sym, pVariable, level + 1);
        }

        free(children);

        s << wxString(wxT('\t'), level + 1) << wxT('}');
    }

    return s;
}

/* static */
wxDbgHelpDLL::SymbolTag
wxDbgHelpDLL::DereferenceSymbol(wxPSYMBOL_INFO pSym, void **ppData)
{
    SymbolTag tag = SYMBOL_TAG_NULL;
    for ( ;; )
    {
        if ( !DoGetTypeInfo(pSym, TI_GET_SYMTAG, &tag) )
            break;

        if ( tag != SYMBOL_TAG_POINTER_TYPE )
            break;

        ULONG tiNew;
        if ( !DoGetTypeInfo(pSym, TI_GET_TYPEID, &tiNew) ||
                tiNew == pSym->TypeIndex )
            break;

        pSym->TypeIndex = tiNew;

        // remove one level of indirection except for the char strings: we want
        // to dump "char *" and not a single "char" for them
        if ( ppData && *ppData && GetBasicType(pSym) != BASICTYPE_CHAR )
        {
            DWORD_PTR *pData = (DWORD_PTR *)*ppData;

            if ( ::IsBadReadPtr(pData, sizeof(DWORD_PTR *)) )
            {
                break;
            }

            *ppData = (void *)*pData;
        }
    }

    return tag;
}

/* static */ wxString
wxDbgHelpDLL::DumpSymbol(wxPSYMBOL_INFO pSym, void *pVariable)
{
    wxString s;
    wxSYMBOL_INFO symDeref = *pSym;
    switch ( DereferenceSymbol(&symDeref, &pVariable) )
    {
        default:
            // Suppress gcc warnings about unhandled enum values, don't assert
            // to avoid problems during fatal crash generation.
            break;

        case SYMBOL_TAG_UDT:
            // show UDT recursively
            s = DumpUDT(&symDeref, pVariable);
            break;

        case SYMBOL_TAG_BASE_TYPE:
            // variable of simple type, show directly
            BasicType bt = GetBasicType(&symDeref);
            if ( bt )
            {
                s = DumpBaseType(bt, pSym->Size, pVariable);
            }
            break;
    }

    return s;
}

// ----------------------------------------------------------------------------
// do the best functions and structures
// ----------------------------------------------------------------------------

struct wxMswEnmLddMdlsHelperStruct
{
public:
    wxMswEnmLddMdlsHelperStruct(wxPENUMLOADED_MODULES_CALLBACK64 ptr, PVOID content)
        : m_pointer_to_callback(ptr), m_user_content(content)
    { }
    wxPENUMLOADED_MODULES_CALLBACK64 m_pointer_to_callback;
    PVOID m_user_content;
};

#ifdef UNICODE

static BOOL CALLBACK wxMswEnmLddMdlsCallback1(
    PCSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
    wxMswEnmLddMdlsHelperStruct& alternate =
        *(wxMswEnmLddMdlsHelperStruct*)(UserContext);

    const wxWCharBuffer buf = wxConvLocal.cMB2WC(ModuleName, wxNO_LEN, NULL);
    return (*alternate.m_pointer_to_callback)
        (buf.data(), ModuleBase, ModuleSize, alternate.m_user_content);
}

static BOOL CALLBACK wxMswEnmLddMdlsCallback2(
    PCSTR ModuleName, DWORD_PTR ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
    wxMswEnmLddMdlsHelperStruct& alternate =
        *(wxMswEnmLddMdlsHelperStruct*)(UserContext);

    const wxWCharBuffer buf = wxConvLocal.cMB2WC(ModuleName, wxNO_LEN, NULL);
    return (*alternate.m_pointer_to_callback)
        (buf.data(), ModuleBase, ModuleSize, alternate.m_user_content);
}

#else

static BOOL CALLBACK wxMswEnmLddMdlsCallback(
    PCSTR ModuleName, DWORD_PTR ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
    wxMswEnmLddMdlsHelperStruct& alternate =
        *(wxMswEnmLddMdlsHelperStruct*)(UserContext);

    return (*alternate.m_pointer_to_callback)
        (ModuleName, ModuleBase, ModuleSize, alternate.m_user_content);
}

#endif // UNICODE

/* static */
BOOL wxDbgHelpDLL::EnumerateLoadedModulesT(
    HANDLE handle, wxPENUMLOADED_MODULES_CALLBACK64 callback, PVOID pvoid)
{
#ifdef UNICODE
    if (EnumerateLoadedModulesW64)
    {
        const BOOL retVal = (*EnumerateLoadedModulesW64)(handle, callback, pvoid);
        if (retVal)
            return retVal;
    }
    if (EnumerateLoadedModules64)
    {
        wxMswEnmLddMdlsHelperStruct p(callback, pvoid);
        const BOOL retVal =
            (*EnumerateLoadedModules64)
                (handle, &wxMswEnmLddMdlsCallback1, (PVOID)(&p));
        if (retVal)
            return retVal;
    }
    if (EnumerateLoadedModules)
    {
        wxMswEnmLddMdlsHelperStruct p(callback, pvoid);
        const BOOL retVal =
            (*EnumerateLoadedModules)
                (handle, &wxMswEnmLddMdlsCallback2, (PVOID)(&p));
        if (retVal)
            return retVal;
    }
    return FALSE;
#else
    if (EnumerateLoadedModules64)
    {
        const BOOL retVal = (*EnumerateLoadedModules64)(handle, callback, pvoid);
        if (retVal)
            return retVal;
    }
    if (EnumerateLoadedModules)
    {
        wxMswEnmLddMdlsHelperStruct p(callback, pvoid);
        const BOOL retVal =
            (*EnumerateLoadedModules)
                (handle, &wxMswEnmLddMdlsCallback, (PVOID)(&p));
        if (retVal)
            return retVal;
    }
    return FALSE;
#endif
}

/* static */
BOOL wxDbgHelpDLL::SymInitializeT(HANDLE hProcess, LPCTSTR UserSearchPath, BOOL fInvadeProcess)
{
#ifdef UNICODE
    if (SymInitializeW)
    {
        const BOOL retVal = (*SymInitializeW)(hProcess, UserSearchPath, fInvadeProcess);
        if (retVal)
            return retVal;
    }
    if (SymInitialize)
    {
        BOOL retVal;
        if (UserSearchPath)
        {
            const wxCharBuffer buf = wxConvLocal.cWC2MB(UserSearchPath, wxNO_LEN, NULL);
            retVal = (*SymInitialize)(hProcess, buf.data(), fInvadeProcess);
        }
        else
        {
            retVal = (*SymInitialize)(hProcess, NULL, fInvadeProcess);
        }
        return retVal;
    }
    return FALSE;
#else
    if (SymInitialize)
    {
        return (*SymInitialize)(hProcess, UserSearchPath, fInvadeProcess);
    }
    return FALSE;
#endif
}

/* static */
BOOL wxDbgHelpDLL::SymFromAddrT(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, wxPSYMBOL_INFO Symbol)
{
#ifdef UNICODE
    if (SymFromAddrW)
    {
        const BOOL retVal = (*SymFromAddrW)(hProcess, Address, Displacement, Symbol);
        if (retVal)
            return retVal;
    }
    if (SymFromAddr)
    {
        BYTE* symbolBuffer = new BYTE[sizeof(SYMBOL_INFO) + Symbol->MaxNameLen*sizeof(CHAR)];
        PSYMBOL_INFO data = (SYMBOL_INFO*)(symbolBuffer);
        wxZeroMemory(*data);
        data->SizeOfStruct = sizeof(SYMBOL_INFO);
        data->MaxNameLen = Symbol->MaxNameLen;
        if (! (*SymFromAddr)(hProcess, Address, Displacement, data))
        {
            delete [] symbolBuffer;
            return FALSE;
        }

        // We can't refer data->NameLen. It seems to be unmodified.
        const wxWCharBuffer buf = wxConvLocal.cMB2WC(data->Name, wxNO_LEN, NULL);

        // FIXME: I know too brute but some header names SYMBOL_INFO::Index
        // and the other one defines it as SYMBOL_INFO::info.
        const ULONG dstSize = Symbol->SizeOfStruct;
        CopyMemory(Symbol, data, sizeof(SYMBOL_INFO)-sizeof(CHAR));
        Symbol->SizeOfStruct = dstSize;
        Symbol->NameLen = buf.length();
        wxStrncpy(Symbol->Name, buf.data(), Symbol->MaxNameLen);
        delete [] symbolBuffer;
        return TRUE;
    }
    return FALSE;
#else
    if (SymFromAddr)
    {
        return (*SymFromAddr)(hProcess, Address, Displacement, Symbol);
    }
    return FALSE;
#endif
}

/* static */
BOOL wxDbgHelpDLL::SymGetLineFromAddrT(HANDLE hProcess, DWORD64 dwAddr, PDWORD pdrDisplacement, wxPIMAGEHLP_LINE Line)
{
#ifdef UNICODE
    if (SymGetLineFromAddrW64)
    {
        const BOOL retVal = (*SymGetLineFromAddrW64)(hProcess, dwAddr, pdrDisplacement, Line);
        if (retVal)
            return retVal;
        // TODO: seems always fail with GetLastError() returns 487 with 32bit binary on 64 bit Windows.
    }
    static WCHAR staticBuf[MAX_PATH];
    if (SymGetLineFromAddr64)
    {
        IMAGEHLP_LINE64 LineAlternate;
        LineAlternate.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if ((*SymGetLineFromAddr64)(hProcess, dwAddr, pdrDisplacement, &LineAlternate))
        {
            const wxWCharBuffer ConvBuf =
                wxConvLocal.cMB2WC(LineAlternate.FileName, wxNO_LEN, NULL);
            wxStrncpy(staticBuf, ConvBuf.data(), MAX_PATH);
            Line->Key = LineAlternate.Key;
            Line->LineNumber = LineAlternate.LineNumber;
            Line->FileName = staticBuf;
            Line->Address = LineAlternate.Address;
            return TRUE;
        }
    }
    if (SymGetLineFromAddr)
    {
        IMAGEHLP_LINE LineAlternate;
        LineAlternate.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        if ((*SymGetLineFromAddr)(hProcess, dwAddr, pdrDisplacement, &LineAlternate))
        {
            const wxWCharBuffer ConvBuf =
                wxConvLocal.cMB2WC(LineAlternate.FileName, wxNO_LEN, NULL);
            wxStrncpy(staticBuf, ConvBuf.data(), MAX_PATH);
            Line->Key = LineAlternate.Key;
            Line->LineNumber = LineAlternate.LineNumber;
            Line->FileName = staticBuf;
            Line->Address = LineAlternate.Address;
            return TRUE;
        }
    }
    return FALSE;
#else
    if (SymGetLineFromAddr64)
    {
        return (*SymGetLineFromAddr64)(hProcess, dwAddr, pdrDisplacement, Line);
    }
    if (SymGetLineFromAddr)
    {
        IMAGEHLP_LINE LineAlternate;
        LineAlternate.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        if ((*SymGetLineFromAddr)(hProcess, dwAddr, pdrDisplacement, &LineAlternate))
        {
            Line->Key = LineAlternate.Key;
            Line->LineNumber = LineAlternate.LineNumber;
            Line->FileName = LineAlternate.FileName;
            Line->Address = LineAlternate.Address;
            return TRUE;
        }
    }
    return FALSE;
#endif
}

struct wxMswSymEnumSymbolsHelperStruct
{
public:
    wxMswSymEnumSymbolsHelperStruct(PSYM_ENUMERATESYMBOLS_CALLBACKW ptr, PVOID content)
        : m_pointer_to_callback(ptr), m_user_content(content)
    { }
    PSYM_ENUMERATESYMBOLS_CALLBACKW m_pointer_to_callback;
    PVOID m_user_content;
};

#ifdef UNICODE

static BOOL CALLBACK wxMswSymEnumSymbolsHelperCallback(
    PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    wxMswSymEnumSymbolsHelperStruct& alternate =
        *(wxMswSymEnumSymbolsHelperStruct*)(UserContext);
    const wxWCharBuffer buf = wxConvLocal.cMB2WC(pSymInfo->Name, pSymInfo->MaxNameLen, NULL);
    BYTE* symbolBuffer = new BYTE[sizeof(SYMBOL_INFOW) + buf.length()*sizeof(WCHAR)];
    SYMBOL_INFOW* data = (SYMBOL_INFOW*)(symbolBuffer);

    // FIXME: I know too brute but some header names SYMBOL_INFO::Index
    // and the other one defines it as SYMBOL_INFO::info.
    CopyMemory(data, pSymInfo, sizeof(SYMBOL_INFO)-sizeof(CHAR));
    data->SizeOfStruct = sizeof(SYMBOL_INFOW);
    wxStrncpy(data->Name, buf.data(), buf.length());
    BOOL retVal = (*alternate.m_pointer_to_callback)(data, SymbolSize, alternate.m_user_content);
    delete [] symbolBuffer;
    return retVal;
}

#endif // UNICODE

/* static */
BOOL wxDbgHelpDLL::SymEnumSymbolsT(HANDLE hProcess, ULONG64 baseOfDll, PCTSTR Mask,
                    wxPSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, const PVOID UserContext)
{
#ifdef UNICODE
    if (SymEnumSymbolsW)
    {
        const BOOL retVal = (*SymEnumSymbolsW)(hProcess, baseOfDll, Mask, EnumSymbolsCallback, UserContext);
        if (retVal)
            return retVal;
    }
    if (SymEnumSymbols)
    {
        wxMswSymEnumSymbolsHelperStruct p(EnumSymbolsCallback, UserContext);
        const wxCharBuffer buf = wxConvLocal.cWC2MB(Mask ? Mask : L"", wxNO_LEN, NULL);
        return (*SymEnumSymbols)(hProcess, baseOfDll, buf.data(),
                                 wxMswSymEnumSymbolsHelperCallback, (PVOID)(&p));
    }
    return FALSE;
#else
    if (SymEnumSymbols)
    {
        return (*SymEnumSymbols)(hProcess, baseOfDll, Mask, EnumSymbolsCallback, UserContext);
    }
    return FALSE;
#endif
}

// ----------------------------------------------------------------------------
// debugging helpers
// ----------------------------------------------------------------------------

// this code is very useful when debugging debughlp.dll-related code but
// probably not worth having compiled in normally, please do not remove it!
#if 0 // ndef NDEBUG

static wxString TagString(wxDbgHelpDLL::SymbolTag tag)
{
    static const wxChar *tags[] =
    {
        wxT("null"),
        wxT("exe"),
        wxT("compiland"),
        wxT("compiland details"),
        wxT("compiland env"),
        wxT("function"),
        wxT("block"),
        wxT("data"),
        wxT("annotation"),
        wxT("label"),
        wxT("public symbol"),
        wxT("udt"),
        wxT("enum"),
        wxT("function type"),
        wxT("pointer type"),
        wxT("array type"),
        wxT("base type"),
        wxT("typedef"),
        wxT("base class"),
        wxT("friend"),
        wxT("function arg type"),
        wxT("func debug start"),
        wxT("func debug end"),
        wxT("using namespace"),
        wxT("vtable shape"),
        wxT("vtable"),
        wxT("custom"),
        wxT("thunk"),
        wxT("custom type"),
        wxT("managed type"),
        wxT("dimension"),
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(tags) == wxDbgHelpDLL::SYMBOL_TAG_MAX,
                                SymbolTagStringMismatch );

    wxString s;
    if ( tag < WXSIZEOF(tags) )
        s = tags[tag];
    else
        s.Printf(wxT("unrecognized tag (%d)"), tag);

    return s;
}

static wxString KindString(wxDbgHelpDLL::DataKind kind)
{
    static const wxChar *kinds[] =
    {
         wxT("unknown"),
         wxT("local"),
         wxT("static local"),
         wxT("param"),
         wxT("object ptr"),
         wxT("file static"),
         wxT("global"),
         wxT("member"),
         wxT("static member"),
         wxT("constant"),
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(kinds) == wxDbgHelpDLL::DATA_MAX,
                                DataKindStringMismatch );

    wxString s;
    if ( kind < WXSIZEOF(kinds) )
        s = kinds[kind];
    else
        s.Printf(wxT("unrecognized kind (%d)"), kind);

    return s;
}

static wxString UdtKindString(wxDbgHelpDLL::UdtKind kind)
{
    static const wxChar *kinds[] =
    {
         wxT("struct"),
         wxT("class"),
         wxT("union"),
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(kinds) == wxDbgHelpDLL::UDT_MAX,
                                UDTKindStringMismatch );

    wxString s;
    if ( kind < WXSIZEOF(kinds) )
        s = kinds[kind];
    else
        s.Printf(wxT("unrecognized UDT (%d)"), kind);

    return s;
}

static wxString TypeString(wxDbgHelpDLL::BasicType bt)
{
    static const wxChar *types[] =
    {
        wxT("no type"),
        wxT("void"),
        wxT("char"),
        wxT("wchar"),
        wxT(""),
        wxT(""),
        wxT("int"),
        wxT("uint"),
        wxT("float"),
        wxT("bcd"),
        wxT("bool"),
        wxT(""),
        wxT(""),
        wxT("long"),
        wxT("ulong"),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT(""),
        wxT("CURRENCY"),
        wxT("DATE"),
        wxT("VARIANT"),
        wxT("complex"),
        wxT("bit"),
        wxT("BSTR"),
        wxT("HRESULT"),
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(types) == wxDbgHelpDLL::BASICTYPE_MAX,
                                BasicTypeStringMismatch );

    wxString s;
    if ( bt < WXSIZEOF(types) )
        s = types[bt];

    if ( s.empty() )
        s.Printf(wxT("unrecognized type (%d)"), bt);

    return s;
}

// this function is meant to be called from under debugger to see the
// proprieties of the given type id
extern "C" void DumpTI(ULONG ti)
{
    SYMBOL_INFO sym = { sizeof(SYMBOL_INFO) };
    sym.ModBase = 0x400000; // it's a constant under Win32
    sym.TypeIndex = ti;

    wxDbgHelpDLL::SymbolTag tag = wxDbgHelpDLL::SYMBOL_TAG_NULL;
    DoGetTypeInfo(&sym, TI_GET_SYMTAG, &tag);
    DoGetTypeInfo(&sym, TI_GET_TYPEID, &ti);

    OutputDebugString(wxString::Format(wxT("Type 0x%x: "), sym.TypeIndex));
    wxString name = wxDbgHelpDLL::GetSymbolName(&sym);
    if ( !name.empty() )
    {
        OutputDebugString(wxString::Format(wxT("name=\"%s\", "), name.c_str()));
    }

    DWORD nested;
    if ( !DoGetTypeInfo(&sym, TI_GET_NESTED, &nested) )
    {
        nested = FALSE;
    }

    OutputDebugString(wxString::Format(wxT("tag=%s%s"),
                      nested ? wxT("nested ") : wxEmptyString,
                      TagString(tag).c_str()));
    if ( tag == wxDbgHelpDLL::SYMBOL_TAG_UDT )
    {
        wxDbgHelpDLL::UdtKind udtKind;
        if ( DoGetTypeInfo(&sym, TI_GET_UDTKIND, &udtKind) )
        {
            OutputDebugString(wxT(" (") + UdtKindString(udtKind) + wxT(')'));
        }
    }

    wxDbgHelpDLL::DataKind kind = wxDbgHelpDLL::DATA_UNKNOWN;
    if ( DoGetTypeInfo(&sym, TI_GET_DATAKIND, &kind) )
    {
        OutputDebugString(wxString::Format(
            wxT(", kind=%s"), KindString(kind).c_str()));
        if ( kind == wxDbgHelpDLL::DATA_MEMBER )
        {
            DWORD ofs = 0;
            if ( DoGetTypeInfo(&sym, TI_GET_OFFSET, &ofs) )
            {
                OutputDebugString(wxString::Format(wxT(" (ofs=0x%x)"), ofs));
            }
        }
    }

    wxDbgHelpDLL::BasicType bt = GetBasicType(&sym);
    if ( bt )
    {
        OutputDebugString(wxString::Format(wxT(", type=%s"),
                                TypeString(bt).c_str()));
    }

    if ( ti != sym.TypeIndex )
    {
        OutputDebugString(wxString::Format(wxT(", next ti=0x%x"), ti));
    }

    OutputDebugString(wxT("\r\n"));
}

#endif // NDEBUG

#endif // wxUSE_DBGHELP
