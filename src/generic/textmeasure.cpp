///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/textmeasure.cpp
// Purpose:
// Author:      Vadim Zeitlin
// Created:     2012-10-17
// RCS-ID:      $Id$
// Copyright:   (c) 2012 Vadim Zeitlin <vadim@wxwidgets.org>
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dc.h"
#endif //WX_PRECOMP

#include "wx/private/textmeasure.h"

#if wxUSE_GENERIC_TEXTMEASURE

// ============================================================================
// wxTextMeasure generic implementation
// ============================================================================

// We assume that the ports not providing platform-specific wxTextMeasure
// implementation implement the corresponding functions in their wxDC and
// wxWindow classes, so forward back to them instead of using wxTextMeasure
// from there, as usual.
void wxTextMeasure::DoGetTextExtent(const wxString& string,
                                    wxCoord *width,
                                    wxCoord *height,
                                    wxCoord *descent,
                                    wxCoord *externalLeading)
{
    if ( m_dc )
    {
        m_dc->GetTextExtent(string, width, height,
                            descent, externalLeading, m_font);
    }
    else if ( m_win )
    {
        m_win->GetTextExtent(string, width, height,
                             descent, externalLeading, m_font);
    }
    //else: we already asserted in the ctor, don't do it any more
}

// Each element of the widths array will be the width of the string up to and
// including the corresponding character in text.  This is the generic
// implementation, the port-specific classes should do this with native APIs
// if available and if faster.  Note: pango_layout_index_to_pos is much slower
// than calling GetTextExtent!!

#define FWC_SIZE 256

class FontWidthCache
{
public:
    FontWidthCache() : m_scaleX(1), m_widths(NULL) { }
    ~FontWidthCache() { delete []m_widths; }

    void Reset()
    {
        if ( !m_widths )
            m_widths = new int[FWC_SIZE];

        memset(m_widths, 0, sizeof(int)*FWC_SIZE);
    }

    wxFont m_font;
    double m_scaleX;
    int *m_widths;
};

static FontWidthCache s_fontWidthCache;

bool wxTextMeasure::DoGetPartialTextExtents(const wxString& text,
                                            wxArrayInt& widths,
                                            double scaleX)
{
    int totalWidth = 0;

    // reset the cache if font or horizontal scale have changed
    if ( !s_fontWidthCache.m_widths ||
         !wxIsSameDouble(s_fontWidthCache.m_scaleX, scaleX) ||
         (s_fontWidthCache.m_font != *m_font) )
    {
        s_fontWidthCache.Reset();
        s_fontWidthCache.m_font = *m_font;
        s_fontWidthCache.m_scaleX = scaleX;
    }

    // Calculate the position of each character based on the widths of
    // the previous characters. This is inexact for not fixed fonts.
    int n = 0;
    for ( wxString::const_iterator it = text.begin();
          it != text.end();
          ++it )
    {
        const wxChar c = *it;
        unsigned int c_int = (unsigned int)c;

        int w;
        if ((c_int < FWC_SIZE) && (s_fontWidthCache.m_widths[c_int] != 0))
        {
            w = s_fontWidthCache.m_widths[c_int];
        }
        else
        {
            DoGetTextExtent(c, &w, NULL);
            if (c_int < FWC_SIZE)
                s_fontWidthCache.m_widths[c_int] = w;
        }

        totalWidth += w;
        widths[n++] = totalWidth;
    }

    return true;
}

#endif // wxUSE_GENERIC_TEXTMEASURE
