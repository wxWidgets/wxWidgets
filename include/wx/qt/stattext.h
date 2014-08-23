/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/stattext.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATTEXT_H_
#define _WX_QT_STATTEXT_H_

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

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxStaticText );
};

#endif // _WX_QT_STATTEXT_H_
