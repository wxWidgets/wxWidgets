/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/scrolbar.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SCROLLBAR

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// "value_changed" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_value_changed(GtkRange* range, wxScrollBar* win)
{
    wxEventType eventType = win->GetScrollEventType(range);
    if (eventType != wxEVT_NULL)
    {
        const int orient = win->HasFlag(wxSB_VERTICAL) ? wxVERTICAL : wxHORIZONTAL;
        const int value = win->GetThumbPosition();
        wxScrollEvent event(eventType, win->GetId(), value, orient);
        event.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event);
        if (!win->m_isScrolling)
        {
            wxScrollEvent event(wxEVT_SCROLL_CHANGED, win->GetId(), value, orient);
            event.SetEventObject(win);
            win->GetEventHandler()->ProcessEvent(event);
        }
    }
}
}

//-----------------------------------------------------------------------------
// "button_press_event" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_button_press_event(GtkRange*, GdkEventButton*, wxScrollBar* win)
{
    if (g_isIdle) wxapp_install_idle_handler();

    win->m_mouseButtonDown = true;
    return false;
}
}

//-----------------------------------------------------------------------------
// "button_release_event" from scrollbar
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_button_release_event(GtkRange*, GdkEventButton*, wxScrollBar* win)
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    win->m_mouseButtonDown = false;
    // If thumb tracking
    if (win->m_isScrolling)
    {
        win->m_isScrolling = false;
        const int value = win->GetThumbPosition();
        const int orient = win->HasFlag(wxSB_VERTICAL) ? wxVERTICAL : wxHORIZONTAL;

        wxScrollEvent event(wxEVT_SCROLL_THUMBRELEASE, win->GetId(), value, orient);
        event.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event);

        wxScrollEvent event2(wxEVT_SCROLL_CHANGED, win->GetId(), value, orient);
        event2.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(event2);
    }

    return false;
}
}

//-----------------------------------------------------------------------------
// wxScrollBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar,wxControl)

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::~wxScrollBar()
{
}

bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxValidator& validator, const wxString& name )
{
    m_needParent = true;
    m_acceptsFocus = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxScrollBar creation failed") );
        return false;
    }

    const bool isVertical = (style & wxSB_VERTICAL) != 0;
    if (isVertical)
        m_widget = gtk_vscrollbar_new( (GtkAdjustment *) NULL );
    else
        m_widget = gtk_hscrollbar_new( (GtkAdjustment *) NULL );

    m_scrollBar[int(isVertical)] = (GtkRange*)m_widget;

    g_signal_connect(m_widget, "value_changed",
                     G_CALLBACK(gtk_value_changed), this);
    g_signal_connect(m_widget, "button_press_event",
                     G_CALLBACK(gtk_button_press_event), this);
    g_signal_connect(m_widget, "button_release_event",
                     G_CALLBACK(gtk_button_release_event), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

int wxScrollBar::GetThumbPosition() const
{
    GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
    return int(adj->value + 0.5);
}

int wxScrollBar::GetThumbSize() const
{
    GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
    return int(adj->page_size);
}

int wxScrollBar::GetPageSize() const
{
    GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
    return int(adj->page_increment);
}

int wxScrollBar::GetRange() const
{
    GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
    return int(adj->upper);
}

void wxScrollBar::SetThumbPosition( int viewStart )
{
    if (GetThumbPosition() != viewStart)
    {
        BlockScrollEvent();
        gtk_range_set_value((GtkRange*)m_widget, viewStart);
        UnblockScrollEvent();
        GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
        const int i = HasFlag(wxSB_VERTICAL);
        m_scrollPos[i] = adj->value;
    }
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool)
{
    GtkAdjustment* adj = ((GtkRange*)m_widget)->adjustment;
    adj->value = position;
    adj->step_increment = 1;
    adj->page_increment = pageSize;
    adj->page_size = thumbSize;
    BlockScrollEvent();
    gtk_range_set_range((GtkRange*)m_widget, 0, range);
    UnblockScrollEvent();
    const int i = HasFlag(wxSB_VERTICAL);
    m_scrollPos[i] = adj->value;
}

void wxScrollBar::SetPageSize( int pageLength )
{
    SetScrollbar(GetThumbPosition(), GetThumbSize(), GetRange(), pageLength);
}

void wxScrollBar::SetRange(int range)
{
    SetScrollbar(GetThumbPosition(), GetThumbSize(), range, GetPageSize());
}

bool wxScrollBar::IsOwnGtkWindow( GdkWindow *window )
{
    GtkRange *range = GTK_RANGE(m_widget);
    return ( (window == GTK_WIDGET(range)->window) );
}

// static
wxVisualAttributes
wxScrollBar::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_vscrollbar_new);
}

#endif // wxUSE_SCROLLBAR
