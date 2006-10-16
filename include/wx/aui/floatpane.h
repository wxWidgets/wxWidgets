///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/floatpane.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FLOATPANE_H_
#define _WX_FLOATPANE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/frame.h"

#if defined( __WXMSW__ ) || defined( __WXMAC__ ) ||  defined( __WXGTK__ )
#include "wx/minifram.h"
#define wxFloatingPaneBaseClass wxMiniFrame
#else
#define wxFloatingPaneBaseClass wxFrame
#endif

class WXDLLIMPEXP_AUI wxFloatingPane : public wxFloatingPaneBaseClass
{
public:
    wxFloatingPane(wxWindow* parent,
                   wxFrameManager* owner_mgr,
                   const wxPaneInfo& pane,
                   wxWindowID id = wxID_ANY,
                   long style = wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                                wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT | 
                                wxCLIP_CHILDREN
                   );
    virtual ~wxFloatingPane();
    void SetPaneWindow(const wxPaneInfo& pane);
    
protected:
    virtual void OnMoveStart();
    virtual void OnMoving(const wxRect& window_rect, wxDirection dir);
    virtual void OnMoveFinished();
    
private:
    void OnSize(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnMoveEvent(wxMoveEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnActivate(wxActivateEvent& event);
    static bool isMouseDown();
private:
    wxWindow* m_pane_window;    // pane window being managed
    bool m_moving;
    wxRect m_last_rect;
    wxRect m_last2_rect;
    wxRect m_last3_rect;
    wxSize m_last_size;
    wxDirection m_lastDirection;

    wxFrameManager* m_owner_mgr;
    wxFrameManager m_mgr;

#ifndef SWIG
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxFloatingPaneBaseClass)
#endif // SWIG
};

#endif // wxUSE_AUI
#endif //_WX_FLOATPANE_H_

