/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     wxRadioBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
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

  int FindString(const wxString& s) const;
  void SetSelection(int N);
  int GetSelection() const;
  wxString GetString(int N) const;
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetLabel(const wxString& label) { wxControl::SetLabel(label); };
  void SetLabel(int item, const wxString& label) ;
  wxString GetLabel(int item) const;
  wxString GetLabel() const { return wxControl::GetLabel(); };
  void Enable(bool enable);
  void Enable(int item, bool enable);
  void Show(int item, bool show) ;
  bool Show(bool show) { return wxControl::Show(show); };

  virtual wxString GetStringSelection() const;
  virtual bool SetStringSelection(const wxString& s);
  inline virtual int Number() const { return m_noItems; } ;
  void Command(wxCommandEvent& event);
  
  inline int GetNumberOfRowsOrCols() const { return m_noRowsOrCols; }
  inline void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

  WXWidget GetTopWidget() const { return m_formWidget; }
  WXWidget GetLabelWidget() const { return m_labelWidget; }
  inline WXWidget* GetRadioButtons() const { return m_radioButtons; }
  inline void SetSel(int i) { m_selectedButton = i; }

protected:
  int               m_majorDim ;
  int               m_noItems;
  int               m_noRowsOrCols;
  int               m_selectedButton;

  WXWidget          m_formWidget;
  WXWidget          m_labelWidget;
  WXWidget*         m_radioButtons;
  wxString*         m_radioButtonLabels;
};

#endif
    // _WX_RADIOBOX_H_
