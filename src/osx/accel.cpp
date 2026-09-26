/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_ACCEL

#include "wx/accel.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject);

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class wxAcceleratorTable;
public:
    wxAcceleratorRefData() = default;

    std::vector<wxAcceleratorEntry> m_accels;
};

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorTable::wxAcceleratorTable()
{
}

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    if ( n == 0 )
    {
        // This is valid but useless.
        return;
    }

    wxCHECK_RET( n > 0, "Invalid number of accelerator entries" );

    m_refData = new wxAcceleratorRefData;

    for (int i = 0; i < n; i++)
    {
        int flag    = entries[i].GetFlags();
        int keycode = entries[i].GetKeyCode();
        int command = entries[i].GetCommand();
        if ((keycode >= (int)'a') && (keycode <= (int)'z')) keycode = (int)toupper( (char)keycode );
        M_ACCELDATA->m_accels.emplace_back( flag, keycode, command );
    }
}

bool wxAcceleratorTable::IsOk() const
{
    return (m_refData != nullptr);
}

const wxAcceleratorEntry *
wxAcceleratorTable::GetEntry(const wxKeyEvent& event) const
{
    if (!IsOk()) return nullptr;

    for ( const auto& entry : M_ACCELDATA->m_accels )
    {
        if ( entry.MatchesEvent(event) )
            return &entry;
    }

    return nullptr;
}

int wxAcceleratorTable::GetCommand( wxKeyEvent &event )
{
    const wxAcceleratorEntry* const entry = GetEntry(event);

    return entry ? entry->GetCommand() : -1;
}

#endif // wxUSE_ACCEL
