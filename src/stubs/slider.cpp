/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"

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

    m_pageSize = (int)((maxValue-minValue)/10);

    // TODO create slider

    return FALSE;
}

wxSlider::~wxSlider()
{
}

int wxSlider::GetValue() const
{
    // TODO
    return 0;
}

void wxSlider::SetValue(int value)
{
    // TODO
}

void wxSlider::GetSize(int *width, int *height) const
{
    // TODO
}

void wxSlider::GetPosition(int *x, int *y) const
{
    // TODO
}

void wxSlider::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    // TODO
}

// For trackbars only
void wxSlider::SetTickFreq(int n, int pos)
{
    // TODO
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // TODO
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // TODO
}

void wxSlider::ClearTicks()
{
    // TODO
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    // TODO
}

int wxSlider::GetLineSize() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelEnd() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelStart() const
{
    // TODO
    return 0;
}

void wxSlider::SetSelection(int minPos, int maxPos)
{
    // TODO
}

void wxSlider::SetThumbLength(int len)
{
    // TODO
}

int wxSlider::GetThumbLength() const
{
    // TODO
    return 0;
}

void wxSlider::SetTick(int tickPos)
{
    // TODO
}

void wxSlider::Command (wxCommandEvent & event)
{
  SetValue (event.GetInt());
  ProcessCommand (event);
}

bool wxSlider::Show(bool show)
{
    // TODO
    return TRUE;
}

