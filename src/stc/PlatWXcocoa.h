/////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/PlatWXcocoa.h
// Purpose:     Declaration of utility functions for wxSTC with cocoa
// Author:      New Pagodi
// Created:     2019-03-10
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SRC_STC_PLATWXCOCOA_H_
#define _SRC_STC_PLATWXCOCOA_H_

#include "wx/defs.h"

#if wxUSE_STC

// Functions used to create and manage popup windows.
WX_NSWindow CreateFloatingWindow(wxWindow*);
void CloseFloatingWindow(WX_NSWindow win);
void ShowFloatingWindow(WX_NSWindow win);
void HideFloatingWindow(WX_NSWindow win);

// Function needed for list control colours.
wxColour GetListHighlightColour();

#endif // wxUSE_STC

#endif // _SRC_STC_PLATWXCOCOA_H_
