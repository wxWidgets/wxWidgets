///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/statbarma.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"
#include "wx/platinfo.h"
#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dialog.h"
    #include "wx/toplevel.h"
#endif

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"


wxBEGIN_EVENT_TABLE(wxStatusBarMac, wxStatusBarGeneric)
    EVT_PAINT(wxStatusBarMac::OnPaint)
wxEND_EVENT_TABLE()


wxStatusBarMac::wxStatusBarMac(wxWindow *parent,
        wxWindowID id,
        long style,
        const wxString& name)
        :
        wxStatusBarGeneric()
{
    SetParent( nullptr );
    Create( parent, id, style, name );
}

wxStatusBarMac::wxStatusBarMac()
        :
        wxStatusBarGeneric()
{
    SetParent( nullptr );
}

wxStatusBarMac::~wxStatusBarMac()
{
}

bool wxStatusBarMac::Create(wxWindow *parent, wxWindowID id,
                            long style ,
                            const wxString& name)
{
    SetBackgroundStyle( wxBG_STYLE_TRANSPARENT );
    InitCornerInset();

    if ( !wxStatusBarGeneric::Create( parent, id, style, name ) )
        return false;

    // normal system font is too tall for fitting into the standard height
    SetWindowVariant( wxWINDOW_VARIANT_SMALL );

    InitColours();

    return true;
}

void wxStatusBarMac::InitColours()
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_26_0
    if ( WX_IS_MACOS_AVAILABLE(26, 0) )
    {
        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            m_textActive = wxColour(0x9B, 0x9F, 0x9F);
            m_textInactive = wxColour(0x59, 0x5F, 0x60);
            // native separator uses hairline black plus some shading,
            // this approximates it well visually:
            m_separator = wxColour(0x18, 0x18, 0x18);
        }
        else
        {
            m_textActive = wxColour(0x80, 0x80, 0x80);
            m_textInactive = wxColour(0xB8, 0xB8, 0xB8);
            m_separator = wxColour(0xD9, 0xD9, 0xD9);
        }
    }
    else
#endif // MAC_OS_VERSION_26_0
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            m_textActive = wxColour(0xB1, 0xB2, 0xB2);
            m_textInactive = wxColour(0x68, 0x69, 0x6A);
            m_bgActive = wxColour(0x35, 0x36, 0x36);
            m_bgInactive = wxColour(0x27, 0x28, 0x29);
            // native separator uses hairline black plus some shading,
            // this approximates it well visually:
            m_separator = wxColour(0x18, 0x18, 0x18);
        }
        else
        {
            m_textActive = wxColour(0x73, 0x74, 0x74);
            m_textInactive = wxColour(0xA5, 0xA6, 0xA6);
            m_bgActive = wxColour(0xF3, 0xF3, 0xF3);
            m_bgInactive = wxColour(0xE6, 0xE6, 0xE6);
            m_separator = wxColour(0xCC, 0xCC, 0xCC);
        }
    }
    else
#endif // MAC_OS_VERSION_11_0
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14
    if ( WX_IS_MACOS_AVAILABLE(10, 14) )
    {
        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            m_textActive = wxColour(0xA9, 0xA9, 0xA9);
            m_textInactive = wxColour(0x67, 0x67, 0x67);
        }
        else
        {
            m_textActive = wxColour(0x4B, 0x4B, 0x4B);
            m_textInactive = wxColour(0xB1, 0xB1, 0xB1);
        }
    }
    else
#endif // MAC_OS_X_VERSION_10_14
    // 10.10 Yosemite to 10.13:
    {
        m_textActive = wxColour(0x40, 0x40, 0x40);
        m_textInactive = wxColour(0x4B, 0x4B, 0x4B);
    }
}

void wxStatusBarMac::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    // Notice that wxOSXGetKeyWindow (aka [NSApp keyWindow] used below is
    // subtly different from IsActive() (aka [NSWindow iskeyWindow]): the
    // former remains non-null if another application shows a temporary
    // floating window or a status icon's menu is used. That's what we want: in
    // that case, statusbar appearance shouldn't change. It also shouldn't
    // change if a window-modal sheet attached to this window is key.
    wxTopLevelWindow *tlw = wxDynamicCast(MacGetTopLevelWindow(), wxTopLevelWindow);
    wxNonOwnedWindow* directKeyWindow = wxNonOwnedWindow::GetFromWXWindow(wxOSXGetKeyWindow());
    wxWindow *keyWindow = directKeyWindow ? directKeyWindow->MacGetTopLevelWindow() : nullptr;
    while ( keyWindow && keyWindow != tlw )
    {
        wxDialog *dlg = wxDynamicCast(keyWindow, wxDialog);
        if ( dlg && dlg->GetModality() == wxWindowMode::WindowModal )
            keyWindow = dlg->GetParent();
        else
            break;
    }

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_26_0
    if ( WX_IS_MACOS_AVAILABLE(26, 0) )
    {
        // don't paint the background, handled by the OS
    }
    else
#endif
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        // we _do_ need to paint the background on Big Sur up to Tahoe
        // to match Finder's appearance:
        dc.SetBackground(tlw == keyWindow ? m_bgActive : m_bgInactive);
        dc.Clear();
    }
    // else: background is rendered by OS, it is part of NSWindow border
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
    // Draw horizontal separator above the status bar:
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        dc.SetPen(m_separator);
        dc.DrawLine(0, 0, GetSize().x, 0);
    }
#endif

    // Draw the text:

    dc.SetTextForeground(tlw == keyWindow ? m_textActive : m_textInactive);

    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    // compute char height only once for all panes:
    int textHeight = dc.GetCharHeight();

    for ( size_t i = 0; i < m_panes.GetCount(); i ++ )
        DrawField(dc, i, textHeight);
}

void wxStatusBarMac::InitCornerInset()
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_26_0
    if ( WX_IS_MACOS_AVAILABLE(26, 0) )
        m_cornerInset = 8;
    else
#endif
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
        m_cornerInset = 4;
    else
#endif
        m_cornerInset = 0;
}

void wxStatusBarMac::MacSetCornerInset(int inset)
{
    m_cornerInset = inset;
    // force recalculation of the fields:
    m_lastClientSize = wxDefaultSize;
    Refresh();
}

int wxStatusBarMac::GetAvailableWidthForFields(int width) const
{
    return wxStatusBarGeneric::GetAvailableWidthForFields(width) - 2 * m_cornerInset;
}

bool wxStatusBarMac::GetFieldRect(int i, wxRect& rect) const
{
    if ( !wxStatusBarGeneric::GetFieldRect(i, rect) )
        return false;

    rect.x += MacGetCornerInset();
    return true;
}

void wxStatusBarMac::MacHiliteChanged()
{
    Refresh();
    Update();
}

#endif // wxUSE_STATUSBAR

