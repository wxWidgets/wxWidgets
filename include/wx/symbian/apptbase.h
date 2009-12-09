///////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/apptbase.h
// Purpose:     declaration of wxAppTraits for Symbian
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) Andrei Matuk
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYMBIAN_APPTBASE_H_
#define _WX_SYMBIAN_APPTBASE_H_

// ----------------------------------------------------------------------------
// wxAppTraits
// ----------------------------------------------------------------------------

#include "wx/private/fdiomanager.h"

class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
{
public:
#if wxUSE_SOCKETS
    // return a pointer to the object which should be used to integrate
    // monitoring of the file descriptors to the event loop (currently this is
    // used for the sockets only but should be used for arbitrary event loop
    // sources in the future)
    //
    // this object may be different for the console and GUI applications
    //
    // the pointer is not deleted by the caller as normally it points to a
    // static variable
    virtual wxFDIOManager *GetFDIOManager();
#endif // wxUSE_SOCKETS

};

#endif // _WX_SYMBIAN_APPTBASE_H_

