///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontutil.cpp
// Purpose:     font-related helper functions for wxMSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
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

// for MSVC5 and old w32api
#ifndef HANGUL_CHARSET
#    define HANGUL_CHARSET  129
#endif

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

    wxString tmp = tokenizer.GetNextToken();
    if ( tmp.empty() )
    {
        // default charset: but don't use DEFAULT_CHARSET here because it might
        // be different from the machine on which the file we had read this
        // encoding desc from was created
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

    // ANSI_CHARSET is assumed anyhow
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

    extern WXDLLIMPEXP_BASE long wxEncodingToCharset(wxFontEncoding encoding);
    info->charset = wxEncodingToCharset(encoding);
    if ( info->charset == -1 )
        return FALSE;

    info->encoding = encoding;

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    // try to create such font
    LOGFONT lf;
    wxZeroMemory(lf);       // all default values

    lf.lfCharSet = info.charset;
    wxStrncpy(lf.lfFaceName, info.facename, WXSIZEOF(lf.lfFaceName));

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
// wxFontEncoding <-> CHARSET_XXX
// ----------------------------------------------------------------------------

wxFontEncoding wxGetFontEncFromCharSet(int cs)
{
    wxFontEncoding fontEncoding;

    switch ( cs )
    {
        default:
            // assume the system charset
            fontEncoding = wxFONTENCODING_SYSTEM;
            break;

        case ANSI_CHARSET:
            fontEncoding = wxFONTENCODING_CP1252;
            break;

#if defined(__WIN32__) && !defined(__WXMICROWIN__)
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

        case SHIFTJIS_CHARSET:
            fontEncoding = wxFONTENCODING_CP932;
            break;

        case GB2312_CHARSET:
            fontEncoding = wxFONTENCODING_CP936;
            break;

        case HANGUL_CHARSET:
            fontEncoding = wxFONTENCODING_CP949;
            break;

        case CHINESEBIG5_CHARSET:
            fontEncoding = wxFONTENCODING_CP950;
            break;

#endif // Win32

        case OEM_CHARSET:
            fontEncoding = wxFONTENCODING_CP437;
            break;
    }

    return fontEncoding;
}

// ----------------------------------------------------------------------------
// wxFont <-> LOGFONT conversion
// ----------------------------------------------------------------------------

void wxFillLogFont(LOGFONT *logFont, const wxFont *font)
{
    // maybe we already have LOGFONT for this font?
    wxNativeFontInfo *fontinfo = font->GetNativeFontInfo();
    if ( !fontinfo )
    {
        // use wxNativeFontInfo methods to build a LOGFONT for this font
        fontinfo = new wxNativeFontInfo;

        // translate all font parameters
        fontinfo->SetStyle((wxFontStyle)font->GetStyle());
        fontinfo->SetWeight((wxFontWeight)font->GetWeight());
        fontinfo->SetUnderlined(font->GetUnderlined());
        fontinfo->SetPointSize(font->GetPointSize());

        // set the family/facename
        fontinfo->SetFamily((wxFontFamily)font->GetFamily());
        wxString facename = font->GetFaceName();
        if ( !facename.empty() )
        {
            fontinfo->SetFaceName(facename);
        }

        // deal with encoding now (it may override the font family and facename
        // so do it after setting them)
        fontinfo->SetEncoding(font->GetEncoding());
    }

    // transfer all the data to LOGFONT
    *logFont = fontinfo->lf;

    delete fontinfo;
}

wxFont wxCreateFontFromLogFont(const LOGFONT *logFont)
{
    wxNativeFontInfo info;

    info.lf = *logFont;

    return wxFont(info);
}

