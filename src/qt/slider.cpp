/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"
#include "wx/utils.h"

//-----------------------------------------------------------------------------
// wxSlider
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSlider,wxControl)

wxSlider::wxSlider(void)
{
};

wxSlider::wxSlider( wxWindow *parent, wxWindowID id,
        int value, int minValue, int maxValue,
        const wxPoint& pos, const wxSize& size,
        long style,
/*      const wxValidator& validator = wxDefaultValidator, */
        const wxString& name )
{
  Create( parent, id, value, minValue, maxValue,
          pos, size, style, name );
};

wxSlider::~wxSlider(void)
{
};

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
        int value, int minValue, int maxValue,
        const wxPoint& pos, const wxSize& size,
        long style,
/*      const wxValidator& validator = wxDefaultValidator, */
        const wxString& name )
{
  return TRUE;
};

int wxSlider::GetValue(void) const
{
};

void wxSlider::SetValue( int value )
{
};

void wxSlider::SetRange( int minValue, int maxValue )
{
};

int wxSlider::GetMin(void) const
{
};

int wxSlider::GetMax(void) const
{
};

void wxSlider::SetPageSize( int pageSize )
{
};

int wxSlider::GetPageSize(void) const
{
};

void wxSlider::SetThumbLength( int len )
{
};

int wxSlider::GetThumbLength(void) const
{
};

void wxSlider::SetLineSize( int WXUNUSED(lineSize) )
{
};

int wxSlider::GetLineSize(void) const
{
};

void wxSlider::GetSize( int *x, int *y ) const
{
  wxWindow::GetSize( x, y );
};

void wxSlider::SetSize( int x, int y, int width, int height, int sizeFlags )
{
  wxWindow::SetSize( x, y, width, height, sizeFlags );
};

void wxSlider::GetPosition( int *x, int *y ) const
{
  wxWindow::GetPosition( x, y );
};

void wxSlider::SetTick( int WXUNUSED(tickPos) )
{
};

void wxSlider::SetTickFreq( int WXUNUSED(n), int WXUNUSED(pos) )
{
};

int wxSlider::GetTickFreq(void) const
{
  return 0;
};

void wxSlider::ClearTicks(void)
{
};

void wxSlider::SetSelection( int WXUNUSED(minPos), int WXUNUSED(maxPos) )
{
};

int wxSlider::GetSelEnd(void) const
{
  return 0;
};

int wxSlider::GetSelStart(void) const
{
  return 0;
};

void wxSlider::ClearSel(void)
{
};

