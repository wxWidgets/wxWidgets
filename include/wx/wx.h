/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wx.h
// Purpose:     wxWidgets central header including the most often used ones
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WX_H_
#define _WX_WX_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/dynarray.h"
#include "wx/list.h"
#include "wx/hash.h"
#include "wx/string.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/stream.h"

#if wxUSE_GUI

#include "wx/window.h"
#include "wx/panel.h"
#include "wx/frame.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcprint.h"
#include "wx/dcscreen.h"
#include "wx/button.h"
#include "wx/menu.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/palette.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/dialog.h"
#include "wx/timer.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/cmndata.h"

#include "wx/control.h"
#include "wx/ctrlsub.h"
#include "wx/bmpbuttn.h"
#include "wx/checkbox.h"
#include "wx/checklst.h"
#include "wx/choice.h"
#include "wx/scrolbar.h"
#include "wx/stattext.h"
#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/listbox.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/textctrl.h"
#include "wx/slider.h"
#include "wx/gauge.h"
#include "wx/scrolwin.h"
#include "wx/dirdlg.h"
#include "wx/toolbar.h"
#include "wx/combobox.h"
#include "wx/layout.h"
#include "wx/sizer.h"
#include "wx/memory.h"
#include "wx/mdi.h"
#include "wx/statusbr.h"
#include "wx/scrolbar.h"
#include "wx/choicdlg.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"

// always include, even if !wxUSE_VALIDATORS because we need wxDefaultValidator
#include "wx/validate.h"

#if wxUSE_VALIDATORS
    #include "wx/valtext.h"
#endif // wxUSE_VALIDATORS

#endif // wxUSE_GUI

#endif // _WX_WX_H_

