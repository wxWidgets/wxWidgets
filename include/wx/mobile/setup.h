/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/setup.h
// Purpose:     Setup for mobile platforms
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence: wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#ifndef _WX_MOBILE_SETUP_H_
#define _WX_MOBILE_SETUP_H_

/*

This file defines the particular permutation of native and generic controls
in a particular wxMobile build. An individual wxMobile control header,
such as wx/mobile/button.h, relies on this file to determine whether it should
use the native or generic version. (A header may also override these settings
if for a particular platform, a special implementation will always be used -
neither the normal native control nor the default generic implementation.)

Possible scenarios:

1) Everything defined natively, for example targetting iPhone. No emulator.
2) Desktop target, with simulation of all controls and mobile device simulator
   (top-level frames are shown within a rough rendering of a device, with
   controls for changing orientation, etc.)
   Within this category, there may also be adaptation of simulated controls for
   a particular mobile look and feel, such as iPhone or WinCE. This may include
   GUI decisions such as scrollbars/no scrollbars.
3) Mobile target such as WinMobile, with simulation of only some controls and no
   mobile device simulator.

So this setup file must be able to encompass different combinations. Ideally
there would be a matching configure switch such as --with-mobile=emulation
and --with-mobile=winmobile-emulation (on the iPhone there would be just one
standard build so it would be unnecessary to specify the wxMobile configuration).
But it's unrealistic to plan to create individual permuations of emulated/native
controls via configure.

Note:

A generic control may in fact be mostly the native implementation, perhaps
with a few extra functions (typically to cater for different scrolling
behaviour).

*/

// Possible values. Edit this file and set a value, or pass it via
// project or configure flags.

// iPhone - native iPhone controls
// #define wxMOBILE_HAVE_IPHONE               0

// WinMobile - a combination of native and generic controls
// #define wxMOBILE_HAVE_WINMOBILE            0

// Generic iPhone - simulation with iPhone look and feel
// #define wxMOBILE_HAVE_GENERIC_IPHONE       0

// Generic WinMobile - simulation with WinMobile look and feel
// #define wxMOBILE_HAVE_GENERIC_WINMOBILE    0

// Custom - simulation, with your own combination of native and generic controls
// #define wxMOBILE_HAVE_GENERIC_CUSTOM       0

// If none is defined, decide on the most appropriate selection.
#if !defined(wxMOBILE_HAVE_IPHONE) && !defined(wxMOBILE_HAVE_WINMOBILE) && !defined(wxMOBILE_HAVE_GENERIC_IPHONE) && !defined(wxMOBILE_HAVE_GENERIC_CUSTOM)

#if wxOSX_USE_IPHONE
#define wxMOBILE_HAVE_IPHONE                1
#define wxMOBILE_HAVE_WINMOBILE             0
#define wxMOBILE_HAVE_GENERIC_IPHONE        0
#define wxMOBILE_HAVE_GENERIC_CUSTOM        0
#else
// A generic (simulated) wxMobile configuration is the only other one currently allowed.
#define wxMOBILE_HAVE_IPHONE                0
#define wxMOBILE_HAVE_WINMOBILE             0
#define wxMOBILE_HAVE_GENERIC_IPHONE        1
#define wxMOBILE_HAVE_GENERIC_CUSTOM        0
#endif

#endif
    // !defined(...) && ...

// Select the combination of native and generic controls for the selected wxMobile configuration
#if wxMOBILE_HAVE_IPHONE

#define wxUSE_MOBILE_NATIVE_APP                    1
#define wxUSE_MOBILE_NATIVE_BARBUTTON              1
#define wxUSE_MOBILE_NATIVE_BITMAPBUTTON           1
#define wxUSE_MOBILE_NATIVE_BUTTON                 1
#define wxUSE_MOBILE_NATIVE_FRAME                  1
#define wxUSE_MOBILE_NATIVE_GAUGE                  1
#define wxUSE_MOBILE_NATIVE_KEYBOARD               1
#define wxUSE_MOBILE_NATIVE_LISTBOX                1
#define wxUSE_MOBILE_NATIVE_NAVBAR                 1
#define wxUSE_MOBILE_NATIVE_NAVCTRL                1
#define wxUSE_MOBILE_NATIVE_NAVITEM                1
#define wxUSE_MOBILE_NATIVE_NOTEBOOK               1
#define wxUSE_MOBILE_NATIVE_PAGECTRL               1
#define wxUSE_MOBILE_NATIVE_PANEL                  1
#define wxUSE_MOBILE_NATIVE_SCROLLWIN              1
#define wxUSE_MOBILE_NATIVE_SEARCHCTRL             1
#define wxUSE_MOBILE_NATIVE_SEGCTRL                1
#define wxUSE_MOBILE_NATIVE_SHEETS                 1
#define wxUSE_MOBILE_NATIVE_SLIDER                 1
#define wxUSE_MOBILE_NATIVE_STATBMP                1
#define wxUSE_MOBILE_NATIVE_STATTEXT               1
#define wxUSE_MOBILE_NATIVE_SWITCHCTRL             1
#define wxUSE_MOBILE_NATIVE_TABCTRL                1
#define wxUSE_MOBILE_NATIVE_TABLECTRL              1
#define wxUSE_MOBILE_NATIVE_TEXTCTRL               1
#define wxUSE_MOBILE_NATIVE_TOOLBAR                1
#define wxUSE_MOBILE_NATIVE_UTILS                  1
#define wxUSE_MOBILE_NATIVE_VIEWCONTROLLER         1
#define wxUSE_MOBILE_NATIVE_VLBOX                  1
#define wxUSE_MOBILE_NATIVE_VSCROLL                1
#define wxUSE_MOBILE_NATIVE_WEBCTRL                1
#define wxUSE_MOBILE_NATIVE_WHEELSCTRL             1
#define wxUSE_MOBILE_NATIVE_WINDOW                 1

#else // wxMOBILE_HAVE_GENERIC_IPHONE, and others

#define wxUSE_MOBILE_NATIVE_APP                    0
#define wxUSE_MOBILE_NATIVE_BARBUTTON              0
#define wxUSE_MOBILE_NATIVE_BITMAPBUTTON           0
#define wxUSE_MOBILE_NATIVE_BUTTON                 0
#define wxUSE_MOBILE_NATIVE_FRAME                  0
#define wxUSE_MOBILE_NATIVE_GAUGE                  0
#define wxUSE_MOBILE_NATIVE_KEYBOARD               0
#define wxUSE_MOBILE_NATIVE_LISTBOX                0
#define wxUSE_MOBILE_NATIVE_NAVBAR                 0
#define wxUSE_MOBILE_NATIVE_NAVCTRL                0
#define wxUSE_MOBILE_NATIVE_NAVITEM                0
#define wxUSE_MOBILE_NATIVE_NOTEBOOK               0
#define wxUSE_MOBILE_NATIVE_PAGECTRL               0
#define wxUSE_MOBILE_NATIVE_PANEL                  0
#define wxUSE_MOBILE_NATIVE_SCROLLWIN              0
#define wxUSE_MOBILE_NATIVE_SEARCHCTRL             0
#define wxUSE_MOBILE_NATIVE_SEGCTRL                0
#define wxUSE_MOBILE_NATIVE_SHEETS                 0
#define wxUSE_MOBILE_NATIVE_SLIDER                 0
#define wxUSE_MOBILE_NATIVE_STATBMP                0
#define wxUSE_MOBILE_NATIVE_STATTEXT               0
#define wxUSE_MOBILE_NATIVE_SWITCHCTRL             0
#define wxUSE_MOBILE_NATIVE_TABCTRL                0
#define wxUSE_MOBILE_NATIVE_TABLECTRL              0
#define wxUSE_MOBILE_NATIVE_TEXTCTRL               0
#define wxUSE_MOBILE_NATIVE_TOOLBAR                0
#define wxUSE_MOBILE_NATIVE_UTILS                  0
#define wxUSE_MOBILE_NATIVE_VIEWCONTROLLER         0
#define wxUSE_MOBILE_NATIVE_VLBOX                  0
#define wxUSE_MOBILE_NATIVE_VSCROLL                0
#define wxUSE_MOBILE_NATIVE_WEBCTRL                0
#define wxUSE_MOBILE_NATIVE_WHEELSCTRL             0
#define wxUSE_MOBILE_NATIVE_WINDOW                 0

#endif
    // wxMobile configuration

#endif
    // _WX_MOBILE_SETUP_H_
