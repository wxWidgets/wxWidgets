/////////////////////////////////////////////////////////////////////////////
// Name:        common/mimetype.cpp
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

// wxWindows
#ifndef WX_PRECOMP
    #include  "wx/string.h"
    #include  "wx/icon.h"
#endif //WX_PRECOMP

// Doesn't compile in WIN16 mode
#ifndef __WIN16__

#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/confbase.h"

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
    #include "windows.h"
#else  // Unix
    #include "wx/textfile.h"
#endif // OS

#include "wx/mimetype.h"

// other standard headers
#include <ctype.h>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// implementation classes, platform dependent
#ifdef __WXMSW__

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

class wxFileTypeImpl
{
public:
    // ctor
    wxFileTypeImpl() { }

    // initialize us with our file type name
    void SetFileType(const wxString& strFileType)
        { m_strFileType = strFileType; }
    void SetExt(const wxString& ext)
        { m_ext = ext; }

    // implement accessor functions
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const;
    bool GetIcon(wxIcon *icon) const;
    bool GetDescription(wxString *desc) const;
    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters&) const
        { return GetCommand(openCmd, "open"); }
    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters&) const
        { return GetCommand(printCmd, "print"); }

private:
    // helper function
    bool GetCommand(wxString *command, const char *verb) const;

    wxString m_strFileType, m_ext;
};

class wxMimeTypesManagerImpl
{
public:
    // nothing to do here, we don't load any data but just go and fetch it from
    // the registry when asked for
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    // this are NOPs under Windows
    void ReadMailcap(const wxString& filename) { }
    void ReadMimeTypes(const wxString& filename) { }
};

#else  // Unix

// this class uses both mailcap and mime.types to gather information about file
// types.
//
// The information about mailcap file was extracted from metamail(1) sources and
// documentation.
//
// Format of mailcap file: spaces are ignored, each line is either a comment
// (starts with '#') or a line of the form <field1>;<field2>;...;<fieldN>.
// A backslash can be used to quote semicolons and newlines (and, in fact,
// anything else including itself).
//
// The first field is always the MIME type in the form of type/subtype (see RFC
// 822) where subtype may be '*' meaning "any". Following metamail, we accept
// "type" which means the same as "type/*", although I'm not sure whether this
// is standard.
//
// The second field is always the command to run. It is subject to
// parameter/filename expansion described below.
//
// All the following fields are optional and may not be present at all. If
// they're present they may appear in any order, although each of them should
// appear only once. The optional fields are the following:
//  * notes=xxx is an uninterpreted string which is silently ignored
//  * test=xxx is the command to be used to determine whether this mailcap line
//    applies to our data or not. The RHS of this field goes through the
//    parameter/filename expansion (as the 2nd field) and the resulting string
//    is executed. The line applies only if the command succeeds, i.e. returns 0
//    exit code.
//  * print=xxx is the command to be used to print (and not view) the data of
//    this type (parameter/filename expansion is done here too)
//  * edit=xxx is the command to open/edit the data of this type
//  * needsterminal means that a new console must be created for the viewer
//  * copiousoutput means that the viewer doesn't interact with the user but
//    produces (possibly) a lof of lines of output on stdout (i.e. "cat" is a
//    good example), thus it might be a good idea to use some kind of paging
//    mechanism.
//  * textualnewlines means not to perform CR/LF translation (not honored)
//  * compose and composetyped fields are used to determine the program to be
//    called to create a new message pert in the specified format (unused).
//
// Parameter/filename xpansion:
//  * %s is replaced with the (full) file name
//  * %t is replaced with MIME type/subtype of the entry
//  * for multipart type only %n is replaced with the nnumber of parts and %F is
//    replaced by an array of (content-type, temporary file name) pairs for all
//    message parts (TODO)
//  * %{parameter} is replaced with the value of parameter taken from
//    Content-type header line of the message.
//
// FIXME any docs with real descriptions of these files??
//
// There are 2 possible formats for mime.types file, one entry per line (used
// for global mime.types) and "expanded" format where an entry takes multiple
// lines (used for users mime.types).
//
// For both formats spaces are ignored and lines starting with a '#' are
// comments. Each record has one of two following forms:
//  a) for "brief" format:
//      <mime type>  <space separated list of extensions>
//  b) for "expanded" format:
//      type=<mime type> \ desc="<description>" \ exts="ext"
//
// We try to autodetect the format of mime.types: if a non-comment line starts
// with "type=" we assume the second format, otherwise the first one.

// there may be more than one entry for one and the same mime type, to
// choose the right one we have to run the command specified in the test
// field on our data.
class MailCapEntry
{
public:
    // ctor
    MailCapEntry(const wxString& openCmd,
                 const wxString& printCmd,
                 const wxString& testCmd)
        : m_openCmd(openCmd), m_printCmd(printCmd), m_testCmd(testCmd)
    {
        m_next = NULL;
    }

    // accessors
    const wxString& GetOpenCmd()  const { return m_openCmd;  }
    const wxString& GetPrintCmd() const { return m_printCmd; }
    const wxString& GetTestCmd()  const { return m_testCmd;  }

    MailCapEntry *GetNext() const { return m_next; }

    // operations
        // prepend this element to the list
    void Prepend(MailCapEntry *next) { m_next = next; }
        // append to the list
    void Append(MailCapEntry *next)
    {
        // FIXME slooow...
        MailCapEntry *cur;
        for ( cur = next; cur->m_next != NULL; cur = cur->m_next )
            ;

        cur->m_next = this;

        // we initialize it in the ctor and there is no reason to both Prepend()
        // and Append() one and the same entry
        wxASSERT( m_next == NULL );
    }

private:
    wxString m_openCmd,         // command to use to open/view the file
             m_printCmd,        //                     print
             m_testCmd;         // only apply this entry if test yields
                                // true (i.e. the command returns 0)

    MailCapEntry *m_next;       // in the linked list
};

WX_DEFINE_ARRAY(MailCapEntry *, ArrayTypeEntries);

class wxMimeTypesManagerImpl
{
friend class wxFileTypeImpl; // give it access to m_aXXX variables

public:
    // ctor loads all info into memory for quicker access later on
    // @@ it would be nice to load them all, but parse on demand only...
    wxMimeTypesManagerImpl();

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    void ReadMailcap(const wxString& filename);
    void ReadMimeTypes(const wxString& filename);

    // accessors
        // get the string containing space separated extensions for the given
        // file type
    wxString GetExtension(size_t index) { return m_aExtensions[index]; }

private:
    wxArrayString m_aTypes,         // MIME types
                  m_aDescriptions,  // descriptions (just some text)
                  m_aExtensions;    // space separated list of extensions
    ArrayTypeEntries m_aEntries;    // commands and tests for this file type
};

class wxFileTypeImpl
{
public:
    // initialization functions
    void Init(wxMimeTypesManagerImpl *manager, size_t index)
        { m_manager = manager; m_index = index; }

    // accessors
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const
        { *mimeType = m_manager->m_aTypes[m_index]; return TRUE; }
    bool GetIcon(wxIcon *icon) const
        { return FALSE; }   // @@ maybe with Gnome/KDE integration...
    bool GetDescription(wxString *desc) const
        { *desc = m_manager->m_aDescriptions[m_index]; return TRUE; }

    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const
    {
        return GetExpandedCommand(openCmd, params, TRUE);
    }

    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const
    {
        return GetExpandedCommand(printCmd, params, FALSE);
    }

private:
    // get the entry which passes the test (may return NULL)
    MailCapEntry *GetEntry(const wxFileType::MessageParameters& params) const;

    // choose the correct entry to use and expand the command
    bool GetExpandedCommand(wxString *expandedCmd,
                            const wxFileType::MessageParameters& params,
                            bool open) const;

    wxMimeTypesManagerImpl *m_manager;
    size_t                  m_index; // in the wxMimeTypesManagerImpl arrays
};

#endif // OS type

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
    for ( const char *pc = command.c_str(); *pc != '\0'; pc++ ) {
        if ( *pc == '%' ) {
            switch ( *++pc ) {
                case 's':
                    // '%s' expands into file name (quoted because it might
                    // contain spaces) - except if there are already quotes
                    // there because otherwise some programs may get confused
                    // by double double quotes
#if 0
                    if ( *(pc - 2) == '"' )
                        str << params.GetFileName();
                    else
                        str << '"' << params.GetFileName() << '"';
#endif
                    str << params.GetFileName();
                    hasFilename = TRUE;
                    break;

                case 't':
                    // '%t' expands into MIME type (quote it too just to be
                    // consistent)
                    str << '\'' << params.GetMimeType() << '\'';
                    break;

                case '{':
                    {
                        const char *pEnd = strchr(pc, '}');
                        if ( pEnd == NULL ) {
                            wxString mimetype;
                            wxLogWarning(_("Unmatched '{' in an entry for "
                                           "mime type %s."),
                                         params.GetMimeType().c_str());
                            str << "%{";
                        }
                        else {
                            wxString param(pc + 1, pEnd - pc - 1);
                            str << '\'' << params.GetParamValue(param) << '\'';
                            pc = pEnd;
                        }
                    }
                    break;

                case 'n':
                case 'F':
                    // TODO %n is the number of parts, %F is an array containing
                    //      the names of temp files these parts were written to
                    //      and their mime types.
                    break;

                default:
                    wxLogDebug("Unknown field %%%c in command '%s'.",
                               *pc, command.c_str());
                    str << *pc;
            }
        }
        else {
            str << *pc;
        }
    }

    // metamail(1) man page states that if the mailcap entry doesn't have '%s'
    // the program will accept the data on stdin: so give it to it!
    if ( !hasFilename && !str.IsEmpty() ) {
        str << " < '" << params.GetFileName() << '\'';
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

wxMimeTypesManager::wxMimeTypesManager()
{
    m_impl = new wxMimeTypesManagerImpl;
}

wxMimeTypesManager::~wxMimeTypesManager()
{
    delete m_impl;
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromExtension(const wxString& ext)
{
    return m_impl->GetFileTypeFromExtension(ext);
}

wxFileType *
wxMimeTypesManager::GetFileTypeFromMimeType(const wxString& mimeType)
{
    return m_impl->GetFileTypeFromMimeType(mimeType);
}

// ============================================================================
// real (OS specific) implementation
// ============================================================================

#ifdef __WXMSW__

bool wxFileTypeImpl::GetCommand(wxString *command, const char *verb) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxString strKey;
    strKey << m_strFileType << "\\shell\\" << verb << "\\command";
    wxRegKey key(wxRegKey::HKCR, strKey);

    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue("", *command) ) {
            // transform it from '%1' to '%s' style format string
            // @@ we don't make any attempt to verify that the string is valid,
            //    i.e. doesn't contain %2, or second %1 or .... But we do make
            //    sure that we return a string with _exactly_ one '%s'!
            size_t len = command->Len();
            for ( size_t n = 0; n < len; n++ ) {
                if ( command->GetChar(n) == '%' &&
                     (n + 1 < len) && command->GetChar(n + 1) == '1' ) {
                    // replace it with '%s'
                    command->SetChar(n + 1, 's');

                    return TRUE;
                }
            }

            // we didn't find any '%1'!
            // @@@ hack: append the filename at the end, hope that it will do
            *command << " %s";

            return TRUE;
        }
    }

    // no such file type or no value
    return FALSE;
}

// @@ this function is half implemented
bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    if ( m_ext.IsEmpty() ) {
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
    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, m_strFileType);
    if ( key.Open() && key.QueryValue("Content Type", *mimeType) ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

bool wxFileTypeImpl::GetIcon(wxIcon *icon) const
{
    wxString strIconKey;
    strIconKey << m_strFileType << "\\DefaultIcon";

    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, strIconKey);

    if ( key.Open() ) {
        wxString strIcon;
        // it's the default value of the key
        if ( key.QueryValue("", strIcon) ) {
            // the format is the following: <full path to file>, <icon index>
            // NB: icon index may be negative as well as positive and the full
            //     path may contain the environment variables inside '%'
            wxString strFullPath = strIcon.BeforeLast(','),
            strIndex = strIcon.AfterLast(',');

            // index may be omitted, in which case BeforeLast(',') is empty and
            // AfterLast(',') is the whole string
            if ( strFullPath.IsEmpty() ) {
                strFullPath = strIndex;
                strIndex = "0";
            }

            wxString strExpPath = wxExpandEnvVars(strFullPath);
            int nIndex = atoi(strIndex);

            HICON hIcon = ExtractIcon(GetModuleHandle(NULL), strExpPath, nIndex);
            switch ( (int)hIcon ) {
                case 0: // means no icons were found
                case 1: // means no such file or it wasn't a DLL/EXE/OCX/ICO/...
                    wxLogDebug("incorrect registry entry '%s': no such icon.",
                               key.GetName().c_str());
                    break;

                default:
                    icon->SetHICON((WXHICON)hIcon);
                    return TRUE;
            }
        }
    }

    // no such file type or no value or incorrect icon entry
    return FALSE;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxRegKey key(wxRegKey::HKCR, m_strFileType);

    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue("", *desc) ) {
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
    if ( ext[0u] != '.' ) {
        str = '.';
    }
    str << ext;

    // suppress possible error messages
    wxLogNull nolog;

    wxString strFileType;
    wxRegKey key(wxRegKey::HKCR, str);
    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue("", strFileType) ) {
            // create the new wxFileType object
            wxFileType *fileType = new wxFileType;
            fileType->m_impl->SetFileType(strFileType);
            fileType->m_impl->SetExt(ext);

            return fileType;
        }
    }

    // unknown extension
    return NULL;
}

// MIME type -> extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    // @@@ I don't know of any official documentation which mentions this
    //     location, but as a matter of fact IE uses it, so why not we?
    static const char *szMimeDbase = "MIME\\Database\\Content Type\\";

    wxString strKey = szMimeDbase;
    strKey << mimeType;

    // suppress possible error messages
    wxLogNull nolog;

    wxString ext;
    wxRegKey key(wxRegKey::HKCR, strKey);
    if ( key.Open() ) {
        if ( key.QueryValue("Extension", ext) ) {
            return GetFileTypeFromExtension(ext);
        }
    }

    // unknown MIME type
    return NULL;
}

#else  // Unix

MailCapEntry *
wxFileTypeImpl::GetEntry(const wxFileType::MessageParameters& params) const
{
    wxString command;
    MailCapEntry *entry = m_manager->m_aEntries[m_index];
    while ( entry != NULL ) {
        // notice that an empty command would always succeed (@@ is it ok?)
        command = wxFileType::ExpandCommand(entry->GetTestCmd(), params);

        if ( command.IsEmpty() || (system(command) == 0) ) {
            // ok, passed
            wxLogTrace("Test '%s' for mime type '%s' succeeded.",
                       command.c_str(), params.GetMimeType().c_str());
            break;
        }
        else {
            wxLogTrace("Test '%s' for mime type '%s' failed.",
                       command.c_str(), params.GetMimeType().c_str());
        }

        entry = entry->GetNext();
    }

    return entry;
}

bool
wxFileTypeImpl::GetExpandedCommand(wxString *expandedCmd,
                                   const wxFileType::MessageParameters& params,
                                   bool open) const
{
    MailCapEntry *entry = GetEntry(params);
    if ( entry == NULL ) {
        // all tests failed...
        return FALSE;
    }

    wxString cmd = open ? entry->GetOpenCmd() : entry->GetPrintCmd();
    if ( cmd.IsEmpty() ) {
        // may happen, especially for "print"
        return FALSE;
    }

    *expandedCmd = wxFileType::ExpandCommand(cmd, params);
    return TRUE;
}

bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    wxString strExtensions = m_manager->GetExtension(m_index);
    extensions.Empty();

    // one extension in the space or comma delimitid list
    wxString strExt;
    for ( const char *p = strExtensions; ; p++ ) {
        if ( *p == ' ' || *p == ',' || *p == '\0' ) {
            if ( !strExt.IsEmpty() ) {
                extensions.Add(strExt);
                strExt.Empty();
            }
            //else: repeated spaces (shouldn't happen, but it's not that
            //      important if it does happen)

            if ( *p == '\0' )
                break;
        }
        else if ( *p == '.' ) {
            // remove the dot from extension (but only if it's the first char)
            if ( !strExt.IsEmpty() ) {
                strExt += '.';
            }
            //else: no, don't append it
        }
        else {
            strExt += *p;
        }
    }

    return TRUE;
}

// read system and user mailcaps (TODO implement mime.types support)
wxMimeTypesManagerImpl::wxMimeTypesManagerImpl()
{
    // directories where we look for mailcap and mime.types by default
    // (taken from metamail(1) sources)
    static const char *aStandardLocations[] =
    {
        "/etc",
        "/usr/etc",
        "/usr/local/etc",
        "/etc/mail",
        "/usr/public/lib"
    };

    // first read the system wide file(s)
    for ( size_t n = 0; n < WXSIZEOF(aStandardLocations); n++ ) {
        wxString dir = aStandardLocations[n];

        wxString file = dir + "/mailcap";
        if ( wxFile::Exists(file) ) {
            ReadMailcap(file);
        }

        file = dir + "/mime.types";
        if ( wxFile::Exists(file) ) {
            ReadMimeTypes(file);
        }
    }

    wxString strHome = getenv("HOME");

    // and now the users mailcap
    wxString strUserMailcap = strHome + "/.mailcap";
    if ( wxFile::Exists(strUserMailcap) ) {
        ReadMailcap(strUserMailcap);
    }

    // read the users mime.types
    wxString strUserMimeTypes = strHome + "/.mime.types";
    if ( wxFile::Exists(strUserMimeTypes) ) {
        ReadMimeTypes(strUserMimeTypes);
    }
}

wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& ext)
{
    size_t count = m_aExtensions.GetCount();
    for ( size_t n = 0; n < count; n++ ) {
        wxString extensions = m_aExtensions[n];
        while ( !extensions.IsEmpty() ) {
            wxString field = extensions.BeforeFirst(' ');
            extensions = extensions.AfterFirst(' ');

            // consider extensions as not being case-sensitive
            if ( field.IsSameAs(ext, FALSE /* no case */) ) {
                // found
                wxFileType *fileType = new wxFileType;
                fileType->m_impl->Init(this, n);

                return fileType;
            }
        }
    }

    // not found
    return NULL;
}

wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    // mime types are not case-sensitive
    wxString mimetype(mimeType);
    mimetype.MakeLower();

    // first look for an exact match
    int index = m_aTypes.Index(mimetype);
    if ( index == wxNOT_FOUND ) {
        // then try to find "text/*" as match for "text/plain" (for example)
        // NB: if mimeType doesn't contain '/' at all, BeforeFirst() will return
        //     the whole string - ok.
        wxString strCategory = mimetype.BeforeFirst('/');

        size_t nCount = m_aTypes.Count();
        for ( size_t n = 0; n < nCount; n++ ) {
            if ( (m_aTypes[n].BeforeFirst('/') == strCategory ) &&
                 m_aTypes[n].AfterFirst('/') == "*" ) {
                    index = n;
                    break;
            }
        }
    }

    if ( index != wxNOT_FOUND ) {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->Init(this, index);

        return fileType;
    }
    else {
        // not found...
        return NULL;
    }
}

void wxMimeTypesManagerImpl::ReadMimeTypes(const wxString& strFileName)
{
    wxLogTrace("--- Parsing mime.types file '%s' ---", strFileName.c_str());

    wxTextFile file(strFileName);
    if ( !file.Open() )
        return;

    // the information we extract
    wxString strMimeType, strDesc, strExtensions;

    size_t nLineCount = file.GetLineCount();
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ ) {
        // now we're at the start of the line
        const char *pc = file[nLine].c_str();

        // skip whitespace
        while ( isspace(*pc) )
            pc++;

        // comment?
        if ( *pc == '#' )
            continue;

        // detect file format
        const char *pEqualSign = strchr(pc, '=');
        if ( pEqualSign == NULL ) {
            // brief format
            // ------------

            // first field is mime type
            for ( strMimeType.Empty(); !isspace(*pc) && *pc != '\0'; pc++ ) {
                strMimeType += *pc;
            }

            // skip whitespace
            while ( isspace(*pc) )
                pc++;

            // take all the rest of the string
            strExtensions = pc;

            // no description...
            strDesc.Empty();
        }
        else {
            // expanded format
            // ---------------

            // the string on the left of '=' is the field name
            wxString strLHS(pc, pEqualSign - pc);

            // eat whitespace
            for ( pc = pEqualSign + 1; isspace(*pc); pc++ )
                ;

            const char *pEnd;
            if ( *pc == '"' ) {
                // the string is quoted and ends at the matching quote
                pEnd = strchr(++pc, '"');
                if ( pEnd == NULL ) {
                    wxLogWarning(_("Mime.types file %s, line %d: unterminated "
                                   "quoted string."),
                                 strFileName.c_str(), nLine + 1);
                }
            }
            else {
                // unquoted stringends at the first space
                for ( pEnd = pc; !isspace(*pEnd); pEnd++ )
                    ;
            }

            // now we have the RHS (field value)
            wxString strRHS(pc, pEnd - pc);

            // check that it's more or less what we're waiting for, i.e. that
            // only '\' is left on the line
            if ( *pEnd == '"' ) {
                // skip this quote
                pEnd++;
            }

            for ( pc = pEnd; isspace(*pc); pc++ )
                ;

            // only '\\' may be left on the line normally
            bool entryEnded = *pc == '\0';
            if ( !entryEnded && ((*pc != '\\') || (*++pc != '\0')) ) {
                wxLogWarning(_("Mime.types file %s, line %d: extra characters "
                               "after the field value ignored."),
                             strFileName.c_str(), nLine + 1);
            }
            // if there is a trailing backslash entryEnded = FALSE

            // now see what we got
            if ( strLHS == "type" ) {
                strMimeType = strRHS;
            }
            else if ( strLHS == "desc" ) {
                strDesc = strRHS;
            }
            else if ( strLHS == "exts" ) {
                strExtensions = strRHS;
            }
            else {
                wxLogWarning(_("Unknown field in file %s, line %d: '%s'."),
                             strFileName.c_str(), nLine + 1, strLHS.c_str());
            }

            if ( !entryEnded ) {
                // as we don't reset strMimeType, the next line in this entry
                // will be interpreted correctly.
                continue;
            }
        }

        int index = m_aTypes.Index(strMimeType);
        if ( index == wxNOT_FOUND ) {
            // add a new entry
            m_aTypes.Add(strMimeType);
            m_aEntries.Add(NULL);
            m_aExtensions.Add(strExtensions);
            m_aDescriptions.Add(strDesc);
        }
        else {
            // modify an existing one
            if ( !strDesc.IsEmpty() ) {
                m_aDescriptions[index] = strDesc;   // replace old value
            }
            m_aExtensions[index] += strExtensions;
        }
    }

    // check our data integriry
    wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
              m_aTypes.Count() == m_aExtensions.Count() &&
              m_aTypes.Count() == m_aDescriptions.Count() );
}

void wxMimeTypesManagerImpl::ReadMailcap(const wxString& strFileName)
{
    wxLogTrace("--- Parsing mailcap file '%s' ---", strFileName.c_str());

    wxTextFile file(strFileName);
    if ( !file.Open() )
        return;

    // see the comments near the end of function for the reason we need this
    wxArrayInt aEntryIndices;

    size_t nLineCount = file.GetLineCount();
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ ) {
        // now we're at the start of the line
        const char *pc = file[nLine].c_str();

        // skip whitespace
        while ( isspace(*pc) )
            pc++;

        // comment or empty string?
        if ( *pc == '#' || *pc == '\0' )
            continue;

        // no, do parse

        // what field are we currently in? The first 2 are fixed and there may
        // be an arbitrary number of other fields -- currently, we are not
        // interested in any of them, but we should parse them as well...
        enum
        {
            Field_Type,
            Field_OpenCmd,
            Field_Other
        } currentToken = Field_Type;

        // the flags and field values on the current line
        bool needsterminal = FALSE,
             copiousoutput = FALSE;
        wxString strType,
                 strOpenCmd,
                 strPrintCmd,
                 strTest,
                 strDesc,
                 curField; // accumulator
        for ( bool cont = TRUE; cont; pc++ ) {
            switch ( *pc ) {
                case '\\':
                    // interpret the next character literally (notice that
                    // backslash can be used for line continuation)
                    if ( *++pc == '\0' ) {
                        // fetch the next line.

                        // pc currently points to nowhere, but after the next
                        // pc++ in the for line it will point to the beginning
                        // of the next line in the file
                        pc = file[++nLine].c_str() - 1;
                    }
                    else {
                        // just a normal character
                        curField += *pc;
                    }
                    break;

                case '\0':
                    cont = FALSE;   // end of line reached, exit the loop

                    // fall through

                case ';':
                    // store this field and start looking for the next one

                    // trim whitespaces from both sides
                    curField.Trim(TRUE).Trim(FALSE);

                    switch ( currentToken ) {
                        case Field_Type:
                            strType = curField;
                            if ( strType.Find('/') == wxNOT_FOUND ) {
                                // we interpret "type" as "type/*"
                                strType += "/*";
                            }

                            currentToken = Field_OpenCmd;
                            break;

                        case Field_OpenCmd:
                            strOpenCmd = curField;

                            currentToken = Field_Other;
                            break;

                        case Field_Other:
                            {
                                // "good" mailcap entry?
                                bool ok = TRUE;

                                // is this something of the form foo=bar?
                                const char *pEq = strchr(curField, '=');
                                if ( pEq != NULL ) {
                                    wxString lhs = curField.BeforeFirst('='),
                                             rhs = curField.AfterFirst('=');

                                    lhs.Trim(TRUE);     // from right
                                    rhs.Trim(FALSE);    // from left

                                    if ( lhs == "print" )
                                        strPrintCmd = rhs;
                                    else if ( lhs == "test" )
                                        strTest = rhs;
                                    else if ( lhs == "description" ) {
                                        // it might be quoted
                                        if ( rhs[0u] == '"' &&
                                             rhs.Last() == '"' ) {
                                            strDesc = wxString(rhs.c_str() + 1,
                                                               rhs.Len() - 2);
                                        }
                                        else {
                                            strDesc = rhs;
                                        }
                                    }
                                    else if ( lhs == "compose" ||
                                              lhs == "composetyped" ||
                                              lhs == "notes" ||
                                              lhs == "edit" )
                                        ;   // ignore
                                    else
                                        ok = FALSE;

                                }
                                else {
                                    // no, it's a simple flag
                                    // TODO support the flags:
                                    //  1. create an xterm for 'needsterminal'
                                    //  2. append "| $PAGER" for 'copiousoutput'
                                    if ( curField == "needsterminal" )
                                        needsterminal = TRUE;
                                    else if ( curField == "copiousoutput" )
                                        copiousoutput = TRUE;
                                    else if ( curField == "textualnewlines" )
                                        ;   // ignore
                                    else
                                        ok = FALSE;
                                }

                                if ( !ok )
                                {
                                    // don't flood the user with error messages
                                    // if we don't understand something in his
                                    // mailcap, but give them in debug mode
                                    // because this might be useful for the
                                    // programmer
                                    wxLogDebug
                                    (
                                      _("Mailcap file %s, line %d: unknown "
                                        "field '%s' for the MIME type "
                                        "'%s' ignored."),
                                      strFileName.c_str(),
                                      nLine + 1,
                                      curField.c_str(),
                                      strType.c_str()
                                    );
                                }
                            }

                            // it already has this value
                            //currentToken = Field_Other;
                            break;

                        default:
                            wxFAIL_MSG("unknown field type in mailcap");
                    }

                    // next token starts immediately after ';'
                    curField.Empty();
                    break;

                default:
                    curField += *pc;
            }
        }

        // check that we really read something reasonable
        if ( currentToken == Field_Type || currentToken == Field_OpenCmd ) {
            wxLogWarning(_("Mailcap file %s, line %d: incomplete entry "
                           "ignored."),
                         strFileName.c_str(), nLine + 1);
        }
        else {
            MailCapEntry *entry = new MailCapEntry(strOpenCmd,
                                                   strPrintCmd,
                                                   strTest);

            strType.MakeLower();
            int nIndex = m_aTypes.Index(strType);
            if ( nIndex == wxNOT_FOUND ) {
                // new file type
                m_aTypes.Add(strType);

                m_aEntries.Add(entry);
                m_aExtensions.Add("");
                m_aDescriptions.Add(strDesc);
            }
            else {
                // modify the existing entry: the entry in one and the same file
                // are read in top-to-bottom order, i.e. the entries read first
                // should be tried before the entries below. However, the files
                // read later should override the settings in the files read
                // before, thus we Append() the new entry to the list if it has
                // already occured in _this_ file, but Prepend() it if it
                // occured in some of the previous ones.
                if ( aEntryIndices.Index(nIndex) == wxNOT_FOUND ) {
                    // first time in this file
                    aEntryIndices.Add(nIndex);
                    entry->Prepend(m_aEntries[nIndex]);
                    m_aEntries[nIndex] = entry;
                }
                else {
                    // not the first time in _this_ file
                    entry->Append(m_aEntries[nIndex]);
                }

                if ( !strDesc.IsEmpty() ) {
                    // @@ replace the old one - what else can we do??
                    m_aDescriptions[nIndex] = strDesc;
                }
            }
        }

        // check our data integriry
        wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
                  m_aTypes.Count() == m_aExtensions.Count() &&
                  m_aTypes.Count() == m_aDescriptions.Count() );
    }
}

#endif // OS type

#endif
  // __WIN16__
