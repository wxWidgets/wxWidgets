/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __MDIH__
#define __MDIH__

#ifdef __GNUG__
#pragma interface "mdi.h"
#endif

#include "wx/frame.h"

WXDLLEXPORT_DATA(extern const char*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const char*) wxStatusLineNameStr;

class WXDLLEXPORT wxMDIClientWindow;
class WXDLLEXPORT wxMDIChildFrame;

class WXDLLEXPORT wxMDIParentFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)

  friend class WXDLLEXPORT wxMDIChildFrame;
 public:

  wxMDIParentFrame(void);
  inline wxMDIParentFrame(wxWindow *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIParentFrame(void);

  bool Create(wxWindow *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr);

#if WXWIN_COMPATIBILITY
  virtual void OldOnActivate(bool flag);
  virtual void OldOnSize(int x, int y);
#endif

  void OnSize(wxSizeEvent& event);
  void OnActivate(wxActivateEvent& event);

  // Toolbar (currently, for use by Windows MDI parent frames ONLY)
  virtual inline void SetToolBar(wxWindow *toolbar) { m_frameToolBar = toolbar; }
  virtual inline wxWindow *GetToolBar(void) const { return m_frameToolBar; }

  void SetMenuBar(wxMenuBar *menu_bar);

  // Gets the size available for subwindows after menu size, toolbar size
  // and status bar size have been subtracted. If you want to manage your own
  // toolbar(s), don't call SetToolBar.
  void GetClientSize(int *width, int *height) const;

  // Get the active MDI child window (Windows only)
  wxMDIChildFrame *GetActiveChild(void) const ;

  // Get the client window
  inline wxMDIClientWindow *GetClientWindow(void) const ;

  // Create the client window class (don't Create the window,
  // just return a new class)
  virtual wxMDIClientWindow *OnCreateClient(void) ;

  inline WXHMENU GetWindowMenu(void) const ;

  // MDI operations
  virtual void Cascade(void);
  virtual void Tile(void);
  virtual void ArrangeIcons(void);
  virtual void ActivateNext(void);
  virtual void ActivatePrevious(void);

  // Handlers
  void MSWOnSize(const int x, const int y, const WXUINT flag);
  bool MSWOnCommand(const WXWORD id, const WXWORD cmd, const WXHWND control);
  void MSWOnMenuHighlight(const WXWORD item, const WXWORD flags, const WXHMENU sysmenu);
  bool MSWProcessMessage(WXMSG *msg);
  void MSWOnCreate(WXLPCREATESTRUCT cs);
  long MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  bool MSWOnEraseBkgnd(const WXHDC pDC);
  bool MSWOnDestroy(void);
  bool MSWOnActivate(const int state, const bool minimized, const WXHWND activate);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

 protected:
    wxMDIClientWindow *             m_clientWindow;
    wxMDIChildFrame *               m_currentChild;
    WXHMENU                         m_windowMenu;
    bool                            m_parentFrameActive; // TRUE if MDI Frame is intercepting
                                                         // commands, not child
    wxWindow *                      m_frameToolBar ;
DECLARE_EVENT_TABLE()
};

// Inlines
inline wxMDIClientWindow *wxMDIParentFrame::GetClientWindow(void) const { return m_clientWindow; }
inline WXHMENU wxMDIParentFrame::GetWindowMenu(void) const { return m_windowMenu; }

class WXDLLEXPORT wxMDIChildFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
 public:

  wxMDIChildFrame(void);
  inline wxMDIChildFrame(wxMDIParentFrame *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIChildFrame(void);

  bool Create(wxMDIParentFrame *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);
  void SetClientSize(const int width, const int height);
  void GetPosition(int *x, int *y) const ;

  // MDI operations
  virtual void Maximize(void);
  virtual void Restore(void);
  virtual void Activate(void);

  // Handlers

    long MSWOnMDIActivate(const long bActivate, const WXHWND, const WXHWND);
    void MSWOnSize(const int x, const int y, const WXUINT);
    void MSWOnWindowPosChanging(void *lpPos);
    bool MSWOnCommand(const WXWORD id, const WXWORD cmd, const WXHWND control);
    long MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    bool MSWProcessMessage(WXMSG *msg);
    void MSWDestroyWindow(void);

    // Implementation
    bool ResetWindowStyle(void *vrect);
 protected:
//    bool            m_active;
};

class WXDLLEXPORT wxMDIClientWindow: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
 public:

  wxMDIClientWindow(void) ;
  inline wxMDIClientWindow(wxMDIParentFrame *parent, const long style = 0)
  {
      CreateClient(parent, style);
  }

  ~wxMDIClientWindow(void);

  // Note: this is virtual, to allow overridden behaviour.
  virtual bool CreateClient(wxMDIParentFrame *parent, const long style = wxVSCROLL | wxHSCROLL);

  // Explicitly call default scroll behaviour
  void OnScroll(wxScrollEvent& event);

  // Window procedure
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  // Calls an appropriate default window procedure
  virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  // Should hand the message to the default proc
  long MSWOnMDIActivate(const long bActivate, const WXHWND, const WXHWND);

protected:
    int m_scrollX;
    int m_scrollY;
DECLARE_EVENT_TABLE()
};

#endif
    // __MDIH__
