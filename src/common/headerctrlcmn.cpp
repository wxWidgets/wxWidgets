///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/headerctrlcmn.cpp
// Purpose:     implementation of wxHeaderCtrlBase
// Author:      Vadim Zeitlin
// Created:     2008-12-02
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
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

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/headerctrl.h"

// ============================================================================
// wxHeaderCtrlBase implementation
// ============================================================================

extern WXDLLIMPEXP_DATA_CORE(const char) wxHeaderCtrlNameStr[] = "wxHeaderCtrl";

void wxHeaderCtrlBase::DeleteAllColumns()
{
    const unsigned count = GetColumnCount();
    for ( unsigned n = 0; n < count; n++ )
        DeleteColumn(0);
}


