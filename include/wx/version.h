/////////////////////////////////////////////////////////////////////////////
// Name:        wx/version.h
// Purpose:     wxWindows version numbers
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VERSIONH__
#define _WX_VERSIONH__

// Bump-up with each new version
#define wxMAJOR_VERSION    2
#define wxMINOR_VERSION    2
#define wxRELEASE_NUMBER   4
#define wxVERSION_STRING   _T("wxWindows 2.2.4")
#define wxVERSION_NUMBER (wxMAJOR_VERSION * 1000) + (wxMINOR_VERSION * 100) + wxRELEASE_NUMBER
#define wxBETA_NUMBER      0
#define wxVERSION_FLOAT wxMAJOR_VERSION + (wxMINOR_VERSION/10.0) + (wxRELEASE_NUMBER/100.0) + (wxBETA_NUMBER/10000.0)

// check if the current version is at least major.minor.release
#define wxCHECK_VERSION(major,minor,release) \
    (wxMAJOR_VERSION > (major) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION > (minor)) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION == (minor) && wxRELEASE_NUMBER >= (release)))

#endif
// _WX_VERSIONH__
