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

#include <memory>

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
    ~wxComboBox() override;

    void SetFocus() override;

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
    void SetString(unsigned int n, const wxString& value) override;
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
    void SetMaxLength(unsigned long len) override;
    void ForceUpper() override;
    bool SetHint(const wxString& hint) override;
    wxString GetHint() const override;
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

    const wxTextEntry* WXGetTextEntry() const override
        { return m_snapshotTextDispatchDepth ? nullptr : this; }


protected:
    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags = wxSIZE_AUTO) override;
    bool WinUIWantsEditablePeerDuringCreate() const override
        { return m_editable; }
    wxString DoGetValue() const override;
    wxWindow *GetEditableWindow() override { return this; }
    void DoSetValue(const wxString& value, int flags = 0) override;
    void EnableTextChangedEvents(bool enable) override;
    void SendSelectionEvent() override;
    bool MSWShouldPreProcessMessage(WXMSG* msg) override;
    bool DoSetMargins(const wxPoint& margins) override;
    wxPoint DoGetMargins() const override;
    bool DoAutoCompleteStrings(const wxArrayString& choices) override;
    bool DoAutoCompleteFileNames(int flags) override;
    bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

private:
    friend class wxWinUIComboBoxTestAccess;

    void CloseTextPeer();
    void ArmPendingTextValue(const wxString& value,
                             std::uint64_t selectedItemId);
    void ClearPendingTextValue();
    void ReconcilePendingTextValueAtPeerEdge();
    void ClearPendingTextSelection();
    bool CreateSimplePeer();
    void QueueSimplePeerLayoutAtPeerEdge();
    void SendTextEvent(int item = wxNOT_FOUND);
    void SendTextEvent(int item, const wxString& value);
    void SendMaxLengthEvent();
    bool SendClipboardEvent(wxEventType type);
    void ClampTextState();
    void ApplyTextToPeer();
    bool ApplyTextSelectionToPeer();
    void OnPeerTextChanged();
    bool SuppressPendingTextSelectionChange();
    bool HandlePendingTextSelectionChange();
    void ReplayPendingTextSelection();
    void OnPeerDropDownChanged(bool open);
    bool ReadTextSelectionFromPeer();
    bool EnsureThemeTransitionBoundary();
    bool RetireEditPartForTemplateTransition();
    bool ReplayTemplateStateToEditPart();
    void OnPeerTemplateTransition(bool forceEditableReload);
    void OnPeerLayoutUpdated(bool forceTransitionForTesting = false);
    void QueueEditPartResolutionAtLayoutEdge(
        bool forceTransitionForTesting = false);
    // Template application/layout can invoke arbitrary application code.
    // Return false if that code destroyed this control.
    bool ResolveEditPart(bool updateLayout = true);
    void ResolveEditPartOnce(bool updateLayout);
    bool RealizeSimplePeerLayout();
    bool AttachEditPeerHandlers();
    bool NavigateSimpleList(int delta);
    int GetSimpleListPageSize() const;
    bool DismissAutoComplete();
    void RefreshAutoComplete();
    void NavigateAutoComplete(int delta);
    void AcceptAutoCompleteSelection();
    void AcceptAutoCompleteSuggestion(const wxString& suggestion,
                                      std::uint64_t generation);
    WXHWND GetEditHWND() const override { return GetHWND(); }

    wxString m_value;
    long m_insertionPoint = 0;
    long m_selectionStart = 0;
    long m_selectionEnd = 0;
    bool m_editable = true;
    bool m_allowTextEvents = true;
    // wxCommandEvent::GetString() normally re-reads wxEVT_TEXT payloads from
    // WXGetTextEntry(). Snapshot notifications deliberately describe the
    // accepted selection from before arbitrary COMBOBOX-handler re-entry.
    unsigned m_snapshotTextDispatchDepth = 0;
    unsigned long m_maxLength = 0;
    wxCoord m_horizontalMargin = -1;
    bool m_forceUpper = false;
    wxString m_hint;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox);
};

#endif // _WX_WINUI_COMBOBOX_H_
