/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/stattext.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_QT_STATTEXT_H_
#define WX_QT_STATTEXT_H_

#include <QtWidgets/QLabel>

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr );

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxStaticTextNameStr );

    void SetLabel(const wxString& label);

    QLabel *GetHandle() const;
private:
    QLabel *m_qtLabel;

    wxDECLARE_DYNAMIC_CLASS( wxStaticText );
};

#endif // _WX_QT_STATTEXT_H_
