///////////////////////////////////////////////////////////////////////////////
// Name:        univ/slider.cpp
// Purpose:     implementation of the universal version of wxSlider
// Author:      Vadim Zeitlin
// Modified by:
// Created:     09.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
   There is some discrepancy in wxSL_LABELS style handling between wxMSW and
   wxGTK: the latter handles it natively and shows only the current value of
   the slider on the side corresponding to wxSL_TOP/BOTTOM/LEFT/RIGHT style
   given (which can be combined with wxSL_HORIZONTAL/VERTICAL) while wxMSW
   emulates this somehow and shows the min and max values near the ends of the
   slider and the current value in a separate static box nearby.

   We currently follow wxGTK except that wxSL_HORIZONTAL slider can only have
   the label displayed on top or bottom of it and wxSL_VERTICAL - to the left
   or right.

   What we really need is probably a more fine grain control on labels, i.e. we
   should be able to select if we show nothign at all, the current value only
   or the value and the limits - the current approach is just that of the
   greatest common denominator.

   TODO:

   1. support for ticks (probably only regular one) - search for TICKS
   2. support for all orientations
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univslider.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SLIDER

#ifndef WX_PRECOMP
#endif

#include "wx/slider.h"

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin around the slider (FIXME: not configurable)
static const wxCoord SLIDER_THUMB_MARGIN = 3;

// the margin between the slider and the label (FIXME: hardcoded)
static const wxCoord SLIDER_LABEL_MARGIN = 2;

// ============================================================================
// implementation of wxSlider
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

BEGIN_EVENT_TABLE(wxSlider, wxControl)
    EVT_SIZE(wxSlider::OnSize)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxSlider creation
// ----------------------------------------------------------------------------

void wxSlider::Init()
{
    m_min =
    m_max =
    m_value = 0;

    m_lineSize =
    m_pageSize = 0;

    m_thumbSize = 0;
}

bool wxSlider::Create(wxWindow *parent,
                      wxWindowID id,
                      int value, int minValue, int maxValue,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    if ( !wxSliderBase::Create(parent, id, pos, size, style,
                               validator, name) )
        return FALSE;

    SetBestSize(size);
    SetRange(minValue, maxValue);
    SetValue(value);

    CreateInputHandler(wxINP_HANDLER_SLIDER);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxSlider range and value
// ----------------------------------------------------------------------------

int wxSlider::GetValue() const
{
    return m_value;
}

int wxSlider::NormalizeValue(int value) const
{
    if ( value < m_min )
        return m_min;
    else if ( value > m_max )
        return m_max;
    else
        return value;
}

bool wxSlider::ChangeValueBy(int inc)
{
    return ChangeValueTo(NormalizeValue(m_value + inc));
}

bool wxSlider::ChangeValueTo(int value)
{
    // this method is protected and we should only call it with normalized
    // value!
    wxCHECK_MSG( IsInRange(value), FALSE, _T("invalid slider value") );

    // check if the value is going to change at all
    if ( value == m_value )
        return FALSE;

    m_value = value;

    Refresh(); // FIXME: shouldn't refresh everything!

    // generate the event
    wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, GetId());
    event.SetInt(m_value);
    event.SetEventObject(this);

    (void)GetEventHandler()->ProcessEvent(event);

    return TRUE;
}

void wxSlider::SetValue(int value)
{
    value = NormalizeValue(value);

    if ( m_value != value )
    {
        m_value = value;

        Refresh();
    }
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    if ( minValue > maxValue )
    {
        // swap them, we always want min to be less than max
        int tmp = minValue;
        minValue = maxValue;
        maxValue = tmp;
    }

    m_min = minValue;
    m_max = maxValue;

    // reset the value to make sure it is in the new range
    SetValue(m_value);

    Refresh();
}

int wxSlider::GetMin() const
{
    return m_min;
}

int wxSlider::GetMax() const
{
    return m_max;
}

// ----------------------------------------------------------------------------
// wxSlider line/page/thumb size
// ----------------------------------------------------------------------------

void wxSlider::SetLineSize(int lineSize)
{
    wxCHECK_RET( lineSize > 0, _T("invalid slider line size") );

    m_lineSize = lineSize;
}

void wxSlider::SetPageSize(int pageSize)
{
    wxCHECK_RET( pageSize > 0, _T("invalid slider page size") );

    m_pageSize = pageSize;
}

int wxSlider::GetLineSize() const
{
    if ( !m_lineSize )
    {
        // the default line increment is 1
        wxConstCast(this, wxSlider)->m_lineSize = 1;
    }

    return m_lineSize;
}

int wxSlider::GetPageSize() const
{
    if ( !m_pageSize )
    {
        // the default page increment is 1/10 of the range
        wxConstCast(this, wxSlider)->m_pageSize = (m_max - m_min) / 10;
    }

    return m_pageSize;
}

void wxSlider::SetThumbLength(int lenPixels)
{
    wxCHECK_RET( lenPixels > 0, _T("invalid slider thumb size") );

    // use m_thumbSize here directly and not GetThumbLength() to avoid setting
    // it to the default value as we don't need it
    if ( lenPixels != m_thumbSize )
    {
        m_thumbSize = lenPixels;

        Refresh();
    }
}

int wxSlider::GetThumbLength() const
{
    if ( !m_thumbSize )
    {
        wxSize sz = GetRenderer()->GetSliderThumbSize();
        wxConstCast(this, wxSlider)->m_thumbSize = IsVertical() ? sz.y : sz.x;
    }

    return m_thumbSize;
}

// ----------------------------------------------------------------------------
// wxSlider geometry
// ----------------------------------------------------------------------------

wxSize wxSlider::CalcLabelSize() const
{
    wxSize size;

    if ( HasLabels() )
    {
        wxCoord w1, h1, w2, h2;
        GetTextExtent(FormatValue(m_min), &w1, &h1);
        GetTextExtent(FormatValue(m_max), &w2, &h2);

        size.x = wxMax(w1, w2);
        size.y = wxMax(h1, h2);
    }
    else // no labels
    {
        size.x =
        size.y = 0;
    }

    return size;
}

wxSize wxSlider::DoGetBestClientSize() const
{
    // this dimension is completely arbitrary
    static const wxCoord SLIDER_WIDTH = 100;

    // first calculate the size of the slider itself: i.e. the shaft and the
    // thumb
    wxCoord height = GetRenderer()->GetSliderThumbSize().y;

    height += 2*SLIDER_THUMB_MARGIN;

    wxSize size;
    if ( IsVertical() )
    {
        size.x = height;
        size.y = SLIDER_WIDTH;
    }
    else // horizontal
    {
        size.x = SLIDER_WIDTH;
        size.y = height;
    }

    // if we have the label, reserve enough space for it
    if ( HasLabels() )
    {
        wxSize sizeLabels = CalcLabelSize();

        if ( IsVertical() )
            size.x += sizeLabels.x + SLIDER_LABEL_MARGIN;
        else
            size.y += sizeLabels.y + SLIDER_LABEL_MARGIN;
    }

    // TODO TICKS: add space for ticks on the other side of the label

    return size;
}

void wxSlider::OnSize(wxSizeEvent& event)
{
    /*
       recalc the label and slider positions, this looks like this for
       wxSL_HORIZONTAL | wxSL_TOP slider:

       LLL             lll
       -------------------------
       |                T      |  <-- this is the slider rect
       | HHHHHHHHHHHHHHHTHHHHH |
       |                T      |
       -------------------------

       LLL is m_rectLabel as calculated here and lll is the real rect used for
       label drawing in OnDraw() (TTT indicated the thumb position)

       in the wxSL_VERTICAL | wxSL_RIGHT case the picture is like this:

       ----- LLL
       | H |
       | H |
       | H |
       | H |
       | H |
       | H |
       | H |
       |TTT| lll
       | H |
       | H |
       -----
    */

    wxRect rectTotal = GetClientRect();
    if ( HasLabels() )
    {
        wxSize sizeLabels = CalcLabelSize();

        m_rectSlider = rectTotal;
        m_rectLabel = wxRect(rectTotal.GetPosition(), sizeLabels);

        // split the rect
        if ( IsVertical() )
        {
            sizeLabels.x += SLIDER_LABEL_MARGIN;

            if ( GetWindowStyle() & wxSL_LEFT )
            {
                // shrink and offset the slider to the right
                m_rectSlider.x += sizeLabels.x;
                m_rectSlider.width -= sizeLabels.x;
            }
            else // wxSL_RIGHT
            {
                // just shrink the slider and move the label to the right
                m_rectSlider.width -= sizeLabels.x;

                m_rectLabel.x += m_rectSlider.width + SLIDER_LABEL_MARGIN;
            }
        }
        else // horizontal
        {
            // same logic as above but x/y are trasnposed
            sizeLabels.y += SLIDER_LABEL_MARGIN;

            if ( GetWindowStyle() & wxSL_TOP )
            {
                m_rectSlider.y += sizeLabels.y;
                m_rectSlider.height -= sizeLabels.y;
            }
            else // wxSL_BOTTOM
            {
                m_rectSlider.height -= sizeLabels.y;

                m_rectLabel.y += m_rectSlider.height + SLIDER_LABEL_MARGIN;
            }
        }
    }
    else // no labels
    {
        // the slider takes the whole client rect
        m_rectSlider = rectTotal;
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxSlider drawing
// ----------------------------------------------------------------------------

wxString wxSlider::FormatValue(int value) const
{
    return wxString::Format(_T("%d"), value);
}

void wxSlider::DoDraw(wxControlRenderer *renderer)
{
    wxRenderer *rend = GetRenderer();
    wxDC& dc = renderer->GetDC();
    bool isVertical = IsVertical();
    wxOrientation orient = isVertical ? wxVERTICAL : wxHORIZONTAL;
    int flags = GetStateFlags();

    // first draw the shaft
    wxRect rectShaft;
    rend->DrawSliderShaft(dc, m_rectSlider, orient, flags, &rectShaft);

    // calculate the thumb position in pixels and draw it
    wxSize sizeThumb = rend->GetSliderThumbSize();
    if ( IsVertical() )
    {
        wxCoord tmp = sizeThumb.x;
        sizeThumb.x = sizeThumb.y;
        sizeThumb.y = tmp;
    }

    wxRect rectThumb(rectShaft.GetPosition(), sizeThumb);
    if ( isVertical )
    {
        rectThumb.x += (rectShaft.width - rectThumb.width) / 2;
        rectThumb.y += (rectShaft.height*(m_value - m_min))/(m_max - m_min);
    }
    else // horz
    {
        rectThumb.x += (rectShaft.width*(m_value - m_min))/(m_max - m_min);
        rectThumb.y += (rectShaft.height - rectThumb.height) / 2;
    }

    rend->DrawSliderThumb(dc, rectThumb, orient, flags);

    // draw the label near the thumb
    if ( HasLabels() )
    {
        wxRect rectLabel = m_rectLabel;

        // centre the label relatively to the thumb position
        if ( IsVertical() )
        {
            rectLabel.y =
                rectThumb.y + (rectThumb.height - m_rectLabel.height)/2;
        }
        else // horz
        {
            rectLabel.x =
                rectThumb.x + (rectThumb.width - m_rectLabel.width)/2;
        }

        dc.SetFont(GetFont());
        dc.SetTextForeground(GetForegroundColour());

        // the slider label is never drawn focused
        rend->DrawLabel(dc, FormatValue(m_value), rectLabel,
                        flags & ~wxCONTROL_FOCUSED, wxALIGN_CENTRE);
    }

    // TODO TICKS: draw them
}

// ----------------------------------------------------------------------------
// wxSlider input processing
// ----------------------------------------------------------------------------

bool wxSlider::PerformAction(const wxControlAction& action,
                             long numArg,
                             const wxString& strArg)
{
    if ( action == wxACTION_SLIDER_START )
    {
        ChangeValueTo(m_min);
    }
    else if ( action == wxACTION_SLIDER_END )
    {
        ChangeValueTo(m_max);
    }
    else if ( action == wxACTION_SLIDER_LINE_UP )
    {
        ChangeValueBy(-m_lineSize);
    }
    else if ( action == wxACTION_SLIDER_PAGE_UP )
    {
        ChangeValueBy(-m_pageSize);
    }
    else if ( action == wxACTION_SLIDER_LINE_DOWN )
    {
        ChangeValueBy(m_lineSize);
    }
    else if ( action == wxACTION_SLIDER_PAGE_DOWN )
    {
        ChangeValueBy(m_pageSize);
    }
    else if ( action == wxACTION_SLIDER_THUMB_DRAG )
    {
        // no special processing for it
        return TRUE;
    }
    else if ( action == wxACTION_SLIDER_THUMB_MOVE ||
              action == wxACTION_SLIDER_THUMB_RELEASE )
    {
        ChangeValueTo((int)numArg);
    }
    else
    {
        return wxControl::PerformAction(action, numArg, strArg);
    }

    return TRUE;
}

#endif // wxUSE_SLIDER
