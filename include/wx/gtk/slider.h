/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/slider.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SLIDER_H_
#define _WX_GTK_SLIDER_H_

#include "wx/gtk/private/wrapgtk.h"

// ----------------------------------------------------------------------------
// wxSlider
// ----------------------------------------------------------------------------

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
             const wxString& name = wxSliderNameStr)
    {
        Create( parent, id, value, minValue, maxValue,
                pos, size, style, validator, name );
    }
    ~wxSlider();

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSliderNameStr);

    // implement the base class pure virtuals
    virtual int GetValue() const wxOVERRIDE;
    virtual void SetValue(int value) wxOVERRIDE;

    virtual void SetRange(int minValue, int maxValue) wxOVERRIDE;
    virtual int GetMin() const wxOVERRIDE;
    virtual int GetMax() const wxOVERRIDE;

    virtual void SetLineSize(int lineSize) wxOVERRIDE;
    virtual void SetPageSize(int pageSize) wxOVERRIDE;
    virtual int GetLineSize() const wxOVERRIDE;
    virtual int GetPageSize() const wxOVERRIDE;

    virtual void SetThumbLength(int lenPixels) wxOVERRIDE;
    virtual int GetThumbLength() const wxOVERRIDE;

#if GTK_CHECK_VERSION(2,16,0)
    virtual void ClearTicks() wxOVERRIDE;
    virtual void SetTick(int tickPos) wxOVERRIDE;
    int GetTickFreq() const wxOVERRIDE 
                        { return wx_is_at_least_gtk2(16) ? m_tickFreq : -1; }
#endif

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    void GTKDisableEvents();
    void GTKEnableEvents();
    bool GTKEventsDisabled() const;

    double m_pos;
    int m_scrollEventType;
    bool m_needThumbRelease;
    GtkWidget *m_scale;

protected:
    GtkWidget *m_minLabel,*m_maxLabel;
    bool m_blockScrollEvent;

    // Note the following two members are not used in GTK+2 < 2.16.
    int m_tickFreq;
    GtkPositionType m_posTicks;

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const wxOVERRIDE;

    // set the slider value unconditionally
    void GTKSetValue(int value);

    // Platform-specific implementation of SetTickFreq
    virtual void DoSetTickFreq(int freq) wxOVERRIDE;

    wxDECLARE_DYNAMIC_CLASS(wxSlider);
};

#endif // _WX_GTK_SLIDER_H_
