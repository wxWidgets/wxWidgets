/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dc.cpp
// Purpose:     wxRetro68DCImpl class: QuickDraw-based wxDC implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
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

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
    #include "wx/log.h"
    #include "wx/math.h"
    #include "wx/icon.h"
#endif

#include "wx/retro68/dc.h"
#include "wx/retro68/dcmemory.h"
#include "wx/retro68/private/qd.h"

#include <vector>

// ===========================================================================
// helpers
// ===========================================================================

namespace
{

// The DC whose port and clipping region are currently selected.
const wxRetro68DCImpl* gs_selectedDC = nullptr;

// Patterns used for hatched brushes and dashed pens.
const Pattern wxPatternBDiagonal =
    { { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 } };
const Pattern wxPatternFDiagonal =
    { { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 } };
const Pattern wxPatternCrossDiag =
    { { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 } };
const Pattern wxPatternHorizontal =
    { { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
const Pattern wxPatternVertical =
    { { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 } };
const Pattern wxPatternCross =
    { { 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 } };
const Pattern wxPatternDot =
    { { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 } };
const Pattern wxPatternShortDash =
    { { 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0x33, 0x33 } };
const Pattern wxPatternLongDash =
    { { 0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f } };
const Pattern wxPatternDotDash =
    { { 0xe4, 0xe4, 0x4e, 0x4e, 0xe4, 0xe4, 0x4e, 0x4e } };
const Pattern wxPatternSolid =
    { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
const Pattern wxPatternEmpty =
    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

// Return the pattern corresponding to the given brush style or null if the
// style doesn't use a pattern.
const Pattern* GetBrushPattern(wxBrushStyle style)
{
    switch ( style )
    {
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:  return &wxPatternBDiagonal;
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:  return &wxPatternFDiagonal;
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:  return &wxPatternCrossDiag;
        case wxBRUSHSTYLE_HORIZONTAL_HATCH: return &wxPatternHorizontal;
        case wxBRUSHSTYLE_VERTICAL_HATCH:   return &wxPatternVertical;
        case wxBRUSHSTYLE_CROSS_HATCH:      return &wxPatternCross;
        default:                            return nullptr;
    }
}

const Pattern* GetPenPattern(wxPenStyle style)
{
    switch ( style )
    {
        case wxPENSTYLE_DOT:                return &wxPatternDot;
        case wxPENSTYLE_SHORT_DASH:         return &wxPatternShortDash;
        case wxPENSTYLE_LONG_DASH:          return &wxPatternLongDash;
        case wxPENSTYLE_DOT_DASH:           return &wxPatternDotDash;
        case wxPENSTYLE_USER_DASH:          return &wxPatternShortDash;
        case wxPENSTYLE_BDIAGONAL_HATCH:    return &wxPatternBDiagonal;
        case wxPENSTYLE_FDIAGONAL_HATCH:    return &wxPatternFDiagonal;
        case wxPENSTYLE_CROSSDIAG_HATCH:    return &wxPatternCrossDiag;
        case wxPENSTYLE_HORIZONTAL_HATCH:   return &wxPatternHorizontal;
        case wxPENSTYLE_VERTICAL_HATCH:     return &wxPatternVertical;
        case wxPENSTYLE_CROSS_HATCH:        return &wxPatternCross;
        default:                            return &wxPatternSolid;
    }
}

// Colour helpers working for both colour and black and white QuickDraw.
bool IsDarkColour(const wxColour& col)
{
    return col.GetLuminance() < 0.5;
}

// Set the colours, mapping them to black or white ourselves if the
// destination is monochrome as QuickDraw maps all the non-white colours to
// black in this case, which is not what we want.
void SetForeColour(const wxColour& col, bool mono = false)
{
    if ( wxMacHasColorQD() && !mono )
    {
        RGBColor rgb = wxMacColourToRGB(col);
        RGBForeColor(&rgb);
    }
    else
    {
        ForeColor(IsDarkColour(col) ? blackColor : whiteColor);
    }
}

void SetBackColour(const wxColour& col, bool mono = false)
{
    if ( wxMacHasColorQD() && !mono )
    {
        RGBColor rgb = wxMacColourToRGB(col);
        RGBBackColor(&rgb);
    }
    else
    {
        BackColor(IsDarkColour(col) ? blackColor : whiteColor);
    }
}

// Set colours to black and white which is necessary for CopyBits() to not
// colourize the image.
void SetCopyBitsColours()
{
    SetForeColour(*wxBLACK);
    SetBackColour(*wxWHITE);
}

// Convert wx raster operation to QuickDraw source transfer mode, return -1
// if it's not supported.
short GetSourceMode(wxRasterOperationMode rop)
{
    switch ( rop )
    {
        case wxCOPY:        return srcCopy;
        case wxOR:          return srcOr;
        case wxXOR:         return srcXor;
        case wxSRC_INVERT:  return notSrcCopy;
        case wxAND_INVERT:  return srcBic;
        case wxOR_INVERT:   return notSrcOr;
        default:            return -1;
    }
}

} // anonymous namespace

// Offscreen GWorld used for text measurements when the DC is not available.
GWorldPtr wxRetro68GetScratchPort()
{
    static GWorldPtr s_gworld = nullptr;
    if ( !s_gworld )
    {
        Rect r = { 0, 0, 1, 1 };
        if ( NewGWorld(&s_gworld, 1, &r, nullptr, nullptr, 0) != noErr )
            s_gworld = nullptr;
    }

    return s_gworld;
}

namespace
{

// Compute the end point of the line excluding the last point as QuickDraw
// includes it but wx doesn't.
void ExcludeLastPoint(int x1, int y1, int& x2, int& y2)
{
    const int dx = x2 - x1;
    const int dy = y2 - y1;

    if ( abs(dx) >= abs(dy) )
    {
        const int x2new = x2 - (dx > 0 ? 1 : -1);
        y2 = y1 + (dx ? (dy * (x2new - x1) + dx / 2) / dx : 0);
        x2 = x2new;
    }
    else
    {
        const int y2new = y2 - (dy > 0 ? 1 : -1);
        x2 = x1 + (dy ? (dx * (y2new - y1) + dy / 2) / dy : 0);
        y2 = y2new;
    }
}

} // anonymous namespace

// Measure the text using the given font, this is used by the DC but also
// by wxWindow and other code needing to measure the text without a DC.
void wxRetro68GetTextExtent(const wxFont& font,
                            const wxString& text,
                            wxCoord *x, wxCoord *y,
                            wxCoord *descent,
                            wxCoord *externalLeading)
{
    wxMacPortSaver saver;

    GWorldPtr gworld = wxRetro68GetScratchPort();
    if ( !gworld )
    {
        if ( x ) *x = 0;
        if ( y ) *y = 0;
        if ( descent ) *descent = 0;
        if ( externalLeading ) *externalLeading = 0;
        return;
    }

    SetGWorld(gworld, nullptr);

    const wxFont& f = font.IsOk() ? font : *wxNORMAL_FONT;
    TextFont(f.GetMacFontID());
    TextSize(f.GetMacFontSize());
    TextFace(f.GetMacFontStyle());

    FontInfo fi;
    GetFontInfo(&fi);

    if ( x )
    {
        const wxCharBuffer buf = wxMacTextToSystem(text);
        *x = buf.length()
                ? TextWidth(const_cast<char*>(buf.data()), 0,
                            static_cast<short>(wxMin(buf.length(), size_t(32767))))
                : 0;
    }

    if ( y )
        *y = fi.ascent + fi.descent + fi.leading;
    if ( descent )
        *descent = fi.descent;
    if ( externalLeading )
        *externalLeading = fi.leading;

    wxRetro68DCImpl::InvalidateCachedPortState();
}

// ===========================================================================
// wxRetro68DCImpl implementation
// ===========================================================================

wxIMPLEMENT_ABSTRACT_CLASS(wxRetro68DCImpl, wxDCImpl);

wxRetro68DCImpl::wxRetro68DCImpl(wxDC *owner)
    : wxDCImpl(owner)
{
    m_port = nullptr;
    m_device = nullptr;
}

wxRetro68DCImpl::~wxRetro68DCImpl()
{
    if ( gs_selectedDC == this )
        gs_selectedDC = nullptr;
}

void wxRetro68DCImpl::InitForPort(GrafPtr port,
                                  const wxPoint& offset,
                                  const wxRect& clipRect)
{
    m_port = port;
    m_portOffset = offset;

    // Check if we're drawing on a monochrome device: this is only possible
    // for the window ports, as we always use 32bpp offscreen GWorlds.
    m_isMono = !wxMacHasColorQD();
    if ( !m_isMono && !m_device )
    {
        GDHandle device = GetMainDevice();
        m_isMono = device && (**(**device).gdPMap).pixelSize == 1;
    }
    m_portClipRect = clipRect;

    m_ok = port != nullptr;

    // default font, pen and brush:
    SetFont(*wxNORMAL_FONT);
    SetPen(*wxBLACK_PEN);
    SetBrush(*wxWHITE_BRUSH);
}

/* static */
void wxRetro68DCImpl::InvalidateCachedPortState()
{
    gs_selectedDC = nullptr;
}

void wxRetro68DCImpl::SelectPort() const
{
    if ( gs_selectedDC == this )
    {
        // Check that the port is still current, it could have been changed
        // by some other code.
        CGrafPtr port;
        GDHandle device;
        GetGWorld(&port, &device);
        if ( reinterpret_cast<GrafPtr>(port) == m_port )
            return;
    }

    if ( m_device )
    {
        // This is an offscreen GWorld, the device is ignored for it.
        SetGWorld(reinterpret_cast<CGrafPtr>(m_port), nullptr);
    }
    else
    {
        wxMacSetWindowPort(m_port);
    }

    UpdateClipRgn();

    gs_selectedDC = this;
}

void wxRetro68DCImpl::UpdateClipRgn() const
{
    RgnHandle rgn = NewRgn();

    const Rect r = wxMacRectFromWX(m_portClipRect);
    RectRgn(rgn, &r);

    if ( m_clipping )
    {
        RgnHandle rgnUser = wxMacRegionToRgn(m_userClipRegion, m_portOffset);
        SectRgn(rgn, rgnUser, rgn);
        DisposeRgn(rgnUser);
    }

    SetClip(rgn);
    DisposeRgn(rgn);
}

// ---------------------------------------------------------------------------
// coordinates
// ---------------------------------------------------------------------------

int wxRetro68DCImpl::XToPort(wxCoord x) const
{
    return LogicalToDeviceX(x) + m_portOffset.x;
}

int wxRetro68DCImpl::YToPort(wxCoord y) const
{
    return LogicalToDeviceY(y) + m_portOffset.y;
}

Rect wxRetro68DCImpl::PortRectFromLogical(wxCoord x, wxCoord y,
                                          wxCoord w, wxCoord h) const
{
    int x1 = XToPort(x),
        y1 = YToPort(y),
        x2 = XToPort(x + w),
        y2 = YToPort(y + h);

    if ( x1 > x2 )
        wxSwap(x1, x2);
    if ( y1 > y2 )
        wxSwap(y1, y2);

    Rect r;
    r.left = wxMacClampCoord(x1);
    r.top = wxMacClampCoord(y1);
    r.right = wxMacClampCoord(x2);
    r.bottom = wxMacClampCoord(y2);
    return r;
}

void wxRetro68DCImpl::CalcBoundingBoxFromPort(const Rect& r)
{
    // Convert back to the logical coordinates.
    const wxPoint p1 = DeviceToLogical(r.left - m_portOffset.x,
                                       r.top - m_portOffset.y);
    const wxPoint p2 = DeviceToLogical(r.right - m_portOffset.x,
                                       r.bottom - m_portOffset.y);
    CalcBoundingBox(p1.x, p1.y, p2.x, p2.y);
}

// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------

void wxRetro68DCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                          wxCoord width, wxCoord height)
{
    wxRect rectDev(LogicalToDevice(x, y), LogicalToDeviceRel(width, height));
    if ( rectDev.width < 0 )
    {
        rectDev.x += rectDev.width;
        rectDev.width = -rectDev.width;
    }
    if ( rectDev.height < 0 )
    {
        rectDev.y += rectDev.height;
        rectDev.height = -rectDev.height;
    }

    wxRegion region(rectDev);
    if ( m_clipping )
        region.Intersect(m_userClipRegion);
    else
        region.Intersect(wxRect(m_size));

    // Let the base class update the clipping box.
    wxDCImpl::DoSetClippingRegion(x, y, width, height);

    m_userClipRegion = region;

    InvalidateCachedPortState();
}

void wxRetro68DCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    wxRegion regionNew(region);
    if ( m_clipping )
        regionNew.Intersect(m_userClipRegion);
    else
        regionNew.Intersect(wxRect(m_size));

    // Let the base class update the clipping box, it takes logical
    // coordinates.
    const wxRect box = regionNew.GetBox();
    wxDCImpl::DoSetClippingRegion(DeviceToLogicalX(box.x),
                                  DeviceToLogicalY(box.y),
                                  DeviceToLogicalXRel(box.width),
                                  DeviceToLogicalYRel(box.height));

    m_userClipRegion = regionNew;

    InvalidateCachedPortState();
}

void wxRetro68DCImpl::DestroyClippingRegion()
{
    ResetClipping();
    m_userClipRegion.Clear();

    InvalidateCachedPortState();
}

// ---------------------------------------------------------------------------
// attributes
// ---------------------------------------------------------------------------

void wxRetro68DCImpl::SetFont(const wxFont& font)
{
    m_font = font;
}

void wxRetro68DCImpl::SetPen(const wxPen& pen)
{
    m_pen = pen.IsOk() ? pen : *wxBLACK_PEN;
}

void wxRetro68DCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush.IsOk() ? brush : *wxWHITE_BRUSH;
}

void wxRetro68DCImpl::SetBackground(const wxBrush& brush)
{
    m_backgroundBrush = brush;
}

void wxRetro68DCImpl::SetBackgroundMode(int mode)
{
    m_backgroundMode = mode;
}

void wxRetro68DCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
    m_logicalFunction = function;
}

bool wxRetro68DCImpl::SelectPen() const
{
    if ( !m_pen.IsOk() || m_pen.GetStyle() == wxPENSTYLE_TRANSPARENT )
        return false;

    if ( m_logicalFunction == wxNO_OP )
        return false;

    SelectPort();

    PenNormal();

    int width = abs(LogicalToDeviceXRel(m_pen.GetWidth()));
    if ( width < 1 )
        width = 1;
    PenSize(width, width);

    PenPat(GetPenPattern(m_pen.GetStyle()));

    switch ( m_logicalFunction )
    {
        case wxXOR:
        case wxINVERT:
            SetForeColour(*wxBLACK);
            PenMode(patXor);
            break;

        case wxCLEAR:
            SetForeColour(*wxWHITE);
            PenMode(patCopy);
            break;

        case wxSET:
            SetForeColour(*wxBLACK);
            PenMode(patCopy);
            break;

        default:
            SetForeColour(m_pen.GetColour(), m_isMono);

            if ( m_pen.GetStyle() != wxPENSTYLE_SOLID && wxMacHasColorQD() )
            {
                // Only draw the pattern pixels, without using the background
                // colour for the others.
                SetBackColour(m_pen.GetColour().ChangeLightness(50), m_isMono);
                PenMode(transparent);
            }
            else
            {
                PenMode(m_pen.GetStyle() == wxPENSTYLE_SOLID ? patCopy : patOr);
            }
    }

    return true;
}

bool wxRetro68DCImpl::SelectBrush() const
{
    if ( !m_brush.IsOk() || m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT )
        return false;

    if ( m_logicalFunction == wxNO_OP )
        return false;

    SelectPort();

    PenNormal();

    const Pattern* const pattern = GetBrushPattern(m_brush.GetStyle());
    PenPat(pattern ? pattern : &wxPatternSolid);

    switch ( m_logicalFunction )
    {
        case wxXOR:
        case wxINVERT:
            SetForeColour(*wxBLACK);
            PenMode(patXor);
            break;

        case wxCLEAR:
            SetForeColour(*wxWHITE);
            PenMode(patCopy);
            break;

        case wxSET:
            SetForeColour(*wxBLACK);
            PenMode(patCopy);
            break;

        default:
            SetForeColour(m_brush.GetColour(), m_isMono);

            if ( pattern )
            {
                if ( m_backgroundMode == wxBRUSHSTYLE_SOLID )
                {
                    SetBackColour(m_textBackgroundColour.IsOk()
                                    ? m_textBackgroundColour
                                    : *wxWHITE, m_isMono);
                    PenMode(patCopy);
                }
                else if ( wxMacHasColorQD() )
                {
                    SetBackColour(m_brush.GetColour().ChangeLightness(50), m_isMono);
                    PenMode(transparent);
                }
                else
                {
                    PenMode(patOr);
                }
            }
            else
            {
                PenMode(patCopy);
            }
    }

    return true;
}

void wxRetro68DCImpl::SelectFont(const wxFont& font) const
{
    const wxFont& f = font.IsOk() ? font : *wxNORMAL_FONT;

    TextFont(f.GetMacFontID());
    TextSize(f.GetMacFontSize());
    TextFace(f.GetMacFontStyle());
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

extern void wxRetro68GetTextExtent(const wxFont& font,
                                   const wxString& text,
                                   wxCoord *x, wxCoord *y,
                                   wxCoord *descent,
                                   wxCoord *externalLeading);

wxCoord wxRetro68DCImpl::GetCharHeight() const
{
    wxCoord h;
    wxRetro68GetTextExtent(m_font, wxString(), nullptr, &h, nullptr, nullptr);
    return DeviceToLogicalYRel(h);
}

wxCoord wxRetro68DCImpl::GetCharWidth() const
{
    wxCoord w;
    wxRetro68GetTextExtent(m_font, "x", &w, nullptr, nullptr, nullptr);
    return DeviceToLogicalXRel(w);
}

void wxRetro68DCImpl::DoGetTextExtent(const wxString& string,
                                      wxCoord *x, wxCoord *y,
                                      wxCoord *descent,
                                      wxCoord *externalLeading,
                                      const wxFont *theFont) const
{
    wxRetro68GetTextExtent(theFont ? *theFont : m_font, string,
                           x, y, descent, externalLeading);

    if ( x )
        *x = DeviceToLogicalXRel(*x);
    if ( y )
        *y = DeviceToLogicalYRel(*y);
    if ( descent )
        *descent = DeviceToLogicalYRel(*descent);
    if ( externalLeading )
        *externalLeading = DeviceToLogicalYRel(*externalLeading);
}

// ---------------------------------------------------------------------------
// misc info
// ---------------------------------------------------------------------------

int wxRetro68DCImpl::GetDepth() const
{
    if ( !wxMacHasColorQD() )
        return 1;

    GDHandle device = GetMainDevice();
    if ( !device )
        return 32;

    return (**(**device).gdPMap).pixelSize;
}

wxSize wxRetro68DCImpl::GetPPI() const
{
    // Classic Mac OS always assumes 72 DPI.
    return wxSize(72, 72);
}

void wxRetro68DCImpl::DoGetSize(int *w, int *h) const
{
    if ( w )
        *w = m_size.x;
    if ( h )
        *h = m_size.y;
}

void wxRetro68DCImpl::DoGetSizeMM(int *width, int *height) const
{
    // 72 DPI means 72 pixels per 25.4mm.
    if ( width )
        *width = m_size.x * 254 / 720;
    if ( height )
        *height = m_size.y * 254 / 720;
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxRetro68DCImpl::Clear()
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( m_backgroundBrush.IsOk() &&
            m_backgroundBrush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT )
        return;

    SelectPort();

    const wxColour col = m_backgroundBrush.IsOk()
                            ? m_backgroundBrush.GetColour()
                            : *wxWHITE;

    PenNormal();
    SetBackColour(col, m_isMono);
    BackPat(&wxPatternEmpty);

    const Rect r = wxMacRectFromWX(m_portClipRect);
    EraseRect(&r);

    // restore the default background colour as other QuickDraw functions
    // could be using it
    SetBackColour(*wxWHITE);
}

bool wxRetro68DCImpl::DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                  const wxColour& WXUNUSED(col),
                                  wxFloodFillStyle WXUNUSED(style))
{
    return false;
}

bool wxRetro68DCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid dc") );
    wxCHECK_MSG( col, false, "null colour" );

    SelectPort();

    const short h = wxMacClampCoord(XToPort(x));
    const short v = wxMacClampCoord(YToPort(y));

    if ( wxMacHasColorQD() )
    {
        RGBColor rgb;
        GetCPixel(h, v, &rgb);
        *col = wxMacRGBToColour(rgb);
    }
    else
    {
        *col = GetPixel(h, v) ? *wxBLACK : *wxWHITE;
    }

    return true;
}

void wxRetro68DCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( !SelectPen() )
        return;

    PenSize(1, 1);

    const short h = wxMacClampCoord(XToPort(x));
    const short v = wxMacClampCoord(YToPort(y));
    MoveTo(h, v);
    LineTo(h, v);

    CalcBoundingBox(x, y);
}

void wxRetro68DCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    int px1 = XToPort(x1),
        py1 = YToPort(y1),
        px2 = XToPort(x2),
        py2 = YToPort(y2);

    if ( px1 == px2 && py1 == py2 )
        return;

    if ( !SelectPen() )
        return;

    ExcludeLastPoint(px1, py1, px2, py2);

    MoveTo(wxMacClampCoord(px1), wxMacClampCoord(py1));
    LineTo(wxMacClampCoord(px2), wxMacClampCoord(py2));

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxRetro68DCImpl::DoDrawLines(int n, const wxPoint points[],
                                  wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( n < 2 || !SelectPen() )
        return;

    MoveTo(wxMacClampCoord(XToPort(points[0].x + xoffset)),
           wxMacClampCoord(YToPort(points[0].y + yoffset)));
    CalcBoundingBox(points[0].x + xoffset, points[0].y + yoffset);

    for ( int i = 1; i < n; i++ )
    {
        int px2 = XToPort(points[i].x + xoffset);
        int py2 = YToPort(points[i].y + yoffset);

        if ( i == n - 1 )
        {
            const int px1 = XToPort(points[i - 1].x + xoffset);
            const int py1 = YToPort(points[i - 1].y + yoffset);
            if ( px1 != px2 || py1 != py2 )
                ExcludeLastPoint(px1, py1, px2, py2);
        }

        LineTo(wxMacClampCoord(px2), wxMacClampCoord(py2));
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }
}

void wxRetro68DCImpl::DoDrawPolygon(int n, const wxPoint points[],
                                    wxCoord xoffset, wxCoord yoffset,
                                    wxPolygonFillMode WXUNUSED(fillStyle))
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( n < 2 )
        return;

    SelectPort();

    PolyHandle poly = OpenPoly();

    const short x0 = wxMacClampCoord(XToPort(points[0].x + xoffset));
    const short y0 = wxMacClampCoord(YToPort(points[0].y + yoffset));
    MoveTo(x0, y0);
    for ( int i = 1; i < n; i++ )
    {
        LineTo(wxMacClampCoord(XToPort(points[i].x + xoffset)),
               wxMacClampCoord(YToPort(points[i].y + yoffset)));
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }
    LineTo(x0, y0);

    ClosePoly();

    if ( !poly )
        return;

    if ( SelectBrush() )
        PaintPoly(poly);

    if ( SelectPen() )
        FramePoly(poly);

    KillPoly(poly);

    CalcBoundingBox(points[0].x + xoffset, points[0].y + yoffset);
}

void wxRetro68DCImpl::DoDrawRectangle(wxCoord x, wxCoord y,
                                      wxCoord width, wxCoord height)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    const Rect r = PortRectFromLogical(x, y, width, height);
    if ( EmptyRect(&r) )
        return;

    if ( SelectBrush() )
        PaintRect(&r);

    if ( SelectPen() )
        FrameRect(&r);

    CalcBoundingBoxFromPort(r);
}

void wxRetro68DCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                             wxCoord width, wxCoord height,
                                             double radius)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    const Rect r = PortRectFromLogical(x, y, width, height);
    if ( EmptyRect(&r) )
        return;

    // negative radius means proportion of the smaller dimension
    if ( radius < 0.0 )
        radius = -radius * wxMin(r.right - r.left, r.bottom - r.top);
    else
        radius = abs(XLenToPort(wxRound(radius)));

    const short oval = wxMacClampCoord(wxRound(2 * radius));

    if ( SelectBrush() )
        PaintRoundRect(&r, oval, oval);

    if ( SelectPen() )
        FrameRoundRect(&r, oval, oval);

    CalcBoundingBoxFromPort(r);
}

void wxRetro68DCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    const Rect r = PortRectFromLogical(x, y, width, height);
    if ( EmptyRect(&r) )
        return;

    if ( SelectBrush() )
        PaintOval(&r);

    if ( SelectPen() )
        FrameOval(&r);

    CalcBoundingBoxFromPort(r);
}

void wxRetro68DCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y,
                                        wxCoord w, wxCoord h,
                                        double sa, double ea)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    const Rect r = PortRectFromLogical(x, y, w, h);
    if ( EmptyRect(&r) )
        return;

    // wx angles are counter-clockwise from 3 o'clock while QuickDraw ones
    // are clockwise from 12 o'clock.
    double sweep = ea - sa;
    while ( sweep <= 0 )
        sweep += 360;
    while ( sweep > 360 )
        sweep -= 360;

    const short startAngle = static_cast<short>(wxRound(90 - sa));
    const short arcAngle = static_cast<short>(-wxRound(sweep));

    if ( SelectBrush() )
        PaintArc(&r, startAngle, arcAngle);

    if ( SelectPen() )
        FrameArc(&r, startAngle, arcAngle);

    CalcBoundingBoxFromPort(r);
}

void wxRetro68DCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                                wxCoord x2, wxCoord y2,
                                wxCoord xc, wxCoord yc)
{
    const double dx1 = x1 - xc;
    const double dy1 = y1 - yc;
    const double radius = sqrt(dx1 * dx1 + dy1 * dy1);
    const wxCoord r = wxRound(radius);

    double sa, ea;
    if ( x1 == x2 && y1 == y2 )
    {
        // full circle
        sa = 0;
        ea = 360;
    }
    else
    {
        sa = atan2(-dy1, dx1) * 180 / M_PI;
        ea = atan2(-(double)(y2 - yc), (double)(x2 - xc)) * 180 / M_PI;
    }

    DoDrawEllipticArc(xc - r, yc - r, 2 * r, 2 * r, sa, ea);
}

void wxRetro68DCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( !SelectPen() )
        return;

    const short h = wxMacClampCoord(XToPort(x));
    const short v = wxMacClampCoord(YToPort(y));

    const Rect r = wxMacRectFromWX(m_portClipRect);
    MoveTo(r.left, v);
    LineTo(r.right, v);
    MoveTo(h, r.top);
    LineTo(h, r.bottom);
}

// ---------------------------------------------------------------------------
// text
// ---------------------------------------------------------------------------

void wxRetro68DCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );

    if ( text.empty() )
        return;

    const wxCharBuffer buf = wxMacTextToSystem(text);
    const short len = static_cast<short>(wxMin(buf.length(), size_t(32767)));

    SelectPort();
    SelectFont(m_font);

    FontInfo fi;
    GetFontInfo(&fi);

    PenNormal();

    SetForeColour(m_textForegroundColour.IsOk() ? m_textForegroundColour
                                                : *wxBLACK, m_isMono);

    const short h = wxMacClampCoord(XToPort(x));
    const short v = wxMacClampCoord(YToPort(y));

    const short width = TextWidth(const_cast<char*>(buf.data()), 0, len);

    if ( m_backgroundMode == wxBRUSHSTYLE_SOLID )
    {
        SetBackColour(m_textBackgroundColour.IsOk() ? m_textBackgroundColour
                                                    : *wxWHITE, m_isMono);

        // Erase the text background explicitly as srcCopy mode only erases
        // the background of the glyphs themselves.
        Rect r = { v, h,
                   short(v + fi.ascent + fi.descent + fi.leading),
                   short(h + width) };
        BackPat(&wxPatternEmpty);
        EraseRect(&r);
    }

    TextMode(srcOr);

    MoveTo(h, short(v + fi.ascent));
    DrawText(const_cast<char*>(buf.data()), 0, len);

    // restore the default background colour as other QuickDraw functions
    // could be using it
    SetBackColour(*wxWHITE);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + DeviceToLogicalXRel(width),
                    y + DeviceToLogicalYRel(fi.ascent + fi.descent + fi.leading));
}

void wxRetro68DCImpl::DoDrawRotatedText(const wxString& text,
                                        wxCoord x, wxCoord y,
                                        double angle)
{
    // QuickDraw doesn't support rotated text at all, so only handle the
    // trivial case.
    if ( wxIsNullDouble(angle) )
    {
        DoDrawText(text, x, y);
        return;
    }

    wxLogDebug("Rotated text is not supported under classic Mac OS.");
    DoDrawText(text, x, y);
}

// ---------------------------------------------------------------------------
// bitmaps and blitting
// ---------------------------------------------------------------------------

void wxRetro68DCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    DoDrawBitmap(icon, x, y, true);
}

void wxRetro68DCImpl::DoDrawBitmap(const wxBitmap &bmp,
                                   wxCoord x, wxCoord y,
                                   bool useMask)
{
    wxCHECK_RET( IsOk(), wxT("invalid dc") );
    wxCHECK_RET( bmp.IsOk(), wxT("invalid bitmap") );

    const int w = bmp.GetWidth();
    const int h = bmp.GetHeight();

    DrawBitmapPart(bmp,
                   wxRect(0, 0, w, h),
                   wxRect(XToPort(x), YToPort(y),
                          XLenToPort(w), YLenToPort(h)),
                   wxCOPY,
                   useMask);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);
}

void wxRetro68DCImpl::DrawBitmapPart(const wxBitmap& bmp,
                                     const wxRect& rectSrc,
                                     const wxRect& rectDst,
                                     wxRasterOperationMode rop,
                                     bool useMask) const
{
    GWorldPtr gw = bmp.GetGWorld();
    if ( !gw )
        return;

    wxBitmap bmpMask;
    if ( useMask && bmp.GetMask() )
        bmpMask = bmp.GetMask()->GetBitmap();

    const bool hasAlpha = bmp.HasAlpha();

    short mode = GetSourceMode(rop);
    if ( mode == -1 )
        mode = srcCopy;

    SelectPort();

    if ( !bmpMask.IsOk() && !hasAlpha )
    {
        // Simple case: just copy the bits.
        PenNormal();

        // Monochrome bitmaps use the current text colours.
        if ( bmp.GetDepth() == 1 )
        {
            SetForeColour(m_textForegroundColour.IsOk()
                            ? m_textForegroundColour : *wxBLACK, m_isMono);
            SetBackColour(m_textBackgroundColour.IsOk()
                            ? m_textBackgroundColour : *wxWHITE, m_isMono);
        }
        else
        {
            SetCopyBitsColours();
        }

        const Rect src = wxMacRectFromWX(rectSrc);
        const Rect dst = wxMacRectFromWX(rectDst);

        int rowBytes;
        bmp.LockPixels(&rowBytes);
        CopyBits(wxMacGetPortBitMap(reinterpret_cast<GrafPtr>(gw)),
                 wxMacGetPortBitMap(m_port),
                 &src, &dst, mode, nullptr);
        bmp.UnlockPixels();

        SetCopyBitsColours();
        return;
    }

    // We need to composite the bitmap with the existing contents of the
    // destination manually: get them into a temporary buffer first.
    const int w = rectDst.width;
    const int h = rectDst.height;
    if ( w <= 0 || h <= 0 )
        return;

    wxBitmap bmpDst(w, h, 32);
    GWorldPtr gwDst = bmpDst.GetGWorld();
    if ( !gwDst )
        return;

    const Rect rectTmp = { 0, 0, static_cast<short>(h), static_cast<short>(w) };
    const Rect dst = wxMacRectFromWX(rectDst);

    PenNormal();
    SetCopyBitsColours();

    int rowBytesDst;
    unsigned char* const pixDst = bmpDst.LockPixels(&rowBytesDst);

    {
        // Read the current destination contents. Note that the clipping
        // region of the GWorld is wide open, so we can just copy into it.
        CopyBits(wxMacGetPortBitMap(m_port),
                 wxMacGetPortBitMap(reinterpret_cast<GrafPtr>(gwDst)),
                 &dst, &rectTmp, srcCopy, nullptr);
    }

    // Possibly scale the source bitmap to the destination size.
    wxBitmap bmpSrc(bmp);
    wxRect rSrc(rectSrc);
    if ( rectSrc.GetSize() != rectDst.GetSize() )
    {
#if wxUSE_IMAGE
        wxImage img = bmp.ConvertToImage().GetSubImage(rectSrc);
        img.Rescale(w, h);
        bmpSrc = wxBitmap(img);
        rSrc = wxRect(0, 0, w, h);
        if ( bmpMask.IsOk() )
            bmpMask = wxBitmap(); // the mask is now in the image alpha/mask
        if ( bmpSrc.GetMask() )
            bmpMask = bmpSrc.GetMask()->GetBitmap();
#else
        return;
#endif
    }

    int rowBytesSrc;
    const unsigned char* const pixSrc = bmpSrc.LockPixels(&rowBytesSrc);

    int rowBytesMask = 0;
    const unsigned char* pixMask = nullptr;
    if ( bmpMask.IsOk() )
        pixMask = bmpMask.LockPixels(&rowBytesMask);

    const bool srcHasAlpha = bmpSrc.HasAlpha();

    for ( int y = 0; y < h; y++ )
    {
        const unsigned char* s = pixSrc + (rSrc.y + y) * rowBytesSrc + rSrc.x * 4;
        const unsigned char* m = pixMask
            ? pixMask + (rSrc.y + y) * rowBytesMask + rSrc.x * 4
            : nullptr;
        unsigned char* d = pixDst + y * rowBytesDst;

        for ( int x = 0; x < w; x++, s += 4, d += 4 )
        {
            // Pixels are stored as ARGB, with the alpha in the first byte.
            unsigned alpha = srcHasAlpha ? s[0] : 255;
            if ( m )
            {
                // Mask pixels are white for the visible pixels.
                if ( !m[1] )
                    alpha = 0;
                m += 4;
            }

            if ( alpha == 255 )
            {
                d[1] = s[1];
                d[2] = s[2];
                d[3] = s[3];
            }
            else if ( alpha )
            {
                const unsigned inv = 255 - alpha;
                d[1] = static_cast<unsigned char>((s[1] * alpha + d[1] * inv) / 255);
                d[2] = static_cast<unsigned char>((s[2] * alpha + d[2] * inv) / 255);
                d[3] = static_cast<unsigned char>((s[3] * alpha + d[3] * inv) / 255);
            }
        }
    }

    if ( pixMask )
        bmpMask.UnlockPixels();
    bmpSrc.UnlockPixels();

    // And copy the result back.
    SelectPort();
    PenNormal();
    SetCopyBitsColours();

    CopyBits(wxMacGetPortBitMap(reinterpret_cast<GrafPtr>(gwDst)),
             wxMacGetPortBitMap(m_port),
             &rectTmp, &dst, srcCopy, nullptr);

    bmpDst.UnlockPixels();
}

bool wxRetro68DCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                             wxCoord width, wxCoord height,
                             wxDC *source, wxCoord xsrc, wxCoord ysrc,
                             wxRasterOperationMode rop, bool useMask,
                             wxCoord xsrcMask, wxCoord ysrcMask)
{
    return DoStretchBlit(xdest, ydest, width, height,
                         source, xsrc, ysrc, width, height,
                         rop, useMask, xsrcMask, ysrcMask);
}

bool wxRetro68DCImpl::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                                    wxCoord dstWidth, wxCoord dstHeight,
                                    wxDC *source,
                                    wxCoord xsrc, wxCoord ysrc,
                                    wxCoord srcWidth, wxCoord srcHeight,
                                    wxRasterOperationMode rop,
                                    bool useMask,
                                    wxCoord WXUNUSED(xsrcMask),
                                    wxCoord WXUNUSED(ysrcMask))
{
    wxCHECK_MSG( IsOk(), false, "invalid dc" );
    wxCHECK_MSG( source, false, "invalid source dc" );

    wxRetro68DCImpl* const src = wxDynamicCast(source->GetImpl(), wxRetro68DCImpl);
    wxCHECK_MSG( src && src->IsOk(), false, "unsupported source DC" );

    if ( rop == wxNO_OP )
        return true;

    const wxRect rectDst(XToPort(xdest), YToPort(ydest),
                         XLenToPort(dstWidth), YLenToPort(dstHeight));

    if ( rop == wxCLEAR || rop == wxSET || rop == wxINVERT )
    {
        SelectPort();
        PenNormal();

        const Rect dst = wxMacRectFromWX(rectDst);
        if ( rop == wxINVERT )
        {
            InvertRect(&dst);
        }
        else
        {
            SetForeColour(rop == wxSET ? *wxBLACK : *wxWHITE, m_isMono);
            PaintRect(&dst);
        }

        CalcBoundingBoxFromPort(dst);
        return true;
    }

    const wxRect rectSrc(src->XToPort(xsrc), src->YToPort(ysrc),
                         src->XLenToPort(srcWidth), src->YLenToPort(srcHeight));

    // If the source is a memory DC, use its bitmap directly as this allows
    // to handle the mask and alpha.
    wxMemoryDCImpl* const memSrc = wxDynamicCast(src, wxMemoryDCImpl);
    if ( memSrc && memSrc->GetSelectedBitmap().IsOk() &&
            ((useMask && memSrc->GetSelectedBitmap().GetMask()) ||
                memSrc->GetSelectedBitmap().HasAlpha()) )
    {
        DrawBitmapPart(memSrc->GetSelectedBitmap(), rectSrc, rectDst, rop, useMask);
    }
    else
    {
        short mode = GetSourceMode(rop);
        if ( mode == -1 )
        {
            wxLogDebug("Unsupported raster operation %d.", rop);
            mode = srcCopy;
        }

        SelectPort();
        PenNormal();
        SetCopyBitsColours();

        const Rect s = wxMacRectFromWX(rectSrc);
        const Rect d = wxMacRectFromWX(rectDst);

        // Lock the source pixels if it's an offscreen GWorld.
        if ( memSrc && memSrc->GetSelectedBitmap().IsOk() )
        {
            int rowBytes;
            memSrc->GetSelectedBitmap().LockPixels(&rowBytes);
        }

        CopyBits(wxMacGetPortBitMap(src->GetPort()),
                 wxMacGetPortBitMap(m_port),
                 &s, &d, mode, nullptr);

        if ( memSrc && memSrc->GetSelectedBitmap().IsOk() )
            memSrc->GetSelectedBitmap().UnlockPixels();
    }

    const Rect d = wxMacRectFromWX(rectDst);
    const_cast<wxRetro68DCImpl*>(this)->CalcBoundingBoxFromPort(d);

    return true;
}
