/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_BMPBUTTN_H_
#define _WX_OSX_BMPBUTTN_H_

#include "wx/button.h"

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLIMPEXP_CORE wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton()
        {
            SetMargins(wxDEFAULT_BUTTON_MARGIN, wxDEFAULT_BUTTON_MARGIN);
        }

    wxBitmapButton(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
        {
            Create(parent, id, bitmap, pos, size, style, validator, name);
        }

    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

protected:
    void OnEnterWindow( wxMouseEvent& event);
    void OnLeaveWindow( wxMouseEvent& event);
    
    virtual wxSize DoGetBestSize() const;

    virtual void DoSetBitmap(const wxBitmap& bitmap, State which);

    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_OSX_BMPBUTTN_H_
