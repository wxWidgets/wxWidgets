///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/colschem.h
// Purpose:     wxColourScheme class provides the colours to use for drawing
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_COLSCHEM_H_
#define _WX_UNIV_COLSCHEM_H_

#ifdef __GNUG__
    #pragma interface "colschem.h"
#endif

#include "wx/colour.h"

// ----------------------------------------------------------------------------
// wxColourScheme
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxColourScheme
{
public:
    // the standard colours
    enum StdColour
    {
        // the background and text colour for the control
        CONTROL,
        CONTROL_TEXT,

        // the background and text colour for the highlighted item
        HIGHLIGHT,
        HIGHLIGHT_TEXT,

        // these colours are used for drawing the shadows of 3D objects, use
        // only NORMAL in the renderers which don't use 3D shading
        SHADOW_DARK,
        SHADOW_HIGHLIGHT,
        SHADOW_IN,
        SHADOW_OUT,

        MAX
    };

    // get the colour for the control in the state (combination of
    // wxCONTROL_XXX values)
    virtual wxColour Get(StdColour col, int flags = 0) const = 0;

    // virtual dtor for any base class
    virtual ~wxColourScheme();
};

// some people just can't spell it correctly :-)
typedef wxColourScheme wxColorScheme;

#endif // _WX_UNIV_COLSCHEM_H_
