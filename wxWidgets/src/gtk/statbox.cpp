/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/statbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"
#include "wx/gtk/private.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"


// ============================================================================
// implementation
// ============================================================================

// constants taken from GTK sources
#define LABEL_PAD 1
#define LABEL_SIDE_PAD 2

//-----------------------------------------------------------------------------
// "gtk_frame_size_allocate" signal
//-----------------------------------------------------------------------------

extern "C" {

static void
gtk_frame_size_allocate (GtkWidget     *widget,
                         GtkAllocation *allocation,
                         wxStaticBox *p)
{
    GtkFrame *frame = GTK_FRAME (widget);

    // this handler gets called _after_ the GTK+'s own signal handler; thus we
    // need to fix only the width of the GtkLabel
    // (everything else has already been handled by the GTK+ signal handler).

    if (frame->label_widget && GTK_WIDGET_VISIBLE (frame->label_widget))
    {
        GtkAllocation ca = frame->label_widget->allocation;

        // we want the GtkLabel to not exceed maxWidth:
        int maxWidth = allocation->width - 2*LABEL_SIDE_PAD - 2*LABEL_PAD;
        maxWidth = wxMax(2, maxWidth);      // maxWidth must always be positive!

        // truncate the label to the GtkFrame width...
        ca.width = wxMin(ca.width, maxWidth);
        gtk_widget_size_allocate(frame->label_widget, &ca);
    }
}

}


//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox( wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name )
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create( wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBox creation failed") );
        return FALSE;
    }

    m_widget = GTKCreateFrame(label);
    wxControl::SetLabel(label);

    m_parent->DoAddChild( this );

    PostCreation(size);

    // need to set non default alignment?
    gfloat xalign;
    if ( style & wxALIGN_CENTER )
        xalign = 0.5;
    else if ( style & wxALIGN_RIGHT )
        xalign = 1.0;
    else // wxALIGN_LEFT
        xalign = 0.0;

    if ( style & (wxALIGN_RIGHT | wxALIGN_CENTER) ) // left alignment is default
        gtk_frame_set_label_align(GTK_FRAME( m_widget ), xalign, 0.5);

    // in order to clip the label widget, we must connect to the size allocate
    // signal of this GtkFrame after the default GTK+'s allocate size function
    g_signal_connect_after (m_widget, "size_allocate",
                            G_CALLBACK (gtk_frame_size_allocate), this);

    return TRUE;
}

void wxStaticBox::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid staticbox") );

    GTKSetLabelForFrame(GTK_FRAME(m_widget), label);
}

void wxStaticBox::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GTKFrameApplyWidgetStyle(GTK_FRAME(m_widget), style);
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
    return GetDefaultAttributesFromGTKWidget(gtk_frame_new);
}


void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    const int BORDER = 5; // FIXME: hardcoded value

    *borderTop = GetLabel().empty() ? 2*BORDER : GetCharHeight();
    *borderOther = BORDER;
}

#endif // wxUSE_STATBOX
