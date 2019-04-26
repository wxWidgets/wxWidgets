/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/mimecmn.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
//  Chris Elliott (biol75@york.ac.uk) 5 Dec 00: write support for Win32
// Created:     23.09.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE

#include "wx/mimetype.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/module.h"
    #include "wx/crt.h"
#endif //WX_PRECOMP

#include "wx/file.h"
#include "wx/iconloc.h"
#include "wx/confbase.h"

// other standard headers
#include <ctype.h>

// implementation classes:
#if defined(__WINDOWS__)
    #include "wx/msw/mimetype.h"
#elif ( defined(__DARWIN__) )
    #include "wx/osx/mimetype.h"
#else // Unix
    #include "wx/unix/mimetype.h"
#endif

// ============================================================================
// common classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxMimeTypeCommands
// ----------------------------------------------------------------------------

void
wxMimeTypeCommands::AddOrReplaceVerb(const wxString& verb, const wxString& cmd)
{
    int n = m_verbs.Index(verb, false /* ignore case */);
    if ( n == wxNOT_FOUND )
    {
        m_verbs.Add(verb);
        m_commands.Add(cmd);
    }
    else
    {
        m_commands[n] = cmd;
    }
}

wxString
wxMimeTypeCommands::GetCommandForVerb(const wxString& verb, size_t *idx) const
{
    wxString s;

    int n = m_verbs.Index(verb);
    if ( n != wxNOT_FOUND )
    {
        s = m_commands[(size_t)n];
        if ( idx )
            *idx = n;
    }
    else if ( idx )
    {
        // different from any valid index
        *idx = (size_t)-1;
    }

    return s;
}

wxString wxMimeTypeCommands::GetVerbCmd(size_t n) const
{
    return m_verbs[n] + wxT('=') + m_commands[n];
}

// ----------------------------------------------------------------------------
// wxFileTypeInfo
// ----------------------------------------------------------------------------

void wxFileTypeInfo::DoVarArgInit(const wxString& mimeType,
                                  const wxString& openCmd,
                                  const wxString& printCmd,
                                  const wxString& desc,
                                  va_list argptr)
{
    m_mimeType = mimeType;
    m_openCmd = openCmd;
    m_printCmd = printCmd;
    m_desc = desc;

    for ( ;; )
    {
        // icc gives this warning in its own va_arg() macro, argh
#ifdef __INTELC__
    #pragma warning(push)
    #pragma warning(disable: 1684)
#endif

        wxArgNormalizedString ext(WX_VA_ARG_STRING(argptr));

#ifdef __INTELC__
    #pragma warning(pop)
#endif
        if ( !ext )
        {
            // NULL terminates the list
            break;
        }

        m_exts.Add(ext.GetString());
    }
}

void wxFileTypeInfo::VarArgInit(const wxString *mimeType,
                                const wxString *openCmd,
                                const wxString *printCmd,
                                const wxString *desc,
                                ...)
{
    va_list argptr;
    va_start(argptr, desc);

    DoVarArgInit(*mimeType, *openCmd, *printCmd, *desc, argptr);

    va_end(argptr);
}


wxFileTypeInfo::wxFileTypeInfo(const wxArrayString& sArray)
    : m_mimeType(sArray[0u])
    , m_openCmd( sArray[1u])
    , m_printCmd(sArray[2u])
    , m_desc(    sArray[3u])
{
    size_t count = sArray.GetCount();
    for ( size_t i = 4; i < count; i++ )
    {
        m_exts.Add(sArray[i]);
    }
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayFileTypeInfo)

// ============================================================================
// implementation of the wrapper classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileType
// ----------------------------------------------------------------------------

/* static */
wxString wxFileType::ExpandCommand(const wxString& command,
                                   const wxFileType::MessageParameters& params)
{
    bool hasFilename = false;

    // We consider that only the file names with spaces in them need to be
    // handled specially. This is not perfect, but this can be done easily
    // under all platforms while handling the file names with quotes in them,
    // for example, needs to be done differently.
    const bool needToQuoteFilename = params.GetFileName().find_first_of(" \t")
                                        != wxString::npos;

    wxString str;
    for ( const wxChar *pc = command.c_str(); *pc != wxT('\0'); pc++ ) {
        if ( *pc == wxT('%') ) {
            switch ( *++pc ) {
                case wxT('s'):
                    // don't quote the file name if it's already quoted: notice
                    // that we check for a quote following it and not preceding
                    // it as at least under Windows we can have commands
                    // containing "file://%s" (with quotes) in them so the
                    // argument may be quoted even if there is no quote
                    // directly before "%s" itself
                    if ( needToQuoteFilename && pc[1] != '"' )
                        str << wxT('"') << params.GetFileName() << wxT('"');
                    else
                        str << params.GetFileName();
                    hasFilename = true;
                    break;

                case wxT('t'):
                    // '%t' expands into MIME type (quote it too just to be
                    // consistent)
                    str << wxT('\'') << params.GetMimeType() << wxT('\'');
                    break;

                case wxT('{'):
                    {
                        const wxChar *pEnd = wxStrchr(pc, wxT('}'));
                        if ( pEnd == NULL ) {
                            wxString mimetype;
                            wxLogWarning(_("Unmatched '{' in an entry for mime type %s."),
                                         params.GetMimeType().c_str());
                            str << wxT("%{");
                        }
                        else {
                            wxString param(pc + 1, pEnd - pc - 1);
                            str << wxT('\'') << params.GetParamValue(param) << wxT('\'');
                            pc = pEnd;
                        }
                    }
                    break;

                case wxT('n'):
                case wxT('F'):
                    // TODO %n is the number of parts, %F is an array containing
                    //      the names of temp files these parts were written to
                    //      and their mime types.
                    break;

                default:
                    wxLogDebug(wxT("Unknown field %%%c in command '%s'."),
                               *pc, command.c_str());
                    str << *pc;
            }
        }
        else {
            str << *pc;
        }
    }

    // metamail(1) man page states that if the mailcap entry doesn't have '%s'
    // the program will accept the data on stdin so normally we should append
    // "< %s" to the end of the command in such case, but not all commands
    // behave like this, in particular a common test is 'test -n "$DISPLAY"'
    // and appending "< %s" to this command makes the test fail... I don't
    // know of the correct solution, try to guess what we have to do.

    // test now carried out on reading file so test should never get here
    if ( !hasFilename && !str.empty()
#ifdef __UNIX__
                      && !str.StartsWith(wxT("test "))
#endif // Unix
       )
    {
        str << wxT(" < ");
        if ( needToQuoteFilename )
            str << '"';
        str << params.GetFileName();
        if ( needToQuoteFilename )
            str << '"';
    }

    return str;
}

wxFileType::wxFileType(const wxFileTypeInfo& info)
{
    m_info = &info;
    m_impl = NULL;
}

wxFileType::wxFileType()
{
    m_info = NULL;
    m_impl = new wxFileTypeImpl;
}

wxFileType::~wxFileType()
{
    if ( m_impl )
        delete m_impl;
}

bool wxFileType::GetExtensions(wxArrayString& extensions)
{
    if ( m_info )
    {
        extensions = m_info->GetExtensions();
        return true;
    }

    return m_impl->GetExtensions(extensions);
}

bool wxFileType::GetMimeType(wxString *mimeType) const
{
    wxCHECK_MSG( mimeType, false, wxT("invalid parameter in GetMimeType") );

    if ( m_info )
    {
        *mimeType = m_info->GetMimeType();

        return true;
    }

    return m_impl->GetMimeType(mimeType);
}

bool wxFileType::GetMimeTypes(wxArrayString& mimeTypes) const
{
    if ( m_info )
    {
        mimeTypes.Clear();
        mimeTypes.Add(m_info->GetMimeType());

        return true;
    }

    return m_impl->GetMimeTypes(mimeTypes);
}

bool wxFileType::GetIcon(wxIconLocation *iconLoc) const
{
    if ( m_info )
    {
        if ( iconLoc )
        {
            iconLoc->SetFileName(m_info->GetIconFile());
#ifdef __WINDOWS__
            iconLoc->SetIndex(m_info->GetIconIndex());
#endif // __WINDOWS__
        }

        return true;
    }

    return m_impl->GetIcon(iconLoc);
}

bool
wxFileType::GetIcon(wxIconLocation *iconloc,
                    const MessageParameters& params) const
{
    if ( !GetIcon(iconloc) )
    {
        return false;
    }

    // we may have "%s" in the icon location string, at least under Windows, so
    // expand this
    if ( iconloc )
    {
        iconloc->SetFileName(ExpandCommand(iconloc->GetFileName(), params));
    }

    return true;
}

bool wxFileType::GetDescription(wxString *desc) const
{
    wxCHECK_MSG( desc, false, wxT("invalid parameter in GetDescription") );

    if ( m_info )
    {
        *desc = m_info->GetDescription();

        return true;
    }

    return m_impl->GetDescription(desc);
}

bool
wxFileType::GetOpenCommand(wxString *openCmd,
                           const wxFileType::MessageParameters& params) const
{
    wxCHECK_MSG( openCmd, false, wxT("invalid parameter in GetOpenCommand") );

    if ( m_info )
    {
        *openCmd = ExpandCommand(m_info->GetOpenCommand(), params);

        return true;
    }

    return m_impl->GetOpenCommand(openCmd, params);
}

wxString wxFileType::GetOpenCommand(const wxString& filename) const
{
    wxString cmd;
    if ( !GetOpenCommand(&cmd, filename) )
    {
        // return empty string to indicate an error
        cmd.clear();
    }

    return cmd;
}

bool
wxFileType::GetPrintCommand(wxString *printCmd,
                            const wxFileType::MessageParameters& params) const
{
    wxCHECK_MSG( printCmd, false, wxT("invalid parameter in GetPrintCommand") );

    if ( m_info )
    {
        *printCmd = ExpandCommand(m_info->GetPrintCommand(), params);

        return true;
    }

    return m_impl->GetPrintCommand(printCmd, params);
}

wxString
wxFileType::GetExpandedCommand(const wxString& verb,
                               const wxFileType::MessageParameters& params) const
{
    return m_impl->GetExpandedCommand(verb, params);
}


size_t wxFileType::GetAllCommands(wxArrayString *verbs,
                                  wxArrayString *commands,
                                  const wxFileType::MessageParameters& params) const
{
    if ( verbs )
        verbs->Clear();
    if ( commands )
        commands->Clear();

#if defined (__WINDOWS__)  || defined(__UNIX__)
    return m_impl->GetAllCommands(verbs, commands, params);
#else // !__WINDOWS__ || __UNIX__
    // we don't know how to retrieve all commands, so just try the 2 we know
    // about
    size_t count = 0;
    wxString cmd;
    if ( GetOpenCommand(&cmd, params) )
    {
        if ( verbs )
            verbs->Add(wxT("Open"));
        if ( commands )
            commands->Add(cmd);
        count++;
    }

    if ( GetPrintCommand(&cmd, params) )
    {
        if ( verbs )
            verbs->Add(wxT("Print"));
        if ( commands )
            commands->Add(cmd);

        count++;
    }

    return count;
#endif // __WINDOWS__/| __UNIX__
}

bool wxFileType::Unassociate()
{
#if defined(__WINDOWS__)
    return m_impl->Unassociate();
#elif defined(__UNIX__)
    return m_impl->Unassociate(this);
#else
    wxFAIL_MSG( wxT("not implemented") ); // TODO
    return false;
#endif
}

bool wxFileType::SetCommand(const wxString& cmd,
                            const wxString& verb,
                            bool overwriteprompt)
{
#if defined (__WINDOWS__)  || defined(__UNIX__)
    return m_impl->SetCommand(cmd, verb, overwriteprompt);
#else
    wxUnusedVar(cmd);
    wxUnusedVar(verb);
    wxUnusedVar(overwriteprompt);
    wxFAIL_MSG(wxT("not implemented"));
    return false;
#endif
}

bool wxFileType::SetDefaultIcon(const wxString& cmd, int index)
{
    wxString sTmp = cmd;
#ifdef __WINDOWS__
    // VZ: should we do this?
    // chris elliott : only makes sense in MS windows
    if ( sTmp.empty() )
        GetOpenCommand(&sTmp, wxFileType::MessageParameters(wxEmptyString, wxEmptyString));
#endif
    wxCHECK_MSG( !sTmp.empty(), false, wxT("need the icon file") );

#if defined (__WINDOWS__) || defined(__UNIX__)
    return m_impl->SetDefaultIcon (cmd, index);
#else
    wxUnusedVar(index);
    wxFAIL_MSG(wxT("not implemented"));
    return false;
#endif
}

// ----------------------------------------------------------------------------
// wxMimeTypesManagerFactory
// ----------------------------------------------------------------------------

wxMimeTypesManagerFactory *wxMimeTypesManagerFactory::m_factory = NULL;

/* static */
void wxMimeTypesManagerFactory::Set(wxMimeTypesManagerFactory *factory)
{
    delete m_factory;

    m_factory = factory;
}

/* static */
wxMimeTypesManagerFactory *wxMimeTypesManagerFactory::Get()
{
    if ( !m_factory )
        m_factory = new wxMimeTypesManagerFactory;

    return m_factory;
}

wxMimeTypesManagerImpl *wxMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxMimeTypesManagerImpl;
}

// ----------------------------------------------------------------------------
// wxMimeTypesManager
// ----------------------------------------------------------------------------

void wxMimeTypesManager::EnsureImpl()
{
    if ( !m_impl )
        m_impl = wxMimeTypesManagerFactory::Get()->CreateMimeTypesManagerImpl();
}

bool wxMimeTypesManager::IsOfType(const wxString& mimeType,
                                  const wxString& wildcard)
{
    wxASSERT_MSG( mimeType.Find(wxT('*')) == wxNOT_FOUND,
                  wxT("first MIME type can't contain wildcards") );

    // all comparaisons are case insensitive (2nd arg of IsSameAs() is false)
    if ( wildcard.BeforeFirst(wxT('/')).
            IsSameAs(mimeType.BeforeFirst(wxT('/')), false) )
    {
        wxString strSubtype = wildcard.AfterFirst(wxT('/'));

        if ( strSubtype == wxT("*") ||
             strSubtype.IsSameAs(mimeType.AfterFirst(wxT('/')), false) )
        {
            // matches (either exactly or it's a wildcard)
            return true;
        }
    }

    return false;
}

wxMimeTypesManager::wxMimeTypesManager()
{
    m_impl = NULL;
}

wxMimeTypesManager::~wxMimeTypesManager()
{
    if ( m_impl )
        delete m_impl;
}

bool wxMimeTypesManager::Unassociate(wxFileType *ft)
{
    EnsureImpl();

#if defined(__UNIX__) && !defined(__CYGWIN__) && !defined(__WINE__)
    return m_impl->Unassociate(ft);
#else
    return ft->Unassociate();
#endif
}


wxFileType *
wxMimeTypesManager::Associate(const wxFileTypeInfo& ftInfo)
{
    EnsureImpl();

#if defined(__WINDOWS__) || defined(__UNIX__)
    return m_impl->Associate(ftInfo);
#else // other platforms
    wxUnusedVar(ftInfo);
    wxFAIL_MSG( wxT("not implemented") ); // TODO
    return NULL;
#endif // platforms
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromExtension(const wxString& ext)
{
    EnsureImpl();

    wxString::const_iterator i = ext.begin();
    const wxString::const_iterator end = ext.end();
    wxString extWithoutDot;
    if ( i != end && *i == '.' )
        extWithoutDot.assign(++i, ext.end());
    else
        extWithoutDot = ext;

    wxCHECK_MSG( !ext.empty(), NULL, wxT("extension can't be empty") );

    wxFileType *ft = m_impl->GetFileTypeFromExtension(extWithoutDot);

    if ( !ft ) {
        // check the fallbacks
        //
        // TODO linear search is potentially slow, perhaps we should use a
        //       sorted array?
        size_t count = m_fallbacks.GetCount();
        for ( size_t n = 0; n < count; n++ ) {
            if ( m_fallbacks[n].GetExtensions().Index(ext) != wxNOT_FOUND ) {
                ft = new wxFileType(m_fallbacks[n]);

                break;
            }
        }
    }

    return ft;
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromMimeType(const wxString& mimeType)
{
    EnsureImpl();
    wxFileType *ft = m_impl->GetFileTypeFromMimeType(mimeType);

    if ( !ft ) {
        // check the fallbacks
        //
        // TODO linear search is potentially slow, perhaps we should use a
        //      sorted array?
        size_t count = m_fallbacks.GetCount();
        for ( size_t n = 0; n < count; n++ ) {
            if ( wxMimeTypesManager::IsOfType(mimeType,
                                              m_fallbacks[n].GetMimeType()) ) {
                ft = new wxFileType(m_fallbacks[n]);

                break;
            }
        }
    }

    return ft;
}

void wxMimeTypesManager::AddFallbacks(const wxFileTypeInfo *filetypes)
{
    EnsureImpl();
    for ( const wxFileTypeInfo *ft = filetypes; ft && ft->IsValid(); ft++ ) {
        AddFallback(*ft);
    }
}

size_t wxMimeTypesManager::EnumAllFileTypes(wxArrayString& mimetypes)
{
    EnsureImpl();
    size_t countAll = m_impl->EnumAllFileTypes(mimetypes);

    // add the fallback filetypes
    size_t count = m_fallbacks.GetCount();
    for ( size_t n = 0; n < count; n++ ) {
        if ( mimetypes.Index(m_fallbacks[n].GetMimeType()) == wxNOT_FOUND ) {
            mimetypes.Add(m_fallbacks[n].GetMimeType());
            countAll++;
        }
    }

    return countAll;
}

void wxMimeTypesManager::Initialize(int mcapStyle,
                                    const wxString& sExtraDir)
{
#if defined(__UNIX__) && !defined(__CYGWIN__) && !defined(__WINE__)
    EnsureImpl();

    m_impl->Initialize(mcapStyle, sExtraDir);
#else
    (void)mcapStyle;
    (void)sExtraDir;
#endif // Unix
}

// and this function clears all the data from the manager
void wxMimeTypesManager::ClearData()
{
#if defined(__UNIX__) && !defined(__CYGWIN__) && !defined(__WINE__)
    EnsureImpl();

    m_impl->ClearData();
#endif // Unix
}

// ----------------------------------------------------------------------------
// global data and wxMimeTypeCmnModule
// ----------------------------------------------------------------------------

// private object
static wxMimeTypesManager gs_mimeTypesManager;

// and public pointer
wxMimeTypesManager *wxTheMimeTypesManager = &gs_mimeTypesManager;

class wxMimeTypeCmnModule: public wxModule
{
public:
    wxMimeTypeCmnModule() : wxModule() { }

    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE
    {
        wxMimeTypesManagerFactory::Set(NULL);

        if ( gs_mimeTypesManager.m_impl != NULL )
        {
            wxDELETE(gs_mimeTypesManager.m_impl);
            gs_mimeTypesManager.m_fallbacks.Clear();
        }
    }

    wxDECLARE_DYNAMIC_CLASS(wxMimeTypeCmnModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxMimeTypeCmnModule, wxModule);

#endif // wxUSE_MIMETYPE
