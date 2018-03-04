/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valgen.h
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Created:     Jan 22 1999
// Copyright:   (c) 1999 Julian Smart (assigned from Kevin)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALGENH__
#define _WX_VALGENH__

#include "wx/validate.h"

#if wxUSE_VALIDATORS

class WXDLLIMPEXP_FWD_BASE wxArrayInt;
class WXDLLIMPEXP_FWD_BASE wxDateTime;
class WXDLLIMPEXP_FWD_BASE wxFileName;
class WXDLLIMPEXP_FWD_BASE wxString;

// ----------------------------------------------------------------------------
// wxGenericValidator performs data transfer between many standard controls and
// variables of the type corresponding to their values.
//
// It doesn't do any validation so its name is a slight misnomer.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidator: public wxValidator
{
public:
    wxGenericValidator(void* value, wxDataTransferTypes type = wxData_void)
        : m_value(value), m_type(type)
    {}

    // Different constructors: each of them creates a validator which can only
    // be used with some controls, the comments before each constructor
    // indicate which ones:
        // wxCheckBox, wxRadioButton, wx(Bitmap)ToggleButton
    wxGenericValidator(bool* value) : m_value(value), m_type(wxData_bool){}

        // wxChoice, wxGauge, wxRadioBox, wxScrollBar, wxSlider, wxSpinButton
    wxGenericValidator(int* value) : m_value(value), m_type(wxData_int){}

        // wxComboBox, wxTextCtrl, wxButton, wxStaticText (read-only)
    wxGenericValidator(wxString* value) : m_value(value), m_type(wxData_string){}

        // wxListBox, wxCheckListBox
    wxGenericValidator(wxArrayInt* value) : m_value(value), m_type(wxData_arrayint){}

#if wxUSE_DATETIME
        // wxDatePickerCtrl
    wxGenericValidator(wxDateTime* value) : m_value(value), m_type(wxData_datetime){}
#endif // wxUSE_DATETIME

        // wxTextCtrl
    wxGenericValidator(wxFileName* value) : m_value(value), m_type(wxData_filename){}
        // wxTextCtrl
    wxGenericValidator(float* value) : m_value(value), m_type(wxData_float){}
        // wxTextCtrl
    wxGenericValidator(double* value) : m_value(value), m_type(wxData_double){}

    wxGenericValidator(const wxGenericValidator& val);

    virtual ~wxGenericValidator(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidator(*this); }
    bool Copy(const wxGenericValidator& val);

    // Called when the value in the window must be validated: this is not used
    // by this class
    virtual bool Validate(wxWindow * WXUNUSED(parent)) wxOVERRIDE { return true; }

    // Called to transfer data to the window
    virtual bool TransferToWindow() wxOVERRIDE;

    // Called to transfer data to the window
    virtual bool TransferFromWindow() wxOVERRIDE;

protected:

    void* m_value;
    wxDataTransferTypes m_type;

private:
    wxDECLARE_CLASS(wxGenericValidator);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidator);
};

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
