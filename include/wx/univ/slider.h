///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/slider.h
// Purpose:     wxSlider control for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     09.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "univslider.h"
#endif

#ifndef _WX_UNIV_SLIDER_H_
#define _WX_UNIV_SLIDER_H_

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// our actions are the same as scrollbars

#define wxACTION_SLIDER_START       _T("start")     // to the beginning
#define wxACTION_SLIDER_END         _T("end")       // to the end
#define wxACTION_SLIDER_LINE_UP     _T("lineup")    // one line up/left
#define wxACTION_SLIDER_PAGE_UP     _T("pageup")    // one page up/left
#define wxACTION_SLIDER_LINE_DOWN   _T("linedown")  // one line down/right
#define wxACTION_SLIDER_PAGE_DOWN   _T("pagedown")  // one page down/right

#define wxACTION_SLIDER_THUMB_DRAG      _T("thumbdrag")
#define wxACTION_SLIDER_THUMB_MOVE      _T("thumbmove")
#define wxACTION_SLIDER_THUMB_RELEASE   _T("thumbrelease")

// ----------------------------------------------------------------------------
// wxSlider
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSlider: public wxSliderBase
{
public:
    // ctors and such
    wxSlider() { Init(); }

    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxSliderNameStr)
    {
        Init();

        (void)Create(parent, id, value, minValue, maxValue,
                     pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSliderNameStr);

    // implement base class pure virtuals
    virtual int GetValue() const;
    virtual void SetValue(int value);

    virtual void SetRange(int minValue, int maxValue);
    virtual int GetMin() const;
    virtual int GetMax() const;

    virtual void SetLineSize(int lineSize);
    virtual void SetPageSize(int pageSize);
    virtual int GetLineSize() const;
    virtual int GetPageSize() const;

    virtual void SetThumbLength(int lenPixels);
    virtual int GetThumbLength() const;

    // wxUniv-specific methods
    // -----------------------

    // is this a vertical slider?
    bool IsVertical() const { return (GetWindowStyle() & wxSL_VERTICAL) != 0; }

    // get the slider orientation
    wxOrientation GetOrientation() const
        { return IsVertical() ? wxVERTICAL : wxHORIZONTAL; }

    // do we have labels?
    bool HasLabels() const { return (GetWindowStyle() & wxSL_LABELS) != 0; }

protected:
    // overridden base class virtuals
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0,
                               const wxString& strArg = wxEmptyString);

    // event handlers
    void OnSize(wxSizeEvent& event);

    // common part of all ctors
    void Init();

    // normalize the value to fit in the range
    int NormalizeValue(int value) const;

    // change the value by the given increment, return TRUE if really changed
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

    // calc the current thumb position using the shaft rect (if the pointer is
    // NULL, we calculate it here too)
    void CalcThumbRect(const wxRect *rectShaft,
                       wxRect *rectThumbOut,
                       wxRect *rectLabelOut) const;

    // return the slider rect calculating it if needed
    const wxRect& GetSliderRect() const;

    // refresh the current thumb position
    void RefreshThumb();

private:
    // get the default thumb size (without using m_thumbSize)
    wxSize GetDefaultThumbSize() const;

    // the slider range and value
    int m_min,
        m_max,
        m_value;

    // the line and page increments (logical units)
    int m_lineSize,
        m_pageSize;

    // the size of the thumb (in pixels)
    int m_thumbSize;

    // the part of the client area reserved for the label and the part for the
    // slider itself
    wxRect m_rectLabel,
           m_rectSlider;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxSlider)
};

// ----------------------------------------------------------------------------
// wxStdSliderButtonInputHandler: default slider input handling
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdSliderButtonInputHandler : public wxStdInputHandler
{
public:
    wxStdSliderButtonInputHandler(wxInputHandler *inphand)
        : wxStdInputHandler(inphand)
    {
    }

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control,
                                 const wxMouseEvent& event);

    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event);
};

#endif // _WX_UNIV_SLIDER_H_
