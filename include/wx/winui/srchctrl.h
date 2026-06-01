/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/srchctrl.h
// Purpose:     wxWinUI wxSearchCtrl declaration (WinUI AutoSuggestBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SRCHCTRL_H_
#define _WX_WINUI_SRCHCTRL_H_

#include "wx/arrstr.h"

#include <memory>

class WXDLLIMPEXP_FWD_CORE wxMenu;
class wxWinUISearchCtrlImpl;

class WXDLLIMPEXP_CORE wxSearchCtrl : public wxSearchCtrlBase
{
public:
    wxSearchCtrl();
    wxSearchCtrl(wxWindow *parent,
                 wxWindowID id,
                 const wxString& value = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxSearchCtrlNameStr));
    ~wxSearchCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSearchCtrlNameStr));

    // wxSearchCtrlBase
    void SetMenu(wxMenu *menu) override;
    wxMenu *GetMenu() override;
    void ShowSearchButton(bool show) override;
    bool IsSearchButtonVisible() const override;
    void ShowCancelButton(bool show) override;
    bool IsCancelButtonVisible() const override;
    void SetDescriptiveText(const wxString& text) override;
    wxString GetDescriptiveText() const override;

    // wxTextEntry
    void WriteText(const wxString& text) override;
    void Remove(long from, long to) override;
    void Copy() override;
    void Cut() override;
    void Paste() override;
    void Undo() override;
    void Redo() override;
    bool CanUndo() const override;
    bool CanRedo() const override;
    void SetInsertionPoint(long pos) override;
    long GetInsertionPoint() const override;
    long GetLastPosition() const override;
    void SetSelection(long from, long to) override;
    void GetSelection(long *from, long *to) const override;
    bool IsEditable() const override;
    void SetEditable(bool editable) override;

protected:
    wxSize DoGetBestSize() const override;

    void DoSetValue(const wxString& value, int flags) override;
    wxString DoGetValue() const override;

    WXHWND GetEditHWND() const override;

    void ApplyValueToPeer();
    void ApplySuggestions(const wxArrayString& choices);

    // wxTextEntry hook for AutoComplete(wxArrayString)
    bool DoAutoCompleteStrings(const wxArrayString& choices) override;

    std::unique_ptr<wxWinUISearchCtrlImpl> m_winui;
    wxString m_value;
    wxString m_descriptiveText;
    wxArrayString m_suggestions;
    wxMenu *m_menu = nullptr;
    long m_insertionPoint = 0;
    long m_selectionStart = 0;
    long m_selectionEnd = 0;
    bool m_editable = true;
    bool m_searchButtonVisible = true;
    bool m_cancelButtonVisible = false;
    bool m_updatingPeer = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSearchCtrl);
};

#endif // _WX_WINUI_SRCHCTRL_H_
