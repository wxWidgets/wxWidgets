/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/textctrl.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TEXTCTRL_H_
#define _WX_GTK_TEXTCTRL_H_

typedef struct _GtkTextMark GtkTextMark;
typedef struct _GTKGetTextBuffer GTKGetTextBuffer;
typedef struct _GTKGetEditable GTKGetEditable;

//-----------------------------------------------------------------------------
// wxTextCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextCtrl: public wxTextCtrlBase
{
public:
    wxTextCtrl() { Init(); }
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString &value = wxEmptyString,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxASCII_STR(wxTextCtrlNameStr));

    virtual ~wxTextCtrl();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &value = wxEmptyString,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxASCII_STR(wxTextCtrlNameStr));

    // implement base class pure virtuals
    // ----------------------------------

    virtual void WriteText(const wxString& text) override;
    virtual wxString GetValue() const override;
    virtual bool IsEmpty() const;

    virtual int GetLineLength(long lineNo) const override;
    virtual wxString GetLineText(long lineNo) const override;
    virtual int GetNumberOfLines() const override;

    virtual bool IsModified() const override;
    virtual bool IsEditable() const override;

    virtual void GetSelection(long* from, long* to) const override;

    virtual void Remove(long from, long to) override;

    virtual void MarkDirty() override;
    virtual void DiscardEdits() override;

    virtual bool SetStyle(long start, long end, const wxTextAttr& style) override;
    virtual bool GetStyle(long position, wxTextAttr& style) override;

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

    // Clipboard operations
    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    // Insertion point
    virtual void SetInsertionPoint(long pos) override;
    virtual long GetInsertionPoint() const override;
    virtual wxTextPos GetLastPosition() const override;

    virtual void SetSelection(long from, long to) override;
    virtual void SetEditable(bool editable) override;

    // Overridden wxWindow methods
    virtual void SetWindowStyleFlag( long style ) override;

#if wxUSE_SPELLCHECK && defined(__WXGTK3__)
    // Use native spelling and grammar checking functions.
    virtual bool EnableProofCheck(const wxTextProofOptions& options
                                    = wxTextProofOptions::Default()) override;
    virtual wxTextProofOptions GetProofCheckOptions() const override;
#endif // wxUSE_SPELLCHECK && __WXGTK3__

    // GTK-specific functions

    // Get the underlying text buffer for multi-line controls.
    GtkTextBuffer *GTKGetTextBuffer() { return IsMultiLine() ? m_buffer : nullptr; }
    // Get the underlying text control.
    GtkEditable *GTKGetEditable() const { return GetEditable(); }


    // Implementation from now on
    void OnDropFiles( wxDropFilesEvent &event );
    void OnChar( wxKeyEvent &event );

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);

    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    GtkWidget* GetConnectWidget() override;

    void SetUpdateFont(bool WXUNUSED(update)) { }

    // implementation only from now on

    // tell the control to ignore next text changed signal
    void IgnoreNextTextUpdate(int n = 1) { m_countUpdatesToIgnore = n; }

    // should we ignore the changed signal? always resets the flag
    bool IgnoreTextUpdate();

    // call this to indicate that the control is about to be changed
    // programmatically and so m_modified flag shouldn't be set
    void DontMarkDirtyOnNextChange() { m_dontMarkDirty = true; }

    // should we mark the control as dirty? always resets the flag
    bool MarkDirtyOnChange();

    // always let GTK have mouse release events for multiline controls
    virtual bool GTKProcessEvent(wxEvent& event) const override;


    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    void GTKOnTextChanged() override;
    void GTKAfterLayout();

protected:
    // overridden wxWindow virtual methods
    virtual wxSize DoGetBestSize() const override;
    virtual void DoApplyWidgetStyle(GtkRcStyle *style) override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    virtual wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;

    virtual void DoFreeze() override;
    virtual void DoThaw() override;

    virtual void DoEnable(bool enable) override;

    // Widgets that use the style->base colour for the BG colour should
    // override this and return true.
    virtual bool UseGTKStyleBase() const override { return true; }

    virtual wxString DoGetValue() const override;

    // Override this to use either GtkEntry or GtkTextView IME depending on the
    // kind of control we are.
    virtual int GTKIMFilterKeypress(GdkEventKey* event) const override;

    virtual wxPoint DoPositionToCoords(long pos) const override;

    // wrappers hiding the differences between functions doing the same thing
    // for GtkTextView and GtkEntry (all of them use current window style to
    // set the given characteristic)
    void GTKSetEditable();
    void GTKSetVisibility();
    void GTKSetActivatesDefault();
    void GTKSetWrapMode();
    void GTKSetJustification();

private:
    void Init();

    // overridden wxTextEntry virtual methods
    virtual GtkEditable *GetEditable() const override;
    virtual GtkEntry *GetEntry() const override;

    // change the font for everything in this control
    void ChangeFontGlobally();

    // get the encoding which is used in this control: this looks at our font
    // and default style but not the current style (i.e. the style for the
    // current position); returns wxFONTENCODING_SYSTEM if we have no specific
    // encoding
    wxFontEncoding GetTextEncoding() const;

    // returns either m_text or m_buffer depending on whether the control is
    // single- or multi-line; convenient for the GTK+ functions which work with
    // both
    void *GetTextObject() const override
    {
        return IsMultiLine() ? static_cast<void *>(m_buffer)
                             : static_cast<void *>(m_text);
    }


    // the widget used for single line controls
    GtkWidget  *m_text;

    bool m_modified;
    bool m_dontMarkDirty;

    int         m_countUpdatesToIgnore;

    // Our text buffer. Convenient, and holds the buffer while using
    // a dummy one when frozen
    GtkTextBuffer *m_buffer;

    GtkTextMark* m_showPositionDefer;
    GSList* m_anonymousMarkList;
    unsigned m_afterLayoutId;

    // For wxTE_AUTO_URL
    void OnUrlMouseEvent(wxMouseEvent&);

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxTextCtrl);
};

#endif // _WX_GTK_TEXTCTRL_H_
