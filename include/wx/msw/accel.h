/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Julian Smart
// Modified by:
// Created:     31/7/98
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

class WXDLLEXPORT wxAcceleratorTable;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

 // Hold no other key
#define wxACCEL_NORMAL  0x00

// ----------------------------------------------------------------------------
// an entry in wxAcceleratorTable corresponds to one accelerator
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorEntry
{
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0)
    {
        Set(flags, keyCode, cmd);
    }

    void Set(int flags, int keyCode, int cmd)
    {
        m_flags = flags; m_keyCode = keyCode; m_command = cmd;
    }

    int GetFlags() const { return m_flags; }
    int GetKeyCode() const { return m_keyCode; }
    int GetCommand() const { return m_command; }

//private:
    int m_flags;
    int m_keyCode; // ASCII or virtual keycode
    int m_command; // Command id to generate
};

// ----------------------------------------------------------------------------
// the accel table has all accelerators for a given window or menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorTable : public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(const wxString& resource); // Load from .rc resource
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]); // Load from array

    // Copy constructors
    inline wxAcceleratorTable(const wxAcceleratorTable& accel) { Ref(accel); }
    inline wxAcceleratorTable(const wxAcceleratorTable* accel) { if (accel) Ref(*accel); }

    ~wxAcceleratorTable();

    inline wxAcceleratorTable& operator = (const wxAcceleratorTable& accel) { if ( *this != accel ) Ref(accel); return *this; }
    inline bool operator == (const wxAcceleratorTable& accel) const { return m_refData == accel.m_refData; }
    inline bool operator != (const wxAcceleratorTable& accel) const { return m_refData != accel.m_refData; }

    bool Ok() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL() const;

    // translate the accelerator, return TRUE if done
    bool Translate(wxWindow *window, WXMSG *msg) const;
};

WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
