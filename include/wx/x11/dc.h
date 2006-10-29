/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#ifndef MM_TEXT
#define MM_TEXT         0
#define MM_ISOTROPIC    1
#define MM_ANISOTROPIC  2
#define MM_LOMETRIC     3
#define MM_HIMETRIC     4
#define MM_TWIPS        5
#define MM_POINTS       6
#define MM_METRIC       7
#endif

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC : public wxDCBase
{
public:
    wxDC();
    virtual ~wxDC() { }

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);
    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);

protected:
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height);
    virtual void DoGetSizeMM(int* width, int* height) const;

public:
    virtual void ComputeScaleAndOrigin();

    wxCoord XDEV2LOG(wxCoord x) const
    {
        return wxCoordRound((double)(x - m_deviceOriginX) / m_scaleX) * m_signX + m_logicalOriginX;
    }
    wxCoord XDEV2LOGREL(wxCoord x) const
    {
        return wxCoordRound((double)(x) / m_scaleX);
    }
    wxCoord YDEV2LOG(wxCoord y) const
    {
        return wxCoordRound((double)(y - m_deviceOriginY) / m_scaleY) * m_signY + m_logicalOriginY;
    }
    wxCoord YDEV2LOGREL(wxCoord y) const
    {
        return wCoordRound((double)(y) / m_scaleY);
    }
    wxCoord XLOG2DEV(wxCoord x) const
    {
        return wxCoordRound((double)(x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX;
    }
    wxCoord XLOG2DEVREL(wxCoord x) const
    {
        return wxCoordRound((double)(x) * m_scaleX);
    }
    wxCoord YLOG2DEV(wxCoord y) const
    {
        return wxCoordRound((double)(y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY;
    }
    wxCoord YLOG2DEVREL(wxCoord y) const
    {
        return wxCoordRound((double)(y) * m_scaleY);
    }

public:
    // not sure what for, but what is a mm on a screen you don't know the size of?
    double       m_mm_to_pix_x,m_mm_to_pix_y;

    // recompute scale?
    bool         m_needComputeScaleX, m_needComputeScaleY;


private:
    DECLARE_ABSTRACT_CLASS(wxDC)
};

#endif
// _WX_DC_H_
