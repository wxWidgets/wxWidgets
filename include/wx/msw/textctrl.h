/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

#ifdef __GNUG__
    #pragma interface "textctrl.h"
#endif

#include "wx/setup.h"
#include "wx/control.h"

#if wxUSE_IOSTREAMH
    #include <iostream.h>
#else
    #include <iostream>
#endif

#if defined(__WIN95__) && !defined(__TWIN32__)
    #define wxUSE_RICHEDIT 1
#else
    #define wxUSE_RICHEDIT 0
#endif

WXDLLEXPORT_DATA(extern const wxChar*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxTextCtrl : public wxControl

// 16-bit Borland 4.0 doesn't seem to allow multiple inheritance with wxWindow and streambuf:
// it complains about deriving a huge class from the huge class streambuf. !!
// Also, can't use streambuf if making or using a DLL :-(

#if (defined(__BORLANDC__)) || defined(__MWERKS__) || defined(_WINDLL) || defined(WXUSINGDLL) || defined(WXMAKINGDLL)
#define NO_TEXT_WINDOW_STREAM
#endif

#ifndef NO_TEXT_WINDOW_STREAM
, public streambuf
#endif

{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl)

public:
    // creation
    // --------
    wxTextCtrl();
    wxTextCtrl(wxWindow *parent, wxWindowID id,
            const wxString& value = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxTextCtrlNameStr)
#ifndef NO_TEXT_WINDOW_STREAM
        :streambuf()
#endif
        {
            Create(parent, id, value, pos, size, style, validator, name);
        }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxString& value = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTE_PROCESS_TAB,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxTextCtrlNameStr);

    // accessors
    // ---------
    virtual wxString GetValue() const ;
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
    virtual long GetInsertionPoint() const ;
    virtual long GetLastPosition() const ;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // If the return values from and to are the same, there is no
    // selection.
    virtual void GetSelection(long* from, long* to) const;
    virtual bool IsEditable() const ;

    // streambuf implementation
#ifndef NO_TEXT_WINDOW_STREAM
    int overflow(int i);
    int sync();
    int underflow();
#endif

    wxTextCtrl& operator<<(const wxString& s);
    wxTextCtrl& operator<<(int i);
    wxTextCtrl& operator<<(long i);
    wxTextCtrl& operator<<(float f);
    wxTextCtrl& operator<<(double d);
    wxTextCtrl& operator<<(const char c);

    virtual bool LoadFile(const wxString& file);
    virtual bool SaveFile(const wxString& file);
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);
    virtual void DiscardEdits();
    virtual bool IsModified() const;

#if WXWIN_COMPATIBILITY
    inline bool Modified() const { return IsModified(); }
#endif

    virtual long XYToPosition(long x, long y) const ;
    virtual void PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition(long pos);
    virtual void Clear();

    // callbacks
    // ---------
    void OnDropFiles(wxDropFilesEvent& event);
    void OnChar(wxKeyEvent& event); // Process 'enter' if required

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

    // Implementation
    // --------------
    virtual void Command(wxCommandEvent& event);
    virtual bool MSWCommand(WXUINT param, WXWORD id);

#if wxUSE_RICHEDIT
    bool IsRich() const { return m_isRich; }
    void SetRichEdit(bool isRich) { m_isRich = isRich; }
#endif

    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam,
            WXLPARAM lParam);

    virtual void AdoptAttributesFromHWND();
    virtual void SetupColours();
    virtual long MSWGetDlgCode();

protected:
#if wxUSE_RICHEDIT
    bool      m_isRich; // Are we using rich text edit to implement this?
#endif

    wxString  m_fileName;

    virtual void DoSetSize(int x, int y,
            int width, int height,
            int sizeFlags = wxSIZE_AUTO);

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TEXTCTRL_H_
