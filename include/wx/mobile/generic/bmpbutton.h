/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/bmpbutton.h
// Purpose:     wxMoBitmapButton class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_BMPBUTTON_H_
#define _WX_MOBILE_GENERIC_BMPBUTTON_H_

#include "wx/bitmap.h"
#include "wx/bmpbuttn.h"
#include "wx/mobile/generic/utils.h"
#include "wx/mobile/generic/button.h"

/**
    @class wxMoBitmapButton

    A button that shows a bitmap label. The wxBU_AUTODRAW style is not supported,
    so the control only shows the bitmap and not a button border.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoBitmapButton: public wxBitmapButtonBase
{
public:
    /// Default constructor.
    wxMoBitmapButton() { Init(); }

    /// Constructor taking a bitmap.
    wxMoBitmapButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmap& label = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Init();
        Create(parent, id, label, pos, size, style, validator, name);
    }

    /// Creation function taking a bitmap.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual ~wxMoBitmapButton();

    void Init();

    // Sets the background colour
    virtual bool SetBackgroundColour(const wxColour &colour);

    // Sets the foreground colour
    virtual bool SetForegroundColour(const wxColour &colour);

    // Enables or disables the control
    virtual bool Enable(bool enable);

    /// Sets the border colour
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }

    /// Gets the border colour
    virtual wxColour GetBorderColour() const { return m_borderColour; }

protected:
    wxSize IPGetDefaultSize();

    // send a notification event, return true if processed
    bool SendClickEvent();

    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const;

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoBitmapButton)
    DECLARE_EVENT_TABLE()

    wxMoButtonPressDetector m_pressDetector;
    wxColour                m_borderColour;
};

#endif
    // _WX_MOBILE_GENERIC_BMPBUTTON_H_
