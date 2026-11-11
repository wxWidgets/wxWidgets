///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/bmpbuttn.h
// Purpose:     Private functions related to wxBitmapButton.
// Author:      Vadim Zeitlin
// Created:     2026-10-21
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_BMPBUTTN_H_
#define _WX_PRIVATE_BMPBUTTN_H_

#include "wx/bmpbuttn.h"

#if wxUSE_BMPBUTTON

namespace wxPrivate
{

// Update the button created by wxBitmapButton::NewCloseButton() to use the
// given background colour.
//
// This must be called when the parent background colour changes because the
// bitmaps of the close button may use it under some platforms.
void SetCloseButtonBackground(wxBitmapButton* button, const wxColour& colBg);

} // namespace wxPrivate

#endif // wxUSE_BMPBUTTON

#endif // _WX_PRIVATE_BMPBUTTN_H_
