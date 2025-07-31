///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/sockettype.h
// Purpose:     Low-level header defining wxCloseSocket()
// Author:      Vadim Zeitlin
// Created:     2025-07-29
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SOCKETTYPE_H_
#define _WX_PRIVATE_SOCKETTYPE_H_

// Note that this header is not affected by wxUSE_SOCKETS as wxCloseSocket() is
// also used by wxWebRequestCURL.
#if defined(__WINDOWS__)
    #include "wx/msw/wrapwin.h"

    #define wxCloseSocket closesocket
#else
    #include <unistd.h>

    #define wxCloseSocket close
#endif

#endif // _WX_PRIVATE_SOCKETTYPE_H_
