///////////////////////////////////////////////////////////////////////////////
// Name:        drawer.h
// Purpose:     wxDrawerWindow class
// Author:      Ryan Norton
// Modified by:
// Created:     9/27/2004
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAWER_H_BASE_
#define _WX_DRAWER_H_BASE_

#include "wx/defs.h"

#if defined(__WXMAC_OSX__) && ( MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_2 )
#	include "wx/mac/carbon/drawer.h"
#else
#	include "wx/generic/drawerg.h"
#endif

#endif // _WX_DRAWER_H_BASE_

