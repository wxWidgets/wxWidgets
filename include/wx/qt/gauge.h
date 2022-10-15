/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/gauge.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_GAUGE_H_
#define _WX_QT_GAUGE_H_

class QProgressBar;

class WXDLLIMPEXP_CORE wxGauge : public wxGaugeBase
{
public:
    wxGauge();

    wxGauge(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxGaugeNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxGaugeNameStr));

    virtual QWidget *GetHandle() const override;

    // set/get the control range
    virtual void SetRange(int range) override;
    virtual int GetRange() const override;

    virtual void SetValue(int pos) override;
    virtual int GetValue() const override;

private:
    QProgressBar *m_qtProgressBar;

    wxDECLARE_DYNAMIC_CLASS(wxGauge);
};

#endif // _WX_QT_GAUGE_H_
