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

// Cursor authority/counters remain in the host; projection observes the
// current generation again after setters and records only successful sets.
unsigned long long wxWinUIGetCursorPolicyGeneration();
void wxWinUINoteSlotCursorSet();

#endif // _WX_WINUI_HOSTGEOMETRY_H_
