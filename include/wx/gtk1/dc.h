/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDCH__
#define __GTKDCH__

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDC;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define MM_TEXT         0
#define MM_ISOTROPIC    1
#define MM_ANISOTROPIC  2
#define MM_LOMETRIC     3
#define MM_HIMETRIC     4
#define MM_TWIPS        5
#define MM_POINTS       6
#define MM_METRIC       7

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class wxDC : public wxDCBase
{
    DECLARE_ABSTRACT_CLASS(wxDC)

public:
    wxDC();
    ~wxDC() { }

    void SetColourMap( const wxPalette& palette ) { SetPalette(palette); };

    // the first two must be overridden and called
    virtual void DestroyClippingRegion();

    // Resolution in pixels per logical inch
    virtual wxSize GetPPI() const;

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return TRUE; }
    virtual void EndDoc() { }
    virtual void StartPage() { }
    virtual void EndPage() { }

    virtual void SetMapMode( int mode );
    virtual void SetUserScale( double x, double y );
    virtual void SetLogicalScale( double x, double y );
    virtual void SetLogicalOrigin( long x, long y );
    virtual void SetDeviceOrigin( long x, long y );

    virtual void SetAxisOrientation( bool xLeftRight, bool yBottomUp );

    // implementation
    // --------------

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
    long YLOG2DEVREL(long y) const
    {
        if (y > 0)
            return (long)((double)(y) * m_scaleY + 0.5);
        else
            return (long)((double)(y) * m_scaleY - 0.5);
    }

protected:
    // base class pure virtuals implemented here
    virtual void DoSetClippingRegion(long x, long y, long width, long height);
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

public:
    // GTK-specific member variables

    int          m_textAlignment;    // gone in wxWin 2.0 ?

    // not sure what for, but what is a mm on a screen you don't know the size
    // of?
    double       m_mm_to_pix_x,
                 m_mm_to_pix_y;

    bool         m_needComputeScaleX,
                 m_needComputeScaleY; // not yet used

    float        m_scaleFactor;  // wxPSDC wants to have this. Will disappear.
};

#endif // __GTKDCH__
