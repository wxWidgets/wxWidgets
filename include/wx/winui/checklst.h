/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/checklst.h
// Purpose:     wxWinUI wxCheckListBox declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CHECKLST_H_
#define _WX_WINUI_CHECKLST_H_

class WXDLLIMPEXP_CORE wxCheckListBox : public wxCheckListBoxBase
{
public:
    wxCheckListBox() = default;
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int nStrings = 0,
                   const wxString choices[] = nullptr,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxListBoxNameStr))
    {
        Create(parent, id, pos, size, nStrings, choices, style, validator, name);
    }
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxListBoxNameStr))
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr))
    {
        return wxListBox::Create(parent, id, pos, size, n, choices,
                                 style, validator, name);
    }
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr))
    {
        return wxListBox::Create(parent, id, pos, size, choices,
                                 style, validator, name);
    }

    // items may be checked
    bool IsChecked(unsigned int item) const override;
    void Check(unsigned int item, bool check = true) override;
    void Toggle(unsigned int item);

protected:
    bool WinUIIsCheckable() const override { return true; }
    void WinUIOnItemToggled(unsigned int n, bool check) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCheckListBox);
};

#endif // _WX_WINUI_CHECKLST_H_
