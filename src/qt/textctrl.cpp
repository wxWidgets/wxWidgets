/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/textctrl.h"
#include "wx/utils.h"

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl,wxControl)


BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
//  EVT_CHAR(wxTextCtrl::OnChar)
END_EVENT_TABLE()

wxTextCtrl::wxTextCtrl(void) : streambuf()
{
    if( allocate() )
  setp(base(),ebuf());

  m_modified = FALSE;
};

wxTextCtrl::wxTextCtrl( wxWindow *parent, wxWindowID id, const wxString &value,
      const wxPoint &pos, const wxSize &size,
      int style, const wxString &name ) : streambuf()
{
  if( allocate() )
    setp(base(),ebuf());

  m_modified = FALSE;
  Create( parent, id, value, pos, size, style, name );
};

bool wxTextCtrl::Create( wxWindow *parent, wxWindowID id, const wxString &value,
      const wxPoint &pos, const wxSize &size,
      int style, const wxString &name )
{
  return TRUE;
};

wxString wxTextCtrl::GetValue(void) const
{
  return tmp;
};

void wxTextCtrl::SetValue( const wxString &value )
{
};

void wxTextCtrl::WriteText( const wxString &text )
{
};

bool wxTextCtrl::LoadFile( const wxString &WXUNUSED(file) )
{
  return FALSE;
};

bool wxTextCtrl::SaveFile( const wxString &WXUNUSED(file) )
{
  return FALSE;
};

/*
wxString wxTextCtrl::GetLineText( long lineNo ) const
{
};


void wxTextCtrl::OnDropFiles( wxDropFilesEvent &event )
{
};

long wxTextCtrl::PositionToXY( long pos, long *x, long *y ) const
{
};

long wxTextCtrl::XYToPosition( long x, long y )
{
};

int wxTextCtrl::GetNumberOfLines(void)
{
};

*/
void wxTextCtrl::SetInsertionPoint( long pos )
{
};

void wxTextCtrl::SetInsertionPointEnd(void)
{
};

void wxTextCtrl::SetEditable( bool editable )
{
};

void wxTextCtrl::SetSelection( long from, long to )
{
};

void wxTextCtrl::ShowPosition( long WXUNUSED(pos) )
{
};

long wxTextCtrl::GetInsertionPoint(void) const
{
};

long wxTextCtrl::GetLastPosition(void) const
{
};

void wxTextCtrl::Remove( long from, long to )
{
};

void wxTextCtrl::Replace( long from, long to, const wxString &value )
{
};

void wxTextCtrl::Cut(void)
{
};

void wxTextCtrl::Copy(void)
{
};

void wxTextCtrl::Paste(void)
{
};

void wxTextCtrl::Delete(void)
{
};

void wxTextCtrl::OnChar( wxKeyEvent &WXUNUSED(event) )
{
};

int wxTextCtrl::overflow( int WXUNUSED(c) )
{
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return EOF;
};

int wxTextCtrl::sync(void)
{
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return 0;
};

int wxTextCtrl::underflow(void)
{
  return EOF;
};

wxTextCtrl& wxTextCtrl::operator<<(const wxString& s)
{
  WriteText(s);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(float f)
{
  static char buf[100];
  sprintf(buf, "%.2f", f);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(double d)
{
  static char buf[100];
  sprintf(buf, "%.2f", d);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(int i)
{
  static char buf[100];
  sprintf(buf, "%i", i);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(long i)
{
  static char buf[100];
  sprintf(buf, "%ld", i);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const char c)
{
  char buf[2];

  buf[0] = c;
  buf[1] = 0;
  WriteText(buf);
  return *this;
}


  
