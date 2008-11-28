///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/apptbase.h
// Purpose:     declaration of wxAppTraits for Unix systems
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_APPTBASE_H_
#define _WX_UNIX_APPTBASE_H_

struct wxEndProcessData;
struct wxExecuteData;

// ----------------------------------------------------------------------------
// wxAppTraits: the Unix version adds extra hooks needed by Unix code
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
{
public:
    // wxExecute() support methods
    // ---------------------------

    // wait for the process termination, return whatever wxExecute() must
    // return
    //
    // base class implementation handles all cases except wxEXEC_SYNC without
    // wxEXEC_NOEVENTS one which is implemented at the GUI level
    virtual int WaitForChild(wxExecuteData& execData);

    // integrate the monitoring of the given fd with the port-specific event
    // loop: when this fd, which corresponds to a dummy pipe opened between the
    // parent and child processes, is closed by the child, the parent is
    // notified about this via a call to wxHandleProcessTermination() function
    //
    // the default implementation uses wxFDIODispatcher and so is suitable for
    // the console applications or ports which don't have any specific event
    // loop
    virtual int AddProcessCallback(wxEndProcessData *data, int fd);


protected:
    // a helper for the implementation of WaitForChild() in wxGUIAppTraits:
    // checks the streams used for redirected IO in execData and returns true
    // if there is any activity in them
    bool CheckForRedirectedIO(wxExecuteData& execData);
};

#endif // _WX_UNIX_APPTBASE_H_

