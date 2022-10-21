///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/checklst.h
// Purpose:     wxCheckListBox class
// Author:      Robert Roebling
// Modified by:
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKCHECKLIST_H_
#define _WX_GTKCHECKLIST_H_

//-----------------------------------------------------------------------------
// wxCheckListBox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCheckListBox : public wxCheckListBoxBase
{
public:
    wxCheckListBox();
    wxCheckListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int nStrings = 0,
            const wxString *choices = nullptr,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxListBoxNameStr));
    wxCheckListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxListBoxNameStr));

    virtual bool IsChecked(unsigned int index) const override;
    virtual void Check(unsigned int index, bool check = true) override;

    int GetItemHeight() const;

    void DoCreateCheckList();

private:
    wxDECLARE_DYNAMIC_CLASS(wxCheckListBox);
};

#endif   // _WX_GTKCHECKLIST_H_
