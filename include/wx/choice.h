#ifndef _WX_CHOICE_H_BASE_
#define _WX_CHOICE_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/choice.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/choice.h"
#elif defined(__WXGTK__)
#include "wx/gtk/choice.h"
#elif defined(__WXQT__)
#include "wx/qt/choice.h"
#elif defined(__WXMAC__)
#include "wx/mac/choice.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/choice.h"
#endif

#endif
    // _WX_CHOICE_H_BASE_
