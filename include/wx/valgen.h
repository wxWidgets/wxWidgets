/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valgen.h
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by: Ali Kettab (2018-04-07)
// Created:     Jan 22 1999
// Copyright:   (c) 1999 Julian Smart (assigned from Kevin)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALGENH__
#define _WX_VALGENH__

#include "wx/validate.h"

#if wxUSE_VALIDATORS

#include "wx/typeinfo.h"

// Internal struct to implement type-erasure idiom.
struct wxValidatorData
{
    virtual ~wxValidatorData() {}
    virtual wxValidatorData * Clone() const = 0;

    template<class T> bool IsOfType();

    template<class T> T& GetValue();
    template<class T> const T& GetValue() const;
    template<class T> void SetValue(const T& value);
};

// ----------------------------------------------------------------------------
// wxGenericValidator performs data transfer between many standard controls and
// variables of the type corresponding to their values.
//
// It doesn't do any validation so its name is a slight misnomer.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidatorBase: public wxValidator
{
public:

    wxGenericValidatorBase(const wxGenericValidatorBase& val);

    virtual ~wxGenericValidatorBase(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorBase(*this); }
    bool Copy(const wxGenericValidatorBase& val);

    // Called when the value in the window must be validated: this is not used
    // by this class
    virtual bool Validate(wxWindow * WXUNUSED(parent)) wxOVERRIDE { return true; }

    // Called to transfer data to the window
    virtual bool TransferToWindow() wxOVERRIDE;

    // Called to transfer data to the window
    virtual bool TransferFromWindow() wxOVERRIDE;

protected:
    explicit wxGenericValidatorBase(wxValidatorData* data) : m_data(data){}

    // DataPtr is a wxScopedPtr<wxValidatorData> defined inside wxValidator.
    DataPtr m_data;

private:
    wxDECLARE_CLASS(wxGenericValidatorBase);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidatorBase);
};

//-----------------------------------------------------------------------------

template<typename T>
class wxGenericValidatorType: public wxGenericValidatorBase
{
public:
    struct Data : wxValidatorData
    {
        explicit Data(T* value) : m_value(value) {}

        wxValidatorData* Clone() const wxOVERRIDE { return new Data(m_value); }

        T& GetValue() { return *m_value; }
        const T& GetValue() const { return *m_value; }
        void SetValue(const T& value){ *m_value = value; }
        
    private:
        T* m_value;
    };

public:

    explicit wxGenericValidatorType(T* value) : wxGenericValidatorBase(new Data(value)){}

    virtual ~wxGenericValidatorType(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorType(*this); }
};

//-----------------------------------------------------------------------------
// wxValidatorData implementation
//
// Notice that using static_cast<> here (rather than dynamic_cast<>)
// is just fine & safe AS LONG AS we check the stored value type before
// calling any of these methods (e.g. dataPtr->IsOfType<wxString>())
// 

template<class T> 
inline T& wxValidatorData::GetValue()
{
    typedef typename wxGenericValidatorType<T>::Data Data;
    return static_cast<Data&>(*this).GetValue();
}

template<class T>
inline const T& wxValidatorData::GetValue() const
{ 
    typedef typename wxGenericValidatorType<T>::Data Data;
    return static_cast<const Data&>(*this).GetValue();
}

template<class T>
inline void wxValidatorData::SetValue(const T& value)
{
    typedef typename wxGenericValidatorType<T>::Data Data;
    return static_cast<Data&>(*this).SetValue(value);
}

template<class T>
inline bool wxValidatorData::IsOfType()
{
    typedef typename wxGenericValidatorType<T>::Data Data;
    const Data& data = static_cast<const Data&>(*this);

    return wxTypeId(data.GetValue()) == wxTypeId(T);
}

//-----------------------------------------------------------------------------
// wxGenericValidator function
//-----------------------------------------------------------------------------
//
// Use this function to create your validators.
// e.g.:
//      bool b;
//      wxString s;
//      wxGenaricValidator(&b);
//      wxGenaricValidator(&s);

template<class T>
wxGenericValidatorType<T> wxGenericValidator(T* value)
{
    return wxGenericValidatorType<T>(value);
}

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
