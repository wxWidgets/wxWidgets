/////////////////////////////////////////////////////////////////////////////
// Name:        effects.h
// Purpose:     wxEffects class
//              Draws 3D effects.
// Author:      Julian Smart et al
// Modified by:
// Created:     25/4/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "effects.h"
#endif

#ifndef _WX_EFFECTS_H_
#define _WX_EFFECTS_H_

/*
 * wxEffects: various 3D effects
 */

class WXDLLEXPORT wxEffects: public wxObject
{
DECLARE_CLASS(wxEffects)

public:
    // Assume system colours
    wxEffects() ;
    // Going from lightest to darkest
    wxEffects(const wxColour& highlightColour, const wxColour& lightShadow,
                const wxColour& faceColour, const wxColour& mediumShadow, const wxColour& darkShadow) ;

    // Draw a sunken edge
    void DrawSunkenEdge(wxDC& dc, const wxRect& rect, int borderSize = 1);

    // Tile a bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);
protected:
    wxColour    m_highlightColour;  // Usually white
    wxColour    m_lightShadow;      // Usually light grey
    wxColour    m_faceColour;       // Usually grey
    wxColour    m_mediumShadow;     // Usually dark grey
    wxColour    m_darkShadow;       // Usually black
};

#endif

