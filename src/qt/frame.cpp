/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "frame.h"
#endif

#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/control.h"
#include "wx/app.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/mdi.h"

const wxMENU_HEIGHT    = 28;
const wxSTATUS_HEIGHT  = 25;

extern wxList wxTopLevelWindows;
extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_SIZE(wxFrame::OnSize)
  EVT_CLOSE(wxFrame::OnCloseWindow)
  EVT_IDLE(wxFrame::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame,wxWindow)

wxFrame::wxFrame()
{
  wxTopLevelWindows.Insert( this );
};

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  Create( parent, id, title, pos, size, style, name );
  wxTopLevelWindows.Insert( this );
};

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_title = title;

  return TRUE;
};

wxFrame::~wxFrame()
{
  if (m_frameMenuBar) delete m_frameMenuBar;
  if (m_frameStatusBar) delete m_frameStatusBar;

  wxTopLevelWindows.DeleteObject( this );
  if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
};

bool wxFrame::Show( bool show )
{
  if (show)
  {
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    ProcessEvent( event );
  };
  return wxWindow::Show( show );
};

void wxFrame::Enable( bool enable )
{
  wxWindow::Enable( enable );
};

void wxFrame::OnCloseWindow( wxCloseEvent &event )
{
    if ( GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
};

bool wxFrame::Destroy()
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
}

void wxFrame::GetClientSize( int *width, int *height ) const
{
  wxWindow::GetClientSize( width, height );
  if (height)
  {
    if (m_frameMenuBar) (*height) -= wxMENU_HEIGHT;
    if (m_frameStatusBar) (*height) -= wxSTATUS_HEIGHT;
    if (m_frameToolBar)
    {
      int y = 0;
      m_frameToolBar->GetSize( NULL, &y );
      (*height) -= y;
    }
  };
};


void wxFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  if ( GetAutoLayout() )
    Layout();
  else {
    // no child: go out !
    if (!GetChildren()->First())
      return;
      
    // do we have exactly one child?
    wxWindow *child = NULL;
    for(wxNode *node = GetChildren()->First(); node; node = node->Next())
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!IS_KIND_OF(win,wxFrame) && !IS_KIND_OF(win,wxDialog)
#if 0  // not in m_children anyway
          && (win != m_frameMenuBar) &&
             (win != m_frameToolBar) &&
             (win != m_frameStatusBar)
#endif
         )
      {
        if ( child )  // it's the second one: do nothing
          return;

        child = win;
      };
    };

    // yes: set it's size to fill all the frame
    int client_x, client_y;
    GetClientSize(&client_x, &client_y);
    child->SetSize( 1, 1, client_x-2, client_y);
  }
};

static void SetInvokingWindow( wxMenu *menu, wxWindow *win )
{
  menu->SetInvokingWindow( win );
  wxNode *node = menu->m_items.First();
  while (node)
  {
    wxMenuItem *menuitem = (wxMenuItem*)node->Data();
    if (menuitem->IsSubMenu())
      SetInvokingWindow( menuitem->GetSubMenu(), win );
    node = node->Next();
  };
};

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
  m_frameMenuBar = menuBar;
  
  if (m_frameMenuBar)
  {
    if (m_frameMenuBar->m_parent != this)
    {
      wxNode *node = m_frameMenuBar->m_menus.First();
      while (node)
      {
        wxMenu *menu = (wxMenu*)node->Data();
        SetInvokingWindow( menu, this );
        node = node->Next();
      };
  
    }
  }
};

wxMenuBar *wxFrame::GetMenuBar(void)
{
  return m_frameMenuBar;
};

wxToolBar *wxFrame::CreateToolBar( long style , wxWindowID id, const wxString& name )
{
  m_frameToolBar = new wxToolBar( this, id, wxDefaultPosition, wxDefaultSize, style, name );
  
  return m_frameToolBar;
};

wxToolBar *wxFrame::GetToolBar(void)
{
  return m_frameToolBar;
};

wxStatusBar* wxFrame::CreateStatusBar( int number, long style, wxWindowID id, const wxString& name )
{
  if (m_frameStatusBar)
  delete m_frameStatusBar;

  m_frameStatusBar = new wxStatusBar( this, id, wxPoint(0,0), wxSize(100,20), style, name );

  m_frameStatusBar->SetFieldsCount( number );
  
  return m_frameStatusBar;
};

void wxFrame::SetStatusText( const wxString &text, int number )
{
  if (m_frameStatusBar) m_frameStatusBar->SetStatusText( text, number );
};

void wxFrame::SetStatusWidths( int n, int *width )
{
  if (m_frameStatusBar) m_frameStatusBar->SetStatusWidths( n, width );
};

wxStatusBar *wxFrame::GetStatusBar(void)
{
  return m_frameStatusBar;
};

void wxFrame::SetTitle( const wxString &title )
{
  m_title = title;
};

void wxFrame::SetSizeHints( int WXUNUSED(minW), int WXUNUSED(minH), 
                            int WXUNUSED(maxW), int WXUNUSED(maxH), int WXUNUSED(incW) )
{
}

void wxFrame::SetIcon( const wxIcon &icon )
{
  m_icon = icon;
  if (!icon.Ok()) return;
  
}

