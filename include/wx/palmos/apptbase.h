///////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/apptbase.h
// Purpose:     declaration of wxAppTraits for Palm OS
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALMOS_APPTBASE_H_
#define _WX_PALMOS_APPTBASE_H_

// ----------------------------------------------------------------------------
// wxAppTraits: the MSW version adds extra hooks needed by MSW-only code
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
{
public:
    // wxExecute() support methods
    // ---------------------------

    // called before starting to wait for the child termination, may return
    // some opaque data which will be passed later to AfterChildWaitLoop()
    virtual void *BeforeChildWaitLoop() = 0;

    // process pending Windows messages, even in console app
    virtual void AlwaysYield() = 0;

    // called after starting to wait for the child termination, the parameter
    // is the return value of BeforeChildWaitLoop()
    virtual void AfterChildWaitLoop(void *data) = 0;


    // wxThread helpers
    // ----------------

    // process a message while waiting for a(nother) thread, should return
    // false if and only if we have to exit the application
    virtual bool DoMessageFromThreadWait() = 0;

    // other miscellaneous helpers
    // ---------------------------

    // under MSW this function returns same version for both console and GUI
    // applications so we can implement it directly in the base class and only
    // override it wxGUIAppTraits to fill in toolkit information
    virtual wxToolkitInfo& GetToolkitInfo();
};

#endif // _WX_PALMOS_APPTBASE_H_

