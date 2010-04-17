/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statbox.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATBOX_H_
#define _WX_QT_STATBOX_H_

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox();
    
    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

protected:

private:
};

#endif // _WX_QT_STATBOX_H_
