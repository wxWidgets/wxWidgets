/////////////////////////////////////////////////////////////////////////////
// Name:        common/mimecmn.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
//  Chris Elliott (biol75@york.ac.uk) 5 Dec 00: write support for Win32
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef    __GNUG__
    #pragma implementation "mimetypebase.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/module.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#ifndef WX_PRECOMP
  #include "wx/string.h"
  #if wxUSE_GUI
    #include "wx/icon.h"
  #endif
#endif //WX_PRECOMP

#include "wx/log.h"
#include "wx/file.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/confbase.h"

#include "wx/mimetype.h"

// other standard headers
#include <ctype.h>

// implementation classes:
#if defined(__WXMSW__)
    #include "wx/msw/mimetype.h"
#elif defined(__WXMAC__)
    #include "wx/mac/mimetype.h"
#elif defined(__WXPM__)
    #include "wx/os2/mimetype.h"
#else // Unix
    #include "wx/unix/mimetype.h"
#endif

// ============================================================================
// common classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileTypeInfo
// ----------------------------------------------------------------------------

wxFileTypeInfo::wxFileTypeInfo(const char *mimeType,
                               const char *openCmd,
                               const char *printCmd,
                               const char *desc,
                               ...)
              : m_mimeType(mimeType),
                m_openCmd(openCmd),
                m_printCmd(printCmd),
                m_desc(desc)
{
    va_list argptr;
    va_start(argptr, desc);

    for ( ;; )
    {
        const char *ext = va_arg(argptr, const char *);
        if ( !ext )
        {
            // NULL terminates the list
            break;
        }

        m_exts.Add(ext);
    }

    va_end(argptr);
}


wxFileTypeInfo::wxFileTypeInfo(const wxArrayString& sArray)
{
    m_mimeType = sArray [0u];
    m_openCmd  = sArray [1u];
    m_printCmd = sArray [2u];
    m_desc     = sArray [3u];

    size_t count = sArray.GetCount();
    for ( size_t i = 4; i < count; i++ )
    {
        m_exts.Add(sArray[i]);
    }
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayFileTypeInfo);

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
    bool hasFilename = FALSE;

    wxString str;
    for ( const wxChar *pc = command.c_str(); *pc != wxT('\0'); pc++ ) {
        if ( *pc == wxT('%') ) {
            switch ( *++pc ) {
                case wxT('s'):
                    // '%s' expands into file name (quoted because it might
                    // contain spaces) - except if there are already quotes
                    // there because otherwise some programs may get confused
                    // by double double quotes
#if 0
                    if ( *(pc - 2) == wxT('"') )
                        str << params.GetFileName();
                    else
                        str << wxT('"') << params.GetFileName() << wxT('"');
#endif
                    str << params.GetFileName();
                    hasFilename = TRUE;
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
    if ( !hasFilename && !str.IsEmpty()
#ifdef __UNIX__
                      && !str.StartsWith(_T("test "))
#endif // Unix
       ) {
        str << wxT(" < '") << params.GetFileName() << wxT('\'');
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
        return TRUE;
    }

    return m_impl->GetExtensions(extensions);
}

bool wxFileType::GetMimeType(wxString *mimeType) const
{
    wxCHECK_MSG( mimeType, FALSE, _T("invalid parameter in GetMimeType") );

    if ( m_info )
    {
        *mimeType = m_info->GetMimeType();

        return TRUE;
    }

    return m_impl->GetMimeType(mimeType);
}

bool wxFileType::GetMimeTypes(wxArrayString& mimeTypes) const
{
    if ( m_info )
    {
        mimeTypes.Clear();
        mimeTypes.Add(m_info->GetMimeType());

        return TRUE;
    }

    return m_impl->GetMimeTypes(mimeTypes);
}

bool wxFileType::GetIcon(wxIcon *icon,
                         wxString *iconFile,
                         int *iconIndex) const
{
    if ( m_info )
    {
        if ( iconFile )
            *iconFile = m_info->GetIconFile();
        if ( iconIndex )
            *iconIndex = m_info->GetIconIndex();

#if wxUSE_GUI
        if ( icon && !m_info->GetIconFile().empty() )
        {
            // FIXME: what about the index?
            icon->LoadFile(m_info->GetIconFile());
        }
#endif // wxUSE_GUI

        return TRUE;
    }

#if defined(__WXMSW__) || defined(__UNIX__)
    return m_impl->GetIcon(icon, iconFile, iconIndex);
#else
    return m_impl->GetIcon(icon);
#endif
}

bool wxFileType::GetDescription(wxString *desc) const
{
    wxCHECK_MSG( desc, FALSE, _T("invalid parameter in GetDescription") );

    if ( m_info )
    {
        *desc = m_info->GetDescription();

        return TRUE;
    }

    return m_impl->GetDescription(desc);
}

bool
wxFileType::GetOpenCommand(wxString *openCmd,
                           const wxFileType::MessageParameters& params) const
{
    wxCHECK_MSG( openCmd, FALSE, _T("invalid parameter in GetOpenCommand") );

    if ( m_info )
    {
        *openCmd = ExpandCommand(m_info->GetOpenCommand(), params);

        return TRUE;
    }

    return m_impl->GetOpenCommand(openCmd, params);
}

bool
wxFileType::GetPrintCommand(wxString *printCmd,
                            const wxFileType::MessageParameters& params) const
{
    wxCHECK_MSG( printCmd, FALSE, _T("invalid parameter in GetPrintCommand") );

    if ( m_info )
    {
        *printCmd = ExpandCommand(m_info->GetPrintCommand(), params);

        return TRUE;
    }

    return m_impl->GetPrintCommand(printCmd, params);
}


size_t wxFileType::GetAllCommands(wxArrayString *verbs,
                                  wxArrayString *commands,
                                  const wxFileType::MessageParameters& params) const
{
    if ( verbs )
        verbs->Clear();
    if ( commands )
        commands->Clear();

#if defined (__WXMSW__)  || (__UNIX__)
    return m_impl->GetAllCommands(verbs, commands, params);
#else // !__WXMSW__ || Unix
    // we don't know how to retrieve all commands, so just try the 2 we know
    // about
    size_t count = 0;
    wxString cmd;
    if ( GetOpenCommand(&cmd, params) )
    {
        if ( verbs )
            verbs->Add(_T("Open"));
        if ( commands )
            commands->Add(cmd);
        count++;
    }

    if ( GetPrintCommand(&cmd, params) )
    {
        if ( verbs )
            verbs->Add(_T("Print"));
        if ( commands )
            commands->Add(cmd);

        count++;
    }

    return count;
#endif // __WXMSW__/| __UNIX__
}

bool wxFileType::Unassociate()
{
#if defined(__WXMSW__)
    return m_impl->Unassociate();
#endif

#if defined(__UNIX__)
    return m_impl->Unassociate(this);
#endif

    wxFAIL_MSG( _T("not implemented") ); // TODO
    return FALSE;

}

bool wxFileType::SetCommand(const wxString& cmd, const wxString& verb,
bool overwriteprompt)
{
#if defined (__WXMSW__)  || (__UNIX__)
    return m_impl->SetCommand(cmd, verb, overwriteprompt);
#else
    wxFAIL_MSG(_T("not implemented"));

    return FALSE;
#endif
}

bool wxFileType::SetDefaultIcon(const wxString& cmd, int index)
{
    wxString sTmp = cmd;
#ifdef __WXMSW__
    // VZ: should we do this?
    // chris elliott : only makes sense in MS windows
    if ( sTmp.empty() )
        GetOpenCommand(&sTmp, wxFileType::MessageParameters("", ""));
#endif
    wxCHECK_MSG( !sTmp.empty(), FALSE, _T("need the icon file") );

#if defined (__WXMSW__) || (__UNIX__)
    return m_impl->SetDefaultIcon (cmd, index);
#else
    wxFAIL_MSG(_T("not implemented"));

    return FALSE;
#endif
}


// ----------------------------------------------------------------------------
// wxMimeTypesManager
// ----------------------------------------------------------------------------

void wxMimeTypesManager::EnsureImpl()
{
    if ( !m_impl )
        m_impl = new wxMimeTypesManagerImpl;
}

bool wxMimeTypesManager::IsOfType(const wxString& mimeType,
                                  const wxString& wildcard)
{
    wxASSERT_MSG( mimeType.Find(wxT('*')) == wxNOT_FOUND,
                  wxT("first MIME type can't contain wildcards") );

    // all comparaisons are case insensitive (2nd arg of IsSameAs() is FALSE)
    if ( wildcard.BeforeFirst(wxT('/')).
            IsSameAs(mimeType.BeforeFirst(wxT('/')), FALSE) )
    {
        wxString strSubtype = wildcard.AfterFirst(wxT('/'));

        if ( strSubtype == wxT("*") ||
             strSubtype.IsSameAs(mimeType.AfterFirst(wxT('/')), FALSE) )
        {
            // matches (either exactly or it's a wildcard)
            return TRUE;
        }
    }

    return FALSE;
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
#if defined(__UNIX__)
    return m_impl->Unassociate(ft);
#else
    return ft->Unassociate();
#endif
}


wxFileType *
wxMimeTypesManager::Associate(const wxFileTypeInfo& ftInfo)
{
    EnsureImpl();

#if defined(__WXMSW__) || defined(__UNIX__)
    return m_impl->Associate(ftInfo);
#else // other platforms
    wxFAIL_MSG( _T("not implemented") ); // TODO
    return NULL;
#endif // platforms
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromExtension(const wxString& ext)
{
    EnsureImpl();
    wxFileType *ft = m_impl->GetFileTypeFromExtension(ext);

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

    if ( ft ) {
        // check the fallbacks
        //
        // TODO linear search is potentially slow, perhaps we should use a sorted
        //      array?
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

bool wxMimeTypesManager::ReadMailcap(const wxString& filename, bool fallback)
{
    EnsureImpl();
    return m_impl->ReadMailcap(filename, fallback);
}

bool wxMimeTypesManager::ReadMimeTypes(const wxString& filename)
{
    EnsureImpl();
    return m_impl->ReadMimeTypes(filename);
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
#ifdef __UNIX__
    EnsureImpl();

    m_impl->Initialize(mcapStyle, sExtraDir);
#endif // Unix
}

// and this function clears all the data from the manager
void wxMimeTypesManager::ClearData()
{
#ifdef __UNIX__
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
    virtual bool OnInit() { return TRUE; }
    virtual void OnExit()
    {
        // this avoids false memory leak allerts:
        if ( gs_mimeTypesManager.m_impl != NULL )
        {
            delete gs_mimeTypesManager.m_impl;
            gs_mimeTypesManager.m_impl = NULL;
        }
    }

    DECLARE_DYNAMIC_CLASS(wxMimeTypeCmnModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxMimeTypeCmnModule, wxModule)
