#ifndef _WX_COMBOBOX_H_BASE_
#define _WX_COMBOBOX_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/combobox.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/combobox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/combobox.h"
#elif defined(__WXQT__)
#include "wx/qt/combobox.h"
#elif defined(__WXMAC__)
#include "wx/mac/combobox.h"
#elif defined(__WXPM__)
#include "wx/os2/combobox.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/combobox.h"
#endif

#endif
    // _WX_COMBOBOX_H_BASE_
