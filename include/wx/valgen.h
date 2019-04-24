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

#if wxUSE_VALIDATOR_DATATRANSFER

#include "wx/debug.h"
#include "wx/any.h"

#if defined(HAVE_TYPE_TRAITS)
    #include <type_traits>
#elif defined(HAVE_TR1_TYPE_TRAITS)
    #ifdef __VISUALC__
        #include <type_traits>
    #else
        #include <tr1/type_traits>
    #endif
#endif

#if defined(HAVE_STD_VARIANT)
    #include <variant>
    #include "wx/meta/typelist.h"
#endif // HAVE_STD_VARIANT

class WXDLLIMPEXP_FWD_CORE wxPanel;

template<class W>
struct wxValidatorDataTransfer
{
      // decltype will give us the window type (a pointer type) which
      // implements (specializes) wxValidatorDataTransferImpl<> interface.
      // or void* if no such specialization exists for W type.
    typedef decltype(wxGET_VALIDATOR_DATATRANSFER_IMPLEMENTOR(W)) Type;
    typedef typename std::remove_pointer<Type>::type Window;

      // always cast @win to the parameter @W and not to @Window type
      // as the latter may be resolved to a base not necessarily derived
      // from wxWindow, and the cast would fail as a consequence!
    template<typename T>
    static bool To(wxWindow* win, void* data)
    {
        return wxValidatorDataTransferImpl<Window>::To(
            static_cast<W*>(win), static_cast<T*>(data));
    }

    template<typename T>
    static bool From(wxWindow* win, void* data)
    {
        return wxValidatorDataTransferImpl<Window>::From(
            static_cast<W*>(win), static_cast<T*>(data));
    }

      // No validation by default.
      // Client code may specialize this function to do any validation or filtering.
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

    virtual wxObject *Clone() const wxOVERRIDE 
        { return new wxGenericValidatorBase(*this); }

protected:
    explicit wxGenericValidatorBase(void* data) : m_data(data){}

    // Called by wxGenValidatorSimpleType<wxWindow, T>::SetWindow() to set
    // the right validator based on the dynamic type of the m_validatorWindow.
    wxGenericValidatorBase* Convert(const wxAny& any) const;

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
class wxGenValidatorSimpleType : public wxGenericValidatorBase
{
public:
    explicit wxGenValidatorSimpleType(T* data)
        : wxGenericValidatorBase(data)
    {
    }

    wxGenValidatorSimpleType(const wxGenValidatorSimpleType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual wxObject *Clone() const wxOVERRIDE 
        { return new wxGenValidatorSimpleType(*this); }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE;

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return wxValidatorDataTransfer<W>::template
                    To<T>(this->GetWindow(), this->m_data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return wxValidatorDataTransfer<W>::template
                    From<T>(this->GetWindow(), this->m_data);
    }
};

// This method will be instantiated in user code, and by making it a non-inline
// function, we ensure that the right specialisation ( of DoValidate() if any )
// will be used.
template<class W, typename T>
bool wxGenValidatorSimpleType<W, T>::Validate(wxWindow* parent)
{
    return wxValidatorDataTransfer<W>::DoValidate(
        static_cast<W*>(this->GetWindow()), parent);
}

// ----------------------------------------------------------------------------
// A non-functional intermediary validator that can be converted to the
// appropriate validator at runtime based on the dynamic type of the validator
// window and T.
// ----------------------------------------------------------------------------

template<typename T>
class wxGenValidatorSimpleType<wxWindow, T> : public wxGenericValidatorBase
{
public:
    explicit wxGenValidatorSimpleType(T* data)
        : wxGenericValidatorBase(data)
    {
    }

    wxGenValidatorSimpleType(const wxGenValidatorSimpleType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual wxObject *Clone() const wxOVERRIDE
        { return new wxGenValidatorSimpleType(*this); }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        wxCHECK_RET( win, "Invalid window pointer" );

        wxGenericValidatorBase::SetWindow(win);

        wxScopedPtr<wxGenericValidatorBase> val(
            wxGenericValidatorBase::Convert(wxAny(T())));

        wxCHECK_RET( val, "Validator convertion failed" );

        win->SetValidator(*val);
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
class wxGenValidatorCompositType : public wxGenericValidatorBase
{
    typedef COMPOSIT_TYPE CompositeType;

    // Helper function
    CompositeType& GetData() const
        { return *static_cast<CompositeType*>(this->m_data); }

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
    explicit wxGenValidatorCompositType(CompositeType& data)
        : wxGenericValidatorBase(std::addressof(data))
    {
    }

    wxGenValidatorCompositType(const wxGenValidatorCompositType& val)
        : wxGenericValidatorBase(val)
    {
    }

    virtual wxObject *Clone() const wxOVERRIDE 
        { return new wxGenValidatorCompositType(*this); }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE
    {
        return wxValidatorDataTransfer<W>::DoValidate(
            static_cast<W*>(this->GetWindow()), parent);
    }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        CompositeType& data = this->GetData();
        
        if ( !data )
            return true;

        return wxValidatorDataTransfer<W>::template To<T>(this->GetWindow(), &*data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        T value;

        if ( !wxValidatorDataTransfer<W>::template From<T>(this->GetWindow(), &value) )
            return false;

        CompositeType& data = this->GetData();

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

// ----------------------------------------------------------------------------
// wxMonoValidationEvent
// ----------------------------------------------------------------------------
/*
    wxEVT_SET_ALTERNATIVE
    wxEVT_UNSET_ALTERNATIVE
 */

class WXDLLIMPEXP_CORE wxMonoValidationEvent : public wxEvent
{
public:
    wxMonoValidationEvent(wxEventType type = wxEVT_NULL, int winid = 0)
        : wxEvent(winid, type)
        { m_win = NULL; }

    wxMonoValidationEvent(const wxMonoValidationEvent& event)
        : wxEvent(event)
        { m_win = event.m_win; }

    // The window associated with this event is the window which becomes the
    // active window for SET event and the window which was the active one for
    // the UNSET event.
    wxWindow *GetWindow() const { return m_win; }
    void SetWindow(wxWindow *win) { m_win = win; }

    virtual wxEvent *Clone() const wxOVERRIDE
        { return new wxMonoValidationEvent(*this); }

private:
    wxWindow *m_win;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMonoValidationEvent);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SET_ALTERNATIVE, wxMonoValidationEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_UNSET_ALTERNATIVE, wxMonoValidationEvent);

typedef void (wxEvtHandler::*wxMonoValidationFunction)(wxMonoValidationEvent&);

#define wxMonoValidationEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMonoValidationFunction, func)

#define EVT_SET_ALTERNATIVE(func) \
    wx__DECLARE_EVT0(wxEVT_SET_ALTERNATIVE, wxMonoValidationEventHandler(func))
#define EVT_UNSET_ALTERNATIVE(func) \
    wx__DECLARE_EVT0(wxEVT_UNSET_ALTERNATIVE, wxMonoValidationEventHandler(func))

// ----------------------------------------------------------------------------
// Specialisation to implement a mono validator class
// ----------------------------------------------------------------------------

template<class W, typename T, class... Ws, typename... Ts>
class wxGenValidatorCompositType<std::variant<W, Ws...>, std::variant, T, Ts...>
    : public wxGenericValidatorBase
{
    typedef std::variant<T, Ts...> CompositeType;

    // To make sure that each wxVisitor will be instantiated with a list of
    // unique types only, we extract a list of unique pairs {Y, U} from the
    // parallel lists {W, Ws...} and {T, Ts...} and create our wxVisitors
    // based on the resulting list.

    using WinDataTypes_ =
        typename wxTypeList::TList<wxTypeList::TList<W, T>,
                                   wxTypeList::TList<Ws, Ts>...>::Type;
    using WinDataTypes  = typename wxTypeList::EraseDuplType<WinDataTypes_>::Type;

    // Helper function
    CompositeType& GetData() const
        { return *static_cast<CompositeType*>(this->m_data); }

    // N.B. The CreateVisitorXXX() below have WXUNUSED(tag) as parameter just
    //      to help the compiler deduce the template parameter types.

    // Create a visitor to transfer data _To_ the active window:
    // ---------------------------------------------------------

    template<class Y, typename U>
    inline auto CreateLambdaTo()
    {
        return [](Y* win, U& value)
                {
                    return wxValidatorDataTransfer<Y>::template To<U>(win, &value);
                };
    }

    template<typename... Pairs>
    inline auto CreateVisitorTo(wxTypeList::TList<Pairs...> WXUNUSED(tag))
    {
        return wxVisitor{
                    [](auto*, auto&){ return false; },
                    CreateLambdaTo<typename wxTypeList::FirstType<Pairs>::Type,
                                   typename wxTypeList::LastType<Pairs>::Type>()...
                };
    }

    // Create a visitor to transfer data _From_ the active window:
    // -----------------------------------------------------------

    template<class Y, typename U>
    inline auto CreateLambdaFrom()
    {
        return [](Y* win, U& value)
                {
                    return wxValidatorDataTransfer<Y>::template From<U>(win, &value);
                };
    }

    template<typename... Pairs>
    inline auto CreateVisitorFrom(wxTypeList::TList<Pairs...> WXUNUSED(tag))
    {
        return wxVisitor{
                    [](auto*, auto&){ return false; },
                    CreateLambdaFrom<typename wxTypeList::FirstType<Pairs>::Type,
                                     typename wxTypeList::LastType<Pairs>::Type>()...
                };
    }

    // Create a visitor to validate the active window:
    // -----------------------------------------------

    template<class Y>
    inline auto CreateLambdaValidate(wxWindow* parent)
    {
        return [=](Y* win)
                {
                    return wxValidatorDataTransfer<Y>::DoValidate(win, parent);
                };
    }

    template<class... Ys>
    inline auto CreateVisitorValidate(wxWindow* parent,
                                      wxTypeList::TList<Ys...> WXUNUSED(tag))
    {
        return wxVisitor{ CreateLambdaValidate<Ys>(parent)... };
    }

    // Create a visitor which returns the active window:
    // -------------------------------------------------

    template<class Y>
    inline auto CreateLambdaAlternative()
    {
        return [](Y* win) { return static_cast<wxWindow*>(win); };
    }

    template<class... Ys>
    inline auto CreateVisitorAlternative(wxTypeList::TList<Ys*...> WXUNUSED(tag))
    {
        return wxVisitor{ CreateLambdaAlternative<Ys>()... };
    }

public:
    wxGenValidatorCompositType(CompositeType& data, const std::tuple<W*, Ws*...>& alts)
        : wxGenericValidatorBase(std::addressof(data))
        , m_alternatives(alts)
    {
        static_assert((sizeof...(Ws)==sizeof...(Ts)), "Parameter packs don't match!");
    }

    wxGenValidatorCompositType(const wxGenValidatorCompositType& val)
        : wxGenericValidatorBase(val)
        , m_alternatives(val.m_alternatives)
        , m_activeCtrl(val.m_activeCtrl)
    {
    }

    virtual wxObject *Clone() const wxOVERRIDE
        { return new wxGenValidatorCompositType(*this); }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE
    {
        wxCHECK_RET(wxDynamicCast(win, wxPanel) != NULL, "Invalid window type!");

        wxGenericValidatorBase::SetWindow(win);

        win->Bind(wxEVT_TEXT, &wxGenValidatorCompositType::OnText, this);

        win->Bind(wxEVT_SET_ALTERNATIVE,
            &wxGenValidatorCompositType::OnAlternativeChanged, this);
        win->Bind(wxEVT_UNSET_ALTERNATIVE,
            &wxGenValidatorCompositType::OnAlternativeChanged, this);

        InitAlternatives();
    }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE
    {
        using WinTypes_ = typename wxTypeList::TList<W, Ws...>::Type;
        using WinTypes  = typename wxTypeList::EraseDuplType<WinTypes_>::Type;

        return std::visit(CreateVisitorValidate(parent, WinTypes{}), m_activeCtrl);
    }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        return std::visit(CreateVisitorTo(WinDataTypes{}), m_activeCtrl, this->GetData());
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        return std::visit(CreateVisitorFrom(WinDataTypes{}), m_activeCtrl, this->GetData());
    }

private:
    void InitAlternatives()
    {
        // By default, set the first control as the active alternative.
        auto ctrl = std::get<0>(m_alternatives);
        m_activeCtrl.template emplace<0>(ctrl);

        DoSetAlternative<0>(&this->GetData());

        wxMonoValidationEvent event(wxEVT_SET_ALTERNATIVE, ctrl->GetId());
        event.SetEventObject(this->GetWindow());
        event.SetWindow(ctrl);

        // I expected that calling this->GetWindow()->HandleWindowEvent(event);
        // would work correctly, but it doesn't! i.e. the event will be processed
        // by the event handler defined here instead of the event handler defined
        // in the user code (if any). So we resort to wxQueueEvent().
        wxQueueEvent(this->GetWindow(), event.Clone());
    }

    template <std::size_t N>
    void DoSetAlternative(std::variant<T, Ts...>* var)
    {
        var->template emplace<N>(
            typename wxTypeList::TypeAt<N, wxTypeList::TList<T, Ts...>>::Type{});
    }

    template <std::size_t N>
    wxWindow* DoSetAlternative(int id)
    {
        if constexpr ( N == 0 )
        {
            return nullptr;
        }
        else
        {
            auto ctrl = std::get<N-1>(this->m_alternatives);
            if ( ctrl->GetId() == id )
            {
                m_activeCtrl.template emplace<N-1>(ctrl);
                DoSetAlternative<N-1>(&this->GetData());
                return ctrl;
            }

            return DoSetAlternative<N-1>(id);
        }
    }

    // Event handlers:
    // ---------------

    void OnText(wxCommandEvent& event)
    {
        using WinTypes_ = typename wxTypeList::TList<W*, Ws*...>::Type;
        using WinTypes  = typename wxTypeList::EraseDuplType<WinTypes_>::Type;

        wxWindow* const win = wxDynamicCast(event.GetEventObject(), wxWindow);

        if ( !win ) /* should not happen */
            return;

        wxWindow* altWin = std::visit(CreateVisitorAlternative(WinTypes{}), m_activeCtrl);

        if ( altWin == win )
            return;

        wxMonoValidationEvent evt(wxEVT_UNSET_ALTERNATIVE, altWin->GetId());
        evt.SetEventObject(this->GetWindow());
        evt.SetWindow(altWin);
        this->GetWindow()->HandleWindowEvent(evt);

        altWin = DoSetAlternative<(1+sizeof... (Ws))>(win->GetId());

        evt.SetEventType(wxEVT_SET_ALTERNATIVE);
        evt.SetWindow(altWin);
        evt.SetId(altWin ? altWin->GetId() : 0);
        this->GetWindow()->HandleWindowEvent(evt);
    }

    void OnAlternativeChanged(wxMonoValidationEvent& event)
    {
        wxWindow* const win = event.GetWindow();

        if ( !win )
            return;

        if ( event.GetEventType() == wxEVT_SET_ALTERNATIVE )
        {
            // A very basic way to highlight the active control.
            win->SetBackgroundColour(wxColour("#f2bdcd")); // Orchid pink
        }
        else
        {
            // Only the active control may have a validator associated with it.
            win->SetValidator(wxDefaultValidator);
            win->SetBackgroundColour(wxNullColour);
        }
    }

private:
    const std::tuple<W*, Ws*...> m_alternatives;
    std::variant<W*, Ws*...> m_activeCtrl;
};

#endif // defined(HAVE_STD_VARIANT)

//-----------------------------------------------------------------------------
// convenience functions.
//-----------------------------------------------------------------------------

// Notice that using wxSetGenericValidator() is more convenient than calling
// wxGenericValidator<>() to set generic validators, as the latter can only deduce
// the passed value type.

template<class W, typename T>
inline wxGenValidatorSimpleType<W, T> wxGenericValidator(T* value)
{
    return wxGenValidatorSimpleType<W, T>(value);
}

// This function returns an intermediary (non-functional) validator that will be
// converted to a concrete one (upon the call to SetWindow()) constructed from
// the dynamic type of the associated window and T. And by this, the old code
// continue to work perfectly.
template<typename T>
inline wxGenValidatorSimpleType<wxWindow, T> wxGenericValidator(T* value)
{
    return wxGenValidatorSimpleType<wxWindow, T>(value);
}

template<class W, typename T>
inline void wxSetGenericValidator(W* win, T* value)
{
    win->SetValidator( wxGenValidatorSimpleType<W, T>(value) );
}

#if defined(HAVE_VARIADIC_TEMPLATES)
template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline auto wxGenericValidator(TComposite<T, Ts...>& value)
-> decltype(wxGenValidatorCompositType<W, TComposite, T, Ts...>(value))
{
    return wxGenValidatorCompositType<W, TComposite, T, Ts...>(value);
}

template<class W, template<typename...> class TComposite, typename T, typename... Ts>
inline void wxSetGenericValidator(W* win, TComposite<T, Ts...>& value)
{
    win->SetValidator( wxGenValidatorCompositType<W, TComposite, T, Ts...>(value) );
}

#else // !defined(HAVE_VARIADIC_TEMPLATES)
template<class W, template<typename> class TComposite, typename T>
inline auto wxGenericValidator(TComposite<T>& value)
-> decltype(wxGenValidatorCompositType<W, TComposite, T>(value))
{
    return wxGenValidatorCompositType<W, TComposite, T>(value);
}

template<class W, template<typename> class TComposite, typename T>
inline void wxSetGenericValidator(W* win, TComposite<T>& value)
{
    win->SetValidator( wxGenValidatorCompositType<W, TComposite, T>(value) );
}
#endif // defined(HAVE_VARIADIC_TEMPLATES)

WXDLLIMPEXP_CORE void wxSetGenericValidator(wxPanel* panel, const wxValidator& val);

#if defined(HAVE_STD_VARIANT)
template<typename T, typename... Ts, class... Ws>
inline void wxSetGenericValidator(wxPanel* panel,
                                  std::variant<T, Ts...>& value,
                                  const std::tuple<Ws*...>& alts)
{
    wxSetGenericValidator(panel,
        wxGenValidatorCompositType<
            std::variant<Ws...>, std::variant, T, Ts...>{value, alts});
}
#endif // HAVE_STD_VARIANT

#else // !wxUSE_VALIDATOR_DATATRANSFER

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

#endif // wxUSE_VALIDATOR_DATATRANSFER

#endif // wxUSE_VALIDATORS

#endif // _WX_VALGENH__
