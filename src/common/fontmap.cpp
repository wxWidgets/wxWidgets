/////////////////////////////////////////////////////////////////////////////
// Name:        common/fontmap.cpp
// Purpose:     wxFontMapper class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.11.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
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

#if wxUSE_FONTMAP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif // PCH

#include "wx/module.h"
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
#if wxUSE_CONFIG
static const wxChar* FONTMAPPER_ROOT_PATH = wxT("/wxWindows/FontMapper");
static const wxChar* FONTMAPPER_CHARSET_PATH = wxT("Charsets");
static const wxChar* FONTMAPPER_CHARSET_ALIAS_PATH = wxT("Aliases");

// we only ask questions in GUI mode
#if wxUSE_GUI
    static const wxChar* FONTMAPPER_FONT_FROM_ENCODING_PATH = wxT("Encodings");
    static const wxChar* FONTMAPPER_FONT_DONT_ASK = wxT("none");
#endif // wxUSE_GUI
#endif // wxUSE_CONFIG

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
    wxFONTENCODING_CP932,
    wxFONTENCODING_CP936,
    wxFONTENCODING_CP949,
    wxFONTENCODING_CP950,
    wxFONTENCODING_CP1250,
    wxFONTENCODING_CP1251,
    wxFONTENCODING_CP1252,
    wxFONTENCODING_CP1253,
    wxFONTENCODING_CP1254,
    wxFONTENCODING_CP1255,
    wxFONTENCODING_CP1256,
    wxFONTENCODING_CP1257,
    wxFONTENCODING_CP437,
    wxFONTENCODING_UTF7,
    wxFONTENCODING_UTF8,
    wxFONTENCODING_EUC_JP,
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
    wxTRANSLATE( "Windows Japanese (CP 932)" ),
    wxTRANSLATE( "Windows Chinese Simplified (CP 936)" ),
    wxTRANSLATE( "Windows Korean (CP 949)" ),
    wxTRANSLATE( "Windows Chinese Traditional (CP 950)" ),
    wxTRANSLATE( "Windows Central European (CP 1250)" ),
    wxTRANSLATE( "Windows Cyrillic (CP 1251)" ),
    wxTRANSLATE( "Windows Western European (CP 1252)" ),
    wxTRANSLATE( "Windows Greek (CP 1253)" ),
    wxTRANSLATE( "Windows Turkish (CP 1254)" ),
    wxTRANSLATE( "Windows Hebrew (CP 1255)" ),
    wxTRANSLATE( "Windows Arabic (CP 1256)" ),
    wxTRANSLATE( "Windows Baltic (CP 1257)" ),
    wxTRANSLATE( "Windows/DOS OEM (CP 437)" ),
    wxTRANSLATE( "Unicode 7 bit (UTF-7)" ),
    wxTRANSLATE( "Unicode 8 bit (UTF-8)" ),
    wxTRANSLATE( "Extended Unix Codepage for Japanese (EUC-JP)" ),
};

// and the internal names (these are not translated on purpose!)
static const wxChar* gs_encodingNames[] =
{
    wxT( "iso-8859-1" ),
    wxT( "iso-8859-2" ),
    wxT( "iso-8859-3" ),
    wxT( "iso-8859-4" ),
    wxT( "iso-8859-5" ),
    wxT( "iso-8859-6" ),
    wxT( "iso-8859-7" ),
    wxT( "iso-8859-8" ),
    wxT( "iso-8859-9" ),
    wxT( "iso-8859-10" ),
    wxT( "iso-8859-11" ),
    wxT( "iso-8859-12" ),
    wxT( "iso-8859-13" ),
    wxT( "iso-8859-14" ),
    wxT( "iso-8859-15" ),
    wxT( "koi8-r" ),
    wxT( "windows-932" ),
    wxT( "windows-936" ),
    wxT( "windows-949" ),
    wxT( "windows-950" ),
    wxT( "windows-1250" ),
    wxT( "windows-1251" ),
    wxT( "windows-1252" ),
    wxT( "windows-1253" ),
    wxT( "windows-1254" ),
    wxT( "windows-1255" ),
    wxT( "windows-1256" ),
    wxT( "windows-1257" ),
    wxT( "windows-437" ),
    wxT( "utf-7" ),
    wxT( "utf-8" ),
    wxT( "euc-jp" ),
};

wxCOMPILE_TIME_ASSERT( WXSIZEOF(gs_encodingDescs) == WXSIZEOF(gs_encodings) &&
                       WXSIZEOF(gs_encodingNames) == WXSIZEOF(gs_encodings),
                       EncodingsArraysNotInSync );

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

    DECLARE_NO_COPY_CLASS(wxFontMapperPathChanger)
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
    m_configIsDummy = FALSE;
#endif // wxUSE_CONFIG

#if wxUSE_GUI
    m_windowParent = NULL;
#endif // wxUSE_GUI
}

wxFontMapper::~wxFontMapper()
{
#if wxUSE_CONFIG
    if ( m_configIsDummy )
        delete m_config;
#endif // wxUSE_CONFIG
}

wxFontMapper *wxFontMapper::sm_instance = NULL;

/*static*/ wxFontMapper *wxFontMapper::Get()
{
    if ( !sm_instance )
        sm_instance = new wxFontMapper;
    return sm_instance;
}

/*static*/ wxFontMapper *wxFontMapper::Set(wxFontMapper *mapper)
{
    wxFontMapper *old = sm_instance;
    sm_instance = mapper;
    return old;
}

class wxFontMapperModule: public wxModule
{
public:
    wxFontMapperModule() : wxModule() {}
    virtual bool OnInit() { return TRUE; }
    virtual void OnExit() { delete wxFontMapper::Set(NULL); }

    DECLARE_DYNAMIC_CLASS(wxFontMapperModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxFontMapperModule, wxModule)

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
            m_configIsDummy = TRUE;
            // VS: we can't call wxConfig::Set(m_config) here because that would
            //     disable automatic wxConfig instance creation if this code was
            //     called before wxApp::OnInit (this happens in wxGTK -- it sets
            //     default wxFont encoding in wxApp::Initialize())
        }
    }

    if ( m_configIsDummy && wxConfig::Get(FALSE) != NULL )
    {
        // VS: in case we created dummy m_config (see above), we want to switch back
        //     to the real one as soon as one becomes available.
        delete m_config;
        m_config = wxConfig::Get(FALSE);
        m_configIsDummy = FALSE;
        // FIXME: ideally, we should add keys from dummy config to the real one now,
        //        but it is a low-priority task because typical wxWin application
        //        either doesn't use wxConfig at all or creates wxConfig object in
        //        wxApp::OnInit(), before any real interaction with the user takes
        //        place...
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
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        return _("Default encoding");
    }

    const size_t count = WXSIZEOF(gs_encodingDescs);

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
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        return _("default");
    }

    const size_t count = WXSIZEOF(gs_encodingNames);

    for ( size_t i = 0; i < count; i++ )
    {
        if ( gs_encodings[i] == encoding )
        {
            return gs_encodingNames[i];
        }
    }

    wxString str;
    str.Printf(_("unknown-%d"), encoding);

    return str;
}

wxFontEncoding wxFontMapper::CharsetToEncoding(const wxString& charset,
                                               bool interactive)
{
    // a special pseudo encoding which means "don't ask me about this charset
    // any more" - we need it to avoid driving the user crazy with asking him
    // time after time about the same charset which he [presumably] doesn't
    // have the fonts fot
    static const int wxFONTENCODING_UNKNOWN = -2;

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
            if ( value == wxFONTENCODING_UNKNOWN )
            {
                // don't try to find it, in particular don't ask the user
                return wxFONTENCODING_SYSTEM;
            }

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
#endif // wxUSE_CONFIG

    // if didn't find it there, try to recognize it ourselves
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        // trim any spaces
        cs.Trim(TRUE);
        cs.Trim(FALSE);

        // discard the optional quotes
        if ( !cs.empty() )
        {
            if ( cs[0u] == _T('"') && cs.Last() == _T('"') )
            {
                cs = wxString(cs.c_str(), cs.length() - 1);
            }
        }

        cs.MakeUpper();

        if ( cs.empty() || cs == _T("US-ASCII") )
        {
            encoding = wxFONTENCODING_DEFAULT;
        }
        else if ( cs == wxT("UTF-7") )
        {
            encoding = wxFONTENCODING_UTF7;
        }
        else if ( cs == wxT("UTF-8") )
        {
            encoding = wxFONTENCODING_UTF8;
        }
        else if ( cs == wxT("GB2312") )
        {
            encoding = wxFONTENCODING_GB2312;
        }
        else if ( cs == wxT("BIG5") )
        {
            encoding = wxFONTENCODING_BIG5;
        }
        else if ( cs == wxT("SJIS") ||
                  cs == wxT("SHIFT_JIS") ||
                  cs == wxT("SHIFT-JIS") )
        {
            encoding = wxFONTENCODING_SHIFT_JIS;
        }
        else if ( cs == wxT("EUC-JP") ||
                  cs == wxT("EUC_JP") )
        {
            encoding = wxFONTENCODING_EUC_JP;
        }
        else if ( cs == wxT("KOI8-R") ||
                  cs == wxT("KOI8-U") ||
                  cs == wxT("KOI8-RU") )
        {
            // although koi8-ru is not strictly speaking the same as koi8-r,
            // they are similar enough to make mapping it to koi8 better than
            // not reckognizing it at all
            encoding = wxFONTENCODING_KOI8;
        }
        else if ( cs.Left(3) == wxT("ISO") )
        {
            // the dash is optional (or, to be exact, it is not, but
            // several brokenmails "forget" it)
            const wxChar *p = cs.c_str() + 3;
            if ( *p == wxT('-') )
                p++;
                
            // printf( "iso %s\n", (const char*) cs.ToAscii() );

            unsigned int value;
            if ( wxSscanf(p, wxT("8859-%u"), &value) == 1 )
            {
                // printf( "value %d\n", (int)value );
   
                // make it 0 based and check that it is strictly positive in
                // the process (no such thing as iso8859-0 encoding)
                if ( (value-- > 0) &&
                     (value < wxFONTENCODING_ISO8859_MAX -
                              wxFONTENCODING_ISO8859_1) )
                {
                    // it's a valid ISO8859 encoding
                    value += wxFONTENCODING_ISO8859_1;
                    encoding = (wxFontEncoding)value;
                }
            }
        }
        else if ( cs.Left(4) == wxT("8859") )
        {
            const wxChar *p = cs.c_str();
            
            unsigned int value;
            if ( wxSscanf(p, wxT("8859-%u"), &value) == 1 )
            {
                // printf( "value %d\n", (int)value );
   
                // make it 0 based and check that it is strictly positive in
                // the process (no such thing as iso8859-0 encoding)
                if ( (value-- > 0) &&
                     (value < wxFONTENCODING_ISO8859_MAX -
                              wxFONTENCODING_ISO8859_1) )
                {
                    // it's a valid ISO8859 encoding
                    value += wxFONTENCODING_ISO8859_1;
                    encoding = (wxFontEncoding)value;
                }
            }
        }
        else // check for Windows charsets
        {
            size_t len;
            if ( cs.Left(7) == wxT("WINDOWS") )
            {
                len = 7;
            }
            else if ( cs.Left(2) == wxT("CP") )
            {
                len = 2;
            }
            else // not a Windows encoding
            {
                len = 0;
            }

            if ( len )
            {
                const wxChar *p = cs.c_str() + len;
                if ( *p == wxT('-') )
                    p++;

                int value;
                if ( wxSscanf(p, wxT("%u"), &value) == 1 )
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

                    switch ( value )
                    {
                        case 932:
                            encoding = wxFONTENCODING_CP932;
                            break;

                        case 936:
                            encoding = wxFONTENCODING_CP936;
                            break;

                        case 949:
                            encoding = wxFONTENCODING_CP949;
                            break;

                        case 950:
                            encoding = wxFONTENCODING_CP950;
                            break;
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
        const size_t count = WXSIZEOF(gs_encodingDescs);

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
        }

#if wxUSE_CONFIG
        // save the result in the config now
        if ( ChangePath(FONTMAPPER_CHARSET_PATH, &pathOld) )
        {
            wxConfigBase *config = GetConfig();

            // remember the alt encoding for this charset - or remember that
            // we don't know it
            long value = n == -1 ? wxFONTENCODING_UNKNOWN : (long)encoding;
            if ( !config->Write(charset, value) )
            {
                wxLogError(_("Failed to remember the encoding for the charset '%s'."), charset.c_str());
            }

            RestorePath(pathOld);
        }
#endif // wxUSE_CONFIG
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
    ReentrancyBlocker(bool& flag) : m_flagOld(flag), m_flag(flag)
        { m_flag = TRUE; }
    ~ReentrancyBlocker() { m_flag = m_flagOld; }

private:
    bool m_flagOld;
    bool& m_flag;
};
#endif // wxUSE_GUI

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
        wxLogFatalError(_("can't load any font, aborting"));

        // wxLogFatalError doesn't return
    }

    wxString configEntry,
             encName = GetEncodingName(encoding);
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

        // this special value means that we don't know of fonts for this
        // encoding but, moreover, have already asked the user as well and he
        // didn't specify any font neither
        if ( fontinfo == FONTMAPPER_FONT_DONT_ASK )
        {
            interactive = FALSE;
        }
        else // use the info entered the last time
        {
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
                    //      (should we erase the outdated value?)
                }
                else
                {
                    wxLogDebug(wxT("corrupted config data: string '%s' is not a valid font encoding info"),
                               fontinfo.c_str());
                }
            }
            //else: there is no information in config about this encoding
        }
    }
#endif // wxUSE_CONFIG

    // now try to map this encoding to a compatible one which we have on this
    // system
    wxFontEncodingArray equiv = wxEncodingConverter::GetAllEquivalents(encoding);
    size_t count = equiv.GetCount();
    bool foundEquivEncoding = FALSE;
    wxFontEncoding equivEncoding = wxFONTENCODING_SYSTEM;
    if ( count )
    {
        for ( size_t i = 0; i < count && !foundEquivEncoding; i++ )
        {
            // don't test for encoding itself, we already know we don't have it
            if ( equiv[i] == encoding )
                continue;

            if ( TestAltEncoding(configEntry, equiv[i], info) )
            {
                equivEncoding = equiv[i];

                foundEquivEncoding = TRUE;
            }
        }
    }

    // ask the user
#if wxUSE_FONTDLG
    if ( interactive )
    {
        wxString title(m_titleDialog);
        if ( !title )
            title << wxTheApp->GetAppName() << _(": unknown encoding");

        // built the message
        wxString encDesc = GetEncodingDescription(encoding),
                 msg;
        if ( foundEquivEncoding )
        {
            // ask the user if he wants to override found alternative encoding
            msg.Printf(_("No font for displaying text in encoding '%s' found,\nbut an alternative encoding '%s' is available.\nDo you want to use this encoding (otherwise you will have to choose another one)?"),
                       encDesc.c_str(), GetEncodingDescription(equivEncoding).c_str());
        }
        else
        {
            msg.Printf(_("No font for displaying text in encoding '%s' found.\nWould you like to select a font to be used for this encoding\n(otherwise the text in this encoding will not be shown correctly)?"),
                       encDesc.c_str());
        }

        // the question is different in 2 cases so the answer has to be
        // interpreted differently as well
        int answer = foundEquivEncoding ? wxNO : wxYES;

        if ( wxMessageBox(msg, title,
                          wxICON_QUESTION | wxYES_NO,
                          m_windowParent) == answer )
        {
            wxFontData data;
            data.SetEncoding(encoding);
            data.EncodingInfo() = *info;
            wxFontDialog dialog(m_windowParent, data);
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
#endif // wxUSE_CONFIG

                return TRUE;
            }
            //else: the user canceled the font selection dialog
        }
        else
        {
            // the user doesn't want to select a font for this encoding
            // or selected to use equivalent encoding
            //
            // remember it to avoid asking the same question again later
#if wxUSE_CONFIG
            if ( ChangePath(FONTMAPPER_FONT_FROM_ENCODING_PATH, &pathOld) )
            {
                GetConfig()->Write
                             (
                                configEntry,
                                foundEquivEncoding ? info->ToString().c_str()
                                                   : FONTMAPPER_FONT_DONT_ASK
                             );

                RestorePath(pathOld);
            }
#endif // wxUSE_CONFIG
        }
    }
    //else: we're in non-interactive mode
#endif // wxUSE_FONTDLG

    return foundEquivEncoding;
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

#endif // wxUSE_FONTMAP
