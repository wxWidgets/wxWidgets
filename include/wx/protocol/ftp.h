/////////////////////////////////////////////////////////////////////////////
// Name:        ftp.h
// Purpose:     FTP protocol
// Author:      Vadim Zeitlin
// Modified by:
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

class WXDLLEXPORT wxFTP : public wxProtocol {
  DECLARE_DYNAMIC_CLASS(wxFTP)
  DECLARE_PROTOCOL(wxFTP)
public:
  typedef enum { ASCII, BINARY } wxFTPmode;

  wxFTP();
  ~wxFTP();

  bool Close();
  bool Connect(wxSockAddress& addr, bool wait = TRUE);
  bool Connect(const wxString& host);

  void SetUser(const wxString& user) { m_user = user; }
  void SetPassword(const wxString& passwd) { m_passwd = passwd; }

  // Low-level methods
  bool SendCommand(const wxString& command, char exp_ret);
  inline virtual wxProtocolError GetError()
      { return m_lastError; }
  const wxString& GetLastResult();		// Get the complete return 

  // Filesystem commands
  bool ChDir(const wxString& dir);
  bool MkDir(const wxString& dir);
  bool RmDir(const wxString& dir);
  wxString Pwd();
  bool Rename(const wxString& src, const wxString& dst);
  bool RmFile(const wxString& path);

  // Download methods
  bool Abort();
  wxInputStream *GetInputStream(const wxString& path);
  wxOutputStream *GetOutputStream(const wxString& path);

  // List method
  wxList *GetList(const wxString& wildcard);

protected:
  wxString m_user, m_passwd;
  wxString m_lastResult;
  wxProtocolError m_lastError;
  bool m_streaming;

  friend class wxInputFTPStream;
  friend class wxOutputFTPStream;

  wxSocketClient *GetPort();
  bool GetResult(char exp);
};

#endif
