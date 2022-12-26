///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/textmeasure.cpp
// Purpose:     wxTextMeasure implementation for wxMSW
// Author:      Manuel Martin
// Created:     2012-10-05
// Copyright:   (c) 1997-2012 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/msw/private.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/font.h"
#endif //WX_PRECOMP

#include "wx/private/textmeasure.h"

#include "wx/msw/dc.h"

// ============================================================================
// wxTextMeasure implementation
// ============================================================================

void wxTextMeasure::Init()
{
    m_hdc = nullptr;
    m_hfontOld = nullptr;

    if ( m_dc )
    {
        wxClassInfo* const ci = m_dc->GetImpl()->GetClassInfo();

        if ( ci->IsKindOf(wxCLASSINFO(wxMSWDCImpl)))
        {
            m_useDCImpl = false;
        }
    }
}

void wxTextMeasure::BeginMeasuring()
{
    if ( m_dc )
    {
        m_hdc = m_dc->GetHDC();

        // Non-native wxDC subclasses should override their DoGetTextExtent()
        // and other methods.
        wxASSERT_MSG( m_hdc, wxS("Must not be used with non-native wxDCs") );
    }
    else if ( m_win )
    {
        m_hdc = ::GetDC(GetHwndOf(m_win));
    }

    // We need to set the font if it's explicitly specified, of course, but
    // also if we're associated with a window because the window HDC created
    // above has the default font selected into it and not the font of the
    // window.
    if ( m_font || m_win )
        m_hfontOld = (HFONT)::SelectObject(m_hdc, GetHfontOf(GetFont()));
}

void wxTextMeasure::EndMeasuring()
{
    if ( m_hfontOld )
    {
        ::SelectObject(m_hdc, m_hfontOld);
        m_hfontOld = nullptr;
    }

    if ( m_win )
        ::ReleaseDC(GetHwndOf(m_win), m_hdc);
    //else: our HDC belongs to m_dc, don't touch it

    m_hdc = nullptr;
}

// Notice we don't check here the font. It is supposed to be OK before the call.
void wxTextMeasure::DoGetTextExtent(const wxString& string,
                                       wxCoord *width,
                                       wxCoord *height,
                                       wxCoord *descent,
                                       wxCoord *externalLeading)
{
    SIZE sizeRect;
    const size_t len = string.length();
    if ( !::GetTextExtentPoint32(m_hdc, string.t_str(), len, &sizeRect) )
    {
        wxLogLastError(wxT("GetTextExtentPoint32()"));
    }

    // the result computed by GetTextExtentPoint32() may be too small as it
    // accounts for under/overhang of the first/last character while we want
    // just the bounding rect for this string so adjust the width as needed
    // when using italic fonts as the difference is really noticeable for them
    // (it may still exist, but seems to be at most 1px for the other fonts,
    // and calling GetCharABCWidths() is pretty slow and much slower than
    // calling GetTextExtentPoint32() itself, so avoid its overhead unless it's
    // really, really necessary).
    const wxFont font = GetFont();
    if ( font.IsOk() && font.GetStyle() != wxFONTSTYLE_NORMAL && len > 0 )
    {
        ABC widthABC;
        const wxChar chFirst = *string.begin();
        if ( ::GetCharABCWidths(m_hdc, chFirst, chFirst, &widthABC) )
        {
            if ( widthABC.abcA < 0 )
                sizeRect.cx -= widthABC.abcA;

            if ( len > 1 )
            {
                const wxChar chLast = *string.rbegin();
                ::GetCharABCWidths(m_hdc, chLast, chLast, &widthABC);
            }
            //else: we already have the width of the last character

            if ( widthABC.abcC < 0 )
                sizeRect.cx -= widthABC.abcC;
        }
        //else: GetCharABCWidths() failed, not a TrueType font?
    }

    *width = sizeRect.cx;
    *height = sizeRect.cy;

    if ( descent || externalLeading )
    {
        TEXTMETRIC tm;
        ::GetTextMetrics(m_hdc, &tm);
        if ( descent )
            *descent = tm.tmDescent;
        if ( externalLeading )
            *externalLeading = tm.tmExternalLeading;
    }
}

bool wxTextMeasure::DoGetPartialTextExtents(const wxString& text,
                                            wxArrayInt& widths,
                                            double scaleX)
{
    if ( !m_hdc )
        return wxTextMeasureBase::DoGetPartialTextExtents(text, widths, scaleX);

    int fit = 0;
    SIZE sz = {0,0};
    if ( !::GetTextExtentExPoint(m_hdc,
                                 text.t_str(), // string to check
                                 text.length(),
                                 INT_MAX,      // max allowable width
                                 &fit,         // [out] count of chars
                                               // that will fit
                                 &widths[0],   // array to fill
                                 &sz) )
    {
        wxLogLastError(wxT("GetTextExtentExPoint"));

        return false;
    }

    // The width of \t determined by GetTextExtentExPoint is 0. Determine the
    // actual width using DoGetTextExtent and update the widths accordingly.
    int offset = 0;
    int tabWidth = 0;
    int tabHeight = 0;
    int* widthPtr = &widths[0];
    for ( wxString::const_iterator i = text.begin(); i != text.end(); ++i )
    {
        if ( *i == '\t' )
        {
            if ( tabWidth == 0 )
            {
                DoGetTextExtent("\t", &tabWidth, &tabHeight);
            }
            offset += tabWidth;
        }

        *widthPtr++ += offset;
    }

    return true;
}
