/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_
#define _WX_MDI_H_

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
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIParentFrame(void);

  bool Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr);

/*
#if WXWIN_COMPATIBILITY
  virtual void OldOnActivate(bool flag);
  virtual void OldOnSize(int x, int y);
#endif
*/

  void OnSize(wxSizeEvent& event);
  void OnActivate(wxActivateEvent& event);

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
  void MSWOnSize(int x, int y, WXUINT flag);
  bool MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control);
  void MSWOnMenuHighlight(WXWORD item, WXWORD flags, WXHMENU sysmenu);
  bool MSWProcessMessage(WXMSG *msg);
  bool MSWTranslateMessage(WXMSG *msg);
  void MSWOnCreate(WXLPCREATESTRUCT cs);
  long MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  bool MSWOnEraseBkgnd(WXHDC pDC);
  bool MSWOnDestroy(void);
  bool MSWOnActivate(int state, bool minimized, WXHWND activate);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

 protected:
    wxMDIClientWindow *             m_clientWindow;
    wxMDIChildFrame *               m_currentChild;
    WXHMENU                         m_windowMenu;
    bool                            m_parentFrameActive; // TRUE if MDI Frame is intercepting
                                                         // commands, not child
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
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIChildFrame(void);

  bool Create(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);
  void SetClientSize(int width, int height);
  void GetPosition(int *x, int *y) const ;

  // MDI operations
  virtual void Maximize(void);
  virtual void Restore(void);
  virtual void Activate(void);

  // Handlers

    long MSWOnMDIActivate(long bActivate, WXHWND, WXHWND);
    void MSWOnSize(int x, int y, WXUINT);
    void MSWOnWindowPosChanging(void *lpPos);
    bool MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control);
    long MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    bool MSWProcessMessage(WXMSG *msg);
    bool MSWTranslateMessage(WXMSG *msg);
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
  inline wxMDIClientWindow(wxMDIParentFrame *parent, long style = 0)
  {
      CreateClient(parent, style);
  }

  ~wxMDIClientWindow(void);

  // Note: this is virtual, to allow overridden behaviour.
  virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL);

  // Explicitly call default scroll behaviour
  void OnScroll(wxScrollEvent& event);

  // Window procedure
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  // Calls an appropriate default window procedure
  virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  // Should hand the message to the default proc
  long MSWOnMDIActivate(long bActivate, WXHWND, WXHWND);

protected:
    int m_scrollX;
    int m_scrollY;
DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MDI_H_
