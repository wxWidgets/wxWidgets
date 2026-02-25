/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Stefan Csomor
// Modified by:
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
// wxAccelList: a list of wxAcceleratorEntries
// ----------------------------------------------------------------------------

WX_DECLARE_LIST(wxAcceleratorEntry, wxAccelList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAccelList)

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class wxAcceleratorTable;
public:
    wxAcceleratorRefData()
    {}

    wxAcceleratorRefData(const wxAcceleratorRefData& data)
    : wxObjectRefData()
    {
        m_accels = data.m_accels;
    }
    
    virtual ~wxAcceleratorRefData()
    {
        WX_CLEAR_LIST(wxAccelList, m_accels);
    }

    wxAccelList m_accels;
};

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorTable::wxAcceleratorTable()
{
    m_refData = NULL;
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
        M_ACCELDATA->m_accels.Append( new wxAcceleratorEntry( flag, keycode, command ) );
    }
}

bool wxAcceleratorTable::IsOk() const
{
    return (m_refData != NULL);
}

int wxAcceleratorTable::GetCommand( wxKeyEvent &event )
{
    if (!IsOk()) return -1;

    wxAccelList::compatibility_iterator node = M_ACCELDATA->m_accels.GetFirst();
    while (node)
    {
        wxAcceleratorEntry *entry = node->GetData();
        if ((event.m_keyCode == entry->GetKeyCode()) &&
           (((entry->GetFlags() & wxACCEL_RAW_CTRL) != 0) == event.RawControlDown()) &&
           (((entry->GetFlags() & wxACCEL_SHIFT) != 0) == event.ShiftDown()) &&
           (((entry->GetFlags() & wxACCEL_ALT) != 0) == event.AltDown()) &&
           (((entry->GetFlags() & wxACCEL_CTRL) != 0) == event.ControlDown()))
        {
            return entry->GetCommand();
        }
        node = node->GetNext();
    }

    return -1;
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable updating
// ----------------------------------------------------------------------------

void wxAcceleratorTable::Add(const wxAcceleratorEntry& entry)
{
    AllocExclusive();
    
    if ( !m_refData )
    {
        m_refData = new wxAcceleratorRefData;
    }
    
    M_ACCELDATA->m_accels.Append(new wxAcceleratorEntry(entry));
}

void wxAcceleratorTable::Remove(const wxAcceleratorEntry& entry)
{
    AllocExclusive();
    
    wxAccelList::compatibility_iterator node = M_ACCELDATA->m_accels.GetFirst();
    while ( node )
    {
        const wxAcceleratorEntry *entryCur = node->GetData();
        
        // given entry contains only the information of the accelerator key
        // because it was set that way during creation so do not use the
        // comparison operator which also checks the command field
        if ((entryCur->GetKeyCode() == entry.GetKeyCode()) &&
            (entryCur->GetFlags() == entry.GetFlags()))
        {
            delete node->GetData();
            M_ACCELDATA->m_accels.Erase(node);
            
            return;
        }
        
        node = node->GetNext();
    }
    
    wxFAIL_MSG(wxT("deleting inexistent accel from wxAcceleratorTable"));
}

// Bricsys added: access an accelerator entry by key code and modifier flags so we can disable it
// Bricsys change: add support for entry enable/disable
wxAcceleratorEntry *
wxAcceleratorTable::GetEntry(int keyCode, int keyFlags)
{
    if ( !IsOk() )
    {
        // not an error, the accel table is just empty
        return NULL;
    }

    wxAccelList::compatibility_iterator node = M_ACCELDATA->m_accels.GetFirst();
    while ( node )
    {
        wxAcceleratorEntry *entry = node->GetData();

        // is the key the same?
        if ( keyCode == entry->GetKeyCode() )
        {
            int flags = entry->GetFlags();

            // now check flags
            if ( (((flags & wxACCEL_CTRL) != 0) == ((keyFlags & wxACCEL_CTRL) != 0)) &&
                 (((flags & wxACCEL_SHIFT) != 0) == ((keyFlags & wxACCEL_SHIFT) != 0)) &&
                 (((flags & wxACCEL_ALT) != 0) == ((keyFlags & wxACCEL_ALT) != 0)) )
            {
                return entry;
            }
        }

        node = node->GetNext();
    }

    return NULL;
}

void wxAcceleratorTable::EnableEntries( const int* entries, int length, bool enable )
{
    if ( !IsOk() )
    {
        // not an error, the accel table is just empty
        return;
    }

    wxAccelList::compatibility_iterator node = M_ACCELDATA->m_accels.GetFirst();
    while ( node )
    {
        wxAcceleratorEntry *entry = node->GetData();

        for( int i = 0; i < length; i++ )
        {
            int keyCode = entries[i*2];
            int keyFlags = entries[i*2+1];

            // is the key the same?
            if ( keyCode == entry->GetKeyCode() )
            {
                int flags = entry->GetFlags();

                // now check flags
                if ( (((flags & wxACCEL_CTRL) != 0) == ((keyFlags & wxACCEL_CTRL) != 0)) &&
                     (((flags & wxACCEL_SHIFT) != 0) == ((keyFlags & wxACCEL_SHIFT) != 0)) &&
                     (((flags & wxACCEL_ALT) != 0) == ((keyFlags & wxACCEL_ALT) != 0)) )
                {
                    entry->Enable( enable );
                }
            }
        }

        node = node->GetNext();
    }
}

wxObjectRefData *wxAcceleratorTable::CreateRefData() const
{
    return new wxAcceleratorRefData;
}

wxObjectRefData *wxAcceleratorTable::CloneRefData(const wxObjectRefData *data) const
{
    return new wxAcceleratorRefData(*(wxAcceleratorRefData *)data);
}

#endif // wxUSE_ACCEL
