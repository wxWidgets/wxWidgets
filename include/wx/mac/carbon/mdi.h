/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes.
//              This doesn't have to be implemented just like Windows,
//              it could be a tabbed design as in wxGTK.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_
#define _WX_MDI_H_

#include "wx/frame.h"

WXDLLEXPORT_DATA(extern const wxChar) wxFrameNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxStatusLineNameStr[];

class WXDLLEXPORT wxMDIClientWindow;
class WXDLLEXPORT wxMDIChildFrame;

class WXDLLEXPORT wxMDIParentFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)

public:

  wxMDIParentFrame() { Init(); }
  wxMDIParentFrame(wxWindow *parent,
                   wxWindowID id,
                   const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,  // Scrolling refers to client window
                   const wxString& name = wxFrameNameStr)
  {
      Init();
      Create(parent, id, title, pos, size, style, name);
  }

  virtual ~wxMDIParentFrame();

  bool Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr);

  // Mac OS activate event
  virtual void MacActivate(long timestamp, bool activating);

  // wxWidgets activate event
  void OnActivate(wxActivateEvent& event);
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  void SetMenuBar(wxMenuBar *menu_bar);

  // Get the active MDI child window (Windows only)
  wxMDIChildFrame *GetActiveChild() const ;

  // Get the client window
  inline wxMDIClientWindow *GetClientWindow() const { return m_clientWindow; };
  // Get rect to be used to center top-level children
  virtual void GetRectForTopLevelChildren(int *x, int *y, int *w, int *h);

  // Create the client window class (don't Create the window,
  // just return a new class)
  virtual wxMDIClientWindow *OnCreateClient() ;

  // MDI operations
  virtual void Cascade();
  virtual void Tile(wxOrientation WXUNUSED(orient) = wxHORIZONTAL);
  virtual void ArrangeIcons();
  virtual void ActivateNext();
  virtual void ActivatePrevious();

  virtual bool Show( bool show = true );

  // overridden base clas virtuals
  virtual void AddChild(wxWindowBase *child);
  virtual void RemoveChild(wxWindowBase *child);

protected:
    // common part of all ctors
    void Init();

    // returns true if this frame has some contents and so should be visible,
    // false if it's used solely as container for its children
    bool ShouldBeVisible() const;


    // TODO maybe have this member
    wxMDIClientWindow *m_clientWindow;
    wxMDIChildFrame *m_currentChild;
    wxMenu *m_windowMenu;

    // true if MDI Frame is intercepting commands, not child
    bool m_parentFrameActive;

    // true if the frame should be shown but is not because it is empty and
    // useless otherwise than a container for its children
    bool m_shouldBeShown;

private:
    friend class WXDLLEXPORT wxMDIChildFrame;
    DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxMDIChildFrame: public wxFrame
{
DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
public:

  wxMDIChildFrame();
  inline wxMDIChildFrame(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Init() ;
      Create(parent, id, title, pos, size, style, name);
  }

  virtual ~wxMDIChildFrame();

  bool Create(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr);

  // Mac OS activate event
  virtual void MacActivate(long timestamp, bool activating);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);

  // MDI operations
  virtual void Maximize();
  virtual void Maximize( bool ){ Maximize() ; } // this one is inherited from wxFrame
  virtual void Restore();
  virtual void Activate();
protected:

    // common part of all ctors
    void Init();
};

/* The client window is a child of the parent MDI frame, and itself
 * contains the child MDI frames.
 * However, you create the MDI children as children of the MDI parent:
 * only in the implementation does the client window become the parent
 * of the children. Phew! So the children are sort of 'adopted'...
 */

class WXDLLEXPORT wxMDIClientWindow: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
 public:

  wxMDIClientWindow() ;
  inline wxMDIClientWindow(wxMDIParentFrame *parent, long style = 0)
  {
      CreateClient(parent, style);
  }

  virtual ~wxMDIClientWindow();

  // Note: this is virtual, to allow overridden behaviour.
  virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL);

  // Explicitly call default scroll behaviour
  void OnScroll(wxScrollEvent& event);

protected:
    // Gets the size available for subwindows after menu size, toolbar size
    // and status bar size have been subtracted. If you want to manage your own
    // toolbar(s), don't call SetToolBar.
    void DoGetClientSize(int *width, int *height) const;

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MDI_H_
