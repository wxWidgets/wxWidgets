/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/webctrl.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_WEBCTRL_H_
#define _WX_MOBILE_NATIVE_WEBCTRL_H_

#include "wx/bitmap.h"
#include "wx/control.h"
#include "wx/html/webkit.h"

extern WXDLLEXPORT_DATA(const wxChar) wxWebCtrlNameStr[];

/**
 @class wxMoWebCtrl
 
 A control for displaying remote web pages.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoWebCtrl : public wxWebKitCtrl
{
public:
    /// Default constructor.
    wxMoWebCtrl();
    
    /// Constructor.
    wxMoWebCtrl(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWebCtrlNameStr);
    
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWebCtrlNameStr);
    
    virtual ~wxMoWebCtrl();
    
    void Init();
    
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    
protected:
    virtual wxSize DoGetBestSize() const;
    void OnSize(wxSizeEvent& event);
        
    bool            m_userScaling;
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoWebCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_NATIVE_WEBCTRL_H_
