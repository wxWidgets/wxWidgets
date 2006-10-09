/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcbuffer.h
// Purpose:     wxBufferedDC class
// Author:      Ron Lee <ron@debian.org>
// Modified by: Vadim Zeitlin (refactored, added bg preservation)
// Created:     16/03/02
// RCS-ID:      $Id$
// Copyright:   (c) Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCBUFFER_H_
#define _WX_DCBUFFER_H_

#include "wx/dcmemory.h"
#include "wx/dcclient.h"
#include "wx/window.h"


// I think this patch should be test on wxMac with
// wxTEST_PAINTDCDELEGATION as 1, and then with
// with wxTEST_MEMORYDCDELEGATION as 1.
#define wxTEST_PAINTDCDELEGATION        0
#define wxTEST_MEMORYDCDELEGATION       0


// Split platforms into two groups - those which have well-working
// double-buffering by default, and those which do not.
#if defined(__WXMAC__) || defined(__WXGTK20__)
    #define wxALWAYS_NATIVE_DOUBLE_BUFFER       1
#else
    #define wxALWAYS_NATIVE_DOUBLE_BUFFER       0
#endif


#if wxTEST_PAINTDCDELEGATION || wxTEST_MEMORYDCDELEGATION
    #undef wxALWAYS_NATIVE_DOUBLE_BUFFER
    #define wxALWAYS_NATIVE_DOUBLE_BUFFER 0
#endif


// ----------------------------------------------------------------------------
// Double buffering helper.
// ----------------------------------------------------------------------------

// Assumes the buffer bitmap covers the entire scrolled window,
// and prepares the window DC accordingly
#define wxBUFFER_VIRTUAL_AREA       0x01

// Assumes the buffer bitmap only covers the client area;
// does not prepare the window DC
#define wxBUFFER_CLIENT_AREA        0x02

class WXDLLEXPORT wxBufferedDC : public wxDC
{
public:
    // Default ctor, must subsequently call Init for two stage construction.
    wxBufferedDC() : wxDC(), m_targetDc(NULL), m_mainDc(NULL),
                     m_buffer(NULL), m_style(0)
    {
    }

    // Construct a wxBufferedDC using a user supplied buffer.
    wxBufferedDC(wxDC *dc,
                 const wxBitmap &buffer = wxNullBitmap,
                 int style = wxBUFFER_CLIENT_AREA)
        : wxDC(),
          m_targetDc(NULL),
          m_mainDc(NULL)
    {
        // All other members except dcs are initialized in Init
        Init(dc, buffer, style);
    }

    // Construct a wxBufferedDC with an internal buffer of 'area'
    // (where area is usually something like the size of the window
    // being buffered)
    wxBufferedDC(wxDC *dc, const wxSize &area, int style = wxBUFFER_CLIENT_AREA)
        : wxDC(),
          m_targetDc(NULL),
          m_mainDc(NULL)
    {
        // All other members except dcs are initialized in Init
        Init(NULL, dc, area, style);
    }

    // Same, but also receives window to detect whether it is
    // natively double-buffered.
    wxBufferedDC(wxWindow* win,
                 wxDC *dc,
                 const wxSize &area,
                 int style = wxBUFFER_CLIENT_AREA)
        : wxDC(),
          m_targetDc(NULL),
          m_mainDc(NULL)
    {
        // All other members except dcs are initialized in Init
        Init(win, dc, area, style);
    }

    // default copy ctor ok.

    // The usually desired  action in the dtor is to blit the buffer.
    virtual ~wxBufferedDC()
    {
        UnMask();
    }

    void Init(wxDC *dc,
              const wxBitmap &buffer,
              int style = wxBUFFER_CLIENT_AREA)
    {
        wxASSERT_MSG( m_mainDc == NULL,
                      wxT("wxBufferedDC already initialised") );
        wxASSERT_MSG( buffer.Ok(),
                      wxT("invalid bitmap") );
        m_mainDc = dc;
        m_buffer = &buffer;
        m_style = style;
        UseBuffer();
    }

    void Init(wxWindow* win,
              wxDC *dc,
              const wxSize &area = wxDefaultSize,
              int style = wxBUFFER_CLIENT_AREA)
    {
        wxASSERT_MSG( m_mainDc == NULL,
                      wxT("wxBufferedDC already initialised") );

        m_mainDc = dc;
        m_style = style;

#if wxTEST_MEMORYDCDELEGATION
        if ( 0 )
#elif wxTEST_PAINTDCDELEGATION
        if ( 1 )
#else
        if ( win && win->IsDoubleBuffered() )
#endif
        {
            AttachDC(dc);
            m_buffer = NULL;
        }
        else
        {
            PrepareBuffer(win, area);
            UseBuffer();
        }
    }

    void Init(wxDC *dc, const wxSize &area = wxDefaultSize, int style = wxBUFFER_CLIENT_AREA)
    {
        Init(NULL, dc, area, style);
    }

    // Blits the buffer to the dc, and detaches the dc from the buffer (so it
    // can be effectively used once only).
    //
    // Usually called in the dtor or by the dtor of derived classes if the
    // BufferedDC must blit before the derived class (which may own the dc it's
    // blitting to) is destroyed.
    void UnMask();

    // Set and get the style
    void SetStyle(int style) { m_style = style; }
    int GetStyle() const { return m_style; }

private:
    // Prepares wxMemoryDC.
    void UseBuffer();

    // Allocate m_buffer, if necessary.
    void PrepareBuffer(wxWindow* win, const wxSize& area);

    // DC to which calls are delegated.
    wxDC*           m_targetDc;

    // This the underlying DC to which we copy everything drawn on
    // this one in UnMask().
    //
    // NB: Without the existence of a wxNullDC, this must be a pointer, else it
    //     could probably be a reference.
    wxDC*           m_mainDc;

    // the buffer (selected in this DC)
    const wxBitmap* m_buffer;

    // the buffering style
    int             m_style;

    DECLARE_NO_COPY_CLASS(wxBufferedDC)
public:
    //
    // BEGIN DC-DELEGATION IMPLEMENTATION
    //

    wxDC& GetAttachedDC()
    {
        return *m_targetDc;
    }

    // Use this to set the DC which receives delegated calls.
    void AttachDC(wxDC* dc)
    {
        m_targetDc = dc;
    #ifdef __WXMSW__
        SetHDC( dc ? dc->GetHDC() : NULL );
    #endif
    }

    // Sets DC to NULL
    wxDC* DetachDC()
    {
        wxDC* retDc = m_targetDc;
        AttachDC(NULL);
        return retDc;
    }

#ifdef __WXGTK__
    virtual GdkWindow* GetGDKWindow() const
    {
        return m_targetDc->GetGDKWindow();
    }
#endif

    bool Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
              wxDC *source, wxCoord xsrc, wxCoord ysrc,
              int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord)
        { return m_targetDc->Blit(xdest, ydest, width, height,
                                  source, xsrc, ysrc, rop,
                                  useMask, xsrcMask, ysrcMask); }
    bool Blit(const wxPoint& destPt, const wxSize& sz,
              wxDC *source, const wxPoint& srcPt,
              int rop = wxCOPY, bool useMask = false, const wxPoint& srcPtMask = wxDefaultPosition)
        { return m_targetDc->Blit(destPt, sz, source, srcPt,
                                  rop, useMask, srcPtMask); }
    virtual void CalcBoundingBox(wxCoord x, wxCoord y) { m_targetDc->CalcBoundingBox(x, y); }
#if defined(__WXWINCE__)
    void CalculateEllipticPoints( wxList* points,
                                  wxCoord xStart, wxCoord yStart,
                                  wxCoord w, wxCoord h,
                                  double sa, double ea )
        { m_targetDc->CalculateEllipticPoints(points, xStart, yStart, w,
                                              h, sa, ea); }
#endif // defined(__WXWINCE__)
    virtual bool CanDrawBitmap() const { return m_targetDc->CanDrawBitmap(); }
    virtual bool CanGetTextExtent() const { return m_targetDc->CanGetTextExtent(); }
    virtual void Clear() { m_targetDc->Clear(); }
    virtual void ComputeScaleAndOrigin() { m_targetDc->ComputeScaleAndOrigin(); }
    void CrossHair(wxCoord x, wxCoord y) { m_targetDc->CrossHair(x, y); }
    void CrossHair(const wxPoint& pt) { m_targetDc->CrossHair(pt); }
    virtual void DestroyClippingRegion() { m_targetDc->DestroyClippingRegion(); }
    wxCoord DeviceToLogicalX(wxCoord x) const { return m_targetDc->DeviceToLogicalX(x); }
    wxCoord DeviceToLogicalXRel(wxCoord x) const { return m_targetDc->DeviceToLogicalXRel(x); }
    wxCoord DeviceToLogicalY(wxCoord y) const { return m_targetDc->DeviceToLogicalY(y); }
    wxCoord DeviceToLogicalYRel(wxCoord y) const { return m_targetDc->DeviceToLogicalYRel(y); }
#if defined(__WXWINCE__)
    virtual void DoDrawEllipticArcRot( wxCoord x, wxCoord y,
                                       wxCoord w, wxCoord h,
                                       double sa = 0, double ea = 0, double angle = 0 )
        { m_targetDc->DoDrawEllipticArcRot(x, y, w, h,
                                           sa, ea, angle); }
#endif // defined(__WXWINCE__)
    void DrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                 wxCoord xc, wxCoord yc)
        { m_targetDc->DrawArc(x1, y1, x2, y2,
                              xc, yc); }
    void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
        { m_targetDc->DrawArc(pt1, pt2, centre); }
    void DrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                    bool useMask = false)
        { m_targetDc->DrawBitmap(bmp, x, y, useMask); }
    void DrawBitmap(const wxBitmap &bmp, const wxPoint& pt,
                    bool useMask = false)
        { m_targetDc->DrawBitmap(bmp, pt, useMask); }
    void DrawCheckMark(wxCoord x, wxCoord y,
                       wxCoord width, wxCoord height)
        { m_targetDc->DrawCheckMark(x, y, width, height); }
    void DrawCheckMark(const wxRect& rect) { m_targetDc->DrawCheckMark(rect); }
    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius) { m_targetDc->DrawCircle(x, y, radius); }
    void DrawCircle(const wxPoint& pt, wxCoord radius) { m_targetDc->DrawCircle(pt, radius); }
    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { m_targetDc->DrawEllipse(x, y, width, height); }
    void DrawEllipse(const wxPoint& pt, const wxSize& sz) { m_targetDc->DrawEllipse(pt, sz); }
    void DrawEllipse(const wxRect& rect) { m_targetDc->DrawEllipse(rect); }
    void DrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                         double sa, double ea)
        { m_targetDc->DrawEllipticArc(x, y, w, h,
                                      sa, ea); }
    void DrawEllipticArc(const wxPoint& pt, const wxSize& sz,
                         double sa, double ea)
        { m_targetDc->DrawEllipticArc(pt, sz, sa, ea); }
#if defined(__WXWINCE__)
    void DrawEllipticArcRot( wxCoord x, wxCoord y,
                             wxCoord width, wxCoord height,
                             double sa = 0, double ea = 0, double angle = 0 )
        { m_targetDc->DrawEllipticArcRot(x, y, width, height,
                                         sa, ea, angle); }
    void DrawEllipticArcRot( const wxPoint& pt,
                             const wxSize& sz,
                             double sa = 0, double ea = 0, double angle = 0 )
        { m_targetDc->DrawEllipticArcRot(pt, sz, sa, ea,
                                         angle); }
    void DrawEllipticArcRot( const wxRect& rect,
                             double sa = 0, double ea = 0, double angle = 0 )
        { m_targetDc->DrawEllipticArcRot(rect, sa, ea, angle); }
#endif // defined(__WXWINCE__)
    void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) { m_targetDc->DrawIcon(icon, x, y); }
    void DrawIcon(const wxIcon& icon, const wxPoint& pt) { m_targetDc->DrawIcon(icon, pt); }
    virtual void DrawLabel(const wxString& text,
                           const wxBitmap& image,
                           const wxRect& rect,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1,
                           wxRect *rectBounding = NULL)
        { m_targetDc->DrawLabel(text, image, rect, alignment,
                                indexAccel, rectBounding); }
    void DrawLabel(const wxString& text, const wxRect& rect,
                   int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                   int indexAccel = -1)
        { m_targetDc->DrawLabel(text, rect, alignment, indexAccel); }
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
        { m_targetDc->DrawLine(x1, y1, x2, y2); }
    void DrawLine(const wxPoint& pt1, const wxPoint& pt2) { m_targetDc->DrawLine(pt1, pt2); }
    void DrawLines(int n, wxPoint points[],
                   wxCoord xoffset = 0, wxCoord yoffset = 0)
        { m_targetDc->DrawLines(n, points, xoffset, yoffset); }
    void DrawLines(const wxList *list,
                   wxCoord xoffset = 0, wxCoord yoffset = 0)
        { m_targetDc->DrawLines(list, xoffset, yoffset); }
    virtual void DrawObject(wxDrawObject* drawobject) { m_targetDc->DrawObject(drawobject); }
    void DrawPoint(wxCoord x, wxCoord y) { m_targetDc->DrawPoint(x, y); }
    void DrawPoint(const wxPoint& pt) { m_targetDc->DrawPoint(pt); }
    void DrawPolyPolygon(int n, int count[], wxPoint points[],
                         wxCoord xoffset = 0, wxCoord yoffset = 0,
                         int fillStyle = wxODDEVEN_RULE)
        { m_targetDc->DrawPolyPolygon(n, count, points, xoffset,
                                      yoffset, fillStyle); }
    void DrawPolygon(int n, wxPoint points[],
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE)
        { m_targetDc->DrawPolygon(n, points, xoffset, yoffset,
                                  fillStyle); }
    void DrawPolygon(const wxList *list,
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE)
        { m_targetDc->DrawPolygon(list, xoffset, yoffset, fillStyle); }
    void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { m_targetDc->DrawRectangle(x, y, width, height); }
    void DrawRectangle(const wxPoint& pt, const wxSize& sz) { m_targetDc->DrawRectangle(pt, sz); }
    void DrawRectangle(const wxRect& rect) { m_targetDc->DrawRectangle(rect); }
    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
        { m_targetDc->DrawRotatedText(text, x, y, angle); }
    void DrawRotatedText(const wxString& text, const wxPoint& pt, double angle)
        { m_targetDc->DrawRotatedText(text, pt, angle); }
    void DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height,
                              double radius)
        { m_targetDc->DrawRoundedRectangle(x, y, width, height,
                                           radius); }
    void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz,
                             double radius)
        { m_targetDc->DrawRoundedRectangle(pt, sz, radius); }
    void DrawRoundedRectangle(const wxRect& r, double radius)
        { m_targetDc->DrawRoundedRectangle(r, radius); }
#if wxUSE_SPLINES
    void DrawSpline(wxCoord x1, wxCoord y1,
                    wxCoord x2, wxCoord y2,
                    wxCoord x3, wxCoord y3)
        { m_targetDc->DrawSpline(x1, y1, x2, y2,
                                 x3, y3); }
    void DrawSpline(int n, wxPoint points[]) { m_targetDc->DrawSpline(n, points); }
    void DrawSpline(wxList *points) { m_targetDc->DrawSpline(points); }
#endif // wxUSE_SPLINES
    void DrawText(const wxString& text, wxCoord x, wxCoord y)
        { m_targetDc->DrawText(text, x, y); }
    void DrawText(const wxString& text, const wxPoint& pt) { m_targetDc->DrawText(text, pt); }
    virtual void EndDoc() { m_targetDc->EndDoc(); }
    virtual void EndPage() { m_targetDc->EndPage(); }
    bool FloodFill(wxCoord x, wxCoord y, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        { return m_targetDc->FloodFill(x, y, col, style); }
    bool FloodFill(const wxPoint& pt, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        { return m_targetDc->FloodFill(pt, col, style); }
    virtual const wxBrush&  GetBackground() const { return m_targetDc->GetBackground(); }
    virtual int GetBackgroundMode() const { return m_targetDc->GetBackgroundMode(); }
    virtual const wxBrush&  GetBrush() const { return m_targetDc->GetBrush(); }
    virtual wxCoord GetCharHeight() const { return m_targetDc->GetCharHeight(); }
    virtual wxCoord GetCharWidth() const { return m_targetDc->GetCharWidth(); }
    void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *w, wxCoord *h) const
        { m_targetDc->GetClippingBox(x, y, w, h); }
    void GetClippingBox(wxRect& rect) const { m_targetDc->GetClippingBox(rect); }
    void GetClippingBox(long *x, long *y, long *w, long *h) const
        { m_targetDc->GetClippingBox(x, y, w, h); }
    virtual int GetDepth() const { return m_targetDc->GetDepth(); }
    void GetDeviceOrigin(wxCoord *x, wxCoord *y) const { m_targetDc->GetDeviceOrigin(x, y); }
    wxPoint GetDeviceOrigin() const { return m_targetDc->GetDeviceOrigin(); }
    void GetDeviceOrigin(long *x, long *y) const { m_targetDc->GetDeviceOrigin(x, y); }
    virtual const wxFont&   GetFont() const { return m_targetDc->GetFont(); }
    virtual int GetLogicalFunction() const { return m_targetDc->GetLogicalFunction(); }
    void GetLogicalOrigin(wxCoord *x, wxCoord *y) const { m_targetDc->GetLogicalOrigin(x, y); }
    wxPoint GetLogicalOrigin() const { return m_targetDc->GetLogicalOrigin(); }
    void GetLogicalOrigin(long *x, long *y) const { m_targetDc->GetLogicalOrigin(x, y); }
    virtual void GetLogicalScale(double *x, double *y) { m_targetDc->GetLogicalScale(x, y); }
    virtual int GetMapMode() const { return m_targetDc->GetMapMode(); }
    virtual void GetMultiLineTextExtent(const wxString& text,
                                        wxCoord *width,
                                        wxCoord *height,
                                        wxCoord *heightLine = NULL,
                                        wxFont *font = NULL)
        { m_targetDc->GetMultiLineTextExtent(text, width, height, heightLine,
                                             font); }
#if WXWIN_COMPATIBILITY_2_4
    virtual bool GetOptimization() { return m_targetDc->GetOptimization(); }
#endif // WXWIN_COMPATIBILITY_2_4
    virtual wxSize GetPPI() const { return m_targetDc->GetPPI(); }
    bool GetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
        { return m_targetDc->GetPartialTextExtents(text, widths); }
    virtual const wxPen&    GetPen() const { return m_targetDc->GetPen(); }
    bool GetPixel(wxCoord x, wxCoord y, wxColour *col) const
        { return m_targetDc->GetPixel(x, y, col); }
    bool GetPixel(const wxPoint& pt, wxColour *col) const { return m_targetDc->GetPixel(pt, col); }
    void GetSize(int *width, int *height) const { m_targetDc->GetSize(width, height); }
    wxSize GetSize() const { return m_targetDc->GetSize(); }
    void GetSizeMM(int* width, int* height) const { m_targetDc->GetSizeMM(width, height); }
    wxSize GetSizeMM() const { return m_targetDc->GetSizeMM(); }
    virtual const wxColour& GetTextBackground() const { return m_targetDc->GetTextBackground(); }
    void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxCoord *descent = NULL,
                       wxCoord *externalLeading = NULL,
                       wxFont *theFont = NULL) const
        { m_targetDc->GetTextExtent(string, x, y, descent,
                                    externalLeading, theFont); }
    void GetTextExtent(const wxString& string,
                       long *x, long *y,
                       long *descent = NULL,
                       long *externalLeading = NULL,
                       wxFont *theFont = NULL) const
        { m_targetDc->GetTextExtent(string, x, y, descent,
                                    externalLeading, theFont); }
    virtual const wxColour& GetTextForeground() const { return m_targetDc->GetTextForeground(); }
    virtual void GetUserScale(double *x, double *y) const { m_targetDc->GetUserScale(x, y); }
    void GradientFillConcentric(const wxRect& rect,
                                const wxColour& initialColour,
                                const wxColour& destColour)
        { m_targetDc->GradientFillConcentric(rect, initialColour, destColour); }
    void GradientFillConcentric(const wxRect& rect,
                                const wxColour& initialColour,
                                const wxColour& destColour,
                                const wxPoint& circleCenter)
        { m_targetDc->GradientFillConcentric(rect, initialColour, destColour, circleCenter); }
    void GradientFillLinear(const wxRect& rect,
                            const wxColour& initialColour,
                            const wxColour& destColour,
                            wxDirection nDirection = wxEAST)
        { m_targetDc->GradientFillLinear(rect, initialColour, destColour, nDirection); }
    wxCoord LogicalToDeviceX(wxCoord x) const { return m_targetDc->LogicalToDeviceX(x); }
    wxCoord LogicalToDeviceXRel(wxCoord x) const { return m_targetDc->LogicalToDeviceXRel(x); }
    wxCoord LogicalToDeviceY(wxCoord y) const { return m_targetDc->LogicalToDeviceY(y); }
    wxCoord LogicalToDeviceYRel(wxCoord y) const { return m_targetDc->LogicalToDeviceYRel(y); }
    wxCoord MaxX() const { return m_targetDc->MaxX(); }
    wxCoord MaxY() const { return m_targetDc->MaxY(); }
    wxCoord MinX() const { return m_targetDc->MinX(); }
    wxCoord MinY() const { return m_targetDc->MinY(); }
    virtual bool Ok() const { return m_targetDc->Ok(); }
    void ResetBoundingBox()
        { m_targetDc->ResetBoundingBox(); }
#if defined(__WXWINCE__)
    void Rotate( wxList* points, double angle, wxPoint center = wxPoint(0,0) )
        { m_targetDc->Rotate(points, angle, center, 0)); }
#endif // defined(__WXWINCE__)
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp)
        { m_targetDc->SetAxisOrientation(xLeftRight, yBottomUp); }
    virtual void SetBackground(const wxBrush& brush) { m_targetDc->SetBackground(brush); }
    virtual void SetBackgroundMode(int mode) { m_targetDc->SetBackgroundMode(mode); }
    virtual void SetBrush(const wxBrush& brush) { m_targetDc->SetBrush(brush); }
    void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { m_targetDc->SetClippingRegion(x, y, width, height); }
    void SetClippingRegion(const wxPoint& pt, const wxSize& sz)
        { m_targetDc->SetClippingRegion(pt, sz); }
    void SetClippingRegion(const wxRect& rect) { m_targetDc->SetClippingRegion(rect); }
    void SetClippingRegion(const wxRegion& region) { m_targetDc->SetClippingRegion(region); }
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y) { m_targetDc->SetDeviceOrigin(x, y); }
    virtual void SetFont(const wxFont& font) { m_targetDc->SetFont(font); }
    virtual void SetLogicalFunction(int function) { m_targetDc->SetLogicalFunction(function); }
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y) { m_targetDc->SetLogicalOrigin(x, y); }
    virtual void SetLogicalScale(double x, double y) { m_targetDc->SetLogicalScale(x, y); }
    virtual void SetMapMode(int mode) { m_targetDc->SetMapMode(mode); }
#if WXWIN_COMPATIBILITY_2_4
    virtual void SetOptimization(bool opt) { m_targetDc->SetOptimization(opt); }
#endif // WXWIN_COMPATIBILITY_2_4
#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) { m_targetDc->SetPalette(palette); }
#endif // wxUSE_PALETTE
    virtual void SetPen(const wxPen& pen) { m_targetDc->SetPen(pen); }
    virtual void SetTextBackground(const wxColour& colour) { m_targetDc->SetTextBackground(colour); }
    virtual void SetTextForeground(const wxColour& colour) { m_targetDc->SetTextForeground(colour); }
    virtual void SetUserScale(double x, double y) { m_targetDc->SetUserScale(x, y); }
    virtual bool StartDoc(const wxString& message) { return m_targetDc->StartDoc(message); }
    virtual void StartPage() { m_targetDc->StartPage(); }
protected:
    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord)
        { return m_targetDc->Blit(xdest, ydest, width, height,
                                  source, xsrc, ysrc, rop,
                                  useMask, xsrcMask, ysrcMask); }
    virtual void DoCrossHair(wxCoord x, wxCoord y) { m_targetDc->CrossHair(x, y); }
    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc)
        { m_targetDc->DrawArc(x1, y1, x2, y2,
                              xc, yc); }
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false)
        { m_targetDc->DrawBitmap(bmp, x, y, useMask); }
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
        { m_targetDc->DrawCheckMark(x, y, width, height); }
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height)
        { m_targetDc->DrawEllipse(x, y, width, height); }
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea)
        { m_targetDc->DrawEllipticArc(x, y, w, h,
                                      sa, ea); }
    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
        { m_targetDc->DrawIcon(icon, x, y); }
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
        { m_targetDc->DrawLine(x1, y1, x2, y2); }
    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset)
        { m_targetDc->DrawLines(n, points, xoffset, yoffset); }
    virtual void DoDrawPoint(wxCoord x, wxCoord y) { m_targetDc->DrawPoint(x, y); }
    virtual void DoDrawPolyPolygon(int n, int count[], wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle)
        { m_targetDc->DrawPolyPolygon(n, count, points, xoffset,
                                      yoffset, fillStyle); }
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE)
        { m_targetDc->DrawPolygon(n, points, xoffset, yoffset,
                                  fillStyle); }
    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { m_targetDc->DrawRectangle(x, y, width, height); }
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle)
        { m_targetDc->DrawRotatedText(text, x, y, angle); }
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius)
        { m_targetDc->DrawRoundedRectangle(x, y, width, height,
                                           radius); }
#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points) { m_targetDc->DrawSpline(points); }
#endif // wxUSE_SPLINES
    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y)
        { m_targetDc->DrawText(text, x, y); }
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE)
        { return m_targetDc->FloodFill(x, y, col, style); }
    virtual void DoGetClippingBox(wxCoord *x, wxCoord *y,
                                  wxCoord *w, wxCoord *h) const
        { m_targetDc->GetClippingBox(x, y, w, h); }
    virtual void DoGetDeviceOrigin(wxCoord *x, wxCoord *y) const
        { m_targetDc->GetDeviceOrigin(x, y); }
    virtual void DoGetLogicalOrigin(wxCoord *x, wxCoord *y) const
        { m_targetDc->GetLogicalOrigin(x, y); }
    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
        { return m_targetDc->GetPartialTextExtents(text, widths); }
    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
        { return m_targetDc->GetPixel(x, y, col); }
    virtual void DoGetSize(int *width, int *height) const { m_targetDc->GetSize(width, height); }
    virtual void DoGetSizeMM(int* width, int* height) const { m_targetDc->GetSizeMM(width, height); }
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const
        { m_targetDc->GetTextExtent(string, x, y, descent,
                                    externalLeading, theFont); }
    virtual void DoGradientFillLinear(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      wxDirection nDirection = wxEAST)
        { m_targetDc->GradientFillLinear(rect, initialColour, destColour, nDirection); }
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height)
        { m_targetDc->SetClippingRegion(x, y, width, height); }
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region)
        { m_targetDc->SetClippingRegion(region); }
};


// ----------------------------------------------------------------------------
// Double buffered PaintDC.
// ----------------------------------------------------------------------------

// Creates a double buffered wxPaintDC, optionally allowing the
// user to specify their own buffer to use.
class wxBufferedPaintDC : public wxBufferedDC
{
public:
    // If no bitmap is supplied by the user, a temporary one will be created.
    wxBufferedPaintDC(wxWindow *window, const wxBitmap& buffer, int style = wxBUFFER_CLIENT_AREA)
        : m_paintdc(window)
    {
        // If we're buffering the virtual window, scale the paint DC as well
        if (style & wxBUFFER_VIRTUAL_AREA)
            window->PrepareDC( m_paintdc );

        if( buffer != wxNullBitmap )
            Init(&m_paintdc, buffer, style);
        else
            Init(window, &m_paintdc, window->GetClientSize(), style);
    }

    // If no bitmap is supplied by the user, a temporary one will be created.
    wxBufferedPaintDC(wxWindow *window, int style = wxBUFFER_CLIENT_AREA)
        : m_paintdc(window)
    {
        // If we're using the virtual window, scale the paint DC as well
        if (style & wxBUFFER_VIRTUAL_AREA)
            window->PrepareDC( m_paintdc );

        Init(window, &m_paintdc, window->GetClientSize(), style);
    }

    // default copy ctor ok.

    virtual ~wxBufferedPaintDC()
    {
        // We must UnMask here, else by the time the base class
        // does it, the PaintDC will have already been destroyed.
        UnMask();
    }

private:
    wxPaintDC m_paintdc;

    DECLARE_NO_COPY_CLASS(wxBufferedPaintDC)
};



//
// wxAutoBufferedPaintDC is a wxPaintDC in toolkits which have double-
// buffering by default. Otherwise it is a wxBufferedPaintDC. Thus,
// you can only expect it work with a simple constructor that
// accepts single wxWindow* argument.
//
#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    #define wxAutoBufferedPaintDCBase           wxPaintDC
#else
    #define wxAutoBufferedPaintDCBase           wxBufferedPaintDC
#endif


#ifdef __WXDEBUG__

class wxAutoBufferedPaintDC : public wxAutoBufferedPaintDCBase
{
public:

    wxAutoBufferedPaintDC(wxWindow* win)
        : wxAutoBufferedPaintDCBase(win)
    {
        TestWinStyle(win);
    }

    virtual ~wxAutoBufferedPaintDC() { }

private:

    void TestWinStyle(wxWindow* win)
    {
        // Help the user to get the double-buffering working properly.
        wxASSERT_MSG( win->GetBackgroundStyle() == wxBG_STYLE_CUSTOM,
                      wxT("In constructor, you need to call GetBackgroundStyle(wxBG_STYLE_CUSTOM), ")
                      wxT("and also, if needed, paint the background manually in the paint event handler."));
    }

    DECLARE_NO_COPY_CLASS(wxAutoBufferedPaintDC)
};

#else // !__WXDEBUG__

// In release builds, just use typedef
typedef wxAutoBufferedPaintDCBase wxAutoBufferedPaintDC;

#endif


#endif  // _WX_DCBUFFER_H_
