/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dc.h"
#endif

#include "wx/dc.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define mm2inches		0.0393700787402
#define inches2mm		25.4
#define mm2twips		56.6929133859
#define twips2mm		0.0176388888889
#define mm2pt			2.83464566929
#define pt2mm			0.352777777778

//-----------------------------------------------------------------------------
// wxDCBase
//-----------------------------------------------------------------------------

long wxDCBase::DeviceToLogicalX(long x) const
{
    long new_x = x - m_deviceOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
    else
        return (long)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
};

long wxDCBase::DeviceToLogicalY(long y) const
{
    long new_y = y - m_deviceOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
    else
        return (long)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
};

long wxDCBase::DeviceToLogicalXRel(long x) const
{
    if (x > 0)
        return (long)((double)(x) / m_scaleX + 0.5);
    else
        return (long)((double)(x) / m_scaleX - 0.5);
};

long wxDCBase::DeviceToLogicalYRel(long y) const
{
    if (y > 0)
        return (long)((double)(y) / m_scaleY + 0.5);
    else
        return (long)((double)(y) / m_scaleY - 0.5);
};

long wxDCBase::LogicalToDeviceX(long x) const
{
    long new_x = x - m_logicalOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
    else
    return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
};

long wxDCBase::LogicalToDeviceY(long y) const
{
    long new_y = y - m_logicalOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
    else
        return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
};

long wxDCBase::LogicalToDeviceXRel(long x) const
{
    if (x > 0)
        return (long)((double)(x) * m_scaleX + 0.5);
    else
        return (long)((double)(x) * m_scaleX - 0.5);
};

long wxDCBase::LogicalToDeviceYRel(long y) const
{
    if (y > 0)
        return (long)((double)(y) * m_scaleY + 0.5);
    else
        return (long)((double)(y) * m_scaleY - 0.5);
};

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC(void)
{
    m_canvas  = NULL;

    m_oldBitmap  = 0;
    m_oldPen     = 0;
    m_oldBrush   = 0;
    m_oldFont    = 0;
    m_oldPalette = 0;

    m_bOwnsDC = FALSE;
    m_hDC        = 0;
    m_hDCCount   = 0;
};

wxDC::~wxDC(void)
{
    // TODO:
};

void wxDC::DestroyClippingRegion(void)
{
    // TODO:
};

void wxDC::DoGetSize( int* width, int* height ) const
{
    // TODO:
};

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    // TODO:
};

void wxDC::SetMapMode( int mode )
{
    // TODO:
};

void wxDC::SetUserScale( double x, double y )
{
    m_userScaleX = x;
    m_userScaleY = y;

    SetMapMode(m_mappingMode);
};

void wxDC::SetLogicalScale( double x, double y )
{
    // TODO:
};

void wxDC::SetLogicalOrigin( long x, long y )
{
    // TODO:
};

void wxDC::SetDeviceOrigin( long x, long y )
{
    // TODO:
};

void wxDC::SetInternalDeviceOrigin( long x, long y )
{
    // TODO:
};

void wxDC::GetInternalDeviceOrigin( long *x, long *y )
{
    // TODO:
};

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // TODO:
};


int wxDC::GetDepth() const
{
   // TODO:
   return (1);
}

wxSize wxDC::GetPPI() const
{
   int x = 1;
   int y = 1;
   // TODO:
   return (wxSize(x,y));
}
void wxDC::GetTextExtent( const wxString& string
                         ,long* x
                         ,long* y
                         ,long* decent
                         ,long* externalLeading
                         ,wxFont* theFont
                        ) const
{
   // TODO:
}

long wxDC::GetCharWidth() const
{
    // TODO
    return(1);
}

long wxDC::GetCharHeight() const
{
    // TODO
    return(1);
}

void wxDC::Clear()
{
   // TODO
}

void wxDC::SetFont(const wxFont& font)
{
   // TODO
}

void wxDC::SetPen(const wxPen& pen)
{
   // TODO
}
void wxDC::SetBrush(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetBackground(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetLogicalFunction(int function)
{
   // TODO
}

void wxDC::SetBackgroundMode(int mode)
{
   // TODO
}

void wxDC::SetPalette(const wxPalette& palette)
{
   // TODO
}

void wxDC::DoFloodFill( long x
                       ,long y
                       ,const wxColour& col
                       ,int style
                      )
{
   // TODO
}

bool wxDC::DoGetPixel(long x, long y, wxColour *col) const
{
   // TODO
   return(TRUE);
}

void wxDC::DoDrawPoint(long x, long y)
{
   // TODO
}

void wxDC::DoDrawLine(long x1, long y1, long x2, long y2)
{
   // TODO
}

void wxDC::DoDrawArc( long x1, long y1
                     ,long x2, long y2
                     ,long xc, long yc
                    )
{
   // TODO
}

void wxDC::DoDrawEllipticArc( long x
                             ,long y
                             ,long w
                             ,long h
                             ,double sa
                             ,double ea
                            )
{
   // TODO
}

void wxDC::DoDrawRectangle(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoDrawRoundedRectangle( long x, long y
                                  ,long width, long height
                                  ,double radius
                                 )
{
   // TODO
}

void wxDC::DoDrawEllipse(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoCrossHair(long x, long y)
{
   // TODO
}

void wxDC::DoDrawIcon(const wxIcon& icon, long x, long y)
{
   // TODO
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp
                        ,long x, long y
                        ,bool useMask
                       )
{
   // TODO
}

void wxDC::DoDrawText(const wxString& text, long x, long y)
{
   // TODO
}

bool wxDC::DoBlit( long xdest
                  ,long ydest
                  ,long width
                  ,long height
                  ,wxDC *source
                  ,long xsrc
                  ,long ysrc
                  ,int  rop
                  ,bool useMask
                 )
{
   // TODO
   return(TRUE);
}

void wxDC::DoDrawLines( int n, wxPoint points[]
                       ,long xoffset, long yoffset
                      )
{
   // TODO
}

void wxDC::DoDrawPolygon(int n, wxPoint points[]
                         ,long xoffset, long yoffset
                         ,int fillStyle
                        )
{
   // TODO
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
   // TODO
}

void wxDC::DoSetClippingRegion( long x, long y
                               ,long width, long height
                              )
{
   // TODO
}

#if wxUSE_SPLINES
void wxDC::DoDrawSpline(wxList *points)
{
   // TODO
}
#endif

void wxDC::SetRop(WXHDC dc)
{
    if (!dc || m_logicalFunction < 0)
        return;

    int c_rop;
    // These may be wrong
    switch (m_logicalFunction)
    {
// TODO: Figure this stuff out
        //    case wxXOR: c_rop = R2_XORPEN; break;
//    case wxXOR: c_rop = R2_NOTXORPEN; break;
//    case wxINVERT: c_rop = R2_NOT; break;
//    case wxOR_REVERSE: c_rop = R2_MERGEPENNOT; break;
//    case wxAND_REVERSE: c_rop = R2_MASKPENNOT; break;
//    case wxCLEAR: c_rop = R2_WHITE; break;
//    case wxSET: c_rop = R2_BLACK; break;
//    case wxSRC_INVERT: c_rop = R2_NOTCOPYPEN; break;
//    case wxOR_INVERT: c_rop = R2_MERGENOTPEN; break;
//    case wxAND: c_rop = R2_MASKPEN; break;
//    case wxOR: c_rop = R2_MERGEPEN; break;
//    case wxAND_INVERT: c_rop = R2_MASKNOTPEN; break;
//    case wxEQUIV:
//    case wxNAND:
//    case wxCOPY:
    default:
//      c_rop = R2_COPYPEN;
        break;
    }
//    SetROP2((HDC) dc, c_rop);
}

void wxDC::DoClipping(WXHDC dc)
{
    if (m_clipping && dc)
    {
//      TODO:
//      IntersectClipRect((HDC) dc, XLOG2DEV(m_clipX1), YLOG2DEV(m_clipY1),
//                                  XLOG2DEV(m_clipX2), YLOG2DEV(m_clipY2));
    }
}

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxDC::SelectOldObjects(WXHDC dc)
{
    if (dc)
    {
        if (m_oldBitmap)
        {
//            ::SelectObject((HDC) dc, (HBITMAP) m_oldBitmap);
            if (m_selectedBitmap.Ok())
            {
                m_selectedBitmap.SetSelectedInto(NULL);
            }
        }
        m_oldBitmap = 0;
        if (m_oldPen)
        {
//            ::SelectObject((HDC) dc, (HPEN) m_oldPen);
        }
        m_oldPen = 0;
        if (m_oldBrush)
        {
//            ::SelectObject((HDC) dc, (HBRUSH) m_oldBrush);
        }
        m_oldBrush = 0;
        if (m_oldFont)
        {
//            ::SelectObject((HDC) dc, (HFONT) m_oldFont);
        }
        m_oldFont = 0;
        if (m_oldPalette)
        {
//            ::SelectPalette((HDC) dc, (HPALETTE) m_oldPalette, TRUE);
        }
        m_oldPalette = 0;
    }

    m_brush = wxNullBrush;
    m_pen = wxNullPen;
    m_palette = wxNullPalette;
    m_font = wxNullFont;
    m_backgroundBrush = wxNullBrush;
    m_selectedBitmap = wxNullBitmap;
}

//
// Private functions
//


