/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/collpane.cpp
// Purpose:     wxCollapsiblePane
// Author:      Francesco Montorsi
// Modified By:
// Created:     8/10/2006
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

wxDEFINE_EVENT( wxEVT_COLLAPSIBLEPANE_CHANGED, wxCollapsiblePaneEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePaneEvent, wxCommandEvent);

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
    // When the pane is expanded or collapsed, its best size changes, so it
    // needs to be invalidated in any case.
    p->InvalidateBestSize();

    if (!p->HasFlag(wxCP_NO_TLW_RESIZE))
    {
        wxTopLevelWindow *
            top = wxDynamicCast(wxGetTopLevelParent(p), wxTopLevelWindow);

        // If we want to automatically resize the entire TLW to adopt to the
        // new pane size, we also need to invalidate the cached best sizes of
        // all the intermediate windows to ensure that it's recalculated
        // correctly when doing the layout below.
        for ( wxWindow* w = p->GetParent(); w != top; w = w->GetParent() )
        {
            w->InvalidateBestSize();
        }

        if ( top && top->GetSizer() )
        {
            // 2) recalculate minimal size of the top window
            const wxSize sz = top->GetSizer()->CalcMin();

            if (top->m_mainWidget)
            {
                // 3) MAGIC HACK: if you ever used GtkExpander in a GTK+ program
                //    you know that this magic call is required to make it possible
                //    to shrink the top level window in the expanded->collapsed
                //    transition.  This may be sometimes undesired but *is*
                //    necessary and if you look carefully, all GTK+ programs using
                //    GtkExpander perform this trick (e.g. the standard "open file"
                //    dialog of GTK+>=2.4 is not resizable when the expander is
                //    collapsed!)
                gtk_window_set_resizable (GTK_WINDOW (top->m_widget), p->IsExpanded());

                // 4) set size hints
                top->SetMinClientSize(sz);

                // 5) set size
                top->SetClientSize(sz);
            }
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

wxIMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePane, wxControl);

wxBEGIN_EVENT_TABLE(wxCollapsiblePane, wxCollapsiblePaneBase)
    EVT_SIZE(wxCollapsiblePane::OnSize)
wxEND_EVENT_TABLE()

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
        gtk_expander_new_with_mnemonic(GTKConvertMnemonics(label).utf8_str());
    g_object_ref(m_widget);

    // Connect to the "notify::expanded" signal instead of the more common
    // "activate" one in order to use the new state in our callback, which is
    // more convenient e.g. because calling GetBestSize() returns the suitable
    // size for the new state.
    g_signal_connect(m_widget, "notify::expanded",
                     G_CALLBACK(gtk_collapsiblepane_expanded_callback), this);

    // this the real "pane"
    m_pPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          wxTAB_TRAVERSAL|wxNO_BORDER, wxS("wxCollapsiblePanePane"));

    m_parent->DoAddChild( this );

    PostCreation(size);

    // we should blend into our parent background
    const wxColour bg = parent->GetBackgroundColour();
    SetBackgroundColour(bg);
    m_pPane->SetBackgroundColour(bg);

    // remember the size of this control when it's collapsed
    m_szCollapsed = GTKGetPreferredSize(m_widget);

    return true;
}

wxSize wxCollapsiblePane::DoGetBestSize() const
{
    wxASSERT_MSG( m_widget, wxT("DoGetBestSize called before creation") );

    wxSize sz = m_szCollapsed;

    if ( IsExpanded() )
    {
        const wxSize panesz = m_pPane->GetBestSize();
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
    gtk_expander_set_label(GTK_EXPANDER(m_widget),
                           GTKConvertMnemonics(str).utf8_str());

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
    // is expanded or shrunk, the pane window won't be updated!
    int h = ev.GetSize().y - m_szCollapsed.y;
    if (h < 0) h = 0;
    m_pPane->SetSize(ev.GetSize().x, h);

    // we need to explicitly call m_pPane->Layout() or else it won't correctly relayout
    // (even if SetAutoLayout(true) has been called on it!)
    m_pPane->Layout();
}


GdkWindow *wxCollapsiblePane::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    GtkWidget *label = gtk_expander_get_label_widget( GTK_EXPANDER(m_widget) );
    windows.Add(gtk_widget_get_window(label));
    windows.Add(gtk_widget_get_window(m_widget));

    return nullptr;
}

#endif // wxUSE_COLLPANE && !defined(__WXUNIVERSAL__)

