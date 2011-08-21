/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/tablectrl.cpp
// Purpose:     Common wxTableCtrl parts
// Author:      Julian Smart, Linas Valiukas
// Created:     2011-08-19
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dc.h"
#include "wx/sizer.h"
#endif

#include "wx/tablectrl.h"
#include "wx/arrimpl.cpp"


IMPLEMENT_DYNAMIC_CLASS(wxTableCtrl, wxTableCtrlBase)
IMPLEMENT_DYNAMIC_CLASS(wxTablePath, wxObject)
IMPLEMENT_CLASS(wxTableDataSource, wxEvtHandler)
IMPLEMENT_CLASS(wxTableRow, wxObject)
IMPLEMENT_CLASS(wxTableSection, wxObject)

WX_DEFINE_EXPORTED_OBJARRAY(wxTableSectionArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTableRowArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTablePathArray);

IMPLEMENT_DYNAMIC_CLASS(wxTableCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ROW_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ADD_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_MOVE_DRAGGED)
