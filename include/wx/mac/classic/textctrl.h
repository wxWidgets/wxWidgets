/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar) wxTextCtrlNameStr[];

// Single-line text item
class WXDLLEXPORT wxTextCtrl: public wxTextCtrlBase
{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl)

public:
  // creation
  // --------
  wxTextCtrl() { Init(); }
  virtual ~wxTextCtrl();
  wxTextCtrl(wxWindow *parent, wxWindowID id,
             const wxString& value = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxTextCtrlNameStr)
  {
      Init();

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
  virtual wxString GetValue() const ;
  virtual void SetValue(const wxString& value);

  virtual int GetLineLength(long lineNo) const;
  virtual wxString GetLineText(long lineNo) const;
  virtual int GetNumberOfLines() const;

  virtual bool IsModified() const;
  virtual bool IsEditable() const;

  // If the return values from and to are the same, there is no selection.
  virtual void GetSelection(long* from, long* to) const;

  // operations
  // ----------

  // editing

  virtual void Clear();
  virtual void Replace(long from, long to, const wxString& value);
  virtual void Remove(long from, long to);

  // load the controls contents from the file
  virtual bool LoadFile(const wxString& file);

  // sets/clears the dirty flag
  virtual void MarkDirty();
  virtual void DiscardEdits();

  // set the max number of characters which may be entered in a single line
  // text control
  virtual void SetMaxLength(unsigned long len) ;

  // text control under some platforms supports the text styles: these
  // methods allow to apply the given text style to the given selection or to
  // set/get the style which will be used for all appended text
  virtual bool SetStyle(long start, long end, const wxTextAttr& style);
  virtual bool SetDefaultStyle(const wxTextAttr& style);

  // writing text inserts it at the current position, appending always
  // inserts it at the end
  virtual void WriteText(const wxString& text);
  virtual void AppendText(const wxString& text);

  // translate between the position (which is just an index in the text ctrl
  // considering all its contents as a single strings) and (x, y) coordinates
  // which represent column and line.
  virtual long XYToPosition(long x, long y) const;
  virtual bool PositionToXY(long pos, long *x, long *y) const;

  virtual void ShowPosition(long pos);

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

  // Insertion point
  virtual void SetInsertionPoint(long pos);
  virtual void SetInsertionPointEnd();
  virtual long GetInsertionPoint() const;
  virtual wxTextPos GetLastPosition() const;

  virtual void SetSelection(long from, long to);
  virtual void SetEditable(bool editable);

    // Implementation from now on
    // --------------------------

    // Implementation
    // --------------
    virtual void Command(wxCommandEvent& event);

    virtual bool AcceptsFocus() const;

    // callbacks
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

       virtual bool MacCanFocus() const { return true ; }
    virtual bool MacSetupCursor( const wxPoint& pt ) ;

    virtual void      MacSuperShown( bool show ) ;
    virtual bool     Show(bool show = true) ;

protected:
    // common part of all ctors
    void Init();

  virtual wxSize DoGetBestSize() const;

  bool  m_editable ;

  // flag is set to true when the user edits the controls contents
  bool m_dirty;

  // one of the following objects is used for representation, the other one is NULL
  void*  m_macTE ;
  void*  m_macTXN ;
  void*  m_macTXNvars ;
  bool  m_macUsesTXN ;
  unsigned long  m_maxLength ;

  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TEXTCTRL_H_
