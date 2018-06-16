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
    typedef typename wxFwdDataTransfer<W>::Base Base;
    typedef wxIsPubliclyDerived<W, Base> Fwd2Base;
    typedef typename wxIf<(Fwd2Base::value), Base, W>::value Window;

    // Note: always cast @param win to the parameter W and not to Window
    //       as the latter may be resolved to a base not necessarily derived
    //       from wxWindow, and the cast would fail as a consequence!
    static bool To(wxWindow* win, void* data)
    {
        // Travis emits these error messages when compiling dialogs sample (some configs only !):
        // In function `wxPrivate::wxDataTransferHelper<wxSpinCtrl, int>::To(wxWindow*, void*)':
        // dialogs.cpp:(.text._ZN9wxPrivate20wxDataTransferHelperI10wxSpinCtrliE2ToEP8wxWindowPv[_ZN9wxPrivate20wxDataTransferHelperI10wxSpinCtrliE2ToEP8wxWindowPv]+0x1f): undefined reference to `wxDataTransfer<void>::To(wxWindow*, void*)'
        // dialogs_dialogs.o: In function `wxPrivate::wxDataTransferHelper<wxSpinCtrl, int>::From(wxWindow*, void*)':
        // dialogs.cpp:(.text._ZN9wxPrivate20wxDataTransferHelperI10wxSpinCtrliE4FromEP8wxWindowPv[_ZN9wxPrivate20wxDataTransferHelperI10wxSpinCtrliE4FromEP8wxWindowPv]+0x1f): undefined reference to `wxDataTransfer<void>::From(wxWindow*, void*)'
        //
        // Now, i am intrigued to know why would Window be resolved to void instead of wxSpinCtrl (in some configs only !)
        typedef int (Window::*Mem); // compile time error if Window is not a class type

        return wxDataTransfer<Window>::To(static_cast<W*>(win), static_cast<T*>(data));
    }

    static bool From(wxWindow* win, void* data)
    {
        return wxDataTransfer<Window>::From(static_cast<W*>(win), static_cast<T*>(data));
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

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
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

// TODO: (incomplete!)
// ------
// see *NOTE* below...

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
// convenience functions.
//-----------------------------------------------------------------------------

template<class W, typename T>
inline wxGenericValidatorType<W, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorType<W, T>(value);
}

#if 0

// *NOTE*
// ------
// I am not sure if we should supply this overload and implement 
// wxGenericValidatorType<wxWindow, T> in terms of the old wxGenericValidator
// (class) implementation so that old code still compile and work unchanged ?

template<typename T>
inline wxGenericValidatorType<wxWindow, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorType<wxWindow, T>(value);
}

#endif

//
template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenericValidatorType<W, T>(value) );
}

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
