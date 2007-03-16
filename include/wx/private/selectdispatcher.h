/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/selectdispatcher.h
// Purpose:     wxSelectDispatcher class
// Authors:     Lukasz Michalski
// Modified by:
// Created:     December 2006
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SELECTDISPATCHER_H_
#define _WX_PRIVATE_SELECTDISPATCHER_H_

#include "wx/defs.h"

#include "wx/hashmap.h"

static const int wxSELECT_TIMEOUT_INFINITE = -1;

// handler used to process events on descriptors
class wxFDIOHandler
{
public:
    // called when descriptor is available for non-blocking read
    virtual void OnReadWaiting(int fd) = 0;

    // called when descriptor is available  for non-blocking write
    virtual void OnWriteWaiting(int fd) = 0;

    // called when there is exception on descriptor
    virtual void OnExceptionWaiting(int fd) = 0;

    // virtual dtor for the base class
    virtual ~wxFDIOHandler() { }
};

// those flags describes sets where descriptor should be added
enum wxSelectDispatcherEntryFlags
{
    wxSelectInput = 1,
    wxSelectOutput = 2,
    wxSelectException = 4,
    wxSelectAll = wxSelectInput | wxSelectOutput | wxSelectException
};

WX_DECLARE_HASH_MAP(
  int,
  wxFDIOHandler*,
  wxIntegerHash,
  wxIntegerEqual,
  wxFDIOHandlerMap
);

class WXDLLIMPEXP_CORE wxSelectDispatcher
{
public:
    // returns instance of the table
    static wxSelectDispatcher& Get();

    virtual ~wxSelectDispatcher()
    {
    }

    // register descriptor in sets.
    void RegisterFD(int fd, wxFDIOHandler* handler, int flags = wxSelectAll);

    // unregister descriptor from sets and return handler for cleanup
    wxFDIOHandler* UnregisterFD(int fd, int flags = wxSelectAll);

    // return handler for descriptor or null if fd is not registered
    wxFDIOHandler* FindHandler(int fd);

    // calls select on registered descriptors and
    void RunLoop(int timeout = wxSELECT_TIMEOUT_INFINITE);

protected:
    wxSelectDispatcher() { }

private:
    void ProcessSets(fd_set* readset, fd_set* writeset, fd_set* exeptset, int max_fd);

    fd_set m_readset;
    fd_set m_writeset;
    fd_set m_exeptset;

    int m_maxFD;
    wxFDIOHandlerMap m_handlers;

    static wxSelectDispatcher *ms_instance;

    friend class wxSelectDispatcherModule;
};


#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
