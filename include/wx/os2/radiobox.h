/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     wxRadioBox class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxRadioBoxNameStr;

// List box item
class WXDLLEXPORT wxBitmap ;

class WXDLLEXPORT wxRadioBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioBox)
public:
  wxRadioBox();

  inline wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
  {
    Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
  }

  ~wxRadioBox();

  bool Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);

  virtual bool OS2Command(WXUINT param, WXWORD id);

  int FindString(const wxString& s) const;
  void SetSelection(int N);
  int GetSelection() const;
  wxString GetString(int N) const;

  void GetSize(int *x, int *y) const;
  void GetPosition(int *x, int *y) const;

  void SetLabel(const wxString& label);
  void SetLabel(int item, const wxString& label) ;
  wxString GetLabel(int item) const;
  wxString GetLabel() const;
  bool Show(bool show);
  void SetFocus();
  bool Enable(bool enable);
  void Enable(int item, bool enable);
  void Show(int item, bool show) ;
  inline void SetLabelFont(const wxFont& WXUNUSED(font)) {};
  inline void SetButtonFont(const wxFont& font) { SetFont(font); }

  virtual wxString GetStringSelection() const;
  virtual bool SetStringSelection(const wxString& s);
  inline virtual int Number() const { return m_noItems; } ;
  void Command(wxCommandEvent& event);

  inline int GetNumberOfRowsOrCols() const { return m_noRowsOrCols; }
  inline void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

protected:
  void SubclassRadioButton(WXHWND hWndBtn);

  WXHWND *          m_radioButtons;
  int               m_majorDim ;
  int *             m_radioWidth;  // for bitmaps
  int *             m_radioHeight;

  int               m_noItems;
  int               m_noRowsOrCols;
  int               m_selectedButton;

  virtual void DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags = wxSIZE_AUTO);
};

#endif
    // _WX_RADIOBOX_H_
