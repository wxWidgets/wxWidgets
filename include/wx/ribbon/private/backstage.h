///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/private/backstage.h
// Purpose:     Helpers shared by the backstage view and its controls
// Author:      Blake Madden
// Created:     2026-09-20
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_PRIVATE_BACKSTAGE_H_
#define _WX_RIBBON_PRIVATE_BACKSTAGE_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/bmpbndl.h"
#include "wx/colour.h"
#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/event.h"
#include "wx/font.h"
#include "wx/window.h"

class wxBackstage;

class wxBackstageHelpers
{
public:
    wxNODISCARD
    static bool IsDark(const wxColour& colour)
    {
        wxCHECK_MSG( colour.IsOk(), false, "Invalid colour passed to IsDark()!" );
        return colour.Alpha() > 32 && colour.GetLuminance() < 0.5;
    }
    wxNODISCARD
    static wxColour ShadeOrTint(const wxColour& colour,
        const double shadeOrTintValue = 0.2)
    {
        return IsDark(colour) ?
            colour.ChangeLightness(100 + static_cast<int>(shadeOrTintValue * 100)) :
            colour.ChangeLightness(100 - static_cast<int>(shadeOrTintValue * 100));
    }
    wxNODISCARD
    static wxColour BlackOrWhiteContrast(const wxColour& colour)
    {
        return IsDark(colour) ? wxColour{ 255, 255, 255 } : wxColour{ 0, 0, 0 };
    }
    wxNODISCARD
    static wxColour Blend(const wxColour& from, const wxColour& to, const double amount)
    {
        const auto mix = [amount](const unsigned char a, const unsigned char b)
            {
                return static_cast<unsigned char>(
                    wxRound(a + (static_cast<double>(b) - a) * amount));
            };
        return wxColour{ mix(from.Red(), to.Red()),
                         mix(from.Green(), to.Green()),
                         mix(from.Blue(), to.Blue()) };
    }
    wxNODISCARD
    static const wxBackstage* FindBackstage(const wxWindow* window);
    static void GetPageColours(const wxWindow* window, wxColour& background,
                               wxColour& foreground);

    static void DrawBitmapFit(wxDC& dc, const wxWindow* window,
                              const wxBitmapBundle& bundle, const wxRect& rect);
    static void DrawGlossyRect(wxDC& dc, const wxRect& rect, const wxColour& colour);
    static void DrawFocusRect(wxDC& dc, const wxRect& rect, const wxColour& colour);
    wxNODISCARD
    static wxSize MeasureText(const wxWindow* window, const wxString& text,
                              const wxFont& font);

    static bool SkipIfShortcutKey(wxKeyEvent& event)
    {
        if ( event.HasModifiers() )
        {
            event.Skip();
            return true;
        }
        return false;
    }
    wxNODISCARD
    static bool IsActivateKey(const int keyCode) noexcept
    {
        return keyCode == WXK_SPACE || keyCode == WXK_RETURN ||
            keyCode == WXK_NUMPAD_ENTER;
    }
};

struct wxBackstagePaintBuffer
{
    explicit wxBackstagePaintBuffer(wxWindow* win);

    wxAutoBufferedPaintDC m_buffer;
    wxColour m_background;
    wxColour m_foreground;
};

struct wxBackstagePaintGraphics : public wxBackstagePaintBuffer
{
    wxBackstagePaintGraphics(wxWindow* win, const wxFont& font);

    wxGCDC m_graphics;
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PRIVATE_BACKSTAGE_H_
