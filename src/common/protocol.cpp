/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/protocol.cpp
// Purpose:     Implement protocol base class
// Author:      Guilhem Lavaux
// Created:     07/07/1997
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROTOCOL

#include "wx/protocol/protocol.h"
#include "wx/protocol/log.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/module.h"
#endif

#include "wx/url.h"
#include "wx/log.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// wxProtoInfo
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxProtoInfo, wxObject);

wxProtoInfo::wxProtoInfo(const wxChar *name, const wxChar *serv,
                         const bool need_host1, wxClassInfo *info)
           : m_protoname(name),
             m_servname(serv)
{
    m_cinfo = info;
    m_needhost = need_host1;
#if wxUSE_URL
    next = wxURL::ms_protocols;
    wxURL::ms_protocols = this;
#else
    next = nullptr;
#endif
}


// ----------------------------------------------------------------------------
// wxProtocol
// ----------------------------------------------------------------------------

#if wxUSE_SOCKETS
wxIMPLEMENT_ABSTRACT_CLASS(wxProtocol, wxSocketClient);
#else
wxIMPLEMENT_ABSTRACT_CLASS(wxProtocol, wxObject);
#endif

wxProtocol::wxProtocol()
#if wxUSE_SOCKETS
    // Only use non blocking sockets if we can dispatch events.
    : wxSocketClient(wxSocketClient::GetBlockingFlagIfNeeded() | wxSOCKET_WAITALL)
#endif
{
    m_lastError = wxPROTO_NOERR;
    m_log = nullptr;
    SetDefaultTimeout(60);      // default timeout is 60 seconds
}

void wxProtocol::SetDefaultTimeout(wxUint32 Value)
{
    m_uiDefaultTimeout = Value;
#if wxUSE_SOCKETS
    wxSocketBase::SetTimeout(Value); // sets it for this socket
#endif
}

wxProtocol::~wxProtocol()
{
    delete m_log;
}

#if wxUSE_SOCKETS
bool wxProtocol::Reconnect()
{
    wxIPV4address addr;

    if (!GetPeer(addr))
    {
        Close();
        return false;
    }

    if (!Close())
        return false;

    if (!Connect(addr))
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// Read a line from socket
// ----------------------------------------------------------------------------

/* static */
wxProtocolError wxProtocol::ReadLine(wxSocketBase *sock, wxString& result)
{
    static const int LINE_BUF = 4095;

    result.clear();

    // Although we're supposed to get 7-bit ASCII from the server, some FTP
    // servers are known to send 8-bit data, so we try to decode it in
    // any way that works as this is more useful than just throwing it away.
    wxWhateverWorksConv conv;

    wxCharBuffer buf(LINE_BUF);
    char *pBuf = buf.data();
    while ( sock->WaitForRead() )
    {
        // peek at the socket to see if there is a CRLF
        sock->Peek(pBuf, LINE_BUF);

        size_t nRead = sock->LastCount();
        if ( !nRead )
        {
            // If we didn't read anything, it must mean either an error or EOF,
            // but as we don't have any specific error code for the latter,
            // just return the generic error in either case.
            //
            // Note that we can't return wxPROTO_NOERR from here because wxFTP
            // relies on the function returning some error to exit the loop
            // when retrieving the list of files.
            return wxPROTO_NETERR;
        }

        // look for "\r\n" paying attention to a special case: "\r\n" could
        // have been split by buffer boundary, so check also for \r at the end
        // of the last chunk and \n at the beginning of this one
        pBuf[nRead] = '\0';
        const char *eol = strchr(pBuf, '\n');

        // if we found '\n', is there a '\r' as well?
        if ( eol )
        {
            if ( eol == pBuf )
            {
                // check for case of "\r\n" being split
                if ( result.empty() || result.Last() != wxT('\r') )
                {
                    // ignore the stray '\n'
                    eol = nullptr;
                }
                //else: ok, got real EOL

                // read just this '\n' and restart
                nRead = 1;
            }
            else // '\n' in the middle of the buffer
            {
                // in any case, read everything up to and including '\n'
                nRead = eol - pBuf + 1;

                if ( eol[-1] != '\r' )
                {
                    // as above, simply ignore stray '\n'
                    eol = nullptr;
                }
            }
        }

        sock->Read(pBuf, nRead);
        if ( sock->LastCount() != nRead )
            return wxPROTO_NETERR;

        pBuf[nRead] = '\0';
        result += conv.cMB2WX(pBuf);

        if ( eol )
        {
            // remove trailing "\r\n"
            result.RemoveLast(2);

            return wxPROTO_NOERR;
        }
    }

    return wxPROTO_NETERR;
}

wxProtocolError wxProtocol::ReadLine(wxString& result)
{
    return ReadLine(this, result);
}

#endif // wxUSE_SOCKETS

// ----------------------------------------------------------------------------
// logging
// ----------------------------------------------------------------------------

void wxProtocol::SetLog(wxProtocolLog *log)
{
    delete m_log;
    m_log = log;
}

void wxProtocol::LogRequest(const wxString& str)
{
    if ( m_log )
        m_log->LogRequest(str);
}

void wxProtocol::LogResponse(const wxString& str)
{
    if ( m_log )
        m_log->LogResponse(str);
}

void wxProtocolLog::DoLogString(const wxString& str)
{
    wxUnusedVar(str); // unused if wxLogTrace() is disabled
    wxLogTrace(m_traceMask, "%s", str);
}

#endif // wxUSE_PROTOCOL
