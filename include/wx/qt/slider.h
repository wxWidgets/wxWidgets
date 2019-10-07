/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/slider.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_SLIDER_H_
#define _WX_QT_SLIDER_H_

class QSlider;

class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase
{
public:
    wxSlider();
    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxSliderNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSliderNameStr);

    virtual int GetValue() const wxOVERRIDE;
    virtual void SetValue(int value) wxOVERRIDE;

    virtual void SetRange(int minValue, int maxValue) wxOVERRIDE;
    virtual int GetMin() const wxOVERRIDE;
    virtual int GetMax() const wxOVERRIDE;

    virtual void DoSetTickFreq(int freq) wxOVERRIDE;
    virtual int GetTickFreq() const wxOVERRIDE;

    virtual void SetLineSize(int lineSize) wxOVERRIDE;
    virtual void SetPageSize(int pageSize) wxOVERRIDE;
    virtual int GetLineSize() const wxOVERRIDE;
    virtual int GetPageSize() const wxOVERRIDE;

    virtual void SetThumbLength(int lenPixels) wxOVERRIDE;
    virtual int GetThumbLength() const wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

private:
    QSlider *m_qtSlider;

    wxDECLARE_DYNAMIC_CLASS( wxSlider );
};

#endif // _WX_QT_SLIDER_H_
