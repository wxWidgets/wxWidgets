/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "radiobox.h"
#endif

#include "wx/radiobox.h"
#include "wx/dialog.h"
#include "wx/frame.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox,wxControl)

wxRadioBox::wxRadioBox(void)
{
};

wxRadioBox::wxRadioBox( wxWindow *parent, wxWindowID id, const wxString& title,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      int majorDim, long style,
      const wxString &name )
{
  Create( parent, id, title, pos, size, n, choices, majorDim, style, name );
};

bool wxRadioBox::Create( wxWindow *parent, wxWindowID id, const wxString& title,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      int WXUNUSED(majorDim), long style,
      const wxString &name )
{
  return TRUE;
};

bool wxRadioBox::Show( bool show )
{
  wxWindow::Show( show );
  return TRUE;
};

int wxRadioBox::FindString( const wxString &s ) const
{
  return -1;
};

void wxRadioBox::SetSelection( int n )
{
};

int wxRadioBox::GetSelection(void) const
{
  return -1;
};

wxString wxRadioBox::GetString( int n ) const
{
};

wxString wxRadioBox::GetLabel(void) const
{
  return wxControl::GetLabel();
};

void wxRadioBox::SetLabel( const wxString& WXUNUSED(label) )
{
};

void wxRadioBox::SetLabel( int WXUNUSED(item), const wxString& WXUNUSED(label) )
{
};

void wxRadioBox::SetLabel( int WXUNUSED(item), wxBitmap *WXUNUSED(bitmap) )
{
};

wxString wxRadioBox::GetLabel( int WXUNUSED(item) ) const
{
  return "";
};

void wxRadioBox::Enable( bool WXUNUSED(enable) )
{
};

void wxRadioBox::Enable( int WXUNUSED(item), bool WXUNUSED(enable) )
{
};

void wxRadioBox::Show( int WXUNUSED(item), bool WXUNUSED(show) )
{
};

wxString wxRadioBox::GetStringSelection(void) const
{
  return "";
};

bool wxRadioBox::SetStringSelection( const wxString&s )
{
  return TRUE;
};

int wxRadioBox::Number(void) const
{
  return 0;
};

int wxRadioBox::GetNumberOfRowsOrCols(void) const
{
  return 1;
};

void wxRadioBox::SetNumberOfRowsOrCols( int WXUNUSED(n) )
{
};

