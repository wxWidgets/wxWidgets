/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
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
  virtual void Centre( int direction = wxHORIZONTAL );

  virtual void GetClientSize( int *width, int *height ) const;
  virtual void SetClientSize( int const width, int const height );

  virtual void SetSize( int x, int y, int width, int height,
    int sizeFlags = wxSIZE_AUTO );
    
  virtual wxStatusBar* CreateStatusBar(int number=1, long style = wxST_SIZEGRIP, wxWindowID id = 0,
    const wxString& name = "statusBar");
  virtual wxStatusBar *OnCreateStatusBar( int number, long style, wxWindowID id,
    const wxString& name );
  virtual wxStatusBar *GetStatusBar() const;
  inline void SetStatusBar(wxStatusBar *statusBar) { m_frameStatusBar = statusBar; }
  virtual void SetStatusText( const wxString &text, int number = 0 );
  virtual void SetStatusWidths( int n, const int widths_field[] );

  virtual wxToolBar* CreateToolBar( long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1,
                                    const wxString& name = wxToolBarNameStr);
  virtual wxToolBar *OnCreateToolBar( long style, wxWindowID id, const wxString& name );
  virtual wxToolBar *GetToolBar(void) const;
  inline void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }

  virtual void SetMenuBar( wxMenuBar *menuBar );
  virtual wxMenuBar *GetMenuBar() const;

  virtual void SetTitle( const wxString &title );
  virtual wxString GetTitle() const { return m_title; }

  virtual void SetIcon( const wxIcon &icon );
  virtual void Iconize( bool WXUNUSED(iconize)) { }
  virtual bool IsIconized(void) const { return FALSE; }
  bool Iconized(void) const { return IsIconized(); }
  virtual void Maximize(bool WXUNUSED(maximize)) {}
  virtual void Restore(void) {}

  void OnActivate( wxActivateEvent &WXUNUSED(event) ) { } // called from docview.cpp
  void OnSize( wxSizeEvent &event );
  void OnCloseWindow( wxCloseEvent& event );
  void OnIdle(wxIdleEvent& event);

  // implementation
  
  virtual void GtkOnSize( int x, int y, int width, int height );
  virtual wxPoint GetClientAreaOrigin() const;
  void DoMenuUpdates();
  void DoMenuUpdates(wxMenu* menu);

  wxMenuBar    *m_frameMenuBar;
  wxStatusBar  *m_frameStatusBar;
  wxToolBar    *m_frameToolBar;
  wxString      m_title;
  wxIcon        m_icon;

  DECLARE_EVENT_TABLE()
};

#endif // __GTKFRAMEH__
