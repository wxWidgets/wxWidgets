/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/accel.h
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

// ----------------------------------------------------------------------------
// the accel table has all accelerators for a given window or menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorTable : public wxObject
{
public:
    // default ctor
    wxAcceleratorTable();

    // copy ctor
    wxAcceleratorTable(const wxAcceleratorTable& accel) { Ref(accel); }

    // load from .rc resource (Windows specific)
    wxAcceleratorTable(const wxString& resource);

    // initialize from array
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    virtual ~wxAcceleratorTable();

    wxAcceleratorTable& operator = (const wxAcceleratorTable& accel) { if ( *this != accel ) Ref(accel); return *this; }
    bool operator==(const wxAcceleratorTable& accel) const
        { return m_refData == accel.m_refData; } // FIXME: this is wrong (VZ)
    bool operator!=(const wxAcceleratorTable& accel) const
        { return !(*this == accel); }

    bool Ok() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL() const;

    // translate the accelerator, return TRUE if done
    bool Translate(wxWindow *window, WXMSG *msg) const;

private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif
    // _WX_ACCEL_H_
