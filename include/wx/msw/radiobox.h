/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     wxRadioBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#ifdef __GNUG__
#pragma interface "radiobox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxRadioBoxNameStr;

// List box item
class WXDLLEXPORT wxBitmap ;

class WXDLLEXPORT wxRadioBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioBox)
public:
  wxRadioBox(void);

#if WXWIN_COMPATIBILITY
  wxRadioBox(wxWindow *parent, wxFunction func, const char *title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int n = 0, char **choices = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL, const char *name = wxRadioBoxNameStr);

/*
  inline wxRadioBox(wxWindow *parent, wxFunction func, const char *title,
             int x, int y, int width, int height,
             int n, wxBitmap **choices,
             int majorDim = 0, long style = wxRA_HORIZONTAL, const char *name = wxRadioBoxNameStr)
  {
    Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), n, (const wxBitmap **)choices, majorDim, style,
        wxDefaultValidator, name);
    Callback(func);
  }
*/

#endif

  inline wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
  {
    Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
  }

/*
  wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxBitmap *choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
  {
    Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
  }
*/

  ~wxRadioBox(void);

  bool Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);

/*
  bool Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxBitmap *choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);
*/

  virtual bool MSWCommand(WXUINT param, WXWORD id);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  int FindString(const wxString& s) const;
  void SetSelection(int N);
  int GetSelection(void) const;
  wxString GetString(int N) const;

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
    { wxWindow::SetSize(rect, sizeFlags); }
  void SetSize(const wxSize& size) { wxWindow::SetSize(size); }

  void GetSize(int *x, int *y) const;
  wxSize GetSize() const { return wxWindow::GetSize(); }

  void GetPosition(int *x, int *y) const;
  wxPoint GetPosition() const { return wxWindow::GetPosition(); }

  wxString GetLabel(void) const;
  void SetLabel(const wxString& label);
  void SetLabel(int item, const wxString& label) ;
  void SetLabel(int item, wxBitmap *bitmap) ;
  wxString GetLabel(int item) const;
  bool Show(bool show);
  void SetFocus(void);
  void Enable(bool enable);
  void Enable(int item, bool enable);
  void Show(int item, bool show) ;
  inline void SetLabelFont(const wxFont& WXUNUSED(font)) {};
  inline void SetButtonFont(const wxFont& font) { SetFont(font); }

  virtual wxString GetStringSelection(void) const;
  virtual bool SetStringSelection(const wxString& s);
  inline virtual int Number(void) const { return m_noItems; } ;
  void Command(wxCommandEvent& event);
  
  inline int GetNumberOfRowsOrCols(void) const { return m_noRowsOrCols; }
  inline void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

  // Implementation
  inline WXHWND *GetRadioButtons(void) const { return m_radioButtons; }
  bool ContainsHWND(WXHWND hWnd) const ;

  long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
  WXHWND *          m_radioButtons;
  int               m_majorDim ;
  int *             m_radioWidth ;  // for bitmaps
  int *             m_radioHeight ;

  int               m_noItems;
  int               m_noRowsOrCols;
  int               m_selectedButton;

};

#endif
    // _WX_RADIOBOX_H_
