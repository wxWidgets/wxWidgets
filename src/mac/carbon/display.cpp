/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     Mac implementation of wxDisplay class
// Author:      Brian Victor
// Modified by: Royce Mitchell III
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/dynarray.h"
#endif

#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <Displays.h>
    #include <Quickdraw.h>
#endif

#include "wx/display.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxDisplayMacPriv
{
public:
	GDHandle m_hndl;
};

size_t wxDisplayBase::GetCount()
{
    GDHandle hndl;
    size_t num = 0;
    hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        num++;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
    return num;
}

int wxDisplayBase::GetFromPoint(const wxPoint &p)
{
    GDHandle hndl;
    size_t num = 0;
    hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        Rect screenrect = (*hndl)->gdRect;
        if (p.x >= screenrect.left &&
            p.x <= screenrect.right &&
            p.y >= screenrect.top &&
            p.y <= screenrect.bottom)
        {
            return num;
        }
        num++;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
    return -1;
}

wxDisplay::wxDisplay(size_t index) : wxDisplayBase ( index ),
    m_priv ( new wxDisplayMacPriv() )
{
    GDHandle hndl;
    hndl = DMGetFirstScreenDevice(true);
    m_priv->m_hndl = NULL;
    while(hndl)
    {
        if (index == 0)
        {
            m_priv->m_hndl = hndl;
        }
        index--;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
}

wxRect wxDisplay::GetGeometry() const
{
    if (!(m_priv)) return wxRect(0, 0, 0, 0);
    if (!(m_priv->m_hndl)) return wxRect(0, 0, 0, 0);
    Rect screenrect = (*(m_priv->m_hndl))->gdRect;
    return wxRect( screenrect.left, screenrect.top, 
                   screenrect.right - screenrect.left, screenrect.bottom - screenrect.top);
}

int wxDisplay::GetDepth() const
{
    if (!(m_priv)) return 0;
    if (!(m_priv->m_hndl)) return 0;

    // This cryptic looking code is based on Apple's sample code:
    // http://developer.apple.com/samplecode/Sample_Code/Graphics_2D/GDevVideo/Gen.cp.htm
    return ((*(*(m_priv->m_hndl))->gdPMap)->pixelSize) & 0x0000FFFF;
}

wxString wxDisplay::GetName() const
{
    // Macs don't name their displays...
    return wxT("");
}

wxDisplay::~wxDisplay()
{
    if ( m_priv )
    {
        delete m_priv;
        m_priv = 0;
    }
}

#endif // wxUSE_DISPLAY
