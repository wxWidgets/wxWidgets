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

#if wxUSE_CONFIG
    #include "wx/config.h"
    #include "wx/memconf.h"
#endif

#if wxUSE_GUI
    #include "wx/fontutil.h"
    #include "wx/msgdlg.h"
    #include "wx/fontdlg.h"
    #include "wx/choicdlg.h"
#endif // wxUSE_GUI

#include "wx/encconv.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the config paths we use
static const wxChar* FONTMAPPER_ROOT_PATH = wxT("/wxWindows/FontMapper");
static const wxChar* FONTMAPPER_CHARSET_PATH = wxT("Charsets");
static const wxChar* FONTMAPPER_CHARSET_ALIAS_PATH = wxT("Aliases");
#if wxUSE_GUI
    static const wxChar* FONTMAPPER_FONT_FROM_ENCODING_PATH = wxT("Encodings");
#endif // wxUSE_GUI

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
    wxFONTENCODING_CP437,
};

// the descriptions for them
static const wxChar* gs_encodingDescs[] =
{
    wxTRANSLATE( "Western European (ISO-8859-1)" ),
    wxTRANSLATE( "Central European (ISO-8859-2)" ),
    wxTRANSLATE( "Esperanto (ISO-8859-3)" ),
    wxTRANSLATE( "Baltic (old) (ISO-8859-4)" ),
    wxTRANSLATE( "Cyrillic (ISO-8859-5)" ),
    wxTRANSLATE( "Arabic (ISO-8859-6)" ),
    wxTRANSLATE( "Greek (ISO-8859-7)" ),
    wxTRANSLATE( "Hebrew (ISO-8859-8)" ),
    wxTRANSLATE( "Turkish (ISO-8859-9)" ),
    wxTRANSLATE( "Nordic (ISO-8859-10)" ),
    wxTRANSLATE( "Thai (ISO-8859-11)" ),
    wxTRANSLATE( "Indian (ISO-8859-12)" ),
    wxTRANSLATE( "Baltic (ISO-8859-13)" ),
    wxTRANSLATE( "Celtic (ISO-8859-14)" ),
    wxTRANSLATE( "Western European with Euro (ISO-8859-15)" ),
    wxTRANSLATE( "KOI8-R" ),
    wxTRANSLATE( "Windows Central European (CP 1250)" ),
    wxTRANSLATE( "Windows Cyrillic (CP 1251)" ),
    wxTRANSLATE( "Windows Western European (CP 1252)" ),
    wxTRANSLATE( "Windows Greek (CP 1253)" ),
    wxTRANSLATE( "Windows Turkish (CP 1254)" ),
    wxTRANSLATE( "Windows Hebrew (CP 1255)" ),
    wxTRANSLATE( "Windows Arabic (CP 1256)" ),
    wxTRANSLATE( "Windows Baltic (CP 1257)" ),
    wxTRANSLATE( "Windows/DOS OEM (CP 437)" ),
};

// and the internal names
static const wxChar* gs_encodingNames[] =
{
    wxT( "iso8859-1" ),
    wxT( "iso8859-2" ),
    wxT( "iso8859-3" ),
    wxT( "iso8859-4" ),
    wxT( "iso8859-5" ),
    wxT( "iso8859-6" ),
    wxT( "iso8859-7" ),
    wxT( "iso8859-8" ),
    wxT( "iso8859-9" ),
    wxT( "iso8859-10" ),
    wxT( "iso8859-11" ),
    wxT( "iso8859-12" ),
    wxT( "iso8859-13" ),
    wxT( "iso8859-14" ),
    wxT( "iso8859-15" ),
    wxT( "koi8-r" ),
    wxT( "windows1250" ),
    wxT( "windows1251" ),
    wxT( "windows1252" ),
    wxT( "windows1253" ),
    wxT( "windows1254" ),
    wxT( "windows1255" ),
    wxT( "windows1256" ),
    wxT( "windows1257" ),
    wxT( "windows437" ),
};

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// private object
static wxFontMapper gs_fontMapper;

// and public pointer
wxFontMapper * wxTheFontMapper = &gs_fontMapper;

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
#if wxUSE_CONFIG
    m_config = NULL;
#endif // wxUSE_CONFIG

#if wxUSE_GUI
    m_windowParent = NULL;
#endif // wxUSE_GUI
}

wxFontMapper::~wxFontMapper()
{
}

// ----------------------------------------------------------------------------
// customisation
// ----------------------------------------------------------------------------

#if wxUSE_CONFIG

/* static */ const wxChar *wxFontMapper::GetDefaultConfigPath()
{
    return FONTMAPPER_ROOT_PATH;
}

void wxFontMapper::SetConfigPath(const wxString& prefix)
{
    wxCHECK_RET( !prefix.IsEmpty() && prefix[0] == wxCONFIG_PATH_SEPARATOR,
                 wxT("an absolute path should be given to wxFontMapper::SetConfigPath()") );

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
        m_config = wxConfig::Get(FALSE /*don't create on demand*/ );

        if ( !m_config )
        {
            // we still want to have a config object because otherwise we would
            // keep asking the user the same questions in the interactive mode,
            // so create a dummy config which won't write to any files/registry
            // but will allow us to remember the results of the questions at
            // least during this run
            m_config = new wxMemoryConfig;
            wxConfig::Set(m_config);
        }
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
#endif

bool wxFontMapper::ChangePath(const wxString& pathNew, wxString *pathOld)
{
#if wxUSE_CONFIG
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
                  wxT("should be a relative path") );

    path += pathNew;

    config->SetPath(path);

    return TRUE;
#else
    return FALSE;
#endif
}

void wxFontMapper::RestorePath(const wxString& pathOld)
{
#if wxUSE_CONFIG
    GetConfig()->SetPath(pathOld);
#else
#endif
}

// ----------------------------------------------------------------------------
// charset/encoding correspondence
// ----------------------------------------------------------------------------

/* static */
wxString wxFontMapper::GetEncodingDescription(wxFontEncoding encoding)
{
    size_t count = WXSIZEOF(gs_encodingDescs);

    wxASSERT_MSG( count == WXSIZEOF(gs_encodings),
                  wxT("inconsitency detected - forgot to update one of the arrays?") );

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
                  wxT("inconsistency detected - forgot to update one of the arrays?") );

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

#if wxUSE_CONFIG
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
                wxLogDebug(wxT("corrupted config data: invalid encoding %ld for charset '%s' ignored"),
                           value, charset.c_str());
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
#endif

    // if didn't find it there, try to reckognise it ourselves
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        // discard the optional quotes
        if ( !!cs )
        {
            if ( cs[0u] == _T('"') && cs.Last() == _T('"') )
            {
                cs = wxString(cs.c_str(), cs.length() - 1);
            }
        }

        cs.MakeUpper();

        if ( !cs || cs == wxT("US-ASCII") )
            encoding = wxFONTENCODING_DEFAULT;
        else if ( cs == wxT("KOI8-R") || cs == wxT("KOI8-U") )
            encoding = wxFONTENCODING_KOI8;
        else if ( cs.Left(3) == wxT("ISO") )
        {
            // the dash is optional (or, to be exact, it is not, but
            // several brokenmails "forget" it)
            const wxChar *p = cs.c_str() + 3;
            if ( *p == wxT('-') )
                p++;

            unsigned int value;
            if ( wxSscanf(p, wxT("8859-%u"), &value) == 1 )
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
        else if ( cs.Left(8) == wxT("WINDOWS-") )
        {
            int value;
            if ( wxSscanf(cs.c_str() + 8, wxT("%u"), &value) == 1 )
            {
                if ( value >= 1250 )
                {
                    value -= 1250;
                    if ( value < wxFONTENCODING_CP12_MAX -
                                 wxFONTENCODING_CP1250 )
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

#if wxUSE_GUI
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
        msg.Printf(_("The charset '%s' is unknown. You may select\nanother charset to replace it with or choose\n[Cancel] if it cannot be replaced"), charset.c_str());

        // the list of choices
        size_t count = WXSIZEOF(gs_encodingDescs);

        wxASSERT_MSG( count == WXSIZEOF(gs_encodings),
                      wxT("inconsitency detected - forgot to update one of the arrays?") );

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
            encoding = gs_encodings[n];

#if wxUSE_CONFIG
        // save the result in the config now
            if ( ChangePath(FONTMAPPER_CHARSET_PATH, &pathOld) )
            {
                wxConfigBase *config = GetConfig();

                // remember the alt encoding for this charset
                if ( !config->Write(charset, (long)encoding) )
                {
                    wxLogError(_("Failed to remember the encoding for the charset '%s'."), charset.c_str());
                }

                RestorePath(pathOld);
            }
#endif // wxUSE_CONFIG
        }
        //else: cancelled
    }
#endif // wxUSE_GUI

    return encoding;
}

// ----------------------------------------------------------------------------
// support for unknown encodings: we maintain a map between the
// (platform-specific) strings identifying them and our wxFontEncodings they
// correspond to which is used by GetFontForEncoding() function
// ----------------------------------------------------------------------------

#if wxUSE_GUI

bool wxFontMapper::TestAltEncoding(const wxString& configEntry,
                                   wxFontEncoding encReplacement,
                                   wxNativeEncodingInfo *info)
{
    if ( wxGetNativeFontEncoding(encReplacement, info) &&
         wxTestFontEncoding(*info) )
    {
#if wxUSE_CONFIG
        // remember the mapping in the config
        wxFontMapperPathChanger path(this, FONTMAPPER_FONT_FROM_ENCODING_PATH);

        if ( path.IsOk() )
        {
            GetConfig()->Write(configEntry, info->ToString());
        }
#endif // wxUSE_CONFIG
        return TRUE;
    }

    return FALSE;
}

#if wxUSE_GUI
class ReentrancyBlocker
{
public:
    ReentrancyBlocker(bool& b) : m_b(b) { m_b = TRUE; }
    ~ReentrancyBlocker() { m_b = FALSE; }

private:
    bool& m_b;
};
#endif

bool wxFontMapper::GetAltForEncoding(wxFontEncoding encoding,
                                     wxNativeEncodingInfo *info,
                                     const wxString& facename,
                                     bool interactive)
{
#if wxUSE_GUI
    // we need a flag to prevent infinite recursion which happens, for
    // example, when GetAltForEncoding() is called from an OnPaint() handler:
    // in this case, wxYield() which is called from wxMessageBox() we use here
    // will lead to another call of OnPaint() and hence to another call of
    // GetAltForEncoding() - and it is impossible to catch this from the user
    // code because we are called from wxFont ctor implicitly.

    // assume we're always called from the main thread, so that it is safe to
    // use a static var
    static bool s_inGetAltForEncoding = FALSE;

    if ( interactive && s_inGetAltForEncoding )
        return FALSE;

    ReentrancyBlocker blocker(s_inGetAltForEncoding);
#endif // wxUSE_GUI

    wxCHECK_MSG( info, FALSE, wxT("bad pointer in GetAltForEncoding") );

    info->facename = facename;

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

    wxString configEntry, encName = GetEncodingName(encoding);
    if ( !!facename )
    {
        configEntry = facename + _T("_");
    }
    configEntry += encName;

#if wxUSE_CONFIG
    // do we have a font spec for this encoding?
    wxString pathOld;
    if ( ChangePath(FONTMAPPER_FONT_FROM_ENCODING_PATH, &pathOld) )
    {
        wxConfigBase *config = GetConfig();

        wxString fontinfo = config->Read(configEntry);

        RestorePath(pathOld);

        if ( !!fontinfo && !!facename )
        {
            // we tried to find a match with facename - now try without it
            fontinfo = config->Read(encName);
        }

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
                wxLogDebug(wxT("corrupted config data: string '%s' is not a valid font encoding info"), fontinfo.c_str());
            }
        }
        //else: there is no information in config about this encoding
    }
#endif // wxUSE_CONFIG

    // ask the user
    if ( interactive )
    {
        wxString title(m_titleDialog);
        if ( !title )
            title << wxTheApp->GetAppName() << _(": unknown encoding");

        // the message
        wxString msg;
        msg.Printf(_("The encoding '%s' is unknown.\nWould you like to select a font to be used for this encoding\n(otherwise the text in this encoding will not be shown correctly)?"),
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
                info->encoding = retData.GetEncoding();

#if wxUSE_CONFIG
                // remember this in the config
                if ( ChangePath(FONTMAPPER_FONT_FROM_ENCODING_PATH, &pathOld) )
                {
                    GetConfig()->Write(configEntry, info->ToString());

                    RestorePath(pathOld);
                }
#endif

                return TRUE;
            }
            //else: the user canceled the font selection dialog
        }
        //else: the user doesn't want to select a font
    }
    //else: we're in non-interactive mode

    // now try the default mappings:
    wxFontEncodingArray equiv = wxEncodingConverter::GetAllEquivalents(encoding);
    size_t count = equiv.GetCount();
    if ( count )
    {
        for ( size_t i = (equiv[0] == encoding) ? 1 : 0; i < count; i++ )
        {
            if ( TestAltEncoding(configEntry, equiv[i], info) )
                return TRUE;
        }
    }

    return FALSE;
}

bool wxFontMapper::GetAltForEncoding(wxFontEncoding encoding,
                                     wxFontEncoding *alt_encoding,
                                     const wxString& facename,
                                     bool interactive)
{
    wxNativeEncodingInfo info;
    bool r = GetAltForEncoding(encoding, &info, facename, interactive);
    *alt_encoding = info.encoding;
    return r;
}

bool wxFontMapper::IsEncodingAvailable(wxFontEncoding encoding,
                                       const wxString& facename)
{
    wxNativeEncodingInfo info;

    if (wxGetNativeFontEncoding(encoding, &info))
    {
        info.facename = facename;
        return wxTestFontEncoding(info);
    }

    return FALSE;
}

#endif // wxUSE_GUI
