/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dc.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCH__
#define __GTKDCH__

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
// coordinates transformations
//-----------------------------------------------------------------------------

inline wxCoord wxDCBase::DeviceToLogicalX(wxCoord x) const
{
    return wxRound((x - m_deviceOriginX) / m_scaleX) * m_signX + m_logicalOriginX;
}
inline wxCoord wxDCBase::DeviceToLogicalY(wxCoord y) const
{
    return wxRound((y - m_deviceOriginY) / m_scaleY) * m_signY + m_logicalOriginY;
}
inline wxCoord wxDCBase::DeviceToLogicalXRel(wxCoord x) const
{
    return wxRound(x / m_scaleX);
}
inline wxCoord wxDCBase::DeviceToLogicalYRel(wxCoord y) const
{
    return wxRound(y / m_scaleY);
}
inline wxCoord wxDCBase::LogicalToDeviceX(wxCoord x) const
{
    return wxRound((x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX;
}
inline wxCoord wxDCBase::LogicalToDeviceY(wxCoord y) const
{
    return wxRound((y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY;
}
inline wxCoord wxDCBase::LogicalToDeviceXRel(wxCoord x) const
{
    return wxRound(x * m_scaleX);
}
inline wxCoord wxDCBase::LogicalToDeviceYRel(wxCoord y) const
{
    return wxRound(y * m_scaleY);
}

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

    virtual void ComputeScaleAndOrigin();

    virtual GdkWindow* GetGDKWindow() const { return NULL; }

protected:
    // implementation
    // --------------

    wxCoord XDEV2LOG(wxCoord x) const
    {
        return DeviceToLogicalX(x);
    }
    wxCoord XDEV2LOGREL(wxCoord x) const
    {
        return DeviceToLogicalXRel(x);
    }
    wxCoord YDEV2LOG(wxCoord y) const
    {
        return DeviceToLogicalY(y);
    }
    wxCoord YDEV2LOGREL(wxCoord y) const
    {
        return DeviceToLogicalYRel(y);
    }
    wxCoord XLOG2DEV(wxCoord x) const
    {
        return LogicalToDeviceX(x);
    }
    wxCoord XLOG2DEVREL(wxCoord x) const
    {
        return LogicalToDeviceXRel(x);
    }
    wxCoord YLOG2DEV(wxCoord y) const
    {
        return LogicalToDeviceY(y);
    }
    wxCoord YLOG2DEVREL(wxCoord y) const
    {
        return LogicalToDeviceYRel(y);
    }

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
