/*
 * Name:        wx/osx/cocoa/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Stefan Csomor
 * Modified by:
 * Created:     2008-07-30
 * Copyright:   (c) Stefan Csomor
 * Licence:     wxWindows licence
 */

#ifndef _WX_OSX_COCOA_CHKCONF_H_
#define _WX_OSX_COCOA_CHKCONF_H_

/*
 * native (1) or emulated (0) toolbar
 */

#ifndef wxOSX_USE_NATIVE_TOOLBAR
    #define wxOSX_USE_NATIVE_TOOLBAR 1
#endif

/*
 * leave is isFlipped and don't override
 */
#ifndef wxOSX_USE_NATIVE_FLIPPED 
    #define wxOSX_USE_NATIVE_FLIPPED 1
#endif

/*
 * text rendering system
 */

#define wxOSX_USE_ATSU_TEXT 0

/*
 * Audio System
 */

#define wxOSX_USE_QUICKTIME 0
#define wxOSX_USE_AUDIOTOOLBOX 1

/*
   Use the more efficient FSEvents API instead of kqueue
   events for file system watcher, but only on OS X >= 10.7 since
   that version introduced a flag that allows watching files as
   well as sub directories.
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
    #define wxHAVE_FSEVENTS_FILE_NOTIFICATIONS 1
#endif

/*
 * turning off capabilities that don't work under cocoa yet
 */

#endif
    /* _WX_MAC_CHKCONF_H_ */

