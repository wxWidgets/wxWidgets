/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

bool wxComboBox::Create(wxWindow *parent, wxWindowID id, const wxString& value,
  const wxPoint& pos, const wxSize& size, int n, const wxString choices[],
  long style, const wxString& name )
{
  return TRUE;
};

void wxComboBox::Clear(void)
{
};

void wxComboBox::Append( const wxString &item )
{
  Append( item, (char*)NULL );
};

void wxComboBox::Append( const wxString &WXUNUSED(item), char *WXUNUSED(clientData) )
{
};

void wxComboBox::Delete( int WXUNUSED(n) )
{
};

int wxComboBox::FindString( const wxString &WXUNUSED(item) )
{
  return -1;
};

char* wxComboBox::GetClientData( int WXUNUSED(n) )
{
  return (char*)NULL;
};

void wxComboBox::SetClientData( int WXUNUSED(n), char *WXUNUSED(clientData) )
{
};

int wxComboBox::GetSelection(void) const
{
  return -1;
};

wxString wxComboBox::GetString( int WXUNUSED(n) ) const
{
  return "";
};

wxString wxComboBox::GetStringSelection(void) const
{
  return "";
};

int wxComboBox::Number(void) const
{
  return 0;
};

void wxComboBox::SetSelection( int WXUNUSED(n) )
{
};

void wxComboBox::SetStringSelection( const wxString &string )
{
  int res = FindString( string );
  if (res == -1) return;
  SetSelection( res );
};

wxString wxComboBox::GetValue(void) const
{
  return "";
};

void wxComboBox::SetValue( const wxString& WXUNUSED(value) )
{
};

void wxComboBox::Copy(void)
{
};

void wxComboBox::Cut(void)
{
};

void wxComboBox::Paste(void)
{
};

void wxComboBox::SetInsertionPoint( long WXUNUSED(pos) )
{
};

void wxComboBox::SetInsertionPointEnd(void)
{
};

long wxComboBox::GetInsertionPoint(void) const
{
  return 0;
};

long wxComboBox::GetLastPosition(void) const
{
  return 0;
};

void wxComboBox::Replace( long WXUNUSED(from), long WXUNUSED(to), const wxString& WXUNUSED(value) )
{
};

void wxComboBox::Remove(long WXUNUSED(from), long WXUNUSED(to) )
{
};

void wxComboBox::SetSelection( long WXUNUSED(from), long WXUNUSED(to) )
{
};

void wxComboBox::SetEditable( bool WXUNUSED(editable) )
{
};

      
