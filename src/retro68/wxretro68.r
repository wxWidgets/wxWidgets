/*
 * Name:        src/retro68/wxretro68.r
 * Purpose:     Default resources for wxWidgets applications under Retro68
 * Created:     2026-11-12
 * Copyright:   (c) 2026 wxWidgets development team
 * Licence:     wxWindows licence
 *
 * This file should be passed to Rez after the standard Retro68 application
 * template (Retro68APPL.r, RetroPPCAPPL.r or RetroCarbonAPPL.r) to override
 * the SIZE resource defined there: wxWidgets applications need more memory
 * than the default 1MB and must handle suspend/resume and Apple events.
 *
 * Define WX_RETRO68_PREFERRED_SIZE and WX_RETRO68_MINIMUM_SIZE (in bytes) to
 * override the default memory partition sizes.
 */

#include "Processes.r"

#ifndef WX_RETRO68_PREFERRED_SIZE
#define WX_RETRO68_PREFERRED_SIZE (16 * 1024 * 1024)
#endif

#ifndef WX_RETRO68_MINIMUM_SIZE
#define WX_RETRO68_MINIMUM_SIZE (8 * 1024 * 1024)
#endif

resource 'SIZE' (-1) {
    reserved,
    acceptSuspendResumeEvents,
    reserved,
    canBackground,
    doesActivateOnFGSwitch,
    backgroundAndForeground,
    dontGetFrontClicks,
    ignoreChildDiedEvents,
    is32BitCompatible,
    isHighLevelEventAware,
    onlyLocalHLEvents,
    notStationeryAware,
    dontUseTextEditServices,
    reserved,
    reserved,
    reserved,
    WX_RETRO68_PREFERRED_SIZE,
    WX_RETRO68_MINIMUM_SIZE
};
