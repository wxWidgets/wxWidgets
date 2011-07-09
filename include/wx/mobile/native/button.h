/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/button.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_BUTTON_H_
#define _WX_MOBILE_NATIVE_BUTTON_H_

#include "wx/bitmap.h"
#include "wx/button.h"
#include "wx/mobile/native/utils.h"

// The existing styles
/*
 #define wxBU_LEFT            0x0040
 #define wxBU_TOP             0x0080
 #define wxBU_RIGHT           0x0100
 #define wxBU_BOTTOM          0x0200
 #define wxBU_ALIGN_MASK      ( wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM )
 #define wxBU_AUTODRAW        0x0004
 #define wxBU_EXACTFIT        0x0001
 */

/**
 New wxMoButton styles
 */
enum {  wxBU_ROUNDED_RECTANGLE=0x0800,
    wxBU_DISCLOSURE=0x1000,
    wxBU_INFO_LIGHT=0x2000,
    wxBU_INFO_DARK=0x4000,
    wxBU_CONTACT_ADD=0x8000
};

/**
 @class wxMoButton
 
 This shows a text button.
 
 The new styles are as follows:
 
 @li wxBU_ROUNDED_RECTANGLE: shows a plain style with rounded corners. Without this style,
 the button has the standard bevelled appearance.
 @li wxBU_DISCLOSURE: not yet implemented
 @li wxBU_INFO_LIGHT: not yet implemented
 @li wxBU_INFO_DARK: not yet implemented
 @li wxBU_CONTACT_ADD: not yet implemented
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoButton : public wxButton
{
public:
    /// Default constructor.
    wxMoButton();
    
    /// Constructor taking a text label.
    wxMoButton(wxWindow *parent,
               wxWindowID id,
               const wxString& label = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxButtonNameStr);
    
    /// Creation function taking a text label.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);
    
    virtual ~wxMoButton();
    
    void Init();
    
#if 0
    /// Sets the label
    virtual void SetLabel( const wxString &label );
    
    // Sets the background colour
    virtual bool SetBackgroundColour(const wxColour &colour);
    
    // Gets the background colour
    virtual bool SetForegroundColour(const wxColour &colour);
    
    // Sets the font
    virtual bool SetFont(const wxFont& font);
    
    // Enables or disables the control
    virtual bool Enable(bool enable);
    
    // Sets the border colour
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }
    
    // Gets the border colour
    virtual wxColour GetBorderColour() const { return m_borderColour; }
#endif  // 0
    
protected:
    
#if 0
    wxSize IPGetDefaultSize() const;
    
    // send a notification event, return true if processed
    bool SendClickEvent();
    
    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const;
    
    // function called when any of the bitmaps changes
    virtual void OnSetLabel() { InvalidateBestSize(); Refresh(); }
#endif  // 0
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoButton)
    DECLARE_EVENT_TABLE()
    
    wxMoButtonPressDetector m_pressDetector;
    wxColour                m_borderColour;
};

#endif
    // _WX_MOBILE_NATIVE_BUTTON_H_
