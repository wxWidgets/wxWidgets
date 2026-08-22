/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/dropfiles.h
// Purpose:     shared WM_DROPFILES event dispatch
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_DROPFILES_H_
#define _WX_MSW_PRIVATE_DROPFILES_H_

#include "wx/defs.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/gdicmn.h"
#include "wx/msw/wrapwin.h"

class wxWindow;

// Consume one HDROP and deliver wxEVT_DROP_FILES to target. When clientPoint
// is non-null it is already mapped to target's client coordinates; otherwise
// the coordinates embedded in the HDROP are used (the historical path).
// DragFinish() always runs exactly once and before application callbacks.
WXDLLIMPEXP_CORE bool
wxMSWDispatchDropFiles(wxWindow* target,
                       WXWPARAM drop,
                       const wxPoint* clientPoint = nullptr);

#endif // wxUSE_DRAG_AND_DROP

#endif // _WX_MSW_PRIVATE_DROPFILES_H_
