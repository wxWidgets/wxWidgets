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

  +0. add ticks support
   1. support for all orientations
   2. draw the slider thumb highlighted when it is dragged
  ?3. manual ticks support?
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

#ifdef __VISUALC__
    // warning C4355: 'this' : used in base member initializer list
    #pragma warning(disable:4355)
#endif

wxSlider::wxSlider()
        : m_thumb(this)
{
    Init();
}

wxSlider::wxSlider(wxWindow *parent,
                   wxWindowID id,
                   int value, int minValue, int maxValue,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
        : m_thumb(this)
{
    Init();

    (void)Create(parent, id, value, minValue, maxValue,
                 pos, size, style, validator, name);
}

#ifdef __VISUALC__
    // warning C4355: 'this' : used in base member initializer list
    #pragma warning(default:4355)
#endif

void wxSlider::Init()
{
    m_min =
    m_max =
    m_value = 0;

    m_tickFreq = 1;

    m_lineSize =
    m_pageSize = 0;

    m_thumbSize = 0;
    m_thumbFlags = 0;
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

    SetRange(minValue, maxValue);
    SetValue(value);

    // call this after setting the range as the best size depends (at least if
    // we have wxSL_LABELS style) on the range
    SetBestSize(size);

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

    // refresh the old thumb position
    RefreshThumb();

    m_value = value;

    // and the new one
    RefreshThumb();

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

    if ( m_min != minValue || m_max != maxValue )
    {
        m_min = minValue;
        m_max = maxValue;

        // reset the value to make sure it is in the new range
        SetValue(m_value);

        // the size of the label rect might have changed
        if ( HasLabels() )
        {
            CalcGeometry();
        }

        Refresh();
    }
    //else: nothing changed
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
        wxSize sz = GetDefaultThumbSize();
        return IsVert() ? sz.y : sz.x;
    }

    return m_thumbSize;
}

// ----------------------------------------------------------------------------
// wxSlider ticks
// ----------------------------------------------------------------------------

void wxSlider::SetTickFreq(int n, int WXUNUSED(dummy))
{
    if ( n != m_tickFreq )
    {
        m_tickFreq = n;

        Refresh();
    }
}

// ----------------------------------------------------------------------------
// wxSlider geometry
// ----------------------------------------------------------------------------

wxSize wxSlider::CalcLabelSize() const
{
    wxSize size;

    // there is no sense in trying to calc the labels size if we haven't got
    // any, the caller must check for it
    wxCHECK_MSG( HasLabels(), size, _T("shouldn't be called") );

    wxCoord w1, h1, w2, h2;
    GetTextExtent(FormatValue(m_min), &w1, &h1);
    GetTextExtent(FormatValue(m_max), &w2, &h2);

    size.x = wxMax(w1, w2);
    size.y = wxMax(h1, h2);

    return size;
}

wxSize wxSlider::DoGetBestClientSize() const
{
    // this dimension is completely arbitrary
    static const wxCoord SLIDER_WIDTH = 100;

    // first calculate the size of the slider itself: i.e. the shaft and the
    // thumb
    wxCoord height = GetRenderer()->GetSliderDim();

    wxSize size;
    if ( IsVert() )
    {
        size.x = height;
        size.y = SLIDER_WIDTH;
    }
    else // horizontal
    {
        size.x = SLIDER_WIDTH;
        size.y = height;
    }

    // add space for ticks
    if ( HasTicks() )
    {
        wxCoord lenTick = GetRenderer()->GetSliderTickLen();

        if ( IsVert() )
            size.x += lenTick;
        else
            size.y += lenTick;
    }

    // if we have the label, reserve enough space for it
    if ( HasLabels() )
    {
        wxSize sizeLabels = CalcLabelSize();

        if ( IsVert() )
            size.x += sizeLabels.x + SLIDER_LABEL_MARGIN;
        else
            size.y += sizeLabels.y + SLIDER_LABEL_MARGIN;
    }

    return size;
}

void wxSlider::OnSize(wxSizeEvent& event)
{
    CalcGeometry();

    event.Skip();
}

const wxRect& wxSlider::GetSliderRect() const
{
    if ( m_rectSlider.width < 0 )
    {
        wxConstCast(this, wxSlider)->CalcGeometry();
    }

    return m_rectSlider;
}

void wxSlider::CalcGeometry()
{
    /*
       recalc the label and slider positions, this looks like this for
       wxSL_HORIZONTAL | wxSL_TOP slider:

       LLL             lll
       -------------------------
       |                T      |  <-- this is the slider rect
       | HHHHHHHHHHHHHHHTHHHHH |
       |                T      |
       | *  *  *  *  *  *  *  *|
       -------------------------

       LLL is m_rectLabel as calculated here and lll is the real rect used for
       label drawing in OnDraw() (TTT indicated the thumb position and *s are
       the ticks)

       in the wxSL_VERTICAL | wxSL_RIGHT case the picture is like this:

       ------ LLL
       | H  |
       | H *|
       | H  |
       | H *|
       | H  |
       | H *|
       | H  |
       |TTT*| lll
       | H  |
       | H *|
       ------
    */

    wxRect rectTotal = GetClientRect();
    if ( HasLabels() )
    {
        wxSize sizeLabels = CalcLabelSize();

        m_rectSlider = rectTotal;
        m_rectLabel = wxRect(rectTotal.GetPosition(), sizeLabels);

        // split the rect
        if ( IsVert() )
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

    // now adjust for ticks too
    if ( HasTicks() )
    {
        wxCoord lenTick = GetRenderer()->GetSliderTickLen();

        if ( IsVert() )
        {
            m_rectSlider.width -= lenTick;
        }
        else // horizontal
        {
            m_rectSlider.height -= lenTick;
        }

        // note that we must compute m_rectSlider first as GetShaftRect() uses
        // it
        m_rectTicks = GetShaftRect();

        if ( IsVert() )
        {
            m_rectTicks.x = m_rectSlider.x + m_rectSlider.width;
            m_rectTicks.width = lenTick;
        }
        else // horizontal
        {
            m_rectTicks.y = m_rectSlider.y + m_rectSlider.height;
            m_rectTicks.height = lenTick;
        }

    }
}

wxSize wxSlider::GetDefaultThumbSize() const
{
    return GetRenderer()->GetSliderThumbSize(GetSliderRect(), GetOrientation());
}

wxSize wxSlider::GetThumbSize() const
{
    wxSize sizeThumb = GetDefaultThumbSize();

    // if we have our own thumb length (set by the user), use it instead of the
    // default value
    if ( m_thumbSize )
    {
        if ( IsVert() )
            sizeThumb.y = m_thumbSize;
        else
            sizeThumb.x = m_thumbSize;
    }

    return sizeThumb;
}

// ----------------------------------------------------------------------------
// wxSlider thumb geometry
// ----------------------------------------------------------------------------

wxRect wxSlider::GetShaftRect() const
{
    return GetRenderer()->GetSliderShaftRect(m_rectSlider, GetOrientation());
}

void wxSlider::CalcThumbRect(const wxRect *rectShaftIn,
                             wxRect *rectThumbOut,
                             wxRect *rectLabelOut) const
{
    bool isVertical = IsVert();

    wxRect rectShaft;
    if ( rectShaftIn )
    {
        rectShaft = *rectShaftIn;
    }
    else // no shaft rect provided, calc it
    {
        rectShaft = GetShaftRect();
    }

    wxCoord lenShaft,
            lenThumb,
           *p;
    wxRect rectThumb(rectShaft.GetPosition(), GetThumbSize());
    if ( isVertical )
    {
        rectThumb.x += (rectShaft.width - rectThumb.width) / 2;

        lenThumb = rectThumb.height;
        lenShaft = rectShaft.height;
        p = &rectThumb.y;
    }
    else // horz
    {
        rectThumb.y += (rectShaft.height - rectThumb.height) / 2;

        lenThumb = rectThumb.width;
        lenShaft = rectShaft.width;
        p = &rectThumb.x;
    }

    // the thumb must always be entirely inside the shaft limits, so the max
    // position is not at lenShaft but at lenShaft - thumbSize
    if ( m_max != m_min )
    {
        *p += ((lenShaft - lenThumb)*(m_value - m_min))/(m_max - m_min);
    }

    // calc the label rect
    if ( HasLabels() && rectLabelOut )
    {
        wxRect rectLabel = m_rectLabel;

        // centre the label relatively to the thumb position
        if ( isVertical )
        {
            rectLabel.y =
                rectThumb.y + (rectThumb.height - m_rectLabel.height)/2;
        }
        else // horz
        {
            rectLabel.x =
                rectThumb.x + (rectThumb.width - m_rectLabel.width)/2;
        }

        *rectLabelOut = rectLabel;
    }

    if ( rectThumbOut )
        *rectThumbOut = rectThumb;
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
    bool isVertical = IsVert();
    wxOrientation orient = GetOrientation();
    int flags = GetStateFlags();

    // first draw the shaft
    wxRect rectShaft;
    rend->DrawSliderShaft(dc, m_rectSlider, orient, flags, &rectShaft);

    // calculate the thumb position in pixels and draw it
    wxRect rectThumb, rectLabel;
    CalcThumbRect(&rectShaft, &rectThumb, &rectLabel);

    rend->DrawSliderThumb(dc, rectThumb, orient, flags | m_thumbFlags);

    // draw the ticks
    if ( HasTicks() )
    {
        rend->DrawSliderTicks(dc, m_rectTicks, rectThumb.GetSize(), orient,
                              m_min, m_max, m_tickFreq);
    }

    // draw the label near the thumb
    if ( HasLabels() )
    {
        // align it to be close to the shaft
        int align;
        if ( isVertical )
        {
            align = wxALIGN_CENTRE_VERTICAL |
                    (GetWindowStyle() & wxSL_RIGHT ? wxALIGN_LEFT
                                                   : wxALIGN_RIGHT);
        }
        else // horz
        {
            align = wxALIGN_CENTRE;
        }

        dc.SetFont(GetFont());
        dc.SetTextForeground(GetForegroundColour());

        // the slider label is never drawn focused
        rend->DrawLabel(dc, FormatValue(m_value), rectLabel,
                        flags & ~wxCONTROL_FOCUSED, align);
    }
}

void wxSlider::RefreshThumb()
{
    wxRect rectThumb, rectLabel;
    CalcThumbRect(NULL, &rectThumb, &rectLabel);

    Refresh(TRUE /* erase background */, &rectThumb);
    if ( HasLabels() )
    {
        Refresh(TRUE, &rectLabel);
    }
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
    else if ( action == wxACTION_SLIDER_PAGE_CHANGE )
    {
        ChangeValueBy(numArg * GetPageSize());
    }
    else if ( action == wxACTION_SLIDER_LINE_UP )
    {
        ChangeValueBy(-GetLineSize());
    }
    else if ( action == wxACTION_SLIDER_PAGE_UP )
    {
        return PerformAction(wxACTION_SLIDER_PAGE_CHANGE, -1);
    }
    else if ( action == wxACTION_SLIDER_LINE_DOWN )
    {
        ChangeValueBy(GetLineSize());
    }
    else if ( action == wxACTION_SLIDER_PAGE_DOWN )
    {
        return PerformAction(wxACTION_SLIDER_PAGE_CHANGE, 1);
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

// ----------------------------------------------------------------------------
// wxSlider implementation of wxControlWithThumb interface
// ----------------------------------------------------------------------------

wxScrollThumb::Shaft wxSlider::HitTest(const wxPoint& pt) const
{
    wxRect rectShaft = GetShaftRect();
    if ( !rectShaft.Inside(pt) )
    {
        return wxScrollThumb::Shaft_None;
    }

    // inside the shaft, where is it relatively to the thumb?
    wxRect rectThumb;
    CalcThumbRect(&rectShaft, &rectThumb, NULL);

    // the position to test and the start and end of the thumb
    wxCoord x, x1, x2;
    if ( IsVert() )
    {
        x = pt.y;
        x1 = rectThumb.GetTop();
        x2 = rectThumb.GetBottom();
    }
    else // horz
    {
        x = pt.x;
        x1 = rectThumb.GetLeft();
        x2 = rectThumb.GetRight();
    }

    if ( x < x1 )
    {
        // or to the left
        return wxScrollThumb::Shaft_Above;
    }

    if ( x > x2 )
    {
        // or to the right
        return wxScrollThumb::Shaft_Below;
    }

    // where else can it be?
    return wxScrollThumb::Shaft_Thumb;
}

wxCoord wxSlider::ThumbPosToPixel() const
{
    wxRect rectThumb;
    CalcThumbRect(NULL, &rectThumb, NULL);

    return IsVert() ? rectThumb.y : rectThumb.x;
}

int wxSlider::PixelToThumbPos(wxCoord x) const
{
    wxRect rectShaft = GetShaftRect();
    wxSize sizeThumb = GetThumbSize();

    wxCoord x0, len;
    if ( IsVert() )
    {
        x0 = rectShaft.y;
        len = rectShaft.height - sizeThumb.y;
    }
    else // horz
    {
        x0 = rectShaft.x;
        len = rectShaft.width - sizeThumb.x;
    }

    int pos = m_min;
    if ( len > 0 )
    {
        if ( x > x0 )
        {
            pos += ((x - x0) * (m_max - m_min)) / len;
            if ( pos > m_max )
                pos = m_max;
        }
        //else: x <= x0, leave pos = min
    }

    return pos;
}

void wxSlider::SetShaftPartState(wxScrollThumb::Shaft shaftPart,
                                 int flag,
                                 bool set)
{
    // for now we ignore the flags for the shaft as no renderer uses them
    // anyhow
    if ( shaftPart == wxScrollThumb::Shaft_Thumb )
    {
        if ( set )
            m_thumbFlags |= flag;
        else
            m_thumbFlags &= ~flag;

        RefreshThumb();
    }
}

void wxSlider::OnThumbDragStart(int pos)
{
    PerformAction(wxACTION_SLIDER_THUMB_DRAG, pos);
}

void wxSlider::OnThumbDrag(int pos)
{
    PerformAction(wxACTION_SLIDER_THUMB_MOVE, pos);
}

void wxSlider::OnThumbDragEnd(int pos)
{
    PerformAction(wxACTION_SLIDER_THUMB_RELEASE, pos);
}

void wxSlider::OnPageScrollStart()
{
    // we do nothing here
}

bool wxSlider::OnPageScroll(int pageInc)
{
    int value = GetValue();
    PerformAction(wxACTION_SLIDER_PAGE_CHANGE, pageInc);

    return GetValue() != value;
}

// ----------------------------------------------------------------------------
// wxStdSliderButtonInputHandler
// ----------------------------------------------------------------------------

bool wxStdSliderButtonInputHandler::HandleKey(wxControl *control,
                                              const wxKeyEvent& event,
                                              bool pressed)
{
    if ( pressed )
    {
        int keycode = event.GetKeyCode();

        wxControlAction action;
        switch ( keycode )
        {
            case WXK_HOME:
                action = wxACTION_SLIDER_START;
                break;

            case WXK_END:
                action = wxACTION_SLIDER_END;
                break;

            case WXK_LEFT:
            case WXK_UP:
                action = wxACTION_SLIDER_LINE_UP;
                break;

            case WXK_RIGHT:
            case WXK_DOWN:
                action = wxACTION_SLIDER_LINE_DOWN;
                break;

            case WXK_PRIOR:
                action = wxACTION_SLIDER_PAGE_UP;
                break;

            case WXK_NEXT:
                action = wxACTION_SLIDER_PAGE_DOWN;
                break;
        }

        if ( !!action )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdSliderButtonInputHandler::HandleMouse(wxControl *control,
                                                const wxMouseEvent& event)
{
    wxSlider *slider = wxStaticCast(control, wxSlider);

    if ( slider->GetThumb().HandleMouse(event) )
    {
        // processed by the thumb
        return FALSE;
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdSliderButtonInputHandler::HandleMouseMove(wxControl *control,
                                                    const wxMouseEvent& event)
{
    wxSlider *slider = wxStaticCast(control, wxSlider);

    if ( slider->GetThumb().HandleMouseMove(event) )
    {
        // processed by the thumb
        return FALSE;
    }

    return wxStdInputHandler::HandleMouseMove(control, event);
}

bool wxStdSliderButtonInputHandler::HandleFocus(wxControl *control,
                                                const wxFocusEvent& event)
{
    // slider's appearance changes when it gets/loses focus
    return TRUE;
}

#endif // wxUSE_SLIDER
