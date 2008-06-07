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

} // namespace wxMSWButton

#endif // _WX_MSW_PRIVATE_BUTTON_H_

