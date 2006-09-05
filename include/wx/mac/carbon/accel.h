/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

#include "wx/string.h"
#include "wx/event.h"

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]); // Load from array

    virtual ~wxAcceleratorTable();

    bool operator == (const wxAcceleratorTable& accel) const
    { return m_refData == accel.m_refData; }
    bool operator != (const wxAcceleratorTable& accel) const
    { return m_refData != accel.m_refData; }

    bool Ok() const;

    int GetCommand( wxKeyEvent &event );
};

// WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
