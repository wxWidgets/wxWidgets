/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/apptbase.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_APPTBASE_H_
#define _WX_QT_APPTBASE_H_

struct wxEndProcessData;
struct wxExecuteData;
class wxFDIOManager;

class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
{
public:
    wxAppTraits();

#if wxUSE_CONFIG
    virtual wxConfigBase *CreateConfig();
#endif // wxUSE_CONFIG

#if wxUSE_LOG
    virtual wxLog *CreateLogTarget();
#endif // wxUSE_LOG

    virtual wxMessageOutput *CreateMessageOutput();

#if wxUSE_FONTMAP
    virtual wxFontMapper *CreateFontMapper();
#endif // wxUSE_FONTMAP

    virtual wxRendererNative *CreateRenderer();

    virtual wxStandardPaths& GetStandardPaths();

#if wxUSE_INTL
    virtual void SetLocale();
#endif // wxUSE_INTL

    virtual bool ShowAssertDialog(const wxString& msg);

    virtual bool HasStderr();

#if wxUSE_SOCKETS
    virtual wxSocketManager *GetSocketManager();
#endif

    virtual wxEventLoopBase *CreateEventLoop();

// #if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer);
// #endif

#if wxUSE_THREADS
    virtual void MutexGuiEnter();
    virtual void MutexGuiLeave();
#endif

    virtual wxPortId GetToolkitVersion(int *majVer = NULL, int *minVer = NULL) const;

    virtual bool IsUsingUniversalWidgets() const;

    virtual wxString GetDesktopEnvironment() const;

protected:
#if wxUSE_STACKWALKER
    virtual wxString GetAssertStackTrace();
#endif

    // From ../unix/apptbase.h:
public:
    virtual int WaitForChild(wxExecuteData& execData);
    virtual int AddProcessCallback(wxEndProcessData *data, int fd);
    virtual wxFDIOManager *GetFDIOManager();
protected:
    bool CheckForRedirectedIO(wxExecuteData& execData);

};

#endif // _WX_QT_APPTBASE_H_

