///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/cairo.h
// Purpose:     Wrapper for including cairo.h from wx code
// Author:      Vadim Zeitlin
// Created:     2026-02-23
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_CAIRO_H_
#define _WX_PRIVATE_CAIRO_H_

// We want to prevent cairo.h from using dllimport attribute for the functions
// it declares as we're loading them dynamically, but this is trickier than it
// should be because older versions of cairo.h respected cairo_public being
// defined before including it, but this was broken in 1.18, so we need to
// define an alternative symbol and avoid defining cairo_public as this would
// result in a warning about its redefinition.
#include <cairo-version.h>

#if CAIRO_VERSION_MAJOR > 1 || CAIRO_VERSION_MINOR >= 18
#define CAIRO_WIN32_STATIC_BUILD
#else
#define cairo_public
#endif

#include <cairo.h>

#endif // _WX_PRIVATE_CAIRO_H_
