/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTCTRLH__
#define __TEXTCTRLH__

#ifdef __GNUG__
#pragma interface "textctrl.h"
#endif

#include "wx/control.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

WXDLLEXPORT_DATA(extern const char*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxTextCtrl: public wxControl

// 16-bit Borland 4.0 doesn't seem to allow multiple inheritance with wxWindow and streambuf:
// it complains about deriving a huge class from the huge class streambuf. !!
// Also, can't use streambuf if making or using a DLL :-(

#if (defined(__BORLANDC__) && !defined(__WIN32__)) || defined(__MWERKS__) || defined(_WINDLL) || defined(WXUSINGDLL) || defined(WXMAKINGDLL)
#define NO_TEXT_WINDOW_STREAM
#endif

#ifndef NO_TEXT_WINDOW_STREAM
, public streambuf
#endif

{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl)

 protected:
  wxString fileName;
 public:
  wxTextCtrl(void);
  inline wxTextCtrl(wxWindow *parent, const wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxTextCtrlNameStr)
#ifndef NO_TEXT_WINDOW_STREAM
 :streambuf()
#endif
  {
      Create(parent, id, value, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxTextCtrlNameStr);

  virtual wxString GetValue(void) const ;
  virtual void SetValue(const wxString& value);
  virtual void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);

  // Clipboard operations
  virtual void Copy(void);
  virtual void Cut(void);
  virtual void Paste(void);

  virtual void SetInsertionPoint(const long pos);
  virtual void SetInsertionPointEnd(void);
  virtual long GetInsertionPoint(void) const ;
  virtual long GetLastPosition(void) const ;
  virtual void Replace(const long from, const long to, const wxString& value);
  virtual void Remove(const long from, const long to);
  virtual void SetSelection(const long from, const long to);

  virtual void Command(wxCommandEvent& event);

  virtual void SetEditable(const bool editable);

#ifndef NO_TEXT_WINDOW_STREAM
  int overflow(int i);
  int sync(void);
  int underflow(void);
#endif

  void OnDropFiles(wxDropFilesEvent& event);

  wxTextCtrl& operator<<(const wxString& s);
  wxTextCtrl& operator<<(const int i);
  wxTextCtrl& operator<<(const long i);
  wxTextCtrl& operator<<(const float f);
  wxTextCtrl& operator<<(const double d);
  wxTextCtrl& operator<<(const char c);

  virtual bool LoadFile(const wxString& file);
  virtual bool SaveFile(const wxString& file);
  virtual void WriteText(const wxString& text);
  virtual void DiscardEdits(void);
  virtual bool IsModified(void) const;

#if WXWIN_COMPATIBILITY
  inline bool Modified(void) const { return IsModified(); }
#endif

  virtual long XYToPosition(const long x, const long y) const ;
  virtual void PositionToXY(const long pos, long *x, long *y) const ;
  virtual void ShowPosition(const long pos);
  virtual int GetLineLength(const long lineNo) const ;
  virtual wxString GetLineText(const long lineNo) const ;
  virtual int GetNumberOfLines(void) const ;
  virtual void Clear(void);

  // Process special keys e.g. 'enter' and process as if it were a command, if required
  void OnChar(wxKeyEvent& event);

  void OnEraseBackground(wxEraseEvent& event);

  // Implementation
  virtual bool MSWCommand(const WXUINT param, const WXWORD id);
  inline bool IsRich(void) { return m_isRich; }
  inline void SetRichEdit(const bool isRich) { m_isRich = isRich; }
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
  virtual void AdoptAttributesFromHWND(void);
  virtual void SetupColours(void);

protected:
   bool      m_isRich; // Are we using rich text edit to implement this?

DECLARE_EVENT_TABLE()
};

#endif
    // __TEXTCTRLH__
