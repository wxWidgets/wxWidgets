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

#ifdef __GNUG__
    #pragma interface "dc.h"
#endif

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
    DECLARE_DYNAMIC_CLASS(wxDC)

public:
    wxDC();
    ~wxDC() { }

    // implement base class pure virtuals
    // ----------------------------------

    virtual void DestroyClippingRegion();

    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);
    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(long x, long y);
    virtual void SetDeviceOrigin(long x, long y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);

protected:
    virtual void DoDrawIcon(const wxIcon& icon, long x, long y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, long x, long y,
                              bool useMask = FALSE);

    virtual void DoSetClippingRegion(long x, long y,
                                     long width, long height);
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

public:
    void ComputeScaleAndOrigin();

    long XDEV2LOG(long x) const
    {
        long new_x = x - m_deviceOriginX;
        if (new_x > 0)
            return (long)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
        else
            return (long)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
    }
    long XDEV2LOGREL(long x) const
    {
        if (x > 0)
            return (long)((double)(x) / m_scaleX + 0.5);
        else
            return (long)((double)(x) / m_scaleX - 0.5);
    }
    long YDEV2LOG(long y) const
    {
        long new_y = y - m_deviceOriginY;
        if (new_y > 0)
            return (long)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
        else
            return (long)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
    }
    long YDEV2LOGREL(long y) const
    {
        if (y > 0)
            return (long)((double)(y) / m_scaleY + 0.5);
        else
            return (long)((double)(y) / m_scaleY - 0.5);
    }
    long XLOG2DEV(long x) const
    {
        long new_x = x - m_logicalOriginX;
        if (new_x > 0)
            return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
        else
            return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
    }
    // Without device translation, for backing pixmap purposes
    long XLOG2DEV_2(long x) const
    {
        long new_x = x - m_logicalOriginX;
        if (new_x > 0)
            return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX;
        else
            return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX;
    }
    long XLOG2DEVREL(long x) const
    {
        if (x > 0)
            return (long)((double)(x) * m_scaleX + 0.5);
        else
            return (long)((double)(x) * m_scaleX - 0.5);
    }
    long YLOG2DEV(long y) const
    {
        long new_y = y - m_logicalOriginY;
        if (new_y > 0)
            return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
        else
            return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
    }
    // Without device translation, for backing pixmap purposes
    long YLOG2DEV_2(long y) const
    {
        long new_y = y - m_logicalOriginY;
        if (new_y > 0)
            return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY;
        else
            return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY;
    }
    long YLOG2DEVREL(long y) const
    {
        if (y > 0)
            return (long)((double)(y) * m_scaleY + 0.5);
        else
            return (long)((double)(y) * m_scaleY - 0.5);
    }

    void SetInternalDeviceOrigin( long x, long y );
    void GetInternalDeviceOrigin( long *x, long *y );

public:
    // not sure what for, but what is a mm on a screen you don't know the size of?
    double       m_mm_to_pix_x,m_mm_to_pix_y;

    // If un-scrolled is non-zero or d.o. changes with scrolling. Set using
    // SetInternalDeviceOrigin().
    long         m_internalDeviceOriginX,m_internalDeviceOriginY;

    // To be set by external classes such as wxScrolledWindow using
    // SetDeviceOrigin()
    long         m_externalDeviceOriginX,m_externalDeviceOriginY;

    // recompute scale?
    bool         m_needComputeScaleX, m_needComputeScaleY;

    // wxPSDC wants to have this. Will disappear.
    float        m_scaleFactor;
};

#endif
    // _WX_DC_H_
