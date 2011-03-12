/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/control.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CONTROL_H_
#define _WX_QT_CONTROL_H_

class WXDLLIMPEXP_CORE wxControl : public wxControlBase
{
public:
    wxControl();
    wxControl(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxControlNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxControlNameStr);

protected:
    bool QtCreateControl( wxWindow *parent, wxWindowID id, const wxPoint &pos,
        const wxSize &size, long style, const wxValidator &validator,
        const wxString &name );

private:
    DECLARE_DYNAMIC_CLASS(wxControl)
};

#endif // _WX_QT_CONTROL_H_
