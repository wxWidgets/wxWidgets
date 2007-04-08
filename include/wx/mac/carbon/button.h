/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#include "wx/control.h"
#include "wx/gdicmn.h"

WXDLLEXPORT_DATA(extern const wxChar) wxButtonNameStr[];

// Pushbutton
class WXDLLEXPORT wxButton: public wxButtonBase
{
public:
    wxButton() {}
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual wxInt32 MacControlHit( WXEVENTHANDLERREF handler , WXEVENTREF event ) ;
    static wxSize GetDefaultSize();

    virtual wxWindow *SetDefault();
    virtual void Command(wxCommandEvent& event);

protected:
    virtual wxSize DoGetBestSize() const ;

    DECLARE_DYNAMIC_CLASS(wxButton)
};

#endif
    // _WX_BUTTON_H_
