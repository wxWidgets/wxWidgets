/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_
#define _WX_MDI_H_

#ifdef __GNUG__
#pragma interface "mdi.h"
#endif

/*
New MDI scheme using tabs. We can use a wxNotebook to implement the client
window. wxMDIChildFrame can be implemented as an XmMainWindow widget
as before, and is a child of the notebook _and_ of the parent frame...
but wxMDIChildFrame::GetParent should return the parent frame.

*/

#include "wx/frame.h"
#include "wx/notebook.h"

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
  wxSize GetClientSize() const { return wxWindow::GetClientSize(); }

  // Get the active MDI child window
  wxMDIChildFrame *GetActiveChild() const ;

  // Get the client window
  wxMDIClientWindow *GetClientWindow() const { return m_clientWindow; };

  // Create the client window class (don't Create the window,
  // just return a new class)
  virtual wxMDIClientWindow *OnCreateClient() ;

  // MDI operations
  virtual void Cascade();
  virtual void Tile();
  virtual void ArrangeIcons();
  virtual void ActivateNext();
  virtual void ActivatePrevious();

// Implementation

  // Set the active child
  inline void SetActiveChild(wxMDIChildFrame* child) { m_activeChild = child; }

  // Set the child's menubar into the parent frame
  void SetChildMenuBar(wxMDIChildFrame* frame);

  inline wxMenuBar* GetActiveMenuBar() const { return m_activeMenuBar; }

  // Redirect events to active child first
  virtual bool ProcessEvent(wxEvent& event);

protected:
  virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
  virtual void DoSetClientSize(int width, int height);


protected:

  wxMDIClientWindow*    m_clientWindow;
  wxMDIChildFrame*      m_activeChild;
  wxMenuBar*            m_activeMenuBar;

DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxMDIChildFrame: public wxFrame
{
DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)

public:
    wxMDIChildFrame();
    wxMDIChildFrame(wxMDIParentFrame *parent,
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
    void SetTitle(const wxString& title);

    void SetClientSize(int width, int height);
    void GetClientSize(int *width, int *height) const;
    void GetSize(int *width, int *height) const;
    void GetPosition(int *x, int *y) const ;

    // Set icon
    virtual void SetIcon(const wxIcon& icon);

    // Override wxFrame operations
    void CaptureMouse();
    void ReleaseMouse();
    void Raise();
    void Lower(void);
    void SetSizeHints(int minW = -1, int minH = -1, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1);

    // MDI operations
    virtual void Maximize();
    virtual void Maximize(bool WXUNUSED(maximize)) { };
    inline void Minimize() { Iconize(TRUE); };
    virtual void Iconize(bool iconize);
    virtual void Restore();
    virtual void Activate();
    virtual bool IsIconized() const ;

    // Is the frame maximized? Returns TRUE for
    // wxMDIChildFrame due to the tabbed implementation.
    virtual bool IsMaximized(void) const ;

    bool Show(bool show);

    WXWidget GetMainWidget() const { return m_mainWidget; };
    WXWidget GetTopWidget() const { return m_mainWidget; };
    WXWidget GetClientWidget() const { return m_mainWidget; };

    /*
       virtual void OnRaise();
       virtual void OnLower();
     */

    void SetMDIParentFrame(wxMDIParentFrame* parentFrame) { m_mdiParentFrame = parentFrame; }
    wxMDIParentFrame* GetMDIParentFrame() const { return m_mdiParentFrame; }

protected:
    wxMDIParentFrame* m_mdiParentFrame;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);
};

/* The client window is a child of the parent MDI frame, and itself
 * contains the child MDI frames.
 * However, you create the MDI children as children of the MDI parent:
 * only in the implementation does the client window become the parent
 * of the children. Phew! So the children are sort of 'adopted'...
 */

class WXDLLEXPORT wxMDIClientWindow: public wxNotebook
{
DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)

public:
    wxMDIClientWindow() ;
    wxMDIClientWindow(wxMDIParentFrame *parent, long style = 0)
    {
        CreateClient(parent, style);
    }

    ~wxMDIClientWindow();

    void GetClientSize(int *width, int *height) const;
    void GetSize(int *width, int *height) const ;
    void GetPosition(int *x, int *y) const ;

    // Note: this is virtual, to allow overridden behaviour.
    virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL);

    // Explicitly call default scroll behaviour
    void OnScroll(wxScrollEvent& event);

    // Implementation
    void OnPageChanged(wxNotebookEvent& event);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MDI_H_
