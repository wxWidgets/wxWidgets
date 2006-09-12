/////////////////////////////////////////////////////////////////////////////
// Name:        _slider.i
// Purpose:     SWIG interface defs for wxSlider
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/slider.h>

%}

MAKE_CONST_WXSTRING(SliderNameStr);

//---------------------------------------------------------------------------
%newgroup

enum {
    wxSL_HORIZONTAL,
    wxSL_VERTICAL,
    wxSL_TICKS,
    wxSL_AUTOTICKS,
    wxSL_LABELS,
    wxSL_LEFT,
    wxSL_TOP,
    wxSL_RIGHT,
    wxSL_BOTTOM,
    wxSL_BOTH,
    wxSL_SELRANGE,
    wxSL_INVERSE
};


MustHaveApp(wxSlider);

class wxSlider : public wxControl {
public:
    %pythonPrepend wxSlider         "if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']"
    %pythonPrepend wxSlider()       ""
    %pythonAppend  wxSlider         "self._setOORInfo(self)"
    %pythonAppend  wxSlider()       ""

    wxSlider(wxWindow* parent, wxWindowID id=-1,
             int value=0, int minValue=0, int maxValue=100,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPySliderNameStr);
    %RenameCtor(PreSlider, wxSlider());

    bool Create(wxWindow* parent, wxWindowID id=-1,
             int value=0, int minValue=0, int maxValue=100,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPySliderNameStr);

    // get/set the current slider value (should be in range)
    virtual int GetValue() const;
    virtual void SetValue(int value);

    // retrieve/change the range
    virtual void SetRange(int minValue, int maxValue);
    virtual int GetMin() const;
    virtual int GetMax() const;
    
    void SetMin( int minValue );
    void SetMax( int maxValue );

    // the line/page size is the increment by which the slider moves when
    // cursor arrow key/page up or down are pressed (clicking the mouse is like
    // pressing PageUp/Down) and are by default set to 1 and 1/10 of the range
    virtual void SetLineSize(int lineSize);
    virtual void SetPageSize(int pageSize);
    virtual int GetLineSize() const;
    virtual int GetPageSize() const;

    // these methods get/set the length of the slider pointer in pixels
    virtual void SetThumbLength(int lenPixels);
    virtual int GetThumbLength() const;

    virtual void SetTickFreq(int n, int pos=1);
    virtual int GetTickFreq() const;
    virtual void ClearTicks();
    virtual void SetTick(int tickPos);

    virtual void ClearSel();
    virtual int GetSelEnd() const;
    virtual int GetSelStart() const;
    virtual void SetSelection(int min, int max);


    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    %property(LineSize, GetLineSize, SetLineSize, doc="See `GetLineSize` and `SetLineSize`");
    %property(Max, GetMax, SetMax, doc="See `GetMax` and `SetMax`");
    %property(Min, GetMin, SetMin, doc="See `GetMin` and `SetMin`");
    %property(PageSize, GetPageSize, SetPageSize, doc="See `GetPageSize` and `SetPageSize`");
    %property(SelEnd, GetSelEnd, doc="See `GetSelEnd`");
    %property(SelStart, GetSelStart, doc="See `GetSelStart`");
    %property(ThumbLength, GetThumbLength, SetThumbLength, doc="See `GetThumbLength` and `SetThumbLength`");
    %property(TickFreq, GetTickFreq, SetTickFreq, doc="See `GetTickFreq` and `SetTickFreq`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};

//---------------------------------------------------------------------------
