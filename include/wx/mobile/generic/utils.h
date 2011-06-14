/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/utils.h
// Purpose:     wxMobile 'emulator' utilities.
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_UTILS_H_
#define _WX_MOBILE_GENERIC_UTILS_H_

#include  "wx/event.h"
#include  "wx/control.h"

#include  "wx/mobile/generic/defs.h"
#include  "wx/mobile/generic/settings.h"

enum
{
    wxMO_BUTTON_TWO_TONE =              0x00000001,
    wxMO_BUTTON_SOLID    =              0x00000002,
    wxMO_BUTTON_NO_BORDER =             0x00000004,
    wxMO_BUTTON_SQUARE =                0x00000008,
    wxMO_BUTTON_BACK =                  0x00000010,
    wxMO_BUTTON_NO_BACKGROUND =         0x00000020,
    wxMO_BUTTON_STRAIGHT_LEFT_EDGE =    0x00000040,
    wxMO_BUTTON_STRAIGHT_RIGHT_EDGE =   0x00000080,
    wxMO_BUTTON_HIGHLIGHTED =           0x00000100,
    wxMO_BUTTON_SELECTED =              0x00000200
};

// Class for rendering simulated iPhone controls
class wxMoRenderer
{
public:
    wxMoRenderer() {}

    static bool DrawButtonBackground(wxWindow* window, wxDC& dc,
        const wxColour& backgroundColour, const wxColour& colour,
        const wxColour& borderColour, const wxRect& rect, int state, int buttonStyle, int inc = 8);

    static unsigned char BlendColour(unsigned char fg, unsigned char bg, double alpha);

    static bool IncColourValues(wxColour& colour, int inc);

    static wxBitmap MakeDisabledBitmap(wxBitmap& bmp);
};

// Button press detecter
class wxMoButtonPressDetector
{
public:
    wxMoButtonPressDetector() { m_buttonState = wxCTRL_STATE_NORMAL; m_buttonId = 0; }

    // Returns true if this should send a click event.
    bool ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event, int id = 0);

    void SetButtonState(int state) { m_buttonState = state; }
    int GetButtonState() const { return m_buttonState; }

    void SetButtonId(int id) { m_buttonId = id; }
    int GetButtonId() const { return m_buttonId; }

protected:
    int m_buttonState;
    int m_buttonId;
};

#endif
    // _WX_MOBILE_GENERIC_UTILS_H_
