/////////////////////////////////////////////////////////////////////////////
// Name:        common/fontmap.cpp
// Purpose:     wxFontMapper class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.11.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif // PCH

#include "wx/fontmap.h"
#include "wx/config.h"

#include "wx/msgdlg.h"
#include "wx/fontdlg.h"
#include "wx/choicdlg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the config paths we use
static const char* FONTMAPPER_ROOT_PATH = _T("FontMapper");
static const char* FONTMAPPER_CHARSET_PATH = _T("Charsets");
static const char* FONTMAPPER_CHARSET_ALIAS_PATH = _T("Aliases");
static const char* FONTMAPPER_FONT_FROM_ENCODING_PATH = _T("Encodings");

// encodings supported by GetEncodingDescription
static wxFontEncoding gs_encodings[] =
{
    wxFONTENCODING_ISO8859_1,
    wxFONTENCODING_ISO8859_2,
    wxFONTENCODING_ISO8859_3,
    wxFONTENCODING_ISO8859_4,
    wxFONTENCODING_ISO8859_5,
    wxFONTENCODING_ISO8859_6,
    wxFONTENCODING_ISO8859_7,
    wxFONTENCODING_ISO8859_8,
    wxFONTENCODING_ISO8859_9,
    wxFONTENCODING_ISO8859_10,
    wxFONTENCODING_ISO8859_11,
    wxFONTENCODING_ISO8859_12,
    wxFONTENCODING_ISO8859_13,
    wxFONTENCODING_ISO8859_14,
    wxFONTENCODING_ISO8859_15,
    wxFONTENCODING_KOI8,
    wxFONTENCODING_CP1250,
    wxFONTENCODING_CP1251,
    wxFONTENCODING_CP1252,
    wxFONTENCODING_CP1253,
    wxFONTENCODING_CP1254,
    wxFONTENCODING_CP1255,
    wxFONTENCODING_CP1256,
    wxFONTENCODING_CP1257,
};

// the descriptions for them
static const wxChar* gs_encodingDescs[] =
{
    wxTRANSLATE( "West European (ISO-8859-1/Latin 1)" ),
    wxTRANSLATE( "Central European (ISO-8859-2/Latin 2)" ),
    wxTRANSLATE( "Esperanto (ISO-8859-3)" ),
    wxTRANSLATE( "Baltic (ISO-8859-4)" ),
    wxTRANSLATE( "Cyrillic (Latin 5)" ),
    wxTRANSLATE( "Arabic (ISO-8859-6)" ),
    wxTRANSLATE( "Greek (ISO-8859-7)" ),
    wxTRANSLATE( "Hebrew (ISO-8859-8)" ),
    wxTRANSLATE( "Turkish (ISO-8859-9)" ),
    wxTRANSLATE( "Baltic II (ISO-8859-10)" ),
    wxTRANSLATE( "Thai (ISO-8859-11)" ),
    wxTRANSLATE( "ISO-8859-12" ),
    wxTRANSLATE( "ISO-8859-13" ),
    wxTRANSLATE( "ISO-8859-14" ),
    wxTRANSLATE( "West European new (ISO-8859-15/Latin 0)" ),
    wxTRANSLATE( "KOI8-R" ),
    wxTRANSLATE( "Windows Latin 2 (CP 1250)" ),
    wxTRANSLATE( "Windows Cyrillic (CP 1251)" ),
    wxTRANSLATE( "Windows Latin 1 (CP 1252)" ),
    wxTRANSLATE( "Windows Greek (CP 1253)" ),
    wxTRANSLATE( "Windows Turkish (CP 1254)" ),
    wxTRANSLATE( "Windows Hebrew (CP 1255)" ),
    wxTRANSLATE( "Windows Arabic (CP 1256)" ),
    wxTRANSLATE( "Windows Baltic (CP 1257)" ),
};

// and the internal names
static const wxChar* gs_encodingNames[] =
{
    "iso8859-1",
    "iso8859-2",
    "iso8859-3",
    "iso8859-4",
    "iso8859-5",
    "iso8859-6",
    "iso8859-7",
    "iso8859-8",
    "iso8859-9",
    "iso8859-10",
    "iso8859-11",
    "iso8859-12",
    "iso8859-13",
    "iso8859-14",
    "iso8859-15",
    "koi8-r",
    "windows1250",
    "windows1251",
    "windows1252",
    "windows1253",
    "windows1254",
    "windows1255",
    "windows1256",
    "windows1257",
};

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// private object
static wxFontMapper gs_fontMapper;

// and public pointer
wxFontMapper * WXDLLEXPORT wxTheFontMapper = &gs_fontMapper;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// change the config path during the lifetime of this object
class wxFontMapperPathChanger
{
public:
    wxFontMapperPathChanger(wxFontMapper *fontMapper, const wxString& path)
    {
        m_fontMapper = fontMapper;
        m_ok = m_fontMapper->ChangePath(path, &m_pathOld);
    }

    bool IsOk() const { return m_ok; }

    ~wxFontMapperPathChanger()
    {
        if ( IsOk() )
            m_fontMapper->RestorePath(m_pathOld);
    }

private:
    wxFontMapper *m_fontMapper;
    bool          m_ok;
    wxString      m_pathOld;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor and dtor
// ----------------------------------------------------------------------------

wxFontMapper::wxFontMapper()
{
    m_config = NULL;
    m_windowParent = NULL;
}

wxFontMapper::~wxFontMapper()
{
}

// ----------------------------------------------------------------------------
// customisation
// ----------------------------------------------------------------------------

/* static */ const wxChar *wxFontMapper::GetDefaultConfigPath()
{
    return FONTMAPPER_ROOT_PATH;
}

void wxFontMapper::SetConfigPath(const wxString& prefix)
{
    wxCHECK_RET( !prefix.IsEmpty() && prefix[0] == wxCONFIG_PATH_SEPARATOR,
                 _T("an absolute path should be given to "
                     "wxFontMapper::SetConfigPath()") );

    m_configRootPath = prefix;
}


// ----------------------------------------------------------------------------
// get config object and path for it
// ----------------------------------------------------------------------------

wxConfigBase *wxFontMapper::GetConfig()
{
    if ( !m_config )
    {
        // try the default
        m_config = wxConfig::Get();
    }

    return m_config;
}

const wxString& wxFontMapper::GetConfigPath()
{
    if ( !m_configRootPath )
    {
        // use the default
        m_configRootPath = GetDefaultConfigPath();
    }

    return m_configRootPath;
}

bool wxFontMapper::ChangePath(const wxString& pathNew, wxString *pathOld)
{
    wxConfigBase *config = GetConfig();
    if ( !config )
        return FALSE;

    *pathOld = config->GetPath();

    wxString path = GetConfigPath();
    if ( path.IsEmpty() || path.Last() != wxCONFIG_PATH_SEPARATOR )
    {
        path += wxCONFIG_PATH_SEPARATOR;
    }

    wxASSERT_MSG( !pathNew || (pathNew[0] != wxCONFIG_PATH_SEPARATOR),
                  _T("should be a relative path") );

    path += pathNew;

    config->SetPath(path);

    return TRUE;
}

void wxFontMapper::RestorePath(const wxString& pathOld)
{
    GetConfig()->SetPath(pathOld);
}

// ----------------------------------------------------------------------------
// charset/encoding correspondence
// ----------------------------------------------------------------------------

/* static */
wxString wxFontMapper::GetEncodingDescription(wxFontEncoding encoding)
{
    size_t count = WXSIZEOF(gs_encodingDescs);

    wxASSERT_MSG( count == WXSIZEOF(gs_encodings),
                  _T("inconsitency detected - forgot to update one of "
                     "the arrays?") );

    for ( size_t i = 0; i < count; i++ )
    {
        if ( gs_encodings[i] == encoding )
        {
            return wxGetTranslation(gs_encodingDescs[i]);
        }
    }

    wxString str;
    str.Printf(_("Unknown encoding (%d)"), encoding);

    return str;
}

/* static */
wxString wxFontMapper::GetEncodingName(wxFontEncoding encoding)
{
    size_t count = WXSIZEOF(gs_encodingNames);

    wxASSERT_MSG( count == WXSIZEOF(gs_encodings),
                  _T("inconsitency detected - forgot to update one of "
                     "the arrays?") );

    for ( size_t i = 0; i < count; i++ )
    {
        if ( gs_encodings[i] == encoding )
        {
            return wxGetTranslation(gs_encodingNames[i]);
        }
    }

    wxString str;
    str.Printf(_("unknown-%d"), encoding);

    return str;
}

wxFontEncoding wxFontMapper::CharsetToEncoding(const wxString& charset,
                                               bool interactive)
{
    wxFontEncoding encoding = wxFONTENCODING_SYSTEM;

    // we're going to modify it, make a copy
    wxString cs = charset;

    // first try the user-defined settings
    wxString pathOld;
    if ( ChangePath(FONTMAPPER_CHARSET_PATH, &pathOld) )
    {
        wxConfigBase *config = GetConfig();

        // do we have an encoding for this charset?
        long value = config->Read(charset, -1l);
        if ( value != -1 )
        {
            if ( value >= 0 && value <= wxFONTENCODING_MAX )
            {
                encoding = (wxFontEncoding)value;
            }
            else
            {
                wxLogDebug(_T("corrupted config data - invalid encoding %ld "
                              "for charset '%s'"), value, charset.c_str());
            }
        }

        if ( encoding == wxFONTENCODING_SYSTEM )
        {
            // may be we have an alias?
            config->SetPath(FONTMAPPER_CHARSET_ALIAS_PATH);

            wxString alias = config->Read(charset);
            if ( !!alias )
            {
                // yes, we do - use it instead
                cs = alias;
            }
        }

        RestorePath(pathOld);
    }

    // if didn't find it there, try to reckognise it ourselves
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        cs.MakeUpper();

        if ( !cs || cs == _T("US-ASCII") )
            encoding = wxFONTENCODING_DEFAULT;
        else if ( cs == _T("KOI8-R") || cs == _T("KOI8-U") )
            encoding = wxFONTENCODING_KOI8;
        else if ( cs.Left(3) == _T("ISO") )
        {
            // the dash is optional (or, to be exact, it is not, but
            // several brokenmails "forget" it)
            const wxChar *p = cs.c_str() + 3;
            if ( *p == _T('-') )
                p++;

            unsigned int value;
            if ( wxSscanf(p, _T("8859-%u"), &value) == 1 )
            {
                if ( value < wxFONTENCODING_ISO8859_MAX -
                             wxFONTENCODING_ISO8859_1 )
                {
                    // it's a valid ISO8859 encoding
                    value += wxFONTENCODING_ISO8859_1 - 1;
                    encoding = (wxFontEncoding)value;
                }
            }
        }
        else if ( cs.Left(8) == _T("WINDOWS-") )
        {
            int value;
            if ( wxSscanf(cs.c_str() + 8, "%u", &value) == 1 )
            {
                if ( value >= 1250 )
                {
                    value -= 1250;
                    if ( value < wxFONTENCODING_CP12_MAX -
                                 wxFONTENCODING_CP1250 - 1 )
                    {
                        // a valid Windows code page
                        value += wxFONTENCODING_CP1250;
                        encoding = (wxFontEncoding)value;
                    }
                }
            }
        }
        //else: unknown
    }

    // if still no luck, ask the user - unless disabled
    if ( (encoding == wxFONTENCODING_SYSTEM) && interactive )
    {
        // prepare the dialog data

        // the dialog title
        wxString title(m_titleDialog);
        if ( !title )
            title << wxTheApp->GetAppName() << _(": unknown charset");

        // the message
        wxString msg;
        msg.Printf(_("The charset '%s' is unknown. You may select another "
                     "charset to replace it with or choose [Cancel] if it "
                     "cannot be replaced"), charset.c_str());

        // the list of choices
        size_t count = WXSIZEOF(gs_encodingDescs);

        wxASSERT_MSG( count == WXSIZEOF(gs_encodings),
                      _T("inconsitency detected - forgot to update one of "
                         "the arrays?") );

        wxString *encodingNamesTranslated = new wxString[count];

        for ( size_t i = 0; i < count; i++ )
        {
            encodingNamesTranslated[i] = wxGetTranslation(gs_encodingDescs[i]);
        }

        // the parent window
        wxWindow *parent = m_windowParent;
        if ( !parent )
            parent = wxTheApp->GetTopWindow();

        // do ask the user and get back the index in encodings table
        int n = wxGetSingleChoiceIndex(msg, title,
                                       count,
                                       encodingNamesTranslated,
                                       parent);

        delete [] encodingNamesTranslated;

        if ( n != -1 )
        {
            // TODO save the result in the config!

            encoding = gs_encodings[n];
        }
        //else: cancelled
    }

    return encoding;
}

// ----------------------------------------------------------------------------
// support for unknown encodings: we maintain a map between the
// (platform-specific) strings identifying them and our wxFontEncodings they
// correspond to which is used by GetFontForEncoding() function
// ----------------------------------------------------------------------------

bool wxFontMapper::TestAltEncoding(const wxString& configEntry,
                                   wxFontEncoding encReplacement,
                                   wxNativeEncodingInfo *info)
{
    if ( wxGetNativeFontEncoding(encReplacement, info) &&
         wxTestFontEncoding(*info) )
    {
        // remember the mapping in the config
        wxFontMapperPathChanger path(this, FONTMAPPER_FONT_FROM_ENCODING_PATH);

        if ( path.IsOk() )
        {
            GetConfig()->Write(configEntry, info->ToString());
        }

        return TRUE;
    }

    return FALSE;
}

bool wxFontMapper::GetAltForEncoding(wxFontEncoding encoding,
                                     wxNativeEncodingInfo *info,
                                     bool interactive)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in GetAltForEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    // if we failed to load the system default encoding, something is really
    // wrong and we'd better stop now - otherwise we will go into endless
    // recursion trying to create the font in the msg box with the error
    // message
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        wxFatalError(_("can't load any font, aborting"));

        // wxFatalError doesn't return
    }

    wxString configEntry = GetEncodingName(encoding);

    // do we have a font spec for this encoding?
    wxString pathOld;
    if ( ChangePath(FONTMAPPER_FONT_FROM_ENCODING_PATH, &pathOld) )
    {
        wxConfigBase *config = GetConfig();

        wxString fontinfo = config->Read(configEntry);

        RestorePath(pathOld);

        if ( !!fontinfo )
        {
            if ( info->FromString(fontinfo) )
            {
                if ( wxTestFontEncoding(*info) )
                {
                    // ok, got something
                    return TRUE;
                }
                //else: no such fonts, look for something else
            }
            else
            {
                wxLogDebug(_T("corrupted config data: string '%s' is not "
                              "a valid font encoding info"), fontinfo.c_str());
            }
        }
    }

    // ask the user
    if ( interactive )
    {
        wxString title(m_titleDialog);
        if ( !title )
            title << wxTheApp->GetAppName() << _(": unknown encoding");

        // the message
        wxString msg;
        msg.Printf(_("The encoding '%s' is unknown.\n"
                     "Would you like to select a font to be used for this "
                     "encoding\n"
                     "(otherwise the text in this encoding will not be "
                     "shown correctly)?"),
                     GetEncodingDescription(encoding).c_str());

        wxWindow *parent = m_windowParent;
        if ( !parent )
            parent = wxTheApp->GetTopWindow();

        if ( wxMessageBox(msg, title,
                          wxICON_QUESTION | wxYES_NO, parent) == wxYES )
        {
            wxFontData data;
            data.SetEncoding(encoding);
            data.EncodingInfo() = *info;
            wxFontDialog dialog(parent, &data);
            if ( dialog.ShowModal() == wxID_OK )
            {
                wxFontData retData = dialog.GetFontData();
                wxFont font = retData.GetChosenFont();

                *info = retData.EncodingInfo();

                // remember this in the config
                if ( ChangePath(FONTMAPPER_FONT_FROM_ENCODING_PATH, &pathOld) )
                {
                    GetConfig()->Write(configEntry, info->ToString());

                    RestorePath(pathOld);
                }

                return TRUE;
            }
            //else: the user canceled the font selection dialog
        }
        //else: the user doesn't want to select a font
    }
    //else: we're in non-interactive mode

    // now try the default mappings
    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_15:
            // iso8859-15 is slightly modified iso8859-1
            if ( TestAltEncoding(configEntry, wxFONTENCODING_ISO8859_1, info) )
                return TRUE;
            // fall through

        case wxFONTENCODING_ISO8859_1:
            // iso8859-1 is identical to CP1252
            if ( TestAltEncoding(configEntry, wxFONTENCODING_CP1252, info) )
                return TRUE;

            break;

        case wxFONTENCODING_CP1252:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_ISO8859_1, info) )
                return TRUE;

            break;

        // iso8859-13 is quite similar to WinBaltic
        case wxFONTENCODING_ISO8859_13:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_CP1257, info) )
                return TRUE;

            break;

        case wxFONTENCODING_CP1257:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_ISO8859_13, info) )
                return TRUE;

            break;

        // iso8859-8 is almost identical to WinHebrew
        case wxFONTENCODING_ISO8859_8:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_CP1255, info) )
                return TRUE;

            break;

        case wxFONTENCODING_CP1255:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_ISO8859_8, info) )
                return TRUE;

            break;

        // and iso8859-7 is not too different from WinGreek
        case wxFONTENCODING_ISO8859_7:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_CP1253, info) )
                return TRUE;

            break;

        case wxFONTENCODING_CP1253:
            if ( TestAltEncoding(configEntry, wxFONTENCODING_ISO8859_7, info) )
                return TRUE;

            break;

        default:
           // TODO add other mappings...
           ;
    }

    return FALSE;
}
