#ifndef _WX_TREECTRL_H_BASE_
#define _WX_TREECTRL_H_BASE_

#include "wx/treebase.h"

// ----------------------------------------------------------------------------
// include the platform-dependent wxTreeCtrl class
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXMSW__)
        #include "wx/msw/treectrl.h"
#elif defined(__WXMOTIF__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXGTK__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXMAC__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXCOCOA__)
    #include "wx/generic/treectlg.h"
#elif defined(__WXPM__)
    #include "wx/generic/treectlg.h"
#endif

/*
#if !defined(__WXMSW__)
#define wxTreeCtrl wxGenericTreeCtrl
#endif
*/

#endif // _WX_TREECTRL_H_BASE_

