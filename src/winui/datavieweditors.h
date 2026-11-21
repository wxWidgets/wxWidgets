/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/datavieweditors.h
// Purpose:     Causal lifetime of ephemeral native DataView popup editors
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_DATAVIEWEDITORS_H_
#define _WX_WINUI_DATAVIEWEDITORS_H_

#include "private.h"

// This is a transient fan-in child, admitted before the editor is published.
// Logical Finish/Cancel is immediate in the DataView model; only its native
// mount waits for exact popup closure and the resulting focus transaction.
// No callback in this class captures a wx object. The completion supplied by
// the adapter must independently validate its weak owner/edit generation.
class wxWinUIDataViewEditorRetirement final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<wxWinUIDataViewEditorRetirement>
{
    using Control = winrt::Microsoft::UI::Xaml::Controls::Control;
    using Canvas = winrt::Microsoft::UI::Xaml::Controls::Canvas;
    using ComboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox;
    using DatePicker = winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker;
    using Inspectable = winrt::Windows::Foundation::IInspectable;

public:
    static std::shared_ptr<wxWinUIDataViewEditorRetirement> Create(
        const Control& editor, const Canvas& mount,
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& hostFanIn)
    {
        if ( !editor || !mount || !hostFanIn )
            return {};
        auto state = std::shared_ptr<wxWinUIDataViewEditorRetirement>(
            new wxWinUIDataViewEditorRetirement(editor, mount));
        if ( !state->Initialize() ||
             !wxWinUIAddCompositePhysicalDisconnectGate(hostFanIn, state) )
        {
            return {};
        }
        return state;
    }

    ~wxWinUIDataViewEditorRetirement() override { Revoke(); }

    std::shared_ptr<wxWinUIPhysicalDisconnectGate> Gate()
        { return shared_from_this(); }

    bool IsPopupOpen() const noexcept
    {
        try
        {
            if ( const auto combo = m_editor.try_as<ComboBox>() )
                return combo.IsDropDownOpen();
            if ( const auto date = m_editor.try_as<DatePicker>() )
                return date.IsCalendarOpen();
        }
        catch ( ... ) { return true; }
        return false;
    }

    // Closed/IsOpen(false) can precede restoration of native focus. A queued
    // wx focus check is not a substitute for the exact popup's causal tail.
    // This observer never retires the editor; Retire() replaces its destination
    // if logical deletion wins before the tail. Already closed peers complete
    // synchronously, so the caller must check focus directly, not requeue here.
    bool AfterPopupClosed(std::function<void()> complete) noexcept
    {
        if ( m_retiring || m_terminal || m_sealed || !m_gate || !complete )
            return false;
        const std::weak_ptr<wxWinUIDataViewEditorRetirement> weak =
            shared_from_this();
        if ( !m_gate->SetCompletion(
                 [weak, complete = std::move(complete)]() mutable
                 {
                     const auto state = weak.lock();
                     if ( state && !state->m_retiring && !state->m_terminal &&
                          !state->m_sealed && !state->IsPopupOpen() )
                     {
                         complete();
                     }
                 }) )
        {
            Degrade();
            return false;
        }
        return true;
    }

    void Retire(std::function<void()> complete = {}) noexcept
    {
        if ( m_retiring || m_terminal )
            return;
        m_retiring = true;
        m_afterRetire = std::move(complete);
        // Resolve published-but-not-yet-notified debt BEFORE asking the gate
        // whether it is satisfied, including synchronous DropDownClosed.
        if ( m_combo )
            wxWinUIRequestComboPopupClose(m_combo);
        if ( m_calendar )
            wxWinUIRequestCalendarPopupClose(m_calendar);
        ArmCompletion();
    }

    bool IsSatisfied() const noexcept override
        { return m_satisfied && !m_sealed && !IsDegraded(); }
    bool IsDegraded() const noexcept override
        { return m_degraded || (m_gate && m_gate->IsDegraded()); }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_sealed; }
    winrt::Microsoft::UI::Dispatching::DispatcherQueue
    GetDispatcherQueue() const noexcept override
        { return m_queue; }

    bool SetCompletion(std::function<void()> complete) noexcept override
    {
        if ( !complete || m_terminal )
            return false;
        m_completion = std::move(complete);
        if ( IsSatisfied() )
            NotifyHost();
        return true;
    }

    void SealForHostShutdown() noexcept override
    {
        if ( m_sealed || m_terminal )
            return;
        m_sealed = true;
        m_retiring = true;
        m_afterRetire = {};
        m_gate->SealForHostShutdown();
        ArmCompletion();
    }
    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        if ( m_gate )
            m_gate->LatchPublishedPopupDebtBeforeSourceClose();
    }
    bool CompleteSealedWithoutPopup() noexcept override
    {
        return m_sealed && !IsDegraded() && m_gate &&
               m_gate->CompleteSealedWithoutPopup();
    }
    void CompleteAtXamlBoundary() noexcept override
    {
        if ( m_terminal )
            return;
        m_terminal = true;
        m_completion = {};
        m_afterRetire = {};
        if ( m_gate )
            m_gate->CompleteAtXamlBoundary();
        Revoke();
        ReleaseNative();
    }

private:
    wxWinUIDataViewEditorRetirement(const Control& editor, const Canvas& mount)
        : m_editor(editor), m_mount(mount), m_queue(editor.DispatcherQueue())
    {
    }

    bool Initialize()
    {
        if ( const auto combo = m_editor.try_as<ComboBox>() )
        {
            m_combo = wxWinUICreateComboPopupRetirementState(combo);
            m_gate = wxWinUIGetComboPopupRetirementGate(m_combo);
            const std::weak_ptr<wxWinUIDataViewEditorRetirement> weak =
                shared_from_this();
            m_opened = combo.DropDownOpened(
                [weak](const Inspectable&, const Inspectable&)
                {
                    if ( const auto state = weak.lock() )
                    {
                        wxWinUIObserveComboPopupOpened(state->m_combo);
                        if ( state->m_retiring )
                            wxWinUIRequestComboPopupClose(state->m_combo);
                    }
                });
            m_closed = combo.DropDownClosed(
                [weak](const Inspectable&, const Inspectable&)
                {
                    if ( const auto state = weak.lock() )
                        wxWinUIObserveComboPopupClosed(state->m_combo);
                });
        }
        else if ( const auto picker = m_editor.try_as<DatePicker>() )
        {
            m_calendar = wxWinUICreateCalendarPopupRetirementState(picker);
            m_gate = wxWinUIGetCalendarPopupRetirementGate(m_calendar);
        }
        return m_gate && !m_gate->IsDegraded();
    }

    void ArmCompletion() noexcept
    {
        const std::weak_ptr<wxWinUIDataViewEditorRetirement> weak =
            shared_from_this();
        if ( !m_gate || !m_gate->SetCompletion([weak]()
             {
                 if ( const auto state = weak.lock() )
                     state->Complete();
             }) )
        {
            Degrade();
        }
    }

    void Complete() noexcept
    {
        if ( m_terminal || m_satisfied || IsDegraded() )
            return;
        const auto keepAlive = shared_from_this();
        try
        {
            if ( !m_sealed && m_mount && m_editor )
            {
                uint32_t index = 0;
                if ( m_mount.Children().IndexOf(m_editor, index) )
                    m_mount.Children().RemoveAt(index);
            }
            Revoke();
            if ( IsDegraded() )
                return;
            m_satisfied = true;
            std::function<void()> after = std::move(m_afterRetire);
            ReleaseNative();
            NotifyHost();
            if ( after && !m_sealed )
                after();
        }
        catch ( ... ) { Degrade(); }
    }

    void NotifyHost() noexcept
    {
        auto complete = std::move(m_completion);
        try { if ( complete ) complete(); }
        catch ( ... ) { Degrade(); }
    }

    void Revoke() noexcept
    {
        try
        {
            if ( const auto combo = m_editor.try_as<ComboBox>() )
            {
                if ( m_opened.value ) combo.DropDownOpened(m_opened);
                if ( m_closed.value ) combo.DropDownClosed(m_closed);
            }
            m_opened = {};
            m_closed = {};
        }
        catch ( ... ) { Degrade(); }
    }

    void ReleaseNative() noexcept
    {
        m_editor = nullptr;
        m_mount = nullptr;
        m_combo.reset();
        m_calendar.reset();
    }

    void Degrade() noexcept
    {
        m_degraded = true;
        wxWinUINotePhysicalDisconnectGateDegraded();
    }

    Control m_editor{nullptr};
    Canvas m_mount{nullptr};
    winrt::Microsoft::UI::Dispatching::DispatcherQueue m_queue{nullptr};
    std::shared_ptr<wxWinUIComboPopupRetirementState> m_combo;
    std::shared_ptr<wxWinUICalendarPopupRetirementState> m_calendar;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> m_gate;
    std::function<void()> m_completion, m_afterRetire;
    winrt::event_token m_opened{}, m_closed{};
    bool m_retiring = false;
    bool m_satisfied = false;
    bool m_sealed = false;
    bool m_terminal = false;
    bool m_degraded = false;
};

#endif // _WX_WINUI_DATAVIEWEDITORS_H_
