/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/stattext.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATTEXT_H_
#define _WX_QT_STATTEXT_H_

class QLabel;

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

    virtual void SetLabel(const wxString& label) wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

protected:
    virtual wxString WXGetVisibleLabel() const wxOVERRIDE;
    virtual void WXSetVisibleLabel(const wxString& str) wxOVERRIDE;

private:
    QLabel *m_qtLabel;

    wxDECLARE_DYNAMIC_CLASS( wxStaticText );
};

#endif // _WX_QT_STATTEXT_H_
