/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/frame.h
// Purpose:     Special frame class for use on iPhone or
//              iPhone 'emulator'
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:
// Copyright:   Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_FRAME_H_
#define _WX_MOBILE_GENERIC_FRAME_H_

#include "wx/frame.h"

class wxMoSimulatorScreenWindow;

/**
    @class wxMoFrame

    A special iPhone frame class to use in wxMobile emulation mode or iPhone.
    Currently, only one frame per application is supported.

    @category{wxMobile}
 */

class wxMoFrame: public wxWindow
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

    /// The simulator screen window to use as a parent.
    wxMoSimulatorScreenWindow* GetSimulatorScreenWindow() const { return m_simulatorScreenWindow; }

    /// The simulator screen window to use as a parent. Set this
    /// to ensure that the 'frame' is parented correctly, even if no
    /// parent is passed to the constructor.
    void SetSimulatorScreenWindow(wxMoSimulatorScreenWindow* win) { m_simulatorScreenWindow = win; }

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

    // The simulator screen window to use as a parent. Set this
    // to ensure that the 'frame' is parented correctly, even if no
    // parent is passed to the constructor.
    wxMoSimulatorScreenWindow*  m_simulatorScreenWindow;
};

#endif
    // _WX_MOBILE_GENERIC_FRAME_H_

