/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/frame.h
// Purpose:     wxFrame class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "frame.h"
#endif

class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

    // implement base class pure virtuals
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual void Raise();

    // implementation only from now on
    // -------------------------------

    // event handlers
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnPaint(wxPaintEvent& event);    

    // Toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = -1,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);

    virtual void PositionToolBar();
#endif // wxUSE_TOOLBAR

    // Status bar
#if wxUSE_STATUSBAR
    virtual wxStatusBar* OnCreateStatusBar(int number = 1,
                                           long style = wxST_SIZEGRIP,
                                           wxWindowID id = 0,
                                           const wxString& name = wxStatusLineNameStr);

    virtual void PositionStatusBar();

    // Hint to tell framework which status bar to use: the default is to use
    // native one for the platforms which support it (Win32), the generic one
    // otherwise

    // TODO: should this go into a wxFrameworkSettings class perhaps?
    static void UseNativeStatusBar(bool useNative)
        { m_useNativeStatusBar = useNative; };
    static bool UsesNativeStatusBar()
        { return m_useNativeStatusBar; };
#endif // wxUSE_STATUSBAR

    WXHMENU GetWinMenu() const { return m_hMenu; }

    // event handlers
    bool HandlePaint();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);
    bool HandleMenuLoop(const wxEventType& evtType, WXWORD isPopup);

    // tooltip management
#if wxUSE_TOOLTIPS
    WXHWND GetToolTipCtrl() const { return m_hwndToolTip; }
    void SetToolTipCtrl(WXHWND hwndTT) { m_hwndToolTip = hwndTT; }
#endif // tooltips

    // a MSW only function which sends a size event to the window using its
    // current size - this has an effect of refreshing the window layout
    virtual void SendSizeEvent();

    virtual wxPoint GetClientAreaOrigin() const;

#if wxUSE_MENUS_NATIVE
    bool HandleMenuOpen();
    bool HandleMenuSelect(int ItemID);
#endif // wxUSE_MENUS_NATIVE
 
protected:
    // common part of all ctors
    void Init();

    // override base class virtuals
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetClientSize(int width, int height);

#if wxUSE_MENUS_NATIVE
    // perform MSW-specific action when menubar is changed
    virtual void AttachMenuBar(wxMenuBar *menubar);

    // a plug in for MDI frame classes which need to do something special when
    // the menubar is set
    virtual void InternalSetMenuBar();
#endif // wxUSE_MENUS_NATIVE

    // propagate our state change to all child frames
    void IconizeChildFrames(bool bIconize);

    // we add menu bar accel processing
    bool MSWTranslateMessage(WXMSG* pMsg);

    // window proc for the frames
    WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    // handle WM_INITMENUPOPUP message
    bool HandleInitMenuPopup(WXHMENU hMenu);

    virtual bool IsMDIChild() const { return FALSE; }

    // get default (wxWidgets) icon for the frame
    virtual WXHICON GetDefaultIcon() const;

#if wxUSE_STATUSBAR
    static bool           m_useNativeStatusBar;
    wxStatusBar              *StatusBar;
#endif // wxUSE_STATUSBAR

    // Data to save/restore when calling ShowFullScreen
    int                   m_fsStatusBarFields; // 0 for no status bar
    int                   m_fsStatusBarHeight;
    int                   m_fsToolBarHeight;

private:
#if wxUSE_TOOLTIPS
    WXHWND                m_hwndToolTip;
#endif // tooltips

    // used by IconizeChildFrames(), see comments there
    bool m_wasMinimized;
    
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxFrame)
};

#endif
    // _WX_FRAME_H_
