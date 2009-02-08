/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/statusbr.cpp
// Purpose:     wxStatusBarGeneric class implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#ifdef __WXGTK20__
    #include <gtk/gtk.h>
#endif

// we only have to do it here when we use wxStatusBarGeneric in addition to the
// standard wxStatusBar class, if wxStatusBarGeneric is the same as
// wxStatusBar, then the corresponding IMPLEMENT_DYNAMIC_CLASS is already in
// common/statbar.cpp
#if defined(__WXMAC__) || \
    (defined(wxUSE_NATIVE_STATUSBAR) && wxUSE_NATIVE_STATUSBAR)
    #include "wx/generic/statusbr.h"

    IMPLEMENT_DYNAMIC_CLASS(wxStatusBarGeneric, wxWindow)
#endif // wxUSE_NATIVE_STATUSBAR

// Default status border dimensions
#define wxTHICK_LINE_BORDER 2

// Margin between the field text and the field rect
#define wxFIELD_TEXT_MARGIN 2


// ----------------------------------------------------------------------------
// wxStatusBarGeneric
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxStatusBarGeneric, wxWindow)
    EVT_PAINT(wxStatusBarGeneric::OnPaint)
    EVT_SIZE(wxStatusBarGeneric::OnSize)
    EVT_LEFT_DOWN(wxStatusBarGeneric::OnLeftDown)
    EVT_RIGHT_DOWN(wxStatusBarGeneric::OnRightDown)
    EVT_SYS_COLOUR_CHANGED(wxStatusBarGeneric::OnSysColourChanged)
END_EVENT_TABLE()

void wxStatusBarGeneric::Init()
{
    m_borderX = wxTHICK_LINE_BORDER;
    m_borderY = wxTHICK_LINE_BORDER;
}

wxStatusBarGeneric::~wxStatusBarGeneric()
{
}

bool wxStatusBarGeneric::Create(wxWindow *parent,
                                wxWindowID id,
                                long style,
                                const wxString& name)
{
    style |= wxTAB_TRAVERSAL | wxFULL_REPAINT_ON_RESIZE;
    if ( !wxWindow::Create(parent, id,
                           wxDefaultPosition, wxDefaultSize,
                           style, name) )
        return false;

    // The status bar should have a themed background
    SetThemeEnabled( true );

    InitColours();

#ifdef __WXPM__
    SetFont(*wxSMALL_FONT);
#endif

    int height = (int)((11*GetCharHeight())/10 + 2*GetBorderY());
    SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, height);

    SetFieldsCount(1);

    return true;
}

wxSize wxStatusBarGeneric::DoGetBestSize() const
{
    int width, height;

    // best width is the width of the parent
    if (GetParent())
        GetParent()->GetClientSize(&width, NULL);
    else
        width = 80;     // a dummy value

    // best height is as calculated above in Create()
    height = (int)((11*GetCharHeight())/10 + 2*GetBorderY());

    return wxSize(width, height);
}

void wxStatusBarGeneric::SetFieldsCount(int number, const int *widths)
{
    wxASSERT_MSG( number >= 0, _T("negative number of fields in wxStatusBar?") );

    // enlarge the m_statusStrings array if needed:
    for (size_t i = m_panes.GetCount(); i < (size_t)number; ++i)
        m_statusStrings.Add( wxEmptyString );

    // shrink the m_statusStrings array if needed:
    for (int j = (int)m_panes.GetCount() - 1; j >= number; --j)
        m_statusStrings.RemoveAt(j);

    // forget the old cached pixel widths
    m_widthsAbs.Empty();

    wxStatusBarBase::SetFieldsCount(number, widths);

    wxASSERT_MSG( m_panes.GetCount() == m_statusStrings.GetCount(),
                  _T("This really should never happen, can we do away with m_panes.GetCount() here?") );
}

void wxStatusBarGeneric::SetStatusText(const wxString& text, int number)
{
    wxCHECK_RET( (number >= 0) && ((size_t)number < m_panes.GetCount()),
                 _T("invalid status bar field index") );

    wxString oldText = m_statusStrings[number];
    if (oldText != text)
    {
        m_statusStrings[number] = text;

        wxRect rect;
        GetFieldRect(number, rect);

        Refresh(true, &rect);

        // it's common to show some text in the status bar before starting a
        // relatively lengthy operation, ensure that the text is shown to the
        // user immediately and not after the lengthy operation end
        Update();
    }
}

wxString wxStatusBarGeneric::GetStatusText(int n) const
{
    wxCHECK_MSG( (n >= 0) && ((size_t)n < m_panes.GetCount()), wxEmptyString,
                 _T("invalid status bar field index") );

    return m_statusStrings[n];
}

void wxStatusBarGeneric::SetStatusWidths(int n, const int widths_field[])
{
    // only set status widths when n == number of statuswindows
    wxCHECK_RET( (size_t)n == m_panes.GetCount(), _T("status bar field count mismatch") );

    // forget the old cached pixel widths
    m_widthsAbs.Empty();

    wxStatusBarBase::SetStatusWidths(n, widths_field);
}

bool wxStatusBarGeneric::ShowsSizeGrip() const
{
    if ( !HasFlag(wxST_SIZEGRIP) )
        return false;

    wxTopLevelWindow * const
        tlw = wxDynamicCast(wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
    return tlw && !tlw->IsMaximized() && tlw->HasFlag(wxRESIZE_BORDER);
}

void wxStatusBarGeneric::DrawFieldText(wxDC& dc, const wxRect& rect, int i, int textHeight)
{
    wxString text(GetStatusText(i));
    if (text.empty())
        return;     // optimization

    int xpos = rect.x + wxFIELD_TEXT_MARGIN,
        maxWidth = rect.width - 2*wxFIELD_TEXT_MARGIN,
        ypos = (int) (((rect.height - textHeight) / 2) + rect.y + 0.5);

    if (ShowsSizeGrip())
    {
        // don't write text over the size grip:
        // NOTE: overloading DoGetClientSize() and GetClientAreaOrigin() wouldn't
        //       work because the adjustment needs to be done only when drawing
        //       the field text and not also when drawing the background, the
        //       size grip itself, etc
        if ((GetLayoutDirection() == wxLayout_RightToLeft && i == 0) ||
            (GetLayoutDirection() != wxLayout_RightToLeft &&
                 i == (int)m_panes.GetCount()-1))
        {
            const wxRect& gripRc = GetSizeGripRect();

            // NOTE: we don't need any special treatment wrt to the layout direction
            //       since DrawText() will automatically adjust the origin of the
            //       text accordingly to the layout in use

            maxWidth -= gripRc.width;
        }
    }

    // eventually ellipsize the text so that it fits the field width
    text = wxControl::Ellipsize(
        text, dc,
        GetLayoutDirection() == wxLayout_RightToLeft ? wxELLIPSIZE_START : wxELLIPSIZE_END,
        maxWidth,
        wxELLIPSIZE_EXPAND_TAB);
        // Ellipsize() will do something only if necessary

#if defined( __WXGTK__ ) || defined(__WXMAC__)
    xpos++;
    ypos++;
#endif

    // draw the text
    dc.DrawText(text, xpos, ypos);
}

void wxStatusBarGeneric::DrawField(wxDC& dc, int i, int textHeight)
{
    wxRect rect;
    GetFieldRect(i, rect);

    if (rect.GetWidth() <= 0)
        return;     // happens when the status bar is shrinked in a very small area!

    int style = m_panes[i].nStyle;
    if (style != wxSB_FLAT)
    {
        // Draw border
        // For wxSB_NORMAL:
        // Have grey background, plus 3-d border -
        // One black rectangle.
        // Inside this, left and top sides - dark grey. Bottom and right -
        // white.
        // Reverse it for wxSB_RAISED

        dc.SetPen((style == wxSB_RAISED) ? m_mediumShadowPen : m_hilightPen);

#ifndef __WXPM__

        // Right and bottom lines
        dc.DrawLine(rect.x + rect.width, rect.y,
                    rect.x + rect.width, rect.y + rect.height);
        dc.DrawLine(rect.x + rect.width, rect.y + rect.height,
                    rect.x, rect.y + rect.height);

        dc.SetPen((style == wxSB_RAISED) ? m_hilightPen : m_mediumShadowPen);

        // Left and top lines
        dc.DrawLine(rect.x, rect.y + rect.height,
               rect.x, rect.y);
        dc.DrawLine(rect.x, rect.y,
            rect.x + rect.width, rect.y);
#else

        dc.DrawLine(rect.x + rect.width, rect.height + 2,
                    rect.x, rect.height + 2);
        dc.DrawLine(rect.x + rect.width, rect.y,
                    rect.x + rect.width, rect.y + rect.height);

        dc.SetPen((style == wxSB_RAISED) ? m_hilightPen : m_mediumShadowPen);
        dc.DrawLine(rect.x, rect.y,
                    rect.x + rect.width, rect.y);
        dc.DrawLine(rect.x, rect.y + rect.height,
                   rect.x, rect.y);
#endif
    }

    DrawFieldText(dc, rect, i, textHeight);
}

// Get the position and size of the field's internal bounding rectangle
bool wxStatusBarGeneric::GetFieldRect(int n, wxRect& rect) const
{
    wxCHECK_MSG( (n >= 0) && ((size_t)n < m_panes.GetCount()), false,
                 _T("invalid status bar field index") );

    if (m_widthsAbs.IsEmpty())
        return false;

    rect.x = 0;
    for ( int i = 0; i < n; i++ )
        rect.x += m_widthsAbs[i];
    rect.x += m_borderX;

    rect.y = m_borderY;
    rect.width = m_widthsAbs[n] - 2*m_borderX;
    rect.height = m_lastClientHeight - 2*m_borderY;

    return true;
}

// Initialize colours
void wxStatusBarGeneric::InitColours()
{
#if defined(__WXPM__)
    m_mediumShadowPen = wxPen(wxColour(127, 127, 127), 1, wxSOLID);
    m_hilightPen = *wxWHITE_PEN;

    SetBackgroundColour(*wxLIGHT_GREY);
    SetForegroundColour(*wxBLACK);
#else // !__WXPM__
    m_mediumShadowPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
    m_hilightPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT));
#endif // __WXPM__/!__WXPM__
}

void wxStatusBarGeneric::SetMinHeight(int height)
{
    // check that this min height is not less than minimal height for the
    // current font (min height is as calculated above in Create() except for border)
    int minHeight = (int)((11*GetCharHeight())/10);

    if ( height > minHeight )
        SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, height + 2*m_borderY);
}

wxRect wxStatusBarGeneric::GetSizeGripRect() const
{
    int width, height;
    wxWindow::DoGetClientSize(&width, &height);

    if (GetLayoutDirection() == wxLayout_RightToLeft)
        return wxRect(2, 2, height-2, height-4);
    else
        return wxRect(width-height-2, 2, height-2, height-4);
}

// ----------------------------------------------------------------------------
// wxStatusBarGeneric - event handlers
// ----------------------------------------------------------------------------

void wxStatusBarGeneric::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

#ifdef __WXGTK20__
    // Draw grip first
    if ( ShowsSizeGrip() )
    {
        const wxRect& rc = GetSizeGripRect();
        GdkWindowEdge edge =
            GetLayoutDirection() == wxLayout_RightToLeft ? GDK_WINDOW_EDGE_SOUTH_WEST :
                                                           GDK_WINDOW_EDGE_SOUTH_EAST;
        gtk_paint_resize_grip( m_widget->style,
                            GTKGetDrawingWindow(),
                            (GtkStateType) GTK_WIDGET_STATE (m_widget),
                            NULL,
                            m_widget,
                            "statusbar",
                            edge,
                            rc.x, rc.y, rc.width, rc.height );
    }
#endif // __WXGTK20__

    if (GetFont().IsOk())
        dc.SetFont(GetFont());

    // compute char height only once for all panes:
    int textHeight = dc.GetCharHeight();

    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    for (size_t i = 0; i < m_panes.GetCount(); i ++)
        DrawField(dc, i, textHeight);
}

// Responds to colour changes, and passes event on to children.
void wxStatusBarGeneric::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    InitColours();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

void wxStatusBarGeneric::OnLeftDown(wxMouseEvent& event)
{
#ifdef __WXGTK20__
    int width, height;
    GetClientSize(&width, &height);

    if ( ShowsSizeGrip()  && (event.GetX() > width-height) )
    {
        GtkWidget *ancestor = gtk_widget_get_toplevel( m_widget );

        if (!GTK_IS_WINDOW (ancestor))
            return;

        GdkWindow *source = GTKGetDrawingWindow();

        int org_x = 0;
        int org_y = 0;
        gdk_window_get_origin( source, &org_x, &org_y );

        if (GetLayoutDirection() == wxLayout_RightToLeft)
        {
            gtk_window_begin_resize_drag (GTK_WINDOW (ancestor),
                                  GDK_WINDOW_EDGE_SOUTH_WEST,
                                  1,
                                  org_x - event.GetX() + GetSize().x ,
                                  org_y + event.GetY(),
                                  0);
        }
        else
        {
            gtk_window_begin_resize_drag (GTK_WINDOW (ancestor),
                                  GDK_WINDOW_EDGE_SOUTH_EAST,
                                  1,
                                  org_x + event.GetX(),
                                  org_y + event.GetY(),
                                  0);
        }
    }
    else
    {
        event.Skip( true );
    }
#else
    event.Skip( true );
#endif
}

void wxStatusBarGeneric::OnRightDown(wxMouseEvent& event)
{
#ifdef __WXGTK20__
    int width, height;
    GetClientSize(&width, &height);

    if ( ShowsSizeGrip() && (event.GetX() > width-height) )
    {
        GtkWidget *ancestor = gtk_widget_get_toplevel( m_widget );

        if (!GTK_IS_WINDOW (ancestor))
            return;

        GdkWindow *source = GTKGetDrawingWindow();

        int org_x = 0;
        int org_y = 0;
        gdk_window_get_origin( source, &org_x, &org_y );

        gtk_window_begin_move_drag (GTK_WINDOW (ancestor),
                                2,
                                org_x + event.GetX(),
                                org_y + event.GetY(),
                                0);
    }
    else
    {
        event.Skip( true );
    }
#else
    event.Skip( true );
#endif
}

void wxStatusBarGeneric::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME: workarounds for OS/2 bugs have nothing to do here (VZ)
    int width;
#ifdef __WXPM__
    GetSize(&width, &m_lastClientHeight);
#else
    GetClientSize(&width, &m_lastClientHeight);
#endif

    // recompute the cache of the field widths if the status bar width has changed
    m_widthsAbs = CalculateAbsWidths(width);
}

#endif // wxUSE_STATUSBAR
