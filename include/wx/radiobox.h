///////////////////////////////////////////////////////////////////////////////
// Name:        wx/radiobox.h
// Purpose:     wxRadioBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_BASE_
#define _WX_RADIOBOX_H_BASE_

#if wxUSE_RADIOBOX

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxRadioBoxNameStr;

#if defined(__WXMSW__)
    #include "wx/msw/radiobox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/radiobox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/radiobox.h"
#elif defined(__WXQT__)
    #include "wx/qt/radiobox.h"
#elif defined(__WXMAC__)
    #include "wx/mac/radiobox.h"
#elif defined(__WXPM__)
    #include "wx/os2/radiobox.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/radiobox.h"
#endif

#endif // wxUSE_RADIOBOX

#endif
    // _WX_RADIOBOX_H_BASE_
