/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fddlgcmn.cpp
// Purpose:     common parts of wxFindReplaceDialog implementations
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.08.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FINDREPLDLG

#ifndef WX_PRECOMP
#endif

#include "wx/fdrepdlg.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxFindDialogEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_FIND, wxFindDialogEvent );
wxDEFINE_EVENT( wxEVT_FIND_NEXT, wxFindDialogEvent );
wxDEFINE_EVENT( wxEVT_FIND_REPLACE, wxFindDialogEvent );
wxDEFINE_EVENT( wxEVT_FIND_REPLACE_ALL, wxFindDialogEvent );
wxDEFINE_EVENT( wxEVT_FIND_CLOSE, wxFindDialogEvent );

// ============================================================================
// implementations
// ============================================================================

// ----------------------------------------------------------------------------
// wxFindReplaceData
// ----------------------------------------------------------------------------

void wxFindReplaceData::Init()
{
    m_Flags = 0;
}

// ----------------------------------------------------------------------------
// wxFindReplaceDialogBase
// ----------------------------------------------------------------------------

wxFindReplaceDialogBase::~wxFindReplaceDialogBase()
{
}

void wxFindReplaceDialogBase::Send(wxFindDialogEvent& event)
{
    // we copy the data to dialog->GetData() as well

    m_FindReplaceData->m_Flags = event.GetFlags();
    m_FindReplaceData->m_FindWhat = event.GetFindString();
    if ( HasFlag(wxFR_REPLACEDIALOG) &&
         (event.GetEventType() == wxEVT_FIND_REPLACE ||
          event.GetEventType() == wxEVT_FIND_REPLACE_ALL) )
    {
        m_FindReplaceData->m_ReplaceWith = event.GetReplaceString();
    }

    // translate wxEVT_FIND_NEXT to wxEVT_FIND if needed
    if ( event.GetEventType() == wxEVT_FIND_NEXT )
    {
        if ( m_FindReplaceData->m_FindWhat != m_lastSearch )
        {
            event.SetEventType(wxEVT_FIND);

            m_lastSearch = m_FindReplaceData->m_FindWhat;
        }
    }

    if ( !ProcessWindowEvent(event) )
    {
        // the event is not propagated upwards to the parent automatically
        // because the dialog is a top level window, so do it manually as
        // in 9 cases out of 10 the message must be processed by the dialog
        // owner and not the dialog itself
        if ( GetParent() )
            (void)GetParent()->ProcessWindowEvent(event);
    }
}

#endif // wxUSE_FINDREPLDLG

