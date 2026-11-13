///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/private/qd.h
// Purpose:     Private QuickDraw and Toolbox helpers for wxRetro68 GUI code
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_PRIVATE_QD_H_
#define _WX_RETRO68_PRIVATE_QD_H_

#include "wx/retro68/private.h"

#include "wx/gdicmn.h"
#include "wx/colour.h"

class WXDLLIMPEXP_FWD_CORE wxRegion;
class WXDLLIMPEXP_FWD_CORE wxNonOwnedWindow;

// ----------------------------------------------------------------------------
// Constants missing from Multiverse.h
// ----------------------------------------------------------------------------

enum
{
    // Window definition procedures IDs.
    wxMAC_ZOOM_DOC_PROC         = 8,    // zoomDocProc
    wxMAC_ZOOM_NO_GROW_PROC     = 12,   // zoomNoGrow

    // Event modifiers.
    wxMAC_CONTROL_KEY           = 0x1000, // controlKey

    // osEvt messages and flags.
    wxMAC_SUSPEND_RESUME_MSG    = 1,    // suspendResumeMessage
    wxMAC_RESUME_FLAG           = 1,    // resumeFlag
    wxMAC_CONVERT_CLIPBOARD     = 2     // convertClipboardFlag
};

// ----------------------------------------------------------------------------
// Accessors working for both classic Toolbox and Carbon
// ----------------------------------------------------------------------------

inline GrafPtr wxMacGetWindowGrafPtr(WindowRef win)
{
#if TARGET_API_MAC_CARBON
    return reinterpret_cast<GrafPtr>(GetWindowPort(win));
#else
    return reinterpret_cast<GrafPtr>(win);
#endif
}

inline Rect wxMacGetPortRect(GrafPtr port)
{
    Rect r;
#if TARGET_API_MAC_CARBON
    GetPortBounds(reinterpret_cast<CGrafPtr>(port), &r);
#else
    r = port->portRect;
#endif
    return r;
}

inline BitMap* wxMacGetPortBitMap(GrafPtr port)
{
#if TARGET_API_MAC_CARBON
    return const_cast<BitMap*>(GetPortBitMapForCopyBits(reinterpret_cast<CGrafPtr>(port)));
#else
    // Notice that for the colour ports this field contains the PixMapHandle
    // and special flags in its rowBytes field, but CopyBits() knows how to
    // deal with it, so it's fine to use it for both kinds of ports.
    return &port->portBits;
#endif
}

inline Rect wxMacGetScreenBounds()
{
#if TARGET_API_MAC_CARBON
    BitMap screenBits;
    GetQDGlobalsScreenBits(&screenBits);
    return screenBits.bounds;
#else
    return qd.screenBits.bounds;
#endif
}

// Return the height of the menu bar.
inline int wxMacGetMenuBarHeight()
{
    return GetMBarHeight();
}

// Return true if Color QuickDraw is available.
WXDLLIMPEXP_CORE bool wxMacHasColorQD();

// Make sure the screen device is current and the current port is not an
// offscreen one, as the Toolbox functions drawing on the screen assume this.
inline void wxMacRestoreScreenPort()
{
    GrafPtr wmPort;
    GetWMgrPort(&wmPort);

    if ( wxMacHasColorQD() )
        SetGWorld(reinterpret_cast<CGrafPtr>(wmPort), GetMainDevice());
    else
        SetPort(wmPort);
}

// Make the given window (or Window Manager) port current, also ensuring that
// the main device is the current one, as it could have been changed to an
// offscreen device by drawing on a GWorld before.
inline void wxMacSetWindowPort(GrafPtr port)
{
    if ( wxMacHasColorQD() )
        SetGWorld(reinterpret_cast<CGrafPtr>(port), GetMainDevice());
    else
        SetPort(port);
}

// ----------------------------------------------------------------------------
// Event Manager functions which are not available in 68K interface library
// ----------------------------------------------------------------------------

inline long wxMacGetDblTime()
{
#if defined(__m68k__) && !TARGET_API_MAC_CARBON
    return LMGetDoubleTime();
#else
    return GetDblTime();
#endif
}

inline long wxMacGetCaretTime()
{
#if defined(__m68k__) && !TARGET_API_MAC_CARBON
    return LMGetCaretTime();
#else
    return GetCaretTime();
#endif
}

inline void wxMacSetEventMask(short mask)
{
#if defined(__m68k__) && !TARGET_API_MAC_CARBON
    LMSetSysEvtMask(mask);
#else
    SetEventMask(mask);
#endif
}

inline OSErr wxMacPostEvent(short what, long message)
{
#if defined(__m68k__)
    // _PostEvent is a register-based OS trap taking the event code in A0 and
    // the message in D0 and returning the error code in D0.
    register long d0 asm("d0") = message;
    register long a0 asm("a0") = what;
    asm volatile(".short 0xA02F"
                 : "+r"(d0), "+r"(a0)
                 :
                 : "d1", "a1", "cc", "memory");
    return static_cast<OSErr>(d0);
#else
    return PostEvent(what, message);
#endif
}

// ----------------------------------------------------------------------------
// Conversions between wx and QuickDraw types
// ----------------------------------------------------------------------------

inline short wxMacClampCoord(int x)
{
    return x < -32767 ? -32767 : x > 32767 ? 32767 : static_cast<short>(x);
}

inline Rect wxMacRectFromWX(int x, int y, int w, int h)
{
    Rect r;
    r.left = wxMacClampCoord(x);
    r.top = wxMacClampCoord(y);
    r.right = wxMacClampCoord(x + w);
    r.bottom = wxMacClampCoord(y + h);
    return r;
}

inline Rect wxMacRectFromWX(const wxRect& rect)
{
    return wxMacRectFromWX(rect.x, rect.y, rect.width, rect.height);
}

inline wxRect wxRectFromMac(const Rect& r)
{
    return wxRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

inline RGBColor wxMacColourToRGB(const wxColour& col)
{
    RGBColor rgb;
    rgb.red = static_cast<unsigned short>(col.Red() * 257);
    rgb.green = static_cast<unsigned short>(col.Green() * 257);
    rgb.blue = static_cast<unsigned short>(col.Blue() * 257);
    return rgb;
}

inline wxColour wxMacRGBToColour(const RGBColor& rgb)
{
    return wxColour(rgb.red >> 8, rgb.green >> 8, rgb.blue >> 8);
}

// Create a new QuickDraw region from wxRegion, offsetting it by the given
// amount. The caller is responsible for disposing of the returned region.
WXDLLIMPEXP_CORE RgnHandle wxMacRegionToRgn(const wxRegion& region,
                                            const wxPoint& offset = wxPoint());

// Convert the text to the system encoding, replacing all characters which
// can't be represented in it with question marks.
WXDLLIMPEXP_CORE wxCharBuffer wxMacTextToSystem(const wxString& text);

// ----------------------------------------------------------------------------
// Port state saving
// ----------------------------------------------------------------------------

// Save and restore the current graphics port and device.
class wxMacPortSaver
{
public:
    wxMacPortSaver()
    {
        GetGWorld(&m_port, &m_device);
    }

    ~wxMacPortSaver()
    {
        SetGWorld(m_port, m_device);
    }

private:
    CGrafPtr m_port;
    GDHandle m_device;

    wxDECLARE_NO_COPY_CLASS(wxMacPortSaver);
};

// ----------------------------------------------------------------------------
// Window helpers
// ----------------------------------------------------------------------------

// Return the wx window corresponding to the given native window, if any.
WXDLLIMPEXP_CORE wxNonOwnedWindow* wxMacFindWindow(WindowRef win);

// Return the outer and inner rectangles of the native window in global
// coordinates.
WXDLLIMPEXP_CORE Rect wxMacGetWindowStructureRect(WindowRef win);
WXDLLIMPEXP_CORE Rect wxMacGetWindowContentRect(WindowRef win);

// ----------------------------------------------------------------------------
// Event helpers
// ----------------------------------------------------------------------------

// Convert ticks used by the Event Manager to milliseconds used by wx.
inline long wxMacTicksToMillis(unsigned long ticks)
{
    return static_cast<long>(ticks * 50 / 3);
}

// Handle the given Event Manager event, defined in src/retro68/evtloop.cpp.
WXDLLIMPEXP_CORE void wxMacHandleEvent(EventRecord& event);

// Check if the mouse moved or the mouse button state changed and generate
// the corresponding wx events if necessary.
WXDLLIMPEXP_CORE void wxMacCheckMouse();

#endif // _WX_RETRO68_PRIVATE_QD_H_
