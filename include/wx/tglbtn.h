/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tglbtn.h
// Purpose:     This dummy header includes the proper header file for the
//              system we're compiling under.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     Rocketeer license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOGGLEBUTTON_H_BASE_
#define _WX_TOGGLEBUTTON_H_BASE_

#include "wx/defs.h"

#if wxUSE_TOGGLEBTN

#include "wx/event.h"
#include "wx/control.h"     // base class

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 19)
END_DECLARE_EVENT_TYPES()

#define EVT_TOGGLEBUTTON(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

#if defined(__WXMSW__)
    #include "wx/msw/tglbtn.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/tglbtn.h"
/*
# elif defined(__WXMOTIF__)
#  include "wx/motif/tglbtn.h"
# elif defined(__WXQT__)
#  include "wx/qt/tglbtn.h"
# elif defined(__WXMAC__)
#  include "wx/mac/tglbtn.h"
# elif defined(__WXPM__)
#  include "wx/os2/tglbtn.h"
# elif defined(__WXSTUBS__)
#  include "wx/stubs/tglbtn.h"
*/
#endif

#endif // wxUSE_TOGGLEBTN

#endif // _WX_TOGGLEBUTTON_H_BASE_

