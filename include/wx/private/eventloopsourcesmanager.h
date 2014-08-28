///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/eventloopsourcesmanager.h
// Purpose:     declares wxEventLoopSourcesManagerBase class
// Author:      Rob Bresalier
// Created:     2013-06-19
// Copyright:   (c) 2013 Rob Bresalier
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EVENTLOOPSOURCESMANAGER_H_
#define _WX_PRIVATE_EVENTLOOPSOURCESMANAGER_H_

// For pulling in the value of wxUSE_EVENTLOOP_SOURCE
#include "wx/evtloop.h"

#if wxUSE_EVENTLOOP_SOURCE

class WXDLLIMPEXP_BASE wxEventLoopSourcesManagerBase
{
public:
    virtual wxEventLoopSource*
    AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags) = 0;

    virtual ~wxEventLoopSourcesManagerBase() { }
};

#endif // wxUSE_EVENTLOOP_SOURCE

#endif // _WX_PRIVATE_EVENTLOOPSOURCESMANAGER_H_
