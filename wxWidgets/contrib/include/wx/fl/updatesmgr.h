/////////////////////////////////////////////////////////////////////////////
// Name:        updatesmgr.h
// Purpose:     cbSimpleUpdatesMgr class declaration
// Author:      Aleksandras Gluchovas (@Lithuania)
// Modified by:
// Created:     19/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __UPDATESMGR_G__
#define __UPDATESMGR_G__

#include "wx/fl/controlbar.h"

/*
This class implements slightly optimized logic for refreshing
the areas of frame layout that actually need to be updated.
*/

class WXDLLIMPEXP_FL cbSimpleUpdatesMgr : public cbUpdatesManagerBase
{
    DECLARE_DYNAMIC_CLASS( cbSimpleUpdatesMgr )
protected:

        // Helper function.
    bool WasChanged( cbUpdateMgrData& data, wxRect& currentBounds );

public:
        // Default constructor.
    cbSimpleUpdatesMgr() {}

        // Constructor taking frame layout panel.
    cbSimpleUpdatesMgr( wxFrameLayout* pPanel );

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnStartChanges();

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnRowWillChange( cbRowInfo* pRow, cbDockPane* pInPane );

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnBarWillChange( cbBarInfo* pBar, cbRowInfo* pInRow, cbDockPane* pInPane );

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnPaneMarginsWillChange( cbDockPane* pPane );

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnPaneWillChange( cbDockPane* pPane );

        // Notification received from Frame Layout in the order in which
        // they would usually be invoked.
    virtual void OnFinishChanges();

        // Refreshes the parts of the frame layoutthat need an update.
    virtual void UpdateNow();
};

#endif /* __UPDATESMGR_G__ */

