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

void gtk_text_changed_callback( GtkWidget *WXUNUSED(widget), wxTextCtrl *win )
{
  win->m_modified = TRUE;
};


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

  // we want to be notified about text changes
  gtk_signal_connect(GTK_OBJECT(m_widget), "changed",
                     GTK_SIGNAL_FUNC(gtk_text_changed_callback),
                     (gpointer)this);

  if (!value.IsNull())
  {
    gint tmp = 0;
    gtk_editable_insert_text( GTK_EDITABLE(m_widget), value, value.Length(), &tmp );
  };

  if (style & wxREADONLY)
  {
  }
  else
  {
    if (style & wxTE_MULTILINE) gtk_text_set_editable( GTK_TEXT(m_widget), 1 );
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
    tmp = gtk_editable_get_chars( GTK_EDITABLE(m_widget), 0, len );
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
    gtk_editable_delete_text( GTK_EDITABLE(m_widget), 0, len );
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
    gint len = gtk_text_get_length( GTK_TEXT(m_widget) );
    gtk_editable_insert_text( GTK_EDITABLE(m_widget), text, text.Length(), &len );
  }
  else
  {
    gtk_entry_append_text( GTK_ENTRY(m_widget), text );
  };
};

bool wxTextCtrl::LoadFile( const wxString &WXUNUSED(file) )
{
  wxFAIL_MSG("wxTextCtrl::LoadFile not implemented");

  return FALSE;
};

bool wxTextCtrl::SaveFile( const wxString &WXUNUSED(file) )
{
  wxFAIL_MSG("wxTextCtrl::SaveFile not implemented");

  return FALSE;
};

bool wxTextCtrl::IsModified(void)
{
  return m_modified;
};

void wxTextCtrl::DiscardEdits(void)
{
  m_modified = FALSE;
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

void wxTextCtrl::SetEditable( bool editable )
{
  if (m_windowStyle & wxTE_MULTILINE)
    gtk_text_set_editable( GTK_TEXT(m_widget), editable );
  else
    gtk_entry_set_editable( GTK_ENTRY(m_widget), editable );
};

void wxTextCtrl::SetSelection( long from, long to )
{
  gtk_editable_select_region( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
};

void wxTextCtrl::ShowPosition( long WXUNUSED(pos) )
{
  wxFAIL_MSG("wxTextCtrl::ShowPosition not implemented");
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

void wxTextCtrl::Remove( long from, long to )
{
  gtk_editable_delete_text( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
};

void wxTextCtrl::Replace( long from, long to, const wxString &value )
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

