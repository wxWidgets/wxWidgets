/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/panel.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_PANEL_H_
#define _WX_MOBILE_NATIVE_PANEL_H_

#include "wx/mobile/native/window.h"
#include "wx/containr.h"

class WXDLLIMPEXP_FWD_CORE wxControlContainer;

extern WXDLLEXPORT_DATA(const char) wxPanelNameStr[];

/**
 @class wxMoPanel
 
 A class to contain other controls.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoPanel : public wxMoWindow
{
public:
    /// Default constructor.
    wxMoPanel() { Init(); }
    
    /// Constructor.
    wxMoPanel(wxWindow *parent,
              wxWindowID winid = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER,
              const wxString& name = wxPanelNameStr)
    {
        Init();
        
        Create(parent, winid, pos, size, style, name);
    }
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);
    
    virtual ~wxMoPanel();
    
    // Ignore attempts to hide panel
    bool Show(bool show = true);
    
    // calls layout for layout constraints and sizers
    void OnSize(wxSizeEvent& event);
    
    virtual void InitDialog();
    
#ifdef __WXUNIVERSAL__
    virtual bool IsCanvasWindow() const { return true; }
#endif
    
    WX_DECLARE_CONTROL_CONTAINER();
    
protected:
    // common part of all ctors
    void Init();
    
    // Ignore attempts to set size
    void DoSetSize(int x, int y,
                   int width, int height,
                   int sizeFlags = wxSIZE_AUTO);
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoPanel)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_NATIVE_PANEL_H_
