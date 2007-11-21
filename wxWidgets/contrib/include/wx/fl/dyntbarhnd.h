/////////////////////////////////////////////////////////////////////////////
// Name:        dyntbarhnd.h
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNTBARHND_G__
#define __DYNTBARHND_G__

#include "wx/fl/controlbar.h"
#include "wx/fl/dyntbar.h"

/*
Dynamic toolbar dimension handler.
*/

class WXDLLIMPEXP_FL cbDynToolBarDimHandler : public cbBarDimHandlerBase
{
    DECLARE_DYNAMIC_CLASS( cbDynToolBarDimHandler )
public:

        // Called when the bar changes state.

    void OnChangeBarState(cbBarInfo* pBar, int newState );

        // Called when a bar is resized.

    void OnResizeBar( cbBarInfo* pBar, const wxSize& given, wxSize& preferred );
};

#endif /* __DYNTBARHND_G__ */

