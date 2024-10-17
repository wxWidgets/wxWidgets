/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/textctrl.h
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TEXTCTRL_H_
#define _WX_QT_TEXTCTRL_H_

class wxQtEdit;

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
    wxTextCtrl() = default;

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

    virtual int GetLineLength(long lineNo) const override;
    virtual wxString GetLineText(long lineNo) const override;
    virtual int GetNumberOfLines() const override;

    virtual bool IsModified() const override;
    virtual void MarkDirty() override;
    virtual void DiscardEdits() override;

    virtual bool SetStyle(long start, long end, const wxTextAttr& style) override;
    virtual bool GetStyle(long position, wxTextAttr& style) override;
    virtual bool SetDefaultStyle(const wxTextAttr& style) override;

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

    virtual void SetInsertionPoint(long pos) override;
    virtual long GetInsertionPoint() const override;
    virtual void SetSelection( long from, long to ) override;
    virtual void GetSelection(long *from, long *to) const override;

    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    virtual void Undo() override;
    virtual void Redo() override;
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    virtual void EmptyUndoBuffer() override;

    virtual bool IsEditable() const override;
    virtual void SetEditable(bool editable) override;

    virtual wxString DoGetValue() const override;
    virtual void DoSetValue(const wxString &text, int flags = 0) override;
    virtual void WriteText(const wxString& text) override;

    virtual void SetMaxLength(unsigned long len) override;

protected:
    virtual wxSize DoGetBestSize() const override;

    virtual bool DoLoadFile(const wxString& file, int fileType) override;
    virtual bool DoSaveFile(const wxString& file, int fileType) override;

    // From wxTextEntry:
    virtual wxWindow *GetEditableWindow() override { return this; }

private:
    wxQtEdit *m_qtEdit = nullptr;

    wxDECLARE_DYNAMIC_CLASS( wxTextCtrl );
};

#endif // _WX_QT_TEXTCTRL_H_
