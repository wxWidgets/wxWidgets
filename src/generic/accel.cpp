///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/accel.cpp
// Purpose:     generic implementation of wxAcceleratorTable class
// Author:      Robert Roebling
// Modified:    VZ pn 31.05.01: use typed lists, Unicode cleanup, Add/Remove
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_ACCEL

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif // WX_PRECOMP

#include "wx/accel.h"

#include <ctype.h>

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class wxAccelRefData : public wxObjectRefData
{
public:
    wxAccelRefData()
    {
    }

    wxAccelRefData(const wxAccelRefData& data)
        : wxObjectRefData()
        , m_accels(data.m_accels)
    {
    }

    std::vector<wxAcceleratorEntry> m_accels;
};

// macro which can be used to access wxAccelRefData from wxAcceleratorTable
#define M_ACCELDATA ((wxAccelRefData *)m_refData)


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxAcceleratorTable ctors
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject);

wxAcceleratorTable::wxAcceleratorTable()
{
}

wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    if ( n == 0 )
    {
        // This is valid but useless.
        return;
    }

    wxCHECK_RET( n > 0, "Invalid number of accelerator entries" );

    m_refData = new wxAccelRefData;

    for ( int i = 0; i < n; i++ )
    {
        const wxAcceleratorEntry& entry = entries[i];

        int keycode = entry.GetKeyCode();
        if ( wxIsascii(keycode) )
            keycode = wxToupper(keycode);

        M_ACCELDATA->m_accels.emplace_back(entry.GetFlags(),
                                           keycode,
                                           entry.GetCommand());
    }
}

bool wxAcceleratorTable::IsOk() const
{
    return m_refData != nullptr;
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable updating
// ----------------------------------------------------------------------------

void wxAcceleratorTable::Add(const wxAcceleratorEntry& entry)
{
    AllocExclusive();

    if ( !m_refData )
    {
        m_refData = new wxAccelRefData;
    }

    M_ACCELDATA->m_accels.emplace_back(entry);
}

void wxAcceleratorTable::Remove(const wxAcceleratorEntry& entry)
{
    AllocExclusive();

    auto& accels = M_ACCELDATA->m_accels;

    int n = 0;
    for ( const auto& entryCur : accels )
    {
        // given entry contains only the information of the accelerator key
        // because it was set that way during creation so do not use the
        // comparison operator which also checks the command field
        if ((entryCur.GetKeyCode() == entry.GetKeyCode()) &&
            (entryCur.GetFlags() == entry.GetFlags()))
        {
            accels.erase(accels.begin() + n);

            if ( accels.empty() )
            {
                // wxAcceleratorEntry without any entries shouldn't be "ok", so
                // free the associated data to make it so
                UnRef();
            }

            return;
        }

        ++n;
    }

    wxFAIL_MSG(wxT("deleting inexistent accel from wxAcceleratorTable"));
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable: find a command for the given key press
// ----------------------------------------------------------------------------

const wxAcceleratorEntry *
wxAcceleratorTable::GetEntry(const wxKeyEvent& event) const
{
    if ( !IsOk() )
    {
        // not an error, the accel table is just empty
        return nullptr;
    }

    for ( const auto& entry : M_ACCELDATA->m_accels )
    {
        // is the key the same?
        if ( event.m_keyCode == entry.GetKeyCode() )
        {
            int flags = entry.GetFlags();

            // now check flags
            if ( (((flags & wxACCEL_CTRL) != 0) == event.ControlDown()) &&
                 (((flags & wxACCEL_SHIFT) != 0) == event.ShiftDown()) &&
                 (((flags & wxACCEL_ALT) != 0) == event.AltDown()) )
            {
                return &entry;
            }
        }
    }

    return nullptr;
}

wxMenuItem *wxAcceleratorTable::GetMenuItem(const wxKeyEvent& event) const
{
    const wxAcceleratorEntry *entry = GetEntry(event);

    return entry ? entry->GetMenuItem() : nullptr;
}

int wxAcceleratorTable::GetCommand(const wxKeyEvent& event) const
{
    const wxAcceleratorEntry *entry = GetEntry(event);

    return entry ? entry->GetCommand() : -1;
}

wxObjectRefData *wxAcceleratorTable::CreateRefData() const
{
    return new wxAccelRefData;
}

wxObjectRefData *wxAcceleratorTable::CloneRefData(const wxObjectRefData *data) const
{
    return new wxAccelRefData(*static_cast<const wxAccelRefData*>(data));
}

#endif // wxUSE_ACCEL
