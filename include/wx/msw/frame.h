/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __FRAMEH__
#define __FRAMEH__

#ifdef __GNUG__
#pragma interface "frame.h"
#endif

#include "wx/window.h"

WXDLLEXPORT_DATA(extern const char*) wxFrameNameStr;

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxStatusBar;

class WXDLLEXPORT wxFrame: public wxWindow {

  DECLARE_DYNAMIC_CLASS(wxFrame)

public:
  wxFrame(void);
  inline wxFrame(wxWindow *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxFrame(void);

  bool Create(wxWindow *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr);

#if WXWIN_COMPATIBILITY
  // The default thing is to set the focus for the first child window.
  // Override for your own behaviour.
  virtual void OldOnActivate(bool flag);

  // Default behaviour is to display a help string for the menu item.
  virtual void OldOnMenuSelect(int id);

  inline virtual void OldOnMenuCommand(int WXUNUSED(id)) {};         // Called on frame menu command
  void OldOnSize(int x, int y);
#endif

  virtual bool Destroy(void);
  void SetClientSize(const int width, const int height);
  void GetClientSize(int *width, int *height) const;

  void GetSize(int *width, int *height) const ;
  void GetPosition(int *x, int *y) const ;
  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);

  void OnSize(wxSizeEvent& event);
  void OnMenuHighlight(wxMenuEvent& event);
  void OnActivate(wxActivateEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  bool Show(const bool show);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);
  virtual wxMenuBar *GetMenuBar(void) const ;

  // Set title
  void SetTitle(const wxString& title);
  wxString GetTitle(void) const ;

  void Centre(const int direction = wxBOTH);

  // Call this to simulate a menu command
  virtual void Command(int id);
  virtual void ProcessCommand(int id);

  // Set icon
  virtual void SetIcon(const wxIcon& icon);

  // Create status line
  virtual bool CreateStatusBar(const int number=1);
  inline wxStatusBar *GetStatusBar() const { return m_frameStatusBar; }

  // Set status line text
  virtual void SetStatusText(const wxString& text, const int number = 0);

  // Set status line widths
  virtual void SetStatusWidths(const int n, const int *widths_field);

  // Hint to tell framework which status bar to use
  // TODO: should this go into a wxFrameworkSettings class perhaps?
  static void UseNativeStatusBar(bool useNative) { m_useNativeStatusBar = useNative; };
  static bool UsesNativeStatusBar(void) { return m_useNativeStatusBar; };

  // Fit frame around subwindows
  virtual void Fit(void);

  // Iconize
  virtual void Iconize(const bool iconize);

  virtual bool IsIconized(void) const ;

  // Compatibility
  inline bool Iconized(void) const { return IsIconized(); }

  virtual void Maximize(const bool maximize);
  virtual bool LoadAccelerators(const wxString& table);

  virtual void PositionStatusBar(void);
  virtual wxStatusBar *OnCreateStatusBar(const int number);

  // Query app for menu item updates (called from OnIdle)
  void DoMenuUpdates(void);
  void DoMenuUpdates(wxMenu* menu);

  WXHMENU GetWinMenu(void) const ;

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  // Handlers
  bool MSWOnPaint(void);
  WXHICON MSWOnQueryDragIcon(void);
  void MSWOnSize(const int x, const int y, const WXUINT flag);
  bool MSWOnCommand(const WXWORD id, const WXWORD cmd, const WXHWND control);
  bool MSWOnClose(void);
  void MSWOnMenuHighlight(const WXWORD item, const WXWORD flags, const WXHMENU sysmenu);
  bool MSWProcessMessage(WXMSG *msg);
  void MSWCreate(const int id, wxWindow *parent, const char *WXUNUSED(wclass), wxWindow *wx_win, const char *title,
                   const int x, const int y, const int width, const int height, const long style);

protected:
  wxMenuBar *           m_frameMenuBar;
  wxStatusBar *         m_frameStatusBar;
  wxIcon                m_icon;
  bool                  m_iconized;
  WXHICON               m_defaultIcon;
  static bool           m_useNativeStatusBar;

  DECLARE_EVENT_TABLE()
};

#endif
    // __FRAMEH__
