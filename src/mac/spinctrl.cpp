/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by: Mark Newsam (Based on GTK file)
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinctlg.h"
#endif

#include "wx/defs.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#if 0

// now using the generic impl
//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

#if wxUSE_SPINBTN && !defined(__WXMAC__)

#if !USE_SHARED_LIBRARY
     IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl,wxControl)
#endif

#else // !wxUSE_SPINBTN

#if !USE_SHARED_LIBRARY
     IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl,wxTextCtrl)
#endif

#endif // wxUSE_SPINBTN/!wxUSE_SPINBTN

#endif

#endif   // wxUSE_SPINCTRL

