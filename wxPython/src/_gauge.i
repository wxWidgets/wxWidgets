/////////////////////////////////////////////////////////////////////////////
// Name:        _gauge.i
// Purpose:     SWIG interface defs for wxGauge
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

MAKE_CONST_WXSTRING(GaugeNameStr);

%newgroup

enum {
    wxGA_HORIZONTAL,
    wxGA_VERTICAL,
    wxGA_SMOOTH,
    wxGA_PROGRESSBAR   // obsolete
};

//---------------------------------------------------------------------------


MustHaveApp(wxGauge);

class wxGauge : public wxControl {
public:
    %pythonAppend wxGauge         "self._setOORInfo(self)"
    %pythonAppend wxGauge()       ""

    wxGauge(wxWindow* parent, wxWindowID id=-1, int range=100,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyGaugeNameStr);
    %RenameCtor(PreGauge, wxGauge());

    bool Create(wxWindow* parent, wxWindowID id=-1, int range=100,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyGaugeNameStr);

    // set/get the control range
    virtual void SetRange(int range);
    virtual int GetRange() const;

    // position
    virtual void SetValue(int pos);
    virtual int GetValue() const;

    // simple accessors
    bool IsVertical() const;

    // appearance params (not implemented for most ports)
    virtual void SetShadowWidth(int w);
    virtual int GetShadowWidth() const;
    virtual void SetBezelFace(int w);
    virtual int GetBezelFace() const;


    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
