/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

#include "wx/control.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxControl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxControl, wxWindow)

wxControl::wxControl()
{
    m_needParent = TRUE;
}

wxControl::wxControl( wxWindow *parent,
                      wxWindowID id,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxString &name )
{
    (void)Create(parent, id, pos, size, style, name);
}

#if wxUSE_VALIDATORS
wxControl::wxControl( wxWindow *parent,
                      wxWindowID id,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name)
{
    (void)Create(parent, id, pos, size, style, name);
    SetValidator(validator);
}
#endif

void wxControl::SetLabel( const wxString &label )
{
    m_label.Empty();
    for ( const wxChar *pc = label; *pc != wxT('\0'); pc++ )
    {
        if ( *pc == wxT('&') )
        {
            pc++; // skip it
#if 0 // it would be unused anyhow for now - kbd interface not done yet
            if ( *pc != wxT('&') ) m_chAccel = *pc;
#endif
        }
        m_label << *pc;
    }
}

wxString wxControl::GetLabel() const
{
    return m_label;
}


wxSize wxControl::DoGetBestSize() const
{
    GtkRequisition req;
    (* GTK_WIDGET_CLASS( GTK_OBJECT(m_widget)->klass )->size_request )
        (m_widget, &req );

    return wxSize(req.width, req.height);
}

