/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/combobox.h
// Purpose:     wxWinUI wxComboBox declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_COMBOBOX_H_
#define _WX_WINUI_COMBOBOX_H_

#include "wx/choice.h"
#include "wx/textentry.h"

class WXDLLIMPEXP_CORE wxComboBox : public wxChoice,
                                    public wxTextEntry
{
public:
    wxComboBox();
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = nullptr,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    void Clear() override;
    wxString GetValue() const override;
    void SetValue(const wxString& value) override;
    wxString GetStringSelection() const override
        { return wxChoice::GetStringSelection(); }
    void Popup();
    void Dismiss();
    void SetSelection(int n) override;
    void SetSelection(long from, long to) override;
    int GetSelection() const override { return wxChoice::GetSelection(); }
    void GetSelection(long *from, long *to) const override;

    bool IsEditable() const override;
    void SetEditable(bool editable) override;
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

    void Command(wxCommandEvent& event) override;

    const wxTextEntry* WXGetTextEntry() const override { return this; }

protected:
    wxSize DoGetBestSize() const override;
    wxString DoGetValue() const override;
    wxWindow *GetEditableWindow() override { return this; }
    void DoSetValue(const wxString& value, int flags = 0) override;
    void EnableTextChangedEvents(bool enable) override;

private:
    void SendTextEvent();
    void ClampTextState();
    WXHWND GetEditHWND() const override { return GetHWND(); }

    wxString m_value;
    long m_insertionPoint;
    long m_selectionStart;
    long m_selectionEnd;
    bool m_editable;
    bool m_allowTextEvents;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox);
};

#endif // _WX_WINUI_COMBOBOX_H_
