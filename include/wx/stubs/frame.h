/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

#ifdef __GNUG__
#pragma interface "frame.h"
#endif

#include "wx/window.h"
#include "wx/toolbar.h"

#if wxUSE_ACCEL
#include "wx/accel.h"
#endif

WXDLLEXPORT_DATA(extern const char*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxStatusBar;

class WXDLLEXPORT wxFrame: public wxWindow {

  DECLARE_DYNAMIC_CLASS(wxFrame)

public:
  wxFrame();
  inline wxFrame(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxFrame();

  bool Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr);

  virtual bool Destroy();
  void SetClientSize(int width, int height);
  void GetClientSize(int *width, int *height) const;

  void GetSize(int *width, int *height) const ;
  void GetPosition(int *x, int *y) const ;
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void ClientToScreen(int *x, int *y) const;
  void ScreenToClient(int *x, int *y) const;

  void OnSize(wxSizeEvent& event);
  void OnMenuHighlight(wxMenuEvent& event);
  void OnActivate(wxActivateEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  bool Show(bool show);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);
  virtual wxMenuBar *GetMenuBar() const ;

  // Set title
  void SetTitle(const wxString& title);
  wxString GetTitle() const ;

  void Centre(int direction = wxBOTH);

  // Call this to simulate a menu command
  virtual void Command(int id);
  virtual void ProcessCommand(int id);

  // Set icon
  virtual void SetIcon(const wxIcon& icon);

  // Create status line
  virtual wxStatusBar* CreateStatusBar(int number=1, long style = wxST_SIZEGRIP, wxWindowID id = 0,
    const wxString& name = "statusBar");
  inline wxStatusBar *GetStatusBar() const { return m_frameStatusBar; }
  virtual void PositionStatusBar();
  virtual wxStatusBar *OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name);

  // Create toolbar
  virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1, const wxString& name = wxToolBarNameStr);
  virtual wxToolBar *OnCreateToolBar(long style, wxWindowID id, const wxString& name);
  // If made known to the frame, the frame will manage it automatically.
  virtual inline void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }
  virtual inline wxToolBar *GetToolBar() const { return m_frameToolBar; }
  virtual void PositionToolBar();

  // Set status line text
  virtual void SetStatusText(const wxString& text, int number = 0);

  // Set status line widths
  virtual void SetStatusWidths(int n, const int widths_field[]);

  // Hint to tell framework which status bar to use
  // TODO: should this go into a wxFrameworkSettings class perhaps?
  static void UseNativeStatusBar(bool useNative) { m_useNativeStatusBar = useNative; };
  static bool UsesNativeStatusBar() { return m_useNativeStatusBar; };

  // Fit frame around subwindows
  virtual void Fit();

  // Iconize
  virtual void Iconize(bool iconize);

  virtual bool IsIconized() const ;

  // Compatibility
  inline bool Iconized() const { return IsIconized(); }

  // Is the frame maximized?
  virtual bool IsMaximized(void) const ;

  virtual void Maximize(bool maximize);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  // Query app for menu item updates (called from OnIdle)
  void DoMenuUpdates();
  void DoMenuUpdates(wxMenu* menu, wxWindow* focusWin);

  // Checks if there is a toolbar, and returns the first free client position
  virtual wxPoint GetClientAreaOrigin() const;

protected:
  wxMenuBar *           m_frameMenuBar;
  wxStatusBar *         m_frameStatusBar;
  wxIcon                m_icon;
  bool                  m_iconized;
  static bool           m_useNativeStatusBar;
  wxToolBar *           m_frameToolBar ;

  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_FRAME_H_
