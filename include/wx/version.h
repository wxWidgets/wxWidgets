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

#ifndef _WX_VERSION_H_
#define _WX_VERSION_H_

// Bump-up with each new version
#define wxMAJOR_VERSION    2
#define wxMINOR_VERSION    4
#define wxRELEASE_NUMBER   1
#define wxVERSION_STRING   _T("wxWindows 2.4.1")

// These are used by src/msw/version.rc and should always be ASCII, not Unicode
// and must be updated manually as well each time the version above changes
#define wxVERSION_NUM_DOT_STRING   "2.4.1"
#define wxVERSION_NUM_STRING       "241"

// nothing should be updated below this line when updating the version

#define wxVERSION_NUMBER (wxMAJOR_VERSION * 1000) + (wxMINOR_VERSION * 100) + wxRELEASE_NUMBER
#define wxBETA_NUMBER      0
#define wxVERSION_FLOAT wxMAJOR_VERSION + (wxMINOR_VERSION/10.0) + (wxRELEASE_NUMBER/100.0) + (wxBETA_NUMBER/10000.0)

// check if the current version is at least major.minor.release
#define wxCHECK_VERSION(major,minor,release) \
    (wxMAJOR_VERSION > (major) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION > (minor)) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION == (minor) && wxRELEASE_NUMBER >= (release)))

#endif // _WX_VERSION_H_

