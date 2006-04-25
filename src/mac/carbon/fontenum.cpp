///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/fontenum.cpp
// Purpose:     wxFontEnumerator class for MacOS
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTMAP

#include "wx/fontenum.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/intl.h"
#endif

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/fontutil.h"
#include "wx/encinfo.h"

#include "wx/mac/private.h"


class wxFontEnumeratorHelper
{
public:
    wxFontEnumeratorHelper(wxFontEnumerator *fontEnum);

    // control what exactly are we enumerating
    bool SetEncoding(wxFontEncoding encoding);
    void SetFixedOnly(bool fixedOnly)
        { m_fixedOnly = fixedOnly; }

    // call to start enumeration
    void DoEnumerate();

private:
    // the object we forward calls to OnFont() to
    wxFontEnumerator *m_fontEnum;

    // if != -1, enum only fonts which have this encoding
    int m_charset;

    // if not empty, enum only the fonts with this facename
    wxString m_facename;

    // if true, enum only fixed fonts
    bool m_fixedOnly;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontEnumeratorHelper
// ----------------------------------------------------------------------------

wxFontEnumeratorHelper::wxFontEnumeratorHelper(wxFontEnumerator *fontEnum)
{
    m_fontEnum = fontEnum;
    m_charset = -1;
    m_fixedOnly = false;
}

bool wxFontEnumeratorHelper::SetEncoding(wxFontEncoding encoding)
{
    wxNativeEncodingInfo info;
    if ( !wxGetNativeFontEncoding( encoding, &info ) )
    {
        if ( !wxFontMapper::Get()->GetAltForEncoding( encoding, &info ) )
            // no such encodings at all
            return false;
    }

    m_charset = info.charset;
    m_facename = info.facename;

    return true;
}

void wxFontEnumeratorHelper::DoEnumerate()
{
    MenuHandle    menu;
    short         lines;

    menu = NewMenu( 32000, "\pFont" );
    AppendResMenu( menu, 'FONT' );
    lines = CountMenuItems( menu );

    for ( int i = 1; i < lines + 1; i ++ )
    {
        wxString c_name ;
#if TARGET_API_MAC_CARBON
        CFStringRef menutext ;
        c_name = wxEmptyString ;
        if ( CopyMenuItemTextAsCFString (menu, i, &menutext) == noErr )
        {
            c_name = wxMacCFStringHolder(menutext).AsString(wxLocale::GetSystemEncoding());
        }
#else
        Str255        p_name;
        GetMenuItemText( menu, i, p_name );
        c_name = wxMacMakeStringFromPascal( p_name );
#endif

#if 0
        if ( m_fixedOnly )
        {
            // check that it's a fixed pitch font:
            // there is *no* error here: the flag name is misleading!
            if ( tm->tmPitchAndFamily & TMPF_FIXED_PITCH )
                // not a fixed pitch font
                return true;
        }

        if ( m_charset != -1 )
        {
            // check that we have the right encoding
            if ( lf->lfCharSet != m_charset )
                return true;
        }
#endif

        m_fontEnum->OnFacename( c_name );
    }

    DisposeMenu( menu );
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    wxFontEnumeratorHelper fe(this);
    if ( fe.SetEncoding(encoding) )
    {
        fe.SetFixedOnly(fixedWidthOnly);

        fe.DoEnumerate();
    }
    // else: no such fonts, unknown encoding

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    wxFAIL_MSG(wxT("wxFontEnumerator::EnumerateEncodings() not yet implemented"));

    return true;
}

#endif // wxUSE_FONTMAP
