/////////////////////////////////////////////////////////////////////////////
// Name:        ddesetup.h
// Purpose:     DDE sample settings
// Author:      Julian Smart
// Modified by:
// Created:     25/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// You may set this to 0 to prevent DDE from being used even under Windows
//#define wxUSE_DDE_FOR_IPC 0

#include <wx/ipc.h>

// the default service name
#define IPC_SERVICE _T("4242")

// the IPC topic
#define IPC_TOPIC _T("IPC TEST")

// the name of the item we're being advised about
#define IPC_ADVISE_NAME _T("Item")
