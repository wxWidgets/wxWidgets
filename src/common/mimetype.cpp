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

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if (wxUSE_FILE && wxUSE_TEXTFILE) || defined(__WXMSW__)

#ifndef WX_PRECOMP
  #include "wx/string.h"
  #include "wx/icon.h"
#endif //WX_PRECOMP

// Doesn't compile in WIN16 mode
#ifndef __WIN16__

#include "wx/log.h"
#include "wx/file.h"
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

// although I don't know of any official documentation which mentions this
// location, uses it, so it isn't likely to change
static const wxChar *MIME_DATABASE_KEY = wxT("MIME\\Database\\Content Type\\");

class wxFileTypeImpl
{
public:
    // ctor
    wxFileTypeImpl() { m_info = NULL; }

    // one of these Init() function must be called (ctor can't take any
    // arguments because it's common)

        // initialize us with our file type name and extension - in this case
        // we will read all other data from the registry
    void Init(const wxString& strFileType, const wxString& ext)
        { m_strFileType = strFileType; m_ext = ext; }

        // initialize us with a wxFileTypeInfo object - it contains all the
        // data
    void Init(const wxFileTypeInfo& info)
        { m_info = &info; }

    // implement accessor functions
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const;
    bool GetIcon(wxIcon *icon) const;
    bool GetDescription(wxString *desc) const;
    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const;
    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const;

private:
    // helper function: reads the command corresponding to the specified verb
    // from the registry (returns an empty string if not found)
    wxString GetCommand(const wxChar *verb) const;

    // we use either m_info or read the data from the registry if m_info == NULL
    const wxFileTypeInfo *m_info;
    wxString        m_strFileType,
                    m_ext;
};

WX_DECLARE_EXPORTED_OBJARRAY(wxFileTypeInfo, wxArrayFileTypeInfo);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayFileTypeInfo);

class wxMimeTypesManagerImpl
{
public:
    // nothing to do here, we don't load any data but just go and fetch it from
    // the registry when asked for
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxFileType **filetypes);

    // this are NOPs under Windows
    bool ReadMailcap(const wxString& filename, bool fallback = TRUE)
        { return TRUE; }
    bool ReadMimeTypes(const wxString& filename)
        { return TRUE; }

    void AddFallback(const wxFileTypeInfo& ft) { m_fallbacks.Add(ft); }

private:
    wxArrayFileTypeInfo m_fallbacks;
};

#elif defined( __WXMAC__ )

WX_DECLARE_EXPORTED_OBJARRAY(wxFileTypeInfo, wxArrayFileTypeInfo);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayFileTypeInfo);

class wxMimeTypesManagerImpl
{
public :
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxFileType **filetypes);

    // this are NOPs under MacOS
    bool ReadMailcap(const wxString& filename, bool fallback = TRUE) { return TRUE; }
    bool ReadMimeTypes(const wxString& filename) { return TRUE; }

    void AddFallback(const wxFileTypeInfo& ft) { m_fallbacks.Add(ft); }

private:
    wxArrayFileTypeInfo m_fallbacks;
};

class wxFileTypeImpl
{
public:
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
        // insert into the list at given position
    void Insert(MailCapEntry *next, size_t pos)
    {
        // FIXME slooow...
        MailCapEntry *cur;
        size_t n = 0;
        for ( cur = next; cur != NULL; cur = cur->m_next, n++ ) {
            if ( n == pos )
                break;
        }

        wxASSERT_MSG( n == pos, wxT("invalid position in MailCapEntry::Insert") );

        m_next = cur->m_next;
        cur->m_next = this;
    }
        // append this element to the list
    void Append(MailCapEntry *next)
    {
        wxCHECK_RET( next != NULL, wxT("Append()ing to what?") );

        // FIXME slooow...
        MailCapEntry *cur;
        for ( cur = next; cur->m_next != NULL; cur = cur->m_next )
            ;

        cur->m_next = this;

        wxASSERT_MSG( !m_next, wxT("Append()ing element already in the list?") );
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
    // TODO it would be nice to load them all, but parse on demand only...
    wxMimeTypesManagerImpl();

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxFileType **filetypes);

    bool ReadMailcap(const wxString& filename, bool fallback = FALSE);
    bool ReadMimeTypes(const wxString& filename);

    void AddFallback(const wxFileTypeInfo& filetype);

    // add information about the given mimetype
    void AddMimeTypeInfo(const wxString& mimetype,
                         const wxString& extensions,
                         const wxString& description);
    void AddMailcapInfo(const wxString& strType,
                        const wxString& strOpenCmd,
                        const wxString& strPrintCmd,
                        const wxString& strTest,
                        const wxString& strDesc);

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
    bool GetIcon(wxIcon * WXUNUSED(icon)) const
        { return FALSE; }   // TODO maybe with Gnome/KDE integration...
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
                            wxLogWarning(_("Unmatched '{' in an entry for "
                                           "mime type %s."),
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
    // the program will accept the data on stdin: so give it to it!
    if ( !hasFilename && !str.IsEmpty() ) {
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

bool wxMimeTypesManager::ReadMailcap(const wxString& filename, bool fallback)
{
    return m_impl->ReadMailcap(filename, fallback);
}

bool wxMimeTypesManager::ReadMimeTypes(const wxString& filename)
{
    return m_impl->ReadMimeTypes(filename);
}

void wxMimeTypesManager::AddFallbacks(const wxFileTypeInfo *filetypes)
{
    for ( const wxFileTypeInfo *ft = filetypes; ft->IsValid(); ft++ ) {
        m_impl->AddFallback(*ft);
    }
}

size_t wxMimeTypesManager::EnumAllFileTypes(wxFileType **filetypes)
{
    wxCHECK_MSG( filetypes, 0u, _T("bad pointer in EnumAllFileTypes") );

    return m_impl->EnumAllFileTypes(filetypes);
}

// ============================================================================
// real (OS specific) implementation
// ============================================================================

#ifdef __WXMSW__

wxString wxFileTypeImpl::GetCommand(const wxChar *verb) const
{
    // suppress possible error messages
    wxLogNull nolog;
    wxString strKey;
    strKey << m_strFileType << wxT("\\shell\\") << verb << wxT("\\command");
    wxRegKey key(wxRegKey::HKCR, strKey);

    wxString command;
    if ( key.Open() ) {
        // it's the default value of the key
        if ( key.QueryValue(wxT(""), command) ) {
            // transform it from '%1' to '%s' style format string

            // NB: we don't make any attempt to verify that the string is valid,
            //     i.e. doesn't contain %2, or second %1 or .... But we do make
            //     sure that we return a string with _exactly_ one '%s'!
            bool foundFilename = FALSE;
            size_t len = command.Len();
            for ( size_t n = 0; (n < len) && !foundFilename; n++ ) {
                if ( command[n] == wxT('%') &&
                     (n + 1 < len) && command[n + 1] == wxT('1') ) {
                    // replace it with '%s'
                    command[n + 1] = wxT('s');

                    foundFilename = TRUE;
                }
            }

            if ( !foundFilename ) {
                // we didn't find any '%1'!
                // HACK: append the filename at the end, hope that it will do
                command << wxT(" %s");
            }
        }
    }

    // no such file type or no value
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
    wxRegKey key(wxRegKey::HKCR, /*m_strFileType*/ wxT(".") + m_ext);
    if ( key.Open() && key.QueryValue(wxT("Content Type"), *mimeType) ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
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

    // unknown extension
    return NULL;
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

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxFileType **filetypes)
{
    // enumerate all keys under MIME_DATABASE_KEY
    wxRegKey key(wxRegKey::HKCR, MIME_DATABASE_KEY);

    return 0;
}

#elif defined ( __WXMAC__ )

bool wxFileTypeImpl::GetCommand(wxString *command, const char *verb) const
{
    return FALSE;
}

// @@ this function is half implemented
bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    return FALSE;
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    if ( m_strFileType.Length() > 0 )
    {
        *mimeType = m_strFileType ;
        return TRUE ;
    }
    else
    return FALSE;
}

bool wxFileTypeImpl::GetIcon(wxIcon *icon) const
{
    // no such file type or no value or incorrect icon entry
    return FALSE;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    return FALSE;
}

// extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& e)
{
    wxString ext = e ;
    ext = ext.Lower() ;
    if ( ext == "txt" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("text/text");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "htm" || ext == "html" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("text/html");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "gif" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/gif");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "png" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/png");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "jpg" || ext == "jpeg" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/jpeg");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "bmp" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/bmp");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "tif" || ext == "tiff" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/tiff");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "xpm" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/xpm");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "xbm" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/xbm");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }

    // unknown extension
    return NULL;
}

// MIME type -> extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    return NULL;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxFileType **filetypes)
{
    wxFAIL_MSG( _T("TODO") ); // VZ: don't know anything about this for Mac

    return 0;
}

#else  // Unix

MailCapEntry *
wxFileTypeImpl::GetEntry(const wxFileType::MessageParameters& params) const
{
    wxString command;
    MailCapEntry *entry = m_manager->m_aEntries[m_index];
    while ( entry != NULL ) {
        // notice that an empty command would always succeed (it's ok)
        command = wxFileType::ExpandCommand(entry->GetTestCmd(), params);

        if ( command.IsEmpty() || (wxSystem(command) == 0) ) {
            // ok, passed
            wxLogTrace(wxT("Test '%s' for mime type '%s' succeeded."),
                       command.c_str(), params.GetMimeType().c_str());
            break;
        }
        else {
            wxLogTrace(wxT("Test '%s' for mime type '%s' failed."),
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
    for ( const wxChar *p = strExtensions; ; p++ ) {
        if ( *p == wxT(' ') || *p == wxT(',') || *p == wxT('\0') ) {
            if ( !strExt.IsEmpty() ) {
                extensions.Add(strExt);
                strExt.Empty();
            }
            //else: repeated spaces (shouldn't happen, but it's not that
            //      important if it does happen)

            if ( *p == wxT('\0') )
                break;
        }
        else if ( *p == wxT('.') ) {
            // remove the dot from extension (but only if it's the first char)
            if ( !strExt.IsEmpty() ) {
                strExt += wxT('.');
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
    static const wxChar *aStandardLocations[] =
    {
        wxT("/etc"),
        wxT("/usr/etc"),
        wxT("/usr/local/etc"),
        wxT("/etc/mail"),
        wxT("/usr/public/lib")
    };

    // first read the system wide file(s)
    for ( size_t n = 0; n < WXSIZEOF(aStandardLocations); n++ ) {
        wxString dir = aStandardLocations[n];

        wxString file = dir + wxT("/mailcap");
        if ( wxFile::Exists(file) ) {
            ReadMailcap(file);
        }

        file = dir + wxT("/mime.types");
        if ( wxFile::Exists(file) ) {
            ReadMimeTypes(file);
        }
    }

    wxString strHome = wxGetenv(wxT("HOME"));

    // and now the users mailcap
    wxString strUserMailcap = strHome + wxT("/.mailcap");
    if ( wxFile::Exists(strUserMailcap) ) {
        ReadMailcap(strUserMailcap);
    }

    // read the users mime.types
    wxString strUserMimeTypes = strHome + wxT("/.mime.types");
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
            wxString field = extensions.BeforeFirst(wxT(' '));
            extensions = extensions.AfterFirst(wxT(' '));

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
        wxString strCategory = mimetype.BeforeFirst(wxT('/'));

        size_t nCount = m_aTypes.Count();
        for ( size_t n = 0; n < nCount; n++ ) {
            if ( (m_aTypes[n].BeforeFirst(wxT('/')) == strCategory ) &&
                 m_aTypes[n].AfterFirst(wxT('/')) == wxT("*") ) {
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

void wxMimeTypesManagerImpl::AddFallback(const wxFileTypeInfo& filetype)
{
    wxString extensions;
    const wxArrayString& exts = filetype.GetExtensions();
    size_t nExts = exts.GetCount();
    for ( size_t nExt = 0; nExt < nExts; nExt++ ) {
        if ( nExt > 0 ) {
            extensions += wxT(' ');
        }
        extensions += exts[nExt];
    }

    AddMimeTypeInfo(filetype.GetMimeType(),
                    extensions,
                    filetype.GetDescription());

    AddMailcapInfo(filetype.GetMimeType(),
                   filetype.GetOpenCommand(),
                   filetype.GetPrintCommand(),
                   wxT(""),
                   filetype.GetDescription());
}

void wxMimeTypesManagerImpl::AddMimeTypeInfo(const wxString& strMimeType,
                                             const wxString& strExtensions,
                                             const wxString& strDesc)
{
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
        m_aExtensions[index] += ' ' + strExtensions;
    }
}

void wxMimeTypesManagerImpl::AddMailcapInfo(const wxString& strType,
                                            const wxString& strOpenCmd,
                                            const wxString& strPrintCmd,
                                            const wxString& strTest,
                                            const wxString& strDesc)
{
    MailCapEntry *entry = new MailCapEntry(strOpenCmd, strPrintCmd, strTest);

    int nIndex = m_aTypes.Index(strType);
    if ( nIndex == wxNOT_FOUND ) {
        // new file type
        m_aTypes.Add(strType);

        m_aEntries.Add(entry);
        m_aExtensions.Add(wxT(""));
        m_aDescriptions.Add(strDesc);
    }
    else {
        // always append the entry in the tail of the list - info added with
        // this function can only come from AddFallbacks()
        MailCapEntry *entryOld = m_aEntries[nIndex];
        if ( entryOld )
            entry->Append(entryOld);
        else
            m_aEntries[nIndex] = entry;
    }
}

bool wxMimeTypesManagerImpl::ReadMimeTypes(const wxString& strFileName)
{
    wxLogTrace(wxT("--- Parsing mime.types file '%s' ---"), strFileName.c_str());

    wxTextFile file(strFileName);
    if ( !file.Open() )
        return FALSE;

    // the information we extract
    wxString strMimeType, strDesc, strExtensions;

    size_t nLineCount = file.GetLineCount();
    const wxChar *pc = NULL;
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ ) {
        if ( pc == NULL ) {
            // now we're at the start of the line
            pc = file[nLine].c_str();
        }
        else {
            // we didn't finish with the previous line yet
            nLine--;
        }

        // skip whitespace
        while ( wxIsspace(*pc) )
            pc++;

        // comment?
        if ( *pc == wxT('#') ) {
            // skip the whole line
            pc = NULL;
            continue;
        }

        // detect file format
        const wxChar *pEqualSign = wxStrchr(pc, wxT('='));
        if ( pEqualSign == NULL ) {
            // brief format
            // ------------

            // first field is mime type
            for ( strMimeType.Empty(); !wxIsspace(*pc) && *pc != wxT('\0'); pc++ ) {
                strMimeType += *pc;
            }

            // skip whitespace
            while ( wxIsspace(*pc) )
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
            for ( pc = pEqualSign + 1; wxIsspace(*pc); pc++ )
                ;

            const wxChar *pEnd;
            if ( *pc == wxT('"') ) {
                // the string is quoted and ends at the matching quote
                pEnd = wxStrchr(++pc, wxT('"'));
                if ( pEnd == NULL ) {
                    wxLogWarning(_("Mime.types file %s, line %d: unterminated "
                                   "quoted string."),
                                 strFileName.c_str(), nLine + 1);
                }
            }
            else {
                // unquoted string ends at the first space
                for ( pEnd = pc; !wxIsspace(*pEnd); pEnd++ )
                    ;
            }

            // now we have the RHS (field value)
            wxString strRHS(pc, pEnd - pc);

            // check what follows this entry
            if ( *pEnd == wxT('"') ) {
                // skip this quote
                pEnd++;
            }

            for ( pc = pEnd; wxIsspace(*pc); pc++ )
                ;

            // if there is something left, it may be either a '\\' to continue
            // the line or the next field of the same entry
            bool entryEnded = *pc == wxT('\0'),
                 nextFieldOnSameLine = FALSE;
            if ( !entryEnded ) {
                nextFieldOnSameLine = ((*pc != wxT('\\')) || (pc[1] != wxT('\0')));
            }

            // now see what we got
            if ( strLHS == wxT("type") ) {
                strMimeType = strRHS;
            }
            else if ( strLHS == wxT("desc") ) {
                strDesc = strRHS;
            }
            else if ( strLHS == wxT("exts") ) {
                strExtensions = strRHS;
            }
            else {
                wxLogWarning(_("Unknown field in file %s, line %d: '%s'."),
                             strFileName.c_str(), nLine + 1, strLHS.c_str());
            }

            if ( !entryEnded ) {
                if ( !nextFieldOnSameLine )
                    pc = NULL;
                //else: don't reset it

                // as we don't reset strMimeType, the next field in this entry
                // will be interpreted correctly.

                continue;
            }
        }

        // although it doesn't seem to be covered by RFCs, some programs
        // (notably Netscape) create their entries with several comma
        // separated extensions (RFC mention the spaces only)
        strExtensions.Replace(wxT(","), wxT(" "));

        // also deal with the leading dot
        if ( !strExtensions.IsEmpty() && strExtensions[0u] == wxT('.') )
        {
            strExtensions.erase(0, 1);
        }

        AddMimeTypeInfo(strMimeType, strExtensions, strDesc);

        // finished with this line
        pc = NULL;
    }

    // check our data integriry
    wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
              m_aTypes.Count() == m_aExtensions.Count() &&
              m_aTypes.Count() == m_aDescriptions.Count() );

    return TRUE;
}

bool wxMimeTypesManagerImpl::ReadMailcap(const wxString& strFileName,
                                         bool fallback)
{
    wxLogTrace(wxT("--- Parsing mailcap file '%s' ---"), strFileName.c_str());

    wxTextFile file(strFileName);
    if ( !file.Open() )
        return FALSE;

    // see the comments near the end of function for the reason we need these
    // variables (search for the next occurence of them)
        // indices of MIME types (in m_aTypes) we already found in this file
    wxArrayInt aEntryIndices;
        // aLastIndices[n] is the index of last element in
        // m_aEntries[aEntryIndices[n]] from this file
    wxArrayInt aLastIndices;

    size_t nLineCount = file.GetLineCount();
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ ) {
        // now we're at the start of the line
        const wxChar *pc = file[nLine].c_str();

        // skip whitespace
        while ( wxIsspace(*pc) )
            pc++;

        // comment or empty string?
        if ( *pc == wxT('#') || *pc == wxT('\0') )
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
                case wxT('\\'):
                    // interpret the next character literally (notice that
                    // backslash can be used for line continuation)
                    if ( *++pc == wxT('\0') ) {
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

                case wxT('\0'):
                    cont = FALSE;   // end of line reached, exit the loop

                    // fall through

                case wxT(';'):
                    // store this field and start looking for the next one

                    // trim whitespaces from both sides
                    curField.Trim(TRUE).Trim(FALSE);

                    switch ( currentToken ) {
                        case Field_Type:
                            strType = curField;
                            if ( strType.Find(wxT('/')) == wxNOT_FOUND ) {
                                // we interpret "type" as "type/*"
                                strType += wxT("/*");
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
                                const wxChar *pEq = wxStrchr(curField, wxT('='));
                                if ( pEq != NULL ) {
                                    wxString lhs = curField.BeforeFirst(wxT('=')),
                                             rhs = curField.AfterFirst(wxT('='));

                                    lhs.Trim(TRUE);     // from right
                                    rhs.Trim(FALSE);    // from left

                                    if ( lhs == wxT("print") )
                                        strPrintCmd = rhs;
                                    else if ( lhs == wxT("test") )
                                        strTest = rhs;
                                    else if ( lhs == wxT("description") ) {
                                        // it might be quoted
                                        if ( rhs[0u] == wxT('"') &&
                                             rhs.Last() == wxT('"') ) {
                                            strDesc = wxString(rhs.c_str() + 1,
                                                               rhs.Len() - 2);
                                        }
                                        else {
                                            strDesc = rhs;
                                        }
                                    }
                                    else if ( lhs == wxT("compose") ||
                                              lhs == wxT("composetyped") ||
                                              lhs == wxT("notes") ||
                                              lhs == wxT("edit") )
                                        ;   // ignore
                                    else
                                        ok = FALSE;

                                }
                                else {
                                    // no, it's a simple flag
                                    // TODO support the flags:
                                    //  1. create an xterm for 'needsterminal'
                                    //  2. append "| $PAGER" for 'copiousoutput'
                                    if ( curField == wxT("needsterminal") )
                                        needsterminal = TRUE;
                                    else if ( curField == wxT("copiousoutput") )
                                        copiousoutput = TRUE;
                                    else if ( curField == wxT("textualnewlines") )
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
                                      wxT("Mailcap file %s, line %d: unknown "
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
                            wxFAIL_MSG(wxT("unknown field type in mailcap"));
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

            // NB: because of complications below (we must get entries priority
            //     right), we can't use AddMailcapInfo() here, unfortunately.
            strType.MakeLower();
            int nIndex = m_aTypes.Index(strType);
            if ( nIndex == wxNOT_FOUND ) {
                // new file type
                m_aTypes.Add(strType);

                m_aEntries.Add(entry);
                m_aExtensions.Add(wxT(""));
                m_aDescriptions.Add(strDesc);
            }
            else {
                // modify the existing entry: the entries in one and the same
                // file are read in top-to-bottom order, i.e. the entries read
                // first should be tried before the entries below. However,
                // the files read later should override the settings in the
                // files read before (except if fallback is TRUE), thus we
                // Insert() the new entry to the list if it has already
                // occured in _this_ file, but Prepend() it if it occured in
                // some of the previous ones and Append() to it in the
                // fallback case

                if ( fallback ) {
                    // 'fallback' parameter prevents the entries from this
                    // file from overriding the other ones - always append
                    MailCapEntry *entryOld = m_aEntries[nIndex];
                    if ( entryOld )
                        entry->Append(entryOld);
                    else
                        m_aEntries[nIndex] = entry;
                }
                else {
                    int entryIndex = aEntryIndices.Index(nIndex);
                    if ( entryIndex == wxNOT_FOUND ) {
                        // first time in this file
                        aEntryIndices.Add(nIndex);
                        aLastIndices.Add(0);

                        entry->Prepend(m_aEntries[nIndex]);
                        m_aEntries[nIndex] = entry;
                    }
                    else {
                        // not the first time in _this_ file
                        size_t nEntryIndex = (size_t)entryIndex;
                        MailCapEntry *entryOld = m_aEntries[nIndex];
                        if ( entryOld )
                            entry->Insert(entryOld, aLastIndices[nEntryIndex]);
                        else
                            m_aEntries[nIndex] = entry;

                        // the indices were shifted by 1
                        aLastIndices[nEntryIndex]++;
                    }
                }

                if ( !strDesc.IsEmpty() ) {
                    // replace the old one - what else can we do??
                    m_aDescriptions[nIndex] = strDesc;
                }
            }
        }

        // check our data integriry
        wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
                  m_aTypes.Count() == m_aExtensions.Count() &&
                  m_aTypes.Count() == m_aDescriptions.Count() );
    }

    return TRUE;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxFileType **filetypes)
{
    size_t count = m_aTypes.GetCount();

    *filetypes = new wxFileType *[count];
    for ( size_t n = 0; n < count; n++ )
    {
        (*filetypes)[n] = new wxFileType;
        (*filetypes)[n]->m_impl->Init(this, n);
    }

    return count;
}

#endif
  // OS type

#endif
  // wxUSE_FILE && wxUSE_TEXTFILE

#endif
  // __WIN16__
