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

#include "wx/defs.h"

#if wxUSE_CONTROLS

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

bool wxControl::Create( wxWindow *parent,
                      wxWindowID id,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name )
{
    bool ret = wxWindow::Create(parent, id, pos, size, style, name);
    
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return ret;
}

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
    // Do not return any arbitrary default value...
    wxASSERT_MSG( m_widget, wxT("DoGetBestSize called before creation") );

    GtkRequisition req;
    req.width = 2;
    req.height = 2;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
        (m_widget, &req );

    return wxSize(req.width, req.height);
}

#ifdef __WXGTK20__
wxString wxControl::PrepareLabelMnemonics( const wxString &label ) const
{
    //Format mnemonics properly for GTK2. This can be called from GTK1.x, but
    //it's not very useful because mnemonics don't exist prior to GTK2.
    wxString label2;
    for (size_t i = 0; i < label.Len(); i++)
    {
        if (label.GetChar(i) == wxT('&'))
        {
            //Mnemonic escape sequence "&&" is a literal "&" in the output.
            if (label.GetChar(i + 1) == wxT('&'))
            {
                label2 << wxT('&');
                i++;
            }
            //Handle special case of "&_" (i.e. "_" is the mnemonic).
            //FIXME - Is it possible to use "_" as a GTK mnemonic? Just use a
            //dash for now.
            else if (label.GetChar(i + 1) == wxT('_'))
            {
                label2 << wxT("_-");
                i++;
            }
            //Replace WX mnemonic indicator "&" with GTK indicator "_".
            else
            {
                label2 << wxT('_');
            }
        }
        else if (label.GetChar(i) == wxT('_'))
        {
            //Escape any underlines in the string so GTK doesn't use them.
            label2 << wxT("__");
        }
        else
        {
            label2 << label.GetChar(i);
        }
    }
    return label2;
}
#endif

#endif // wxUSE_CONTROLS

