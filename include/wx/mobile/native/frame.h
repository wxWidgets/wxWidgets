/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/frame.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_FRAME_H_
#define _WX_MOBILE_NATIVE_FRAME_H_

#include "wx/frame.h"

/**
 @class wxMoFrame
 
 A special iPhone frame class to use in wxMobile emulation mode or iPhone.
 Currently, only one frame per application is supported.
 
 @category{wxMobile}
 */

class wxMoFrame: public wxFrame
{
    DECLARE_DYNAMIC_CLASS( wxMoFrame )
    DECLARE_EVENT_TABLE( )
    
public:
    /// Default constructor.
    wxMoFrame() { Init(); }
    
    /// Constructor. You can pass NULL for the parent.
    wxMoFrame(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style = 0);
    
    /// Creation function. You can pass NULL for the parent.
    bool Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style = 0 );
    
    ~wxMoFrame();
    
    void Init();
    
    /// Sets the title.
    void SetTitle(const wxString& title) { m_title = title; }
    
    /// Gets the title.
    wxString GetTitle() const { return m_title; }
    
    // Do layout
    void DoLayout();
    
    // test whether this window makes part of the frame
    // (menubar, toolbar and statusbar are excluded from automatic layout)
    virtual bool IsOneOfBars(const wxWindow *win) const;
    
    // Send a size event.
    void SendSizeEvent();
    
protected:
    
    void OnSize(wxSizeEvent& event);
    void OnIdle(wxIdleEvent& event);
    
    wxString                    m_title;
};

#endif
    // _WX_MOBILE_NATIVE_FRAME_H_
