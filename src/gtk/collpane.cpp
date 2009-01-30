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

#if wxUSE_COLLPANE && !defined(__WXUNIVERSAL__)

#include "wx/collpane.h"
#include "wx/toplevel.h"
#include "wx/sizer.h"
#include "wx/panel.h"

#include "wx/gtk/private.h"

// the lines below duplicate the same definitions in collpaneg.cpp, if we have
// another implementation of this class we should extract them to a common file

const char wxCollapsiblePaneNameStr[] = "collapsiblePane";

wxDEFINE_EVENT( wxEVT_COMMAND_COLLPANE_CHANGED, wxCollapsiblePaneEvent )

IMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePaneEvent, wxCommandEvent)

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "notify::expanded" signal
//-----------------------------------------------------------------------------

extern "C" {

static void
gtk_collapsiblepane_expanded_callback(GObject * WXUNUSED(object),
                                      GParamSpec * WXUNUSED(param_spec),
                                      wxCollapsiblePane *p)
{
    if (!p->IsCollapsed())
    {
       if (p->GetPane()->GetSizer())
            p->GetPane()->GetSizer()->Fit( p->GetPane() );
    }
}
}

extern "C" {
static void
gtk_collpane_map_unmap_callback( GtkWidget *WXUNUSED(pane), GdkEvent *WXUNUSED(event), wxCollapsiblePane* p  )
{
    if (p->HasFlag(wxCP_NO_TLW_RESIZE))
    {
        // fire an event
        wxCollapsiblePaneEvent ev(p, p->GetId(), p->IsCollapsed());
        p->HandleWindowEvent(ev);

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
            // 3) MAGIC HACK: if you ever used GtkExpander in a GTK+ program
            //    you know that this magic call is required to make it possible
            //    to shrink the top level window in the expanded->collapsed
            //    transition.  This may be sometimes undesired but *is*
            //    necessary and if you look carefully, all GTK+ programs using
            //    GtkExpander perform this trick (e.g. the standard "open file"
            //    dialog of GTK+>=2.4 is not resizeable when the expander is
            //    collapsed!)
            gtk_window_set_resizable (GTK_WINDOW (top->m_widget), p->IsExpanded());

            // 4) set size hints
            top->SetMinClientSize(sz);

            // 5) set size
            top->SetClientSize(sz);
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
    p->HandleWindowEvent(ev);
}
}


void wxCollapsiblePane::AddChildGTK(wxWindowGTK* child)
{
    // should be used only once to insert the "pane" into the
    // GtkExpander widget. wxGenericCollapsiblePane::DoAddChild() will check if
    // it has been called only once (and in any case we would get a warning
    // from the following call as GtkExpander is a GtkBin and can contain only
    // a single child!).
    gtk_container_add(GTK_CONTAINER(m_widget), child->m_widget);
}

//-----------------------------------------------------------------------------
// wxCollapsiblePane
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePane, wxControl)

BEGIN_EVENT_TABLE(wxCollapsiblePane, wxCollapsiblePaneBase)
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
    m_bIgnoreNextChange = false;

    if ( !PreCreation( parent, pos, size ) ||
          !wxControl::CreateBase(parent, id, pos, size, style, val, name) )
    {
        wxFAIL_MSG( wxT("wxCollapsiblePane creation failed") );
        return false;
    }

    m_widget =
        gtk_expander_new_with_mnemonic(wxGTK_CONV(GTKConvertMnemonics(label)));
    g_object_ref(m_widget);

    g_signal_connect_after(m_widget, "notify::expanded",
                     G_CALLBACK(gtk_collapsiblepane_expanded_callback), this);
                     
    // this the real "pane"
    m_pPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxTAB_TRAVERSAL|wxNO_BORDER, wxT("wxCollapsiblePanePane") );

    gtk_widget_show(m_widget);
    m_parent->DoAddChild( this );
    
    PostCreation(size);

    // remember the size of this control when it's collapsed
    GtkRequisition req;
    req.width = 2;
    req.height = 2;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
            (m_widget, &req );
            
    m_szCollapsed = wxSize( req.width, req.height );
    
    g_signal_connect (m_pPane->m_widget, "map_event",
                      G_CALLBACK (gtk_collpane_map_unmap_callback), this);
    g_signal_connect (m_pPane->m_widget, "unmap_event",
                      G_CALLBACK (gtk_collpane_map_unmap_callback), this);
    

    return true;
}

wxSize wxCollapsiblePane::DoGetBestSize() const
{
    wxASSERT_MSG( m_widget, wxT("DoGetBestSize called before creation") );

    wxSize sz = m_szCollapsed;
    if ( IsExpanded() )
    {
        wxSize panesz = GetPane()->GetBestSize();
        sz.x = wxMax(sz.x, panesz.x);
        sz.y += gtk_expander_get_spacing(GTK_EXPANDER(m_widget)) + panesz.y;
    }
    
    return sz;
}

void wxCollapsiblePane::Collapse(bool collapse)
{
    // optimization
    if (IsCollapsed() == collapse)
        return;

    // do not send event in next signal handler call
    m_bIgnoreNextChange = true;
    gtk_expander_set_expanded(GTK_EXPANDER(m_widget), !collapse);
}

bool wxCollapsiblePane::IsCollapsed() const
{
    return !gtk_expander_get_expanded(GTK_EXPANDER(m_widget));
}

void wxCollapsiblePane::SetLabel(const wxString &str)
{
    gtk_expander_set_label(GTK_EXPANDER(m_widget), wxGTK_CONV(str));

    // FIXME: we need to update our collapsed width in some way but using GetBestSize()
    // we may get the size of the control with the pane size summed up if we are expanded!
    //m_szCollapsed.x = GetBestSize().x;
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

#endif // wxUSE_COLLPANE && !defined(__WXUNIVERSAL__)

