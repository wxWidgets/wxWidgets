/////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

#include "wx/checklst.h"

#if wxUSE_CHECKLISTBOX

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxCheckListBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox,wxListBox)

wxCheckListBox::wxCheckListBox() : wxListBox()
{
    m_hasCheckBoxes = TRUE;
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 int nStrings, 
                 const wxString *choices,
                 long style,
                 const wxValidator& validator,
                 const wxString& name )
{
    m_hasCheckBoxes = TRUE;
    wxListBox::Create( parent, id, pos, size, nStrings, choices, style, validator, name );
}

bool wxCheckListBox::IsChecked( int index ) const
{
    wxCHECK_MSG( m_list != NULL, FALSE, _T("invalid checklistbox") );

    GList *child = g_list_nth( m_list->children, index );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str = wxString(label->label,*wxConvCurrent);

        return (str[1] == _T('X'));
    }

    wxFAIL_MSG(_T("wrong checklistbox index"));
    return FALSE;
}

void wxCheckListBox::Check( int index, bool check )
{
    wxCHECK_RET( m_list != NULL, _T("invalid checklistbox") );

    GList *child = g_list_nth( m_list->children, index );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );

        wxString str = wxString(label->label,*wxConvCurrent);

        if (check == (str[1] == _T('X'))) return;

        if (check)
            str.SetChar( 1, _T('X') );
        else
            str.SetChar( 1, _T('-') );

        gtk_label_set( label, str.mbc_str() );

        return;
    }

    wxFAIL_MSG(_T("wrong checklistbox index"));
}

int wxCheckListBox::GetItemHeight() const
{
    // FIXME
    return 22;
}

#endif
