/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
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
};

wxTextCtrl::wxTextCtrl( wxWindow *parent, const wxWindowID id, const wxString &value, 
      const wxPoint &pos, const wxSize &size, 
      const int style, const wxString &name ) : streambuf()
{
  Create( parent, id, value, pos, size, style, name );
};

bool wxTextCtrl::Create( wxWindow *parent, const wxWindowID id, const wxString &value,
      const wxPoint &pos, const wxSize &size, 
      const int style, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  if (style & wxTE_MULTILINE)
    m_widget = gtk_text_new( NULL, NULL );
  else
    m_widget = gtk_entry_new();
    
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 80;
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  PostCreation();
  
  if (!value.IsNull())
  {
    gint tmp = 0;
    gtk_editable_insert_text( GTK_EDITABLE(m_widget), value, value.Length(), &tmp );
  };
  
  Show( TRUE );
    
  return TRUE;
};

wxString wxTextCtrl::GetValue(void) const
{
  wxString tmp;
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_widget) );
    tmp = gtk_editable_get_chars( GTK_EDITABLE(m_widget), 0, len-1 );
  }
  else
  {
    tmp = gtk_entry_get_text( GTK_ENTRY(m_widget) );
  };
  return tmp;
};

void wxTextCtrl::SetValue( const wxString &value )
{
  wxString tmp = "";
  if (!value.IsNull()) tmp = value;
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_widget) );
    gtk_editable_delete_text( GTK_EDITABLE(m_widget), 0, len-1 );
    len = 0;
    gtk_editable_insert_text( GTK_EDITABLE(m_widget), tmp, tmp.Length(), &len );
  }
  else
  {
    gtk_entry_set_text( GTK_ENTRY(m_widget), tmp );
  };
};

void wxTextCtrl::WriteText( const wxString &text )
{
  if (text.IsNull()) return;
  
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_widget) ) - 1;
    gtk_editable_insert_text( GTK_EDITABLE(m_widget), text, text.Length(), &len );
  }
  else
  {
    gtk_entry_append_text( GTK_ENTRY(m_widget), text );
  };
};

/*
wxString wxTextCtrl::GetLineText( const long lineNo ) const
{
};

bool wxTextCtrl::LoadFile( const wxString &file )
{
};

bool wxTextCtrl::SaveFile( const wxString &file )
{
};

void wxTextCtrl::DiscardEdits(void)
{
};

bool wxTextCtrl::IsModified(void)
{
};

void wxTextCtrl::OnDropFiles( wxDropFilesEvent &event )
{
};

long wxTextCtrl::PositionToXY( const long pos, long *x, long *y ) const
{
};

long wxTextCtrl::XYToPosition( const long x, const long y )
{
};

int wxTextCtrl::GetNumberOfLines(void)
{
};

*/
void wxTextCtrl::SetInsertionPoint( const long pos )
{
  int tmp = (int) pos;
  if (m_windowStyle & wxTE_MULTILINE)
    gtk_text_set_point( GTK_TEXT(m_widget), tmp );
  else
    gtk_entry_set_position( GTK_ENTRY(m_widget), tmp );
};

void wxTextCtrl::SetInsertionPointEnd(void)
{
  int pos = 0;
  if (m_windowStyle & wxTE_MULTILINE)
    pos = gtk_text_get_length( GTK_TEXT(m_widget) );
  else
    pos = GTK_ENTRY(m_widget)->text_length;
  SetInsertionPoint( pos-1 );
};

void wxTextCtrl::SetEditable( const bool editable )
{
  if (m_windowStyle & wxTE_MULTILINE)
    gtk_text_set_editable( GTK_TEXT(m_widget), editable );
  else
    gtk_entry_set_editable( GTK_ENTRY(m_widget), editable );
};

void wxTextCtrl::SetSelection( const long from, const long to )
{
  gtk_editable_select_region( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
};

void wxTextCtrl::ShowPosition( const long WXUNUSED(pos) )
{
};

long wxTextCtrl::GetInsertionPoint(void) const
{
  return (long) GTK_EDITABLE(m_widget)->current_pos;
};

long wxTextCtrl::GetLastPosition(void) const
{
  int pos = 0;
  if (m_windowStyle & wxTE_MULTILINE)
    pos = gtk_text_get_length( GTK_TEXT(m_widget) );
  else
    pos = GTK_ENTRY(m_widget)->text_length;
  return (long)pos-1;
};

void wxTextCtrl::Remove( const long from, const long to )
{
  gtk_editable_delete_text( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
};

void wxTextCtrl::Replace( const long from, const long to, const wxString &value )
{
  gtk_editable_delete_text( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
  if (value.IsNull()) return;
  gint pos = (gint)to;
  gtk_editable_insert_text( GTK_EDITABLE(m_widget), value, value.Length(), &pos );
};

void wxTextCtrl::Cut(void)
{
  gtk_editable_cut_clipboard( GTK_EDITABLE(m_widget), 0 );
};

void wxTextCtrl::Copy(void)
{
  gtk_editable_copy_clipboard( GTK_EDITABLE(m_widget), 0 );
};

void wxTextCtrl::Paste(void)
{
  gtk_editable_paste_clipboard( GTK_EDITABLE(m_widget), 0 );
};

void wxTextCtrl::Delete(void)
{
  SetValue( "" );
};

void wxTextCtrl::OnChar( wxKeyEvent &WXUNUSED(event) )
{
};

int wxTextCtrl::overflow(int c)
{
  // Make sure there is a holding area
  if ( allocate()==EOF )
  {
    wxError("Streambuf allocation failed","Internal error");
    return EOF;
  }
  
  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  // Reset get area
  setg(0,0,0);

  // Make sure there is a put area
  if ( ! pptr() )
  {
/* This doesn't seem to be fatal so comment out error message */
//    wxError("Put area not opened","Internal error");
    setp( base(), base() );
  }

  // Determine how many characters have been inserted but no consumed
  int plen = pptr() - pbase();

  // Now Jerry relies on the fact that the buffer is at least 2 chars
  // long, but the holding area "may be as small as 1" ???
  // And we need an additional \0, so let's keep this inefficient but
  // safe copy.

  // If c!=EOF, it is a character that must also be comsumed
  int xtra = c==EOF? 0 : 1;

  // Write temporary C-string to wxTextWindow
  {
  char *txt = new char[plen+xtra+1];
  memcpy(txt, pbase(), plen);
  txt[plen] = (char)c;     // append c
  txt[plen+xtra] = '\0';   // append '\0' or overwrite c
    // If the put area already contained \0, output will be truncated there
  WriteText(txt);
    delete[] txt;
  }

  // Reset put area
  setp(pbase(), epptr());

#if defined(__WATCOMC__)
  return __NOT_EOF;
#elif defined(zapeof)     // HP-UX (all cfront based?)
  return zapeof(c);
#else
  return c!=EOF ? c : 0;  // this should make everybody happy
#endif

/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return EOF;
*/
};

int wxTextCtrl::sync(void)
{
  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  if ( pptr() && pptr() > pbase() ) return overflow(EOF);

  return 0;
/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return 0;
*/
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

wxTextCtrl& wxTextCtrl::operator<<(const float f)
{
  static char buf[100];
  sprintf(buf, "%.2f", f);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const double d)
{
  static char buf[100];
  sprintf(buf, "%.2f", d);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const int i)
{
  static char buf[100];
  sprintf(buf, "%i", i);
  WriteText(buf);
  return *this;
}

wxTextCtrl& wxTextCtrl::operator<<(const long i)
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

