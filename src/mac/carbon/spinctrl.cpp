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

#include "wx/spinctrl.h"

#if wxUSE_SPINCTRL


//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
     IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl,wxTextCtrl)
#endif


#endif   // wxUSE_SPINCTRL

