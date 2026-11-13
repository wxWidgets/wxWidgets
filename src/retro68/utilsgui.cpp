/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/utilsgui.cpp
// Purpose:     GUI utility functions for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/region.h"
#endif

#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/private/display.h"
#include "wx/unix/private/timer.h"

#include "wx/retro68/private/qd.h"

#include <string>

// ============================================================================
// private helpers
// ============================================================================

bool wxMacHasColorQD()
{
    static int s_hasColorQD = -1;
    if ( s_hasColorQD == -1 )
        s_hasColorQD = wxMacGestalt(gestaltQuickdrawVersion) >= gestalt8BitQD;

    return s_hasColorQD == 1;
}

RgnHandle wxMacRegionToRgn(const wxRegion& region, const wxPoint& offset)
{
    RgnHandle rgn = NewRgn();
    RgnHandle rgnRect = NewRgn();

    for ( wxRegionIterator it(region); it; ++it )
    {
        wxRect r = it.GetRect();
        r.Offset(offset);

        const Rect macRect = wxMacRectFromWX(r);
        RectRgn(rgnRect, &macRect);
        UnionRgn(rgn, rgnRect, rgn);
    }

    DisposeRgn(rgnRect);

    return rgn;
}

wxCharBuffer wxMacTextToSystem(const wxString& text)
{
    wxCharBuffer buf = text.mb_str(wxConvLibc);
    if ( buf.data() )
        return buf;

    // Some characters couldn't be converted, so do it character by character
    // replacing the ones which can't be represented with question marks.
    std::string s;
    s.reserve(text.length());

    for ( wxString::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        const wxCharBuffer bufChar = wxString(*it).mb_str(wxConvLibc);
        if ( bufChar.data() && bufChar.length() == 1 )
            s += bufChar.data()[0];
        else
            s += '?';
    }

    return wxCharBuffer(s.c_str());
}

// ============================================================================
// wxGUIAppTraits
// ============================================================================

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj,
                                           int *verMin,
                                           int *verMicro) const
{
    // The toolkit is the Toolbox itself, so return the system version.
    wxGetOsVersion(verMaj, verMin, verMicro);

    return wxPORT_RETRO68;
}

wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

#if wxUSE_TIMER

wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxUnixTimerImpl(timer);
}

#endif // wxUSE_TIMER

// ============================================================================
// display characteristics
// ============================================================================

class wxDisplayImplSingleRetro68 : public wxDisplayImplSingle
{
public:
    virtual wxRect GetGeometry() const override
    {
        return wxRectFromMac(wxMacGetScreenBounds());
    }

    virtual wxRect GetClientArea() const override
    {
        wxRect r = GetGeometry();

        const int menuBarHeight = wxMacGetMenuBarHeight();
        r.y += menuBarHeight;
        r.height -= menuBarHeight;

        return r;
    }

    virtual int GetDepth() const override
    {
        if ( !wxMacHasColorQD() )
            return 1;

        GDHandle device = GetMainDevice();
        if ( !device )
            return 8;

        return (**(**device).gdPMap).pixelSize;
    }

    virtual wxSize GetPPI() const override
    {
        // Classic Mac OS always assumes 72 DPI.
        return wxSize(72, 72);
    }
};

class wxDisplayFactorySingleRetro68 : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay() override
    {
        return new wxDisplayImplSingleRetro68;
    }
};

wxDisplayFactory* wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleRetro68;
}

// ============================================================================
// mouse and keyboard
// ============================================================================

void wxGetMousePosition(int *x, int *y)
{
    EventRecord ev;
    EventAvail(0, &ev);

    if ( x )
        *x = ev.where.h;
    if ( y )
        *y = ev.where.v;
}

wxMouseState wxGetMouseState()
{
    EventRecord ev;
    EventAvail(0, &ev);

    wxMouseState ms;
    ms.SetX(ev.where.h);
    ms.SetY(ev.where.v);

    const bool down = Button();
    const bool right = (ev.modifiers & wxMAC_CONTROL_KEY) != 0;
    ms.SetLeftDown(down && !right);
    ms.SetRightDown(down && right);

    ms.SetShiftDown((ev.modifiers & shiftKey) != 0);
    ms.SetControlDown((ev.modifiers & cmdKey) != 0);
    ms.SetAltDown((ev.modifiers & optionKey) != 0);
    ms.SetMetaDown((ev.modifiers & wxMAC_CONTROL_KEY) != 0);

    return ms;
}

bool wxGetKeyState(wxKeyCode key)
{
    wxASSERT_MSG(key != WXK_LBUTTON && key != WXK_RBUTTON && key != WXK_MBUTTON,
                 "can't use wxGetKeyState() for mouse buttons");

    // Virtual key codes of the modifier keys.
    int vk;
    switch ( key )
    {
        case WXK_SHIFT:     vk = 0x38; break;
        case WXK_CONTROL:   vk = 0x37; break; // Command key
        case WXK_ALT:       vk = 0x3A; break; // Option key
        case WXK_CAPITAL:   vk = 0x39; break;
        case WXK_ESCAPE:    vk = 0x35; break;
        case WXK_SPACE:     vk = 0x31; break;
        case WXK_RETURN:    vk = 0x24; break;
        case WXK_TAB:       vk = 0x30; break;
        case WXK_LEFT:      vk = 0x7B; break;
        case WXK_RIGHT:     vk = 0x7C; break;
        case WXK_DOWN:      vk = 0x7D; break;
        case WXK_UP:        vk = 0x7E; break;

        default:
            return false;
    }

    uint8_t keys[16];
    GetKeys(keys);

    return (keys[vk >> 3] & (1 << (vk & 7))) != 0;
}

// ============================================================================
// misc
// ============================================================================

void wxBell()
{
    SysBeep(30);
}

bool wxLaunchDefaultApplication(const wxString& document, int flags)
{
    // There is no way to open a document in its default application without
    // using the Finder Apple events, which is not supported yet, but we can
    // still launch the applications themselves.
    return wxExecute(document, flags) != 0;
}
