///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/setup_inc.h
// Purpose:     OSX-specific setup.h options
// Author:      Stefan Csomor
// Modified by: Stefan Csomor
// Created:     2017-11-09
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Unix-specific options settings
// ----------------------------------------------------------------------------

// use wxSelectDispatcher class
#define wxUSE_SELECT_DISPATCHER 1

// use wxEpollDispatcher class (Linux only)
#define wxUSE_EPOLL_DISPATCHER 0

/*
 Use GStreamer for Unix.

 Default is 0 as this requires a lot of dependencies which might not be
 available.

 Recommended setting: 1 (wxMediaCtrl won't work by default without it).
 */
#define wxUSE_GSTREAMER 0

// This is only used under Unix, but needs to be defined here as it's checked
// by wx/unix/chkconf.h.
#define wxUSE_XTEST 0

// ----------------------------------------------------------------------------
// Mac-specific settings
// ----------------------------------------------------------------------------

#undef wxUSE_GRAPHICS_CONTEXT
#define wxUSE_GRAPHICS_CONTEXT 1


// things not implemented under Mac

#undef wxUSE_STACKWALKER
#define wxUSE_STACKWALKER 0

// wxWebKit is a wrapper for Apple's WebKit framework, use it if you want to
// embed the Safari browser control
// 0 by default because of Jaguar compatibility problems
#define wxUSE_WEBKIT        1


// Set to 0 for no libmspack
#define wxUSE_LIBMSPACK     0

// native toolbar does support embedding controls, but not complex panels, please test
#define wxOSX_USE_NATIVE_TOOLBAR 1

// make sure we have the proper dispatcher for the console event loop
#define wxUSE_SELECT_DISPATCHER 1
#define wxUSE_EPOLL_DISPATCHER 0

// set to 1 if you have older code that still needs icon refs
#define wxOSX_USE_ICONREF 0

// set to 0 if you have code that has problems with the new bitmap implementation
#define wxOSX_BITMAP_NATIVE_ACCESS 1

