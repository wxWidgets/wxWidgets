/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/slider.h
// Purpose:     wxWinUI wxSlider declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SLIDER_H_
#define _WX_WINUI_SLIDER_H_

#include <memory>

class wxWinUISliderImpl;

class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase
{
public:
    wxSlider();
    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value,
             int minValue,
             int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxSliderNameStr));
    ~wxSlider() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value,
                int minValue,
                int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    int GetValue() const override;
    void SetValue(int value) override;
    void SetRange(int minValue, int maxValue) override;
    int GetMin() const override { return m_rangeMin; }
    int GetMax() const override { return m_rangeMax; }
    void SetLineSize(int lineSize) override { m_lineSize = lineSize; }
    void SetPageSize(int pageSize) override { m_pageSize = pageSize; }
    int GetLineSize() const override { return m_lineSize; }
    int GetPageSize() const override { return m_pageSize; }
    void SetThumbLength(int lenPixels) override { m_thumbLength = lenPixels; }
    int GetThumbLength() const override { return m_thumbLength; }
    int GetTickFreq() const override { return m_tickFreq; }
    void SetTick(int tickPos) override;

    void Command(wxCommandEvent& event) override;

protected:
    void DoSetTickFreq(int freq) override;
    wxSize DoGetBestSize() const override;

private:
    int ClampValue(int value) const;
    void ApplyRangeToPeer();
    void ApplyValueToPeer();
    void SendSliderEvent();

    std::unique_ptr<wxWinUISliderImpl> m_winui;
    int m_value;
    int m_rangeMin;
    int m_rangeMax;
    int m_lineSize;
    int m_pageSize;
    int m_thumbLength;
    int m_tickFreq;
    bool m_updatingPeer;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSlider);
};

#endif // _WX_WINUI_SLIDER_H_
