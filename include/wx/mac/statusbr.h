///////////////////////////////////////////////////////////////////////////////
// Name:        statusbr.h
// Purpose:     native implementation of wxStatusBar. Optional; can use generic
//              version instead.
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_STATBAR_H_
#define   _WX_STATBAR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "statbrma.h"
#endif

class WXDLLEXPORT wxStatusBarMac : public wxStatusBarGeneric
{
  DECLARE_DYNAMIC_CLASS(wxStatusBarMac);

  wxStatusBarMac();
   wxStatusBarMac(wxWindow *parent, wxWindowID id,
           long style = 0,
           const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, style, name);
  }

  ~wxStatusBarMac();

  bool Create(wxWindow *parent, wxWindowID id,
              long style ,
              const wxString& name = wxPanelNameStr) ;

  virtual void DrawFieldText(wxDC& dc, int i);
  virtual void DrawField(wxDC& dc, int i);
  virtual void SetStatusText(const wxString& text, int number = 0) ;
  ////////////////////////////////////////////////////////////////////////
  // Implementation

  virtual void MacSuperEnabled( bool enable ) ;

  void OnPaint(wxPaintEvent& event);
protected:

  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATBAR_H_
