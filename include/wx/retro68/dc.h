/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/dc.h
// Purpose:     wxRetro68DCImpl: QuickDraw-based wxDC implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_DC_H_
#define _WX_RETRO68_DC_H_

#include "wx/defs.h"
#include "wx/region.h"
#include "wx/dc.h"

// Toolbox structures, declared in Quickdraw.h
struct GrafPort;
struct Rect;

//-----------------------------------------------------------------------------
// wxRetro68DCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRetro68DCImpl : public wxDCImpl
{
public:
    explicit wxRetro68DCImpl(wxDC *owner);
    virtual ~wxRetro68DCImpl();

    // implement base class pure virtuals
    // ----------------------------------

    virtual bool IsOk() const override { return m_port != nullptr; }

    virtual void Clear() override;

    virtual bool StartDoc(const wxString& WXUNUSED(message)) override { return true; }
    virtual void EndDoc() override { }

    virtual void StartPage() override { }
    virtual void EndPage() override { }

    virtual void SetFont(const wxFont& font) override;
    virtual void SetPen(const wxPen& pen) override;
    virtual void SetBrush(const wxBrush& brush) override;
    virtual void SetBackground(const wxBrush& brush) override;
    virtual void SetBackgroundMode(int mode) override;
#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& WXUNUSED(palette)) override { }
#endif

    virtual void SetLogicalFunction(wxRasterOperationMode function) override;

    virtual void DestroyClippingRegion() override;

    virtual wxCoord GetCharHeight() const override;
    virtual wxCoord GetCharWidth() const override;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = nullptr,
                                 wxCoord *externalLeading = nullptr,
                                 const wxFont *theFont = nullptr) const override;

    virtual bool CanDrawBitmap() const override { return true; }
    virtual bool CanGetTextExtent() const override { return true; }
    virtual int GetDepth() const override;
    virtual wxSize GetPPI() const override;

    // implementation from now on
    // --------------------------

    // Return the QuickDraw port used by this DC.
    GrafPort* GetPort() const { return m_port; }

    // Return the offset of the DC device origin in the port coordinates.
    const wxPoint& GetPortOffset() const { return m_portOffset; }

    // Make the port of this DC current and set up its clipping, should be
    // called before any drawing operation.
    void SelectPort() const;

    // Forget the cached port state, must be called if the port of this DC
    // was changed outside of it.
    static void InvalidateCachedPortState();

protected:
    // Initialize the DC to draw on the given port, with the given offset of
    // the DC origin in the port coordinates and limiting all drawing to the
    // given clipping rectangle (in port coordinates too).
    void InitForPort(GrafPort* port,
                     const wxPoint& offset,
                     const wxRect& clipRect);

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) override;

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const override;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) override;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) override;
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height) override;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) override;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height) override;

    virtual void DoCrossHair(wxCoord x, wxCoord y) override;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) override;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false) override;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) override;
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) override;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY, bool useMask = false,
                        wxCoord xsrcMask = -1, wxCoord ysrcMask = -1) override;

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               wxRasterOperationMode rop = wxCOPY,
                               bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord,
                               wxCoord ysrcMask = wxDefaultCoord) override;

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) override;
    virtual void DoSetDeviceClippingRegion(const wxRegion& region) override;

    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) override;
    virtual void DoDrawPolygon(int n, const wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;

    // Size of the drawing area, in device coordinates.
    wxSize m_size;

private:
    // Coordinates conversion helpers: from logical to port coordinates.
    int XToPort(wxCoord x) const;
    int YToPort(wxCoord y) const;
    int XLenToPort(wxCoord w) const { return LogicalToDeviceXRel(w); }
    int YLenToPort(wxCoord h) const { return LogicalToDeviceYRel(h); }

    // Return the rectangle in port coordinates for the given logical one.
    struct Rect PortRectFromLogical(wxCoord x, wxCoord y,
                                    wxCoord w, wxCoord h) const;

    // Set up the port for filling the area using the current brush, return
    // false if nothing should be drawn.
    bool SelectBrush() const;

    // Set up the port for drawing the outline using the current pen, return
    // false if nothing should be drawn.
    bool SelectPen() const;

    // Set up the port for drawing text using the current font.
    void SelectFont(const wxFont& font) const;

    // Draw the bitmap or a part of it at the given position in port
    // coordinates.
    void DrawBitmapPart(const wxBitmap& bmp,
                        const wxRect& rectSrc,
                        const wxRect& rectDst,
                        wxRasterOperationMode rop,
                        bool useMask) const;

    // Update the QuickDraw clipping region of the port from our clipping
    // region.
    void UpdateClipRgn() const;

    // Update the bounding box using the given rectangle in port coordinates.
    void CalcBoundingBoxFromPort(const struct Rect& r);


    // The QuickDraw port: either the window port or an offscreen GWorld.
    GrafPort* m_port;

    // The device handle associated with the port, only used for GWorlds.
    void* m_device;

    // True if drawing on a monochrome device.
    bool m_isMono = false;

    // Offset of the DC device origin in port coordinates.
    wxPoint m_portOffset;

    // The area to which all drawing is clipped, in port coordinates.
    wxRect m_portClipRect;

    // The user-defined clipping region, in device coordinates (only valid if
    // m_clipping is true).
    wxRegion m_userClipRegion;

    friend class wxMemoryDCImpl;
    friend class wxWindowDCImpl;
    friend class wxScreenDCImpl;

    wxDECLARE_ABSTRACT_CLASS(wxRetro68DCImpl);
    wxDECLARE_NO_COPY_CLASS(wxRetro68DCImpl);
};

#endif // _WX_RETRO68_DC_H_
