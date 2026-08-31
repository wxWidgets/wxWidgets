/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hosttooltip.h
// Purpose:     Private helpers shared by host tooltip projection paths
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTTOOLTIP_H_
#define _WX_WINUI_HOSTTOOLTIP_H_

#include "private.h"

#if wxUSE_TOOLTIPS

// Intracore only: no owner, registry or callback lifetime is stored here.
// Slot state and policy depth stay on the existing host-owned slot; the
// managed-tooltip registry stays in ctrlhost.cpp. Both callers use this one
// suppression implementation without type erasure or callback allocation.
winrt::Windows::Foundation::IInspectable
wxWinUIReadToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content);

void wxWinUIRestoreToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Windows::Foundation::IInspectable& localValue);

// Consumes the existing host content-fault phase; it owns no duplicate mask.
void wxWinUIMaybeInjectToolTipContentFault();

enum class wxWinUIToolTipSuppressionResult
{
    Suppressed,
    Superseded,
    Failed,
    BudgetExhausted
};

// ToolTip::IsOpen(false) and ToolTipService::SetToolTip() are both
// synchronous dependency-property boundaries. An authored IsOpen/property
// callback is allowed to install another object at either boundary. Keep
// consuming those replacements in the same policy call so Enable(false)
// remains synchronous, but put a strict ceiling on hostile replacement
// chains. The real ToolTip::Closed event is delivered asynchronously by the
// Popup; the ToolTipProperty observer starts a fresh guarded transaction for
// replacements published from it.
template <typename IsCurrent>
wxWinUIToolTipSuppressionResult wxWinUISuppressSlotToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const IsCurrent& isCurrent,
    bool& owned,
    winrt::Windows::Foundation::IInspectable& applied,
    winrt::Windows::Foundation::IInspectable& original,
    winrt::Windows::Foundation::IInspectable& originalLocal)
{
    using winrt::Microsoft::UI::Xaml::Controls::ToolTip;
    using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

    constexpr unsigned MaxReplacementPasses = 8;
    for ( unsigned pass = 0; pass < MaxReplacementPasses; ++pass )
    {
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        const auto visible = ToolTipService::GetToolTip(content);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        if ( !visible )
            return wxWinUIToolTipSuppressionResult::Suppressed;

        const auto visibleLocal =
            wxWinUIReadToolTipLocalValue(content);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        const bool exactManagedValue =
            owned && applied && visible == applied &&
            visibleLocal == applied &&
            wxWinUIIsManagedToolTipCurrent(content, applied);
        if ( !exactManagedValue )
        {
            // This is a direct application replacement, including one
            // published by Closed or by the preceding detach setter. It is
            // the baseline that Enable(true) must restore by exact identity.
            const auto previouslyApplied = applied;
            original = visible;
            originalLocal = wxWinUIReadToolTipLocalValue(content);
            owned = false;
            applied = nullptr;

            if ( previouslyApplied )
            {
                // Retire only the registry entry carrying the identity we
                // used to own. A nested wxWinUISetToolTip() may already have
                // installed a newer entry for this element, and the
                // element-only Forget helper would incorrectly erase it.
                if ( !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         previouslyApplied) )
                {
                    return wxWinUIToolTipSuppressionResult::Failed;
                }
                if ( !isCurrent() )
                    return wxWinUIToolTipSuppressionResult::Superseded;

                const auto afterForget =
                    ToolTipService::GetToolTip(content);
                if ( !isCurrent() )
                    return wxWinUIToolTipSuppressionResult::Superseded;
                if ( afterForget != visible )
                {
                    original = afterForget;
                    if ( !afterForget )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Suppressed;
                    }
                    continue;
                }
            }
        }

        if ( const auto object = visible.try_as<ToolTip>() )
        {
            object.IsOpen(false);
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;

            // A synchronous IsOpen dependency-property callback may have
            // installed a replacement. Do not detach it using the stale
            // identity: the next pass adopts and closes it.
            const auto afterClose =
                ToolTipService::GetToolTip(content);
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;
            if ( afterClose != visible )
            {
                const auto previouslyApplied = applied;
                original = afterClose;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                owned = false;
                applied = nullptr;
                if ( previouslyApplied )
                {
                    if ( !wxWinUIRestoreToolTip(
                             content,
                             winrt::Microsoft::UI::Xaml::
                                 DependencyProperty::UnsetValue(),
                             previouslyApplied) )
                    {
                        return wxWinUIToolTipSuppressionResult::Failed;
                    }
                    if ( !isCurrent() )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Superseded;
                    }

                    const auto afterForget =
                        ToolTipService::GetToolTip(content);
                    if ( !isCurrent() )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Superseded;
                    }
                    if ( afterForget != afterClose )
                        original = afterForget;
                    if ( !afterForget )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Suppressed;
                    }
                }
                else if ( !afterClose )
                {
                    return wxWinUIToolTipSuppressionResult::Suppressed;
                }
                continue;
            }
        }

        ToolTipService::SetToolTip(content, nullptr);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        // Null is authoritative only after re-reading the property. A
        // property callback can publish yet another authored value from the
        // setter above; that object becomes the next pass's baseline.
        if ( !ToolTipService::GetToolTip(content) )
        {
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;
            return wxWinUIToolTipSuppressionResult::Suppressed;
        }
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;
    }

    return wxWinUIToolTipSuppressionResult::BudgetExhausted;
}

#endif // wxUSE_TOOLTIPS

#endif // _WX_WINUI_HOSTTOOLTIP_H_
