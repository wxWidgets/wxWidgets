/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/datectrl.cpp
// Purpose:     wxDatePickerCtrl implementation
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     02/14/05
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#if wxUSE_DATEPICKCTRL

#include "wx/datectrl.h"
#include "wx/app.h"
#include "wx/intl.h"
#include "wx/dynlib.h"

#define _WX_DEFINE_DATE_EVENTS_
#include "wx/dateevt.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDatePickerCtrl creation
// ----------------------------------------------------------------------------

bool wxDatePickerCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxDateTime& dt,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    if(!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    wxString label;

    if ( dt.IsValid() )
        label = _T("test2");

    ig(!wxControl::PalmCreateControl(selectorTriggerCtl, label, pos, size))
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl geometry
// ----------------------------------------------------------------------------

wxSize wxDatePickerCtrl::DoGetBestSize() const
{
    const int y = GetCharHeight();

    return wxSize(DEFAULT_ITEM_WIDTH, EDIT_HEIGHT_FROM_CHAR_HEIGHT(y));
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl operations
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    SetLabel(_T("test1"));
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    wxDateTime dt;
    // TODO
    return dt;
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    // TODO
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    // TODO
    return false;
}

#endif // wxUSE_DATEPICKCTRL

