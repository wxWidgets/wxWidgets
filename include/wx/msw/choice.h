/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHOICEH__
#define __CHOICEH__

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
  int no_strings;

  inline wxChoice(void) { no_strings = 0; }

  inline wxChoice(wxWindow *parent, const wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const int n = 0, const wxString choices[] = NULL,
           const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr)
  {
    Create(parent, id, pos, size, n, choices, style, validator, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const int n = 0, const wxString choices[] = NULL,
           const long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr);

  virtual void Append(const wxString& item);
  virtual void Delete(const int n);
  virtual void Clear(void);
  virtual int GetSelection(void) const ;
  virtual void SetSelection(const int n);
  virtual int FindString(const wxString& s) const;
  virtual wxString GetString(const int n) const ;
  virtual void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  virtual wxString GetStringSelection(void) const ;
  virtual bool SetStringSelection(const wxString& sel);

  virtual inline int Number(void) const { return no_strings; }
  virtual void Command(wxCommandEvent& event);

  virtual bool MSWCommand(const WXUINT param, const WXWORD id);

  virtual inline void SetColumns(const int WXUNUSED(n) = 1 ) { /* No effect */ } ;
  virtual inline int GetColumns(void) const { return 1 ; };

  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
};

#endif
	// __CHOICEH__
