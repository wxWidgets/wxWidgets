///////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/toplevel.h
// Purpose:     wxTopLevelWindow
// Author:      Jordan Langholz
// Created:     04/24/07
// RCS-ID:      $Id: toplevel.h,v 1.10 2005/07/01 19:36:51 ABX Exp $
// Copyright:   (c) Jordan Langholz
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYMBIAN_TOPLEVEL_H_
#define _WX_SYMBIAN_TOPLEVEL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "toplevel.h"
#endif

#include <aknview.h>

// ----------------------------------------------------------------------------
// wxTopLevelWindowSymbian
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindowSymbian : public wxTopLevelWindowBase
{
public:
    // constructors and such
    wxTopLevelWindowSymbian() { Init(); }

    wxTopLevelWindowSymbian(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxFrameNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxTopLevelWindowSymbian();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = true);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = true);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void SetIcons(const wxIconBundle& icons );
    virtual void Restore();
    virtual bool SetShape(const wxRegion& region);

    virtual bool Show(bool show = true);

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const { return m_fsIsShowing; }

    // implementation from now on
    // --------------------------

    // called by wxWindow whenever it gets focus
    void SetLastFocus(wxWindow *win) { m_winLastFocused = win; }
    wxWindow *GetLastFocus() const { return m_winLastFocused; }

    // handle native events
    bool HandleControlSelect(WXEVENTPTR event);
    bool HandleControlRepeat(WXEVENTPTR event);
    bool HandleSize(WXEVENTPTR event);

protected:
    // common part of all ctors
    void Init();

    // is the window currently iconized?
    bool m_iconized;

    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;

    // Data to save/restore when calling ShowFullScreen
    long                  m_fsStyle; // Passed to ShowFullScreen
    wxRect                m_fsOldSize;
    long                  m_fsOldWindowStyle;
    bool                  m_fsIsMaximized;
    bool                  m_fsIsShowing;

    // the last focused child: we restore focus to it on activation
    wxWindow             *m_winLastFocused;
    CAknView             *m_internalView;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTopLevelWindowSymbian)
};

#endif // _WX_SYMBIAN_TOPLEVEL_H_

