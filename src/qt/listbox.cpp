/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/dynarray.h"
#include "wx/listbox.h"
#include "wx/utils.h"

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(wxListBox,wxControl)

wxListBox::wxListBox(void)
{
};

wxListBox::wxListBox( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      int n, const wxString choices[],
      long style, const wxString &name )
{
  Create( parent, id, pos, size, n, choices, style, name );
};

bool wxListBox::Create( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size, 
      int n, const wxString choices[],
      long style, const wxString &name )
{
  return TRUE;
};

void wxListBox::Append( const wxString &item )
{
  Append( item, (char*)NULL );
};

void wxListBox::Append( const wxString &WXUNUSED(item), char *WXUNUSED(clientData) )
{
};

void wxListBox::Clear(void)
{
};

void wxListBox::Delete( int WXUNUSED(n) )
{
};

void wxListBox::Deselect( int WXUNUSED(n) )
{
};

int wxListBox::FindString( const wxString &WXUNUSED(item) ) const
{
  return -1;
};

char *wxListBox::GetClientData( int WXUNUSED(n) ) const
{
  return (char*)NULL;
};

int wxListBox::GetSelection(void) const
{
  return -1;
};

int wxListBox::GetSelections( wxArrayInt& WXUNUSED(aSelections) ) const
{
  return 0;
};

wxString wxListBox::GetString( int WXUNUSED(n) ) const
{
  return "";
};

wxString wxListBox::GetStringSelection(void) const
{
  return "";
};

int wxListBox::Number(void)
{
  return 0;
};

bool wxListBox::Selected( int WXUNUSED(n) )
{
  return FALSE;
};

void wxListBox::Set( int WXUNUSED(n), const wxString *WXUNUSED(choices) )
{
};

void wxListBox::SetClientData( int WXUNUSED(n), char *WXUNUSED(clientData) )
{
};

void wxListBox::SetFirstItem( int WXUNUSED(n) )
{
};

void wxListBox::SetFirstItem( const wxString &WXUNUSED(item) )
{
};

void wxListBox::SetSelection( int WXUNUSED(n), bool WXUNUSED(select) )
{
};

void wxListBox::SetString( int WXUNUSED(n), const wxString &WXUNUSED(string) )
{
};

void wxListBox::SetStringSelection( const wxString &WXUNUSED(string), bool WXUNUSED(select) )
{
};



