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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "accel.h"
#endif

#include "wx/string.h"
#include "wx/event.h"

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]); // Load from array

    // Copy constructors
    wxAcceleratorTable(const wxAcceleratorTable& accel)
        : wxObject()
    { Ref(accel); }
    wxAcceleratorTable(const wxAcceleratorTable* accel)
    { if (accel) Ref(*accel); }

    ~wxAcceleratorTable();

    wxAcceleratorTable& operator = (const wxAcceleratorTable& accel)
    { if (*this == accel) return (*this); Ref(accel); return *this; }
    bool operator == (const wxAcceleratorTable& accel)
    { return m_refData == accel.m_refData; }
    bool operator != (const wxAcceleratorTable& accel)
    { return m_refData != accel.m_refData; }

    bool Ok() const;

    int GetCommand( wxKeyEvent &event );
};

// WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
