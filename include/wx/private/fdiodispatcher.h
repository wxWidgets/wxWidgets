/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/fdiodispatcher.h
// Purpose:     classes for dispatching IO notifications for file descriptors
// Authors:     Lukasz Michalski
// Created:     December 2006
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_FDIODISPATCHER_H_
#define _WX_PRIVATE_FDIODISPATCHER_H_

#include "wx/hashmap.h"

// handler used to process events on descriptors
class wxFDIOHandler
{
public:
    // called when descriptor is available for non-blocking read
    virtual void OnReadWaiting() = 0;

    // called when descriptor is available  for non-blocking write
    virtual void OnWriteWaiting() = 0;

    // called when there is exception on descriptor
    virtual void OnExceptionWaiting() = 0;

    // virtual dtor for the base class
    virtual ~wxFDIOHandler() { }
};

// those flags describes sets where descriptor should be added
enum wxFDIODispatcherEntryFlags
{
    wxFDIO_INPUT = 1,
    wxFDIO_OUTPUT = 2,
    wxFDIO_EXCEPTION = 4,
    wxFDIO_ALL = wxFDIO_INPUT | wxFDIO_OUTPUT | wxFDIO_EXCEPTION
};

struct wxFDIOHandlerEntry
{
    wxFDIOHandlerEntry()
    {
    }

    wxFDIOHandlerEntry(wxFDIOHandler *handler_, int flags_)
        : handler(handler_),
          flags(flags_)
    {
    }

    wxFDIOHandler *handler;
    int flags;
};

// this hash is used to map file descriptors to their handlers
WX_DECLARE_HASH_MAP(
  int,
  wxFDIOHandlerEntry,
  wxIntegerHash,
  wxIntegerEqual,
  wxFDIOHandlerMap
);

// base class for wxSelectDispatcher and wxEpollDispatcher
//
// notice that all pure virtual functions for FD management have implementation
// in the base class and should be called from the derived classes
class WXDLLIMPEXP_BASE wxFDIODispatcher
{
public:
    enum { TIMEOUT_INFINITE = -1 };

    // find the handler for the given fd, return NULL if none
    wxFDIOHandler *FindHandler(int fd) const;

    // register handler for the given descriptor with the dispatcher, return
    // true on success or false on error
    virtual bool RegisterFD(int fd, wxFDIOHandler *handler, int flags) = 0;

    // modify descriptor flags or handler, return true on success
    virtual bool ModifyFD(int fd, wxFDIOHandler *handler, int flags) = 0;

    // unregister descriptor previously registered with RegisterFD(), the
    // caller is responsible for deleting the returned handler pointer if
    // necessary
    virtual wxFDIOHandler *UnregisterFD(int fd, int flags) = 0;

    // loops waiting for an event to happen on any of the descriptors
    virtual void RunLoop(int timeout) = 0;

    virtual ~wxFDIODispatcher() { }

protected:
    // the fd -> handler map containing all the registered handlers
    wxFDIOHandlerMap m_handlers;
};

#endif // _WX_PRIVATE_FDIODISPATCHER_H_
