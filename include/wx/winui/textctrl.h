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

#include <memory>

class wxWinUITextCtrlImpl;

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
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
    bool IsEmpty() const;
    void WriteText(const wxString& text) override;
    void AppendText(const wxString& text) override;
    void Remove(long from, long to) override;
    void Clear() override;

    void Copy() override;
    void Cut() override;
    void Paste() override;
    void Undo() override;
    void Redo() override;
    bool CanUndo() const override;
    bool CanRedo() const override;

    void SetInsertionPoint(long pos) override;
    void SetInsertionPointEnd() override;
    long GetInsertionPoint() const override;
    wxTextPos GetLastPosition() const override;
    void SetSelection(long from, long to) override;
    void GetSelection(long *from, long *to) const override;

    bool IsEditable() const override;
    void SetEditable(bool editable) override;
    void SetMaxLength(unsigned long len) override;

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
    wxPoint DoPositionToCoords(long pos) const override;
    wxSize DoGetBestSize() const override;

private:
    void ApplyValueToPeer();
    void UpdateWinUIAppearance();
    void ApplyToolTip();
    void SendTextEvent();
    void ClampInsertionPoint();
    wxArrayString GetLines() const;
    WXHWND GetEditHWND() const override;

    std::unique_ptr<wxWinUITextCtrlImpl> m_winui;
    wxString m_value;
    long m_insertionPoint;
    long m_selectionStart;
    long m_selectionEnd;
    unsigned long m_maxLength;
    bool m_modified;
    bool m_editable;
    bool m_updatingPeer;
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTextCtrl);
};

#endif // _WX_WINUI_TEXTCTRL_H_
