#ifndef _WX_COLORDLG_H_BASE_
#define _WX_COLORDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/colordlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXQT__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXMAC__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXPM__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/colrdlgg.h"
#endif

#if !defined(__WXMSW__)
#define wxColourDialog wxGenericColourDialog
#define sm_classwxColourDialog sm_classwxGenericColourDialog
#endif

// get the colour from user and return it
wxColour WXDLLEXPORT wxGetColourFromUser(wxWindow *parent = (wxWindow *)NULL,
                                         const wxColour& colInit = wxNullColour);

#endif
    // _WX_COLORDLG_H_BASE_
