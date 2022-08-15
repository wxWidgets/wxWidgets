///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textmeasurecmn.cpp
// Purpose:     wxTextMeasureBase implementation
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

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/window.h"
#endif //WX_PRECOMP

#include "wx/private/textmeasure.h"

// ============================================================================
// wxTextMeasureBase implementation
// ============================================================================

wxTextMeasureBase::wxTextMeasureBase(const wxDC *dc, const wxFont *theFont)
    : m_dc(dc),
      m_win(NULL),
      m_font(theFont)
{
    wxASSERT_MSG( dc, wxS("wxTextMeasure needs a valid wxDC") );

    // By default, use wxDC version, we'll explicitly reset this to false in
    // the derived classes if the DC is of native variety.
    m_useDCImpl = true;
}

wxTextMeasureBase::wxTextMeasureBase(const wxWindow *win, const wxFont *theFont)
    : m_dc(NULL),
      m_win(win),
      m_font(theFont)
{
    wxASSERT_MSG( win, wxS("wxTextMeasure needs a valid wxWindow") );

    // We don't have any wxDC so we can't forward to it.
    m_useDCImpl = false;
}

wxFont wxTextMeasureBase::GetFont() const
{
    return m_font ? *m_font
                  : m_win ? m_win->GetFont()
                          : m_dc->GetFont();
}

void wxTextMeasureBase::CallGetTextExtent(const wxString& string,
                                          wxCoord *width,
                                          wxCoord *height,
                                          wxCoord *descent,
                                          wxCoord *externalLeading)
{
    if ( m_useDCImpl )
        m_dc->GetTextExtent(string, width, height, descent, externalLeading);
    else
        DoGetTextExtent(string, width, height, descent, externalLeading);
}

void wxTextMeasureBase::GetTextExtent(const wxString& string,
                                      wxCoord *width,
                                      wxCoord *height,
                                      wxCoord *descent,
                                      wxCoord *externalLeading)
{
    // To make the code simpler, make sure that the width and height pointers
    // are always valid, even if they point to dummy variables.
    int unusedWidth, unusedHeight;
    if ( !width )
        width = &unusedWidth;
    if ( !height )
        height = &unusedHeight;

    // Avoid even setting up the DC for measuring if we don't actually need to
    // measure anything.
    if ( string.empty() && !descent && !externalLeading )
    {
        *width =
        *height = 0;

        return;
    }

    MeasuringGuard guard(*this);

    CallGetTextExtent(string, width, height, descent, externalLeading);
}

int wxTextMeasureBase::GetEmptyLineHeight()
{
    int dummy, height;
    CallGetTextExtent(wxS("W"), &dummy, &height);
    return height;
}

void wxTextMeasureBase::GetMultiLineTextExtent(const wxString& text,
                                               wxCoord *width,
                                               wxCoord *height,
                                               wxCoord *heightOneLine)
{
    // To make the code simpler, make sure that the width and height pointers
    // are always valid, by making them point to dummy variables if necessary.
    int unusedWidth, unusedHeight;
    if ( !width )
        width = &unusedWidth;
    if ( !height )
        height = &unusedHeight;

    *width = 0;
    *height = 0;

    MeasuringGuard guard(*this);

    // It's noticeably faster to handle the case of a string which isn't
    // actually multiline specially here, to skip iteration above in this case.
    if ( text.find('\n') == wxString::npos )
    {
        // This case needs to be handled specially as we're supposed to return
        // a non-zero height even for empty string.
        if ( text.empty() )
            *height = GetEmptyLineHeight();
        else
            CallGetTextExtent(text, width, height);

        if ( heightOneLine )
            *heightOneLine = *height;
        return;
    }

    wxCoord widthLine, heightLine = 0, heightLineDefault = 0;

    wxString::const_iterator lineStart = text.begin();
    for ( wxString::const_iterator pc = text.begin(); ; ++pc )
    {
        if ( pc == text.end() || *pc == wxS('\n') )
        {
            if ( pc == lineStart )
            {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation

                // assume that this line has the same height as the previous
                // one
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;

                // and if we hadn't had any previous one either, compute it now
                if ( !heightLineDefault )
                    heightLineDefault = GetEmptyLineHeight();

                *height += heightLineDefault;
            }
            else
            {
                CallGetTextExtent(wxString(lineStart, pc), &widthLine, &heightLine);
                if ( widthLine > *width )
                    *width = widthLine;
                *height += heightLine;
            }

            if ( pc == text.end() )
            {
               break;
            }
            else // '\n'
            {
               lineStart = pc;
               ++lineStart;
            }
        }
    }

    if ( heightOneLine )
        *heightOneLine = heightLine;
}

wxSize wxTextMeasureBase::GetLargestStringExtent(size_t n,
                                                 const wxString* strings)
{
    MeasuringGuard guard(*this);

    wxCoord w, h, widthMax = 0, heightMax = 0;
    for ( size_t i = 0; i < n; ++i )
    {
        CallGetTextExtent(strings[i], &w, &h);

        if ( w > widthMax )
            widthMax = w;
        if ( h > heightMax )
            heightMax = h;
    }

    return wxSize(widthMax, heightMax);
}

bool wxTextMeasureBase::GetPartialTextExtents(const wxString& text,
                                              wxArrayInt& widths,
                                              double scaleX)
{
    widths.Empty();
    if ( text.empty() )
        return true;

    MeasuringGuard guard(*this);

    widths.Add(0, text.length());

    return DoGetPartialTextExtents(text, widths, scaleX);
}

// ----------------------------------------------------------------------------
// Generic and inefficient DoGetPartialTextExtents() implementation.
// ----------------------------------------------------------------------------

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

bool wxTextMeasureBase::DoGetPartialTextExtents(const wxString& text,
                                                wxArrayInt& widths,
                                                double scaleX)
{
    int totalWidth = 0;

    // reset the cache if font or horizontal scale have changed
    const wxFont& font = GetFont();
    if ( !s_fontWidthCache.m_widths ||
         !wxIsSameDouble(s_fontWidthCache.m_scaleX, scaleX) ||
         (s_fontWidthCache.m_font != font) )
    {
        s_fontWidthCache.Reset();
        s_fontWidthCache.m_font = font;
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
            int dummyHeight;
            DoGetTextExtent(c, &w, &dummyHeight);
            if (c_int < FWC_SIZE)
                s_fontWidthCache.m_widths[c_int] = w;
        }

        totalWidth += w;
        widths[n++] = totalWidth;
    }

    return true;
}

