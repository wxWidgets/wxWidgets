/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/spinctrl.h
// Purpose:     wxSpinCtrl class
// Author:      Robert Roebling
// Modified by:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SPINCTRL_H_
#define _WX_GTK_SPINCTRL_H_

//-----------------------------------------------------------------------------
// wxSpinCtrlGTKBase - Base class for GTK versions of the wxSpinCtrl[Double]
//
// This class manages a double valued GTK spinctrl through the DoGet/SetXXX
// functions that are made public as Get/SetXXX functions for int or double
// for the wxSpinCtrl and wxSpinCtrlDouble classes respectively to avoid
// function ambiguity.
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrlGTKBase : public wxSpinCtrlBase
{
public:
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                double min, double max, double initial,
                double inc,
                const wxString& name);

    // wxSpinCtrl(Double) methods call DoXXX functions of the same name

    // accessors
    virtual wxString GetTextValue() const override;
    // T GetValue() const
    // T GetMin() const
    // T GetMax() const
    // T GetIncrement() const
    virtual bool GetSnapToTicks() const override;

    // operations
    virtual void SetValue(const wxString& value) override;
    // void SetValue(T val)
    // void SetRange(T minVal, T maxVal)
    // void SetIncrement(T inc)
    void SetSnapToTicks( bool snap_to_ticks ) override;

    // Select text in the textctrl
    void SetSelection(long from, long to) override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    void OnChar( wxKeyEvent &event );


    // These values map to the possible return values of "input" GTK signal but
    // are more readable and type-safe.
    enum GTKInputResult
    {
        GTKInput_Error = -1,
        GTKInput_Default,
        GTKInput_Converted
    };

    virtual GTKInputResult GTKInput(double* value) const = 0;
    virtual bool GTKOutput(wxString* text) const = 0;

    virtual void GTKValueChanged() = 0;
    void GTKTextChanged();

protected:
    wxSpinCtrlGTKBase();
    ~wxSpinCtrlGTKBase();

    double DoGetValue() const;
    double DoGetMin() const;
    double DoGetMax() const;
    double DoGetIncrement() const;

    void DoSetValue(double val);
    void DoSetValue(const wxString& strValue);
    void DoSetRange(double min_val, double max_val);
    void DoSetIncrement(double inc);

    void GtkDisableEvents();
    void GtkEnableEvents();

    // Update the width of the entry field to fit the current range (and also
    // base or number of digits depending on the derived class).
    virtual void GtkSetEntryWidth() = 0;

    virtual wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    // Widgets that use the style->base colour for the BG colour should
    // override this and return true.
    virtual bool UseGTKStyleBase() const override { return true; }

    // Set m_textOverride to use the given text instead of the numeric value.
    void GTKSetTextOverride(const wxString& text);

    // Reset the override and changing the value to correspond to the
    // previously overridden numeric value.
    void GTKResetTextOverride();

    // Just reset the override, without touching the value, returning true if
    // we did it. In most cases, the function above should be used instead.
    bool GTKResetTextOverrideOnly();

private:
    // This function does _not_ take into account m_textOverride, so it is
    // private and normally shouldn't be used -- use DoGetValue() instead.
    double GTKGetValue() const;

    // Non-null when the text value is different from the numeric value.
    class wxSpinCtrlGTKTextOverride* m_textOverride;


    friend class wxSpinCtrlEventDisabler;

    wxDECLARE_EVENT_TABLE();
};

//-----------------------------------------------------------------------------
// wxSpinCtrl - An integer valued spin control
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrl : public wxSpinCtrlGTKBase
{
public:
    wxSpinCtrl() { Init(); }
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxS("wxSpinCtrl"))
    {
        Init();

        Create(parent, id, value, pos, size, style, min, max, initial, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = wxS("wxSpinCtrl"))
    {
        return wxSpinCtrlGTKBase::Create(parent, id, value, pos, size,
                                         style, min, max, initial, 1, name);
    }

    // accessors
    int GetValue() const { return int(DoGetValue()); }
    int GetMin() const { return int(DoGetMin()); }
    int GetMax() const { return int(DoGetMax()); }
    int GetIncrement() const { return int(DoGetIncrement()); }

    // operations
    void SetValue(const wxString& value) override    { wxSpinCtrlGTKBase::SetValue(value); } // visibility problem w/ gcc
    void SetValue( int value )              { DoSetValue(value); }
    void SetRange( int minVal, int maxVal ) { DoSetRange(minVal, maxVal); }
    void SetIncrement(int inc) { DoSetIncrement(inc); }

    virtual int GetBase() const override { return m_base; }
    virtual bool SetBase(int base) override;

    virtual GTKInputResult GTKInput(double* value) const override;
    virtual bool GTKOutput(wxString* text) const override;
    virtual void GTKValueChanged() override;

protected:
    virtual void GtkSetEntryWidth() override;

private:
    // Common part of all ctors.
    void Init()
    {
        m_base = 10;
    }

    int m_base;

    wxDECLARE_DYNAMIC_CLASS(wxSpinCtrl);
};

//-----------------------------------------------------------------------------
// wxSpinCtrlDouble - a double valued spin control
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrlDouble : public wxSpinCtrlGTKBase
{
public:
    wxSpinCtrlDouble() {}
    wxSpinCtrlDouble(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_ARROW_KEYS,
                     double min = 0, double max = 100, double initial = 0,
                     double inc = 1,
                     const wxString& name = wxS("wxSpinCtrlDouble"))
    {
        Create(parent, id, value, pos, size, style,
               min, max, initial, inc, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxS("wxSpinCtrlDouble"))
    {
        return wxSpinCtrlGTKBase::Create(parent, id, value, pos, size,
                                         style, min, max, initial, inc, name);
    }

    // accessors
    double GetValue() const     { return DoGetValue(); }
    double GetMin() const       { return DoGetMin(); }
    double GetMax() const       { return DoGetMax(); }
    double GetIncrement() const { return DoGetIncrement(); }
    unsigned GetDigits() const;

    // operations
    void SetValue(const wxString& value) override        { wxSpinCtrlGTKBase::SetValue(value); } // visibility problem w/ gcc
    void SetValue(double value)                 { DoSetValue(value); }
    void SetRange(double minVal, double maxVal) { DoSetRange(minVal, maxVal); }
    void SetIncrement(double inc);
    void SetDigits(unsigned digits);

    virtual int GetBase() const override { return 10; }
    virtual bool SetBase(int WXUNUSED(base)) override { return false; }

    virtual GTKInputResult GTKInput(double* value) const override;
    virtual bool GTKOutput(wxString* text) const override;
    virtual void GTKValueChanged() override;

protected:
    virtual void GtkSetEntryWidth() override;

    wxDECLARE_DYNAMIC_CLASS(wxSpinCtrlDouble);
};

#endif // _WX_GTK_SPINCTRL_H_
