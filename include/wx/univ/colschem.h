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

class WXDLLEXPORT wxWindow;

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
        // the background colour for a window
        WINDOW,

        // the different background and text colours for the control
        CONTROL,
        CONTROL_PRESSED,
        CONTROL_CURRENT,

        // the label text for the normal and the disabled state
        CONTROL_TEXT,
        CONTROL_TEXT_DISABLED,
        CONTROL_TEXT_DISABLED_SHADOW,

        // the scrollbar background colour for the normal and pressed states
        SCROLLBAR,
        SCROLLBAR_PRESSED,

        // the background and text colour for the highlighted item
        HIGHLIGHT,
        HIGHLIGHT_TEXT,

        // these colours are used for drawing the shadows of 3D objects
        SHADOW_DARK,
        SHADOW_HIGHLIGHT,
        SHADOW_IN,
        SHADOW_OUT,

        MAX
    };

    // get a standard colour
    virtual wxColour Get(StdColour col) const = 0;

    // get the background colour for the given window
    virtual wxColour GetBackground(wxWindow *win) const = 0;

    // virtual dtor for any base class
    virtual ~wxColourScheme();
};

// some people just can't spell it correctly :-)
typedef wxColourScheme wxColorScheme;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// retrieve the default colour from the theme or the given scheme
#define wxSCHEME_COLOUR(scheme, what) scheme->Get(wxColorScheme::what)
#define wxTHEME_COLOUR(what) wxSCHEME_COLOUR(wxTheme::Get(), what)

// get the background colour for the window in the current theme
#define wxTHEME_BG_COLOUR(win) \
    wxTheme::Get()->GetColourScheme()->GetBackground(win)

#endif // _WX_UNIV_COLSCHEM_H_
