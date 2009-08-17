///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/fdiohandler.h
// Purpose:     declares wxFDIOHandler class
// Author:      Vadim Zeitlin
// Created:     2009-08-17
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_FDIOHANDLER_H_
#define _WX_PRIVATE_FDIOHANDLER_H_

// ----------------------------------------------------------------------------
// wxFDIOHandler: interface used to process events on file descriptors
// ----------------------------------------------------------------------------

class wxFDIOHandler
{
public:
    // called when descriptor is available for non-blocking read
    virtual void OnReadWaiting() = 0;

    // called when descriptor is available  for non-blocking write
    virtual void OnWriteWaiting() = 0;

    // called when there is exception on descriptor
    virtual void OnExceptionWaiting() = 0;

    // called to check if the handler is still valid, only used by
    // wxSocketImplUnix currently
    virtual bool IsOk() const { return true; }

    // virtual dtor for the base class
    virtual ~wxFDIOHandler() { }
};

#endif // _WX_PRIVATE_FDIOHANDLER_H_

