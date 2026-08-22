/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/textctrl.h
// Purpose:     wxWinUI wxTextCtrl declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TEXTCTRL_H_
#define _WX_WINUI_TEXTCTRL_H_

#include <cstdint>
#include <memory>

class wxWinUITextCtrlImpl;

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
    using WinUICreateLoadedHookForTesting =
        void (*)(wxTextCtrl *owner, void *context);
    using WinUITemporarySelectionHookForTesting =
        void (*)(wxTextCtrl *owner, void *context);
    using WinUIPasswordTextChangingHookForTesting =
        void (*)(wxTextCtrl *owner, void *context);
    enum class WinUIScrollBarVisibilityForTesting
    {
        Disabled,
        Auto,
        Hidden,
        Visible
    };

    wxTextCtrl();
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxTextCtrlNameStr));
    ~wxTextCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxTextCtrlNameStr));

    wxString GetRange(long from, long to) const override;
    wxString GetRTFValue() const override;
    void SetRTFValue(const wxString& val) override;
    bool IsRTFSupported() override { return IsRich(); }
    wxTextSearchResult SearchText(const wxTextSearch& search) const override;
    bool IsEmpty() const;
    void WriteText(const wxString& text) override;
    void AppendText(const wxString& text) override;
    void Replace(long from, long to, const wxString& value) override;
    void Remove(long from, long to) override;
    void Clear() override;

    void Copy() override;
    void Cut() override;
    void Paste() override;
    bool CanCopy() const override;
    bool CanCut() const override;
    void Undo() override;
    void Redo() override;
    bool CanUndo() const override;
    bool CanRedo() const override;
    void EmptyUndoBuffer() override;

    bool SetStyle(long start, long end, const wxTextAttr& style) override;
    bool SetDefaultStyle(const wxTextAttr& style) override;
    bool GetStyle(long position, wxTextAttr& style) override;

    void SetInsertionPoint(long pos) override;
    void SetInsertionPointEnd() override;
    long GetInsertionPoint() const override;
    wxTextPos GetLastPosition() const override;
    void SetSelection(long from, long to) override;
    void GetSelection(long *from, long *to) const override;

    bool IsEditable() const override;
    void SetEditable(bool editable) override;
    void SetMaxLength(unsigned long len) override;
    void ForceUpper() override;
    bool SetHint(const wxString& hint) override;
    wxString GetHint() const override;

    void SetWindowStyleFlag(long style) override;

    // Keep the wxMSW-specific public surface available when building the
    // WinUI port on Windows. RichEditBox is based on the current TOM engine,
    // rather than one of the legacy RichEdit DLL versions.
    bool ShowNativeCaret(bool show = true);
    bool HideNativeCaret() { return ShowNativeCaret(false); }
#if wxUSE_RICHEDIT
    int GetRichVersion() const { return m_winui && IsRich() ? 4 : 0; }
    virtual wxMenu *MSWCreateContextMenu();
#endif // wxUSE_RICHEDIT
    bool IsInkEdit() const { return false; }

    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;
#if wxUSE_TOOLTIPS
    void DoSetToolTipText(const wxString& tip) override;
    void DoSetToolTip(wxToolTip *tip) override;
#endif // wxUSE_TOOLTIPS

    int GetLineLength(long lineNo) const override;
    wxString GetLineText(long lineNo) const override;
    int GetNumberOfLines() const override;

    bool IsModified() const override;
    void MarkDirty() override;
    void DiscardEdits() override;

    bool EmulateKeyPress(const wxKeyEvent& event) override;
    void Command(wxCommandEvent& event) override;

    bool IsRich() const
    {
        return HasFlag(wxTE_RICH | wxTE_RICH2) &&
               !HasFlag(wxTE_PASSWORD);
    }

#if wxUSE_SPELLCHECK
    bool EnableProofCheck(const wxTextProofOptions& options
                            = wxTextProofOptions::Default()) override;
    wxTextProofOptions GetProofCheckOptions() const override;
#endif // wxUSE_SPELLCHECK

    // Implementation-only deterministic seams. The Enter/Tab seams run the
    // same operation as the XAML KeyDown delegate. The password seam injects
    // a complete content-change proposal into the shared processing
    // transaction; it deliberately does not synthesize or claim to raise
    // PasswordChanging.
    bool WinUIProcessEnterForTesting();
    bool WinUIProcessTabForTesting();
    void WinUIPasteTextForTesting(const wxString& text);
    bool WinUIInjectPasswordContentChangeForTesting(
        const wxString& proposed);
    bool WinUISetTextBoxPeerTextForTesting(const wxString& text);
    bool WinUIGetPeerTextForTesting(wxString *text) const;
    bool WinUIReplacePeerSelectionForTesting(const wxString& text);
    bool WinUIHasLocalFontOverridesForTesting() const;
    bool WinUIRichClipboardUsesAllFormatsForTesting() const;
    bool WinUIGetNativeCaretShownForTesting(bool *shown) const;
    bool WinUIHasNoHideSelectionProjectionForTesting() const;
    bool WinUIGetPasswordPeerSecurityForTesting(
        bool *isPassword,
        bool *hasValuePattern,
        bool *hasTextPattern,
        wxString *documentText,
        bool *textPredictionEnabled) const;
    void WinUIForceNextPasswordScrubFailureForTesting(long hresult);
    void WinUIForceNextPasswordScrubPartialWriteForTesting();
    bool WinUIGetPasswordFailClosedForTesting(
        long *hresult,
        bool *documentWasEmpty) const;
    unsigned WinUIGetPasswordClipboardExportAttemptCountForTesting() const;
    unsigned WinUIGetAutoUrlRangeCountForTesting() const;
    bool WinUIGetAutoUrlRangeForTesting(unsigned n,
                                        long *from,
                                        long *to,
                                        wxString *target) const;
    unsigned WinUIGetAutoCompleteSuggestionCountForTesting() const;
    wxString WinUIGetAutoCompleteSuggestionForTesting(unsigned n) const;
    bool WinUIInvokeAutoCompleteSuggestionForTesting(unsigned n);
    bool WinUIGetPeerSelectionForTesting(long *from, long *to) const;
    bool WinUIGetScrollStateForTesting(double *horizontalOffset,
                                       double *verticalOffset,
                                       double *viewportWidth,
                                       double *viewportHeight,
                                       double *rasterizationScale,
                                       int *stage = nullptr,
                                       double *scrollableWidth = nullptr,
                                       double *scrollableHeight = nullptr,
                                       double *viewOriginX = nullptr,
                                       double *viewOriginY = nullptr,
                                       double *viewUnitXScale = nullptr,
                                       double *viewUnitYScale = nullptr) const;
    bool WinUIGetVerticalScrollBarVisibilityForTesting(
        WinUIScrollBarVisibilityForTesting *visibility) const;
    bool WinUIGetPositionVisibilityStateForTesting(
        bool *pending,
        unsigned *queuedRetries,
        bool *hasRetainedPeerReferences = nullptr,
        unsigned *richTrace = nullptr,
        unsigned *richPassCount = nullptr) const;
    void WinUIUseTextBoxPeerForTesting();
    void WinUIForceNextPositionVisibilityRetryForTesting();
    void WinUISetNextTemporarySelectionHookForTesting(
        WinUITemporarySelectionHookForTesting hook,
        void *context);
    void WinUISetNextPasswordTextChangingHookForTesting(
        WinUIPasswordTextChangingHookForTesting hook,
        void *context);
    void WinUISetNextCreateLoadedHookForTesting(
        WinUICreateLoadedHookForTesting hook,
        void *context);

    long XYToPosition(long x, long y) const override;
    bool PositionToXY(long pos, long *x, long *y) const override;
    void ShowPosition(long pos) override;

    wxTextCtrlHitTestResult HitTest(const wxPoint& pt, long *pos) const override;
    wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                    wxTextCoord *col,
                                    wxTextCoord *row) const override
    {
        return wxTextCtrlBase::HitTest(pt, col, row);
    }

protected:
    void DoEnable(bool enable) override;
    void DoSetValue(const wxString& value, int flags = 0) override;
    wxString DoGetValue() const override;
    void EnableTextChangedEvents(bool enable) override
        { m_textEventsEnabled = enable; }
    wxPoint DoPositionToCoords(long pos) const override;
    wxSize DoGetBestSize() const override;
    wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;
    bool MSWShouldPreProcessMessage(WXMSG* msg) override;
    bool DoSetMargins(const wxPoint& pt) override;
    wxPoint DoGetMargins() const override;
    bool DoAutoCompleteStrings(const wxArrayString& choices) override;
    bool DoAutoCompleteFileNames(int flags) override;
    bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

private:
    void WriteTextWithPolicy(const wxString& text, bool enforceMaxLength);
    void ApplyValueToPeer();
    void ReadSelectionFromPeer();
    void UpdateWinUIAppearance();
    void OnSetFocus(wxFocusEvent& event);
    void RefreshAutoComplete();
    void UpdateAutoUrlRanges();
    void OnAutoUrlMouse(wxMouseEvent& event);
#if wxUSE_RICHEDIT && wxUSE_MENUS
    void ShowRichContextMenu();
#endif
#if wxUSE_MENUS
    void OnRichContextMenuCommand(wxCommandEvent& event);
    void OnUpdateRichContextMenuCommand(wxUpdateUIEvent& event);
#endif
#if wxUSE_DRAG_AND_DROP
    void OnDropFiles(wxDropFilesEvent& event);
#endif
    bool SendClipboardEvent(wxEventType type);
    void SendTextEvent();
    void SendMaxLengthEvent();
    void ProcessPasswordPeerChange(const wxString& proposed,
                                   bool isContentChange);
    void ProcessSecurePasswordTextChanging();
    void DispatchSecurePasswordChange();
    void PushSecurePasswordUndoSnapshot();
    void PushSecurePasswordUndoSnapshot(long insertionPoint,
                                        long selectionStart,
                                        long selectionEnd);
    bool RestoreSecurePasswordSnapshot(bool redo);
    void ClampInsertionPoint();
    void ApplyVerticalScrollBarPolicy(unsigned retriesRemaining);
    void EnsurePositionVisible(long pos);
    void FinishPositionVisibility(std::uint64_t generation,
                                  unsigned retriesRemaining);
    wxArrayString GetLines() const;
    WXHWND GetEditHWND() const override;

    std::unique_ptr<wxWinUITextCtrlImpl> m_winui;
    wxString m_value;
    long m_insertionPoint = 0;
    long m_selectionStart = 0;
    long m_selectionEnd = 0;
    unsigned long m_maxLength = 0;
    bool m_modified = false;
    bool m_editable = true;
    bool m_textEventsEnabled = true;
    bool m_updatingPeer = false;
    bool m_forceUpper = false;
    bool m_isNativeCaretShown = true;
    bool m_useTextBoxPeerForTesting = false;
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS
    WinUICreateLoadedHookForTesting m_nextCreateLoadedHookForTesting = nullptr;
    void *m_nextCreateLoadedContextForTesting = nullptr;
    WinUITemporarySelectionHookForTesting
        m_nextTemporarySelectionHookForTesting = nullptr;
    void *m_nextTemporarySelectionContextForTesting = nullptr;
    WinUIPasswordTextChangingHookForTesting
        m_nextPasswordTextChangingHookForTesting = nullptr;
    void *m_nextPasswordTextChangingContextForTesting = nullptr;
    long m_nextPasswordScrubFailureForTesting = 0;
    bool m_nextPasswordScrubPartialWriteForTesting = false;
    long m_passwordPeerFailClosedHresultForTesting = 0;
    bool m_passwordPeerWasEmptyOnFailClosedForTesting = false;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTextCtrl);
};

#endif // _WX_WINUI_TEXTCTRL_H_
