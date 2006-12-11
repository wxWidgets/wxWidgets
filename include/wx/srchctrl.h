/////////////////////////////////////////////////////////////////////////////
// Name:        srchctrl.h
// Purpose:     wxSearchCtrlBase class
// Author:      Vince Harron
// Modified by:
// Created:     2006-02-18
// RCS-ID:      
// Copyright:   (c) Vince Harron
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEARCHCTRL_H_BASE_
#define _WX_SEARCHCTRL_H_BASE_

#include "wx/defs.h"

#if wxUSE_SEARCHCTRL

#include "wx/textctrl.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

extern WXDLLEXPORT_DATA(const wxChar) wxSearchCtrlNameStr[];

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SEARCHCTRL_CANCEL, 1119)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SEARCHCTRL_SEARCH, 1120)
END_DECLARE_EVENT_TYPES()

// ----------------------------------------------------------------------------
// a search ctrl is a text control with a search button and a cancel button
// it is based on the MacOSX 10.3 control HISearchFieldCreate
// ----------------------------------------------------------------------------

// include the platform-dependent class implementation
#if !defined(__WXUNIVERSAL__) && defined(__WXMAC__) && defined(__WXMAC_OSX__) \
        && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
    // search control was introduced in Mac OS X 10.3 Panther
    #define wxUSE_NATIVE_SEARCH_CONTROL 1
#else
    // no native version, use the generic one
    #define wxUSE_NATIVE_SEARCH_CONTROL 0
#endif

#if wxUSE_NATIVE_SEARCH_CONTROL
    #if defined(__WXMAC__)
        #include "wx/mac/srchctrl.h"
    #endif
#else
    #include "wx/generic/srchctlg.h"
#endif

// ----------------------------------------------------------------------------
// macros for handling search events
// ----------------------------------------------------------------------------

#define EVT_SEARCHCTRL_CANCEL(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_SEARCHCTRL_CANCEL, id, wxCommandEventHandler(fn))

#define EVT_SEARCHCTRL_SEARCH(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_SEARCHCTRL_SEARCH, id, wxCommandEventHandler(fn))

#endif // wxUSE_SEARCHCTRL

#endif // _WX_SEARCHCTRL_H_BASE_
