/////////////////////////////////////////////////////////////////////////////
// Name:        wrapwin.h
// Purpose:     Wrapper around <windows.h>, to be included instead of it
// Author:      Vaclav Slavik
// Created:     2003/07/22
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WRAPWIN_H_
#define _WX_WRAPWIN_H_

#include "wx/platform.h"

#ifndef STRICT
    #define STRICT 1
#endif

#include <windows.h>
#include "wx/msw/winundef.h"

#endif // _WX_WRAPWIN_H_

