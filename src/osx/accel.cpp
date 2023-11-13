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
    m_refData = nullptr;
}

wxAcceleratorTable::~wxAcceleratorTable()
{
}

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
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

int wxAcceleratorTable::GetCommand( wxKeyEvent &event )
{
    if (!IsOk()) return -1;

    for ( const auto& entry : M_ACCELDATA->m_accels )
    {
        if ((event.m_keyCode == entry.GetKeyCode()) &&
           (((entry.GetFlags() & wxACCEL_RAW_CTRL) != 0) == event.RawControlDown()) &&
           (((entry.GetFlags() & wxACCEL_SHIFT) != 0) == event.ShiftDown()) &&
           (((entry.GetFlags() & wxACCEL_ALT) != 0) == event.AltDown()) &&
           (((entry.GetFlags() & wxACCEL_CTRL) != 0) == event.ControlDown()))
        {
            return entry.GetCommand();
        }
    }

    return -1;
}

#endif // wxUSE_ACCEL
