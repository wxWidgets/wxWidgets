/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/statbox.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/win_gtk.h"

// constants taken from GTK sources
#define LABEL_PAD 1
#define LABEL_SIDE_PAD 2

//-----------------------------------------------------------------------------
// "size_allocate" from m_widget
//-----------------------------------------------------------------------------

#ifndef __WXGTK3__
extern "C" {
static void size_allocate(GtkWidget* widget, GtkAllocation* alloc, void*)
{
    // clip label as GTK >= 2.12 does
    GtkWidget* label_widget = gtk_frame_get_label_widget(GTK_FRAME(widget));
    int w = alloc->width -
        2 * gtk_widget_get_style(widget)->xthickness - 2 * LABEL_PAD - 2 * LABEL_SIDE_PAD;
    if (w < 0)
        w = 0;

    GtkAllocation a;
    gtk_widget_get_allocation(label_widget, &a);
    if (a.width > w)
    {
        a.width = w;
        gtk_widget_size_allocate(label_widget, &a);
    }
}

static gboolean expose_event(GtkWidget* widget, GdkEventExpose*, wxWindow*)
{
    const GtkAllocation& a = widget->allocation;
    gtk_paint_flat_box(gtk_widget_get_style(widget), gtk_widget_get_window(widget),
        GTK_STATE_NORMAL, GTK_SHADOW_NONE, nullptr, widget, "", a.x, a.y, a.width, a.height);
    return false;
}
}
#endif

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

bool wxStaticBox::DoCreate(wxWindow *parent,
                           wxWindowID id,
                           const wxString* labelStr,
                           wxWindow* labelWin,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBox creation failed") );
        return false;
    }

    if ( labelStr )
    {
        m_widget = GTKCreateFrame(*labelStr);

        // only base SetLabel needs to be called after GTKCreateFrame
        wxControl::SetLabel(*labelStr);
    }
    else // Use the given window as the label.
    {
        wxCHECK_MSG( labelWin, false, wxS("Label window can't be null") );

        GtkWidget* const labelWidget = labelWin->m_widget;
        wxCHECK_MSG( labelWidget, false, wxS("Label window must be created") );

        // The widget must not have any parent at GTK+ level or setting it as
        // label widget would fail.
        GtkWidget* const oldParent = gtk_widget_get_parent(labelWidget);
        gtk_container_remove(GTK_CONTAINER(oldParent), labelWidget);
        gtk_widget_unparent(labelWidget);

        // It also should be our child at wx API level, but without being our
        // child in wxGTK, i.e. it must not be added to the GtkFrame container,
        // so we can't call Reparent() here (not even wxWindowBase version, as
        // it still would end up in our overridden AddChild()), nor the normal
        // AddChild() for the same reason.
        labelWin->GetParent()->RemoveChild(labelWin);
        wxWindowBase::AddChild(labelWin);

        m_labelWin = labelWin;

        m_widget = gtk_frame_new(nullptr);
        gtk_frame_set_label_widget(GTK_FRAME(m_widget), labelWidget);
    }

    g_object_ref(m_widget);

    m_parent->DoAddChild( this );

    PostCreation(size);

    // need to set non default alignment?
    gfloat xalign = 0;
    if ( style & wxALIGN_CENTER )
        xalign = 0.5;
    else if ( style & wxALIGN_RIGHT )
        xalign = 1.0;

    gtk_frame_set_label_align(GTK_FRAME(m_widget), xalign, 0.5);

#ifndef __WXGTK3__
    if (!wx_is_at_least_gtk2(12))
    {
        // we connect this signal to perform label-clipping as GTK >= 2.12 does
        g_signal_connect(m_widget, "size_allocate", G_CALLBACK(size_allocate), nullptr);
    }
#endif

    m_container.DisableSelfFocus();

    return true;
}

void wxStaticBox::AddChild( wxWindowBase *child )
{
    if (!m_wxwindow)
    {
        // make this window a container of other wxWindows by instancing a wxPizza
        // and packing it into the GtkFrame:
        m_wxwindow = wxPizza::New();
        gtk_widget_show( m_wxwindow );
        gtk_container_add( GTK_CONTAINER (m_widget), m_wxwindow );
        GTKApplyWidgetStyle();
    }

    wxStaticBoxBase::AddChild(child);
}

void wxStaticBox::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid staticbox") );

    wxCHECK_RET( !m_labelWin, wxS("Doesn't make sense when using label window") );

    GTKSetLabelForFrame(GTK_FRAME(m_widget), label);
}

void wxStaticBox::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GTKFrameApplyWidgetStyle(GTK_FRAME(m_widget), style);
    if ( m_labelWin )
        GTKDoApplyWidgetStyle(m_labelWin, style);
    if (m_wxwindow)
        GTKApplyStyle(m_wxwindow, style);

#ifndef __WXGTK3__
    g_signal_handlers_disconnect_by_func(m_widget, (void*)expose_event, this);
    if (m_backgroundColour.IsOk())
        g_signal_connect(m_widget, "expose-event", G_CALLBACK(expose_event), this);
#endif
}

bool wxStaticBox::GTKWidgetNeedsMnemonic() const
{
    return true;
}

void wxStaticBox::GTKWidgetDoSetMnemonic(GtkWidget* w)
{
    GTKFrameSetMnemonicWidget(GTK_FRAME(m_widget), w);
}

// static
wxVisualAttributes
wxStaticBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_frame_new(""));
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    GtkWidget* label = gtk_frame_get_label_widget(GTK_FRAME(m_widget));
#ifdef __WXGTK3__
    *borderOther = 0;
    *borderTop = 0;
    if (label)
    {
        int nat_width;
        gtk_widget_get_preferred_width(label, nullptr, &nat_width);
        gtk_widget_get_preferred_height_for_width(label, nat_width, borderTop, nullptr);
    }
#else
    gtk_widget_ensure_style(m_widget);
    const int border_width = GTK_CONTAINER(m_widget)->border_width;
    *borderOther = border_width + m_widget->style->xthickness;
    *borderTop = border_width;
    if (label)
    {
        GtkRequisition req;
        gtk_widget_size_request(label, &req);
        *borderTop += req.height;
    }
    else
        *borderTop += m_widget->style->ythickness;
#endif
}

#endif // wxUSE_STATBOX
