///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/socket.h
// Purpose:     various wxSocket-related private declarations
// Author:      Vadim Zeitlin
// Created:     2008-11-23
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SOCKET_H_
#define _WX_PRIVATE_SOCKET_H_

// these definitions are for MSW when we don't use configure, otherwise these
// symbols are defined by configure
#ifndef WX_SOCKLEN_T
    #define WX_SOCKLEN_T int
#endif

#ifndef SOCKOPTLEN_T
    #define SOCKOPTLEN_T int
#endif

// define some symbols which winsock.h defines but traditional BSD headers
// don't
#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
#endif

#if wxUSE_IPV6
    typedef struct sockaddr_storage wxSockAddr;
#else
    typedef struct sockaddr wxSockAddr;
#endif

#endif // _WX_PRIVATE_SOCKET_H_

