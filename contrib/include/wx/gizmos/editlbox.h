/////////////////////////////////////////////////////////////////////////////
// Name:        editlbox.h
// Purpose:     ListBox with editable items
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_EDITLBOX_H__
#define __WX_EDITLBOX_H__

#ifdef __GNUG__
	#pragma interface "editlbox.h"
#endif

#include "wx/panel.h"


class WXDLLEXPORT wxBitmapButton;
class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxListEvent;
               
// This class provides a composite control that lets the
// user easily enter list of strings

class WXDLLEXPORT wxEditableListBox : public wxPanel
{
	DECLARE_CLASS(wxEditableListBox);

public:
    wxEditableListBox(wxWindow *parent, wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      const wxString& name = wxT("editableListBox"));

    void SetStrings(const wxArrayString& strings);
    void GetStrings(wxArrayString& strings);

protected:
    wxBitmapButton *m_bDel, *m_bNew, *m_bUp, *m_bDown, *m_bEdit;
    wxListCtrl *m_listCtrl;
    int m_selection;
    bool m_edittingNew;

    void OnItemSelected(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnNewItem(wxCommandEvent& event);
    void OnDelItem(wxCommandEvent& event);
    void OnEditItem(wxCommandEvent& event);
    void OnUpItem(wxCommandEvent& event);
    void OnDownItem(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
