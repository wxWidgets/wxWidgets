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
