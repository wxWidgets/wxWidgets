/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/textctrl.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/settings.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

static void 
gtk_text_changed_callback( GtkWidget *WXUNUSED(widget), wxTextCtrl *win )
{
    win->SetModified();
    
    win->CalculateScrollbar();

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, win->m_windowId );
    wxString val( win->GetValue() );
    if (!val.IsNull()) event.m_commandString = WXSTRINGCAST val;
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void 
gtk_text_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* WXUNUSED(alloc), wxTextCtrl *win )
{
    win->CalculateScrollbar();
}

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl,wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(wxTextCtrl::OnChar)
END_EVENT_TABLE()

wxTextCtrl::wxTextCtrl() : streambuf()
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
    m_acceptsFocus = TRUE;

    PreCreation( parent, id, pos, size, style, name );

    SetValidator( validator );
    
    m_vScrollbarVisible = TRUE;

    bool multi_line = (style & wxTE_MULTILINE) != 0;
    if ( multi_line )
    {
        // a multi-line edit control: create a vertical scrollbar by default and
        // horizontal if requested
        bool bHasHScrollbar = (style & wxHSCROLL) != 0;
 
        // create our control...
        m_text = gtk_text_new( (GtkAdjustment *) NULL, (GtkAdjustment *) NULL );

        // ... and put into the upper left hand corner of the table
        m_widget = gtk_table_new(bHasHScrollbar ? 2 : 1, 2, FALSE);
        gtk_table_attach( GTK_TABLE(m_widget), m_text, 0, 1, 0, 1,
                      (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                      (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                       0, 0);

        // put the horizontal scrollbar in the lower left hand corner
        if (bHasHScrollbar)
        {
            GtkWidget *hscrollbar = gtk_hscrollbar_new(GTK_TEXT(m_text)->hadj);
            gtk_table_attach(GTK_TABLE(m_widget), hscrollbar, 0, 1, 1, 2,
                       (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                       GTK_FILL,
                       0, 0);
            gtk_widget_show(hscrollbar);
        }

        // finally, put the vertical scrollbar in the upper right corner
        m_vScrollbar = gtk_vscrollbar_new( GTK_TEXT(m_text)->vadj );
        gtk_table_attach(GTK_TABLE(m_widget), m_vScrollbar, 1, 2, 0, 1,
                     GTK_FILL,
                     (GtkAttachOptions)(GTK_EXPAND | GTK_FILL | GTK_SHRINK),
                     0, 0);
        gtk_widget_show( m_vScrollbar );
	
        gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
          GTK_SIGNAL_FUNC(gtk_text_size_callback), (gpointer)this );
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

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );

    PostCreation();

    if (multi_line)
    {
        gtk_widget_realize(m_text);
        gtk_widget_show(m_text);
    }

    // we want to be notified about text changes
    gtk_signal_connect( GTK_OBJECT(m_text), "changed",
      GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this);

    if (!value.IsNull())
    {
        gint tmp = 0;
        gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &tmp );
        SetInsertionPointEnd();
    }

    if (style & wxTE_PASSWORD)
    {
        if (!multi_line)
            gtk_entry_set_visibility( GTK_ENTRY(m_text), FALSE );
    }

    if (style & wxTE_READONLY)
    {
        if (!multi_line)
            gtk_entry_set_editable( GTK_ENTRY(m_text), FALSE );
    }
    else
    {
        if (multi_line)
            gtk_text_set_editable( GTK_TEXT(m_text), 1 );
    }
    
    Show( TRUE );
    
    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    return TRUE;
}

void wxTextCtrl::CalculateScrollbar()
{
    if ((m_windowStyle & wxTE_MULTILINE) == 0) return;

    GtkAdjustment *adj = GTK_TEXT(m_text)->vadj;
    
    if (adj->upper - adj->page_size < 0.8)
    {
        if (m_vScrollbarVisible)
        {
	    gtk_widget_hide( m_vScrollbar );
	
	    m_vScrollbarVisible = FALSE;
        }
    }
    else
    {
        if (!m_vScrollbarVisible)
        {
	    gtk_widget_show( m_vScrollbar );
	    
	    m_vScrollbarVisible = TRUE;
        }
    }
}

wxString wxTextCtrl::GetValue() const
{
    wxCHECK_MSG( m_text != NULL, "", "invalid text ctrl" );

    wxString tmp;
    if (m_windowStyle & wxTE_MULTILINE)
    {
        gint len = gtk_text_get_length( GTK_TEXT(m_text) );
        char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );
        tmp = text;
        g_free( text );
    }
    else
    {
        tmp = gtk_entry_get_text( GTK_ENTRY(m_text) );
    }
    return tmp;
}

void wxTextCtrl::SetValue( const wxString &value )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

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
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

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

bool wxTextCtrl::LoadFile( const wxString &file )
{
  wxCHECK_MSG( m_text != NULL, FALSE, "invalid text ctrl" );

  if (!wxFileExists(file)) return FALSE;

  Clear();

  FILE *fp = NULL;
  struct stat statb;

  if ((stat ((char*) (const char*) file, &statb) == -1) || (statb.st_mode & S_IFMT) != S_IFREG ||
      !(fp = fopen ((char*) (const char*) file, "r")))
  {
      return FALSE;
  }
  else
  {
    gint len = statb.st_size;
    char *text;
    if (!(text = (char*)malloc ((unsigned) (len + 1))))
    {
      fclose (fp);
      return FALSE;
    }
    if (fread (text, sizeof (char), len, fp) != (size_t) len)
	{
	}
    fclose (fp);

    text[len] = 0;

    if (m_windowStyle & wxTE_MULTILINE)
    {
      gint pos = 0;
      gtk_editable_insert_text( GTK_EDITABLE(m_text), text, len, &pos );
    }
    else
    {
      gtk_entry_set_text( GTK_ENTRY(m_text), text );
    }

    free (text);
    m_modified = FALSE;
    return TRUE;
  }
  return FALSE;
}

bool wxTextCtrl::SaveFile( const wxString &file )
{
  wxCHECK_MSG( m_text != NULL, FALSE, "invalid text ctrl" );

  if (file == "") return FALSE;

  FILE *fp;

  if (!(fp = fopen ((char*) (const char*) file, "w")))
    {
      return FALSE;
    }
  else
    {
      char *text = NULL;
      gint len = 0;

      if (m_windowStyle & wxTE_MULTILINE)
      {
        len = gtk_text_get_length( GTK_TEXT(m_text) );
        text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );
      }
      else
      {
        text = gtk_entry_get_text( GTK_ENTRY(m_text) );
      }

      if (fwrite (text, sizeof (char), len, fp) != (size_t) len)
	{
	  // Did not write whole file
	}

      // Make sure newline terminates the file
      if (text[len - 1] != '\n')
	fputc ('\n', fp);

      fclose (fp);

      if (m_windowStyle & wxTE_MULTILINE) g_free( text );

      m_modified = FALSE;
      return TRUE;
    }

  return TRUE;
}

wxString wxTextCtrl::GetLineText( long lineNo ) const
{
  if (m_windowStyle & wxTE_MULTILINE)
  {
    gint len = gtk_text_get_length( GTK_TEXT(m_text) );
    char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );

    if (text)
    {
        wxString buf("");
        long i;
        int currentLine = 0;
        for (i = 0; currentLine != lineNo && text[i]; i++ )
          if (text[i] == '\n')
            currentLine++;
        // Now get the text
        int j;
        for (j = 0; text[i] && text[i] != '\n'; i++, j++ )
            buf += text[i];

        g_free( text );
        return buf;
    }
    else
      return wxEmptyString;
  }
  else
  {
    if (lineNo == 0) return GetValue();
    return wxEmptyString;
  }
}

void wxTextCtrl::OnDropFiles( wxDropFilesEvent &WXUNUSED(event) )
{
    wxFAIL_MSG( "wxTextCtrl::OnDropFiles not implemented" );
}

long wxTextCtrl::PositionToXY(long pos, long *x, long *y ) const
{
  if (!(m_windowStyle & wxTE_MULTILINE))
    return 0;
  gint len = gtk_text_get_length( GTK_TEXT(m_text) );
  char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );
  if(!text)
    return 0;
  if( pos >= len)
    return pos=len-1;

  *x=1;   // Col 1
  *y=1;   // Line 1
  for (int i = 0; i < pos; i++ )
    {
      if (text[i] == '\n')
	{
	  (*y)++;
	  *x=1;
	}
      else
	(*x)++;
    }
  g_free( text );
  return 1;
}

long wxTextCtrl::XYToPosition(long x, long y ) const
{
    if (!(m_windowStyle & wxTE_MULTILINE)) return 0;
    
    long pos=0;

    for( int i=1; i<y; i++ ) pos += GetLineLength(i);
    
    pos +=x-1; // Pos start with 0
    return pos;
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    wxString str = GetLineText (lineNo);
    return (int) str.Length();
}

int wxTextCtrl::GetNumberOfLines() const
{
    if (m_windowStyle & wxTE_MULTILINE)
    {
        gint len = gtk_text_get_length( GTK_TEXT(m_text) );
        char *text = gtk_editable_get_chars( GTK_EDITABLE(m_text), 0, len );

        if (text)
        {
            int currentLine = 0;
            for (int i = 0; i < len; i++ )
	    {
                if (text[i] == '\n')
                  currentLine++;
	    }
            g_free( text );
            return currentLine;
        }
        else
	{
            return 0;
	}
    }
    else
    {
       return 1;
    }
}

void wxTextCtrl::SetInsertionPoint( long pos )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    int tmp = (int) pos;
    if (m_windowStyle & wxTE_MULTILINE)
        gtk_text_set_point( GTK_TEXT(m_text), tmp );
    else
        gtk_entry_set_position( GTK_ENTRY(m_text), tmp );
}

void wxTextCtrl::SetInsertionPointEnd()
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    int pos = 0;
    if (m_windowStyle & wxTE_MULTILINE)
        pos = gtk_text_get_length( GTK_TEXT(m_text) );
    else
      pos = GTK_ENTRY(m_text)->text_length;
      
    SetInsertionPoint((pos-1)>0 ? (pos-1):0);
}

void wxTextCtrl::SetEditable( bool editable )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    if (m_windowStyle & wxTE_MULTILINE)
        gtk_text_set_editable( GTK_TEXT(m_text), editable );
    else
        gtk_entry_set_editable( GTK_ENTRY(m_text), editable );
}

void wxTextCtrl::SetSelection( long from, long to )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    gtk_editable_select_region( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::ShowPosition( long WXUNUSED(pos) )
{
    wxFAIL_MSG( "wxTextCtrl::ShowPosition not implemented" );
}

long wxTextCtrl::GetInsertionPoint() const
{
    wxCHECK_MSG( m_text != NULL, 0, "invalid text ctrl" );

    return (long) GTK_EDITABLE(m_text)->current_pos;
}

long wxTextCtrl::GetLastPosition() const
{
    wxCHECK_MSG( m_text != NULL, 0, "invalid text ctrl" );

    int pos = 0;
    if (m_windowStyle & wxTE_MULTILINE)
        pos = gtk_text_get_length( GTK_TEXT(m_text) );
    else
        pos = GTK_ENTRY(m_text)->text_length;
	
    return (long)pos-1;
}

void wxTextCtrl::Remove( long from, long to )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
}

void wxTextCtrl::Replace( long from, long to, const wxString &value )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    gtk_editable_delete_text( GTK_EDITABLE(m_text), (gint)from, (gint)to );
    if (value.IsNull()) return;
    gint pos = (gint)from;
    gtk_editable_insert_text( GTK_EDITABLE(m_text), value, value.Length(), &pos );
}

void wxTextCtrl::Cut()
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

#if (GTK_MINOR_VERSION == 1)
    gtk_editable_cut_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_cut_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

void wxTextCtrl::Copy()
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

#if (GTK_MINOR_VERSION == 1)
    gtk_editable_copy_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_copy_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

void wxTextCtrl::Paste()
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

#if (GTK_MINOR_VERSION == 1)
    gtk_editable_paste_clipboard( GTK_EDITABLE(m_text) );
#else
    gtk_editable_paste_clipboard( GTK_EDITABLE(m_text), 0 );
#endif
}

void wxTextCtrl::Clear()
{
    SetValue( "" );
}

void wxTextCtrl::OnChar( wxKeyEvent &key_event )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );
    
    if ((key_event.KeyCode() == WXK_RETURN) && (m_windowStyle & wxPROCESS_ENTER))
    {
        wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
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

int wxTextCtrl::sync()
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

int wxTextCtrl::underflow()
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

GtkWidget* wxTextCtrl::GetConnectWidget()
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

void wxTextCtrl::SetFont( const wxFont &WXUNUSED(font) )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    // doesn't work
}

void wxTextCtrl::SetForegroundColour( const wxColour &WXUNUSED(colour) )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    // doesn't work
}

void wxTextCtrl::SetBackgroundColour( const wxColour &colour )
{
    wxCHECK_RET( m_text != NULL, "invalid text ctrl" );

    wxControl::SetBackgroundColour( colour );

    wxColour sysbg = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE );
    if (sysbg.Red() == colour.Red() && 
        sysbg.Green() == colour.Green() && 
        sysbg.Blue() == colour.Blue())
    {
        return;
    } 
    
    if (!m_backgroundColour.Ok()) return;

    if (m_windowStyle & wxTE_MULTILINE)
    {
        GdkWindow *window = GTK_TEXT(m_text)->text_area;
        m_backgroundColour.CalcPixel( gdk_window_get_colormap( window ) );
        gdk_window_set_background( window, m_backgroundColour.GetColor() );
        gdk_window_clear( window );
    }
}

void wxTextCtrl::ApplyWidgetStyle()
{
    if (m_windowStyle & wxTE_MULTILINE)
    {
        // how ?
    } 
    else
    {
        SetWidgetStyle();
        gtk_widget_set_style( m_text, m_widgetStyle );
    }
}

