#ifndef _WX_TREECTRL_H_BASE_
#define _WX_TREECTRL_H_BASE_

#include "wx/treebase.h"

// ----------------------------------------------------------------------------
// include the platform-dependent wxTreeCtrl class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #ifdef __WIN16__
        #include "wx/generic/treectlg.h"
    #else
        #include "wx/msw/treectrl.h"
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXGTK__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXQT__)
    #include "wx/qt/treectrl.h"
#elif defined(__WXMAC__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXPM__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXSTUBS__)
    #include "wx/generic/treectlg.h"
#endif

#if !defined(__WXMSW__)
#define wxTreeCtrl wxGenericTreeCtrl
#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif

#endif
    // _WX_TREECTRL_H_BASE_


