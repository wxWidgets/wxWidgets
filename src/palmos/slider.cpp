/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/slider.cpp
// Purpose:     wxSlider
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "slider.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SLIDER

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/brush.h"
#include "wx/slider.h"
#endif

#include "wx/toplevel.h"

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxSliderStyle )

wxBEGIN_FLAGS( wxSliderStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxSL_HORIZONTAL)
    wxFLAGS_MEMBER(wxSL_VERTICAL)
    wxFLAGS_MEMBER(wxSL_AUTOTICKS)
    wxFLAGS_MEMBER(wxSL_LABELS)
    wxFLAGS_MEMBER(wxSL_LEFT)
    wxFLAGS_MEMBER(wxSL_TOP)
    wxFLAGS_MEMBER(wxSL_RIGHT)
    wxFLAGS_MEMBER(wxSL_BOTTOM)
    wxFLAGS_MEMBER(wxSL_BOTH)
    wxFLAGS_MEMBER(wxSL_SELRANGE)

wxEND_FLAGS( wxSliderStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxSlider, wxControl,"wx/scrolbar.h")

wxBEGIN_PROPERTIES_TABLE(wxSlider)
    wxEVENT_RANGE_PROPERTY( Scroll , wxEVT_SCROLL_TOP , wxEVT_SCROLL_ENDSCROLL , wxScrollEvent )
    wxEVENT_PROPERTY( Updated , wxEVT_COMMAND_SLIDER_UPDATED , wxCommandEvent )

    wxPROPERTY( Value , int , SetValue, GetValue , 0, 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Minimum , int , SetMin, GetMin, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Maximum , int , SetMax, GetMax, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( PageSize , int , SetPageSize, GetLineSize, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( LineSize , int , SetLineSize, GetLineSize, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( ThumbLength , int , SetThumbLength, GetThumbLength, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxSliderStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxSlider)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_8( wxSlider , wxWindow* , Parent , wxWindowID , Id , int , Value , int , Minimum , int , Maximum , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)
#endif

// Slider
void wxSlider::Init()
{
    m_oldValue = m_oldPos = 0;
    m_lineSize = 1;
}

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    // wxSL_AUTOTICKS is ignored - always on
    // wxSL_LABELS is ignored - always off
    // wxSL_LEFT is ignored - always off
    // wxSL_RIGHT is ignored - always off
    // wxSL_TOP is ignored - always off
    // wxSL_SELRANGE is ignored - always off
    // wxSL_INVERSE is ignored - always off
    // wxSL_VERTICAL is impossible in native form
    wxCHECK_MSG(!(style & wxSL_VERTICAL), false, _T("non vertical slider on PalmOS"));

    if(!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    FormType* form = GetParentForm();
    if(form==NULL)
        return false;

    m_oldValue = m_oldPos = value;

    SliderControlType *slider = CtlNewSliderControl (
                                   (void **)&form,
                                   GetId(),
                                   feedbackSliderCtl,
                                   NULL,
                                   0,
                                   0,
                                   pos.x,
                                   pos.y,
                                   size.x,
                                   size.y,
                                   minValue,
                                   maxValue,
                                   1,
                                   value
                              );

    if(slider==NULL)
        return false;

    Show();
    return true;
}

wxSlider::~wxSlider()
{
}

int wxSlider::GetMin() const
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if(control==NULL)
        return 0;
    uint16_t ret;
    CtlGetSliderValues(control, &ret, NULL, NULL, NULL);
    return ret;
}

int wxSlider::GetMax() const
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if(control==NULL)
        return 0;
    uint16_t ret;
    CtlGetSliderValues(control, NULL, &ret, NULL, NULL);
    return ret;
}

int wxSlider::GetPageSize() const
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if(control==NULL)
        return 0;
    uint16_t ret;
    CtlGetSliderValues(control, NULL, NULL, &ret, NULL);
    return ret;
}

int wxSlider::GetValue() const
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if(control==NULL)
        return 0;
    uint16_t ret;
    CtlGetSliderValues(control, NULL, NULL, NULL, &ret);
    return ret;
}

void wxSlider::SetValue(int value)
{
    SetIntValue(value);
    m_oldValue = m_oldPos = value;
}

wxSize wxSlider::DoGetBestSize() const
{
    // 15 is taken as used in one of official samples
    // 45 is dummy height tripled, any idea what's better ?
    return wxSize(45,15);
}


void wxSlider::SetRange(int WXUNUSED(minValue), int WXUNUSED(maxValue))
{
    // unsupported feature
}

void wxSlider::SetTickFreq(int WXUNUSED(n), int WXUNUSED(pos))
{
    // unsupported feature
}

void wxSlider::SetPageSize(int pageSize)
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if(control==NULL)
        return;
    uint16_t val = pageSize;
    CtlSetSliderValues(control, NULL, NULL, &val, NULL);
}

void wxSlider::ClearSel()
{
    // unsupported feature
}

void wxSlider::ClearTicks()
{
    // unsupported feature
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
}

int wxSlider::GetLineSize() const
{
    return m_lineSize;
}

int wxSlider::GetSelEnd() const
{
    // unsupported feature
    return GetValue();
}

int wxSlider::GetSelStart() const
{
    // unsupported feature
    return GetValue();
}

void wxSlider::SetSelection(int WXUNUSED(minPos), int WXUNUSED(maxPos))
{
    // unsupported feature
}

void wxSlider::SetThumbLength(int WXUNUSED(len))
{
    // unsupported feature
}

int wxSlider::GetThumbLength() const
{
    // unsupported feature
    return 0;
}

int wxSlider::GetTickFreq() const
{
    // unsupported feature
    return GetPageSize();
}

void wxSlider::SetTick(int WXUNUSED(tickPos))
{
    // unsupported feature
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

bool wxSlider::SendUpdatedEvent()
{
    m_oldPos = GetValue();

    // first thumb event
    wxScrollEvent eventWxTrack(wxEVT_SCROLL_THUMBRELEASE, GetId());
    eventWxTrack.SetPosition(m_oldPos);
    eventWxTrack.SetEventObject(this);
    bool handled = GetEventHandler()->ProcessEvent(eventWxTrack);

    // then slider event if position changed
    if( m_oldValue != m_oldPos )
    {
        m_oldValue = m_oldPos;
        wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, GetId());
        event.SetEventObject(this);
        event.SetInt(m_oldPos);
        return ProcessCommand(event);
    }

    return handled;
}

bool wxSlider::SendScrollEvent(EventType* event)
{
    wxEventType scrollEvent;
    int newPos = event->data.ctlRepeat.value;
    if ( newPos == m_oldPos )
    {
        // nothing changed since last event
        return false;
    }

    m_oldPos = newPos;

    // first track event
    wxScrollEvent eventWx(wxEVT_SCROLL_THUMBTRACK, GetId());
    eventWx.SetPosition(newPos);
    eventWx.SetEventObject(this);
    return GetEventHandler()->ProcessEvent(eventWx);
}

void wxSlider::Command (wxCommandEvent & event)
{
}

#endif // wxUSE_SLIDER
