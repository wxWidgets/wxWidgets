/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/slider.h
// Purpose:     wxMoSlider class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_SLIDER_H_
#define _WX_MOBILE_GENERIC_SLIDER_H_

#include "wx/slider.h"

/**
    @class wxMoSlider

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoSlider : public wxSliderBase
{
public:
    /// Default constructor.
    wxMoSlider() { Init(); }

    /// Constructor.
    wxMoSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxSliderNameStr)
    {
        Init();
        Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSliderNameStr);

    virtual ~wxMoSlider();

    void Init();

    /// Gets the current slider value (should be in range).
    virtual int GetValue() const;

    /// Sets the current slider value (should be in range).
    virtual void SetValue(int value);

    /// Sets the range.
    virtual void SetRange(int minValue, int maxValue);

    /// Gets the range minimum.
    virtual int GetMin() const;

    /// Gets the range maximum.
    virtual int GetMax() const;

    // the line/page size is the increment by which the slider moves when
    // cursor arrow key/page up or down are pressed (clicking the mouse is like
    // pressing PageUp/Down) and are by default set to 1 and 1/10 of the range
    // Not relevant to iPhone.
    virtual void SetLineSize(int lineSize);
    virtual void SetPageSize(int pageSize);
    virtual int GetLineSize() const;
    virtual int GetPageSize() const;

    // these methods get/set the length of the slider pointer in pixels
    virtual void SetThumbLength(int lenPixels);
    virtual int GetThumbLength() const;

// New iPhone functionality

    /// Set the minimum value bitmap (drawn on the left side of the slider)
    virtual void SetMinValueBitmap(const wxBitmap& bitmap);

    /// Get the minimum value bitmap (drawn on the left side of the slider)
    virtual wxBitmap GetMinValueBitmap() { return m_minValueBitmap; }

    /// Set the maximum value bitmap (drawn on the right side of the slider)
    virtual void SetMaxValueBitmap(const wxBitmap& bitmap);

    /// Get the maximum value bitmap (drawn on the right side of the slider)
    virtual wxBitmap GetMaxValueBitmap() { return m_maxValueBitmap; }

// Standard window functions

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);

protected:
    // Gets the track area rectangle
    bool GetTrackDimensions(wxRect& retTrackRect, wxRect& thumbRect) const;

    virtual wxSize DoGetBestSize() const;

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    int         m_value;
    int         m_minValue;
    int         m_maxValue;
    wxBitmap    m_minValueBitmap;
    wxBitmap    m_maxValueBitmap;
    int         m_dragStatus;
    wxPoint     m_startPosition;

    enum DragStatus { DragNone, DragStarting, DragDragging };

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoSlider)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_GENERIC_SLIDER_H_
