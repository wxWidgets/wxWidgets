/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/radiobox.h
// Purpose:     wxWinUI wxRadioBox declaration (WinUI RadioButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_RADIOBOX_H_
#define _WX_WINUI_RADIOBOX_H_

#include "wx/arrstr.h"

#include <memory>
#include <vector>

class wxWinUIRadioBoxImpl;

class WXDLLIMPEXP_CORE wxRadioBox : public wxControl,
                                    public wxRadioBoxBase
{
public:
    wxRadioBox();
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = nullptr,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    ~wxRadioBox() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    // wxItemContainerImmutable
    unsigned int GetCount() const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;
    void SetSelection(int n) override;
    int GetSelection() const override;

    // wxRadioBoxBase
    bool Enable(unsigned int n, bool enable = true) override;
    bool Show(unsigned int n, bool show = true) override;
    bool IsItemEnabled(unsigned int n) const override;
    bool IsItemShown(unsigned int n) const override;

    // bring the wxWindow versions back into scope
    bool Enable(bool enable = true) override { return wxControl::Enable(enable); }
    bool Show(bool show = true) override { return wxControl::Show(show); }

protected:
    wxSize DoGetBestSize() const override;

    bool DoCreate(wxWindow *parent, wxWindowID id, const wxString& title,
                  const wxPoint& pos, const wxSize& size, int majorDim,
                  long style, const wxValidator& validator,
                  const wxString& name);
    void RebuildItems();
    void SendSelectionEvent();
    int FindSelectedItem() const;

    std::unique_ptr<wxWinUIRadioBoxImpl> m_winui;
    wxArrayString m_strings;
    std::vector<bool> m_itemEnabled;
    std::vector<bool> m_itemShown;
    int m_selection = wxNOT_FOUND;
    bool m_updating = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioBox);
};

#endif // _WX_WINUI_RADIOBOX_H_
