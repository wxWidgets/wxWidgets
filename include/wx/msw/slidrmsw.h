/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/slidrmsw.h
// Purpose:     wxSliderMSW class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SLIDRMSW_H_
#define _SLIDRMSW_H_

#ifdef __GNUG__
#pragma interface "slidrmsw.h"
#endif

// Slider
class WXDLLEXPORT wxSliderMSW : public wxSliderBase
{
public:
    wxSliderMSW();

    wxSliderMSW(wxWindow *parent, wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSL_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxSliderNameStr)
    {
        Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
    }

    ~wxSliderMSW();

    bool Create(wxWindow *parent, wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSL_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxSliderNameStr);

    virtual int GetValue() const;
    virtual void SetValue(int);

    void GetSize(int *x, int *y) const;
    void GetPosition(int *x, int *y) const;

    bool Show(bool show);

    void SetRange(int minValue, int maxValue);

    int GetMin() const { return m_rangeMin; }
    int GetMax() const { return m_rangeMax; }

    // For trackbars only
    void SetPageSize(int pageSize);
    int GetPageSize() const;
    void SetLineSize(int lineSize);
    int GetLineSize() const;

    // IMPLEMENTATION
    WXHWND GetStaticMin() const { return m_staticMin; }
    WXHWND GetStaticMax() const { return m_staticMax; }
    WXHWND GetEditValue() const { return m_staticValue; }
    virtual bool ContainsHWND(WXHWND hWnd) const;

    void Command(wxCommandEvent& event);
    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWOnScroll(int orientation, WXWORD wParam,
                             WXWORD pos, WXHWND control);

protected:
    WXHWND        m_staticMin;
    WXHWND        m_staticMax;
    WXHWND        m_staticValue;
    int           m_rangeMin;
    int           m_rangeMax;
    int           m_pageSize;
    int           m_lineSize;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    DECLARE_DYNAMIC_CLASS(wxSliderMSW)
};

#endif
    // _SLIDRMSW_H_
