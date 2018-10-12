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
 * Audio System
 */

#define wxOSX_USE_QUICKTIME 0
#define wxOSX_USE_AUDIOTOOLBOX 1

/*
   Use the more efficient FSEvents API instead of kqueue
   events for file system watcher since that version introduced a flag that
   allows watching files as well as sub directories.
 */
#define wxHAVE_FSEVENTS_FILE_NOTIFICATIONS 1

/*
 * turn off old style icon format if not asked for
 */
#ifndef wxOSX_USE_ICONREF
    #define wxOSX_USE_ICONREF 0
#endif

/*
 * turning off capabilities that don't work under cocoa yet
 */

#endif
    /* _WX_MAC_CHKCONF_H_ */

