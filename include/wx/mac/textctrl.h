/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrl class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

#ifdef __GNUG__
#pragma interface "textctrl.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxTextCtrl: public wxTextCtrlBase
{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl)
    
public:
  // creation
  // --------
  wxTextCtrl();
  inline wxTextCtrl(wxWindow *parent, wxWindowID id,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
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

  // clears the dirty flag
  virtual void DiscardEdits();

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
  virtual long GetLastPosition() const;

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
    
protected:
  virtual wxSize DoGetBestSize() const;
  
  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TEXTCTRL_H_
