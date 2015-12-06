/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE

#include "wx/msw/mimetype.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/crt.h"
    #if wxUSE_GUI
        #include "wx/icon.h"
        #include "wx/msgdlg.h"
    #endif
#endif //WX_PRECOMP

#include "wx/file.h"
#include "wx/iconloc.h"
#include "wx/confbase.h"
#include "wx/dynlib.h"

#ifdef __WINDOWS__
    #include "wx/msw/registry.h"
    #include "wx/msw/private.h"
    #include <shlwapi.h>

    // For MSVC we can link in the required library explicitly, for the other
    // compilers (e.g. MinGW) this needs to be done at makefiles level.
    #ifdef __VISUALC__
        #pragma comment(lib, "shlwapi")
    #endif
#endif // OS

// Unfortunately the corresponding SDK constants are absent from the headers
// shipped with some old MinGW versions (e.g. 4.2.1 from Debian) and we can't
// even test whether they're defined or not, as they're enum elements and not
// preprocessor constants. So we have to always use our own constants.
#define wxASSOCF_NOTRUNCATE (static_cast<ASSOCF>(0x20))
#define wxASSOCSTR_DEFAULTICON (static_cast<ASSOCSTR>(15))

// other standard headers
#include <ctype.h>

// in case we're compiling in non-GUI mode
class WXDLLIMPEXP_FWD_CORE wxIcon;

// These classes use Windows registry to retrieve the required information.
//
// Keys used (not all of them are documented, so it might actually stop working
// in future versions of Windows...):
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

// Notice that HKCR can be used only when reading from the registry, when
// writing to it, we need to write to HKCU\Software\Classes instead as HKCR is
// a merged view of that location and HKLM\Software\Classes that we generally
// wouldn't have the write permissions to but writing to HKCR will try writing
// to the latter unless the key being written to already exists under the
// former, resulting in a "Permission denied" error without administrative
// permissions. So the right thing to do is to use HKCR when reading, to
// respect both per-user and machine-global associations, but only write under
// HKCU.
static const wxChar *CLASSES_ROOT_KEY = wxS("Software\\Classes\\");

// although I don't know of any official documentation which mentions this
// location, uses it, so it isn't likely to change
static const wxChar *MIME_DATABASE_KEY = wxT("MIME\\Database\\Content Type\\");

// this function replaces Microsoft %1 with Unix-like %s
static bool CanonicalizeParams(wxString& command)
{
    // transform it from '%1' to '%s' style format string (now also test for %L
    // as apparently MS started using it as well for the same purpose)

    // NB: we don't make any attempt to verify that the string is valid, i.e.
    //     doesn't contain %2, or second %1 or .... But we do make sure that we
    //     return a string with _exactly_ one '%s'!
    bool foundFilename = false;
    size_t len = command.length();
    for ( size_t n = 0; (n < len) && !foundFilename; n++ )
    {
        if ( command[n] == wxT('%') &&
                (n + 1 < len) &&
                (command[n + 1] == wxT('1') || command[n + 1] == wxT('L')) )
        {
            // replace it with '%s'
            command[n + 1] = wxT('s');

            foundFilename = true;
        }
    }

    if ( foundFilename )
    {
        // Some values also contain an addition %* expansion string which is
        // presumably supposed to be replaced with the names of the other files
        // accepted by the command. As we don't support more than one file
        // anyhow, simply ignore it.
        command.Replace(" %*", "");
    }

    return foundFilename;
}

void wxFileTypeImpl::Init(const wxString& strFileType, const wxString& ext)
{
    // VZ: does it? (FIXME)
    wxCHECK_RET( !ext.empty(), wxT("needs an extension") );

    if ( ext[0u] != wxT('.') ) {
        m_ext = wxT('.');
    }
    m_ext << ext;

    m_strFileType = strFileType;
    if ( !strFileType ) {
        m_strFileType = m_ext.AfterFirst('.') + wxT("_auto_file");
    }
}

wxString wxFileTypeImpl::GetVerbPath(const wxString& verb) const
{
    wxString path;
    path << m_strFileType << wxT("\\shell\\") << verb << wxT("\\command");
    return path;
}

size_t wxFileTypeImpl::GetAllCommands(wxArrayString *verbs,
                                      wxArrayString *commands,
                                      const wxFileType::MessageParameters& params) const
{
    wxCHECK_MSG( !m_ext.empty(), 0, wxT("GetAllCommands() needs an extension") );

    if ( m_strFileType.empty() )
    {
        // get it from the registry
        wxFileTypeImpl *self = wxConstCast(this, wxFileTypeImpl);
        wxRegKey rkey(wxRegKey::HKCR, m_ext);
        if ( !rkey.Exists() || !rkey.QueryValue(wxEmptyString, self->m_strFileType) )
        {
            wxLogDebug(wxT("Can't get the filetype for extension '%s'."),
                       m_ext.c_str());

            return 0;
        }
    }

    // enum all subkeys of HKCR\filetype\shell
    size_t count = 0;
    wxRegKey rkey(wxRegKey::HKCR, m_strFileType  + wxT("\\shell"));
    long dummy;
    wxString verb;
    bool ok = rkey.GetFirstKey(verb, dummy);
    while ( ok )
    {
        wxString command = wxFileType::ExpandCommand(GetCommand(verb), params);

        // we want the open bverb to eb always the first

        if ( verb.CmpNoCase(wxT("open")) == 0 )
        {
            if ( verbs )
                verbs->Insert(verb, 0);
            if ( commands )
                commands->Insert(command, 0);
        }
        else // anything else than "open"
        {
            if ( verbs )
                verbs->Add(verb);
            if ( commands )
                commands->Add(command);
        }

        count++;

        ok = rkey.GetNextKey(verb, dummy);
    }

    return count;
}

// ----------------------------------------------------------------------------
// modify the registry database
// ----------------------------------------------------------------------------

bool wxFileTypeImpl::EnsureExtKeyExists()
{
    wxRegKey rkey(wxRegKey::HKCU, CLASSES_ROOT_KEY + m_ext);
    if ( !rkey.Exists() )
    {
        if ( !rkey.Create() || !rkey.SetValue(wxEmptyString, m_strFileType) )
        {
            wxLogError(_("Failed to create registry entry for '%s' files."),
                       m_ext.c_str());
            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// get the command to use
// ----------------------------------------------------------------------------

// Helper wrapping AssocQueryString() Win32 function: returns the value of the
// given associated string for the specified extension (which may or not have
// the leading period).
//
// Returns empty string if the association is not found.
static
wxString wxAssocQueryString(ASSOCSTR assoc,
                            wxString ext,
                            const wxString& verb = wxString())
{
    DWORD dwSize = MAX_PATH;
    TCHAR bufOut[MAX_PATH] = { 0 };

    if ( ext.empty() || ext[0] != '.' )
        ext.Prepend('.');

    HRESULT hr = ::AssocQueryString
                 (
                    wxASSOCF_NOTRUNCATE,// Fail if buffer is too small.
                    assoc,              // The association to retrieve.
                    ext.t_str(),        // The extension to retrieve it for.
                    verb.empty() ? NULL
                                 : static_cast<const TCHAR*>(verb.t_str()),
                    bufOut,             // The buffer for output value.
                    &dwSize             // And its size
                 );

    // Do not use SUCCEEDED() here as S_FALSE could, in principle, be returned
    // but would still be an error in this context.
    if ( hr != S_OK )
    {
        // The only really expected error here is that no association is
        // defined, anything else is not expected. The confusing thing is that
        // different errors are returned for this expected error under
        // different Windows versions: XP returns ERROR_FILE_NOT_FOUND while 7
        // returns ERROR_NO_ASSOCIATION. Just check for both to be sure.
        if ( hr != HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION) &&
                hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) )
        {
            wxLogApiError("AssocQueryString", hr);
        }

        return wxString();
    }

    return wxString(bufOut);
}


wxString wxFileTypeImpl::GetCommand(const wxString& verb) const
{
    wxString command = wxAssocQueryString(ASSOCSTR_COMMAND, m_ext, verb);
    bool foundFilename = CanonicalizeParams(command);

#if wxUSE_IPC
    wxString ddeCommand = wxAssocQueryString(ASSOCSTR_DDECOMMAND, m_ext);
    wxString ddeTopic = wxAssocQueryString(ASSOCSTR_DDETOPIC, m_ext);

    if ( !ddeCommand.empty() && !ddeTopic.empty() )
    {
        wxString ddeServer = wxAssocQueryString( ASSOCSTR_DDEAPPLICATION, m_ext );

        ddeCommand.Replace(wxT("%1"), wxT("%s"));

        if ( ddeTopic.empty() )
            ddeTopic = wxT("System");

        // HACK: we use a special feature of wxExecute which exists
        //       just because we need it here: it will establish DDE
        //       conversation with the program it just launched
        command.Prepend(wxT("WX_DDE#"));
        command << wxT('#') << ddeServer
                << wxT('#') << ddeTopic
                << wxT('#') << ddeCommand;
    }
    else
#endif // wxUSE_IPC
    if ( !foundFilename && !command.empty() )
    {
        // we didn't find any '%1' - the application doesn't know which
        // file to open (note that we only do it if there is no DDEExec
        // subkey)
        //
        // HACK: append the filename at the end, hope that it will do
        command << wxT(" %s");
    }

    return command;
}

bool
wxFileTypeImpl::GetOpenCommand(wxString *openCmd,
                               const wxFileType::MessageParameters& params)
                               const
{
    wxString cmd = GetCommand(wxT("open"));

    // Some viewers don't define the "open" verb but do define "show" one, try
    // to use it as a fallback.
    if ( cmd.empty() )
        cmd = GetCommand(wxT("show"));

    *openCmd = wxFileType::ExpandCommand(cmd, params);

    return !openCmd->empty();
}

bool
wxFileTypeImpl::GetPrintCommand(wxString *printCmd,
                                const wxFileType::MessageParameters& params)
                                const
{
    wxString cmd = GetCommand(wxT("print"));

    *printCmd = wxFileType::ExpandCommand(cmd, params);

    return !printCmd->empty();
}

// ----------------------------------------------------------------------------
// getting other stuff
// ----------------------------------------------------------------------------

// TODO this function is half implemented
bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    if ( m_ext.empty() ) {
        // the only way to get the list of extensions from the file type is to
        // scan through all extensions in the registry - too slow...
        return false;
    }
    else {
        extensions.Empty();
        extensions.Add(m_ext);

        // it's a lie too, we don't return _all_ extensions...
        return true;
    }
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, m_ext);

    return key.Open(wxRegKey::Read) &&
                key.QueryValue(wxT("Content Type"), *mimeType);
}

bool wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
    wxString s;

    if ( !GetMimeType(&s) )
    {
        return false;
    }

    mimeTypes.Clear();
    mimeTypes.Add(s);
    return true;
}


bool wxFileTypeImpl::GetIcon(wxIconLocation *iconLoc) const
{
    wxString strIcon = wxAssocQueryString(wxASSOCSTR_DEFAULTICON, m_ext);

    if ( !strIcon.empty() )
    {
        wxString strFullPath = strIcon.BeforeLast(wxT(',')),
        strIndex = strIcon.AfterLast(wxT(','));

        // index may be omitted, in which case BeforeLast(',') is empty and
        // AfterLast(',') is the whole string
        if ( strFullPath.empty() ) {
            strFullPath = strIndex;
            strIndex = wxT("0");
        }

        // if the path contains spaces, it can be enclosed in quotes but we
        // must not pass a filename in that format to any file system function,
        // so remove them here.
        if ( strFullPath.StartsWith('"') && strFullPath.EndsWith('"') )
            strFullPath = strFullPath.substr(1, strFullPath.length() - 2);

        if ( iconLoc )
        {
            iconLoc->SetFileName(wxExpandEnvVars(strFullPath));

            iconLoc->SetIndex(wxAtoi(strIndex));
        }

      return true;
    }

    // no such file type or no value or incorrect icon entry
    return false;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, m_strFileType);

    if ( key.Open(wxRegKey::Read) ) {
        // it's the default value of the key
        if ( key.QueryValue(wxEmptyString, *desc) ) {
            return true;
        }
    }

    return false;
}

// helper function
wxFileType *
wxMimeTypesManagerImpl::CreateFileType(const wxString& filetype, const wxString& ext)
{
    wxFileType *fileType = new wxFileType;
    fileType->m_impl->Init(filetype, ext);
    return fileType;
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

    bool knownExtension = false;

    wxString strFileType;
    wxRegKey key(wxRegKey::HKCR, str);
    if ( key.Open(wxRegKey::Read) ) {
        // it's the default value of the key
        if ( key.QueryValue(wxEmptyString, strFileType) ) {
            // create the new wxFileType object
            return CreateFileType(strFileType, ext);
        }
        else {
            // this extension doesn't have a filetype, but it's known to the
            // system and may be has some other useful keys (open command or
            // content-type), so still return a file type object for it
            knownExtension = true;
        }
    }

    if ( !knownExtension )
    {
        // unknown extension
        return NULL;
    }

    return CreateFileType(wxEmptyString, ext);
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
    if ( key.Open(wxRegKey::Read) ) {
        if ( key.QueryValue(wxT("Extension"), ext) ) {
            return GetFileTypeFromExtension(ext);
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

// ----------------------------------------------------------------------------
// create a new association
// ----------------------------------------------------------------------------

wxFileType *wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& ftInfo)
{
    wxCHECK_MSG( !ftInfo.GetExtensions().empty(), NULL,
                 wxT("Associate() needs extension") );

    bool ok;
    size_t iExtCount = 0;
    wxString filetype;
    wxString extWithDot;

    wxString ext = ftInfo.GetExtensions()[iExtCount];

    wxCHECK_MSG( !ext.empty(), NULL,
                 wxT("Associate() needs non empty extension") );

    if ( ext[0u] != wxT('.') )
        extWithDot = wxT('.');
    extWithDot += ext;

    // start by setting the entries under ".ext"
    // default is filetype; content type is mimetype
    const wxString& filetypeOrig = ftInfo.GetShortDesc();

    wxRegKey key(wxRegKey::HKCU, CLASSES_ROOT_KEY + extWithDot);
    if ( !key.Exists() )
    {
        // create the mapping from the extension to the filetype
        ok = key.Create();
        if ( ok )
        {

            if ( filetypeOrig.empty() )
            {
                // make it up from the extension
                filetype << extWithDot.c_str() + 1 << wxT("_file");
            }
            else
            {
                // just use the provided one
                filetype = filetypeOrig;
            }

            key.SetValue(wxEmptyString, filetype);
        }
    }
    else
    {
        // key already exists, maybe we want to change it ??
        if (!filetypeOrig.empty())
        {
            filetype = filetypeOrig;
            key.SetValue(wxEmptyString, filetype);
        }
        else
        {
            key.QueryValue(wxEmptyString, filetype);
        }
    }

    // now set a mimetypeif we have it, but ignore it if none
    const wxString& mimetype = ftInfo.GetMimeType();
    if ( !mimetype.empty() )
    {
        // set the MIME type
        ok = key.SetValue(wxT("Content Type"), mimetype);

        if ( ok )
        {
            // create the MIME key
            wxString strKey = MIME_DATABASE_KEY;
            strKey << mimetype;
            wxRegKey keyMIME(wxRegKey::HKCU, CLASSES_ROOT_KEY + strKey);
            ok = keyMIME.Create();

            if ( ok )
            {
                // and provide a back link to the extension
                keyMIME.SetValue(wxT("Extension"), extWithDot);
            }
        }
    }


    // now make other extensions have the same filetype

    for (iExtCount=1; iExtCount < ftInfo.GetExtensionsCount(); iExtCount++ )
    {
        ext = ftInfo.GetExtensions()[iExtCount];
        if ( ext[0u] != wxT('.') )
           extWithDot = wxT('.');
        extWithDot += ext;

        wxRegKey key2(wxRegKey::HKCU, CLASSES_ROOT_KEY + extWithDot);
        if ( !key2.Exists() )
            key2.Create();
        key2.SetValue(wxEmptyString, filetype);

        // now set any mimetypes we may have, but ignore it if none
        const wxString& mimetype2 = ftInfo.GetMimeType();
        if ( !mimetype2.empty() )
        {
            // set the MIME type
            ok = key2.SetValue(wxT("Content Type"), mimetype2);

            if ( ok )
            {
                // create the MIME key
                wxString strKey = MIME_DATABASE_KEY;
                strKey << mimetype2;
                wxRegKey keyMIME(wxRegKey::HKCU, CLASSES_ROOT_KEY + strKey);
                ok = keyMIME.Create();

                if ( ok )
                {
                    // and provide a back link to the extension
                    keyMIME.SetValue(wxT("Extension"), extWithDot);
                }
            }
        }

    } // end of for loop; all extensions now point to .ext\Default

    // create the filetype key itself (it will be empty for now, but
    // SetCommand(), SetDefaultIcon() &c will use it later)
    wxRegKey keyFT(wxRegKey::HKCU, CLASSES_ROOT_KEY + filetype);
    keyFT.Create();

    wxFileType *ft = CreateFileType(filetype, extWithDot);

    if (ft)
    {
        if (! ftInfo.GetOpenCommand ().empty() ) ft->SetCommand (ftInfo.GetOpenCommand (), wxT("open"  ) );
        if (! ftInfo.GetPrintCommand().empty() ) ft->SetCommand (ftInfo.GetPrintCommand(), wxT("print" ) );
        // chris: I don't like the ->m_impl-> here FIX this ??
        if (! ftInfo.GetDescription ().empty() ) ft->m_impl->SetDescription (ftInfo.GetDescription ()) ;
        if (! ftInfo.GetIconFile().empty() ) ft->SetDefaultIcon (ftInfo.GetIconFile(), ftInfo.GetIconIndex() );

    }

    return ft;
}

bool wxFileTypeImpl::SetCommand(const wxString& cmd,
                                const wxString& verb,
                                bool WXUNUSED(overwriteprompt))
{
    wxCHECK_MSG( !m_ext.empty() && !verb.empty(), false,
                 wxT("SetCommand() needs an extension and a verb") );

    if ( !EnsureExtKeyExists() )
        return false;

    wxRegKey rkey(wxRegKey::HKCU, CLASSES_ROOT_KEY + GetVerbPath(verb));

    // TODO:
    // 1. translate '%s' to '%1' instead of always adding it
    // 2. create DDEExec value if needed (undo GetCommand)
    return rkey.Create() && rkey.SetValue(wxEmptyString, cmd + wxT(" \"%1\"") );
}

bool wxFileTypeImpl::SetDefaultIcon(const wxString& cmd, int index)
{
    wxCHECK_MSG( !m_ext.empty(), false, wxT("SetDefaultIcon() needs extension") );
    wxCHECK_MSG( !m_strFileType.empty(), false, wxT("File key not found") );
//    the next line fails on a SMBshare, I think because it is case mangled
//    wxCHECK_MSG( !wxFileExists(cmd), false, wxT("Icon file not found.") );

    if ( !EnsureExtKeyExists() )
        return false;

    wxRegKey rkey(wxRegKey::HKCU,
                  CLASSES_ROOT_KEY + m_strFileType + wxT("\\DefaultIcon"));

    return rkey.Create() &&
           rkey.SetValue(wxEmptyString,
                         wxString::Format(wxT("%s,%d"), cmd.c_str(), index));
}

bool wxFileTypeImpl::SetDescription (const wxString& desc)
{
    wxCHECK_MSG( !m_strFileType.empty(), false, wxT("File key not found") );
    wxCHECK_MSG( !desc.empty(), false, wxT("No file description supplied") );

    if ( !EnsureExtKeyExists() )
        return false;

    wxRegKey rkey(wxRegKey::HKCU, CLASSES_ROOT_KEY + m_strFileType );

    return rkey.Create() &&
           rkey.SetValue(wxEmptyString, desc);
}

// ----------------------------------------------------------------------------
// remove file association
// ----------------------------------------------------------------------------

bool wxFileTypeImpl::Unassociate()
{
    bool result = true;
    if ( !RemoveOpenCommand() )
        result = false;
    if ( !RemoveDefaultIcon() )
        result = false;
    if ( !RemoveMimeType() )
        result = false;
    if ( !RemoveDescription() )
        result = false;

    return result;
}

bool wxFileTypeImpl::RemoveOpenCommand()
{
   return RemoveCommand(wxT("open"));
}

bool wxFileTypeImpl::RemoveCommand(const wxString& verb)
{
    wxCHECK_MSG( !m_ext.empty() && !verb.empty(), false,
                 wxT("RemoveCommand() needs an extension and a verb") );

    wxRegKey rkey(wxRegKey::HKCU, CLASSES_ROOT_KEY + GetVerbPath(verb));

    // if the key already doesn't exist, it's a success
    return !rkey.Exists() || rkey.DeleteSelf();
}

bool wxFileTypeImpl::RemoveMimeType()
{
    wxCHECK_MSG( !m_ext.empty(), false, wxT("RemoveMimeType() needs extension") );

    wxRegKey rkey(wxRegKey::HKCU, CLASSES_ROOT_KEY + m_ext);
    return !rkey.Exists() || rkey.DeleteSelf();
}

bool wxFileTypeImpl::RemoveDefaultIcon()
{
    wxCHECK_MSG( !m_ext.empty(), false,
                 wxT("RemoveDefaultIcon() needs extension") );

    wxRegKey rkey (wxRegKey::HKCU,
                   CLASSES_ROOT_KEY + m_strFileType  + wxT("\\DefaultIcon"));
    return !rkey.Exists() || rkey.DeleteSelf();
}

bool wxFileTypeImpl::RemoveDescription()
{
    wxCHECK_MSG( !m_ext.empty(), false,
                 wxT("RemoveDescription() needs extension") );

    wxRegKey rkey (wxRegKey::HKCU, CLASSES_ROOT_KEY + m_strFileType );
    return !rkey.Exists() || rkey.DeleteSelf();
}

#endif // wxUSE_MIMETYPE
