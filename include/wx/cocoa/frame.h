/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/frame.h
// Purpose:     wxFrame class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_FRAME_H_
#define _WX_COCOA_FRAME_H_

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenuItem;

class WXDLLEXPORT wxFrame: public wxFrameBase 
{
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxFrame)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
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

    virtual ~wxFrame();

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);
protected:
    void Init();
// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
protected:
    virtual void Cocoa_wxMenuItemAction(wxMenuItem& item);
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    void AttachMenuBar(wxMenuBar *mbar);
    void DetachMenuBar();

    // implementation only from now on
    // -------------------------------

    // override some more virtuals
    virtual bool Show( bool show = true );

    // get the origin of the client area (which may be different from (0, 0)
    // if the frame has a toolbar) in client coordinates
    virtual wxPoint GetClientAreaOrigin() const;
};

#endif // _WX_COCOA_FRAME_H_
