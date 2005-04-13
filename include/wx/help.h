/////////////////////////////////////////////////////////////////////////////
// Name:        help.h
// Purpose:     wxHelpController base header
// Author:      wxWidgets Team
// Modified by:
// Created:
// Copyright:   (c) wxWidgets Team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#include "wx/defs.h"

#if wxUSE_HELP

#include "wx/helpbase.h"

#ifdef __WXWINCE__
    #include "wx/msw/wince/helpwce.h"

    #define wxHelpController wxWinceHelpController
#elif defined(__WXPALMOS__)
    #include "wx/palmos/help.h"

    #define wxHelpController wxPalmHelpController
#elif defined(__WXMSW__)
    #include "wx/msw/helpbest.h"

    // CHM or WinHelp, whichever is available
    #define wxHelpController wxBestHelpController
#else // !MSW

#if wxUSE_WXHTML_HELP
    #include "wx/html/helpctrl.h"
    #define wxHelpController wxHtmlHelpController
#else
    #include "wx/generic/helpext.h"
    #define wxHelpController wxExtHelpController
#endif

#endif // MSW/!MSW

#endif // wxUSE_HELP

#endif
    // _WX_HELP_H_BASE_
