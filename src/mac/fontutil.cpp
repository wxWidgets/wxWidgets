///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontutil.cpp
// Purpose:     font-related helper functions for wxMSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/fontutil.h"
#include "wx/fontmap.h"

#include "wx/tokenzr.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      facename[;charset]

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    facename = tokenizer.GetNextToken();
    if ( !facename )
        return FALSE;

    wxString tmp = tokenizer.GetNextToken();
    if ( !tmp )
    {
        // default charset (don't use DEFAULT_CHARSET though because of subtle
        // Windows 9x/NT differences in handling it)
        charset = 0;
    }
    else
    {
        if ( wxSscanf(tmp, _T("%u"), &charset) != 1 )
        {
            // should be a number!
            return FALSE;
        }
    }

    return TRUE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s(facename);
    if ( charset != 0 )
    {
        s << _T(';') << charset;
    }

    return s;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        // although this function is supposed to return an exact match, do do
        // some mappings here for the most common case of "standard" encoding
        case wxFONTENCODING_SYSTEM:
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_15:
        case wxFONTENCODING_CP1252:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1250:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1251:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1253:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1254:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1255:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1256:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP1257:
            info->charset = 0;
            break;

        case wxFONTENCODING_CP437:
             info->charset = 0;
           break;

        default:
            // no way to translate this encoding into a Windows charset
            return FALSE;
    }

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    return TRUE;
}
