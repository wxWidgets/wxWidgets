/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes.
//              This doesn't have to be implemented just like Windows,
//              it could be a tabbed design as in wxGTK.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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

  wxMDIParentFrame();
  inline wxMDIParentFrame(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,  // Scrolling refers to client window
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIParentFrame();

  bool Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
           const wxString& name = wxFrameNameStr);

  void OnSize(wxSizeEvent& event);
  void OnActivate(wxActivateEvent& event);
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  void SetMenuBar(wxMenuBar *menu_bar);

  // Gets the size available for subwindows after menu size, toolbar size
  // and status bar size have been subtracted. If you want to manage your own
  // toolbar(s), don't call SetToolBar.
  void GetClientSize(int *width, int *height) const;

  // Get the active MDI child window (Windows only)
  wxMDIChildFrame *GetActiveChild() const ;

  // Get the client window
  inline wxMDIClientWindow *GetClientWindow() const { return m_clientWindow; };

  // Create the client window class (don't Create the window,
  // just return a new class)
  virtual wxMDIClientWindow *OnCreateClient() ;

  // MDI operations
  virtual void Cascade();
  virtual void Tile();
  virtual void ArrangeIcons();
  virtual void ActivateNext();
  virtual void ActivatePrevious();

protected:

  // TODO maybe have this member
  wxMDIClientWindow     *m_clientWindow;

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
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMDIChildFrame();

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
  virtual void Maximize();
  virtual void Restore();
  virtual void Activate();
private:
  // Supress VA's hidden function warning
  void Maximize(bool maximize) {wxFrame::Maximize(maximize);}
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

  ~wxMDIClientWindow();

  // Note: this is virtual, to allow overridden behaviour.
  virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL);

  // Explicitly call default scroll behaviour
  void OnScroll(wxScrollEvent& event);

protected:

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MDI_H_
