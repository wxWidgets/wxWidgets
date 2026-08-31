/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostinput.h
// Purpose:     Private stateless boundaries for existing host input adapters
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTINPUT_H_
#define _WX_WINUI_HOSTINPUT_H_

#include "wx/winui/private/inputstate.h"
#include "wx/msw/wrapwin.h"

// Constructors, cursor policy and the native bridge share these exact queries.
// There is no router/capture state here; the host and slots retain their sole
// wxWinUIInputState instances and existing generation-qualified records.
wxWinUIClickSettings wxWinUIGetClickSettings();
bool wxWinUIShouldEmitSetCursor(wxWinUIInputKind kind);
UINT wxWinUIInternalCancelMessage();

#endif // _WX_WINUI_HOSTINPUT_H_
