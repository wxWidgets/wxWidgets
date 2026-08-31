/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/dialogcontracts.h
// Purpose:     Pure contract helpers shared by WinUI dialog code and tests
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_DIALOGCONTRACTS_H_
#define _WX_WINUI_PRIVATE_DIALOGCONTRACTS_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/gdicmn.h"
#include "wx/string.h"

#include <climits>
#include <cstddef>

class wxWindow;
class wxMessageDialog;

struct wxWinUIMessageDialogButton
{
    wxString label;
    int id = wxID_NONE;
};

// ContentDialog has exactly three command slots. Keep the wx button model
// independent from XAML so every style combination and dismissal path can be
// exhaustively tested without showing a modal surface.
struct wxWinUIMessageDialogLayout
{
    int buttonIds[4] =
        { wxID_NONE, wxID_NONE, wxID_NONE, wxID_NONE };
    std::size_t buttonCount = 0;
    std::size_t defaultIndex = 0;
    bool requiresNativeFallback = false;
    bool canDismissExternally = true;
};

struct wxWinUIMessageDialogButtons
{
    wxWinUIMessageDialogButton values[4];
    wxWinUIMessageDialogLayout layout;
};

inline wxWinUIMessageDialogLayout
wxWinUIBuildMessageDialogLayout(long style)
{
    wxWinUIMessageDialogLayout layout;

    if ( style & wxYES_NO )
    {
        layout.canDismissExternally = (style & wxCANCEL) != 0;
        layout.buttonIds[layout.buttonCount++] = wxID_YES;
        layout.buttonIds[layout.buttonCount++] = wxID_NO;

        if ( style & wxCANCEL )
            layout.buttonIds[layout.buttonCount++] = wxID_CANCEL;
        if ( style & wxHELP )
            layout.buttonIds[layout.buttonCount++] = wxID_HELP;

        if ( style & wxNO_DEFAULT )
            layout.defaultIndex = 1;
        else if ( style & wxCANCEL_DEFAULT )
            layout.defaultIndex = 2;
    }
    else
    {
        layout.buttonIds[layout.buttonCount++] = wxID_OK;

        // Keep Cancel in the close slot. Help is secondary when both exist.
        if ( style & wxHELP )
            layout.buttonIds[layout.buttonCount++] = wxID_HELP;
        if ( style & wxCANCEL )
            layout.buttonIds[layout.buttonCount++] = wxID_CANCEL;

        if ( style & wxCANCEL_DEFAULT )
            layout.defaultIndex = layout.buttonCount - 1;
    }

    return layout;
}

inline int
wxWinUIResolveMessageDialogDismissal(
    const wxWinUIMessageDialogLayout& layout,
    int presenterResult)
{
    if ( presenterResult != wxID_CANCEL )
        return presenterResult;

    // With a real Cancel button, both that button and an external dismissal
    // have the same public result. A single-OK message also traditionally
    // treats the title-bar close/Escape path as OK. Crucially, never infer
    // Help merely because it occupies ContentDialog's close-button slot: the
    // presenter records an actual third-button click independently and would
    // already have returned wxID_HELP for it.
    for ( std::size_t i = 0; i < layout.buttonCount; ++i )
    {
        if ( layout.buttonIds[i] == wxID_CANCEL )
            return wxID_CANCEL;
    }

    if ( layout.buttonCount == 1 && layout.buttonIds[0] == wxID_OK )
        return wxID_OK;

    return wxID_CANCEL;
}

inline int wxWinUITextEntryMaxLength(unsigned long length)
{
    if ( length == 0 )
        return 0;

    return length > static_cast<unsigned long>(INT_MAX)
        ? INT_MAX
        : static_cast<int>(length);
}

inline wxSize
wxWinUIComputeGrowOnlyClientSize(const wxSize& current,
                                 const wxSize& desired)
{
    return wxSize(
        desired.x > current.x ? desired.x : current.x,
        desired.y > current.y ? desired.y : current.y);
}

inline wxSize
wxWinUIComputeColourDialogBodySize(bool chooseFull,
                                   bool chooseAlpha,
                                   const wxSize& workAreaDIP)
{
    const wxSize desired(
        380,
        chooseFull
            ? (chooseAlpha ? 650 : 610)
            : (chooseAlpha ? 490 : 450));

    // Reserve room for the title, outer margins and command row. The body is
    // placed in a ScrollViewer, so constraining it never makes the picker or
    // the OK/Cancel row unreachable on short/high-DPI work areas.
    const int maxWidth = wxMax(160, workAreaDIP.x - 80);
    const int maxHeight = wxMax(120, workAreaDIP.y - 160);
    return wxSize(wxMin(desired.x, maxWidth),
                  wxMin(desired.y, maxHeight));
}

inline wxSize
wxWinUIPhysicalWorkAreaToDIP(const wxSize& physical, int dpi)
{
    const int effectiveDpi = dpi > 0 ? dpi : 96;
    return wxSize(
        (physical.x * 96 + effectiveDpi / 2) / effectiveDpi,
        (physical.y * 96 + effectiveDpi / 2) / effectiveDpi);
}

// Apply the production grow-only policy immediately. Content-loaded callbacks
// call the same function from their coalesced deferred pass; tests use it
// directly to avoid sleeps and physical window interaction.
WXDLLIMPEXP_CORE bool wxWinUIRefitDialogGrowOnly(wxWindow *top);

// Schedule the shared, coalesced post-layout pass for a dialog which has just
// been shown. Loaded can legitimately have fired while its TLW was hidden, so
// the show boundary must provide a second (bounded) chance to observe stable
// template-derived best sizes. The callback retains only a weak wx identity.
WXDLLIMPEXP_CORE void
wxWinUIScheduleDialogRefitAfterShow(wxWindow *top);

// Build the exact public button/label model consumed by both the WinUI
// presenter and the native TaskDialog fallback. Keeping this seam exported
// avoids tests reaching into non-exported MSW implementation classes.
WXDLLIMPEXP_CORE bool
wxWinUIBuildMessageDialogButtons(const wxMessageDialog& dialog,
                                 wxWinUIMessageDialogButtons& buttons);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_DIALOGCONTRACTS_H_
