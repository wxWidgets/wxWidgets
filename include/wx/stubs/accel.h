/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

#ifdef __GNUG__
#pragma interface "accel.h"
#endif

#include "wx/object.h"

class WXDLLEXPORT wxAcceleratorTable;

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

class WXDLLEXPORT wxAcceleratorEntry
{
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0)
    {
        m_flags = flags; m_keyCode = keyCode; m_command = cmd;
    }

    inline void Set(int flags, int keyCode, int cmd)
         { m_flags = flags; m_keyCode = keyCode; m_command = cmd; }

    inline int GetFlags() const { return m_flags; }
    inline int GetKeyCode() const { return m_keyCode; }
    inline int GetCommand() const { return m_command; }

    int             m_flags;
    int			    m_keyCode; // ASCII or virtual keycode
    int			    m_command; // Command id to generate
};

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(const wxString& resource); // Load from .rc resource
    wxAcceleratorTable(int n, wxAcceleratorEntry entries[]); // Load from array

    // Copy constructors
    inline wxAcceleratorTable(const wxAcceleratorTable& accel) { Ref(accel); }
    inline wxAcceleratorTable(const wxAcceleratorTable* accel) { if (accel) Ref(*accel); }

    ~wxAcceleratorTable();

    inline wxAcceleratorTable& operator = (const wxAcceleratorTable& accel) { if (*this == accel) return (*this); Ref(accel); return *this; }
    inline bool operator == (const wxAcceleratorTable& accel) { return m_refData == accel.m_refData; }
    inline bool operator != (const wxAcceleratorTable& accel) { return m_refData != accel.m_refData; }

    bool Ok() const;
};

WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
