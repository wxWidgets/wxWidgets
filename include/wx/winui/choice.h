/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/choice.h
// Purpose:     wxWinUI wxChoice declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CHOICE_H_
#define _WX_WINUI_CHOICE_H_

#include "wx/bitmap.h"

#include <memory>
#include <vector>

class wxWinUIChoiceImpl;

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    wxChoice();
    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0,
             const wxString choices[] = nullptr,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr));
    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr));
    ~wxChoice() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));

    void SetLabel(const wxString& label) override;

    unsigned int GetCount() const override;
    int GetSelection() const override;
    void SetSelection(int n) override;
    int FindString(const wxString& s, bool bCase = false) const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;

protected:
    void DoDeleteOneItem(unsigned int n) override;
    void DoClear() override;
    int DoInsertItems(const wxArrayStringsAdapter& items,
                      unsigned int pos,
                      void **clientData,
                      wxClientDataType type) override;
    void DoSetItemClientData(unsigned int n, void* clientData) override;
    void* DoGetItemClientData(unsigned int n) const override;
    wxSize DoGetBestSize() const override;

    // Hook allowing derived classes (wxBitmapComboBox) to show a per-item
    // image in the dropdown; returns an invalid bitmap by default, in which
    // case the item is rendered as plain text.
    virtual wxBitmap WinUIGetItemBitmap(unsigned int n) const;

    // Rebuild the WinUI item list from the wx-side state (used by derived
    // classes after changing per-item images).
    void WinUIRefreshItems();

    void ApplyItemsToPeer();
    void ApplySelectionToPeer();
    void SendSelectionEvent();

    std::unique_ptr<wxWinUIChoiceImpl> m_winui;
    wxArrayString m_items;
    std::vector<void *> m_clientData;
    int m_selection;
    bool m_updatingPeer;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxChoice);
};

#endif // _WX_WINUI_CHOICE_H_
