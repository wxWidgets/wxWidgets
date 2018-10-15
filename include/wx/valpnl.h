/////////////////////////////////////////////////////////////////////////////
// Name:        wx/valpnl.h
// Purpose:     wxDataTransferImpl<> declarations
// Author:      Ali Kettab
// Created:     2018-10-07
// Copyright:   (c) 2018 Ali Kettab
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_VALPNLH__
#define _WX_VALPNLH__

#include "wx/valgen.h"

#if wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

#include "wx/panel.h"

// TODO: wxMultiValidatorPanel

#if defined(HAVE_STD_VARIANT)

#include <variant>
#include "wx/meta/typelist.h"

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

template<class W, typename T, class... Ws, typename... Ts>
class wxGenericValidatorCompositType<std::variant<W, Ws...>, std::variant, T, Ts...>
    : public wxGenericValidatorBase
{
    typedef std::variant<W*, Ws*...> WindowsType;
    typedef std::variant<T, Ts...> CompositeType;

    using WinDataTypes_ = 
        typename wxTypeList::TList<wxTypeList::TList<W, T>, 
                                   wxTypeList::TList<Ws, Ts>...>::Type;
    using WinDataTypes  = typename wxTypeList::EraseDuplType<WinDataTypes_>::Type;


    template<class W_, typename T_>
    inline auto CreateLambdaTo()
    {
        return [](W_* win, T_& value)
                {
                    return wxDataTransfer<W_>::template To<T_>(win, &value);
                };
    }

    template<typename... Pairs>
    inline auto CreateLambdaToVistitor(wxTypeList::TList<Pairs...>)
    {
        return wxVisitor{
                    [](auto*, auto&){ return false; },
                    CreateLambdaTo<typename wxTypeList::FirstType<Pairs>::Type,
                                   typename wxTypeList::LastType<Pairs>::Type>()...
                };
    }

    template<class W_, typename T_>
    inline auto CreateLambdaFrom()
    {
        return [](W_* win, T_& value)
                {
                    return wxDataTransfer<W_>::template From<T_>(win, &value);
                };
    }

    template<typename... Pairs>
    inline auto CreateLambdaFromVistitor(wxTypeList::TList<Pairs...>)
    {
        return wxVisitor{
                    [](auto*, auto&){ return false; },
                    CreateLambdaFrom<typename wxTypeList::FirstType<Pairs>::Type, 
                                     typename wxTypeList::LastType<Pairs>::Type>()...
                };
    }

    template<class W_>
    inline auto CreateLambdaValidate(wxWindow* parent)
    {
        return [=](W_* win)
                {
                    return wxDataTransfer<W_>::DoValidate(win, parent);
                };
    }


    template<class... Ws_>
    inline auto CreateValidateVisitor(wxWindow* parent, wxTypeList::TList<Ws_...>)
    {
        return wxVisitor{ CreateLambdaValidate<Ws_>(parent)... };
    }

public:

    wxGenericValidatorCompositType(WindowsType& wins, CompositeType& data)
        : wxGenericValidatorBase(std::addressof(data))
        , m_wins(wins)
    {
        static_assert((sizeof...(Ws)==sizeof...(Ts)), "Parameter packs don't match!");
    }

    wxGenericValidatorCompositType(const wxGenericValidatorCompositType& val)
        : wxGenericValidatorBase(val)
        , m_wins(val.m_wins)
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

        wxASSERT_MSG(wxDynamicCast(win, wxPanel) != NULL, "Invalid window type!");
    }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE
    {
        using WinTypes_ = typename wxTypeList::TList<W, Ws...>::Type;
        using WinTypes  = typename wxTypeList::EraseDuplType<WinTypes_>::Type;

        return std::visit(CreateValidateVisitor(parent, WinTypes{}), m_wins);
    }

    virtual bool TransferToWindow() wxOVERRIDE
    {
        CompositeType& data = *static_cast<CompositeType*>(this->m_data);

        return std::visit(CreateLambdaToVistitor(WinDataTypes{}), m_wins, data);
    }

    virtual bool TransferFromWindow() wxOVERRIDE
    {
        CompositeType& data = *static_cast<CompositeType*>(this->m_data);

        return std::visit(CreateLambdaFromVistitor(WinDataTypes{}), m_wins, data);
    }

private:
    WindowsType& m_wins;
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

template <std::size_t N, class... Args>
static inline void wxSetAlternative(std::variant<Args...>* var)
{
    var->template emplace<N>(typename wxTypeList::TypeAt<N, 
                                        wxTypeList::TList<Args...>>::Type{});
}

template <typename DataType, class... Ws>
class wxControlSelector
{
protected:
    wxControlSelector(DataType& data) : m_data(data){}

    wxControlSelector(const wxControlSelector&) = delete;
    wxControlSelector& operator=(const wxControlSelector&) = delete;

    void InitAlternatives(wxPanel* panel, Ws*... ws)
    {
        const bool b = (... && ws);
        wxASSERT_MSG(b, "Alternatives should not be null.");

        m_ctrls = std::make_tuple(ws...);

        auto ctrl = std::get<0>(m_ctrls);
        m_altCtrl.template emplace<0>(ctrl);
        wxSetAlternative<0>(&m_data);

        wxSetGenericValidator(panel, m_altCtrl, m_data);
    }

    bool SelectAlternative(int id)
    {
        return DoSelectAlternative<sizeof... (Ws)>(id);
    }

private:
    template <std::size_t N>
    bool DoSelectAlternative(int id)
    {
        if constexpr (N > 0)
        {
            auto ctrl = std::get<N-1>(m_ctrls);
            if ( ctrl->GetId() == id )
            {
                m_altCtrl.template emplace<N-1>(ctrl);
                wxSetAlternative<N-1>(&m_data);
                return true;
            }

            return DoSelectAlternative<N-1>(id);
        }
        else
        {
            return false;
        }
    }

protected:
    DataType& m_data;

    std::tuple<Ws*...> m_ctrls;
    std::variant<Ws*...> m_altCtrl;
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

template<typename DataType, class... Ws>
class wxMonoValidatorPanel 
    : public wxPanel
    , private wxControlSelector<DataType, Ws...>
{
    using BaseImpl = wxControlSelector<DataType, Ws...>;

public:
    wxMonoValidatorPanel(wxWindow *parent, DataType& data)
        : wxPanel(parent, wxID_ANY), BaseImpl(data)
    {
        Bind(wxEVT_TEXT, &wxMonoValidatorPanel::OnText, this);
    }

protected:
    template <std::size_t I>
    auto Get() const
    {
        return std::get<I>(this->m_ctrls);
    }

    void SetAlternatives(Ws*... ws)
    {
        this->InitAlternatives(this, ws...);
    }

    void OnText(wxCommandEvent& event)
    {
        if ( !this->SelectAlternative(event.GetId()) )
            event.Skip();
        else
            OnAlternativeSelected(event.GetId());
    }

    // Override this if you want to do something useful
    // at the time an alternative (control) is being selected.
    virtual void OnAlternativeSelected(int WXUNUSED(id)){}
};


#endif // defined(HAVE_STD_VARIANT)

#endif // wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

#endif // _WX_VALPNLH__
