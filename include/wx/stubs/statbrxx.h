///////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbrxx.h
// Purpose:     native implementation of wxStatusBar. Optional.
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_STATBRXX_H
#define   _WX_STATBRXX_H

#ifdef __GNUG__
#pragma interface "statbrxx.h"
#endif

class WXDLLEXPORT wxStatusBarXX : public wxStatusBar
{
  DECLARE_DYNAMIC_CLASS(wxStatusBarXX);

public:
  // ctors
  wxStatusBarXX();
  wxStatusBarXX(wxWindow *parent, wxWindowID id = -1, long style = wxST_SIZEGRIP);

  // create status line
  bool Create(wxWindow *parent, wxWindowID id = -1, long style = wxST_SIZEGRIP);

  // a status line can have several (<256) fields numbered from 0
  virtual void SetFieldsCount(int number = 1, const int widths[] = NULL);

  // each field of status line has it's own text
  virtual void     SetStatusText(const wxString& text, int number = 0);
  virtual wxString GetStatusText(int number = 0) const;

  // set status line fields' widths
  virtual void SetStatusWidths(int n, const int widths_field[]);

  // we're going to process WM_SIZE (of the parent window)
  void OnSize(wxSizeEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATBRXX_H