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
