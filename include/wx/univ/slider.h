///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/slider.h
// Purpose:     wxSlider control for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     09.02.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SLIDER_H_
#define _WX_UNIV_SLIDER_H_

#include "wx/univ/scrthumb.h"

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// our actions are the same as scrollbars

#define wxACTION_SLIDER_START       wxT("start")     // to the beginning
#define wxACTION_SLIDER_END         wxT("end")       // to the end
#define wxACTION_SLIDER_LINE_UP     wxT("lineup")    // one line up/left
#define wxACTION_SLIDER_PAGE_UP     wxT("pageup")    // one page up/left
#define wxACTION_SLIDER_LINE_DOWN   wxT("linedown")  // one line down/right
#define wxACTION_SLIDER_PAGE_DOWN   wxT("pagedown")  // one page down/right
#define wxACTION_SLIDER_PAGE_CHANGE wxT("pagechange")// change page by numArg

#define wxACTION_SLIDER_THUMB_DRAG      wxT("thumbdrag")
#define wxACTION_SLIDER_THUMB_MOVE      wxT("thumbmove")
#define wxACTION_SLIDER_THUMB_RELEASE   wxT("thumbrelease")

// ----------------------------------------------------------------------------
// wxSlider
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase,
                             public wxControlWithThumb
{
public:
    // ctors and such
    wxSlider();

    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxSliderNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    // implement base class pure virtuals
    virtual int GetValue() const override;
    virtual void SetValue(int value) override;

    virtual void SetRange(int minValue, int maxValue) override;
    virtual int GetMin() const override;
    virtual int GetMax() const override;

    virtual void SetLineSize(int lineSize) override;
    virtual void SetPageSize(int pageSize) override;
    virtual int GetLineSize() const override;
    virtual int GetPageSize() const override;

    virtual void SetThumbLength(int lenPixels) override;
    virtual int GetThumbLength() const override;

    virtual int GetTickFreq() const override { return m_tickFreq; }

    // wxUniv-specific methods
    // -----------------------

    // is this a vertical slider?
    bool IsVert() const { return (GetWindowStyle() & wxSL_VERTICAL) != 0; }

    // get the slider orientation
    wxOrientation GetOrientation() const
        { return IsVert() ? wxVERTICAL : wxHORIZONTAL; }

    // do we have labels?
    bool HasLabels() const
        { return ((GetWindowStyle() & wxSL_LABELS) != 0) &&
                 ((GetWindowStyle() & (wxSL_TOP|wxSL_BOTTOM|wxSL_LEFT|wxSL_RIGHT)) != 0); }

    // do we have ticks?
    bool HasTicks() const
        { return ((GetWindowStyle() & wxSL_TICKS) != 0) &&
                 ((GetWindowStyle() & (wxSL_TOP|wxSL_BOTTOM|wxSL_LEFT|wxSL_RIGHT|wxSL_BOTH)) != 0); }

    // implement wxControlWithThumb interface
    virtual wxWindow *GetWindow() override { return this; }
    virtual bool IsVertical() const override { return IsVert(); }

    virtual wxScrollThumb::Shaft HitTest(const wxPoint& pt) const override;
    virtual wxCoord ThumbPosToPixel() const override;
    virtual int PixelToThumbPos(wxCoord x) const override;

    virtual void SetShaftPartState(wxScrollThumb::Shaft shaftPart,
                                   int flag,
                                   bool set = true) override;

    virtual void OnThumbDragStart(int pos) override;
    virtual void OnThumbDrag(int pos) override;
    virtual void OnThumbDragEnd(int pos) override;
    virtual void OnPageScrollStart() override;
    virtual bool OnPageScroll(int pageInc) override;

    // for wxStdSliderInputHandler
    wxScrollThumb& GetThumb() { return m_thumb; }

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0,
                               const wxString& strArg = wxEmptyString) override;

    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) override
    {
        return GetStdInputHandler(handlerDef);
    }

protected:
    enum
    {
        INVALID_THUMB_VALUE = -0xffff
    };

    // Platform-specific implementation of SetTickFreq
    virtual void DoSetTickFreq(int freq) override;

    // overridden base class virtuals
    virtual wxSize DoGetBestClientSize() const override;
    virtual void DoDraw(wxControlRenderer *renderer) override;
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    // event handlers
    void OnSize(wxSizeEvent& event);

    // common part of all ctors
    void Init();

    // normalize the value to fit in the range
    int NormalizeValue(int value) const;

    // change the value by the given increment, return true if really changed
    bool ChangeValueBy(int inc);

    // change the value to the given one
    bool ChangeValueTo(int value);

    // is the value inside the range?
    bool IsInRange(int value) { return (value >= m_min) && (value <= m_max); }

    // format the value for printing as label
    virtual wxString FormatValue(int value) const;

    // calculate max label size
    wxSize CalcLabelSize() const;

    // calculate m_rectLabel/Slider
    void CalcGeometry();

    // get the thumb size
    wxSize GetThumbSize() const;

    // get the shaft rect (uses m_rectSlider which is supposed to be calculated)
    wxRect GetShaftRect() const;

    // calc the current thumb position using the shaft rect (if the pointer is
    // null, we calculate it here too)
    void CalcThumbRect(const wxRect *rectShaft,
                       wxRect *rectThumbOut,
                       wxRect *rectLabelOut,
                       int value = INVALID_THUMB_VALUE) const;

    // return the slider rect calculating it if needed
    const wxRect& GetSliderRect() const;

    // refresh the current thumb position
    void RefreshThumb();

private:
    // get the default thumb size (without using m_thumbSize)
    wxSize GetDefaultThumbSize() const;

    // the object which manages our thumb
    wxScrollThumb m_thumb;

    // the slider range and value
    int m_min,
        m_max,
        m_value;

    // the tick frequence (default is 1)
    int m_tickFreq;

    // the line and page increments (logical units)
    int m_lineSize,
        m_pageSize;

    // the size of the thumb (in pixels)
    int m_thumbSize;

    // the part of the client area reserved for the label, the ticks and the
    // part for the slider itself
    wxRect m_rectLabel,
           m_rectTicks,
           m_rectSlider;

    // the state of the thumb (wxCONTROL_XXX constants sum)
    int m_thumbFlags;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxSlider);
};

#endif // _WX_UNIV_SLIDER_H_
