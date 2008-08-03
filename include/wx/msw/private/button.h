///////////////////////////////////////////////////////////////////////////////
// Name:        msw/private/button.h
// Purpose:     helper functions used with native BUTTON control
// Author:      Vadim Zeitlin
// Created:     2008-06-07
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_BUTTON_H_
#define _WX_MSW_PRIVATE_BUTTON_H_

// define some standard button constants which may be missing in the headers
#ifndef BS_PUSHLIKE
    #define BS_PUSHLIKE 0x00001000L
#endif

#ifndef BST_UNCHECKED
    #define BST_UNCHECKED 0x0000
#endif

#ifndef BST_CHECKED
    #define BST_CHECKED 0x0001
#endif

#ifndef BST_INDETERMINATE
    #define BST_INDETERMINATE 0x0002
#endif

#ifndef DT_HIDEPREFIX
    #define DT_HIDEPREFIX 0x00100000
#endif

namespace wxMSWButton
{

// returns BS_MULTILINE if the label contains new lines or 0 otherwise
inline int GetMultilineStyle(const wxString& label)
{
    return label.find(_T('\n')) == wxString::npos ? 0 : BS_MULTILINE;
}

// update the style of the specified HWND to include or exclude BS_MULTILINE
// depending on whether the label contains the new lines
inline void UpdateMultilineStyle(HWND hwnd, const wxString& label)
{
    // update BS_MULTILINE style depending on the new label (resetting it
    // doesn't seem to do anything very useful but it shouldn't hurt and we do
    // have to set it whenever the label becomes multi line as otherwise it
    // wouldn't be shown correctly as we don't use BS_MULTILINE when creating
    // the control unless it already has new lines in its label)
    long styleOld = ::GetWindowLong(hwnd, GWL_STYLE),
         styleNew;
    if ( label.find(_T('\n')) != wxString::npos )
        styleNew = styleOld | BS_MULTILINE;
    else
        styleNew = styleOld & ~BS_MULTILINE;

    if ( styleNew != styleOld )
        ::SetWindowLong(hwnd, GWL_STYLE, styleNew);
}

// common implementation of wxButton and wxToggleButton::DoGetBestSize()
inline wxSize ComputeBestSize(wxControl *btn)
{
    wxClientDC dc(btn);

    wxCoord wBtn,
            hBtn;
    dc.GetMultiLineTextExtent(btn->GetLabelText(), &wBtn, &hBtn);

    // FIXME: this is pure guesswork, need to retrieve the real button margins
    wBtn += 3*btn->GetCharWidth();
    hBtn = 11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(hBtn)/10;

    // all buttons have at least the standard size unless the user explicitly
    // wants them to be of smaller size and used wxBU_EXACTFIT style when
    // creating the button
    if ( !btn->HasFlag(wxBU_EXACTFIT) )
    {
        wxSize sz = wxButton::GetDefaultSize();
        if ( wBtn < sz.x )
            wBtn = sz.x;
        if ( hBtn < sz.y )
            hBtn = sz.y;
    }

    wxSize best(wBtn, hBtn);
    btn->CacheBestSize(best);
    return best;
}

} // namespace wxMSWButton

#endif // _WX_MSW_PRIVATE_BUTTON_H_

