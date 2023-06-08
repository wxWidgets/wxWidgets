/////////////////////////////////////////////////////////////////////////////
// Name:        wx/logclient.h
// Purpose:     Log dump to log server tool
// Author:      Carlos Alberto Rodrigues Azevedo
// Modified by:
// Created:     05/09/22
// Copyright:   (c) 2022 Carlos Alberto Rodrigues Azevedo <carlosazevedonachina@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_LOGCLIENT_H_
#define   _WX_LOGCLIENT_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// Log into server app: this class forwards all log messages to a log server,
// running as a standalone tool. This allows logging even when the app's main
// loop is not running.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxIPCClient;

class WXDLLIMPEXP_CORE wxLogClient : public wxLog
{
public:
    wxLogClient(const wxString& szPort);
    virtual ~wxLogClient();
    bool IsConnected() { return (m_client != nullptr); }

    void AddLevelToRGBAMapping(wxLogLevel level, unsigned rgba);
    void RemoveLevelToRGBAMapping(wxLogLevel level);
    void ClearLevelToRGBAMapping();
protected:
    virtual void DoLogRecord(wxLogLevel level, const wxString& msg,
                             const wxLogRecordInfo& info) wxOVERRIDE;
private:
    wxIPCClient* m_client;
    wxCriticalSection synchro;

    wxDECLARE_NO_COPY_CLASS(wxLogClient);
};

#endif  // _WX_LOGCLIENT_H_
