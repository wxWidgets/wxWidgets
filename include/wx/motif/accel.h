/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

#ifdef __GNUG__
#pragma interface "accel.h"
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/event.h"

class WXDLLEXPORT wxAcceleratorTable;

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

 // Hold no key down
#define wxACCEL_NORMAL  0x00

class WXDLLEXPORT wxAcceleratorEntry
{
public:
    wxAcceleratorEntry(const wxAcceleratorEntry& entry)
    {
        m_flags = entry.m_flags; m_keyCode = entry.m_keyCode; m_command = entry.m_command;
    }
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0)
    {
        m_flags = flags; m_keyCode = keyCode; m_command = cmd;
    }

    void Set(int flags, int keyCode, int cmd)
         { m_flags = flags; m_keyCode = keyCode; m_command = cmd; }

    int GetFlags() const { return m_flags; }
    int GetKeyCode() const { return m_keyCode; }
    int GetCommand() const { return m_command; }

    void operator = (const wxAcceleratorEntry& entry)
    {
        m_flags = entry.m_flags; m_keyCode = entry.m_keyCode; m_command = entry.m_command;
    }

    // Implementation use only
    bool MatchesEvent(const wxKeyEvent& event) const;

public:
    int          m_flags;
    int          m_keyCode; // ASCII or virtual keycode
    int          m_command; // Command id to generate
};

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(const wxString& resource); // Load from .rc resource
    wxAcceleratorTable(int n, wxAcceleratorEntry entries[]); // Load from array

    // Copy constructors
    wxAcceleratorTable(const wxAcceleratorTable& accel) { Ref(accel); }
    wxAcceleratorTable(const wxAcceleratorTable* accel) { if (accel) Ref(*accel); }

    ~wxAcceleratorTable();

    wxAcceleratorTable& operator = (const wxAcceleratorTable& accel) { if (*this == accel) return (*this); Ref(accel); return *this; }
    bool operator == (const wxAcceleratorTable& accel) { return m_refData == accel.m_refData; }
    bool operator != (const wxAcceleratorTable& accel) { return m_refData != accel.m_refData; }

    bool Ok() const;

// Implementation only
    int GetCount() const;
    wxAcceleratorEntry* GetEntries() const;
};

WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
