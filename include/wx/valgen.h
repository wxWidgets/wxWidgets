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

#include "wx/private/datatransfer.h"

#if wxUSE_DATATRANSFER

#include "wx/debug.h"
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
    #include "wx/meta/typelist.h"
#endif // HAVE_STD_VARIANT

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
        return wxDataTransferImpl<Window>::To(
            static_cast<W*>(win), static_cast<T*>(data));
    }

    template<typename T>
    static bool From(wxWindow* win, void* data)
    {
        return wxDataTransferImpl<Window>::From(
            static_cast<W*>(win), static_cast<T*>(data));
    }

      // No validation by default.
      // specialize this function in your own code if you want 
      // to do any validation or filtering.
    static bool DoValidate(W* WXUNUSED(win), wxWindow* WXUNUSED(parent))
    { return true; }
};

// ----------------------------------------------------------------------------
// wxGenericValidatorBase 
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericValidatorBase : public wxValidator
{
public:

    wxGenericValidatorBase(const wxGenericValidatorBase& val);

    virtual ~wxGenericValidatorBase(){}

    virtual wxObject *Clone() const wxOVERRIDE 
    {
        return new wxGenericValidatorBase(*this); 
    }

protected:
    explicit wxGenericValidatorBase(void* data) : m_data(data){}

#ifndef wxNO_RTTI
    // Called by wxGenericValidatorSimpleType<wxWindow, T>::SetWindow() to set
    // the right validator based on the dynamic type of the m_validatorWindow.
    wxGenericValidatorBase* Convert(const std::type_info& tid) const;
#endif // wxNO_RTTI

    void* m_data;

private:
    wxDECLARE_CLASS(wxGenericValidatorBase);
    wxDECLARE_NO_ASSIGN_CLASS(wxGenericValidatorBase);
};

// ----------------------------------------------------------------------------
// Simple type (T) is any type supported directly by the window W. e.g. int,
// bool, wxString, wxArrayInt, ...
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

    virtual wxObject *Clone() const wxOVERRIDE 
    {
        return new wxGenericValidatorSimpleType(*this); 
    }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE;

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return wxDataTransfer<W>::template To<T>(this->GetWindow(), this->m_data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return wxDataTransfer<W>::template From<T>(this->GetWindow(), this->m_data);
    }
};

template<class W, typename T>
bool wxGenericValidatorSimpleType<W, T>::Validate(wxWindow* parent)
{
    return wxDataTransfer<W>::DoValidate(
        static_cast<W*>(this->GetWindow()), parent);
}

template<typename T>
class wxGenericValidatorSimpleType<wxWindow, T> : public wxGenericValidatorBase
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

    virtual wxObject *Clone() const wxOVERRIDE
    {
        return new wxGenericValidatorSimpleType(*this);
    }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        wxGenericValidatorBase::SetWindow(win);

    #ifndef wxNO_RTTI
        wxScopedPtr<wxGenericValidatorBase> val(
            wxGenericValidatorBase::Convert(typeid(T)));

        wxCHECK_RET( val, "Validator convertion failed" );

        if ( win )
            win->SetValidator(*val);
    #endif // wxNO_RTTI
    }
};

// ----------------------------------------------------------------------------
// Composit type (TComposite<>) is a pointer-like type or a value-like type
// having the form TComposite<T[, Ts...]> where T is a simple type (see above)
//
// Pointer-like types (e.g. std::unique_ptr, std::shared_ptr, wxScopedPtr...)
// Value-like types (e.g. std::optional)
//
// Requirements: TComposite<> should be dereferencable & swappable
//
// Notice that in order to support this type of validators, the window W should
// already know how to deal with the simple type T.
// ----------------------------------------------------------------------------

#if defined(HAVE_VARIADIC_TEMPLATES)
    template<class W, template<typename...> class TComposite, typename T, typename... Ts>
    #define COMPOSIT_TYPE TComposite<T, Ts...>
#else
    template<class W, template<typename> class TComposite, typename T>
    #define COMPOSIT_TYPE TComposite<T>
#endif // defined(HAVE_VARIADIC_TEMPLATES)
class wxGenericValidatorCompositType : public wxGenericValidatorBase
{
    typedef COMPOSIT_TYPE CompositeType;

    // Assigning (or re-assigning) new value to pointer-like types is different than
    // assigning (or re-assigning) new value to value-like types. so we need to resort
    // to the SFINAE mechanism to know what CompositeType represents at compile-time.
    // 
    // Hint: a standard conforming smart pointer should have a nested element_type defined.

    template <typename U>
    static auto IsSmartPtr(U const&) 
    -> decltype(typename U::element_type(), std::true_type())
    {
        return std::true_type{};
    }

    static std::false_type IsSmartPtr(...) { return std::false_type{}; }

    static auto NewValue(const T& value, const std::true_type&) 
    -> decltype(new T(value)){ return new T{value}; }

    static auto NewValue(const T& value, const std::false_type&) 
    -> decltype(value){ return value; }

public:

    explicit wxGenericValidatorCompositType(CompositeType& data)
        : wxGenericValidatorBase(std::addressof(data))
    {
    }

    wxGenericValidatorCompositType(const wxGenericValidatorCompositType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual ~wxGenericValidatorCompositType(){}

    virtual wxObject *Clone() const wxOVERRIDE 
    {
        return new wxGenericValidatorCompositType(*this);
    }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        this->m_validatorWindow = win; 

        wxASSERT_MSG((wxTypeId(*win) == wxTypeId(W)), "Invalid window type!");
    }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE
    {
        return wxDataTransfer<W>::DoValidate(
            static_cast<W*>(this->GetWindow()), parent);
    }

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
            // FIXME 1: 'data' should not be assigned any value if 'value' is not valid.
            //          
            // FIXME 2: wxSharedPtr<> can't be used with this class as it doesn't
            //          define the standard member function swap().

            CompositeType tmp(NewValue(value, IsSmartPtr(data)));

            data.swap(tmp);
        }

        return true;
    }
};

#if defined(HAVE_STD_VARIANT)

template<class... Ls> struct wxVisitor : Ls... { using Ls::operator()...; };
template<class... Ls> wxVisitor(Ls...) -> wxVisitor<Ls...>;

#endif // defined(HAVE_STD_VARIANT)

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

template<typename T>
inline wxGenericValidatorSimpleType<wxWindow, T> wxGenericValidator(T* value)
{
    return wxGenericValidatorSimpleType<wxWindow, T>(value);
}

template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenericValidatorSimpleType<W, T>(value) );
}

#if defined(HAVE_VARIADIC_TEMPLATES)
template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline auto wxGenericValidator(TComposite<T, Ts...>& value)
-> decltype(wxGenericValidatorCompositType<W, TComposite, T, Ts...>(value))
{
    return wxGenericValidatorCompositType<W, TComposite, T, Ts...>(value);
}

template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline void wxSetGenericValidator(W* win, TComposite<T, Ts...>& value)
{
    win->SetValidator( wxGenericValidatorCompositType<W, TComposite, T, Ts...>(value) );
}

template<class Panel, 
         template<typename...> class TWindows, 
         template<typename...> class TComposite,
         class W, typename T, class... Ws, typename... Ts>
inline void wxSetGenericValidator(Panel* panel, 
                                  TWindows<W*, Ws*...>& wins, 
                                  TComposite<T, Ts...>& value)
{
    panel->SetValidator( 
        wxGenericValidatorCompositType<TWindows<W, Ws...>, 
                                       TComposite, T, Ts...>{wins, value} );
}

#else // !defined(HAVE_VARIADIC_TEMPLATES)
template<class W, template<typename> class TComposite, typename T>
inline auto wxGenericValidator(TComposite<T>& value)
-> decltype(wxGenericValidatorCompositType<W, TComposite, T>(value))
{
    return wxGenericValidatorCompositType<W, TComposite, T>(value);
}

template<class W, template<typename> class TComposite, typename T>
inline void wxSetGenericValidator(W* win, TComposite<T>& value)
{
    win->SetValidator( wxGenericValidatorCompositType<W, TComposite, T>(value) );
}
#endif // defined(HAVE_VARIADIC_TEMPLATES)

#else // !wxUSE_DATATRANSFER

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

#endif // wxUSE_DATATRANSFER

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
