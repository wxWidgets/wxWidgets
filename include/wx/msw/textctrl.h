/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
    // creation
    // --------

    wxTextCtrl();
    wxTextCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxTextCtrlNameStr));
    virtual ~wxTextCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxTextCtrlNameStr));

    // overridden wxTextEntry methods
    // ------------------------------

    virtual wxString GetValue() const override;
    virtual wxString GetRange(long from, long to) const override;

    virtual bool IsEmpty() const;

    virtual void WriteText(const wxString& text) override;
    virtual void AppendText(const wxString& text) override;
    virtual void Clear() override;

    virtual int GetLineLength(long lineNo) const override;
    virtual wxString GetLineText(long lineNo) const override;
    virtual int GetNumberOfLines() const override;

    virtual void SetMaxLength(unsigned long len) override;

    virtual void GetSelection(long *from, long *to) const override;

    virtual void Paste() override;

    virtual void Redo() override;
    virtual bool CanRedo() const override;
#if wxUSE_RICHEDIT
    virtual void EmptyUndoBuffer() override;
#endif // wxUSE_RICHEDIT

    virtual void SetInsertionPointEnd() override;
    virtual long GetInsertionPoint() const override;
    virtual wxTextPos GetLastPosition() const override;

    // implement base class pure virtuals
    // ----------------------------------

    virtual bool IsModified() const override;
    virtual void MarkDirty() override;
    virtual void DiscardEdits() override;

    virtual bool EmulateKeyPress(const wxKeyEvent& event) override;

#if wxUSE_RICHEDIT
    // apply text attribute to the range of text (only works with richedit
    // controls)
    virtual bool SetStyle(long start, long end, const wxTextAttr& style) override;
    virtual bool SetDefaultStyle(const wxTextAttr& style) override;
    virtual bool GetStyle(long position, wxTextAttr& style) override;
#endif // wxUSE_RICHEDIT

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const override;
    virtual bool PositionToXY(long pos, long *x, long *y) const override;

    virtual void ShowPosition(long pos) override;
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt, long *pos) const override;
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                            wxTextCoord *col,
                                            wxTextCoord *row) const override
    {
        return wxTextCtrlBase::HitTest(pt, col, row);
    }

    virtual void SetLayoutDirection(wxLayoutDirection dir) override;
    virtual wxLayoutDirection GetLayoutDirection() const override;

    // Caret handling (Windows only)
    bool ShowNativeCaret(bool show = true);
    bool HideNativeCaret() { return ShowNativeCaret(false); }

#if wxUSE_RICHEDIT && wxUSE_SPELLCHECK
    // Use native spelling and grammar checking functions.
    // This is only available in wxTE_RICH2 controls.
    virtual bool EnableProofCheck(const wxTextProofOptions& options
                                    = wxTextProofOptions::Default()) override;
    virtual wxTextProofOptions GetProofCheckOptions() const override;
#endif // wxUSE_RICHEDIT && wxUSE_SPELLCHECK

    // Implementation from now on
    // --------------------------

#if wxUSE_DRAG_AND_DROP && wxUSE_RICHEDIT
    virtual void SetDropTarget(wxDropTarget *dropTarget) override;
#endif // wxUSE_DRAG_AND_DROP && wxUSE_RICHEDIT

    virtual void SetWindowStyleFlag(long style) override;

    virtual void Command(wxCommandEvent& event) override;
    virtual bool MSWCommand(WXUINT param, WXWORD id) override;
    virtual WXHBRUSH MSWControlColor(WXHDC hDC, WXHWND hWnd) override;

#if wxUSE_RICHEDIT
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    int GetRichVersion() const { return m_verRichEdit; }
    bool IsRich() const { return m_verRichEdit != 0; }

    // rich edit controls are not compatible with normal ones and we must set
    // the colours and font for them otherwise
    virtual bool SetBackgroundColour(const wxColour& colour) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;
    virtual bool SetFont(const wxFont& font) override;
#else
    bool IsRich() const { return false; }
#endif // wxUSE_RICHEDIT

#if wxUSE_INKEDIT && wxUSE_RICHEDIT
    bool IsInkEdit() const { return m_isInkEdit != 0; }
#else
    bool IsInkEdit() const { return false; }
#endif

    virtual void AdoptAttributesFromHWND() override;

    virtual bool AcceptsFocusFromKeyboard() const override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override;

    // callbacks
    void OnDropFiles(wxDropFilesEvent& event);
    void OnChar(wxKeyEvent& event); // Process 'enter' if required

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnUpdateDelete(wxUpdateUIEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);

    // Show a context menu for Rich Edit controls (the standard
    // EDIT control has one already)
    void OnContextMenu(wxContextMenuEvent& event);

#if wxUSE_RICHEDIT
    // Create context menu for RICHEDIT controls. This may be called once during
    // the control's lifetime or every time the menu is shown, depending on
    // implementation.
    virtual wxMenu *MSWCreateContextMenu();
#endif // wxUSE_RICHEDIT

    // be sure the caret remains invisible if the user
    // called HideNativeCaret() before
    void OnSetFocus(wxFocusEvent& event);

    // intercept WM_GETDLGCODE
    virtual bool MSWHandleMessage(WXLRESULT *result,
                                  WXUINT message,
                                  WXWPARAM wParam,
                                  WXLPARAM lParam) override;

    virtual bool MSWShouldPreProcessMessage(WXMSG* pMsg) override;
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

protected:
    // common part of all ctors
    void Init();

    // creates the control of appropriate class (plain or rich edit) with the
    // styles corresponding to m_windowStyle
    //
    // this is used by ctor/Create() and when we need to recreate the control
    // later
    bool MSWCreateText(const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size);

    virtual void DoSetValue(const wxString &value, int flags = 0) override;

    virtual wxPoint DoPositionToCoords(long pos) const override;

    // return true if this control has a user-set limit on amount of text (i.e.
    // the limit is due to a previous call to SetMaxLength() and not built in)
    bool HasSpaceLimit(unsigned int *len) const;

    // replace the contents of the selection or of the entire control with the
    // given text
    void DoWriteText(const wxString& text,
                     int flags = SetValue_SendEvent | SetValue_SelectionOnly);

    // set the selection (possibly without scrolling the caret into view)
    void DoSetSelection(long from, long to, int flags) override;

    // get the length of the line containing the character at the given
    // position
    long GetLengthOfLineContainingPos(long pos) const;

    // send TEXT_UPDATED event, return true if it was handled, false otherwise
    bool SendUpdateEvent();

    virtual wxSize DoGetBestSize() const override;
    virtual wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;

    virtual void DoMoveWindow(int x, int y, int width, int height) override;

#if wxUSE_RICHEDIT
    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;

    // Apply m_richDPIscale zoom to rich control.
    void MSWSetRichZoom();

    // Apply the character-related parts of wxTextAttr to the given selection
    // or the entire control if start == end == -1.
    //
    // This function is private and should only be called for rich edit
    // controls and with (from, to) already in correct order, i.e. from <= to.
    bool MSWSetCharFormat(const wxTextAttr& attr, long from = -1, long to = -1);

    // Same as above for paragraph-related parts of wxTextAttr. Note that this
    // can only be applied to the selection as RichEdit doesn't support setting
    // the paragraph styles globally.
    bool MSWSetParaFormat(const wxTextAttr& attr, long from, long to);

    // Send wxEVT_CONTEXT_MENU event from here if the control doesn't do it on
    // its own.
    void OnRightUp(wxMouseEvent& event);

    // we're using RICHEDIT (and not simple EDIT) control if this field is not
    // 0, it also gives the version of the RICHEDIT control being used
    // (although not directly: 1 is for 1.0, 2 is for either 2.0 or 3.0 as we
    // can't nor really need to distinguish between them and 4 is for 4.1)
    int m_verRichEdit;

    // Rich text controls need temporary scaling when they are created on a
    // display with non-system DPI.
    float m_richDPIscale;
#endif // wxUSE_RICHEDIT

    // number of EN_UPDATE events sent by Windows when we change the controls
    // text ourselves: we want this to be exactly 1
    int m_updatesCount;

private:
    virtual void EnableTextChangedEvents(bool enable) override
    {
        m_updatesCount = enable ? -1 : -2;
    }

    // implement wxTextEntry pure virtual: it implements all the operations for
    // the simple EDIT controls
    virtual WXHWND GetEditHWND() const override { return m_hWnd; }

#if wxUSE_OLE
    virtual void MSWProcessSpecialKey(wxKeyEvent& event) override;
#endif // wxUSE_OLE

    // Do we need to handle Ctrl+Backspace ourselves?
    bool MSWNeedsToHandleCtrlBackspace() const;

    // Delete backwards until the start of the previous word before caret in a
    // way compatible with the standard MSW Ctrl+Backspace shortcut.
    void MSWDeleteWordBack();

    void OnKeyDown(wxKeyEvent& event);

    // Used by EN_MAXTEXT handler to increase the size limit (will do nothing
    // if the current limit is big enough). Should never be called directly.
    //
    // Returns true if we increased the limit to allow entering more text,
    // false if we hit the limit set by SetMaxLength() and so didn't change it.
    bool AdjustSpaceLimit();

    // Called before pasting to ensure that the limit is at big enough to allow
    // pasting the entire text on the clipboard.
    void AdjustMaxLengthBeforePaste();


    wxMenu* m_privateContextMenu;

    bool m_isNativeCaretShown;

#if wxUSE_INKEDIT && wxUSE_RICHEDIT
    int  m_isInkEdit;
#endif

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTextCtrl);
};

#endif // _WX_TEXTCTRL_H_
