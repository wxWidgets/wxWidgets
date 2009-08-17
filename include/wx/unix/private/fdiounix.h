///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/fdiounix.h
// Purpose:     wxFDIOManagerUnix class used by console Unix applications
// Author:      Vadim Zeitlin
// Created:     2009-08-17
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _UNIX_PRIVATE_FDIOUNIX_H_
#define _UNIX_PRIVATE_FDIOUNIX_H_

#include "wx/private/fdiomanager.h"

// ----------------------------------------------------------------------------
// wxFDIOManagerUnix: implement wxFDIOManager interface using wxFDIODispatcher
// ----------------------------------------------------------------------------

class wxFDIOManagerUnix : public wxFDIOManager
{
public:
    virtual int AddInput(wxFDIOHandler *handler, int fd, Direction d);
    virtual void RemoveInput(wxFDIOHandler *handler, int fd, Direction d);
};

#endif // _UNIX_PRIVATE_FDIOUNIX_H_

