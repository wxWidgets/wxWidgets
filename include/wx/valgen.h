/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valgen.h
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by: Ali Kettab 2018-09-07
// Created:     Jan 22 1999
// Copyright:   (c) 1999 Julian Smart (assigned from Kevin)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALGENH__
#define _WX_VALGENH__

#include "wx/validate.h"

#if wxUSE_VALIDATORS

#if wxUSE_DATATRANSFER
    #include "wx/private/datatransfer.h"
    #include "wx/debug.h"
//  #include "wx/scopedptr.h"
    #include "wx/typeinfo.h"

    #if defined(HAVE_TYPE_TRAITS)
        #include <type_traits>
    #elif defined(HAVE_TR1_TYPE_TRAITS)
        #ifdef __VISUALC__
            #include <type_traits>
        #else
            #include <tr1/type_traits>
        #endif
    #endif

    #ifdef HAVE_STD_VARIANT
        #include <variant>
    #endif // HAVE_STD_VARIANT
#endif // wxUSE_DATATRANSFER


#if wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

template<class W>
struct wxDataTransfer
{
      // decltype will give us the window type (a pointer type) which
      // implements (specializes) wxDataTransferImpl<> interface. or
      // void* if no such specialization exists for W type.
    typedef decltype(wxDATA_TRANSFER_IMPLEMENTOR(W)) Type;
    typedef typename std::remove_pointer<Type>::type Window;

      // always cast @win to the parameter @W and not to @Window type
      // as the latter may be resolved to a base not necessarily derived
      // from wxWindow, and the cast would fail as a consequence!
    template<typename T>
    static bool To(wxWindow* win, void* data)
    {
        return wxDataTransferImpl<Window>::To(static_cast<W*>(win), static_cast<T*>(data));
    }

    template<typename T>
    static bool From(wxWindow* win, void* data)
    {
        return wxDataTransferImpl<Window>::From(static_cast<W*>(win), static_cast<T*>(data));
    }

      // No validation by default.
      // specialize this function in your own code if you want to do any validation or filtering.
    static bool DoValidate(W* WXUNUSED(win), wxWindow* WXUNUSED(parent)){ return true; }
};

// ----------------------------------------------------------------------------
// wxGenericValidatorBase 
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidatorBase : public wxValidator
{
public:

    wxGenericValidatorBase(const wxGenericValidatorBase& val);

    virtual ~wxGenericValidatorBase(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorBase(*this); }
    bool Copy(const wxGenericValidatorBase& val);

protected:
    explicit wxGenericValidatorBase(void* data) : m_data(data){}

    void* m_data;

private:
    wxDECLARE_CLASS(wxGenericValidatorBase);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidatorBase);
};

// ----------------------------------------------------------------------------
// 
//
// ----------------------------------------------------------------------------

template<class W, typename T>
class wxGenericValidatorSimpleType : public wxGenericValidatorBase
{
public:

    explicit wxGenericValidatorSimpleType(T* data)
        : wxGenericValidatorBase(data)
    {
    }

    wxGenericValidatorSimpleType(const wxGenericValidatorSimpleType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual ~wxGenericValidatorSimpleType(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorSimpleType(*this); }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        this->m_validatorWindow = win; 

        wxASSERT_MSG((wxTypeId(*win) == wxTypeId(W)), "Invalid window type!");
    }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE
    {
        return wxDataTransfer<W>::DoValidate(static_cast<W*>(this->GetWindow()), parent);
    }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return wxDataTransfer<W>::template To<T>(this->GetWindow(), this->m_data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return wxDataTransfer<W>::template From<T>(this->GetWindow(), this->m_data);
    }

protected:

    explicit wxGenericValidatorSimpleType(void* data) : wxGenericValidatorBase(data){}
};


#if defined(HAVE_VARIADIC_TEMPLATES)

template<class W, template<typename...> class TComposite, typename T, typename... Ts>
class wxGenericValidatorCompositType : public wxGenericValidatorSimpleType<W, T>
{
    typedef TComposite<T, Ts...> CompositeType;

    // Assigning (or re-assigning) new value to pointer-like types is different than
    // assigning (or re-assigning) new value to value-like types. so we need to resort
    // to the SFINAE mechanism to know what CompositeType represents at compile-time.
    // 
    // Hint: a standard conforming smart pointer should have a nested element_type defined.

    template <typename U>
    static auto IsSmartPtr(U const&) -> decltype(typename U::element_type(), std::true_type())
    {
        return std::true_type{};
    }

    static std::false_type IsSmartPtr(...) { return std::false_type{}; }

    static auto NewValue(const T& value, const std::true_type&){ return new T{value}; }
    static auto NewValue(const T& value, const std::false_type&){ return value; }

public:

    explicit wxGenericValidatorCompositType(CompositeType& data)
        : wxGenericValidatorSimpleType<W, T>(std::addressof(data))
    {
    }

    wxGenericValidatorCompositType(const wxGenericValidatorCompositType& val)
        : wxGenericValidatorSimpleType<W, T>(val)
    {
    }

    virtual ~wxGenericValidatorCompositType(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorCompositType(*this); }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        CompositeType& data = *static_cast<CompositeType*>(this->m_data);
        
        if ( !data )
            return true;

        return wxDataTransfer<W>::template To<T>(this->GetWindow(), &*data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        T value;

        if ( !wxDataTransfer<W>::template From<T>(this->GetWindow(), &value) )
            return false;

        CompositeType& data = *static_cast<CompositeType*>(this->m_data);

        if ( data )
        {
            *data = value;
        }
        else
        {
            // FIXME: wxSharedPtr<> can't be used with this class as it doesn't
            //        define the standard member function swap().
            data.swap(CompositeType(NewValue(value, IsSmartPtr(data))));
        }

        return true;
    }
};

#if defined(HAVE_STD_VARIANT)

template<class W, typename T, typename... Ts>
class wxGenericValidatorCompositType<W, std::variant, T, Ts...> 
    : public wxGenericValidatorSimpleType<W, T>
{
    typedef std::variant<T, Ts...> CompositeType;

public:

    explicit wxGenericValidatorCompositType(CompositeType& data)
        : wxGenericValidatorSimpleType<W, T>(std::addressof(data))
    {
    }

    wxGenericValidatorCompositType(const wxGenericValidatorCompositType& val)
        : wxGenericValidatorSimpleType<W, T>(val)
    {
    }

    virtual ~wxGenericValidatorCompositType(){}

    virtual wxObject *Clone() const wxOVERRIDE { return new wxGenericValidatorCompositType(*this); }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        CompositeType* data = static_cast<CompositeType*>(this->m_data);

        auto value = std::get_if<T>(data);

        if ( value )
            return wxDataTransfer<W>::template To<T>(this->GetWindow(), value);

        return false;
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        CompositeType* data = static_cast<CompositeType*>(this->m_data);

        auto value = std::get_if<T>(data);

        if ( value && wxDataTransfer<W>::template From<T>(this->GetWindow(), value) )
            return true;

        return false;
    }
};

#endif // HAVE_STD_VARIANT
#endif // HAVE_VARIADIC_TEMPLATES

//-----------------------------------------------------------------------------
// convenience functions.
//-----------------------------------------------------------------------------

// Notice that using wxSetGenericValidator() is more convenient than calling
// wxGenericValidator<>() to set generic validators, as the latter can only deduce
// the passed value type.
    
template<class W, typename T>
inline wxGenericValidatorSimpleType<W, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorSimpleType<W, T>(value);
}

template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenericValidatorSimpleType<W, T>(value) );
}

#if defined(HAVE_VARIADIC_TEMPLATES)

template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline auto wxGenericValidator(TComposite<T, Ts...>& value)
{
    return wxGenericValidatorCompositType<W, TComposite, T, Ts...>(value);
}

template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline void wxSetGenericValidator(W* win, TComposite<T, Ts...>& value)
{
    win->SetValidator( wxGenericValidatorCompositType<W, TComposite, T, Ts...>(value) );
}

#endif // HAVE_VARIADIC_TEMPLATES

#else // !wxUSE_DATATRANSFER || !wxCAN_USE_DATATRANSFER

class WXDLLIMPEXP_FWD_BASE wxDateTime;
class WXDLLIMPEXP_FWD_BASE wxFileName;

// ----------------------------------------------------------------------------
// wxGenericValidator performs data transfer between many standard controls and
// variables of the type corresponding to their values.
//
// It doesn't do any validation so its name is a slight misnomer.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidator: public wxValidator
{
public:
    // Different constructors: each of them creates a validator which can only
    // be used with some controls, the comments before each constructor
    // indicate which ones:
        // wxCheckBox, wxRadioButton, wx(Bitmap)ToggleButton
    wxGenericValidator(bool* val);
        // wxChoice, wxGauge, wxRadioBox, wxScrollBar, wxSlider, wxSpinButton
    wxGenericValidator(int* val);
        // wxComboBox, wxTextCtrl, wxButton, wxStaticText (read-only)
    wxGenericValidator(wxString* val);
        // wxListBox, wxCheckListBox
    wxGenericValidator(wxArrayInt* val);
#if wxUSE_DATETIME
        // wxDatePickerCtrl
    wxGenericValidator(wxDateTime* val);
#endif // wxUSE_DATETIME
        // wxTextCtrl
    wxGenericValidator(wxFileName* val);
        // wxTextCtrl
    wxGenericValidator(float* val);
        // wxTextCtrl
    wxGenericValidator(double* val);

    wxGenericValidator(const wxGenericValidator& copyFrom);

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
    void Initialize();

    bool*       m_pBool;
    int*        m_pInt;
    wxString*   m_pString;
    wxArrayInt* m_pArrayInt;
#if wxUSE_DATETIME
    wxDateTime* m_pDateTime;
#endif // wxUSE_DATETIME
    wxFileName* m_pFileName;
    float*      m_pFloat;
    double*     m_pDouble;

private:
    wxDECLARE_CLASS(wxGenericValidator);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidator);
};

//-----------------------------------------------------------------------------
// convenience function.
//-----------------------------------------------------------------------------

template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenericValidator(static_cast<T*>(value)) );
}

#endif // wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
