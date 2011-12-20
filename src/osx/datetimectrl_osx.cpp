///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/datetimectrl_osx.cpp
// Purpose:     Implementation of wxDateTimePickerCtrl for OS X.
// Author:      Vadim Zeitlin
// Created:     2011-12-18
// RCS-ID:      $Id: wxhead.cpp,v 1.11 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/datetimectrl.h"

#include "wx/osx/core/private/datetimectrl.h"

// ============================================================================
// wxDateTimePickerCtrl implementation
// ============================================================================

wxDateTimeWidgetImpl* wxDateTimePickerCtrl::GetDateTimePeer() const
{
    return static_cast<wxDateTimeWidgetImpl*>(GetPeer());
}

void wxDateTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    GetDateTimePeer()->SetDateTime(dt);
}

wxDateTime wxDateTimePickerCtrl::GetValue() const
{
    return GetDateTimePeer()->GetDateTime();
}


#endif // wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL
