/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     wxRadioBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __RADIOBOXH__
#define __RADIOBOXH__

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

  inline wxRadioBox(wxWindow *parent, const wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             const int n = 0, const wxString choices[] = NULL,
             const int majorDim = 0, const long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
  {
    Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
  }

/*
  wxRadioBox(wxWindow *parent, const wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             const int n = 0, const wxBitmap *choices[] = NULL,
             const int majorDim = 0, const long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
  {
    Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
  }
*/

  ~wxRadioBox(void);

  bool Create(wxWindow *parent, const wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             const int n = 0, const wxString choices[] = NULL,
             const int majorDim = 0, const long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);

/*
  bool Create(wxWindow *parent, const wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             const int n = 0, const wxBitmap *choices[] = NULL,
             const int majorDim = 0, const long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);
*/

  virtual bool MSWCommand(const WXUINT param, const WXWORD id);
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  int FindString(const wxString& s) const;
  void SetSelection(const int N);
  int GetSelection(void) const;
  wxString GetString(const int N) const;
  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  void GetSize(int *x, int *y) const;
  void GetPosition(int *x, int *y) const;
  wxString GetLabel(void) const;
  void SetLabel(const wxString& label);
  void SetLabel(const int item, const wxString& label) ;
  void SetLabel(const int item, wxBitmap *bitmap) ;
  wxString GetLabel(const int item) const;
  bool Show(const bool show);
  void SetFocus(void);
  void Enable(const bool enable);
  void Enable(const int item, const bool enable);
  void Show(const int item, const bool show) ;
  inline void SetLabelFont(const wxFont& WXUNUSED(font)) {};
  inline void SetButtonFont(const wxFont& font) { SetFont(font); }

  virtual wxString GetStringSelection(void) const;
  virtual bool SetStringSelection(const wxString& s);
  inline virtual int Number(void) const { return m_noItems; } ;
  void Command(wxCommandEvent& event);
  
  inline int GetNumberOfRowsOrCols(void) const { return m_noRowsOrCols; }
  inline void SetNumberOfRowsOrCols(const int n) { m_noRowsOrCols = n; }

  // Implementation
  inline WXHWND *GetRadioButtons(void) const { return m_radioButtons; }
  bool ContainsHWND(WXHWND hWnd) const ;

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
    // __RADIOBOXH__
