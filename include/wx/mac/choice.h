/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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
  inline wxChoice() { m_noStrings = 0; }

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
  virtual void Clear();
  virtual int GetSelection() const ;
  virtual void SetSelection(int n);
  virtual int FindString(const wxString& s) const;
  virtual wxString GetString(int n) const ;
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  virtual wxString GetStringSelection() const ;
  virtual bool SetStringSelection(const wxString& sel);

  virtual inline int Number() const { return m_noStrings; }
  virtual void Command(wxCommandEvent& event);

  virtual inline void SetColumns(int WXUNUSED(n) = 1 ) { /* No effect */ } ;
  virtual inline int GetColumns() const { return 1 ; };
	void		MacHandleControlClick( ControlHandle control , SInt16 controlpart ) ;

protected:
  int m_noStrings;
  MenuHandle	m_macPopUpMenuHandle ;
};

#endif
	// _WX_CHOICE_H_
