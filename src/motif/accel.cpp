/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "accel.h"
#endif

#include "wx/setup.h"
#include "wx/accel.h"
#include "wx/string.h"
#include "wx/utils.h"
#include <ctype.h>

IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class WXDLLEXPORT wxAcceleratorTable;
public:
    wxAcceleratorRefData();
    ~wxAcceleratorRefData();

public:
    int m_count;
    wxAcceleratorEntry* m_entries;
};

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorRefData::wxAcceleratorRefData()
{
    m_count = 0;
    m_entries = (wxAcceleratorEntry*) NULL;
}

wxAcceleratorRefData::~wxAcceleratorRefData()
{
    delete[] m_entries;
    m_entries = (wxAcceleratorEntry*) NULL;
    m_count = 0;
}

wxAcceleratorTable::wxAcceleratorTable()
{
    m_refData = (wxAcceleratorRefData*) NULL;
}

wxAcceleratorTable::~wxAcceleratorTable()
{
    // Data deleted in ~wxObject
}

// Load from .rc resource
wxAcceleratorTable::wxAcceleratorTable(const wxString& WXUNUSED(resource))
{
    m_refData = new wxAcceleratorRefData;
}

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, wxAcceleratorEntry entries[])
{
    wxAcceleratorRefData* data = new wxAcceleratorRefData;
    m_refData = data;

    data->m_count = n;
    data->m_entries = new wxAcceleratorEntry[n];
    int i;
    for (i = 0; i < n; i++)
        data->m_entries[i] = entries[i];

}

bool wxAcceleratorTable::Ok() const
{
    return (m_refData != (wxAcceleratorRefData*) NULL);
}

int wxAcceleratorTable::GetCount() const
{
    return M_ACCELDATA->m_count;
}

wxAcceleratorEntry* wxAcceleratorTable::GetEntries() const
{
    return M_ACCELDATA->m_entries;
}

// Implementation use only
bool wxAcceleratorEntry::MatchesEvent(const wxKeyEvent& event) const
{
    bool eventAltDown = event.AltDown();
    bool eventCtrlDown = event.ControlDown();
    bool eventShiftDown = event.ShiftDown();
    int  eventKeyCode = event.KeyCode();

    bool accAltDown = ((GetFlags() & wxACCEL_ALT) == wxACCEL_ALT);
    bool accCtrlDown = ((GetFlags() & wxACCEL_CTRL) == wxACCEL_CTRL);
    bool accShiftDown = ((GetFlags() & wxACCEL_SHIFT) == wxACCEL_SHIFT);
    int  accKeyCode = GetKeyCode();
    int  accKeyCode2 = GetKeyCode();
    if (isascii(accKeyCode2))
        accKeyCode2 = tolower(accKeyCode2);

    return ((eventAltDown == accAltDown) && (eventCtrlDown == accCtrlDown) &&
        (eventShiftDown == accShiftDown) &&
        ((eventKeyCode == accKeyCode || eventKeyCode == accKeyCode2))) ;
}

