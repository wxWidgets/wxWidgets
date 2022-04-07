/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dc.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __WXGTK3__

#include "wx/window.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcscreen.h"
#include "wx/display.h"
#include "wx/gdicmn.h"
#include "wx/icon.h"
#include "wx/gtk/dc.h"

#include "wx/gtk/private/wrapgtk.h"

wxGTKCairoDCImpl::wxGTKCairoDCImpl(wxDC* owner)
    : wxGCDCImpl(owner)
{
    m_layoutDir = wxLayout_Default;
}

wxGTKCairoDCImpl::wxGTKCairoDCImpl(wxDC* owner, wxWindow* window, wxLayoutDirection dir, int width)
    : wxGCDCImpl(owner, 0)
    , m_size(width, 0)
{
    m_layoutDir = dir;
    if ( window )
    {
        m_window = window;
        m_font = window->GetFont();
        m_textForegroundColour = window->GetForegroundColour();
        m_textBackgroundColour = window->GetBackgroundColour();
        m_contentScaleFactor = window->GetContentScaleFactor();
    }
}

void wxGTKCairoDCImpl::InitSize(GdkWindow* window)
{
    m_size.x = gdk_window_get_width(window);
    m_size.y = gdk_window_get_height(window);
}

void wxGTKCairoDCImpl::DoDrawBitmap(const wxBitmap& bitmap, int x, int y, bool useMask)
{
    wxCHECK_RET(IsOk(), "invalid DC");

    cairo_t* cr = NULL;
    if (m_graphicContext)
        cr = static_cast<cairo_t*>(m_graphicContext->GetNativeContext());
    if (cr)
    {
        cairo_save(cr);
        if (m_layoutDir == wxLayout_RightToLeft)
        {
            // bitmap is not mirrored
            cairo_scale(cr, -1, 1);
            x = -x - bitmap.GetWidth();
        }
        bitmap.Draw(cr, x, y, useMask, &m_textForegroundColour, &m_textBackgroundColour);
        cairo_restore(cr);
    }
}

void wxGTKCairoDCImpl::DoDrawIcon(const wxIcon& icon, int x, int y)
{
    DoDrawBitmap(icon, x, y, true);
}

#if wxUSE_IMAGE
bool wxDoFloodFill(wxDC* dc, int x, int y, const wxColour& col, wxFloodFillStyle style);

bool wxGTKCairoDCImpl::DoFloodFill(int x, int y, const wxColour& col, wxFloodFillStyle style)
{
    return wxDoFloodFill(GetOwner(), x, y, col, style);
}
#endif

void wxGTKCairoDCImpl::DoDrawText(const wxString& text, int x, int y)
{
    wxCHECK_RET(IsOk(), "invalid DC");

    if (text.empty())
        return;

    const bool xInverted = m_signX < 0 || m_layoutDir == wxLayout_RightToLeft;
    if (xInverted && text.find('\n') != wxString::npos)
    {
        // RTL needs each line separately to position text properly.
        // DrawLabel() will split the text and call back for each line.
        GetOwner()->DrawLabel(text, wxRect(x, y, 0, 0));
        return;
    }

    int w, h;
    DoGetTextExtent(text, &w, &h);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);

    const bool yInverted = m_signY < 0;
    if (xInverted || yInverted)
        m_graphicContext->PushState();
    if (xInverted)
    {
        // text is not mirrored
        m_graphicContext->Scale(-1, 1);
        x = -x - w;
    }
    if (yInverted)
    {
        m_graphicContext->Scale(1, -1);
        y = -y - h;
    }

    wxCompositionMode curMode = m_graphicContext->GetCompositionMode();
    m_graphicContext->SetCompositionMode(wxCOMPOSITION_OVER);

    if (m_backgroundMode == wxBRUSHSTYLE_TRANSPARENT)
        m_graphicContext->DrawText(text, x, y);
    else
        m_graphicContext->DrawText(text, x, y, m_graphicContext->CreateBrush(m_textBackgroundColour));

    m_graphicContext->SetCompositionMode(curMode);

    if (xInverted || yInverted)
        m_graphicContext->PopState();
}

void wxGTKCairoDCImpl::DoDrawRotatedText(const wxString& text, int x, int y, double angle)
{
    wxCHECK_RET(IsOk(), "invalid DC");

    // save current bounding box
    // rotation will cause DoDrawText() to update it incorrectly
    const bool isBBoxValid = m_isBBoxValid;
    const int minX = m_minX;
    const int minY = m_minY;
    const int maxX = m_maxX;
    const int maxY = m_maxY;

    const double rad = wxDegToRad(-angle);
    m_graphicContext->PushState();
    m_graphicContext->Translate(x, y);
    m_graphicContext->Rotate(rad);
    DoDrawText(text, 0, 0);
    m_graphicContext->PopState();

    // restore bounding box and update it correctly
    m_isBBoxValid = isBBoxValid;
    m_minX = minX;
    m_minY = minY;
    m_maxX = maxX;
    m_maxY = maxY;

    CalcBoundingBox(x, y);
    int w, h;
    DoGetTextExtent(text, &w, &h);
    cairo_matrix_t m;
    cairo_matrix_init_translate(&m, x, y);
    cairo_matrix_rotate(&m, rad);
    double xx = w, yy = 0;
    cairo_matrix_transform_point(&m, &xx, &yy);
    CalcBoundingBox(int(xx), int(yy));
    xx = w; yy = h;
    cairo_matrix_transform_point(&m, &xx, &yy);
    CalcBoundingBox(int(xx), int(yy));
    xx = 0; yy = h;
    cairo_matrix_transform_point(&m, &xx, &yy);
    CalcBoundingBox(int(xx), int(yy));
}

void wxGTKCairoDCImpl::DoDrawCheckMark(int x, int y, int width, int height)
{
    if (m_layoutDir == wxLayout_RightToLeft)
    {
        wxCHECK_RET(IsOk(), "invalid DC");

        // checkmark is not mirrored
        m_graphicContext->PushState();
        m_graphicContext->Scale(-1, 1);
        BaseType::DoDrawCheckMark(-x - width, y, width, height);
        m_graphicContext->PopState();
    }
    else
        BaseType::DoDrawCheckMark(x, y, width, height);
}

wxBitmap wxGTKCairoDCImpl::DoGetAsBitmap(const wxRect* /*subrect*/) const
{
    wxFAIL_MSG("DoGetAsBitmap not implemented");
    return wxBitmap();
}

bool wxGTKCairoDCImpl::DoGetPixel(int x, int y, wxColour* col) const
{
    if (col)
    {
        cairo_t* cr = NULL;
        if (m_graphicContext)
            cr = static_cast<cairo_t*>(m_graphicContext->GetNativeContext());
        if (cr)
        {
            cairo_surface_t* surface = cairo_get_target(cr);
            x = LogicalToDeviceX(x);
            y = LogicalToDeviceY(y);
            GdkPixbuf* pixbuf = gdk_pixbuf_get_from_surface(surface, x, y, 1, 1);
            if (pixbuf)
            {
                const guchar* src = gdk_pixbuf_get_pixels(pixbuf);
                col->Set(src[0], src[1], src[2]);
                g_object_unref(pixbuf);
                return true;
            }
            *col = wxColour();
        }
    }
    return false;
}

void wxGTKCairoDCImpl::DoGetSize(int* width, int* height) const
{
    if (width)
        *width = m_size.x;
    if (height)
        *height = m_size.y;
}

bool wxGTKCairoDCImpl::DoStretchBlit(int xdest, int ydest, int dstWidth, int dstHeight, wxDC* source, int xsrc, int ysrc, int srcWidth, int srcHeight, wxRasterOperationMode rop, bool useMask, int xsrcMask, int ysrcMask)
{
    wxCHECK_MSG(IsOk(), false, "invalid DC");
    wxCHECK_MSG(source && source->IsOk(), false, "invalid source DC");

    cairo_t* cr = NULL;
    if (m_graphicContext)
        cr = static_cast<cairo_t*>(m_graphicContext->GetNativeContext());
    cairo_t* cr_src = NULL;
    wxGraphicsContext* gc_src = source->GetGraphicsContext();
    if (gc_src)
        cr_src = static_cast<cairo_t*>(gc_src->GetNativeContext());

    if (cr == NULL || cr_src == NULL)
        return false;

    const int xsrc_dev = source->LogicalToDeviceX(xsrc);
    const int ysrc_dev = source->LogicalToDeviceY(ysrc);

    cairo_surface_t* surfaceSrc = cairo_get_target(cr_src);
    cairo_surface_flush(surfaceSrc);

    cairo_surface_t* surfaceTmp = NULL;
    // If destination (this) and source wxDC refer to the same Cairo context
    // it means that we operate on one surface and results of drawing
    // can be invalid if destination and source regions overlap.
    // In such situation we have to copy source surface to the temporary
    // surface and use this copy in the drawing operations.
    if ( cr == cr_src )
    {
        // Check if destination and source regions overlap.
        // If necessary, copy source surface to the temporary one.
        if (wxRect(xdest, ydest, dstWidth, dstHeight)
            .Intersects(wxRect(xsrc, ysrc, srcWidth, srcHeight)))
        {
            const int w = cairo_image_surface_get_width(surfaceSrc);
            const int h = cairo_image_surface_get_height(surfaceSrc);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
            if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 12, 0) )
            {
                surfaceTmp = cairo_surface_create_similar_image(surfaceSrc,
                     cairo_image_surface_get_format(surfaceSrc),
                     w, h);
            }
            else
#endif // Cairo 1.12
            {
                surfaceTmp = cairo_surface_create_similar(surfaceSrc,
                     CAIRO_CONTENT_COLOR_ALPHA,
                     w, h);
            }
            cairo_t* crTmp = cairo_create(surfaceTmp);
            cairo_set_source_surface(crTmp, surfaceSrc, 0, 0);
            cairo_rectangle(crTmp, 0.0, 0.0, w, h);
            cairo_set_operator(crTmp, CAIRO_OPERATOR_SOURCE);
            cairo_fill(crTmp);
            cairo_destroy(crTmp);
            cairo_surface_flush(surfaceTmp);
            surfaceSrc = surfaceTmp;
        }
    }
    cairo_save(cr);
    if (m_layoutDir == wxLayout_RightToLeft)
    {
        // blit is not mirrored
        cairo_scale(cr, -1, 1);
        xdest = -xdest - dstWidth;
    }
    cairo_translate(cr, xdest, ydest);
    cairo_rectangle(cr, 0, 0, dstWidth, dstHeight);
    double sx, sy;
    source->GetUserScale(&sx, &sy);

    const wxBitmap& bitmap = source->GetImpl()->GetSelectedBitmap();
    const double bmpScale = bitmap.IsOk() ? bitmap.GetScaleFactor() : 1.0;

    cairo_scale(cr, dstWidth / (sx * srcWidth * bmpScale), dstHeight / (sy * srcHeight * bmpScale));
    cairo_set_source_surface(cr, surfaceSrc, -xsrc_dev, -ysrc_dev);
    const wxRasterOperationMode rop_save = m_logicalFunction;
    SetLogicalFunction(rop);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_surface_t* maskSurf = NULL;
    if (useMask)
    {
        if (bitmap.IsOk())
        {
            wxMask* mask = bitmap.GetMask();
            if (mask)
                maskSurf = *mask;
        }
    }
    if (maskSurf)
    {
        int xsrcMask_dev = xsrc_dev;
        int ysrcMask_dev = ysrc_dev;
        if (xsrcMask != -1)
            xsrcMask_dev = source->LogicalToDeviceX(xsrcMask);
        if (ysrcMask != -1)
            ysrcMask_dev = source->LogicalToDeviceY(ysrcMask);
        cairo_clip(cr);
        cairo_mask_surface(cr, maskSurf, -xsrcMask_dev, -ysrcMask_dev);
    }
    else
    {
        cairo_fill(cr);
    }
    cairo_restore(cr);
    if ( surfaceTmp )
    {
        cairo_surface_destroy(surfaceTmp);
    }
    m_logicalFunction = rop_save;
    return true;
}

void* wxGTKCairoDCImpl::GetCairoContext() const
{
    cairo_t* cr = NULL;
    if (m_graphicContext)
        cr = static_cast<cairo_t*>(m_graphicContext->GetNativeContext());
    return cr;
}

wxSize wxGTKCairoDCImpl::GetPPI() const
{
    if ( m_window )
    {
        return wxDisplay(m_window).GetPPI();
    }

    // For a non-window-based DC the concept of PPI doesn't make much sense
    // anyhow, so just return the hardcoded value used by the base class.
    return wxGCDCImpl::GetPPI();
}

void wxGTKCairoDCImpl::SetLayoutDirection(wxLayoutDirection dir)
{
    if (dir == wxLayout_Default && m_window)
        dir = m_window->GetLayoutDirection();

    m_layoutDir = dir;
}

wxLayoutDirection wxGTKCairoDCImpl::GetLayoutDirection() const
{
    // LTR unless explicitly RTL
    return
        m_layoutDir == wxLayout_RightToLeft
            ? wxLayout_RightToLeft
            : wxLayout_LeftToRight;
}

void wxGTKCairoDCImpl::AdjustForRTL(cairo_t* cr)
{
    if (m_layoutDir == wxLayout_RightToLeft)
    {
        cairo_translate(cr, m_size.x, 0);
        cairo_scale(cr, -1, 1);
    }
}
//-----------------------------------------------------------------------------

wxWindowDCImpl::wxWindowDCImpl(wxWindowDC* owner, wxWindow* window)
    : wxGTKCairoDCImpl(owner, window)
{
    GtkWidget* widget = window->m_wxwindow;
    if (widget == NULL)
        widget = window->m_widget;
    GdkWindow* gdkWindow = NULL;
    if (widget)
    {
        gdkWindow = gtk_widget_get_window(widget);
        m_ok = true;
    }
    if (gdkWindow)
    {
        cairo_t* cr = gdk_cairo_create(gdkWindow);
        SetLayoutDirection(wxLayout_Default);
        AdjustForRTL(cr);
        wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(cr);
        cairo_destroy(cr);
        gc->SetContentScaleFactor(m_contentScaleFactor);
        SetGraphicsContext(gc);
        GtkAllocation a;
        gtk_widget_get_allocation(widget, &a);
        int x, y;
        if (gtk_widget_get_has_window(widget))
        {
            m_size.x = gdk_window_get_width(gdkWindow);
            m_size.y = gdk_window_get_height(gdkWindow);
            x = m_size.x - a.width;
            y = m_size.y - a.height;
        }
        else
        {
            m_size.x = a.width;
            m_size.y = a.height;
            x = a.x;
            y = a.y;
            cairo_rectangle(cr, a.x, a.y, a.width, a.height);
            cairo_clip(cr);
        }
        if (x || y)
            SetDeviceLocalOrigin(x, y);
    }
    else
        SetGraphicsContext(wxGraphicsContext::Create());
}
//-----------------------------------------------------------------------------

wxClientDCImpl::wxClientDCImpl(wxClientDC* owner, wxWindow* window)
    : wxGTKCairoDCImpl(owner, window)
{
    GtkWidget* widget = window->m_wxwindow;
    if (widget == NULL)
        widget = window->m_widget;
    GdkWindow* gdkWindow = NULL;
    if (widget)
    {
        window->GetClientSize(&m_size.x, &m_size.y);
        gdkWindow = gtk_widget_get_window(widget);
        m_ok = true;
    }
    if (gdkWindow)
    {
        cairo_t* cr = gdk_cairo_create(gdkWindow);
        SetLayoutDirection(wxLayout_Default);
        AdjustForRTL(cr);
        wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(cr);
        cairo_destroy(cr);
        gc->SetContentScaleFactor(m_contentScaleFactor);
        SetGraphicsContext(gc);
        if (!gtk_widget_get_has_window(widget))
        {
            GtkAllocation a;
            gtk_widget_get_allocation(widget, &a);
            cairo_rectangle(cr, a.x, a.y, a.width, a.height);
            cairo_clip(cr);
            SetDeviceLocalOrigin(a.x, a.y);
        }
    }
    else
        SetGraphicsContext(wxGraphicsContext::Create());
}
//-----------------------------------------------------------------------------

wxPaintDCImpl::wxPaintDCImpl(wxPaintDC* owner, wxWindow* window)
    : wxGTKCairoDCImpl(owner, window)
    , m_clip(window->m_nativeUpdateRegion)
{
    cairo_t* cr = window->GTKPaintContext();
    wxCHECK_RET(cr, "using wxPaintDC without being in a native paint event");
    InitSize(gtk_widget_get_window(window->m_wxwindow));
    wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(cr);
    gc->SetContentScaleFactor(m_contentScaleFactor);
    SetGraphicsContext(gc);
    // context is already adjusted for RTL
    m_layoutDir = window->GetLayoutDirection();
}

void wxPaintDCImpl::DestroyClippingRegion()
{
    BaseType::DestroyClippingRegion();

    // re-establish clip for paint update area
    int x, y, w, h;
    m_clip.GetBox(x, y, w, h);
    cairo_t* cr = static_cast<cairo_t*>(GetCairoContext());
    cairo_rectangle(cr,
        DeviceToLogicalX(x), DeviceToLogicalY(y),
        DeviceToLogicalXRel(w), DeviceToLogicalYRel(h));
    cairo_clip(cr);
}
//-----------------------------------------------------------------------------

wxScreenDCImpl::wxScreenDCImpl(wxScreenDC* owner)
    : wxGTKCairoDCImpl(owner, static_cast<wxWindow*>(NULL))
{
    GdkWindow* window = gdk_get_default_root_window();
    InitSize(window);

    cairo_t* cr = gdk_cairo_create(window);
    wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(cr);
    cairo_destroy(cr);
    gc->SetContentScaleFactor(m_contentScaleFactor);
    SetGraphicsContext(gc);
}

wxSize wxScreenDCImpl::GetPPI() const
{
    return wxGetDisplayPPI();
}

//-----------------------------------------------------------------------------

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC* owner)
    : wxGTKCairoDCImpl(owner)
{
    m_ok = false;
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC* owner, wxBitmap& bitmap)
    : wxGTKCairoDCImpl(owner, static_cast<wxWindow*>(NULL))
    , m_bitmap(bitmap)
{
    Setup();
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC* owner, wxDC*)
    : wxGTKCairoDCImpl(owner)
{
    m_ok = false;
}

wxBitmap wxMemoryDCImpl::DoGetAsBitmap(const wxRect* subrect) const
{
    return subrect ? m_bitmap.GetSubBitmap(*subrect) : m_bitmap;
}

void wxMemoryDCImpl::DoSelect(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    Setup();
}

const wxBitmap& wxMemoryDCImpl::GetSelectedBitmap() const
{
    return m_bitmap;
}

wxBitmap& wxMemoryDCImpl::GetSelectedBitmap()
{
    return m_bitmap;
}

void wxMemoryDCImpl::Setup()
{
    wxGraphicsContext* gc = NULL;
    m_ok = m_bitmap.IsOk();
    if (m_ok)
    {
        m_size = m_bitmap.GetLogicalSize();
        m_contentScaleFactor = m_bitmap.GetScaleFactor();
        cairo_t* cr = m_bitmap.CairoCreate();
        AdjustForRTL(cr);
        gc = wxGraphicsContext::CreateFromNative(cr);
        cairo_destroy(cr);
        gc->SetContentScaleFactor(m_contentScaleFactor);
    }
    SetGraphicsContext(gc);
}
//-----------------------------------------------------------------------------

wxGTKCairoDC::wxGTKCairoDC(cairo_t* cr, wxWindow* window, wxLayoutDirection dir, int width)
    : wxDC(new wxGTKCairoDCImpl(this, window, dir, width))
{
    wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(cr);
    gc->SetContentScaleFactor(window->GetContentScaleFactor());
    SetGraphicsContext(gc);
    if (dir == wxLayout_Default)
        SetLayoutDirection(window->GetLayoutDirection());
    // else context is already adjusted for RTL
}

#else

#include "wx/gtk/dc.h"

//-----------------------------------------------------------------------------
// wxGTKDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxGTKDCImpl, wxDCImpl);

wxGTKDCImpl::wxGTKDCImpl( wxDC *owner )
   : wxDCImpl( owner )
{
    m_ok = FALSE;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;
}

wxGTKDCImpl::~wxGTKDCImpl()
{
}

void wxGTKDCImpl::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxASSERT_MSG( width >= 0 && height >= 0,
                  "Clipping box size values cannot be negative" );

    wxRect newRegion(x, y, width, height);

    wxRect clipRegion;
    if ( m_clipping )
    {
        // New clipping box is an intersection
        // of required clipping box and the current one.
        wxRect curRegion(m_clipX1, m_clipY1, m_clipX2 - m_clipX1, m_clipY2 - m_clipY1);
        clipRegion = curRegion.Intersect(newRegion);
    }
    else
    {
        // Effective clipping box is an intersection
        // of required clipping box and DC surface.
        int dcWidth, dcHeight;
        DoGetSize(&dcWidth, &dcHeight);
        wxRect dcRect(DeviceToLogicalX(0), DeviceToLogicalY(0),
                      DeviceToLogicalXRel(dcWidth), DeviceToLogicalYRel(dcHeight));
        clipRegion = dcRect.Intersect(newRegion);

        m_clipping = true;
    }

    if ( clipRegion.IsEmpty() )
    {
        m_clipX1 = m_clipY1 = m_clipX2 = m_clipY2 = 0;
    }
    else
    {
        m_clipX1 = clipRegion.GetLeftTop().x;
        m_clipY1 = clipRegion.GetLeftTop().y;
        m_clipX2 = clipRegion.GetBottomRight().x + 1;
        m_clipY2 = clipRegion.GetBottomRight().y + 1;
    }
}

// ---------------------------------------------------------------------------
// get DC capabilities
// ---------------------------------------------------------------------------

void wxGTKDCImpl::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0;
    int h = 0;
    GetOwner()->GetSize( &w, &h );
    if (width) *width = int( double(w) / (m_userScaleX*GetMMToPXx()) );
    if (height) *height = int( double(h) / (m_userScaleY*GetMMToPXy()) );
}

// Resolution in pixels per logical inch
wxSize wxGTKDCImpl::GetPPI() const
{
    // TODO (should probably be pure virtual)
    return wxSize(0, 0);
}
#endif
