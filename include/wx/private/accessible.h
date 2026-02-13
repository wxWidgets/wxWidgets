///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/accessible.h
// Purpose:     Accessible helpers for owner-drawn controls
// Author:      Quin Gillespie
// Created:     2026-02-13
// Copyright:   (c) 2026 Quin Gillespie
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ACCESSIBLE_H_
#define _WX_PRIVATE_ACCESSIBLE_H_
#include "wx/access.h"
#if wxUSE_ACCESSIBILITY
// When controls such as wxCheckBox and wxRadioButton are switched to owner-drawn mode (e.g. to support custom foreground colours in dark mode), Windows replaces the native button style with BS_OWNERDRAW. The standard IAccessible implementation that Windows provides via CreateStdAccessibleObject() inspects the window style to determine the control role, so it reports owner-drawn controls as generic buttons.
// This base class provides the correct role and state for owner-drawn controls. It only takes effect when the control is actually owner-drawn; otherwise it returns wxACC_NOT_IMPLEMENTED to fall back to the standard Windows accessible.
template <class TControl>
class wxOwnerDrawnAccessible : public wxWindowAccessible
{
public:
    explicit wxOwnerDrawnAccessible(TControl* win) : wxWindowAccessible(win) { }

    virtual wxAccStatus GetRole(int childId, wxAccRole* role) override
    {
        if ( childId != wxACC_SELF ) return wxACC_NOT_IMPLEMENTED;
        TControl* ctrl = wxDynamicCast(GetWindow(), TControl);
        wxCHECK(ctrl, wxACC_FAIL);
        if ( !ctrl->IsOwnerDrawn() ) return wxACC_NOT_IMPLEMENTED;
        *role = MSWGetRole();
        return wxACC_OK;
    }

    virtual wxAccStatus GetState(int childId, long* state) override
    {
        if ( childId != wxACC_SELF ) return wxACC_NOT_IMPLEMENTED;
        TControl* ctrl = wxDynamicCast(GetWindow(), TControl);
        wxCHECK(ctrl, wxACC_FAIL);
        if ( !ctrl->IsOwnerDrawn() ) return wxACC_NOT_IMPLEMENTED;
        long st = 0;
        if ( !ctrl->IsEnabled() ) st |= wxACC_STATE_SYSTEM_UNAVAILABLE;
        if ( !ctrl->IsShown() ) st |= wxACC_STATE_SYSTEM_INVISIBLE;
        if ( ctrl->IsFocusable() ) st |= wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( ctrl->HasFocus() ) st |= wxACC_STATE_SYSTEM_FOCUSED;
        st |= MSWGetCheckedState(ctrl);
        *state = st;
        return wxACC_OK;
    }

protected:
    // Return the correct accessible role for the derived control type.
    virtual wxAccRole MSWGetRole() const = 0;
    // Return checked state flags (e.g. wxACC_STATE_SYSTEM_CHECKED) for the control.
    virtual long MSWGetCheckedState(TControl* ctrl) const = 0;
};
#endif // wxUSE_ACCESSIBILITY
#endif // _WX_PRIVATE_ACCESSIBLE_H_
