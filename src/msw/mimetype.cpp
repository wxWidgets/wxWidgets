/////////////////////////////////////////////////////////////////////////////
// Name:        msw/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifdef    __GNUG__
#pragma implementation "mimetype.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_MIMETYPE

// Doesn't compile in WIN16 mode
#ifndef __WIN16__

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
#endif // OS

#include "wx/msw/mimetype.h"

// other standard headers
#include <ctype.h>

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;

// These classes use Windows registry to retrieve the required information.
//
// Keys used (not all of them are documented, so it might actually stop working
// in futur versions of Windows...):
//  1. "HKCR\MIME\Database\Content Type" contains subkeys for all known MIME
//     types, each key has a string value "Extension" which gives (dot preceded)
//     extension for the files of this MIME type.
//
//  2. "HKCR\.ext" contains
//   a) unnamed value containing the "filetype"
//   b) value "Content Type" containing the MIME type
//
// 3. "HKCR\filetype" contains
//   a) unnamed value containing the description
//   b) subkey "DefaultIcon" with single unnamed value giving the icon index in
//      an icon file
//   c) shell\open\command and shell\open\print subkeys containing the commands
//      to open/print the file (the positional parameters are introduced by %1,
//      %2, ... in these strings, we change them to %s ourselves)

// although I don't know of any official documentation which mentions this
// location, uses it, so it isn't likely to change
static const wxChar *MIME_DATABASE_KEY = wxT("MIME\\Database\\Content Type\\");

wxString wxFileTypeImpl::GetCommand(const wxChar *verb) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxString strKey;

    if ( wxRegKey(wxRegKey::HKCR, m_ext + _T("\\shell")).Exists() )
        strKey = m_ext;
    if ( wxRegKey(wxRegKey::HKCR, m_strFileType + _T("\\shell")).Exists() )
        strKey = m_strFileType;

    if ( !strKey )
    {
        // no info
        return wxEmptyString;
    }

    strKey << wxT("\\shell\\") << verb;
    wxRegKey key(wxRegKey::HKCR, strKey + _T("\\command"));
    wxString command;
    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue(wxT(""), command) ) {
            // transform it from '%1' to '%s' style format string (now also
            // test for %L - apparently MS started using it as well for the
            // same purpose)

            // NB: we don't make any attempt to verify that the string is valid,
            //     i.e. doesn't contain %2, or second %1 or .... But we do make
            //     sure that we return a string with _exactly_ one '%s'!
            bool foundFilename = FALSE;
            size_t len = command.Len();
            for ( size_t n = 0; (n < len) && !foundFilename; n++ ) {
                if ( command[n] == wxT('%') &&
                     (n + 1 < len) &&
                     (command[n + 1] == wxT('1') ||
                      command[n + 1] == wxT('L')) ) {
                    // replace it with '%s'
                    command[n + 1] = wxT('s');

                    foundFilename = TRUE;
                }
            }

#if wxUSE_IPC
            // look whether we must issue some DDE requests to the application
            // (and not just launch it)
            strKey += _T("\\DDEExec");
            wxRegKey keyDDE(wxRegKey::HKCR, strKey);
            if ( keyDDE.Open() ) {
                wxString ddeCommand, ddeServer, ddeTopic;
                keyDDE.QueryValue(_T(""), ddeCommand);
                ddeCommand.Replace(_T("%1"), _T("%s"));

                wxRegKey(wxRegKey::HKCR, strKey + _T("\\Application")).
                    QueryValue(_T(""), ddeServer);
                wxRegKey(wxRegKey::HKCR, strKey + _T("\\Topic")).
                    QueryValue(_T(""), ddeTopic);

                // HACK: we use a special feature of wxExecute which exists
                //       just because we need it here: it will establish DDE
                //       conversation with the program it just launched
                command.Prepend(_T("WX_DDE#"));
                command << _T('#') << ddeServer
                        << _T('#') << ddeTopic
                        << _T('#') << ddeCommand;
            }
            else
#endif // wxUSE_IPC
                if ( !foundFilename ) {
                // we didn't find any '%1' - the application doesn't know which
                // file to open (note that we only do it if there is no DDEExec
                // subkey)
                //
                // HACK: append the filename at the end, hope that it will do
                command << wxT(" %s");
            }
        }
    }
    //else: no such file type or no value, will return empty string

    return command;
}

bool
wxFileTypeImpl::GetOpenCommand(wxString *openCmd,
                               const wxFileType::MessageParameters& params)
                               const
{
    wxString cmd;
    if ( m_info ) {
        cmd = m_info->GetOpenCommand();
    }
    else {
        cmd = GetCommand(wxT("open"));
    }

    *openCmd = wxFileType::ExpandCommand(cmd, params);

    return !openCmd->IsEmpty();
}

bool
wxFileTypeImpl::GetPrintCommand(wxString *printCmd,
                                const wxFileType::MessageParameters& params)
                                const
{
    wxString cmd;
    if ( m_info ) {
        cmd = m_info->GetPrintCommand();
    }
    else {
        cmd = GetCommand(wxT("print"));
    }

    *printCmd = wxFileType::ExpandCommand(cmd, params);

    return !printCmd->IsEmpty();
}

// TODO this function is half implemented
bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    if ( m_info ) {
        extensions = m_info->GetExtensions();

        return TRUE;
    }
    else if ( m_ext.IsEmpty() ) {
        // the only way to get the list of extensions from the file type is to
        // scan through all extensions in the registry - too slow...
        return FALSE;
    }
    else {
        extensions.Empty();
        extensions.Add(m_ext);

        // it's a lie too, we don't return _all_ extensions...
        return TRUE;
    }
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    if ( m_info ) {
        // we already have it
        *mimeType = m_info->GetMimeType();

        return TRUE;
    }

    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, wxT(".") + m_ext);

    return key.Open() && key.QueryValue(wxT("Content Type"), *mimeType);
}


bool wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
    wxString s;

    if ( !GetMimeType(&s) )
    {
        return FALSE;
    }

    mimeTypes.Clear();
    mimeTypes.Add(s);
    return TRUE;
}


bool wxFileTypeImpl::GetIcon(wxIcon *icon) const
{
#if wxUSE_GUI
    if ( m_info ) {
        // we don't have icons in the fallback resources
        return FALSE;
    }

    wxString strIconKey;
    strIconKey << m_strFileType << wxT("\\DefaultIcon");

    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, strIconKey);

    if ( key.Open() ) {
        wxString strIcon;
        // it's the default value of the key
        if ( key.QueryValue(wxT(""), strIcon) ) {
            // the format is the following: <full path to file>, <icon index>
            // NB: icon index may be negative as well as positive and the full
            //     path may contain the environment variables inside '%'
            wxString strFullPath = strIcon.BeforeLast(wxT(',')),
            strIndex = strIcon.AfterLast(wxT(','));

            // index may be omitted, in which case BeforeLast(',') is empty and
            // AfterLast(',') is the whole string
            if ( strFullPath.IsEmpty() ) {
                strFullPath = strIndex;
                strIndex = wxT("0");
            }

            wxString strExpPath = wxExpandEnvVars(strFullPath);
            int nIndex = wxAtoi(strIndex);

            HICON hIcon = ExtractIcon(GetModuleHandle(NULL), strExpPath, nIndex);
            switch ( (int)hIcon ) {
                case 0: // means no icons were found
                case 1: // means no such file or it wasn't a DLL/EXE/OCX/ICO/...
                    wxLogDebug(wxT("incorrect registry entry '%s': no such icon."),
                               key.GetName().c_str());
                    break;

                default:
                    icon->SetHICON((WXHICON)hIcon);
                    return TRUE;
            }
        }
    }

    // no such file type or no value or incorrect icon entry
#endif // wxUSE_GUI

    return FALSE;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    if ( m_info ) {
        // we already have it
        *desc = m_info->GetDescription();

        return TRUE;
    }

    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, m_strFileType);

    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue(wxT(""), *desc) ) {
            return TRUE;
        }
    }

    return FALSE;
}

// extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& ext)
{
    // add the leading point if necessary
    wxString str;
    if ( ext[0u] != wxT('.') ) {
        str = wxT('.');
    }
    str << ext;

    // suppress possible error messages
    wxLogNull nolog;

    bool knownExtension = FALSE;

    wxString strFileType;
    wxRegKey key(wxRegKey::HKCR, str);
    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue(wxT(""), strFileType) ) {
            // create the new wxFileType object
            wxFileType *fileType = new wxFileType;
            fileType->m_impl->Init(strFileType, ext);

            return fileType;
        }
        else {
            // this extension doesn't have a filetype, but it's known to the
            // system and may be has some other useful keys (open command or
            // content-type), so still return a file type object for it
            knownExtension = TRUE;
        }
    }

    // check the fallbacks
    // TODO linear search is potentially slow, perhaps we should use a sorted
    //      array?
    size_t count = m_fallbacks.GetCount();
    for ( size_t n = 0; n < count; n++ ) {
        if ( m_fallbacks[n].GetExtensions().Index(ext) != wxNOT_FOUND ) {
            wxFileType *fileType = new wxFileType;
            fileType->m_impl->Init(m_fallbacks[n]);

            return fileType;
        }
    }

    if ( !knownExtension )
    {
        // unknown extension
        return NULL;
    }

    wxFileType *fileType = new wxFileType;
    fileType->m_impl->Init(wxEmptyString, ext);

    return fileType;
}

// MIME type -> extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    wxString strKey = MIME_DATABASE_KEY;
    strKey << mimeType;

    // suppress possible error messages
    wxLogNull nolog;

    wxString ext;
    wxRegKey key(wxRegKey::HKCR, strKey);
    if ( key.Open() ) {
        if ( key.QueryValue(wxT("Extension"), ext) ) {
            return GetFileTypeFromExtension(ext);
        }
    }

    // check the fallbacks
    // TODO linear search is potentially slow, perhaps we should use a sorted
    //      array?
    size_t count = m_fallbacks.GetCount();
    for ( size_t n = 0; n < count; n++ ) {
        if ( wxMimeTypesManager::IsOfType(mimeType,
                                          m_fallbacks[n].GetMimeType()) ) {
            wxFileType *fileType = new wxFileType;
            fileType->m_impl->Init(m_fallbacks[n]);

            return fileType;
        }
    }

    // unknown MIME type
    return NULL;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& mimetypes)
{
    // enumerate all keys under MIME_DATABASE_KEY
    wxRegKey key(wxRegKey::HKCR, MIME_DATABASE_KEY);

    wxString type;
    long cookie;
    bool cont = key.GetFirstKey(type, cookie);
    while ( cont )
    {
        mimetypes.Add(type);

        cont = key.GetNextKey(type, cookie);
    }

    return mimetypes.GetCount();
}


#endif
  // __WIN16__

#endif // wxUSE_MIMETYPE
