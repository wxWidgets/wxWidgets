#ifndef _WX_WAVE_H_BASE_
#define _WX_WAVE_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/wave.h"
#elif defined(__WXGTK__)
#include "wx/unix/wave.h"
#elif defined(__WXX11__)
#include "wx/unix/wave.h"
#elif defined(__WXMOTIF__)
#include "wx/unix/wave.h"
#elif defined(__WXMAC__)
#include "wx/mac/wave.h"
#elif defined(__WXPM__)
#include "wx/os2/wave.h"
#endif

#endif
    // _WX_TREECTRL_H_BASE_
