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
#include <wx/intl.h>

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

static void gtk_text_changed_callback( GtkWidget *WXUNUSED(widget), wxTextCtrl *win )
{
  win->SetModified();
  
  wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, win->m_windowId );
  wxString val( win->GetValue() );
  if (!val.IsNull()) event.m_commandString = WXSTRINGCAST val;
  event.SetEventObject( win );
  win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl,wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
  EVT_CHAR(wxTextCtrl::OnChar)
END_EVENT_TABLE()

wxTextCtrl::wxTextCtrl(void) : streambuf()
{
  if (allocate()) setp(base(),ebuf());

  m_modified = FALSE;
}

wxTextCtrl::wxTextCtrl( wxWindow *parent, wxWindowID id, const wxString &value,
      const wxPoint &pos, const wxSize &size,
      int style, const wxValidator& validator, const wxString &name ) : streambuf()
{
  if (allocate()) setp(base(),ebuf());

  m_modified = FALSE;
  Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create( wxWindow *parent, wxWindowID id, const wxString &value,
      const wxPoint &pos, const wxSize &size,
      int style, const wxValidator& validator, const wxString &name )
{
  m_needParent = TRUE;

  PreCreation( parent, id, pos, size, style, name );

  SetValidator( validator );

  bool bMultiLine = (style & wxTE_MULTILINE) != 0;
  if ( bMultiLine )
  {
    // a multi-line edit control: create a vertical scrollbar by default and
    // horizontal if requested
    bool bHasHScrollbar = (style & wxHSCROLL) != 0;

    // create our control...
    m_text = gtk_text_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );

    // ... and put into the upper left hand corner of the table
    m_widget = gtk_table_new(bHasHScrollbar ? 2 : 1, 2, FALSE);
    gtk_table_attach(GTK_TABLE(m_widget), m_text, 0, 1, 0, 1,
                     GTK_FILL | GTK_EXPAND,
                     GTK_FILL | GTK_EXPAND | GTK_SHRINK,
                     0, 0);

    // put the horizontal scrollbar in the lower left hand corner
    if (bHasHScrollbar) 
    {
      GtkWidget *hscrollbar = gtk_hscrollbar_new(GTK_TEXT(m_text)->hadj);
      gtk_table_attach(GTK_TABLE(m_widget), hscrollbar, 0, 1, 1, 2,
                       GTK_EXPAND | GTK_FILL,
                       GTK_FILL,
                       0, 0);
      gtk_widget_show(hscrollbar);
    }

    // finally, put the vertical scrollbar in the upper right corner
    GtkWidget *vscrollbar = gtk_vscrollbar_new(GTK_TEXT(m_text)->vadj);
    gtk_table_attach(GTK_TABLE(m_widget), vscrollbar, 1, 2, 0, 1,
                     GTK_FILL,
                     GTK_EXPAND | GTK_FILL | GTK_SHRINK,
                     0, 0);
    gtk_widget_show( vscrollbar );
  }
  else 
  {
    // a single-line text control: no need for scrollbars
    m_widget =
    m_text = gtk_entry_new();
  }

  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 80;
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );

  PostCreation();

  if (bMultiLine) 
  {
    gtk_widget_realize(m_text);
    gtk_widget_show(m_text);
  }

  // we want to be notified about text changes
  gtk_signal_connect(GTK_OBJECT(m_text), "changed",
                     GTK_SIGNAL_FUNC(gtk_text_changed_callback),
                     (gpointer)this);

  if (!value.IsNull())
  {
    gint tmp = 0;
    gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &tmp );
  }

  if (style & wxTE_READONLY)
  {
  }
  else
  {
    if (bMultiLine)
      gtk_text_set_editable( GTK_TEXT(m_text), 1 );
  }

  Show( TRUE );

  return TRUE;
}

wxString wxTextCtrl::GetValue(void) const
{
  wxString tmp;
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_text) );
    tmp = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );
  }
  else
  {
    tmp = gtk_entry_get_text( GTK_ENTRY(m_text) );
  }
  return tmp;
}

void wxTextCtrl::SetValue( const wxString &value )
{
  wxString tmp = "";
  if (!value.IsNull()) tmp = value;
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_text) );
    gtk_editable_delete_text( GTK_EDITABLE(m_text), 0, len );
    len = 0;
    gtk_editable_insert_text( GTK_EDITABLE(m_text), tmp, tmp.Length(), &len );
  }
  else
  {
    gtk_entry_set_text( GTK_ENTRY(m_text), tmp );
  }
}

void wxTextCtrl::WriteText( const wxString &text )
{
  if (text.IsNull()) return;

  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_text) );
    gtk_editable_insert_text( GTK_EDITABLE(m_text), text, text.Length(), &len );
  }
  else
  {
    gtk_entry_append_text( GTK_ENTRY(m_text), text );
  }
}

bool wxTextCtrl::LoadFile( const wxString &WXUNUSED(file) )
{
  wxFAIL_MSG( "wxTextCtrl::LoadFile not implemented" );

  return FALSE;
}

bool wxTextCtrl::SaveFile( const wxString &WXUNUSED(file) )
{
  wxFAIL_MSG( "wxTextCtrl::SaveFile not implemented" );

  return FALSE;
}

/*
wxString wxTextCtrl::GetLineText( long lineNo ) const
{
}


void wxTextCtrl::OnDropFiles( wxDropFilesEvent &event )
{
}

long wxTextCtrl::PositionToXY( long pos, long *x, long *y ) const
{
}

long wxTextCtrl::XYToPosition( long x, long y )
{
}

int wxTextCtrl::GetNumberOfLines(void)
{
}

*/
void wxTextCtrl::SetInsertionPoint( long pos )
{
  int tmp = (int) pos;
  if (m_windowStyle & wxTE_MULTILINE)
    gtk_text_set_point( GTK_TEXT(m_text), tmp );
  else
    gtk_entry_set_position( GTK_ENTRY(m_text), tmp );
}

void wxTextCtrl::SetInsertionPointEnd(void)
{
  int pos = 0;
  if (m_windowStyle & wxTE_MULTILINE)
    pos = gtk_text_get_length( GTK_TEXT(m_text) );
  else
    pos = GTK_ENTRY(m_text)->text_length;
  SetInsertionPoint( pos-1 );
}

void wxTextCtrl::SetEditable( bool editable )
{
  if (m_windowStyle & wxTE_MULTILINE)
    gtk_text_set_editable( GTK_TEXT(m_text), editable );
  else
    gtk_entry_set_editable( GTK_ENTRY(m_text), editable );
}

void wxTextCtrl::SetSelection( long from, long to )
{
  gtk_editable_select_region( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::ShowPosition( long WXUNUSED(pos) )
{
  wxFAIL_MSG(_("wxTextCtrl::ShowPosition not implemented"));
}

long wxTextCtrl::GetInsertionPoint(void) const
{
  return (long) GTK_EDITABLE(m_text)->current_pos;
}

long wxTextCtrl::GetLastPosition(void) const
{
  int pos = 0;
  if (m_windowStyle & wxTE_MULTILINE)
    pos = gtk_text_get_length( GTK_TEXT(m_text) );
  else
    pos = GTK_ENTRY(m_text)->text_length;
  return (long)pos-1;
}

void wxTextCtrl::Remove( long from, long to )
{
  gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::Replace( long from, long to, const wxString &value )
{
  gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
  if (value.IsNull()) return;
  gint pos = (gint)to;
  gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &pos );
}

void wxTextCtrl::Cut(void)
{
  gtk_editable_cut_clipboard( GTK_EDITABLE(m_text), 0 );
}

void wxTextCtrl::Copy(void)
{
  gtk_editable_copy_clipboard( GTK_EDITABLE(m_text), 0 );
}

void wxTextCtrl::Paste(void)
{
  gtk_editable_paste_clipboard( GTK_EDITABLE(m_text), 0 );
}

void wxTextCtrl::Delete(void)
{
  SetValue( "" );
}

void wxTextCtrl::OnChar( wxKeyEvent &key_event )
{
  if ((key_event.KeyCode() == WXK_RETURN) && (m_windowStyle & wxPROCESS_ENTER))
  {
    wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
    event.SetEventObject(this);
    if (GetEventHandler()->ProcessEvent(event)) return;
  }
  else if (key_event.KeyCode() == WXK_TAB) 
  {
    wxNavigationKeyEvent event;
    event.SetDirection( key_event.m_shiftDown );
    event.SetWindowChange(FALSE);
    event.SetEventObject(this);

    if (GetEventHandler()->ProcessEvent(event)) return;
  }
  key_event.Skip();
}

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
}

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
}

int wxTextCtrl::underflow(void)
{
  return EOF;
}

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

GtkWidget* wxTextCtrl::GetConnectWidget(void)
{
  return GTK_WIDGET(m_text);
}

bool wxTextCtrl::IsOwnGtkWindow( GdkWindow *window )
{
  if (m_windowStyle & wxTE_MULTILINE)
    return (window == GTK_TEXT(m_text)->text_area);
  else
    return (window == GTK_ENTRY(m_text)->text_area);
}

void wxTextCtrl::SetFont( const wxFont &font )
{
  if (((wxFont*)&font)->Ok())
    m_font = font;
  else
    m_font = *wxSWISS_FONT;
  
  GtkStyle *style = (GtkStyle*) NULL;
  if (!m_hasOwnStyle)
  {
    m_hasOwnStyle = TRUE;
    style = gtk_style_copy( gtk_widget_get_style( m_text ) );
  }
  else
  {
    style = gtk_widget_get_style( m_text );
  }
  
  gdk_font_unref( style->font );
  style->font = gdk_font_ref( m_font.GetInternalFont( 1.0 ) );
  
  gtk_widget_set_style( m_text, style );
}


