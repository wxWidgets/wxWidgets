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

#include "wx/msw/private.h"

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
//      encodingid;facename[;charset]

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    wxString encid = tokenizer.GetNextToken();
    long enc;
    if ( !encid.ToLong(&enc) )
        return FALSE;
    encoding = (wxFontEncoding)enc;

    facename = tokenizer.GetNextToken();
    if ( !facename )
        return FALSE;

    wxString tmp = tokenizer.GetNextToken();
    if ( !tmp )
    {
        // default charset (don't use DEFAULT_CHARSET though because of subtle
        // Windows 9x/NT differences in handling it)
        charset = ANSI_CHARSET;
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
    wxString s;
    
    s << (long)encoding << _T(';') << facename;
    if ( charset != ANSI_CHARSET )
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
            info->charset = ANSI_CHARSET;
            break;

#if !defined(__WIN16__)
        case wxFONTENCODING_CP1250:
            info->charset = EASTEUROPE_CHARSET;
            break;

        case wxFONTENCODING_CP1251:
            info->charset = RUSSIAN_CHARSET;
            break;

        case wxFONTENCODING_CP1253:
            info->charset = GREEK_CHARSET;
            break;

        case wxFONTENCODING_CP1254:
            info->charset = TURKISH_CHARSET;
            break;

        case wxFONTENCODING_CP1255:
            info->charset = HEBREW_CHARSET;
            break;

        case wxFONTENCODING_CP1256:
            info->charset = ARABIC_CHARSET;
            break;

        case wxFONTENCODING_CP1257:
            info->charset = BALTIC_CHARSET;
            break;

        case wxFONTENCODING_CP874:
            info->charset = THAI_CHARSET;
            break;
#endif // !Win16

        case wxFONTENCODING_CP437:
            info->charset = OEM_CHARSET;
            break;

        default:
            // no way to translate this encoding into a Windows charset
            return FALSE;
    }

    info->encoding = encoding;
   
    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    // try to create such font
    LOGFONT lf;
    wxZeroMemory(lf);       // all default values

    lf.lfCharSet = info.charset;
    wxStrncpy(lf.lfFaceName, info.facename, sizeof(lf.lfFaceName));

    HFONT hfont = ::CreateFontIndirect(&lf);
    if ( !hfont )
    {
        // no such font
        return FALSE;
    }

    ::DeleteObject((HGDIOBJ)hfont);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFont <-> LOGFONT conversion
// ----------------------------------------------------------------------------

void wxFillLogFont(LOGFONT *logFont, const wxFont *font)
{
    int ff_family;
    wxString ff_face;

    switch ( font->GetFamily() )
    {
        case wxSCRIPT:
            ff_family = FF_SCRIPT;
            ff_face = _T("Script");
            break;

        case wxDECORATIVE:
            ff_family = FF_DECORATIVE;
            break;

        case wxROMAN:
            ff_family = FF_ROMAN;
            ff_face = _T("Times New Roman");
            break;

        case wxTELETYPE:
        case wxMODERN:
            ff_family = FF_MODERN;
            ff_face = _T("Courier New");
            break;

        case wxSWISS:
            ff_family = FF_SWISS;
            ff_face = _T("Arial");
            break;

        case wxDEFAULT:
        default:
            ff_family = FF_SWISS;
            ff_face = _T("MS Sans Serif");
    }

    BYTE ff_italic;
    switch ( font->GetStyle() )
    {
        case wxITALIC:
        case wxSLANT:
            ff_italic = 1;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            ff_italic = 0;
    }

    int ff_weight;
    switch ( font->GetWeight() )
    {
        default:
            wxFAIL_MSG(_T("unknown font weight"));
            // fall through

        case wxNORMAL:
            ff_weight = FW_NORMAL;
            break;

        case wxLIGHT:
            ff_weight = FW_LIGHT;
            break;

        case wxBOLD:
            ff_weight = FW_BOLD;
            break;
    }

#if 0
    HDC dc = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc, LOGPIXELSY);
    ::ReleaseDC(NULL, dc);
#else
    // New behaviour: apparently ppInch varies according to Large/Small Fonts
    // setting in Windows. This messes up fonts. So, set ppInch to a constant
    // 96 dpi.
    static const int ppInch = 96;
#endif // 0/1

#if wxFONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (font->GetPointSize()*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (font->GetPointSize()*ppInch/72);
#endif

    wxString facename = font->GetFaceName();
    if ( !!facename )
    {
        ff_face = facename;
    }
    //else: ff_face is a reasonable default facename for this font family

    // deal with encoding now
    wxNativeEncodingInfo info;
    wxFontEncoding encoding = font->GetEncoding();
    if ( !wxGetNativeFontEncoding(encoding, &info) )
    {
        if ( !wxTheFontMapper->GetAltForEncoding(encoding, &info) )
        {
            // unsupported encoding, replace with the default
            info.charset = ANSI_CHARSET;
        }
    }

    if ( !info.facename.IsEmpty() )
    {
        // the facename determined by the encoding overrides everything else
        ff_face = info.facename;
    }

    // transfer all the data to LOGFONT
    logFont->lfHeight = nHeight;
    logFont->lfWidth = 0;
    logFont->lfEscapement = 0;
    logFont->lfOrientation = 0;
    logFont->lfWeight = ff_weight;
    logFont->lfItalic = ff_italic;
    logFont->lfUnderline = (BYTE)font->GetUnderlined();
    logFont->lfStrikeOut = 0;
    logFont->lfCharSet = info.charset;
    logFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont->lfQuality = PROOF_QUALITY;
    logFont->lfPitchAndFamily = DEFAULT_PITCH | ff_family;
    wxStrncpy(logFont->lfFaceName, ff_face, WXSIZEOF(logFont->lfFaceName));
}

wxFont wxCreateFontFromLogFont(const LOGFONT *logFont)
{
    // extract family from pitch-and-family
    int lfFamily = logFont->lfPitchAndFamily;
    if ( lfFamily & FIXED_PITCH )
        lfFamily -= FIXED_PITCH;
    if ( lfFamily & VARIABLE_PITCH )
        lfFamily -= VARIABLE_PITCH;

    int fontFamily;
    switch ( lfFamily )
    {
        case FF_ROMAN:
            fontFamily = wxROMAN;
            break;

        case FF_SWISS:
            fontFamily = wxSWISS;
            break;

        case FF_SCRIPT:
            fontFamily = wxSCRIPT;
            break;

        case FF_MODERN:
            fontFamily = wxMODERN;
            break;

        case FF_DECORATIVE:
            fontFamily = wxDECORATIVE;
            break;

        default:
            fontFamily = wxSWISS;
    }

    // weight and style
    int fontWeight = wxNORMAL;
    switch ( logFont->lfWeight )
    {
        case FW_LIGHT:
            fontWeight = wxLIGHT;
            break;

        default:
        case FW_NORMAL:
            fontWeight = wxNORMAL;
            break;

        case FW_BOLD:
            fontWeight = wxBOLD;
            break;
    }

    int fontStyle = logFont->lfItalic ? wxITALIC : wxNORMAL;

    bool fontUnderline = logFont->lfUnderline != 0;

    wxString fontFace = logFont->lfFaceName;

    // font size
    HDC dc = ::GetDC(NULL);

    // remember that 1pt = 1/72inch
    int height = abs(logFont->lfHeight);
    int fontPoints = (72*height)/GetDeviceCaps(dc, LOGPIXELSY);

    ::ReleaseDC(NULL, dc);

    wxFontEncoding fontEncoding;
    switch ( logFont->lfCharSet )
    {
        default:
            wxFAIL_MSG(wxT("unsupported charset"));
            // fall through

        case ANSI_CHARSET:
            fontEncoding = wxFONTENCODING_CP1252;
            break;

#ifdef __WIN32__
        case EASTEUROPE_CHARSET:
            fontEncoding = wxFONTENCODING_CP1250;
            break;

        case BALTIC_CHARSET:
            fontEncoding = wxFONTENCODING_CP1257;
            break;

        case RUSSIAN_CHARSET:
            fontEncoding = wxFONTENCODING_CP1251;
            break;

        case ARABIC_CHARSET:
            fontEncoding = wxFONTENCODING_CP1256;
            break;

        case GREEK_CHARSET:
            fontEncoding = wxFONTENCODING_CP1253;
            break;

        case HEBREW_CHARSET:
            fontEncoding = wxFONTENCODING_CP1255;
            break;

        case TURKISH_CHARSET:
            fontEncoding = wxFONTENCODING_CP1254;
            break;

        case THAI_CHARSET:
            fontEncoding = wxFONTENCODING_CP437;
            break;
#endif

        case OEM_CHARSET:
            fontEncoding = wxFONTENCODING_CP437;
            break;
    }

    return wxFont(fontPoints, fontFamily, fontStyle,
                  fontWeight, fontUnderline, fontFace,
                  fontEncoding);
}


