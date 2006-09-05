/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/dc.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCH__
#define __GTKDCH__

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDC;

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

class WXDLLIMPEXP_CORE wxDC : public wxDCBase
{
public:
    wxDC();
    virtual ~wxDC() { }

#if wxUSE_PALETTE
    void SetColourMap( const wxPalette& palette ) { SetPalette(palette); };
#endif // wxUSE_PALETTE

    // Resolution in pixels per logical inch
    virtual wxSize GetPPI() const;

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return true; }
    virtual void EndDoc() { }
    virtual void StartPage() { }
    virtual void EndPage() { }

    virtual void SetMapMode( int mode );
    virtual void SetUserScale( double x, double y );
    virtual void SetLogicalScale( double x, double y );
    virtual void SetLogicalOrigin( wxCoord x, wxCoord y );
    virtual void SetDeviceOrigin( wxCoord x, wxCoord y );

    virtual void SetAxisOrientation( bool xLeftRight, bool yBottomUp );

    // implementation
    // --------------

    virtual void ComputeScaleAndOrigin();

    wxCoord XDEV2LOG(wxCoord x) const
    {
        wxCoord new_x = x - m_deviceOriginX;
        if (new_x > 0)
            return (wxCoord)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
        else
            return (wxCoord)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
    }
    wxCoord XDEV2LOGREL(wxCoord x) const
    {
        if (x > 0)
            return (wxCoord)((double)(x) / m_scaleX + 0.5);
        else
            return (wxCoord)((double)(x) / m_scaleX - 0.5);
    }
    wxCoord YDEV2LOG(wxCoord y) const
    {
        wxCoord new_y = y - m_deviceOriginY;
        if (new_y > 0)
            return (wxCoord)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
        else
            return (wxCoord)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
    }
    wxCoord YDEV2LOGREL(wxCoord y) const
    {
        if (y > 0)
            return (wxCoord)((double)(y) / m_scaleY + 0.5);
        else
            return (wxCoord)((double)(y) / m_scaleY - 0.5);
    }
    wxCoord XLOG2DEV(wxCoord x) const
    {
        wxCoord new_x = x - m_logicalOriginX;
        if (new_x > 0)
            return (wxCoord)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
        else
            return (wxCoord)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
    }
    wxCoord XLOG2DEVREL(wxCoord x) const
    {
        if (x > 0)
            return (wxCoord)((double)(x) * m_scaleX + 0.5);
        else
            return (wxCoord)((double)(x) * m_scaleX - 0.5);
    }
    wxCoord YLOG2DEV(wxCoord y) const
    {
        wxCoord new_y = y - m_logicalOriginY;
        if (new_y > 0)
            return (wxCoord)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
        else
            return (wxCoord)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
    }
    wxCoord YLOG2DEVREL(wxCoord y) const
    {
        if (y > 0)
            return (wxCoord)((double)(y) * m_scaleY + 0.5);
        else
            return (wxCoord)((double)(y) * m_scaleY - 0.5);
    }

protected:
    // base class pure virtuals implemented here
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoGetSizeMM(int* width, int* height) const;

public:
    // GTK-specific member variables

    // not sure what for, but what is a mm on a screen you don't know the size
    // of?
    double       m_mm_to_pix_x,
                 m_mm_to_pix_y;

    bool         m_needComputeScaleX,
                 m_needComputeScaleY; // not yet used


private:
    DECLARE_ABSTRACT_CLASS(wxDC)
};

// this must be defined when wxDC::Blit() honours the DC origian and needed to
// allow wxUniv code in univ/winuniv.cpp to work with versions of wxGTK
// 2.3.[23]
#ifndef wxHAS_WORKING_GTK_DC_BLIT
    #define wxHAS_WORKING_GTK_DC_BLIT
#endif

#endif // __GTKDCH__
