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

#include "wx/datatransf.h"
#include "wx/debug.h"
#include "wx/typeinfo.h"
#include "wx/meta/convertible.h"
#include "wx/meta/if.h"


namespace wxPrivate
{

template<class W, typename T>
struct wxDataTransferHelper
{
    // These typedefs help to instantiate the right wxDataTransfer<Window>, i.e.:
    //      wxDataTransfer<Base> if Base is base of W.
    //      wxDataTransfer<W> if Base is void.

    typedef typename wxFwdDataTransfer<W>::Base Base;
    typedef wxIsPubliclyDerived<W, Base> Fwd2Base;
    typedef typename wxIf<(Fwd2Base::value), Base, W>::value Window;

    // Note: always cast @param win to the parameter W and not to Window
    //       as the latter may be resolved to a base not necessarily derived
    //       from wxWindow, and the cast would fail as a consequence!
    static bool To(wxWindow* win, void* data)
    {
        return wxDataTransfer<Window>::To(static_cast<W*>(win), static_cast<T*>(data));
    }

    static bool From(wxWindow* win, void* data)
    {
        return wxDataTransfer<Window>::From(static_cast<W*>(win), static_cast<T*>(data));
    }
};

} // namespace wxPrivate

// ----------------------------------------------------------------------------
// wxGenericValidatorBase 
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidatorBase : public wxValidator
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

    // Called when the value in the window must be validated: no validation by default.
    virtual bool Validate(wxWindow * WXUNUSED(parent)) wxOVERRIDE { return true; }

protected:
    explicit wxGenericValidatorBase(void* data) : m_data(data){}

    void* m_data;

private:
    wxDECLARE_CLASS(wxGenericValidatorBase);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidatorBase);
};

// ----------------------------------------------------------------------------
// wxGenericValidatorType performs data transfer between many standard controls
// and variables of the type corresponding to their values.
//
// N.B. wxGenericValidatorType doesn't do any validation by default - (as it is
//      intended to do data transfer only). But if the data should be validated
//      before doing any transfer, then all you have to do is specialize memfun
//      Validate ((see samples/validate for an example)).
// ----------------------------------------------------------------------------

template<class W, typename T>
class wxGenericValidatorType : public wxGenericValidatorBase
{
public:

    explicit wxGenericValidatorType(T* data)
        : wxGenericValidatorBase(data)
    {
    }

    wxGenericValidatorType(const wxGenericValidatorType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual ~wxGenericValidatorType(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorType(*this); }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        m_validatorWindow = win; 

        wxASSERT_MSG((wxTypeId(*win) == wxTypeId(W)), "Invalid window type!");
    }

    // No validation by default.
    virtual bool Validate(wxWindow * WXUNUSED(parent)) wxOVERRIDE { return true; }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return wxPrivate::wxDataTransferHelper<W, T>::To(m_validatorWindow, m_data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return wxPrivate::wxDataTransferHelper<W, T>::From(m_validatorWindow, m_data);
    }
};


//-----------------------------------------------------------------------------
// convenience functions.
//-----------------------------------------------------------------------------

// Notice that while the value type can be deduced from the passed parameter,
// the window type needs to be passed explicitly to be able to create the right
// wxGenericValidatorType (which is a compile-time type).
//    
template<class W, typename T>
inline wxGenericValidatorType<W, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorType<W, T>(value);
}

// A more convenient way to set generic validators 
template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenericValidatorType<W, T>(value) );
}

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
