/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statbox.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATBOX_H_
#define _WX_QT_STATBOX_H_

class QGroupBox;

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox() = default;

    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const override;

    virtual void SetLabel(const wxString& label) override;
    virtual wxString GetLabel() const override;

    QGroupBox* GetQGroupBox() const;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBox);
};

#endif // _WX_QT_STATBOX_H_
