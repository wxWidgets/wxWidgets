///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/private/bgtask.h
// Purpose:     Periodic background task run from the classic Mac event loop
// Created:     2026-11-13
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_PRIVATE_BGTASK_H_
#define _WX_RETRO68_PRIVATE_BGTASK_H_

#include "wx/defs.h"

// Classic Mac OS has no preemptive threads, so the code which needs to run
// regularly in the background (e.g. feeding the Sound Manager with audio data
// for wxSound) must be called from the main event loop.
//
// While a task is registered, the event loop doesn't sleep for more than one
// tick (1/60s) and calls it on every iteration. Note that it is not called
// while the Toolbox itself runs a modal loop, e.g. when tracking a menu or
// dragging a window.
//
// Only a single task is currently supported, registering a new one replaces
// the previous one and passing nullptr unregisters it.
typedef void (*wxRetro68BackgroundTask)();

WXDLLIMPEXP_CORE void wxRetro68SetBackgroundTask(wxRetro68BackgroundTask task);

// Run the background task, if any, right now.
WXDLLIMPEXP_CORE void wxRetro68RunBackgroundTask();

#endif // _WX_RETRO68_PRIVATE_BGTASK_H_
