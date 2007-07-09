/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/collpane.cpp
// Purpose:     wxCollapsiblePane
// Author:      Francesco Montorsi
// Modified By:
// Created:     8/10/2006
// Id:          $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COLLPANE && defined(__WXGTK24__) && !defined(__WXUNIVERSAL__)

#include "wx/collpane.h"
#include "wx/toplevel.h"
#include "wx/sizer.h"
#include "wx/panel.h"

#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

#include <gtk/gtkexpander.h>

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "notify::expanded" signal
//-----------------------------------------------------------------------------

extern "C" {

static void gtk_collapsiblepane_expanded_callback (GObject    *object,
                                                    GParamSpec *param_spec,
                                                    wxCollapsiblePane *p)
{
    // NB: unlike for the "activate" signal, when this callback is called, if
    //     we try to query the "collapsed" status through p->IsCollapsed(), we
    //     get the right value. I.e. here p->IsCollapsed() will return false if
    //     this callback has been called at the end of a collapsed->expanded
    //     transition and viceversa. Inside the "activate" signal callback
    //     p->IsCollapsed() would return the wrong value!

    wxSize sz;
    if ( p->IsExpanded() )
    {
        // NB: we cannot use the p->GetBestSize() or p->GetMinSize() functions
        //     here as they would return the size for the collapsed expander
        //     even if the collapsed->expanded transition has already been
        //     completed; we solve this problem doing:

        sz = p->m_szCollapsed;

        wxSize panesz = p->GetPane()->GetBestSize();
        sz.x = wxMax(sz.x, panesz.x);
        sz.y += gtk_expander_get_spacing(GTK_EXPANDER(p->m_widget)) + panesz.y;
    }
    else // collapsed
    {
        // same problem described above: using p->Get[Best|Min]Size() here we
        // would get the size of the control when it is expanded even if the
        // expanded->collapsed transition should be complete now...
        // So, we use the size cached at control-creation time...
        sz = p->m_szCollapsed;
    }

    // VERY IMPORTANT:
    // just calling
    //          p->OnStateChange(sz);
    // here would work work BUT:
    //     1) in the expanded->collapsed transition it provokes a lot of flickering
    //     2) in the collapsed->expanded transition using the "Change status" wxButton
    //        in samples/collpane application some strange warnings would be generated
    //        by the "clearlooks" theme, if that's your theme.
    //
    // So we prefer to use some GTK+ native optimized calls, which prevent too many resize
    // calculations to happen. Note that the following code has been very carefully designed
    // and tested - be VERY careful when changing it!

    // 1) need to update our size hints
    // NB: this function call won't actually do any long operation
    //     (redraw/relayouting/resizing) so that it's flicker-free
    p->SetMinSize(sz);

    if (p->HasFlag(wxCP_NO_TLW_RESIZE))
    {
        // fire an event
        wxCollapsiblePaneEvent ev(p, p->GetId(), p->IsCollapsed());
        p->GetEventHandler()->ProcessEvent(ev);
    
        // the user asked to explicitely handle the resizing itself...
        return;
    }

    wxTopLevelWindow *
        top = wxDynamicCast(wxGetTopLevelParent(p), wxTopLevelWindow);
    if ( top && top->GetSizer() )
    {
        // 2) recalculate minimal size of the top window
        wxSize sz = top->GetSizer()->CalcMin();

        if (top->m_mainWidget)
        {
            // 3) MAGIC HACK: if you ever used GtkExpander in a GTK+ program you know
            //    that this magic call is required to make it possible to shrink the
            //    top level window in the expanded->collapsed transition.
            //    This may be sometimes undesired but *is* necessary and if you look
            //    carefully, all GTK+ programs using GtkExpander perform this trick
            //    (e.g. the standard "open file" dialog of GTK+>=2.4 is not resizeable
            //     when the expander is collapsed!)
            gtk_window_set_resizable (GTK_WINDOW (top->m_widget), p->IsExpanded());

            // 4) set size hints: note that this code has been taken and adapted
            //    from src/gtk/toplevel.cpp
            GdkGeometry geom;

            geom.min_width = sz.x;
            geom.min_height = sz.y;

            gtk_window_set_geometry_hints( GTK_WINDOW(top->m_widget),
                                           (GtkWidget*) NULL,
                                           &geom,
                                           GDK_HINT_MIN_SIZE );

            // 5) set size: also this code has been adapted from src/gtk/toplevel.cpp
            //    to do the size changes immediately and not delaying them in the idle
            //    time
            top->m_width = sz.x;
            top->m_height = sz.y;

            int client_x = top->m_miniEdge;
            int client_y = top->m_miniEdge + top->m_miniTitle;
            int client_w = top->m_width - 2*top->m_miniEdge;
            int client_h = top->m_height - 2*top->m_miniEdge - top->m_miniTitle;
            if (client_w < 0)
                client_w = 0;
            if (client_h < 0)
                client_h = 0;

            gtk_pizza_set_size( GTK_PIZZA(top->m_mainWidget),
                                top->m_wxwindow,
                                client_x, client_y, client_w, client_h );

            gtk_widget_set_size_request( top->m_wxwindow, sz.x, sz.y );

        }
    }

    if ( p->m_bIgnoreNextChange )
    {
        // change generated programmatically - do not send an event!
        p->m_bIgnoreNextChange = false;
        return;
    }

    // fire an event
    wxCollapsiblePaneEvent ev(p, p->GetId(), p->IsCollapsed());
    p->GetEventHandler()->ProcessEvent(ev);
}
}

static void
gtk_collapsiblepane_insert_callback(wxWindowGTK* parent, wxWindowGTK* child)
{
    // this callback should be used only once to insert the "pane" into the
    // GtkExpander widget. wxGenericCollapsiblePane::DoAddChild() will check if
    // it has been called only once (and in any case we would get a warning
    // from the following call as GtkExpander is a GtkBin and can contain only
    // a single child!).
    gtk_container_add (GTK_CONTAINER (parent->m_widget), child->m_widget);
}

//-----------------------------------------------------------------------------
// wxCollapsiblePane
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePane, wxGenericCollapsiblePane)

BEGIN_EVENT_TABLE(wxCollapsiblePane, wxGenericCollapsiblePane)
    EVT_SIZE(wxCollapsiblePane::OnSize)
END_EVENT_TABLE()

bool wxCollapsiblePane::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& val,
                               const wxString& name)
{
    if (gtk_check_version(2,4,0))
        return wxGenericCollapsiblePane::Create(parent, id, label,
                                                pos, size, style, val, name);

    m_needParent = true;
    m_acceptsFocus = true;
    m_bIgnoreNextChange = false;

    if ( !PreCreation( parent, pos, size ) ||
          !wxControl::CreateBase(parent, id, pos, size, style, val, name) )
    {
        wxFAIL_MSG( wxT("wxCollapsiblePane creation failed") );
        return false;
    }

    m_widget =
        gtk_expander_new_with_mnemonic(wxGTK_CONV(GTKConvertMnemonics(label)));

    // see the gtk_collapsiblepane_expanded_callback comments to understand why
    // we connect to the "notify::expanded" signal instead of the more common
    // "activate" one
    g_signal_connect(m_widget, "notify::expanded",
                     G_CALLBACK(gtk_collapsiblepane_expanded_callback), this);

    // before creating m_pPane, we need to makesure our own insert callback
    // will be used
    m_insertCallback = gtk_collapsiblepane_insert_callback;

    // this the real "pane"
    m_pPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxTAB_TRAVERSAL|wxNO_BORDER);

    gtk_widget_show( GTK_WIDGET(m_widget) );
    m_parent->DoAddChild( this );

    PostCreation(size);

    // remember the size of this control when it's collapsed
    m_szCollapsed = GetBestSize();

    return true;
}

wxSize wxCollapsiblePane::DoGetBestSize() const
{
    if (!gtk_check_version(2,4,0))
    {
        wxASSERT_MSG( m_widget, wxT("DoGetBestSize called before creation") );

        GtkRequisition req;
        req.width = 2;
        req.height = 2;
        (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
                (m_widget, &req );

        // notice that we do not cache our best size here as it changes
        // all times the user expands/hide our pane
        return wxSize(req.width, req.height);
    }

    return wxGenericCollapsiblePane::DoGetBestSize();
}

void wxCollapsiblePane::Collapse(bool collapse)
{
    if (!gtk_check_version(2,4,0))
    {
        // optimization
        if (IsCollapsed() == collapse)
            return;

        // do not send event in next signal handler call
        m_bIgnoreNextChange = true;
        gtk_expander_set_expanded(GTK_EXPANDER(m_widget), !collapse);
    }
    else
        wxGenericCollapsiblePane::Collapse(collapse);
}

bool wxCollapsiblePane::IsCollapsed() const
{
    if (!gtk_check_version(2,4,0))
        return !gtk_expander_get_expanded(GTK_EXPANDER(m_widget));

    return wxGenericCollapsiblePane::IsCollapsed();
}

void wxCollapsiblePane::SetLabel(const wxString &str)
{
    if (!gtk_check_version(2,4,0))
    {
        gtk_expander_set_label(GTK_EXPANDER(m_widget), wxGTK_CONV(str));

        // FIXME: we need to update our collapsed width in some way but using GetBestSize()
        // we may get the size of the control with the pane size summed up if we are expanded!
        //m_szCollapsed.x = GetBestSize().x;
    }
    else
        wxGenericCollapsiblePane::SetLabel(str);
}

void wxCollapsiblePane::OnSize(wxSizeEvent &ev)
{
#if 0       // for debug only
    wxClientDC dc(this);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(wxPoint(0,0), GetSize());
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(wxPoint(0,0), GetBestSize());
#endif

    // here we need to resize the pane window otherwise, even if the GtkExpander container
    // is expanded or shrinked, the pane window won't be updated!
    m_pPane->SetSize(ev.GetSize().x, ev.GetSize().y - m_szCollapsed.y);

    // we need to explicitely call m_pPane->Layout() or else it won't correctly relayout
    // (even if SetAutoLayout(true) has been called on it!)
    m_pPane->Layout();
}

#endif // wxUSE_COLLPANE && defined(__WXGTK24__) && !defined(__WXUNIVERSAL__)

