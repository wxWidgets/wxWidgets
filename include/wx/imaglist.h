#ifndef _WX_IMAGLIST_H_BASE_
#define _WX_IMAGLIST_H_BASE_

#if defined(__WXMSW__)
#ifdef __WIN16__
#include "wx/generic/imaglist.h"
#else
#include "wx/msw/imaglist.h"
#endif
#elif defined(__WXMOTIF__)
#include "wx/generic/imaglist.h"
#elif defined(__WXGTK__)
#include "wx/generic/imaglist.h"
#elif defined(__WXQT__)
#include "wx/generic/imaglist.h"
#elif defined(__WXMAC__)
#include "wx/generic/imaglist.h"
#elif defined(__WXPM__)
#include "wx/generic/imaglist.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/imaglist.h"
#endif

#endif
    // _WX_IMAGLIST_H_BASE_
