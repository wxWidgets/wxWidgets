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
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxScrollBar
//-----------------------------------------------------------------------------

static void gtk_scrollbar_callback( GtkWidget *WXUNUSED(widget), wxScrollBar *win )
{ 
  if (!win->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
    
  float diff = win->m_adjust->value - win->m_oldPos;
  if (fabs(diff) < 0.2) return;
  
  wxEventType command = wxEVT_NULL;
  
  float line_step = win->m_adjust->step_increment;
  float page_step = win->m_adjust->page_increment;
  
  if (fabs(diff-line_step) < 0.2) command = wxEVT_SCROLL_LINEDOWN;
  else if (fabs(diff+line_step) < 0.2) command = wxEVT_SCROLL_LINEUP;
  else if (fabs(diff-page_step) < 0.2) command = wxEVT_SCROLL_PAGEDOWN;
  else if (fabs(diff+page_step) < 0.2) command = wxEVT_SCROLL_PAGEUP;
  else command = wxEVT_SCROLL_THUMBTRACK;

  int value = (int)(win->m_adjust->value+0.5);
      
  int orient = wxHORIZONTAL;
  if (win->GetWindowStyleFlag() & wxSB_VERTICAL == wxSB_VERTICAL) orient = wxHORIZONTAL;
  
  wxScrollEvent event( command, win->GetId(), value, orient );
  event.SetEventObject( win );
  win->GetEventHandler()->ProcessEvent( event );
  
/*
  wxCommandEvent cevent( wxEVT_COMMAND_SCROLLBAR_UPDATED, win->GetId() );
  cevent.SetEventObject( win );
  win->ProcessEvent( cevent );
*/
};

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
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_oldPos = 0.0;

  if (style & wxSB_VERTICAL == wxSB_VERTICAL)
    m_widget = gtk_hscrollbar_new( NULL );
  else
    m_widget = gtk_vscrollbar_new( NULL );
    
  m_adjust = gtk_range_get_adjustment( GTK_RANGE(m_widget) );
  
  gtk_signal_connect (GTK_OBJECT (m_adjust), "value_changed",
		      (GtkSignalFunc) gtk_scrollbar_callback, (gpointer) this );
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};

int wxScrollBar::GetPosition(void) const
{
  return (int)(m_adjust->value+0.5);
};

int wxScrollBar::GetThumbSize() const
{
  return (int)(m_adjust->page_size+0.5);
};

int wxScrollBar::GetPageSize() const
{
  return (int)(m_adjust->page_increment+0.5);
};

int wxScrollBar::GetRange() const
{
  return (int)(m_adjust->upper+0.5);
};

void wxScrollBar::SetPosition( int viewStart )
{
  float fpos = (float)viewStart;
  m_oldPos = fpos;
  if (fabs(fpos-m_adjust->value) < 0.2) return;
  m_adjust->value = fpos;
  
  gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "value_changed" );
};

void wxScrollBar::SetScrollbar( int position, int thumbSize, int range, int pageSize,
      bool WXUNUSED(refresh) )
{
  float fpos = (float)position;
  m_oldPos = fpos;
  float frange = (float)range;
  float fthumb = (float)thumbSize;
  float fpage = (float)pageSize;
      
  if ((fabs(fpos-m_adjust->value) < 0.2) &&
      (fabs(frange-m_adjust->upper) < 0.2) &&
      (fabs(fthumb-m_adjust->page_size) < 0.2) &&
      (fabs(fpage-m_adjust->page_increment) < 0.2))
      return;
      
  m_adjust->lower = 0.0;
  m_adjust->upper = frange;
  m_adjust->value = fpos;
  m_adjust->step_increment = 1.0;
  m_adjust->page_increment = (float)(wxMax(fpage-2,0));
  m_adjust->page_size = fthumb;

  gtk_signal_emit_by_name( GTK_OBJECT(m_adjust), "changed" );
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
  int pos = (int)(m_adjust->value+0.5);
  int thumb = (int)(m_adjust->page_size+0.5);
  int page = (int)(m_adjust->page_increment+0.5);
  int range = (int)(m_adjust->upper+0.5);
  
  *viewStart = pos;
  *viewLength = range;
  *objectLength = thumb;
  *pageLength = page;
};

int wxScrollBar::GetViewLength() const
{
  return (int)(m_adjust->upper+0.5);
};

int wxScrollBar::GetObjectLength() const
{
  return (int)(m_adjust->page_size+0.5);
};

void wxScrollBar::SetPageSize( int pageLength )
{
  int pos = (int)(m_adjust->value+0.5);
  int thumb = (int)(m_adjust->page_size+0.5);
  int range = (int)(m_adjust->upper+0.5);
  SetScrollbar( pos, thumb, range, pageLength );
};

void wxScrollBar::SetObjectLength( int objectLength )
{
  int pos = (int)(m_adjust->value+0.5);
  int page = (int)(m_adjust->page_increment+0.5);
  int range = (int)(m_adjust->upper+0.5);
  SetScrollbar( pos, objectLength, range, page );
};

void wxScrollBar::SetViewLength( int viewLength )
{
  int pos = (int)(m_adjust->value+0.5);
  int thumb = (int)(m_adjust->page_size+0.5);
  int page = (int)(m_adjust->page_increment+0.5);
  SetScrollbar( pos, thumb, viewLength, page );
};

