///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/pipestream.h
// Purpose:     Unix wxPipeInputStream and wxPipeOutputStream declarations
// Author:      Vadim Zeitlin
// Created:     2013-06-08 (extracted from wx/unix/pipe.h)
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_PIPESTREAM_H_
#define _WX_UNIX_PRIVATE_PIPESTREAM_H_

#include "wx/wfstream.h"

class wxPipeInputStream : public wxFileInputStream
{
public:
    explicit wxPipeInputStream(int fd) : wxFileInputStream(fd) { }

    // return true if the pipe is still opened
    bool IsOpened() const { return !Eof(); }

    // return true if we have anything to read, don't block
    virtual bool CanRead() const wxOVERRIDE;
};

class wxPipeOutputStream : public wxFileOutputStream
{
public:
    wxPipeOutputStream(int fd) : wxFileOutputStream(fd) { }

    // Override the base class version to ignore "pipe full" errors: this is
    // not an error for this class.
    size_t OnSysWrite(const void *buffer, size_t size) wxOVERRIDE;
};

#endif // _WX_UNIX_PRIVATE_PIPESTREAM_H_
