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

#include "wx/panel.h"
#include "wx/gizmos/gizmos.h"

class WXDLLEXPORT wxBitmapButton;
class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxListEvent;

#define wxEL_ALLOW_NEW          0x0100
#define wxEL_ALLOW_EDIT         0x0200
#define wxEL_ALLOW_DELETE       0x0400
#define wxEL_NO_REORDER         0x0800

// This class provides a composite control that lets the
// user easily enter list of strings

class WXDLLIMPEXP_GIZMOS wxEditableListBox : public wxPanel
{
    DECLARE_CLASS(wxEditableListBox)

public:
    wxEditableListBox(wxWindow *parent, wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE,
                      const wxString& name = wxT("editableListBox"));

    void SetStrings(const wxArrayString& strings);
    void GetStrings(wxArrayString& strings) const;

    wxListCtrl* GetListCtrl()       { return m_listCtrl; }
    wxBitmapButton* GetDelButton()  { return m_bDel; }
    wxBitmapButton* GetNewButton()  { return m_bNew; }
    wxBitmapButton* GetUpButton()   { return m_bUp; }
    wxBitmapButton* GetDownButton() { return m_bDown; }
    wxBitmapButton* GetEditButton() { return m_bEdit; }

protected:
    wxBitmapButton *m_bDel, *m_bNew, *m_bUp, *m_bDown, *m_bEdit;
    wxListCtrl *m_listCtrl;
    int m_selection;
    long m_style;

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
