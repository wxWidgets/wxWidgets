/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"
#include "wx/utils.h"

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>

#include <wx/motif/private.h>

void wxSliderCallback (Widget widget, XtPointer clientData, XmScaleCallbackStruct * cbs);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

BEGIN_EVENT_TABLE(wxSlider, wxControl)
END_EVENT_TABLE()
#endif



// Slider
wxSlider::wxSlider()
{
    m_pageSize = 1;
    m_lineSize = 1;
    m_rangeMax = 0;
    m_rangeMin = 0;
    m_tickFreq = 0;
}

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
                      int value, int minValue, int maxValue,
                      const wxPoint& pos,
                      const wxSize& size, long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    
    if (parent) parent->AddChild(this);
    
    m_lineSize = 1;
    m_windowStyle = style;
    m_tickFreq = 0;
    
    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;
    
    m_rangeMax = maxValue;
    m_rangeMin = minValue;
    
    // Not used in Motif, I think
    m_pageSize = (int)((maxValue-minValue)/10);
    
    Widget parentWidget = (Widget) parent->GetClientWidget();
    
    Widget sliderWidget = XtVaCreateManagedWidget ("sliderWidget",
        xmScaleWidgetClass, parentWidget,
        XmNorientation,
        (((m_windowStyle & wxSL_VERTICAL) == wxSL_VERTICAL) ? XmVERTICAL : XmHORIZONTAL),
        XmNprocessingDirection,
        (((m_windowStyle & wxSL_VERTICAL) == wxSL_VERTICAL) ? XmMAX_ON_TOP : XmMAX_ON_RIGHT),
        XmNmaximum, maxValue,
        XmNminimum, minValue,
        XmNvalue, value,
        XmNshowValue, True,
        NULL);
    
    m_mainWidget = (WXWidget) sliderWidget;
    
    if(style & wxSL_NOTIFY_DRAG)
        XtAddCallback (sliderWidget, XmNdragCallback,
        (XtCallbackProc) wxSliderCallback, (XtPointer) this);
    else
        XtAddCallback (sliderWidget, XmNvalueChangedCallback,
        (XtCallbackProc) wxSliderCallback, (XtPointer) this);
    
    XtAddCallback (sliderWidget, XmNdragCallback, (XtCallbackProc) wxSliderCallback, (XtPointer) this);
    
    m_font = parent->GetFont();
    
    ChangeFont(FALSE);
    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);
    
    ChangeBackgroundColour();
    
    return TRUE;
}

wxSlider::~wxSlider()
{
}

int wxSlider::GetValue() const
{
    int val;
    XtVaGetValues ((Widget) m_mainWidget, XmNvalue, &val, NULL);
    return val;
}

void wxSlider::SetValue(int value)
{
    XtVaSetValues ((Widget) m_mainWidget, XmNvalue, value, NULL);
}

void wxSlider::GetSize(int *width, int *height) const
{
    wxControl::GetSize(width, height);
}

void wxSlider::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    Widget widget = (Widget) m_mainWidget;
    
    bool managed = XtIsManaged(widget);
    
    if (managed)
        XtUnmanageChild (widget);
    
    if (((m_windowStyle & wxHORIZONTAL) == wxHORIZONTAL) && (width > -1))
    {
        XtVaSetValues (widget, XmNscaleWidth, wxMax (width, 10), NULL);
    }
    
    if (((m_windowStyle & wxVERTICAL) == wxVERTICAL) && (height > -1))
    {
        XtVaSetValues (widget, XmNscaleHeight, wxMax (height, 10), NULL);
    }
    
    int xx = x; int yy = y;
    AdjustForParentClientOrigin(xx, yy, sizeFlags);
    
    if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        XtVaSetValues (widget, XmNx, xx, NULL);
    if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        XtVaSetValues (widget, XmNy, yy, NULL);
    
    if (managed)
        XtManageChild (widget);
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    
    XtVaSetValues ((Widget) m_mainWidget, XmNminimum, minValue, XmNmaximum, maxValue, NULL);
}

// For trackbars only
void wxSlider::SetTickFreq(int n, int pos)
{
    // Not implemented in Motif
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // Not implemented in Motif
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // Not implemented in Motif
}

void wxSlider::ClearTicks()
{
    // Not implemented in Motif
}

void wxSlider::SetLineSize(int lineSize)
{
    // Not implemented in Motif
    m_lineSize = lineSize;
}

int wxSlider::GetLineSize() const
{
    // Not implemented in Motif
    return m_lineSize;
}

int wxSlider::GetSelEnd() const
{
    // Not implemented in Motif
    return 0;
}

int wxSlider::GetSelStart() const
{
    // Not implemented in Motif
    return 0;
}

void wxSlider::SetSelection(int WXUNUSED(minPos), int WXUNUSED(maxPos))
{
    // Not implemented in Motif
}

void wxSlider::SetThumbLength(int WXUNUSED(len))
{
    // Not implemented in Motif (?)
}

int wxSlider::GetThumbLength() const
{
    // Not implemented in Motif (?)
    return 0;
}

void wxSlider::SetTick(int WXUNUSED(tickPos))
{
    // Not implemented in Motif
}

void wxSlider::Command (wxCommandEvent & event)
{
    SetValue (event.GetInt());
    ProcessCommand (event);
}

void wxSlider::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxSlider::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxSlider::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

void wxSliderCallback (Widget widget, XtPointer clientData, XmScaleCallbackStruct * cbs)
{
    wxSlider *slider = (wxSlider *) clientData;
    switch (cbs->reason)
    {
    case XmCR_VALUE_CHANGED:
    case XmCR_DRAG:
    default:
        {
            // TODO: the XmCR_VALUE_CHANGED case should be handled
            // differently (it's not sent continually as the slider moves).
            // In which case we need a similar behaviour for other platforms.
            
            wxScrollEvent event(wxEVT_SCROLL_THUMBTRACK, slider->GetId());
            XtVaGetValues (widget, XmNvalue, &event.m_commandInt, NULL);
            event.SetEventObject(slider);
            slider->ProcessCommand(event);
            
            // Also send a wxCommandEvent for compatibility.
            wxCommandEvent event2(wxEVT_COMMAND_SLIDER_UPDATED, slider->GetId());
            event2.SetEventObject(slider);
            slider->ProcessCommand(event2);
            break;
        }
    }
}

