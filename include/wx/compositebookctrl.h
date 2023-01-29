///////////////////////////////////////////////////////////////////////////////
// Name:        wx/custombookctrl.h
// Purpose:     Helper for wxBookCtrlBase subclasses composed of several windows
// Author:      Vadim Zeitlin
// Created:     2023-01-29
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMPOSITEBOOKCTRL_H_
#define _WX_COMPOSITEBOOKCTRL_H_

#include "wx/bookctrl.h"

#if wxUSE_BOOKCTRL

#include "wx/containr.h"

// ----------------------------------------------------------------------------
// wxCompositeBookCtrlBase
// ----------------------------------------------------------------------------

// This class is specifically DLL-exported, even though it's trivial, in order
// to ensure that there is only a single copy of it in the wx DLL.
class WXDLLIMPEXP_CORE wxCompositeBookCtrlBase : public wxNavigationEnabled<wxBookCtrlBase>
{
public:
    wxCompositeBookCtrlBase();
};

#endif // wxUSE_BOOKCTRL

#endif // _WX_COMPOSITEBOOKCTRL_H_
