/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     wxStaticBox class
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBOX_H_
#define _WX_STATBOX_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBoxNameStr;

// Group box
class WXDLLEXPORT wxStaticBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticBox)

 public:
    inline wxStaticBox() {}
    inline wxStaticBox( wxWindow*       parent
                       ,wxWindowID      id
                       ,const wxString& label
                       ,const wxPoint&  pos = wxDefaultPosition
                       ,const wxSize&   size = wxDefaultSize
                       ,long            style = 0
                       ,const wxString& name = wxStaticBoxNameStr
                      )
    {
        Create(parent, id, label, pos, size, style, name);
    }

  bool Create( wxWindow*       parent
              ,wxWindowID      id
              ,const wxString& label
              ,const wxPoint&  pos = wxDefaultPosition
              ,const wxSize&   size = wxDefaultSize
              ,long            style = 0
              ,const wxString& name = wxStaticBoxNameStr
             );

    // implementation from now on
    // --------------------------

    void OnEraseBackground(wxEraseEvent& event);

    virtual MRESULT OS2WindowProc(HWND hwnd, WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATBOX_H_

