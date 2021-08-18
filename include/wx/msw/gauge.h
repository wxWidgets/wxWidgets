/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/gauge.h
// Purpose:     wxGauge implementation for MSW
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_GAUGE_H_
#define _WX_MSW_GAUGE_H_

#if wxUSE_GAUGE

extern WXDLLIMPEXP_DATA_CORE(const char) wxGaugeNameStr[];

// Group box
class WXDLLIMPEXP_CORE wxGauge : public wxGaugeBase
{
public:
    wxGauge() { }

    wxGauge(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxGaugeNameStr))
    {
        (void)Create(parent, id, range, pos, size, style, validator, name);
    }

    virtual ~wxGauge();

    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxGaugeNameStr));

    // set gauge range/value
    virtual void SetRange(int range) wxOVERRIDE;
    virtual void SetValue(int pos) wxOVERRIDE;

    // overridden base class virtuals
    virtual bool SetForegroundColour(const wxColour& col) wxOVERRIDE;
    virtual bool SetBackgroundColour(const wxColour& col) wxOVERRIDE;

    virtual void Pulse() wxOVERRIDE;

    WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const wxOVERRIDE;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const wxOVERRIDE { return false; }

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

private:
    // returns true if the control is currently in indeterminate (a.k.a.
    // "marquee") mode
    bool IsInIndeterminateMode() const;

    // switch to/from indeterminate mode
    void SetIndeterminateMode();
    void SetDeterminateMode();

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGauge);
};

#endif // wxUSE_GAUGE

#endif // _WX_MSW_GAUGE_H_
