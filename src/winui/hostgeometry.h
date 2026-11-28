/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostgeometry.h
// Purpose:     Intracore host authorities used by geometry projection
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTGEOMETRY_H_
#define _WX_WINUI_HOSTGEOMETRY_H_

// Cursor authority remains in the host. Projection observes the current
// generation again after setters; test-enabled builds also record successful
// sets without changing the production path.
unsigned long long wxWinUIGetCursorPolicyGeneration();
#ifdef WXWINUI_TEST_SUPPORT
void wxWinUINoteSlotCursorSet();
#endif

#endif // _WX_WINUI_HOSTGEOMETRY_H_
