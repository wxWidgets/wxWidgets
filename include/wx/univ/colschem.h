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
        // these colours are used for drawing the shadows of 3D objects, use
        // only FACE in the renderers which don't use 3D shading
        DARK_SHADOW,
        FACE,
        HIGHLIGHT,
        LIGHT,
        SHADOW,

        // the default colours for the controls
        CONTROL_TEXT,
        HIGHLIGHT,
        HIGHLIGHT_TEXT,

        MAX
    };

    virtual wxColour Get(StdColour col) = 0;
};

// some people just can't spell it correctly :-)
typedef wxColourScheme wxColorScheme;

#endif // _WX_UNIV_COLSCHEM_H_
