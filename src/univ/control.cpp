/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/control.cpp
// Purpose:     universal wxControl: adds handling of mnemonics
// Author:      Vadim Zeitlin
// Created:     14.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_CONTROLS

#include "wx/control.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxControl, wxWindow);

wxBEGIN_EVENT_TABLE(wxControl, wxControlBase)
    WX_EVENT_TABLE_INPUT_CONSUMER(wxControl)
wxEND_EVENT_TABLE()

WX_FORWARD_TO_INPUT_CONSUMER(wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxControl::Init()
{
    m_indexAccel = -1;
}

bool wxControl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    if ( !wxControlBase::Create(parent, id, pos, size, style, validator, name) )
    {
        // underlying window creation failed?
        return false;
    }

    return true;
}

wxControl::~wxControl()
{
#if wxUSE_ACCEL
    wxChar accelChar = GetAccelChar();

    if ( accelChar != wxNO_ACCEL_CHAR )
    {
        if ( wxWindow *win = wxGetTopLevelParent(this) )
        {
            wxAcceleratorEntry accelEntry(wxACCEL_ALT, (int)accelChar);
            wxAcceleratorTable *accelTable = win->GetAcceleratorTable();
            if ( accelTable && accelTable->IsOk() )
                accelTable->Remove(accelEntry);
        }
    }
#endif // wxUSE_ACCEL
}

// ----------------------------------------------------------------------------
// mnemonics handling
// ----------------------------------------------------------------------------

void wxControl::SetLabel(const wxString& label)
{
    // save original label
    wxControlBase::SetLabel(label);

    UnivDoSetLabel(label);
}

void wxControl::UnivDoSetLabel(const wxString& label)
{
    wxString labelOld = m_label;
    int indexAccelOld = m_indexAccel;
#if wxUSE_ACCEL
    wxChar accelCharOld = GetAccelChar();
#endif // wxUSE_ACCEL

    m_indexAccel = FindAccelIndex(label, &m_label);

#if wxUSE_ACCEL
    wxChar accelChar = GetAccelChar();

    if ( accelCharOld != accelChar )
    {
        if ( wxWindow *win = wxGetTopLevelParent(this) )
        {
            if ( wxAcceleratorTable *accelTable = win->GetAcceleratorTable() )
            {
                // remove entry only from valid accelerator table
                if ( ( accelCharOld != wxNO_ACCEL_CHAR ) && accelTable->IsOk() )
                {
                    wxAcceleratorEntry accelEntryOld(wxACCEL_ALT, (int)accelCharOld);
                    accelTable->Remove(accelEntryOld);
                }

                // accelerator table doesn't have to be valid to add
                if ( accelChar != wxNO_ACCEL_CHAR )
                {
                    wxAcceleratorEntry accelEntryNew(wxACCEL_ALT, (int)accelChar, GetId());
                    accelTable->Add(accelEntryNew);
                }
            }
        }
    }
#endif // wxUSE_ACCEL

    if ( ( m_label != labelOld ) || ( m_indexAccel != indexAccelOld ) )
    {
        Refresh();
    }
}

#endif // wxUSE_CONTROLS
