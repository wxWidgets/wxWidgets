/////////////////////////////////////////////////////////////////////////////
// Name:        common/dlgcmn.cpp
// Purpose:     common (to all ports) wxDialog functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if 0
#ifdef __GNUG__
    #pragma implementation
#endif
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const long wxDialogBase::LAYOUT_X_MARGIN = 5;
const long wxDialogBase::LAYOUT_Y_MARGIN = 5;

const long wxDialogBase::MARGIN_BETWEEN_BUTTONS = 3*LAYOUT_X_MARGIN;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dialog layout functions
// ----------------------------------------------------------------------------

wxSize wxDialogBase::SplitTextMessage(const wxString& message,
                                      wxArrayString *lines)
{
    wxClientDC dc(this);
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    wxString curLine;
    long height, width, heightTextMax = 0, widthTextMax = 0;
    for ( const wxChar *pc = message; ; pc++ )
    {
        if ( *pc == _T('\n') || !*pc )
        {
            dc.GetTextExtent(curLine, &width, &height);
            if ( width > widthTextMax )
                widthTextMax = width;
            if ( height > heightTextMax )
                heightTextMax = height;

            lines->Add(curLine);

            if ( !*pc )
            {
               // the end of string
               break;
            }

            curLine.Empty();
        }
        else
        {
            curLine += *pc;
        }
    }

    return wxSize(widthTextMax, heightTextMax);
}

long wxDialogBase::CreateTextMessage(const wxArrayString& lines,
                                     const wxPoint& posText,
                                     const wxSize& sizeText)
{
    wxStaticText *text;
    int y = posText.y;
    size_t nLineCount = lines.GetCount();
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
    {
        text = new wxStaticText(this, -1, lines[nLine],
                                wxPoint(posText.x, y),
                                sizeText);
        y += sizeText.GetHeight();
    }

    return y;
}

wxSize wxDialogBase::GetStandardButtonSize(bool hasCancel)
{
    int wButton = 0;
    GetTextExtent(_("OK"), &wButton, NULL);

    if ( hasCancel )
    {
        int width;
        GetTextExtent(_("Cancel"), &width, NULL);
        if ( width > wButton )
            wButton = width;
    }

    if ( wButton < 75 )
    {
        // the minimal acceptable width
        wButton = 75;
    }
    else
    {
        // the width of the button is not just the width of the label...
        wButton += 2*LAYOUT_X_MARGIN;
    }

    // a nice looking proportion
    int hButton = (wButton * 23) / 75;

    return wxSize(wButton, hButton);
}

void wxDialogBase::CreateStandardButtons(long wDialog,
                                         long y,
                                         long wButton,
                                         long hButton,
                                         bool hasCancel)
{
    // NB: create [Ok] first to get the right tab order
    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;

    long x = wDialog / 2;
    if ( hasCancel )
        x -= MARGIN_BETWEEN_BUTTONS / 2 + wButton;
    else
        x -= wButton / 2;

    ok = new wxButton( this, wxID_OK, _("OK"),
                       wxPoint(x, y),
                       wxSize(wButton, hButton) );

    if ( hasCancel )
    {
        x += MARGIN_BETWEEN_BUTTONS + wButton;
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel"),
                               wxPoint(x, y),
                               wxSize(wButton, hButton) );
    }

    ok->SetDefault();
    ok->SetFocus();
}

long wxDialogBase::GetStandardTextHeight()
{
    return (3*GetCharHeight()) / 2;
}
