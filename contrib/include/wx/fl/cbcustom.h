/////////////////////////////////////////////////////////////////////////////
// Name:        cbcustom.h
// Purpose:     cbSimpleCustomizationPlugin class declaration
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     28/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __CBCUSTOM_G__
#define __CBCUSTOM_G__

#include "wx/fl/controlbar.h"

/*
This class enables customization of a bar, popping up a
menu and handling basic customization such as floating
and horizontal/vertical alignment of the bar.
*/

class WXDLLIMPEXP_FL cbSimpleCustomizationPlugin : public cbPluginBase
{
public:
    DECLARE_DYNAMIC_CLASS( cbSimpleCustomizationPlugin )

    int mCustMenuItemId;
public:

        // Default constructor.
    cbSimpleCustomizationPlugin(void);

        // Constructor, taking parent pane and a pane mask flag.
    cbSimpleCustomizationPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Plugin event handler for cbCustomizeBarEvent.
    void OnCustomizeBar( cbCustomizeBarEvent& event );

        // Plugin event handler for cbCustomizeLayoutEvent.
    void OnCustomizeLayout( cbCustomizeLayoutEvent& event );

        // Menu event handler.
    void OnMenuItemSelected( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __CBCUSTOM_G__ */

