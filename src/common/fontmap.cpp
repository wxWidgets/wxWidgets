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
#include "wx/choicdlg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the config paths we use
static const char* FONTMAPPER_ROOT_PATH = _T("FontMapper");
static const char* FONTMAPPER_CHARSET_PATH = _T("Charsets");
static const char* FONTMAPPER_CHARSET_ALIAS_PATH = _T("Aliases");

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
        static wxFontEncoding encodings[] =
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

        static const wxChar* encodingNames[] =
        {
            "West European (ISO-8859-1/Latin 1)",
            "Central European (ISO-8859-2/Latin 2)",
            "Esperanto (ISO-8859-3)",
            "Baltic (ISO-8859-4)",
            "Cyrillic (Latin 5)",
            "Arabic (ISO-8859-6)"
            "Greek (ISO-8859-7)",
            "Hebrew (ISO-8859-8)",
            "Turkish (ISO-8859-9)",
            "Baltic II (ISO-8859-10)",
            "Thai (ISO-8859-11)",
            "ISO-8859-12",
            "ISO-8859-13",
            "ISO-8859-14",
            "West European new (ISO-8859-15/Latin 0)",
            "KOI8-R",
            "Windows Latin 2 (CP 1250)",
            "Windows Cyrillic (CP 1251)",
            "Windows Latin 1 (CP 1252)",
            "Windows Greek (CP 1253)",
            "Windows Turkish (CP 1254)",
            "Windows Hebrew (CP 1255)",
            "Windows Arabic (CP 1256)",
            "Windows Baltic (CP 1257)",
        };

        size_t count = WXSIZEOF(encodingNames);

        wxASSERT_MSG( count == WXSIZEOF(encodings),
                      _T("inconsitency detected - forgot to update one of "
                         "the arrays?") );

        wxString *encodingNamesTranslated = new wxString[count];

        for ( size_t n = 0; n < count; n++ )
        {
            encodingNamesTranslated[n] = wxGetTranslation(encodingNames[n]);
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
            encoding = encodings[n];
        }
        //else: cancelled
    }

    return encoding;
}

