/////////////////////////////////////////////////////////////////////////////
// Name:        wx_cw_d_no_mch.h
// Purpose:     wxWindows definitions for CodeWarrior builds (Debug w/o precompiled hdrs)
// Author:      Stefan Csomor
// Modified by:
// Created:     12/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CW__
#define _WX_CW__

#if __option(profile)
#error "profiling is not supported in debug versions"
#else
#ifdef __cplusplus
    #include <wx_cw++_d.h>
#else
    #include <wx_cw_d.h>
#endif
#endif

#endif
    // _WX_CW__
