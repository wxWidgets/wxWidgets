/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKACCELH__
#define __GTKACCELH__

#ifdef __GNUG__
#pragma interface "accel.h"
#endif

#include "wx/defs.h"

#if wxUSE_ACCEL

#include "wx/object.h"
#include "wx/event.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxAcceleratorEntry;
class wxAcceleratorTable;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

extern wxAcceleratorTable wxNullAcceleratorTable;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

 // Hold no other key
#define wxACCEL_NORMAL  0x00

//-----------------------------------------------------------------------------
// wxAcceleratorEntry
//-----------------------------------------------------------------------------

class wxAcceleratorEntry: public wxObject
{
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0)
      { m_flags = flags; m_keyCode = keyCode; m_command = cmd; }

    inline void Set(int flags, int keyCode, int cmd)
      { m_flags = flags; m_keyCode = keyCode; m_command = cmd; }

    inline int GetFlags() const { return m_flags; }
    inline int GetKeyCode() const { return m_keyCode; }
    inline int GetCommand() const { return m_command; }

    int  m_flags;
    int  m_keyCode; // ASCII or virtual keycode
    int  m_command; // Command id to generate
};

//-----------------------------------------------------------------------------
// wxAcceleratorTable
//-----------------------------------------------------------------------------

class wxAcceleratorTable: public wxObject
{
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, wxAcceleratorEntry entries[] );
    ~wxAcceleratorTable();

    inline wxAcceleratorTable(const wxAcceleratorTable& accel) : wxObject()
      { Ref(accel); }
    inline wxAcceleratorTable(const wxAcceleratorTable* accel) 
      { if (accel) Ref(*accel); }
    inline bool operator == (const wxAcceleratorTable& accel)
      { return m_refData == accel.m_refData; }
    inline bool operator != (const wxAcceleratorTable& accel) 
      { return m_refData != accel.m_refData; }
    inline wxAcceleratorTable& operator = (const wxAcceleratorTable& accel)
      { if (*this == accel) return (*this); Ref(accel); return *this; }

    bool Ok() const;
    
    // implementation
    // --------------
 
    int GetCommand( wxKeyEvent &event );
    
private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif

#endif
