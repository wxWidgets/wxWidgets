/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/dc.cpp
// Purpose:     wxDC class
// Author:      Vaclav Slavik
// Created:     2006-08-07
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/log.h"
#endif

#include "wx/dfb/private.h"

// these values are used to initialize newly created DC
#define DEFAULT_FONT      (*wxNORMAL_FONT)
#define DEFAULT_PEN       (*wxBLACK_PEN)
#define DEFAULT_BRUSH     (*wxWHITE_BRUSH)

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

// Default constructor
wxDC::wxDC()
{
    m_ok = false;
}

wxDC::wxDC(const wxIDirectFBSurfacePtr& surface)
{
    Init(surface);
}

void wxDC::Init(const wxIDirectFBSurfacePtr& surface)
{
    m_ok = (surface != NULL);
    wxCHECK_RET( surface != NULL, _T("invalid surface") );

    m_surface = surface;

    m_mm_to_pix_x = (double)wxGetDisplaySize().GetWidth() /
                    (double)wxGetDisplaySizeMM().GetWidth();
    m_mm_to_pix_y = (double)wxGetDisplaySize().GetHeight() /
                    (double)wxGetDisplaySizeMM().GetHeight();

    SetFont(DEFAULT_FONT);
    SetPen(DEFAULT_PEN);
    SetBrush(DEFAULT_BRUSH);
}


// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------


#define DO_SET_CLIPPING_BOX(rg)                      \
{                                                    \
    wxRect rect = rg.GetBox();                       \
    m_clipX1 = (wxCoord) XDEV2LOG(rect.GetLeft());   \
    m_clipY1 = (wxCoord) YDEV2LOG(rect.GetTop());    \
    m_clipX2 = (wxCoord) XDEV2LOG(rect.GetRight());  \
    m_clipY2 = (wxCoord) YDEV2LOG(rect.GetBottom()); \
}

void wxDC::DoSetClippingRegion(wxCoord cx, wxCoord cy, wxCoord cw, wxCoord ch)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxSize size(GetSize());

    // NB: We intersect the clipping rectangle with surface's area here because
    //     DirectFB will return an error if you try to set clipping rectangle
    //     that is partially outside of the surface.
    DFBRegion r;
    r.x1 = wxMax(0, XLOG2DEV(cx));
    r.y1 = wxMax(0, YLOG2DEV(cy));
    r.x2 = wxMin(r.x1 + XLOG2DEVREL(cw), size.x) - 1;
    r.y2 = wxMin(r.y1 + YLOG2DEVREL(ch), size.y) - 1;

    if ( !m_surface->SetClip(&r) )
        return;

    m_clipX1 = cx;
    m_clipY1 = cy;
    m_clipX2 = cx + cw - 1;
    m_clipY2 = cy + ch -1;
    m_clipping = true;
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
    // NB: this can be done because wxDFB only supports
    //     rectangular regions
    SetClippingRegion(region.AsRect());
}

void wxDC::DestroyClippingRegion()
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    m_surface->SetClip(NULL);

    ResetClipping();
}

// ---------------------------------------------------------------------------
// query capabilities
// ---------------------------------------------------------------------------

int wxDC::GetDepth() const
{
    return m_surface->GetDepth();
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxDC::Clear()
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    if ( m_backgroundBrush.GetStyle() == wxTRANSPARENT )
        return;

    wxColour clr = m_backgroundBrush.GetColour();
    m_surface->Clear(clr.Red(), clr.Green(), clr.Blue(), clr.Alpha());
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);

bool wxDC::DoFloodFill(wxCoord x, wxCoord y,
                       const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    wxCHECK_MSG( col, false, _T("NULL colour parameter in wxDC::GetPixel"));

    wxFAIL_MSG( _T("GetPixel not implemented") );
    return false;
}

void wxDC::DoCrossHair(wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("CrossHair not implemented") );
}

void wxDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    if ( m_pen.GetStyle() == wxTRANSPARENT )
        return;

    m_surface->DrawLine(XLOG2DEV(x1), YLOG2DEV(y1),
                        XLOG2DEV(x2), YLOG2DEV(y2));

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

// Draws an arc of a circle, centred on (xc, yc), with starting point (x1, y1)
// and ending at (x2, y2)
void wxDC::DoDrawArc(wxCoord x1, wxCoord y1,
                     wxCoord x2, wxCoord y2,
                     wxCoord xc, wxCoord yc)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawArc not implemented") );
}

void wxDC::DoDrawPoint(wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    // NB: DirectFB API doesn't provide a function for drawing points, so
    //     implement it as 1px long line. This is inefficient, but then, so is
    //     using DrawPoint() for drawing more than a few points.
    DoDrawLine(x, y, x, y);

    // FIXME_DFB: implement special cases for common formats (RGB24,RGBA/RGB32)
}

void wxDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int WXUNUSED(fillStyle))
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawPolygon not implemented") );
}

void wxDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    // TODO: impl. using DirectDB's DrawLines
    wxFAIL_MSG( _T("DrawLines not implemented") );
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    if ( ww == 0 || hh == 0 ) return;

    if ( ww < 0 )
    {
        ww = -ww;
        xx = xx - ww;
    }
    if ( hh < 0 )
    {
        hh = -hh;
        yy = yy - hh;
    }

    if ( m_brush.GetStyle() != wxTRANSPARENT )
    {
        SelectColour(m_brush.GetColour());
        m_surface->FillRectangle(xx, yy, ww, hh);
        // restore pen's colour
        SelectColour(m_pen.GetColour());
    }

    if ( m_pen.GetStyle() != wxTRANSPARENT )
    {
        m_surface->DrawRectangle(xx, yy, ww, hh);
    }

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + width, y + height);
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawRoundedRectangle not implemented") );
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawElipse not implemented") );
}

void wxDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawElipticArc not implemented") );
}

void wxDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);

    // update the bounding box
    wxCoord w, h;
    CalcBoundingBox(x, y);
    GetTextExtent(text, &w, &h);
    CalcBoundingBox(x + w, y + h);

    // if background mode is solid, DrawText must paint text's background:
    if ( m_backgroundMode == wxSOLID )
    {
        wxCHECK_RET( m_backgroundBrush.Ok(), wxT("invalid background brush") );

        SelectColour(m_backgroundBrush.GetColour());
        m_surface->FillRectangle(xx, yy, XLOG2DEVREL(w), YLOG2DEVREL(h));
        // restore pen's colour
        SelectColour(m_pen.GetColour());
    }

    // finally draw the text itself:
    m_surface->DrawString(wxSTR_TO_DFB(text), -1, xx, yy, DSTF_LEFT | DSTF_TOP);
}

void wxDC::DoDrawRotatedText(const wxString& text,
                             wxCoord x, wxCoord y,
                             double angle)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("DrawRotatedText not implemented") );
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

void wxDC::SetPen(const wxPen& pen)
{
    m_pen = pen.Ok() ? pen : DEFAULT_PEN;

    SelectColour(m_pen.GetColour());
}

void wxDC::SetBrush(const wxBrush& brush)
{
    m_brush = brush.Ok() ? brush : DEFAULT_BRUSH;
}

void wxDC::SelectColour(const wxColour& clr)
{
    m_surface->SetColor(clr.Red(), clr.Green(), clr.Blue(), clr.Alpha());
    #warning "use SetColorIndex?"
}

#if wxUSE_PALETTE
void wxDC::SetPalette(const wxPalette& WXUNUSED(palette))
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("SetPalette not implemented") );
}
#endif // wxUSE_PALETTE

void wxDC::SetFont(const wxFont& font)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFont f(font.Ok() ? font : DEFAULT_FONT);

    if ( !m_surface->SetFont(f.GetDirectFBFont()) )
        return;

    m_font = f;
}

void wxDC::SetBackground(const wxBrush& brush)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    if (!brush.Ok()) return;

    m_backgroundBrush = brush;
}

void wxDC::SetBackgroundMode(int mode)
{
    m_backgroundMode = mode;
}

void wxDC::SetLogicalFunction(int function)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    wxFAIL_MSG( _T("SetLogicalFunction not implemented") );

    m_logicalFunction = function;
}

bool wxDC::StartDoc(const wxString& WXUNUSED(message))
{
    // We might be previewing, so return true to let it continue.
    return true;
}

void wxDC::EndDoc()
{
}

void wxDC::StartPage()
{
}

void wxDC::EndPage()
{
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

wxCoord wxDC::GetCharHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid dc") );
    wxCHECK_MSG( m_font.Ok(), -1, wxT("no font selected") );

    int h = -1;
    m_font.GetDirectFBFont()->GetHeight(&h);
    return YDEV2LOGREL(h);
}

wxCoord wxDC::GetCharWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid dc") );
    wxCHECK_MSG( m_font.Ok(), -1, wxT("no font selected") );

    int w = -1;
    m_font.GetDirectFBFont()->GetStringWidth("H", 1, &w);
    // VS: YDEV is corrent, it should *not* be XDEV, because font's are only
    //     scaled according to m_scaleY
    return YDEV2LOGREL(w);
}

void wxDC::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                           wxCoord *descent, wxCoord *externalLeading,
                           wxFont *theFont) const
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );
    wxCHECK_RET( m_font.Ok(), wxT("no font selected") );
    wxCHECK_RET( !theFont || theFont->Ok(), wxT("invalid font") );

    wxFont oldFont;
    if ( theFont != NULL )
    {
        oldFont = m_font;
        wxConstCast(this, wxDC)->SetFont(*theFont);
    }

    wxCoord xx = 0, yy = 0;
    DFBRectangle rect;
    wxIDirectFBFontPtr f = m_font.GetDirectFBFont();

    if ( f->GetStringExtents(wxSTR_TO_DFB(string), -1, &rect, NULL) )
    {
        // VS: YDEV is corrent, it should *not* be XDEV, because font's are
        //     only scaled according to m_scaleY
        xx = YDEV2LOGREL(rect.w);
        yy = YDEV2LOGREL(rect.h);

        if ( descent )
        {
            int d;
            if ( f->GetDescender(&d) )
                *descent = YDEV2LOGREL(-d);
            else
                *descent = 0;
        }
    }

    if ( x ) *x = xx;
    if ( y ) *y = yy;
    if ( externalLeading ) *externalLeading = 0;

    if ( theFont != NULL )
        wxConstCast(this, wxDC)->SetFont(oldFont);
}



// ---------------------------------------------------------------------------
// mapping modes
// ---------------------------------------------------------------------------

void wxDC::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;

    // FIXME_DFB: scaling affects pixel size of font, pens, brushes, which
    //            is not currently implemented here; probably makes sense to
    //            switch to Cairo instead of implementing everything for DFB
    wxASSERT_MSG( m_scaleX == 1.0 && m_scaleY == 1.0,
                  _T("scaling is not implemented in wxDFB") );
}

void wxDC::SetMapMode(int mode)
{
    #warning "move this to common code, it's shared by almost all ports!"
    switch (mode)
    {
        case wxMM_TWIPS:
          SetLogicalScale(twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y);
          break;
        case wxMM_POINTS:
          SetLogicalScale(pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y);
          break;
        case wxMM_METRIC:
          SetLogicalScale(m_mm_to_pix_x, m_mm_to_pix_y);
          break;
        case wxMM_LOMETRIC:
          SetLogicalScale(m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0);
          break;
        default:
        case wxMM_TEXT:
          SetLogicalScale(1.0, 1.0);
          break;
    }
    m_mappingMode = mode;
}

void wxDC::SetUserScale(double x, double y)
{
    #warning "move this to common code?"
    // allow negative ? -> no
    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetLogicalScale(double x, double y)
{
    #warning "move this to common code?"
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    #warning "move this to common code?"
    m_logicalOriginX = x * m_signX;   // is this still correct ?
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    #warning "move this to common code?"
    // only wxPostScripDC has m_signX = -1, we override SetDeviceOrigin there
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    #warning "move this to common code?"
    // only wxPostScripDC has m_signX = -1, we override SetAxisOrientation there
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

wxCoord wxDCBase::DeviceToLogicalX(wxCoord x) const
{
    return ((wxDC *)this)->XDEV2LOG(x);
}

wxCoord wxDCBase::DeviceToLogicalY(wxCoord y) const
{
    return ((wxDC *)this)->YDEV2LOG(y);
}

wxCoord wxDCBase::DeviceToLogicalXRel(wxCoord x) const
{
    return ((wxDC *)this)->XDEV2LOGREL(x);
}

wxCoord wxDCBase::DeviceToLogicalYRel(wxCoord y) const
{
    return ((wxDC *)this)->YDEV2LOGREL(y);
}

wxCoord wxDCBase::LogicalToDeviceX(wxCoord x) const
{
    return ((wxDC *)this)->XLOG2DEV(x);
}

wxCoord wxDCBase::LogicalToDeviceY(wxCoord y) const
{
    return ((wxDC *)this)->YLOG2DEV(y);
}

wxCoord wxDCBase::LogicalToDeviceXRel(wxCoord x) const
{
    return ((wxDC *)this)->XLOG2DEVREL(x);
}

wxCoord wxDCBase::LogicalToDeviceYRel(wxCoord y) const
{
    return ((wxDC *)this)->YLOG2DEVREL(y);
}


void wxDC::DoGetSize(int *w, int *h) const
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );

    m_surface->GetSize(w, h);
}

void wxDC::DoGetSizeMM(int *width, int *height) const
{
    #warning "move this to common code?"
    int w = 0;
    int h = 0;
    GetSize(&w, &h);
    if ( width ) *width = int(double(w) / (m_userScaleX*m_mm_to_pix_x));
    if ( height ) *height = int(double(h) / (m_userScaleY*m_mm_to_pix_y));
}

wxSize wxDC::GetPPI() const
{
    #warning "move this to common code?"
    return wxSize(int(double(m_mm_to_pix_x) * inches2mm),
                  int(double(m_mm_to_pix_y) * inches2mm));
}


// ---------------------------------------------------------------------------
// Blitting
// ---------------------------------------------------------------------------

bool wxDC::DoBlit(wxCoord xdest, wxCoord ydest,
                  wxCoord width, wxCoord height,
                  wxDC *source, wxCoord xsrc, wxCoord ysrc,
                  int rop, bool useMask,
                  wxCoord xsrcMask, wxCoord ysrcMask)
{
#warning "FIXME"
    return false; 
#if 0
    wxCHECK_MSG( Ok(), false, wxT("invalid dc") );
    wxCHECK_MSG( source, false, wxT("invalid source dc") );

    // transform the source DC coords to the device ones
    xsrc = source->LogicalToDeviceX(xsrc);
    ysrc = source->LogicalToDeviceY(ysrc);

    /* FIXME_MGL: use the mask origin when drawing transparently */
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }
    else
    {
        xsrcMask = source->LogicalToDeviceX(xsrcMask);
        ysrcMask = source->LogicalToDeviceY(ysrcMask);
    }

    CalcBoundingBox(xdest, ydest);
    CalcBoundingBox(xdest + width, ydest + height);

    /* scale/translate size and position */
    wxCoord xx = XLOG2DEV(xdest);
    wxCoord yy = YLOG2DEV(ydest);
    wxCoord ww = XLOG2DEVREL(width);
    wxCoord hh = YLOG2DEVREL(height);

    if ( source->m_isMemDC )
    {
        wxMemoryDC *memDC = (wxMemoryDC*) source;
        DoDrawSubBitmap(memDC->GetSelectedObject(), xsrc, ysrc, ww, hh,
                        xdest, ydest, rop, useMask);
    }
    else
    {
        m_MGLDC->makeCurrent(); // will go away with MGL6.0
        m_MGLDC->bitBlt(*source->GetMGLDC(),
                        xsrc, ysrc, xsrc + ww, ysrc + hh,
                        xx, yy, LogicalFunctionToMGLRop(rop));
    }

    return true;
#endif
}

void wxDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask)
{
    wxCHECK_RET( Ok(), wxT("invalid dc") );
    wxCHECK_RET( bmp.Ok(), wxT("invalid bitmap") );

    wxCoord w = bmp.GetWidth();
    wxCoord h = bmp.GetHeight();

    DoDrawSubBitmap(bmp, 0, 0, w, h, x, y, m_logicalFunction, useMask);
}

void wxDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    // VZ: egcs 1.0.3 refuses to compile this without cast, no idea why
    DoDrawBitmap((const wxBitmap&)icon, x, y, true);
}

void wxDC::DoDrawSubBitmap(const wxBitmap &bmp,
                           wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                           wxCoord destx, wxCoord desty, int rop, bool useMask)
{
#if 0
    wxCHECK_RET( Ok(), wxT("invalid dc") );
    wxCHECK_RET( bmp.Ok(), wxT("invalid bitmap") );

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);

    wxCoord dx = XLOG2DEV(destx);
    wxCoord dy = YLOG2DEV(desty);
    wxCoord dw = XLOG2DEVREL(w);
    wxCoord dh = YLOG2DEVREL(h);

    m_MGLDC->makeCurrent(); // will go away with MGL6.0

    bool useStretching = ((w != dw) || (h != dh));
    bool putSection = (w != bmp.GetWidth() || h != bmp.GetHeight());
    MGL_writeModeType mglRop = (MGL_writeModeType)LogicalFunctionToMGLRop(rop);

    if ( bmp.GetDepth() == 1 )
    {
        // Mono bitmaps are handled in special way -- all 1s are drawn in
        // foreground colours, all 0s in background colour.

        ((wxBitmap&)bmp).SetMonoPalette(m_textForegroundColour, m_textBackgroundColour);
    }

    if ( useMask && bmp.GetMask() )
    {
        // Since MGL does not support masks directly (in MGL, mask is handled
        // in same way as in wxImage, i.e. there is one "key" color), we
        // simulate masked bitblt in 6 steps (same as in MSW):
        //
        // 1. Create a temporary bitmap and copy the destination area into it.
        // 2. Copy the source area into the temporary bitmap using the
        //    specified logical function.
        // 3. Set the masked area in the temporary bitmap to BLACK by ANDing
        //    the mask bitmap with the temp bitmap with the foreground colour
        //    set to WHITE and the bg colour set to BLACK.
        // 4. Set the unmasked area in the destination area to BLACK by
        //    ANDing the mask bitmap with the destination area with the
        //    foreground colour set to BLACK and the background colour set
        //    to WHITE.
        // 5. OR the temporary bitmap with the destination area.
        // 6. Delete the temporary bitmap.
        //
        // This sequence of operations ensures that the source's transparent
        // area need not be black, and logical functions are supported.

        wxBitmap *mask = bmp.GetMask()->GetBitmap();

        MGLMemoryDC *temp;

        if ( GetDepth() <= 8 )
        {
            temp = new MGLMemoryDC(dw, dh, GetDepth(), NULL);
            wxDC tempdc;
            tempdc.SetMGLDC(temp, false);
            tempdc.SetPalette(m_palette);
        }
        else
        {
            pixel_format_t pf;
            m_MGLDC->getPixelFormat(pf);
            temp = new MGLMemoryDC(dw, dh, GetDepth(), &pf);
        }

        wxCHECK_RET( temp->isValid(), wxT("cannot create temporary dc") );

        temp->bitBlt(*m_MGLDC, dx, dy, dx + dw, dy + dh, 0, 0, MGL_REPLACE_MODE);

        DoBitBlt(bmp, temp, x, y, w, h, 0, 0, dw, dh, mglRop,
                 useStretching, putSection);

        mask->SetMonoPalette(wxColour(0,0,0), wxColour(255,255,255));
        DoBitBlt(*mask, temp, x, y, w, h, 0, 0, dw, dh, MGL_R2_MASKSRC,
                 useStretching, putSection);
        DoBitBlt(*mask, m_MGLDC, x, y, w, h, dx, dy, dw, dh, MGL_R2_MASKNOTSRC,
                 useStretching, putSection);

        m_MGLDC->bitBlt(*temp, 0, 0, dw, dh, dx, dy, MGL_OR_MODE);

        delete temp;
    }

    else
    {
        DoBitBlt(bmp, m_MGLDC, x, y, w, h, dx, dy, dw, dh, mglRop,
                 useStretching, putSection);
    }
#endif
}
