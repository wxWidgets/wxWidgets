/////////////////////////////////////////////////////////////////////////////
// Name:        version.h
// Purpose:     wxWindows version numbers
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __VERSIONH__
#define __VERSIONH__

/* Bump-up with each new version */
#define wxMAJOR_VERSION    2
#define wxMINOR_VERSION    0
#define wxRELEASE_NUMBER   0
#define wxVERSION_STRING "wxWindows 2.0"
#define wxVERSION_NUMBER (wxMAJOR_VERSION * 1000) + (wxMINOR_VERSION * 100) + wxRELEASE_NUMBER
#define wxBETA_NUMBER      12
#define wxVERSION_FLOAT float(wxMAJOR_VERSION + (wxMINOR_VERSION/10.0) + (wxRELEASE_NUMBER/100.0) + (wxBETA_NUMBER/10000.0))

#endif
	// __VERSIONH__
