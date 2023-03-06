/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/grideditors.h
// Purpose:     wxGridCellEditorEvtHandler and wxGrid editors
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by: Santiago Palacios
// Created:     1/08/1999
// Copyright:   (c) Michael Bedward
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_GRID_EDITORS_H_
#define _WX_GENERIC_GRID_EDITORS_H_

#include "wx/defs.h"

#if wxUSE_GRID

#if wxUSE_VALIDATORS
    #include <memory>
#endif

class wxGridCellEditorEvtHandler : public wxEvtHandler
{
public:
    wxGridCellEditorEvtHandler(wxGrid* grid, wxGridCellEditor* editor)
        : m_grid(grid),
          m_editor(editor),
          m_inSetFocus(false)
    {
    }

    void DismissEditor();

    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);

    void SetInSetFocus(bool inSetFocus) { m_inSetFocus = inSetFocus; }

private:
    wxGrid             *m_grid;
    wxGridCellEditor   *m_editor;

    // Work around the fact that a focus kill event can be sent to
    // a combobox within a set focus event.
    bool                m_inSetFocus;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxGridCellEditorEvtHandler);
    wxDECLARE_NO_COPY_CLASS(wxGridCellEditorEvtHandler);
};


#if wxUSE_TEXTCTRL

// the editor for string/text data
class WXDLLIMPEXP_ADV wxGridCellTextEditor : public wxGridCellEditor
{
public:
    explicit wxGridCellTextEditor(size_t maxChars = 0)
        : wxGridCellEditor(),
          m_maxChars(maxChars)
    {
    }

    wxGridCellTextEditor(const wxGridCellTextEditor& other);

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;
    virtual void SetSize(const wxRect& rect) override;

    virtual bool IsAcceptedKey(wxKeyEvent& event) override;
    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;
    virtual void StartingKey(wxKeyEvent& event) override;
    virtual void HandleReturn(wxKeyEvent& event) override;

    // parameters string format is "max_width"
    virtual void SetParameters(const wxString& params) override;
#if wxUSE_VALIDATORS
    virtual void SetValidator(const wxValidator& validator);
#endif

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellTextEditor(*this); }

    // added GetValue so we can get the value which is in the control
    virtual wxString GetValue() const override;

protected:
    wxTextCtrl *Text() const { return (wxTextCtrl *)m_control; }

    // parts of our virtual functions reused by the derived classes
    void DoCreate(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler,
                  long style = 0);
    void DoBeginEdit(const wxString& startValue);
    void DoReset(const wxString& startValue);

private:
    size_t                   m_maxChars;        // max number of chars allowed
#if wxUSE_VALIDATORS
    std::unique_ptr<wxValidator> m_validator;
#endif
    wxString                 m_value;
};

// the editor for numeric (long) data
class WXDLLIMPEXP_ADV wxGridCellNumberEditor : public wxGridCellTextEditor
{
public:
    // allows to specify the range - if min == max == -1, no range checking is
    // done
    explicit wxGridCellNumberEditor(int min = -1, int max = -1)
        : wxGridCellTextEditor(),
          m_min(min),
          m_max(max),
          m_value(0L)
    {
    }

    wxGridCellNumberEditor(const wxGridCellNumberEditor& other)
        : wxGridCellTextEditor(other),
          m_min(other.m_min),
          m_max(other.m_max),
          m_value(other.m_value)
    {
    }

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual void SetSize(const wxRect& rect) override;

    virtual bool IsAcceptedKey(wxKeyEvent& event) override;
    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;
    virtual void StartingKey(wxKeyEvent& event) override;

    // parameters string format is "min,max"
    virtual void SetParameters(const wxString& params) override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellNumberEditor(*this); }

    // added GetValue so we can get the value which is in the control
    virtual wxString GetValue() const override;

protected:
#if wxUSE_SPINCTRL
    wxSpinCtrl *Spin() const { return (wxSpinCtrl *)m_control; }
#endif

    // if HasRange(), we use wxSpinCtrl - otherwise wxTextCtrl
    bool HasRange() const
    {
#if wxUSE_SPINCTRL
        return m_min != m_max;
#else
        return false;
#endif
    }

    // string representation of our value
    wxString GetString() const
        { return wxString::Format(wxT("%ld"), m_value); }

private:
    int m_min,
        m_max;

    long m_value;
};


enum wxGridCellFloatFormat
{
    // Decimal floating point (%f)
    wxGRID_FLOAT_FORMAT_FIXED      = 0x0010,

    // Scientific notation (mantise/exponent) using e character (%e)
    wxGRID_FLOAT_FORMAT_SCIENTIFIC = 0x0020,

    // Use the shorter of %e or %f (%g)
    wxGRID_FLOAT_FORMAT_COMPACT    = 0x0040,

    // To use in combination with one of the above formats (%F/%E/%G)
    wxGRID_FLOAT_FORMAT_UPPER      = 0x0080,

    // Format used by default.
    wxGRID_FLOAT_FORMAT_DEFAULT    = wxGRID_FLOAT_FORMAT_FIXED,

    // A mask to extract format from the combination of flags.
    wxGRID_FLOAT_FORMAT_MASK       = wxGRID_FLOAT_FORMAT_FIXED |
                                     wxGRID_FLOAT_FORMAT_SCIENTIFIC |
                                     wxGRID_FLOAT_FORMAT_COMPACT |
                                     wxGRID_FLOAT_FORMAT_UPPER
};

// the editor for floating point numbers (double) data
class WXDLLIMPEXP_ADV wxGridCellFloatEditor : public wxGridCellTextEditor
{
public:
    explicit wxGridCellFloatEditor(int width = -1,
                                   int precision = -1,
                                   int format = wxGRID_FLOAT_FORMAT_DEFAULT)
        : wxGridCellTextEditor(),
          m_width(width),
          m_precision(precision),
          m_value(0.0),
          m_style(format)
    {
    }

    wxGridCellFloatEditor(const wxGridCellFloatEditor& other)
        : wxGridCellTextEditor(other),
          m_width(other.m_width),
          m_precision(other.m_precision),
          m_value(other.m_value),
          m_style(other.m_style),
          m_format(other.m_format)
    {
    }

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual bool IsAcceptedKey(wxKeyEvent& event) override;
    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;
    virtual void StartingKey(wxKeyEvent& event) override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellFloatEditor(*this); }

    // parameters string format is "width[,precision[,format]]"
    // format to choose between f|e|g|E|G (f is used by default)
    virtual void SetParameters(const wxString& params) override;

protected:
    // string representation of our value
    wxString GetString();

private:
    int m_width,
        m_precision;
    double m_value;

    int m_style;
    wxString m_format;
};

#endif // wxUSE_TEXTCTRL

#if wxUSE_CHECKBOX

// the editor for boolean data
class WXDLLIMPEXP_ADV wxGridCellBoolEditor : public wxGridCellEditor
{
public:
    wxGridCellBoolEditor()
        : wxGridCellEditor(),
          m_value(false)
    {
    }

    wxGridCellBoolEditor(const wxGridCellBoolEditor& other)
        : wxGridCellEditor(other),
          m_value(other.m_value)
    {
    }

    virtual wxGridActivationResult
    TryActivate(int row, int col, wxGrid* grid,
                const wxGridActivationSource& actSource) override;
    virtual void DoActivate(int row, int col, wxGrid* grid) override;

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual void SetSize(const wxRect& rect) override;
    virtual void Show(bool show, wxGridCellAttr *attr = nullptr) override;

    virtual bool IsAcceptedKey(wxKeyEvent& event) override;
    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;
    virtual void StartingClick() override;
    virtual void StartingKey(wxKeyEvent& event) override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellBoolEditor(*this); }

    // added GetValue so we can get the value which is in the control, see
    // also UseStringValues()
    virtual wxString GetValue() const override;

    // set the string values returned by GetValue() for the true and false
    // states, respectively
    static void UseStringValues(const wxString& valueTrue = wxT("1"),
                                const wxString& valueFalse = wxString());

    // return true if the given string is equal to the string representation of
    // true value which we currently use
    static bool IsTrueValue(const wxString& value);

protected:
    wxCheckBox *CBox() const { return (wxCheckBox *)m_control; }

private:
    // These functions modify or use m_value.
    void SetValueFromGrid(int row, int col, wxGrid* grid);
    void SetGridFromValue(int row, int col, wxGrid* grid) const;

    wxString GetStringValue() const { return GetStringValue(m_value); }

    static
    wxString GetStringValue(bool value) { return ms_stringValues[value]; }

    bool m_value;

    static wxString ms_stringValues[2];
};

#endif // wxUSE_CHECKBOX

#if wxUSE_COMBOBOX

// the editor for string data allowing to choose from the list of strings
class WXDLLIMPEXP_ADV wxGridCellChoiceEditor : public wxGridCellEditor
{
public:
    // if !allowOthers, user can't type a string not in choices array
    explicit wxGridCellChoiceEditor(size_t count = 0,
                                    const wxString choices[] = nullptr,
                                    bool allowOthers = false);
    explicit wxGridCellChoiceEditor(const wxArrayString& choices,
                                    bool allowOthers = false)
        : wxGridCellEditor(),
          m_choices(choices),
          m_allowOthers(allowOthers)
    {
    }

    wxGridCellChoiceEditor(const wxGridCellChoiceEditor& other)
        : wxGridCellEditor(other),
          m_value(other.m_value),
          m_choices(other.m_choices),
          m_allowOthers(other.m_allowOthers)
    {
    }

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual void SetSize(const wxRect& rect) override;

    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;

    // parameters string format is "item1[,item2[...,itemN]]"
    virtual void SetParameters(const wxString& params) override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellChoiceEditor(*this); }

    // added GetValue so we can get the value which is in the control
    virtual wxString GetValue() const override;

protected:
    wxComboBox *Combo() const { return (wxComboBox *)m_control; }

    void OnComboCloseUp(wxCommandEvent& evt);

    wxString        m_value;
    wxArrayString   m_choices;
    bool            m_allowOthers;
};

#endif // wxUSE_COMBOBOX

#if wxUSE_COMBOBOX

class WXDLLIMPEXP_ADV wxGridCellEnumEditor : public wxGridCellChoiceEditor
{
public:
    explicit wxGridCellEnumEditor(const wxString& choices = wxString());

    wxGridCellEnumEditor(const wxGridCellEnumEditor& other)
        : wxGridCellChoiceEditor(other),
          m_index(other.m_index)
    {
    }

    virtual ~wxGridCellEnumEditor() {}

    virtual wxGridCellEditor* Clone() const override
        { return new wxGridCellEnumEditor(*this); }

    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

private:
    long m_index;
};

#endif // wxUSE_COMBOBOX

class WXDLLIMPEXP_ADV wxGridCellAutoWrapStringEditor : public wxGridCellTextEditor
{
public:
    wxGridCellAutoWrapStringEditor()
        : wxGridCellTextEditor()
    {
    }

    wxGridCellAutoWrapStringEditor(const wxGridCellAutoWrapStringEditor& other)
        : wxGridCellTextEditor(other)
    {
    }

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellAutoWrapStringEditor(*this); }
};

#if wxUSE_DATEPICKCTRL

class WXDLLIMPEXP_ADV wxGridCellDateEditor : public wxGridCellEditor
{
public:
    explicit wxGridCellDateEditor(const wxString& format = wxString());

    wxGridCellDateEditor(const wxGridCellDateEditor& other)
        : wxGridCellEditor(other),
          m_value(other.m_value),
          m_format(other.m_format)
    {
    }

    virtual void SetParameters(const wxString& params) override;

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) override;

    virtual void SetSize(const wxRect& rect) override;

    virtual void BeginEdit(int row, int col, wxGrid* grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid* grid,
                         const wxString& oldval, wxString *newval) override;
    virtual void ApplyEdit(int row, int col, wxGrid* grid) override;

    virtual void Reset() override;

    virtual wxGridCellEditor *Clone() const override
        { return new wxGridCellDateEditor(*this); }

    virtual wxString GetValue() const override;

protected:
    wxDatePickerCtrl* DatePicker() const;

private:
    wxDateTime m_value;
    wxString m_format;
};

#endif // wxUSE_DATEPICKCTRL

#endif // wxUSE_GRID

#endif // _WX_GENERIC_GRID_EDITORS_H_
