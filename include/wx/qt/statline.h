/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statline.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATLINE_H_
#define _WX_QT_STATLINE_H_

class QFrame;

class WXDLLIMPEXP_CORE wxStaticLine : public wxStaticLineBase
{
public:
    wxStaticLine();

    wxStaticLine( wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString &name = wxASCII_STR(wxStaticLineNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString &name = wxASCII_STR(wxStaticLineNameStr) );

    virtual QWidget *GetHandle() const override;

private:
    QFrame *m_qtFrame;

    wxDECLARE_DYNAMIC_CLASS( wxStaticLine );
};

#endif // _WX_QT_STATLINE_H_
