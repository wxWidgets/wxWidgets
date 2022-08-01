/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/panelg.h
// Purpose:     wxPanel: a container for child controls
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PANELG_H_
#define _WX_GENERIC_PANELG_H_

#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxPanel : public wxPanelBase
{
public:
    wxPanel() { }

    // Constructor
    wxPanel(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Create(parent, winid, pos, size, style, name);
    }

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPanel);
};

#endif // _WX_GENERIC_PANELG_H_
