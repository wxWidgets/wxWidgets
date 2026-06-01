/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/checklst.cpp
// Purpose:     wxWinUI wxCheckListBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

bool wxCheckListBox::IsChecked(unsigned int item) const
{
    wxCHECK_MSG( item < m_checks.size(), false, wxT("invalid checklistbox index") );
    return m_checks[item];
}

void wxCheckListBox::Check(unsigned int item, bool check)
{
    wxCHECK_RET( item < m_checks.size(), wxT("invalid checklistbox index") );

    if ( m_checks[item] == check )
        return;

    m_checks[item] = check;

    // Rebuild the WinUI items so the corresponding CheckBox reflects the new
    // state.  The rebuild runs with the updating guard set, so it does not
    // re-enter through the CheckBox click handler.
    WinUIRefreshItems();
}

void wxCheckListBox::Toggle(unsigned int item)
{
    wxCHECK_RET( item < m_checks.size(), wxT("invalid checklistbox index") );
    Check(item, !m_checks[item]);
}

void wxCheckListBox::WinUIOnItemToggled(unsigned int n, bool check)
{
    if ( n >= m_checks.size() )
        return;

    m_checks[n] = check;

    wxCommandEvent event(wxEVT_CHECKLISTBOX, GetId());
    event.SetInt(static_cast<int>(n));
    event.SetEventObject(this);
    event.SetString(GetString(n));
    ProcessCommand(event);
}

#endif // wxUSE_CHECKLISTBOX
