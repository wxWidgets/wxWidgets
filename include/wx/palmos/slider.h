/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/slider95.h
// Purpose:     wxSlider class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SLIDER95_H_
#define _SLIDER95_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "slider95.h"
#endif

// Slider
class WXDLLEXPORT wxSlider : public wxSliderBase
{
public:
    wxSlider();

    wxSlider(wxWindow *parent, wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSL_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxSliderNameStr)
    {
        Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
    }

    ~wxSlider();

    bool Create(wxWindow *parent, wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSL_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxSliderNameStr);

    virtual int GetValue() const;
    virtual void SetValue(int);

    void GetPosition(int *x, int *y) const;

    void SetRange(int minValue, int maxValue);

    int GetMin() const;
    int GetMax() const;

    // For trackbars only
    void SetTickFreq(int n, int pos);
    int GetTickFreq() const { return GetPageSize(); }
    void SetPageSize(int pageSize);
    int GetPageSize() const;
    void ClearSel();
    void ClearTicks();
    void SetLineSize(int lineSize);
    int GetLineSize() const;
    int GetSelEnd() const;
    int GetSelStart() const;
    void SetSelection(int minPos, int maxPos);
    void SetThumbLength(int len);
    int GetThumbLength() const;
    void SetTick(int tickPos);

    // implementation
    void Command(wxCommandEvent& event);

protected:
    virtual void DoGetSize(int *width, int *height) const;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual wxSize DoGetBestSize() const;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxSlider)
};

#endif
    // _SLIDER95_H_
