/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

#ifdef __GNUG__
#pragma interface "choice.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxChoiceNameStr;

// Choice item
class WXDLLEXPORT wxChoice: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxChoice)

 public:
  inline wxChoice(void) { m_noStrings = 0; }

  inline wxChoice(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr)
  {
    Create(parent, id, pos, size, n, choices, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr);

  virtual void Append(const wxString& item);
  virtual void Delete(int n);
  virtual void Clear(void);
  virtual int GetSelection(void) const ;
  virtual void SetSelection(int n);
  virtual int FindString(const wxString& s) const;
  virtual wxString GetString(int n) const ;
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
    { wxWindow::SetSize(rect, sizeFlags); }
  void SetSize(const wxSize& size) { wxWindow::SetSize(size); }
  virtual wxString GetStringSelection(void) const ;
  virtual bool SetStringSelection(const wxString& sel);

  virtual inline int Number(void) const { return m_noStrings; }
  virtual void Command(wxCommandEvent& event);

  virtual bool MSWCommand(WXUINT param, WXWORD id);

  virtual inline void SetColumns(int WXUNUSED(n) = 1 ) { /* No effect */ } ;
  virtual inline int GetColumns(void) const { return 1 ; };

  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
  int       m_noStrings;

};

#endif
	// _WX_CHOICE_H_
