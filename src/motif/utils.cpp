/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/setup.h"

#ifdef __VMS
#define XtDisplay XTDISPLAY
#include "[-.x11]utils.cpp"
#else

#include "../src/x11/utils.cpp"
#endif
//#error no longer needed - wxX11 utils.cpp used instead

