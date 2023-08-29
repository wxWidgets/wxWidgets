/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/slider.h
// Purpose:     wxSlider class implementation using trackbar control
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDER_H_
#define _WX_SLIDER_H_

class WXDLLIMPEXP_FWD_CORE wxSubwindows;

// Slider
class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase
{
public:
    wxSlider() { Init(); }

    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value,
             int minValue,
             int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxSliderNameStr))
    {
        Init();

        (void)Create(parent, id, value, minValue, maxValue,
                     pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value,
                int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    virtual ~wxSlider();

    // slider methods
    virtual int GetValue() const override;
    virtual void SetValue(int) override;

    void SetRange(int minValue, int maxValue) override;

    int GetMin() const override { return m_rangeMin; }
    int GetMax() const override { return m_rangeMax; }

    // Win32-specific slider methods
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

    // implementation only from now on
    WXHWND GetStaticMin() const;
    WXHWND GetStaticMax() const;
    WXHWND GetEditValue() const;
    virtual bool ContainsHWND(WXHWND hWnd) const override;

    // we should let background show through the slider (and its labels)
    virtual bool HasTransparentBackground() override { return true; }

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

    void Command(wxCommandEvent& event) override;
    virtual bool MSWOnScroll(int orientation, WXWORD wParam,
                             WXWORD pos, WXHWND control) override;

    virtual bool Show(bool show = true) override;
    virtual bool Enable(bool show = true) override;
    virtual bool SetFont(const wxFont& font) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;
    virtual bool SetBackgroundColour(const wxColour& colour) override;

    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;

protected:
    // common part of all ctors
    void Init();

    // format an integer value as string
    static wxString Format(int n) { return wxString::Format(wxT("%d"), n); }

    // get the boundig box for the slider and possible labels
    wxRect GetBoundingBox() const;

    // Get the height and, if the pointers are non null, widths of both labels.
    //
    // Notice that the return value will be 0 if we don't have wxSL_LABELS
    // style but we do fill widthMin and widthMax even if we don't have
    // wxSL_MIN_MAX_LABELS style set so the caller should account for it.
    int GetLabelsSize(int *widthMin = nullptr, int *widthMax = nullptr) const;


    // overridden base class virtuals
    virtual void DoGetPosition(int *x, int *y) const override;
    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual wxSize DoGetBestSize() const override;

    WXHBRUSH DoMSWControlColor(WXHDC pDC, wxColour colBg, WXHWND hWnd) override;

    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;
    virtual void MSWBeforeDPIChangedEvent(const wxDPIChangedEvent& event) override;


    // the labels windows, if any
    wxSubwindows  *m_labels;

    // Last background brush we returned from DoMSWControlColor(), see there.
    WXHBRUSH m_hBrushBg;

    int           m_rangeMin;
    int           m_rangeMax;
    int           m_pageSize;
    int           m_lineSize;
    int           m_tickFreq;

    // flag needed to detect whether we're getting THUMBRELEASE event because
    // of dragging the thumb or scrolling the mouse wheel
    bool m_isDragging;

    // Platform-specific implementation of SetTickFreq
    virtual void DoSetTickFreq(int freq) override;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSlider);
};

#endif // _WX_SLIDER_H_

