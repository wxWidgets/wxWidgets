/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "scrolbar.h"
#endif

#include "wx/scrolbar.h"
#include "wx/utils.h"

//-----------------------------------------------------------------------------
// wxScrollBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar,wxControl)

wxScrollBar::wxScrollBar(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxString& name )
{
  Create( parent, id, pos, size, style, name );
};

wxScrollBar::~wxScrollBar(void)
{
};

bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxString& name )
{
  return TRUE;
};

int wxScrollBar::GetPosition(void) const
{
};

int wxScrollBar::GetThumbSize() const
{
};

int wxScrollBar::GetPageSize() const
{
};

int wxScrollBar::GetRange() const
{
};

void wxScrollBar::SetPosition( int viewStart )
{
};

void wxScrollBar::SetScrollbar( int position, int thumbSize, int range, int pageSize,
      bool WXUNUSED(refresh) )
{
};

// Backward compatibility
int wxScrollBar::GetValue(void) const
{
  return GetPosition();
};

void wxScrollBar::SetValue( int viewStart )
{
  SetPosition( viewStart );
};

void wxScrollBar::GetValues( int *viewStart, int *viewLength, int *objectLength, int *pageLength ) const
{
};

int wxScrollBar::GetViewLength() const
{
};

int wxScrollBar::GetObjectLength() const
{
};

void wxScrollBar::SetPageSize( int pageLength )
{
};

void wxScrollBar::SetObjectLength( int objectLength )
{
};

void wxScrollBar::SetViewLength( int viewLength )
{
};

