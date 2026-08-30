/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxTextCtrl test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TEXTCTRL_TEST_ACCESS_H
#define WX_WINUI_TEXTCTRL_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/textctrl.h"

#if wxUSE_TEXTCTRL
class WXDLLIMPEXP_CORE wxWinUITextCtrlTestAccess final
{
public:
    using CallbackHook = void (*)(wxTextCtrl*, void*);

    enum class ScrollBarVisibility
    {
        Disabled,
        Auto,
        Hidden,
        Visible
    };

    // Exercise the same production operations as the XAML KeyDown delegate.
    static bool ProcessEnter(wxTextCtrl& textCtrl);
    static bool ProcessTab(wxTextCtrl& textCtrl);
    static void PasteText(wxTextCtrl& textCtrl, const wxString& text);
    // Inject a complete content-change proposal into the shared transaction;
    // this deliberately does not synthesize or claim a PasswordChanging event.
    static bool InjectPasswordContentChange(wxTextCtrl& textCtrl,
                                            const wxString& proposed);
    static bool SetTextBoxPeerText(wxTextCtrl& textCtrl, const wxString& text);
    static bool GetPeerText(const wxTextCtrl& textCtrl, wxString *text);
    static bool ReplacePeerSelection(wxTextCtrl& textCtrl,
                                     const wxString& text);
    static bool HasLocalFontOverrides(const wxTextCtrl& textCtrl);
    static bool RichClipboardUsesAllFormats(const wxTextCtrl& textCtrl);
    static bool GetNativeCaretShown(const wxTextCtrl& textCtrl, bool *shown);
    static bool HasNoHideSelectionProjection(const wxTextCtrl& textCtrl);
    static bool GetPasswordPeerSecurity(const wxTextCtrl& textCtrl,
                                        bool *isPassword,
                                        bool *hasValuePattern,
                                        bool *hasTextPattern,
                                        wxString *documentText,
                                        bool *textPredictionEnabled);
    static void ForceNextPasswordScrubFailure(wxTextCtrl& textCtrl,
                                              long hresult);
    static void ForceNextPasswordScrubPartialWrite(wxTextCtrl& textCtrl);
    static bool GetPasswordFailClosed(const wxTextCtrl& textCtrl,
                                      long *hresult,
                                      bool *documentWasEmpty);
    static unsigned GetPasswordClipboardExportAttemptCount(
        const wxTextCtrl& textCtrl);
    static unsigned GetAutoUrlRangeCount(const wxTextCtrl& textCtrl);
    static bool GetAutoUrlRange(const wxTextCtrl& textCtrl,
                                unsigned n,
                                long *from,
                                long *to,
                                wxString *target);
    static unsigned GetAutoCompleteSuggestionCount(const wxTextCtrl& textCtrl);
    static wxString GetAutoCompleteSuggestion(const wxTextCtrl& textCtrl,
                                              unsigned n);
    static bool InvokeAutoCompleteSuggestion(wxTextCtrl& textCtrl, unsigned n);
    static bool GetPeerSelection(const wxTextCtrl& textCtrl,
                                 long *from,
                                 long *to);
    static bool GetScrollState(const wxTextCtrl& textCtrl,
                               double *horizontalOffset,
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
                               double *viewUnitYScale = nullptr);
    static bool GetVerticalScrollBarVisibility(const wxTextCtrl& textCtrl,
                                               ScrollBarVisibility *visibility);
    static bool GetPositionVisibilityState(const wxTextCtrl& textCtrl,
                                           bool *pending,
                                           unsigned *queuedRetries,
                                           bool *hasRetainedPeerReferences = nullptr,
                                           unsigned *richTrace = nullptr,
                                           unsigned *richPassCount = nullptr);
    static void UseTextBoxPeer(wxTextCtrl& textCtrl);
    static void ForceNextPositionVisibilityRetry(wxTextCtrl& textCtrl);
    // One-shot hooks are cleared before calling application code. The real
    // callback generation and peer identity must still match on return.
    static void SetNextTemporarySelectionHook(wxTextCtrl& textCtrl,
                                              CallbackHook hook,
                                              void *context);
    static void SetNextPasswordTextChangingHook(wxTextCtrl& textCtrl,
                                                CallbackHook hook,
                                                void *context);
    static void SetNextCreateLoadedHook(wxTextCtrl& textCtrl,
                                        CallbackHook hook,
                                        void *context);
};
#endif // wxUSE_TEXTCTRL

#endif // WX_WINUI_TEXTCTRL_TEST_ACCESS_H
