/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dataview.h
// Purpose:     wxDataViewCtrl base classes
// Author:      Robert Roebling
// Modified by:
// Created:     08.01.06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAVIEW_H_BASE_
#define _WX_DATAVIEW_H_BASE_

#include "wx/defs.h"

#if wxUSE_DATAVIEWCTRL

// ----------------------------------------------------------------------------
// wxDataViewCtrl flags 
// ----------------------------------------------------------------------------

#include "wx/control.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"

extern WXDLLEXPORT_DATA(const wxChar) wxDataViewCtrlNameStr[];





#if defined(__WXGTK20__)
    #include "wx/gtk/dataview.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dataview.h"
#else
    #include "wx/generic/dataview.h"
#endif

#endif // wxUSE_DATAVIEWCTRL

#endif
    // _WX_DATAVIEW_H_BASE_
