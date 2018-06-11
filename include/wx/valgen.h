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

#include "wx/debug.h"
#include "wx/typeinfo.h"
#include "wx/meta/convertible.h"
#include "wx/meta/if.h"


// wxWindow-derived classes (W) need not to specialize wxDataTransfer<> if
// a base already has one and the derived just want to forward to its base
// implementation. if so, all you have to do is specialize this class.
template<class W>
struct wxFwdDataTransfer
{
    typedef void Base;
};

// wxDataTransfer implements the actual data transfer.
template<class W>
struct wxDataTransfer
{
    static bool To(wxWindow*, void*);
    static bool From(wxWindow*, void*);
};

namespace wxPrivate
{

template<class W, typename T>
struct wxDataTransferHelper
{
    static bool To(wxWindow* win, void* data)
    {
        typedef typename wxFwdDataTransfer<W>::Base Base;
        typedef wxIsPubliclyDerived<Base, wxWindow> Fwd2Base;
        typedef typename wxIf<Fwd2Base::value, Base, W>::value Window;

        return wxDataTransfer<Window>::To(win, static_cast<T*>(data));
    }

    static bool From(wxWindow* win, void* data)
    {
        typedef typename wxFwdDataTransfer<W>::Base Base;
        typedef wxIsPubliclyDerived<Base, wxWindow> Fwd2Base;
        typedef typename wxIf<Fwd2Base::value, Base, W>::value Window;

        return wxDataTransfer<Window>::From(win, static_cast<T*>(data));
    }
};

} // namespace wxPrivate

// ----------------------------------------------------------------------------
// wxGenericValidatorType performs data transfer between many standard controls
// and variables of the type corresponding to their values.
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

protected:
    explicit wxGenericValidatorBase(void* data) : m_data(data){}

    void* m_data;

private:
    wxDECLARE_CLASS(wxGenericValidatorBase);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidatorBase);
};

//-----------------------------------------------------------------------------

template<class W, typename T>
class wxGenericValidatorType: public wxGenericValidatorBase
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

    virtual void SetWindow(wxWindow *win) 
    {
        m_validatorWindow = win; 

        wxASSERT_MSG((wxTypeId(*win) == wxTypeId(W)), "Invalid window type!");
    }

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

#if 0

template<typename T>
class wxGenericValidatorType<wxWindow, T>: public wxGenericValidatorBase
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

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return wxPrivate::wxDataTransferHelper<wxWindow, T>::To(m_validatorWindow, m_data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return wxPrivate::wxDataTransferHelper<wxWindow, T>::From(m_validatorWindow, m_data);
    }
};

#endif 

//-----------------------------------------------------------------------------
// Helper function for creating generic validators which allows to avoid
// explicitly specifying the type as it deduces it from its parameter.
//-----------------------------------------------------------------------------

template<class W = wxWindow, typename T>
inline wxGenericValidatorType<W, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorType<W, T>(value);
}

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
