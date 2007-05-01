/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcsvg.h
// Purpose:     wxSVGFileDC
// Author:      Chris Elliott
// Modified by:
// Created:
// Copyright:   (c) Chris Elliott
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DCSVG_H
#define __DCSVG_H

#include "wx/wfstream.h"
#include "wx/string.h"

#define wxSVGVersion wxT("v0100")

#ifdef __BORLANDC__
#pragma warn -8008
#pragma warn -8066
#endif

class WXDLLIMPEXP_CORE wxSVGFileDC : public wxDC
{
public:
    wxSVGFileDC (wxString f);
    wxSVGFileDC (wxString f, int Width, int Height);
    wxSVGFileDC (wxString f, int Width, int Height, float dpi);
    
    ~wxSVGFileDC();

    bool CanDrawBitmap() const { return true; }
    bool CanGetTextExtent() const { return true; }

    int GetDepth() const
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::GetDepth Call not implemented")); return -1 ; }

    void Clear()
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::Clear() Call not implemented \nNot sensible for an output file?")); return ; }

    void DestroyClippingRegion()
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::void Call not yet implemented")); return ; }

    wxCoord GetCharHeight() const;
    wxCoord GetCharWidth() const;

        void SetClippingRegion(wxCoord  WXUNUSED(x), wxCoord  WXUNUSED(y), wxCoord  WXUNUSED(width), wxCoord  WXUNUSED(height))
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::SetClippingRegion not implemented")); return ; }

        void SetPalette(const wxPalette&  WXUNUSED(palette))
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::SetPalette not implemented")); return ; }

        void GetClippingBox(wxCoord *WXUNUSED(x), wxCoord *WXUNUSED(y), wxCoord * WXUNUSED(width), wxCoord * WXUNUSED(height))
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::GetClippingBox not implemented")); return ; }

        void SetLogicalFunction(int  WXUNUSED(function))
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::SetLogicalFunction Call not implemented")); return ; }

        int GetLogicalFunction() const
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::GetLogicalFunction() not implemented")); return wxCOPY ; }

        void SetBackground( const wxBrush &brush ) ;
        void SetBackgroundMode( int mode ) ;
        void SetBrush(const wxBrush& brush) ;
        void SetFont(const wxFont& font) ;
        void SetPen(const wxPen& pen)  ;
          
        bool IsOk() const {return m_OK;}

    virtual wxCoord DeviceToLogicalX(wxCoord x) const;
    virtual wxCoord DeviceToLogicalY(wxCoord y) const;
    virtual wxCoord DeviceToLogicalXRel(wxCoord x) const;
    virtual wxCoord DeviceToLogicalYRel(wxCoord y) const;
    virtual wxCoord LogicalToDeviceX(wxCoord x) const;
    virtual wxCoord LogicalToDeviceY(wxCoord y) const;
    virtual wxCoord LogicalToDeviceXRel(wxCoord x) const;
    virtual wxCoord LogicalToDeviceYRel(wxCoord y) const ;

    virtual void SetMapMode( int mode );
    virtual void SetUserScale( double x, double y );
    virtual void SetLogicalScale( double x, double y );
    virtual void SetLogicalOrigin( wxCoord x, wxCoord y );
    virtual void SetDeviceOrigin( wxCoord x, wxCoord y );

    virtual void SetAxisOrientation( bool xLeftRight, bool yBottomUp );

    virtual void ComputeScaleAndOrigin();

private:
        bool DoGetPixel(wxCoord, wxCoord, class wxColour *) const
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::DoGetPixel Call not implemented")); return true; }

        virtual bool DoBlit(wxCoord, wxCoord, wxCoord, wxCoord, class wxDC *,
            wxCoord, wxCoord, int = wxCOPY, bool = 0, int = -1, int = -1) ;

        void DoCrossHair(wxCoord, wxCoord)
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::CrossHair Call not implemented")); return  ; }

        void DoDrawArc(wxCoord, wxCoord, wxCoord, wxCoord, wxCoord, wxCoord);

        void DoDrawBitmap(const class wxBitmap &, wxCoord, wxCoord, bool = 0) ;

        void DoDrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea) ;

        void DoDrawIcon(const class wxIcon &, wxCoord, wxCoord) ;

        void DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) ;

        void DoDrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0) ;

        void DoDrawPoint(wxCoord, wxCoord) ;

        void DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle) ;

        void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle) ;

        void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20)  ;

        void DoDrawText(const wxString& text, wxCoord x, wxCoord y);

        bool DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxColour& WXUNUSED(col),
                             int WXUNUSED(style) = wxFLOOD_SURFACE)
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::DoFloodFill Call not implemented")); return false ; }

        void DoGetSize(int * x, int *y) const { *x = m_width; *y = m_height ; return ; }

        void DoGetTextExtent(const wxString& string, wxCoord *w, wxCoord *h, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, const wxFont *font = NULL) const ;

        void DoSetClippingRegionAsRegion(const class wxRegion &)
            { wxASSERT_MSG (false, wxT("wxSVGFILEDC::DoSetClippingRegionAsRegion Call not yet implemented")); return  ; }

       void Init (wxString f, int Width, int Height, float dpi);

       void NewGraphics();
       
       void write( const wxString &s );

private:
    wxFileOutputStream *m_outfile;
    wxString            m_filename;
    int                 m_sub_images; // number of png format images we have
    bool                m_OK;
    bool                m_graphics_changed;
    int                 m_width, m_height;
    double              m_mm_to_pix_x, m_mm_to_pix_y;
    
private:
    DECLARE_ABSTRACT_CLASS(wxSVGFileDC)
};

#endif      // __DCSVG_H
