///////////////////////////////////////////////////////////////////////////////
// Name:        common/fmapbase.cpp
// Purpose:     wxFontMapperBase class implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.06.2003 (extracted from common/fontmap.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 1999-2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_FONTMAP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#if defined(__WXMSW__)
  #include  "wx/msw/private.h"  // includes windows.h for LOGFONT
  #include  "wx/msw/winundef.h"
#endif

#include "wx/fontmap.h"
#include "wx/fmappriv.h"

#include "wx/apptrait.h"
#include "wx/module.h"

// wxMemoryConfig uses wxFileConfig
#if wxUSE_CONFIG && wxUSE_FILECONFIG
    #include "wx/config.h"
    #include "wx/memconf.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

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
    wxFONTENCODING_KOI8_U,
    wxFONTENCODING_CP874,
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
    wxFONTENCODING_UTF16,
    wxFONTENCODING_UTF16BE,
    wxFONTENCODING_UTF16LE,
    wxFONTENCODING_UTF32,
    wxFONTENCODING_UTF32BE,
    wxFONTENCODING_UTF32LE,
    wxFONTENCODING_EUC_JP,
    wxFONTENCODING_DEFAULT,
    wxFONTENCODING_BIG5,
    wxFONTENCODING_SHIFT_JIS,
    wxFONTENCODING_GB2312,
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
    wxTRANSLATE( "KOI8-U" ),
    wxTRANSLATE( "Windows Thai (CP 874)" ),
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
    wxTRANSLATE( "Unicode 16 bit (UTF-16)" ),
    wxTRANSLATE( "Unicode 16 bit Big Endian (UTF-16BE)" ),
    wxTRANSLATE( "Unicode 16 bit Little Endian (UTF-16LE)" ),
    wxTRANSLATE( "Unicode 32 bit (UTF-32)" ),
    wxTRANSLATE( "Unicode 32 bit Big Endian (UTF-32BE)" ),
    wxTRANSLATE( "Unicode 32 bit Little Endian (UTF-32LE)" ),
    wxTRANSLATE( "Extended Unix Codepage for Japanese (EUC-JP)" ),
    wxTRANSLATE( "US-ASCII" ),
    wxTRANSLATE( "BIG5" ),
    wxTRANSLATE( "SHIFT-JIS" ),
    wxTRANSLATE( "GB-2312" ),
};

// and the internal names (these are not translated on purpose!)
static const wxChar* gs_encodingNames[WXSIZEOF(gs_encodingDescs)][10] =
{
    // names from the first column are used under Linux, from the second one
    // under Solaris and IRIX and from the third one -- under AIX
    { wxT( "ISO-8859-1" ), wxT( "ISO8859-1" ), wxT( "8859-1" ), wxT("iso_8859_1"), NULL },
    { wxT( "ISO-8859-2" ), wxT( "ISO8859-2" ), wxT( "8859-2" ), NULL },
    { wxT( "ISO-8859-3" ), wxT( "ISO8859-3" ), wxT( "8859-3" ), NULL },
    { wxT( "ISO-8859-4" ), wxT( "ISO8859-4" ), wxT( "8859-4" ), NULL },
    { wxT( "ISO-8859-5" ), wxT( "ISO8859-5" ), wxT( "8859-5" ), NULL },
    { wxT( "ISO-8859-6" ), wxT( "ISO8859-6" ), wxT( "8859-6" ), NULL },
    { wxT( "ISO-8859-7" ), wxT( "ISO8859-7" ), wxT( "8859-7" ), NULL },
    { wxT( "ISO-8859-8" ), wxT( "ISO8859-8" ), wxT( "8859-8" ), NULL },
    { wxT( "ISO-8859-9" ), wxT( "ISO8859-9" ), wxT( "8859-9" ), NULL },
    { wxT( "ISO-8859-10" ), wxT( "ISO8859-10" ), wxT( "8859-10" ), NULL },
    { wxT( "ISO-8859-11" ), wxT( "ISO8859-11" ), wxT( "8859-11" ), NULL },
    { wxT( "ISO-8859-12" ), wxT( "ISO8859-12" ), wxT( "8859-12" ), NULL },
    { wxT( "ISO-8859-13" ), wxT( "ISO8859-13" ), wxT( "8859-13" ), NULL },
    { wxT( "ISO-8859-14" ), wxT( "ISO8859-14" ), wxT( "8859-14" ), NULL },
    { wxT( "ISO-8859-15" ), wxT( "ISO8859-15" ), wxT( "8859-15" ), NULL },

    // although koi8-ru is not strictly speaking the same as koi8-r,
    // they are similar enough to make mapping it to koi8 better than
    // not recognizing it at all
    { wxT( "KOI8-R" ), wxT( "KOI8-RU" ), NULL },
    { wxT( "KOI8-U" ), NULL },

    { wxT( "WINDOWS-874" ), wxT( "CP-874" ), NULL },
    { wxT( "WINDOWS-932" ), wxT( "CP-932" ), NULL },
    { wxT( "WINDOWS-936" ), wxT( "CP-936" ), NULL },
    { wxT( "WINDOWS-949" ), wxT( "CP-949" ), wxT( "EUC-KR" ), wxT( "eucKR" ), wxT( "euc_kr" ), NULL },
    { wxT( "WINDOWS-950" ), wxT( "CP-950" ), NULL },
    { wxT( "WINDOWS-1250" ),wxT( "CP-1250" ), NULL },
    { wxT( "WINDOWS-1251" ),wxT( "CP-1251" ), NULL },
    { wxT( "WINDOWS-1252" ),wxT( "CP-1252" ), wxT("IBM-1252"), NULL },
    { wxT( "WINDOWS-1253" ),wxT( "CP-1253" ), NULL },
    { wxT( "WINDOWS-1254" ),wxT( "CP-1254" ), NULL },
    { wxT( "WINDOWS-1255" ),wxT( "CP-1255" ), NULL },
    { wxT( "WINDOWS-1256" ),wxT( "CP-1256" ), NULL },
    { wxT( "WINDOWS-1257" ),wxT( "CP-1257" ), NULL },
    { wxT( "WINDOWS-437" ), wxT( "CP-437" ), NULL },

    { wxT( "UTF-7" ), NULL },
    { wxT( "UTF-8" ), NULL },
    { wxT( "UTF-16" ), NULL },
    { wxT( "UTF-16BE" ), NULL },
    { wxT( "UTF-16LE" ), NULL },
    { wxT( "UTF-32" ), wxT( "UCS-4" ), NULL },
    { wxT( "UTF-32BE" ), wxT( "UCS-4BE" ), NULL },
    { wxT( "UTF-32LE" ), wxT( "UCS-4LE" ), NULL },

    { wxT( "EUC-JP" ), wxT( "eucJP" ), wxT( "euc_jp" ), wxT( "IBM-eucJP" ), NULL },

    // 646 is for Solaris, roman8 -- for HP-UX
    { wxT( "US-ASCII" ), wxT( "ASCII" ), wxT("C"), wxT("POSIX"), wxT("ANSI_X3.4-1968"),
      wxT("646"), wxT("roman8"), wxT( "" ), NULL },

    { wxT( "BIG5" ), NULL },
    { wxT( "shift-jis" ), wxT( "shift_jis" ), wxT( "sjis" ), NULL },
    { wxT( "GB2312" ), NULL },
};

wxCOMPILE_TIME_ASSERT( WXSIZEOF(gs_encodingDescs) == WXSIZEOF(gs_encodings), EncodingsArraysNotInSync );
wxCOMPILE_TIME_ASSERT( WXSIZEOF(gs_encodingNames) == WXSIZEOF(gs_encodings), EncodingsArraysNotInSync );

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// clean up the font mapper object
class wxFontMapperModule : public wxModule
{
public:
    wxFontMapperModule() : wxModule() { }
    virtual bool OnInit() { return true; }
    virtual void OnExit() { delete (wxFontMapperBase*)wxFontMapperBase::Set(NULL); }

    DECLARE_DYNAMIC_CLASS(wxFontMapperModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxFontMapperModule, wxModule)


// ============================================================================
// wxFontMapperBase implementation
// ============================================================================

wxFontMapper *wxFontMapperBase::sm_instance = NULL;

// ----------------------------------------------------------------------------
// ctor and dtor
// ----------------------------------------------------------------------------

wxFontMapperBase::wxFontMapperBase()
{
#if wxUSE_CONFIG && wxUSE_FILECONFIG
    m_config = NULL;
    m_configIsDummy = false;
#endif // wxUSE_CONFIG
}

wxFontMapperBase::~wxFontMapperBase()
{
#if wxUSE_CONFIG && wxUSE_FILECONFIG
    if ( m_configIsDummy )
        delete m_config;
#endif // wxUSE_CONFIG
}

bool wxFontMapperBase::IsWxFontMapper()
{   return false; }

/* static */
wxFontMapperBase *wxFontMapperBase::Get()
{
    if ( !sm_instance )
    {
        wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
        if ( traits )
        {
            sm_instance = traits->CreateFontMapper();

            wxASSERT_MSG( sm_instance,
                            _T("wxAppTraits::CreateFontMapper() failed") );
        }

        if ( !sm_instance )
        {
            // last resort: we must create something because the existing code
            // relies on always having a valid font mapper object
            sm_instance = (wxFontMapper *)new wxFontMapperBase;
        }
    }

    return (wxFontMapperBase*)sm_instance;
}

/* static */
wxFontMapper *wxFontMapperBase::Set(wxFontMapper *mapper)
{
    wxFontMapper *old = sm_instance;
    sm_instance = mapper;
    return old;
}

#if wxUSE_CONFIG && wxUSE_FILECONFIG

// ----------------------------------------------------------------------------
// config usage customisation
// ----------------------------------------------------------------------------

/* static */
const wxChar *wxFontMapperBase::GetDefaultConfigPath()
{
    return FONTMAPPER_ROOT_PATH;
}

void wxFontMapperBase::SetConfigPath(const wxString& prefix)
{
    wxCHECK_RET( !prefix.IsEmpty() && prefix[0] == wxCONFIG_PATH_SEPARATOR,
                 wxT("an absolute path should be given to wxFontMapper::SetConfigPath()") );

    m_configRootPath = prefix;
}

// ----------------------------------------------------------------------------
// get config object and path for it
// ----------------------------------------------------------------------------

wxConfigBase *wxFontMapperBase::GetConfig()
{
    if ( !m_config )
    {
        // try the default
        m_config = wxConfig::Get(false /*don't create on demand*/ );

        if ( !m_config )
        {
            // we still want to have a config object because otherwise we would
            // keep asking the user the same questions in the interactive mode,
            // so create a dummy config which won't write to any files/registry
            // but will allow us to remember the results of the questions at
            // least during this run
            m_config = new wxMemoryConfig;
            m_configIsDummy = true;
            // VS: we can't call wxConfig::Set(m_config) here because that would
            //     disable automatic wxConfig instance creation if this code was
            //     called before wxApp::OnInit (this happens in wxGTK -- it sets
            //     default wxFont encoding in wxApp::Initialize())
        }
    }

    if ( m_configIsDummy && wxConfig::Get(false) != NULL )
    {
        // VS: in case we created dummy m_config (see above), we want to switch back
        //     to the real one as soon as one becomes available.
        delete m_config;
        m_config = wxConfig::Get(false);
        m_configIsDummy = false;
        // FIXME: ideally, we should add keys from dummy config to the real one now,
        //        but it is a low-priority task because typical wxWin application
        //        either doesn't use wxConfig at all or creates wxConfig object in
        //        wxApp::OnInit(), before any real interaction with the user takes
        //        place...
    }
    // KH: Always update the config object to the current one (if one exists), otherwise
    // if a new config object is created, we continue to use the old one (which was
    // almost certainly deleted).
    // Caching of the config object was removed in 2.7, but added and removed
    // member variables. This solution is convoluted but does not change any headers.
    else if ( wxConfig::Get(false) != NULL )
    {
        m_config = wxConfig::Get();
    }

    return m_config;
}

const wxString& wxFontMapperBase::GetConfigPath()
{
    if ( !m_configRootPath )
    {
        // use the default
        m_configRootPath = GetDefaultConfigPath();
    }

    return m_configRootPath;
}

// ----------------------------------------------------------------------------
// config helpers
// ----------------------------------------------------------------------------

bool wxFontMapperBase::ChangePath(const wxString& pathNew, wxString *pathOld)
{
    wxConfigBase *config = GetConfig();
    if ( !config )
        return false;

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

    return true;
}

void wxFontMapperBase::RestorePath(const wxString& pathOld)
{
    GetConfig()->SetPath(pathOld);
}

#endif

// ----------------------------------------------------------------------------
// charset/encoding correspondence
// ----------------------------------------------------------------------------

wxFontEncoding
wxFontMapperBase::CharsetToEncoding(const wxString& charset,
                                    bool WXUNUSED(interactive))
{
    int enc = NonInteractiveCharsetToEncoding(charset);
    if ( enc == wxFONTENCODING_UNKNOWN )
    {
        // we should return wxFONTENCODING_SYSTEM from here for unknown
        // encodings
        enc = wxFONTENCODING_SYSTEM;
    }

    return (wxFontEncoding)enc;
}

int
wxFontMapperBase::NonInteractiveCharsetToEncoding(const wxString& charset)
{
    wxFontEncoding encoding = wxFONTENCODING_SYSTEM;

    // we're going to modify it, make a copy
    wxString cs = charset;

#if wxUSE_CONFIG && wxUSE_FILECONFIG
    // first try the user-defined settings
    wxFontMapperPathChanger path(this, FONTMAPPER_CHARSET_PATH);
    if ( path.IsOk() )
    {
        wxConfigBase *config = GetConfig();

        // do we have an encoding for this charset?
        long value = config->Read(charset, -1l);
        if ( value != -1 )
        {
            if ( value == wxFONTENCODING_UNKNOWN )
            {
                // don't try to find it, in particular don't ask the user
                return value;
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
            if ( !alias.IsEmpty() )
            {
                // yes, we do - use it instead
                cs = alias;
            }
        }
    }
#endif // wxUSE_CONFIG

    // if didn't find it there, try to recognize it ourselves
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        // trim any spaces
        cs.Trim(true);
        cs.Trim(false);

        // discard the optional quotes
        if ( !cs.empty() )
        {
            if ( cs[0u] == _T('"') && cs.Last() == _T('"') )
            {
                cs = wxString(cs.c_str(), cs.length() - 1);
            }
        }

        for ( size_t i = 0; i < WXSIZEOF(gs_encodingNames); ++i )
        {
            for ( const wxChar** encName = gs_encodingNames[i]; *encName; ++encName )
            {
                if ( cs.CmpNoCase(*encName) == 0 )
                    return gs_encodings[i];
            }
        }

        cs.MakeUpper();

        if ( cs.Left(3) == wxT("ISO") )
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
                        case 874:
                            encoding = wxFONTENCODING_CP874;
                            break;

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

    return encoding;
}

/* static */
size_t wxFontMapperBase::GetSupportedEncodingsCount()
{
    return WXSIZEOF(gs_encodings);
}

/* static */
wxFontEncoding wxFontMapperBase::GetEncoding(size_t n)
{
    wxCHECK_MSG( n < WXSIZEOF(gs_encodings), wxFONTENCODING_SYSTEM,
                    _T("wxFontMapper::GetEncoding(): invalid index") );

    return gs_encodings[n];
}

/* static */
wxString wxFontMapperBase::GetEncodingDescription(wxFontEncoding encoding)
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
wxString wxFontMapperBase::GetEncodingName(wxFontEncoding encoding)
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
            return gs_encodingNames[i][0];
        }
    }

    wxString str;
    str.Printf(_("unknown-%d"), encoding);

    return str;
}

/* static */
const wxChar** wxFontMapperBase::GetAllEncodingNames(wxFontEncoding encoding)
{
    static const wxChar* dummy[] = { NULL };

    for ( size_t i = 0; i < WXSIZEOF(gs_encodingNames); i++ )
    {
        if ( gs_encodings[i] == encoding )
        {
            return gs_encodingNames[i];
        }
    }

    return dummy;
}

/* static */
wxFontEncoding wxFontMapperBase::GetEncodingFromName(const wxString& name)
{
    const size_t count = WXSIZEOF(gs_encodingNames);

    for ( size_t i = 0; i < count; i++ )
    {
        for ( const wxChar** encName = gs_encodingNames[i]; *encName; ++encName )
        {
            if ( name.CmpNoCase(*encName) == 0 )
                return gs_encodings[i];
        }
    }

    return wxFONTENCODING_MAX;
}

#endif // wxUSE_FONTMAP

