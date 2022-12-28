///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/statbarma.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      Stefan Csomor
// Modified by:
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

// Margin between the field text and the field rect
#define wxFIELD_TEXT_MARGIN 2


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

    if ( !wxStatusBarGeneric::Create( parent, id, style, name ) )
        return false;

    // normal system font is too tall for fitting into the standard height
    SetWindowVariant( wxWINDOW_VARIANT_SMALL );

    InitColours();

    return true;
}

void wxStatusBarMac::InitColours()
{
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
    else // 10.10 Yosemite to 10.13:
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
        if ( dlg && dlg->GetModality() == wxDIALOG_MODALITY_WINDOW_MODAL )
            keyWindow = dlg->GetParent();
        else
            break;
    }

    // Don't paint any background, that's handled by the OS. Only draw text:

    dc.SetTextForeground(tlw == keyWindow ? m_textActive : m_textInactive);

    if ( GetFont().IsOk() )
        dc.SetFont(GetFont());
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    // compute char height only once for all panes:
    int textHeight = dc.GetCharHeight();

    for ( size_t i = 0; i < m_panes.GetCount(); i ++ )
        DrawField(dc, i, textHeight);
}

void wxStatusBarMac::MacHiliteChanged()
{
    Refresh();
    Update();
}

#endif // wxUSE_STATUSBAR

