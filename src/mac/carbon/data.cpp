/////////////////////////////////////////////////////////////////////////////
// Name:        mac/data.cpp
// Purpose:     Various global Mac-specific data
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/event.h"

#if wxUSE_SHARED_LIBRARY
///// Event tables (also must be in one, statically-linked file for shared libraries)

// This is the base, wxEvtHandler 'bootstrap' code which is expanded manually here
const wxEventTable *wxEvtHandler::GetEventTable() const { return &wxEvtHandler::sm_eventTable; }

const wxEventTable wxEvtHandler::sm_eventTable =
    { NULL, &wxEvtHandler::sm_eventTableEntries[0] };

const wxEventTableEntry wxEvtHandler::sm_eventTableEntries[] = { { 0, 0, 0, NULL } };
#endif

