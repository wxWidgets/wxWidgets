/////////////////////////////////////////////////////////////////////////////
// Name:        ftp.h
// Purpose:     FTP protocol
// Author:      Vadim Zeitlin
// Modified by: Mark Johnson, wxWindows@mj10777.de 
//              20000917 : RmDir, GetLastResult, GetList 
// Created:     07/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_FTP_H__
#define __WX_FTP_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/object.h"
#include "wx/sckaddr.h"
#include "wx/protocol/protocol.h"
#include "wx/url.h"

class WXDLLEXPORT wxFTP : public wxProtocol
{
public:
    enum TransferMode
    {
        ASCII,
        BINARY
    };

    wxFTP();
    virtual ~wxFTP();

    // Connecting and disconnecting
    void SetUser(const wxString& user) { m_user = user; }
    void SetPassword(const wxString& passwd) { m_passwd = passwd; }

    bool Connect(wxSockAddress& addr, bool wait = TRUE);
    bool Connect(const wxString& host);

    // disconnect
    virtual bool Close();

    // Parameters set up

    // set transfer mode now
    bool SetBinary() { return SetTransferMode(BINARY); }
    bool SetAscii() { return SetTransferMode(ASCII); }
    bool SetTransferMode(TransferMode mode);

    // Generic FTP interface

    // the error code
    virtual wxProtocolError GetError() { return m_lastError; }

    // the last FTP server reply
    const wxString& GetLastResult() { return m_lastResult; }

    // send any FTP command (should be full FTP command line but without
    // trailing "\r\n") and return its return code
    char SendCommand(const wxString& command);

    // check that the command returned the given code
    bool CheckCommand(const wxString& command, char expectedReturn)
    {
        return SendCommand(command) == expectedReturn;
    }

    // Filesystem commands
    bool ChDir(const wxString& dir);
    bool MkDir(const wxString& dir);
    bool RmDir(const wxString& dir);
    wxString Pwd();
    bool Rename(const wxString& src, const wxString& dst);
    bool RmFile(const wxString& path);

    // Download methods
    bool Abort();

    virtual wxInputStream *GetInputStream(const wxString& path);
    virtual wxOutputStream *GetOutputStream(const wxString& path);

    // Directory listing

    // get the list of full filenames, the format is fixed: one file name per
    // line
    bool GetFilesList(wxArrayString& files,
                      const wxString& wildcard = wxEmptyString)
    {
        return GetList(files, wildcard, FALSE);
    }

    // get a directory list in server dependent format - this can be shown
    // directly to the user
    bool GetDirList(wxArrayString& files,
                    const wxString& wildcard = wxEmptyString)
    {
        return GetList(files, wildcard, TRUE);
    }

    // equivalent to either GetFilesList() (default) or GetDirList()
    bool GetList(wxArrayString& files,
                 const wxString& wildcard = wxEmptyString,
                 bool details = FALSE);

#ifdef WXWIN_COMPATIBILITY_2
    // deprecated
    wxList *GetList(const wxString& wildcard, bool details = FALSE);
#endif // WXWIN_COMPATIBILITY_2

protected:
    // this executes a simple ftp command with the given argument and returns
    // TRUE if it its return code starts with '2'
    bool DoSimpleCommand(const wxChar *command,
                         const wxString& arg = wxEmptyString);

    // get the server reply, return the first character of the reply code,
    // '1'..'5' for normal FTP replies, 0 (*not* '0') if an error occured
    char GetResult();

    // check that the result is equal to expected value
    bool CheckResult(char ch) { return GetResult() == ch; }

    wxSocketClient *GetPort();

    wxString m_user,
             m_passwd;

    wxString m_lastResult;
    wxProtocolError m_lastError;

    // true if there is an FTP transfer going on
    bool m_streaming;

    // true if the user had set the transfer mode
    bool m_modeSet;

    friend class wxInputFTPStream;
    friend class wxOutputFTPStream;

    DECLARE_DYNAMIC_CLASS(wxFTP)
    DECLARE_PROTOCOL(wxFTP)
};

#endif // __WX_FTP_H__
