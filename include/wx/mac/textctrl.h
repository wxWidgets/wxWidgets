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

#if wxUSE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

WXDLLEXPORT_DATA(extern const char*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxTextCtrl: public wxControl

// TODO Some platforms/compilers don't like inheritance from streambuf.

#if (defined(__BORLANDC__) && !defined(__WIN32__)) || defined(__MWERKS__)
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
  inline wxTextCtrl(wxWindow *parent, wxWindowID id,
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

  // operations
  // ----------
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  
  // Clipboard operations
  virtual void Copy();
  virtual void Cut();
  virtual void Paste();
  
  virtual void SetInsertionPoint(long pos);
  virtual void SetInsertionPointEnd();
  virtual long GetInsertionPoint() const ;
  virtual long GetLastPosition() const ;
  virtual void Replace(long from, long to, const wxString& value);
  virtual void Remove(long from, long to);
  virtual void SetSelection(long from, long to);
  virtual void SetEditable(bool editable);
  
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
  
  virtual long XYToPosition(long x, long y) const ;
  virtual void PositionToXY(long pos, long *x, long *y) const ;
  virtual void ShowPosition(long pos);
  virtual void Clear();
	virtual bool MacCanFocus() const { return true ; }
  
  // callbacks
  // ---------
  void OnDropFiles(wxDropFilesEvent& event);
	void OnChar(wxKeyEvent& event); // Process 'enter' if required
//  void OnEraseBackground(wxEraseEvent& event);
  
  // Implementation
  // --------------
  virtual void Command(wxCommandEvent& event);

protected:
  wxString  m_fileName;
  
  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TEXTCTRL_H_
