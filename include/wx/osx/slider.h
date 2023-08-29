/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/slider.h
// Purpose:     wxSlider class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDER_H_
#define _WX_SLIDER_H_

#include "wx/compositewin.h"
#include "wx/stattext.h"

// Slider
class WXDLLIMPEXP_CORE wxSlider: public wxCompositeWindow<wxSliderBase>
{
    wxDECLARE_DYNAMIC_CLASS(wxSlider);

public:
    wxSlider();

    wxSlider(wxWindow *parent, wxWindowID id,
                    int value, int minValue, int maxValue,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxSL_HORIZONTAL,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxASCII_STR(wxSliderNameStr))
    {
        Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
    }

    virtual ~wxSlider();

    bool Create(wxWindow *parent, wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    virtual int GetValue() const override;
    virtual void SetValue(int) override;

    void SetRange(int minValue, int maxValue) override;

    int GetMin() const override { return m_rangeMin; }
    int GetMax() const override { return m_rangeMax; }

    void SetMin(int minValue) { SetRange(minValue, m_rangeMax); }
    void SetMax(int maxValue) { SetRange(m_rangeMin, maxValue); }

    // For trackbars only
    int GetTickFreq() const override { return m_tickFreq; }
    void SetPageSize(int pageSize) override;
    int GetPageSize() const override;
    void ClearSel() override;
    void ClearTicks() override;
    void SetLineSize(int lineSize) override;
    int GetLineSize() const override;
    int GetSelEnd() const override;
    int GetSelStart() const override;
    void SetSelection(int minPos, int maxPos) override;
    void SetThumbLength(int len) override;
    int GetThumbLength() const override;
    void SetTick(int tickPos) override;

    void Command(wxCommandEvent& event) override;
    // osx specific event handling common for all osx-ports

    virtual bool OSXHandleClicked( double timestampsec ) override;
    virtual void TriggerScrollEvent( wxEventType scrollEvent ) override;

protected:
    // Platform-specific implementation of SetTickFreq
    virtual void DoSetTickFreq(int freq) override;

    virtual wxSize DoGetBestSize() const override;
    virtual void   DoSetSize(int x, int y, int w, int h, int sizeFlags) override;

    // set min/max size of the slider
    virtual void DoSetSizeHints( int minW, int minH,
                                 int maxW, int maxH,
                                 int incW, int incH) override;

    // Common processing to invert slider values based on wxSL_INVERSE
    virtual int ValueInvertOrNot(int value) const override;

    wxStaticText*    m_macMinimumStatic ;
    wxStaticText*    m_macMaximumStatic ;
    wxStaticText*    m_macValueStatic ;

    int           m_rangeMin;
    int           m_rangeMax;
    int           m_pageSize;
    int           m_lineSize;
    int           m_tickFreq;
private :
    virtual wxWindowList GetCompositeWindowParts() const override
    {
        wxWindowList parts;
        parts.push_back(m_macMinimumStatic);
        parts.push_back(m_macMaximumStatic);
        parts.push_back(m_macValueStatic);
        return parts;
    }

    wxDECLARE_EVENT_TABLE();
};

#endif
    // _WX_SLIDER_H_
