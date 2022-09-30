///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/threads.h
// Purpose:     Wrappers for GDK threads support.
// Author:      Vadim Zeitlin
// Created:     2022-09-23
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_THREADS_H_
#define _WX_GTK_PRIVATE_THREADS_H_

// ----------------------------------------------------------------------------
// Redefine GDK functions to avoiding deprecation warnings
// ----------------------------------------------------------------------------

wxGCC_WARNING_SUPPRESS(deprecated-declarations)

static inline void wx_gdk_threads_enter() { gdk_threads_enter(); }
#define gdk_threads_enter wx_gdk_threads_enter

static inline void wx_gdk_threads_leave() { gdk_threads_leave(); }
#define gdk_threads_leave wx_gdk_threads_leave

wxGCC_WARNING_RESTORE(deprecated-declarations)

// ----------------------------------------------------------------------------
// RAII wrapper for acquiring/leaving GDK lock in ctor/dtor
// ----------------------------------------------------------------------------

class wxGDKThreadsLock
{
public:
    wxGDKThreadsLock() { gdk_threads_enter(); }
   ~wxGDKThreadsLock() { gdk_threads_leave(); }

   wxDECLARE_NO_COPY_CLASS(wxGDKThreadsLock);
};

#endif // _WX_GTK_PRIVATE_THREADS_H_
