/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#include "wx/choice.h"

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoice,wxControl)

wxChoice::wxChoice(void)
{
};

wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      long style, const wxString &name )
{
  Create( parent, id, pos, size, n, choices, style, name );
};

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      int n, const wxString choices[],
      long style, const wxString &name )
{
  return TRUE;
};
      
void wxChoice::Append( const wxString &WXUNUSED(item) )
{
};
 
void wxChoice::Clear(void)
{
};

int wxChoice::FindString( const wxString &WXUNUSED(string) ) const
{
  return -1;
};

int wxChoice::GetColumns(void) const
{
  return 1;
};

int wxChoice::GetSelection(void)
{
  return -1;
};

wxString wxChoice::GetString( int WXUNUSED(n) ) const
{
  return "";
};

wxString wxChoice::GetStringSelection(void) const
{
  return "";
};

int wxChoice::Number(void) const
{
  return 0;
};

void wxChoice::SetColumns( int WXUNUSED(n) )
{
};

void wxChoice::SetSelection( int WXUNUSED(n) )
{
};

void wxChoice::SetStringSelection( const wxString &string )
{
  int n = FindString( string );
  if (n != -1) SetSelection( n );
};

