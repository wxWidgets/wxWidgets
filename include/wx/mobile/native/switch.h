/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/switch.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_SWITCH_H_
#define _WX_MOBILE_NATIVE_SWITCH_H_

#include "wx/bitmap.h"
#include "wx/button.h"
#include "wx/mobile/native/utils.h"

extern WXDLLEXPORT_DATA(const wxChar) wxSwitchCtrlNameStr[];

/**
 @class wxMoSwitchCtrl
 
 This represents a boolean value, and generates the same events as wxCheckBox.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoSwitchCtrl : public wxControl
{
public:
    /// Default constructor.
    wxMoSwitchCtrl() { Init(); }
    
    /// Constructor.
    wxMoSwitchCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxSwitchCtrlNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSwitchCtrlNameStr);
    
    virtual ~wxMoSwitchCtrl();
    
    void Init();
    
    /// Sets the switch value.
    virtual void SetValue(bool value);
    
    /// Gets the switch value.
    virtual bool GetValue() const;
    
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    
protected:
    // send a notification event, return true if processed
    bool SendClickEvent();
    
    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const;
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoSwitchCtrl)
    DECLARE_EVENT_TABLE()
    
    wxMoButtonPressDetector m_pressDetector;
    bool                    m_value;
};

#endif
    // _WX_MOBILE_NATIVE_SWITCH_H_
