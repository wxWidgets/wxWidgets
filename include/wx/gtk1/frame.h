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
#include "wx/menu.h"
#include "wx/statusbr.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMDIChildFrame;

class wxFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxFrameNameStr;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

class wxFrame: public wxWindow
{
public:
  // construction
  wxFrame();
  wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
  bool Create( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
  ~wxFrame();
  bool Destroy();

  // operations
    //
  virtual bool Show( bool show );
  virtual void Enable( bool enable );

    // frame size
  virtual void GetClientSize( int *width, int *height ) const;
    // set minimal size for the frame (@@@ other params not implemented)
  void SetSizeHints(int minW, int minH,
                    int maxW = -1, int maxH = -1,
                    int incW = -1);

    // status bar
  virtual bool CreateStatusBar( int number = 1 );
  wxStatusBar *GetStatusBar();
  virtual void SetStatusText( const wxString &text, int number = 0 );
  virtual void SetStatusWidths( int n, int *width );

    // menu bar
  void SetMenuBar( wxMenuBar *menuBar );
  wxMenuBar *GetMenuBar();

    // frame title
  void SetTitle( const wxString &title );
  wxString GetTitle() const { return m_title; }

  // implementation
  void OnActivate( wxActivateEvent &event ) { } // called from docview.cpp
  void OnSize( wxSizeEvent &event );
  void OnCloseWindow( wxCloseEvent& event );
  void OnIdle(wxIdleEvent& event);

  virtual void GtkOnSize( int x, int y, int width, int height );

private:
  friend  wxWindow;
  friend  wxMDIChildFrame;

  // update frame's menus (called from OnIdle)
  void DoMenuUpdates();
  void DoMenuUpdates(wxMenu* menu);

  GtkWidget    *m_mainWindow;
  wxMenuBar    *m_frameMenuBar;
  wxStatusBar  *m_frameStatusBar;
  bool          m_doingOnSize;
  wxString      m_title;

  DECLARE_DYNAMIC_CLASS(wxFrame)
  DECLARE_EVENT_TABLE()
};

#endif // __GTKFRAMEH__
