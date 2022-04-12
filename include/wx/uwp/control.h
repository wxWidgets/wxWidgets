/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/control.h
// Purpose:     wxControl class
// Author:      Yann Clotioloman Yéo
// Modified by:
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UWP_CONTROL_H_
#define _WX_UWP_CONTROL_H_

#include "wx/dynarray.h"

// General item class
class WXDLLIMPEXP_CORE wxControl : public wxControlBase
{
public:
    wxControl() { }

    wxControl(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxControlNameStr))
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxControlNameStr));

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxControl);
};

#endif // _WX_UWP_CONTROL_H_
