/////////////////////////////////////////////////////////////////////////////
// Name:        unix/fontutil.cpp
// Purpose:     Font helper functions for X11 (GDK/X)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // PCH


#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/tokenzr.h"
#include "wx/hash.h"



// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encodingid;registry;encoding[;facename]
bool wxNativeEncodingInfo::FromString(const wxString& s)
{
#if 0 // FIXME_MGL
    wxStringTokenizer tokenizer(s, _T(";"));
            // cannot use "-" because it may be part of encoding name

    wxString encid = tokenizer.GetNextToken();
    long enc;
    if ( !encid.ToLong(&enc) )
        return FALSE;
    encoding = (wxFontEncoding)enc;

    xregistry = tokenizer.GetNextToken();
    if ( !xregistry )
        return FALSE;

    xencoding = tokenizer.GetNextToken();
    if ( !xencoding )
        return FALSE;

    // ok even if empty
    facename = tokenizer.GetNextToken();

    return TRUE;
#endif
}

wxString wxNativeEncodingInfo::ToString() const
{
#if 0 // FIXME_MGL
    wxString s;
    s << (long)encoding << _T(';') << xregistry << _T(';') << xencoding;
    if ( !!facename )
    {
        s << _T(';') << facename;
    }

    return s;
#endif
}

// ----------------------------------------------------------------------------
// common functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
#if 0 // FIXME_MGL
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_ISO8859_3:
        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_ISO8859_6:
        case wxFONTENCODING_ISO8859_7:
        case wxFONTENCODING_ISO8859_8:
        case wxFONTENCODING_ISO8859_9:
        case wxFONTENCODING_ISO8859_10:
        case wxFONTENCODING_ISO8859_11:
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            {
                int cp = encoding - wxFONTENCODING_ISO8859_1 + 1;
                info->xregistry = wxT("iso8859");
                info->xencoding.Printf(wxT("%d"), cp);
            }
            break;

        case wxFONTENCODING_KOI8:
            info->xregistry = wxT("koi8");

            // we don't make distinction between koi8-r and koi8-u (so far)
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
        case wxFONTENCODING_CP1253:
        case wxFONTENCODING_CP1254:
        case wxFONTENCODING_CP1255:
        case wxFONTENCODING_CP1256:
        case wxFONTENCODING_CP1257:
            {
                int cp = encoding - wxFONTENCODING_CP1250 + 1250;
                info->xregistry = wxT("microsoft");
                info->xencoding.Printf(wxT("cp%d"), cp);
            }
            break;

        case wxFONTENCODING_SYSTEM:
            info->xregistry =
            info->xencoding = wxT("*");
            break;

        default:
            // don't know how to translate this encoding into X fontspec
            return FALSE;
    }

   info->encoding = encoding;

    return TRUE;
#endif
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
#if 0 // FIXME_MGL
    wxString fontspec;
    fontspec.Printf(_T("-*-%s-*-*-*-*-*-*-*-*-*-*-%s-%s"),
                    !info.facename ? _T("*") : info.facename.c_str(),
                    info.xregistry.c_str(),
                    info.xencoding.c_str());

    return wxTestFontSpec(fontspec);
#endif
}


