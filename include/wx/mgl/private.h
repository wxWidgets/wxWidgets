/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWindows itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Vaclav Slavik
// Created:     2001/04/07
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include <mgraph.hpp>

// ---------------------------------------------------------------------------
// private variables
// ---------------------------------------------------------------------------

extern MGLDevCtx *g_displayDC;


// ---------------------------------------------------------------------------
// helper functions
// ---------------------------------------------------------------------------


// This function converts wxBitmap into pixpattern24_t representation
// (used by wxBrush and wxPen)

extern void wxBitmapToPixPattern(const wxBitmap& bitmap, 
                                 pixpattern24_t *pix, pattern_t *mask);
                       

// Sets current DC and restores previous one upon destruction:
class wxCurrentDCSwitcher
{
    public:
        wxCurrentDCSwitcher(MGLDevCtx *dc)
                { m_old = dc->makeCurrent(); }
        ~wxCurrentDCSwitcher()
                { MGL_makeCurrentDC(m_old); }
        MGLDC *m_old;
};


#endif // _WX_PRIVATE_H_
