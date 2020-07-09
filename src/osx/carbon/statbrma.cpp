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
    SetParent( NULL );
    Create( parent, id, style, name );
}

wxStatusBarMac::wxStatusBarMac()
        :
        wxStatusBarGeneric()
{
    SetParent( NULL );
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
        // FIXME: None of this is correct and is only very loose
        //        approximation. 10.14's dark mode uses dynamic colors that
        //        use desktop tinting. The only correct way to render the
        //        statusbar is to use windowBackgroundColor in a NSBox.
        wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
        m_textActive = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
        m_textInactive = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            // dark mode appearance
            m_textActive = wxColour(0xB0, 0xB0, 0xB0);
            m_bgActiveFrom = wxColour(0x32, 0x32, 0x34);
            m_bgActiveTo = wxColour(0x29, 0x29, 0x2A);
            m_borderActive = wxColour(0x00, 0x00, 0x00);
            m_borderInactive = wxColour(0x00, 0x00, 0x00);
        }
        else
        {
            m_bgActiveFrom = wxColour(0xE9, 0xE7, 0xEA);
            m_bgActiveTo = wxColour(0xCD, 0xCB, 0xCE);
            m_borderActive = wxColour(0xBA, 0xB8, 0xBB);
            m_borderInactive = wxColour(0xC3, 0xC3, 0xC3);
        }
        SetBackgroundColour(bg); // inactive bg
    }
    else
    {
        // 10.10 Yosemite to 10.13 :
        m_textActive = wxColour(0x40, 0x40, 0x40);
        m_textInactive = wxColour(0x4B, 0x4B, 0x4B);
        m_bgActiveFrom = wxColour(0xE9, 0xE7, 0xEA);
        m_bgActiveTo = wxColour(0xCD, 0xCB, 0xCE);
        m_borderActive = wxColour(0xBA, 0xB8, 0xBB);
        m_borderInactive = wxColour(0xC3, 0xC3, 0xC3);
        SetBackgroundColour(wxColour(0xF4, 0xF4, 0xF4)); // inactive bg
    }
}

void wxStatusBarMac::DrawFieldText(wxDC& dc, const wxRect& rect, int i, int textHeight)
{
    int w, h;
    GetSize( &w , &h );

    wxString text(GetStatusText( i ));

    int xpos = rect.x + wxFIELD_TEXT_MARGIN + 1;
    int ypos = 2 + (rect.height - textHeight) / 2;

    if ( MacGetTopLevelWindow()->GetExtraStyle() & wxFRAME_EX_METAL )
        ypos++;

    dc.SetClippingRegion(rect.x, 0, rect.width, h);
    dc.DrawText(text, xpos, ypos);
    dc.DestroyClippingRegion();
}

void wxStatusBarMac::DrawField(wxDC& dc, int i, int textHeight)
{
    wxRect rect;
    GetFieldRect(i, rect);

    DrawFieldText(dc, rect, i, textHeight);
}

void wxStatusBarMac::DoUpdateStatusText(int number)
{
    wxRect rect;
    GetFieldRect(number, rect);

    int w, h;
    GetSize( &w, &h );

    rect.y = 0;
    rect.height = h ;

    Refresh( true, &rect );
    // we should have to force the update here
    // TODO Remove if no regressions occur
#if 0
    Update();
#endif
}

void wxStatusBarMac::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.Clear();

    int w, h;
    GetSize( &w, &h );

    // Notice that wxOSXGetKeyWindow (aka [NSApp keyWindow] used below is
    // subtly different from IsActive() (aka [NSWindow iskeyWindow]): the
    // former remains non-NULL if another application shows a temporary
    // floating window or a status icon's menu is used. That's what we want: in
    // that case, statusbar appearance shouldn't change. It also shouldn't
    // change if a window-modal sheet attached to this window is key.
    wxTopLevelWindow *tlw = wxDynamicCast(MacGetTopLevelWindow(), wxTopLevelWindow);
    wxNonOwnedWindow* directKeyWindow = wxNonOwnedWindow::GetFromWXWindow(wxOSXGetKeyWindow());
    wxWindow *keyWindow = directKeyWindow ? directKeyWindow->MacGetTopLevelWindow() : NULL;
    while ( keyWindow && keyWindow != tlw )
    {
        wxDialog *dlg = wxDynamicCast(keyWindow, wxDialog);
        if ( dlg && dlg->GetModality() == wxDIALOG_MODALITY_WINDOW_MODAL )
            keyWindow = dlg->GetParent();
        else
            break;
    }

    if ( tlw == keyWindow )
    {
        dc.GradientFillLinear(dc.GetSize(), m_bgActiveFrom, m_bgActiveTo, wxBOTTOM);

        // Finder statusbar border color
        dc.SetPen(wxPen(m_borderActive, 2, wxPENSTYLE_SOLID));
        dc.SetTextForeground(m_textActive);
    }
    else
    {
        // Finder statusbar border color
        dc.SetPen(wxPen(m_borderInactive, 2, wxPENSTYLE_SOLID));
        dc.SetTextForeground(m_textInactive);
    }

    dc.DrawLine(0, 0, w, 0);

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

