///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/apptrait.h
// Purpose:     standard implementations of wxAppTraits for Unix
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_APPTRAIT_H_
#define _WX_UNIX_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
    virtual bool CreateEndProcessPipe(wxExecuteData& execData);
    virtual bool IsWriteFDOfEndProcessPipe(wxExecuteData& execData, int fd);
    virtual void DetachWriteFDOfEndProcessPipe(wxExecuteData& execData);
    virtual int WaitForChild(wxExecuteData& execData);

    virtual int GetOSVersion(int *verMaj, int *verMin);
};

#if wxUSE_GUI

class WXDLLEXPORT wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual bool CreateEndProcessPipe(wxExecuteData& execData);
    virtual bool IsWriteFDOfEndProcessPipe(wxExecuteData& execData, int fd);
    virtual void DetachWriteFDOfEndProcessPipe(wxExecuteData& execData);
    virtual int WaitForChild(wxExecuteData& execData);

    virtual int GetOSVersion(int *verMaj, int *verMin);
};

#endif // wxUSE_GUI

#endif // _WX_UNIX_APPTRAIT_H_

