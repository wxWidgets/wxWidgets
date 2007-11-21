/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

#include "wx/object.h"

class WXDLLEXPORT wxAcceleratorTable;

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

 // Hold no key down
#define wxACCEL_NORMAL  0x00

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(const wxString& rsResource); // Load from .rc resource
    wxAcceleratorTable( int                n
                       ,const wxAcceleratorEntry vaEntries[]
                      ); // Load from array

    virtual ~wxAcceleratorTable();

    bool Ok() const { return IsOk(); }
    bool IsOk() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL(void) const;

    // translate the accelerator, return TRUE if done
    bool Translate( WXHWND hWnd
                   ,WXMSG* pMsg
                  ) const;
};

WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

WXDLLEXPORT wxString wxPMTextToLabel(const wxString& rsTitle);
#endif
    // _WX_ACCEL_H_
