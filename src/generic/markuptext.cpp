///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/markuptext.cpp
// Purpose:     wxMarkupText implementation
// Author:      Vadim Zeitlin
// Created:     2011-02-21
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
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


#if wxUSE_MARKUP

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/control.h"
    #include "wx/dc.h"
#endif // WX_PRECOMP

#include "wx/renderer.h"

#include "wx/generic/private/markuptext.h"

#include "wx/private/markupparserattr.h"

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/graphics.h"

    #include <memory>
#endif

namespace
{

// ----------------------------------------------------------------------------
// wxMarkupParserMeasureOutput: measure the extends of a markup string.
// ----------------------------------------------------------------------------

class wxMarkupParserMeasureOutput : public wxMarkupParserAttrOutput
{
public:
    // Initialize the base class with the font to use. As we don't care about
    // colours (which don't affect the text measurements), don't bother to
    // specify them at all.
    wxMarkupParserMeasureOutput(wxDC& dc, int *visibleHeight)
        : wxMarkupParserAttrOutput(dc.GetFont(), wxColour(), wxColour()),
          m_dc(dc),
          m_visibleHeight(visibleHeight)
    {
        if ( visibleHeight )
            *visibleHeight = 0;
    }

    const wxSize& GetSize() const { return m_size; }


    virtual void OnText(const wxString& text) override
    {
        // TODO-MULTILINE-MARKUP: Must use GetMultiLineTextExtent().
        const wxSize size = m_dc.GetTextExtent(text);

        m_size.x += size.x;
        if ( size.y > m_size.y )
            m_size.y = size.y;

        if ( m_visibleHeight )
        {
            wxFontMetrics tm = m_dc.GetFontMetrics();
            int visibleHeight = tm.ascent - tm.internalLeading;
            if ( *m_visibleHeight < visibleHeight )
                *m_visibleHeight = visibleHeight;
        }
    }

    virtual void OnAttrStart(const Attr& attr) override
    {
        m_dc.SetFont(attr.font);
    }

    virtual void OnAttrEnd(const Attr& WXUNUSED(attr)) override
    {
        m_dc.SetFont(GetFont());
    }

private:
    wxDC& m_dc;

    // The values that we compute.
    wxSize m_size;
    int * const m_visibleHeight;    // may be null

    wxDECLARE_NO_COPY_CLASS(wxMarkupParserMeasureOutput);
};

// ----------------------------------------------------------------------------
// wxMarkupParserRenderOutput: render a markup string.
// ----------------------------------------------------------------------------

class wxMarkupParserRenderOutput : public wxMarkupParserAttrOutput
{
public:
    // Notice that the bottom of rectangle passed to our ctor is used as the
    // baseline for the text we draw, i.e. it needs to be adjusted to exclude
    // descent by the caller.
    wxMarkupParserRenderOutput(wxDC& dc,
                               const wxRect& rect,
                               int flags)
        : wxMarkupParserAttrOutput(dc.GetFont(),
                                   dc.GetTextForeground(),
                                   wxColour()),
          m_dc(dc),
          m_rect(rect),
          m_flags(flags)

        // We don't initialize the base class initial text background colour to
        // the valid value because we want to be able to detect when we revert
        // to the "absence of background colour" and set the background mode to
        // be transparent in OnAttrStart() below. But do remember it to be able
        // to restore it there later -- this doesn't affect us as the text
        // background isn't used anyhow when the background mode is transparent
        // but it might affect the caller if it sets the background mode to
        // opaque and draws some text after using us.
        , m_origTextBackground(dc.GetTextBackground())
        , m_pos(m_rect.x)
    {
    }

    virtual void OnAttrStart(const Attr& attr) override
    {
        m_dc.SetFont(attr.font);
        if ( attr.foreground.IsOk() )
            m_dc.SetTextForeground(attr.foreground);

        if ( attr.background.IsOk() )
        {
            // Setting the background colour is not enough, we must also change
            // the mode to ensure that it is actually used.
            m_dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
            m_dc.SetTextBackground(attr.background);
        }
    }

    virtual void OnAttrEnd(const Attr& attr) override
    {
        // We always restore the font because we always change it...
        m_dc.SetFont(GetFont());

        // ...but we only need to restore the colours if we had changed them.
        if ( attr.foreground.IsOk() )
            m_dc.SetTextForeground(GetAttr().effectiveForeground);

        if ( attr.background.IsOk() )
        {
            wxColour background = GetAttr().effectiveBackground;
            if ( !background.IsOk() )
            {
                // Invalid background colour indicates that the background
                // should actually be made transparent and in this case the
                // actual value of background colour doesn't matter but we also
                // restore it just in case, see comment in the ctor.
                m_dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
                background = m_origTextBackground;
            }

            m_dc.SetTextBackground(background);
        }
    }

protected:
    wxDC& m_dc;
    const wxRect m_rect;
    const int m_flags;

    wxColour m_origTextBackground;

    // Current horizontal text output position.
    //
    // TODO-MULTILINE-MARKUP: Must keep vertical position too.
    int m_pos;

    wxDECLARE_NO_COPY_CLASS(wxMarkupParserRenderOutput);
};

// An output renderer suitable for control labels.
class wxMarkupParserRenderLabelOutput : public wxMarkupParserRenderOutput
{
public:
    wxMarkupParserRenderLabelOutput(wxDC& dc,
                                    const wxRect& rect,
                                    int flags)
        : wxMarkupParserRenderOutput(dc, rect, flags)
    {
    }

    virtual void OnText(const wxString& text_) override
    {
        wxString text;
        int indexAccel = wxControl::FindAccelIndex(text_, &text);
        if ( !(m_flags & wxMarkupText::Render_ShowAccels) )
            indexAccel = wxNOT_FOUND;

        // Adjust the position (unfortunately we need to do this manually as
        // there is no notion of current text position in wx API) rectangle to
        // ensure that all text segments use the same baseline (as there is
        // nothing equivalent to Windows SetTextAlign(TA_BASELINE) either).
        wxRect rect(m_rect);
        rect.x = m_pos;

        int descent;
        m_dc.GetTextExtent(text, &rect.width, &rect.height, &descent);
        rect.height -= descent;
        rect.y += m_rect.height - rect.height;

        wxRect bounds;
        m_dc.DrawLabel(text, wxBitmap(),
                       rect, wxALIGN_LEFT | wxALIGN_TOP,
                       indexAccel,
                       &bounds);

        m_pos += bounds.width;
    }
};

// An output renderer suitable for multi-item controls items.
class wxMarkupParserRenderItemOutput : public wxMarkupParserRenderOutput
{
public:
    wxMarkupParserRenderItemOutput(wxWindow *win,
                                   wxDC& dc,
                                   const wxRect& rect,
                                   int rendererFlags,
                                   wxEllipsizeMode ellipsizeMode)
        : wxMarkupParserRenderOutput(dc, rect, wxMarkupText::Render_Default),
          m_win(win),
          m_rendererFlags(rendererFlags),
          m_renderer(&wxRendererNative::Get())
    {
        // TODO: Support all ellipsizing modes
        m_ellipsizeMode = ellipsizeMode == wxELLIPSIZE_NONE ? wxELLIPSIZE_NONE : wxELLIPSIZE_END;
    }

    virtual void OnText(const wxString& text) override
    {
        wxRect rect(m_rect);
        rect.x = m_pos;
        rect.SetRight(m_rect.GetRight());

        const wxSize extent = m_dc.GetTextExtent(text);

        // DrawItemText() ignores background color, so render it ourselves
        if ( m_dc.GetBackgroundMode() == wxBRUSHSTYLE_SOLID)
        {
#if wxUSE_GRAPHICS_CONTEXT
            // Prefer to use wxGraphicsContext because it supports alpha channel; fall back to wxDC
            if ( !m_gc )
                m_gc.reset(wxGraphicsContext::CreateFromUnknownDC(m_dc));

            if ( m_gc )
            {
                m_gc->SetBrush(wxBrush(m_dc.GetTextBackground()));
                m_gc->SetPen(*wxTRANSPARENT_PEN);
                m_gc->DrawRectangle(rect.x, rect.y, extent.x, extent.y);
            }
            else
#endif // wxUSE_GRAPHICS_CONTEXT
            {
                wxDCPenChanger pen(m_dc, *wxTRANSPARENT_PEN);
                wxDCBrushChanger brush(m_dc, wxBrush(m_dc.GetTextBackground()));
                m_dc.DrawRectangle(rect.x, rect.y, extent.x, extent.y);
            }
        }

        m_renderer->DrawItemText(m_win,
                                 m_dc,
                                 text,
                                 rect,
                                 wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL,
                                 m_rendererFlags,
                                 m_ellipsizeMode);

        m_pos += extent.x;
    }

private:
#if wxUSE_GRAPHICS_CONTEXT
    std::unique_ptr<wxGraphicsContext> m_gc;
#endif
    wxWindow* const m_win;
    int const m_rendererFlags;
    wxEllipsizeMode m_ellipsizeMode;
    wxRendererNative* const m_renderer;

    wxDECLARE_NO_COPY_CLASS(wxMarkupParserRenderItemOutput);
};

} // anonymous namespace

// ============================================================================
// wxMarkupText implementation
// ============================================================================

wxSize wxMarkupTextBase::Measure(wxDC& dc, int *visibleHeight) const
{
    wxMarkupParserMeasureOutput out(dc, visibleHeight);
    wxMarkupParser parser(out);
    if ( !parser.Parse(GetMarkupForMeasuring()) )
    {
        wxFAIL_MSG( "Invalid markup" );
        return wxDefaultSize;
    }

    return out.GetSize();
}

wxString wxMarkupText::GetMarkupForMeasuring() const
{
    return wxControl::RemoveMnemonics(m_markup);
}

void wxMarkupText::Render(wxDC& dc, const wxRect& rect, int flags)
{
    // We want to center the above-baseline parts of the letter vertically, so
    // we use the visible height and not the total height (which includes
    // descent and internal leading) here.
    int visibleHeight;
    wxRect rectText(rect.GetPosition(), Measure(dc, &visibleHeight));
    rectText.height = visibleHeight;

    wxMarkupParserRenderLabelOutput out(dc, rectText.CentreIn(rect), flags);
    wxMarkupParser parser(out);
    parser.Parse(m_markup);
}


// ============================================================================
// wxItemMarkupText implementation
// ============================================================================

void wxItemMarkupText::Render(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int rendererFlags,
                              wxEllipsizeMode ellipsizeMode)
{
    wxMarkupParserRenderItemOutput out(win, dc, rect, rendererFlags, ellipsizeMode);
    wxMarkupParser parser(out);
    parser.Parse(m_markup);
}

#endif // wxUSE_MARKUP
