#ifndef _WX_COLOUR_H_BASE_
#define _WX_COLOUR_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/colour.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/colour.h"
#elif defined(__WXGTK__)
#include "wx/gtk/colour.h"
#elif defined(__WXMGL__)
#include "wx/mgl/colour.h"
#elif defined(__WXX11__)
#include "wx/x11/colour.h"
#elif defined(__WXMAC__)
#include "wx/mac/colour.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/colour.h"
#elif defined(__WXPM__)
#include "wx/os2/colour.h"
#endif

#define wxColor wxColour

#endif
    // _WX_COLOUR_H_BASE_
