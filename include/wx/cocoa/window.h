/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/window.h
// Purpose:     wxWindowCocoa
// Author:      David Elliott
// Modified by:
// Created:     2002/12/26
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_WINDOW_H__
#define __WX_COCOA_WINDOW_H__

#include "wx/cocoa/NSView.h"

// ========================================================================
// wxWindowCocoa
// ========================================================================
class WXDLLEXPORT wxWindowCocoa: public wxWindowBase, protected wxCocoaNSView
{
    DECLARE_DYNAMIC_CLASS(wxWindowCocoa)
    DECLARE_NO_COPY_CLASS(wxWindowCocoa)
    DECLARE_EVENT_TABLE()
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxWindowCocoa() { Init(); }
    inline wxWindowCocoa(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, winid, pos, size, style, name);
    }

    virtual ~wxWindowCocoa();

    bool Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxPanelNameStr);
protected:
    void Init();
// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    inline WX_NSView GetNSView() { return m_cocoaNSView; }
    void CocoaAddChild(wxWindowCocoa *child);
    void CocoaRemoveFromParent(void);
protected:
    void InitMouseEvent(wxMouseEvent &event, WX_NSEvent cocoaEvent);
    virtual void Cocoa_FrameChanged(void);
    virtual bool Cocoa_drawRect(const NSRect &rect);
    virtual bool Cocoa_mouseDown(WX_NSEvent theEvent);
    virtual bool Cocoa_mouseDragged(WX_NSEvent theEvent);
    virtual bool Cocoa_mouseUp(WX_NSEvent theEvent);
    virtual bool Cocoa_mouseMoved(WX_NSEvent theEvent);
    virtual bool Cocoa_mouseEntered(WX_NSEvent theEvent);
    virtual bool Cocoa_mouseExited(WX_NSEvent theEvent);
    virtual bool Cocoa_rightMouseDown(WX_NSEvent theEvent);
    virtual bool Cocoa_rightMouseDragged(WX_NSEvent theEvent);
    virtual bool Cocoa_rightMouseUp(WX_NSEvent theEvent);
    virtual bool Cocoa_otherMouseDown(WX_NSEvent theEvent);
    virtual bool Cocoa_otherMouseDragged(WX_NSEvent theEvent);
    virtual bool Cocoa_otherMouseUp(WX_NSEvent theEvent);
    void SetNSView(WX_NSView cocoaNSView);
    WX_NSView m_cocoaNSView;
    WX_NSView m_dummyNSView;
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    /* Pure Virtuals */
    // Raise the window to the top of the Z order
    virtual void Raise();
    // Lower the window to the bottom of the Z order
    virtual void Lower();
    // Set the focus to this window
    virtual void SetFocus();
    // Warp the pointer the given position
    virtual void WarpPointer(int x_pos, int y_pos) ;
    // Send the window a refresh event
    virtual void Refresh(bool eraseBack = TRUE, const wxRect *rect = NULL);
    // Clear the window
    virtual void Clear();
    // Set/get the window's font
    virtual bool SetFont(const wxFont& f);
//    inline virtual wxFont& GetFont() const;
    // Get character size
    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string, int *x, int *y,
                             int *descent = NULL,
                             int *externalLeading = NULL,
                             const wxFont *theFont = NULL) const;
    // Scroll stuff
    virtual void SetScrollbar(int orient, int pos, int thumbVisible,
      int range, bool refresh = TRUE);
    virtual void SetScrollPos(int orient, int pos, bool refresh = TRUE);
    virtual int GetScrollPos(int orient) const;
    virtual int GetScrollThumb(int orient) const;
    virtual int GetScrollRange(int orient) const;
    virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);
    // Get the private handle (platform-dependent)
    virtual WXWidget GetHandle() const;
    // Convert client to screen coordinates
    virtual void DoClientToScreen(int *x, int *y) const;
    // Convert screen to client coordinates
    virtual void DoScreenToClient(int *x, int *y) const;
    // Capture/release mouse
    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();
    // Get window position, relative to parent (or screen if no parent)
    virtual void DoGetPosition(int *x, int *y) const;
    // Get overall window size
    virtual void DoGetSize(int *width, int *height) const;
    // Get/set client (application-useable) size
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetClientSize(int width, int size);
    // Set overall size and position
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual void DoMoveWindow(int x, int y, int width, int height);
    // Popup a menu
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y);

    /* Other implementation */

    // NOTE: typically Close() is not virtual, but we want this for Cocoa
    virtual bool Close( bool force = false );
    virtual bool Show( bool show = true );
};

#endif // __WX_COCOA_WINDOW_H__
