/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imaglist.h
// Purpose:     wxImageList base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGLIST_H_BASE_
#define _WX_IMAGLIST_H_BASE_

#if defined(__WIN32__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/imaglist.h"
#elif defined(__WXMAC_CARBON__)
    #include "wx/mac/imaglist.h"
#else
    #include "wx/generic/imaglist.h"
#endif

#endif
    // _WX_IMAGLIST_H_BASE_
