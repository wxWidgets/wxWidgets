/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

#ifdef __GNUG__
    #pragma interface "textctrl.h"
#endif

WXDLLEXPORT_DATA(extern const char*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxTextCtrl : public wxTextCtrlBase
{
    DECLARE_DYNAMIC_CLASS(wxTextCtrl)

public:
    // creation
    // --------
    wxTextCtrl();
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr)
        {
            Create(parent, id, value, pos, size, style, validator, name);
        }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    // accessors
    // ---------
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    // operations
    // ----------

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);
    // If the return values from and to are the same, there is no
    // selection.
    virtual void GetSelection(long* from, long* to) const;
    virtual bool IsEditable() const ;

    virtual bool LoadFile(const wxString& file);
    virtual bool SaveFile(const wxString& file);
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);
    virtual void DiscardEdits();
    virtual bool IsModified() const;

    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;
    virtual void ShowPosition(long pos);
    virtual void Clear();

    // callbacks
    // ---------
    void OnDropFiles(wxDropFilesEvent& event);
    void OnChar(wxKeyEvent& event);
    //  void OnEraseBackground(wxEraseEvent& event);

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

    virtual void Command(wxCommandEvent& event);

    // implementation from here to the end
    // -----------------------------------
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    void SetModified(bool mod) { m_modified = mod; }
    virtual WXWidget GetTopWidget() const;

    // send the CHAR and TEXT_UPDATED events
    void DoSendEvents(void /* XmTextVerifyCallbackStruct */ *cbs,
                      long keycode);

protected:
    wxString  m_fileName;

public:
    // Motif-specific
    void*     m_tempCallbackStruct;
    bool      m_modified;
    wxString  m_value;            // Required for password text controls

    // Did we call wxTextCtrl::OnChar? If so, generate a command event.
    bool      m_processedDefault;

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TEXTCTRL_H_
