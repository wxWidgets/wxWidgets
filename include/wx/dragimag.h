#ifndef _WX_DRAGIMAG_H_BASE_
#define _WX_DRAGIMAG_H_BASE_

#if defined(__WXMSW__)
#ifdef __WIN16__
#include "wx/generic/dragimag.h"
#else
#include "wx/msw/dragimag.h"
#endif
#elif defined(__WXMOTIF__)
#include "wx/generic/dragimag.h"
#elif defined(__WXGTK__)
#include "wx/generic/dragimag.h"
#elif defined(__WXQT__)
#include "wx/generic/dragimag.h"
#elif defined(__WXMAC__)
#include "wx/generic/dragimag.h"
#elif defined(__WXPM__)
#include "wx/generic/dragimag.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/dragimag.h"
#endif

#endif
    // _WX_DRAGIMAG_H_BASE_
