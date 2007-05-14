/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/selectdispatcher.h
// Purpose:     wxSelectDispatcher class
// Authors:     Lukasz Michalski and Vadim Zeitlin
// Created:     December 2006
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SELECTDISPATCHER_H_
#define _WX_PRIVATE_SELECTDISPATCHER_H_

#include "wx/defs.h"

#include "wx/private/fdiodispatcher.h"

// helper class storing all the select() fd sets
class WXDLLIMPEXP_BASE wxSelectSets
{
public:
    // ctor zeroes out all fd_sets
    wxSelectSets();

    // default copy ctor, assignment operator and dtor are ok


    // return true if fd appears in any of the sets
    bool HasFD(int fd) const;

    // add or remove FD to our sets depending on whether flags contains
    // wxFDIO_INPUT/OUTPUT/EXCEPTION bits
    bool SetFD(int fd, int flags);

    // same as SetFD() except it unsets the bits set in the flags for the given
    // fd
    bool ClearFD(int fd, int flags)
    {
        return SetFD(fd, wxFDIO_ALL & ~flags);
    }


    // call select() with our sets: the other parameters are the same as for
    // select() itself
    int Select(int nfds, struct timeval *tv);

    // call the handler methods corresponding to the sets having this fd
    void Handle(int fd, wxFDIOHandler& handler) const;

private:
    typedef void (wxFDIOHandler::*Callback)();

    // the FD sets indices
    enum
    {
        Read,
        Write,
        Except,
        Max
    };

    // the sets used with select()
    fd_set m_fds[Max];

    // the wxFDIO_XXX flags, functions and names (used for debug messages only)
    // corresponding to the FD sets above
    static int ms_flags[Max];
    static const char *ms_names[Max];
    static Callback ms_handlers[Max];
};

class WXDLLIMPEXP_BASE wxSelectDispatcher : public wxFDIODispatcher
{
public:
    // returns the unique instance of this class, the pointer shouldn't be
    // deleted and is normally never NULL
    static wxSelectDispatcher *Get();

    // if we have any registered handlers, check for any pending events to them
    // and dispatch them -- this is used from wxX11 and wxDFB event loops
    // implementation
    static void DispatchPending();

    // implement pure virtual methods of the base class
    virtual bool RegisterFD(int fd, wxFDIOHandler *handler, int flags = wxFDIO_ALL);
    virtual bool ModifyFD(int fd, wxFDIOHandler *handler, int flags = wxFDIO_ALL);
    virtual wxFDIOHandler *UnregisterFD(int fd, int flags = wxFDIO_ALL);
    virtual void RunLoop(int timeout = TIMEOUT_INFINITE);

protected:
    wxSelectDispatcher();

private:
    // common part of RegisterFD() and ModifyFD()
    bool DoUpdateFDAndHandler(int fd, wxFDIOHandler *handler, int flags);

    // call the handlers for the fds present in the given sets
    void ProcessSets(const wxSelectSets& sets);

    // helper of ProcessSets(): call the handler if its fd is in the set
    void DoProcessFD(int fd, const fd_set& fds, wxFDIOHandler *handler,
                     const char *name);


    // the select sets containing all the registered fds
    wxSelectSets m_sets;

    // the highest registered fd value or -1 if none
    int m_maxFD;
};


#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
