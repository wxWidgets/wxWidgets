/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFRAMEH__
#define __GTKFRAMEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMDIChildFrame;
class wxMDIClientWindow;
class wxMenu;
class wxMenuBar;
class wxToolBar;
class wxStatusBar;

class wxFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxFrameNameStr;
extern const char *wxToolBarNameStr;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

class wxFrame: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxFrame)
public:

  wxFrame();
  wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
  bool Create( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
  ~wxFrame();
  bool Destroy();

  virtual bool Show( bool show );
  virtual void Enable( bool enable );

  virtual void GetClientSize( int *width, int *height ) const;
  virtual void SetClientSize( int const width, int const height );
  
    // set minimal/maxmimal size for the frame
  virtual void SetSizeHints( int minW, int minH, int maxW, int maxH, int incW = -1 );

  virtual wxStatusBar* CreateStatusBar(int number=1, long style = wxST_SIZEGRIP, wxWindowID id = 0,
    const wxString& name = "statusBar");
  virtual wxStatusBar *GetStatusBar();
  virtual void SetStatusText( const wxString &text, int number = 0 );
  virtual void SetStatusWidths( int n, int *width );

  virtual wxToolBar* CreateToolBar( long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1, 
                                    const wxString& name = wxToolBarNameStr);
  virtual wxToolBar *GetToolBar();
  
  virtual void SetMenuBar( wxMenuBar *menuBar );
  virtual wxMenuBar *GetMenuBar();

  virtual void SetTitle( const wxString &title );
  virtual wxString GetTitle() const { return m_title; }
  
  virtual void SetIcon( const wxIcon &icon );

  void OnActivate( wxActivateEvent &WXUNUSED(event) ) { } // called from docview.cpp
  void OnSize( wxSizeEvent &event );
  void OnCloseWindow( wxCloseEvent& event );
  void OnIdle(wxIdleEvent& event);

  virtual void AddChild( wxWindow *child );
  virtual void GtkOnSize( int x, int y, int width, int height );

private:
  friend  wxWindow;
  friend  wxMDIChildFrame;
  friend  wxMDIClientWindow;

  // update frame's menus (called from OnIdle)
  void DoMenuUpdates();
  void DoMenuUpdates(wxMenu* menu);

  GtkWidget    *m_mainWindow;
  wxMenuBar    *m_frameMenuBar;
  wxStatusBar  *m_frameStatusBar;
  wxToolBar    *m_frameToolBar;
  int           m_toolBarHeight;
  bool          m_addPrivateChild;   // for toolbar (and maybe menubar)
  wxString      m_title;
  wxIcon        m_icon;

  DECLARE_EVENT_TABLE()
};

#endif // __GTKFRAMEH__
