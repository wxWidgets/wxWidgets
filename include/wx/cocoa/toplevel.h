///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/toplevel.h
// Purpose:     wxTopLevelWindowCocoa is the Cocoa implementation of wxTLW
// Author:      David Elliott
// Modified by:
// Created:     2002/12/08
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_TOPLEVEL_H__
#define __WX_COCOA_TOPLEVEL_H__

#include "wx/hashmap.h"
#include "wx/cocoa/NSWindow.h"

// ========================================================================
// wxTopLevelWindowCocoa
// ========================================================================
class WXDLLEXPORT wxTopLevelWindowCocoa : public wxTopLevelWindowBase, protected wxCocoaNSWindow
{
    DECLARE_EVENT_TABLE();
    DECLARE_NO_COPY_CLASS(wxTopLevelWindowCocoa);
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    inline wxTopLevelWindowCocoa() { Init(); }

    inline wxTopLevelWindowCocoa(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxFrameNameStr)
    {
        Init();
        Create(parent, winid, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxTopLevelWindowCocoa();

protected:
    // common part of all ctors
    void Init();

// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    inline WX_NSWindow GetNSWindow() { return m_cocoaNSWindow; }
    virtual void Cocoa_close(void);
    virtual bool Cocoa_windowShouldClose(void);
    virtual void Cocoa_wxMenuItemAction(wxMenuItem& item);
protected:
    void SetNSWindow(WX_NSWindow cocoaNSWindow);
    WX_NSWindow m_cocoaNSWindow;
    static wxCocoaNSWindowHash sm_cocoaHash;

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    // Pure virtuals
    virtual void Maximize(bool maximize = true);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = true);
    virtual bool IsIconized() const;
    virtual void Restore();
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const;
    // other
    virtual bool Show( bool show = true );
    virtual bool Close( bool force = false );
    virtual void OnCloseWindow(wxCloseEvent& event);
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetPosition(int *x, int *y) const;


// Things I may/may not do
//    virtual void SetIcon(const wxIcon& icon);
//    virtual void SetIcons(const wxIconBundle& icons) { SetIcon( icons.GetIcon( -1 ) ); }
//    virtual void Clear() ;
//    virtual void Raise();
//    virtual void Lower();
//    virtual void SetTitle( const wxString& title);
protected:
    // is the frame currently iconized?
    bool m_iconized;
    // has the frame been closed
    bool m_closed;
    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;
};

// list of all frames and modeless dialogs
extern WXDLLEXPORT_DATA(wxWindowList) wxModelessWindows;

#endif // __WX_COCOA_TOPLEVEL_H__
