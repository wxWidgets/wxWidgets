/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/listbox.h
// Purpose:     wxWinUI wxListBox declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_LISTBOX_H_
#define _WX_WINUI_LISTBOX_H_

#include <memory>
#include <vector>

class wxWinUIListBoxImpl;

class WXDLLIMPEXP_CORE wxListBox : public wxListBoxBase
{
public:
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int n = 0, const wxString choices[] = nullptr,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr));
    wxListBox(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr));

    ~wxListBox() override;

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    unsigned int GetCount() const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;
    int FindString(const wxString& s, bool bCase = false) const override;

    bool IsSelected(int n) const override;
    int GetSelection() const override;
    int GetSelections(wxArrayInt& aSelections) const override;

protected:
    void DoSetFirstItem(int n) override;
    void DoSetSelection(int n, bool select) override;

    void DoClear() override;
    void DoDeleteOneItem(unsigned int n) override;
    int DoInsertItems(const wxArrayStringsAdapter& items,
                      unsigned int pos,
                      void **clientData,
                      wxClientDataType type) override;
    void DoSetItemClientData(unsigned int n, void* clientData) override;
    void* DoGetItemClientData(unsigned int n) const override;

    wxSize DoGetBestSize() const override;

    // Hooks used by wxCheckListBox: when WinUIIsCheckable() returns true, each
    // item is rendered with a leading WinUI CheckBox whose state is taken from
    // m_checks and whose toggles are reported back through WinUIOnItemToggled().
    // The check state vector is maintained here, in lockstep with the item and
    // client-data arrays, so that it survives sorting, insertion and deletion.
    virtual bool WinUIIsCheckable() const { return false; }
    virtual void WinUIOnItemToggled(unsigned int n, bool check)
        { if ( n < m_checks.size() ) m_checks[n] = check; }

    // Rebuild the WinUI item list from the wx-side state; used by derived
    // classes (wxCheckListBox) after changing a check state.
    void WinUIRefreshItems();

    void Init();

    void ApplyItemsToPeer();
    void ApplySelectionToPeer();
    void SendSelectionEvent();

    std::unique_ptr<wxWinUIListBoxImpl> m_winui;
    wxArrayString m_items;
    std::vector<void *> m_clientData;
    std::vector<bool> m_checks;
    int m_selection = wxNOT_FOUND;
    bool m_updatingPeer = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxListBox);
};

#endif // _WX_WINUI_LISTBOX_H_
