///////////////////////////////////////////////////////////////////////////////
// Name:        statusbr.h
// Purpose:     native implementation of wxStatusBar. Optional; can use generic
//              version instead.
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_STATBAR_H_
#define   _WX_STATBAR_H_

class WXDLLEXPORT wxStatusBarPM : public wxStatusBar
{
  DECLARE_DYNAMIC_CLASS(wxStatusBarPM);

public:
  // ctors
  wxStatusBarPM();
  wxStatusBarPM(wxWindow *parent, wxWindowID id = -1, long style = wxST_SIZEGRIP);

  // create status line
  bool Create(wxWindow *parent, wxWindowID id = -1, long style = wxST_SIZEGRIP);

  // a status line can have several (<256) fields numbered from 0
  virtual void SetFieldsCount(int number = 1, const int widths[] = NULL);

  // each field of status line has its own text
  virtual void     SetStatusText(const wxString& text, int number = 0);
  virtual wxString GetStatusText(int number = 0) const;

  // set status line fields' widths
  virtual void SetStatusWidths(int n, const int widths_field[]);

  void OnSize(wxSizeEvent& event);

  DECLARE_EVENT_TABLE()

protected:
  void CopyFieldsWidth(const int widths[]);
  void SetFieldsWidth();
};

#endif // _WX_STATBAR_H_

