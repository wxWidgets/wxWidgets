/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:     Various OS/2-specific global data
// Author:      David Webster
// Modified by:
// Created:     09/20/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// Current cursor, in order to hang on to
// cursor handle when setting the cursor globally
wxCursor *g_globalCursor = NULL;

wxNativeFontInfo wxNullNativeFontInfo;

