/////////////////////////////////////////////////////////////////////////////
// Name:        common/mimecmn.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifdef    __GNUG__
#pragma implementation "mimetypebase.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE

#ifndef WX_PRECOMP
    #include "wx/module.h"
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

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
    #include "windows.h"
#elif defined(__UNIX__)  || defined(__WXPM__)
    #include "wx/ffile.h"
    #include "wx/textfile.h"
    #include "wx/dir.h"
    #include "wx/utils.h"
    #include "wx/tokenzr.h"
#endif // OS

#include "wx/mimetype.h"

// other standard headers
#include <ctype.h>

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;


// implementation classes:

#if defined(__WXMSW__)
#include "wx/msw/mimetype.h"
#elif defined (__WXMAC__)
#include "wx/mac/mimetype.h"
#elif defined (__WXPM__)
#include "wx/os2/mimetype.h"
#else
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

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayFileTypeInfo);


// ============================================================================
// implementation of the wrapper classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileType
// ----------------------------------------------------------------------------

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
    if ( !hasFilename && !str.IsEmpty()
#ifdef __UNIX__
                      && !str.StartsWith(_T("test "))
#endif // Unix
       ) {
        str << wxT(" < '") << params.GetFileName() << wxT('\'');
    }

    return str;
}

wxFileType::wxFileType()
{
    m_impl = new wxFileTypeImpl;
}

wxFileType::~wxFileType()
{
    delete m_impl;
}

bool wxFileType::GetExtensions(wxArrayString& extensions)
{
    return m_impl->GetExtensions(extensions);
}

bool wxFileType::GetMimeType(wxString *mimeType) const
{
    return m_impl->GetMimeType(mimeType);
}

bool wxFileType::GetMimeTypes(wxArrayString& mimeTypes) const
{
    return m_impl->GetMimeTypes(mimeTypes);
}

bool wxFileType::GetIcon(wxIcon *icon) const
{
    return m_impl->GetIcon(icon);
}

bool wxFileType::GetDescription(wxString *desc) const
{
    return m_impl->GetDescription(desc);
}

bool
wxFileType::GetOpenCommand(wxString *openCmd,
                           const wxFileType::MessageParameters& params) const
{
    return m_impl->GetOpenCommand(openCmd, params);
}

bool
wxFileType::GetPrintCommand(wxString *printCmd,
                            const wxFileType::MessageParameters& params) const
{
    return m_impl->GetPrintCommand(printCmd, params);
}

// ----------------------------------------------------------------------------
// wxMimeTypesManager
// ----------------------------------------------------------------------------

void wxMimeTypesManager::EnsureImpl()
{
    if (m_impl == NULL)
        m_impl = new wxMimeTypesManagerImpl;
}

bool wxMimeTypesManager::IsOfType(const wxString& mimeType,
                                  const wxString& wildcard)
{
    wxASSERT_MSG( mimeType.Find(wxT('*')) == wxNOT_FOUND,
                  wxT("first MIME type can't contain wildcards") );

    // all comparaisons are case insensitive (2nd arg of IsSameAs() is FALSE)
    if ( wildcard.BeforeFirst(wxT('/')).IsSameAs(mimeType.BeforeFirst(wxT('/')), FALSE) )
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
    if (m_impl != NULL)
        delete m_impl;
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromExtension(const wxString& ext)
{
    EnsureImpl();
    return m_impl->GetFileTypeFromExtension(ext);
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromMimeType(const wxString& mimeType)
{
    EnsureImpl();
    return m_impl->GetFileTypeFromMimeType(mimeType);
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
    for ( const wxFileTypeInfo *ft = filetypes; ft->IsValid(); ft++ ) {
        m_impl->AddFallback(*ft);
    }
}

size_t wxMimeTypesManager::EnumAllFileTypes(wxArrayString& mimetypes)
{
    EnsureImpl();
    return m_impl->EnumAllFileTypes(mimetypes);
}


// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// private object
static wxMimeTypesManager gs_mimeTypesManager;

// and public pointer
wxMimeTypesManager * wxTheMimeTypesManager = &gs_mimeTypesManager;





class wxMimeTypeCmnModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxMimeTypeCmnModule)
public:
    wxMimeTypeCmnModule() : wxModule() {}
    bool OnInit() { return TRUE; }
    void OnExit() 
    {   // this avoids false memory leak allerts:
        if (gs_mimeTypesManager.m_impl != NULL)
        {
            delete gs_mimeTypesManager.m_impl;
            gs_mimeTypesManager.m_impl = NULL;
        }
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxMimeTypeCmnModule, wxModule)

#endif // wxUSE_MIMETYPE
