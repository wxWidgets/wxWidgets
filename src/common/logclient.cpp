/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/logclient.cpp
// Purpose:     Log dump to log server tool
// Author:      Carlos Alberto Rodrigues Azevedo
// Modified by:
// Created:     05/09/22
// Copyright:   (c) 2022 Carlos Alberto Rodrigues Azevedo <carlosazevedonachina@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#define wxUSE_DDE_FOR_IPC 0

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/ipc.h"
    #include "wx/datetime.h"
#endif // WX_PRECOMP

#include "wx/logclient.h"

WX_DECLARE_HASH_MAP(wxLogLevel, unsigned, wxIntegerHash, wxIntegerEqual, wxLevelToRGBAMap);

// ----------------------------------------------------------------------------
// wxLogServer and accompaning classes implementation
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxLogClient class implementation
// ----------------------------------------------------------------------------

class wxIPCClient: public wxClient
{
public:
    wxIPCClient() : m_connection(nullptr) {}
    ~wxIPCClient() { Disconnect(); }

    void AddLevelToRGBAMapping(wxLogLevel level, unsigned rgba)
    {
        m_levelRGBAMap[level] = rgba;
    }

    void RemoveLevelToRGBAMapping(wxLogLevel level)
    {
        m_levelRGBAMap.erase(level);
    }

    void ClearLevelToRGBAMapping() { m_levelRGBAMap.clear(); }

    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();

    wxConnectionBase* OnMakeConnection() wxOVERRIDE { return new wxConnection; }
    wxConnection* GetConnection() { return m_connection; }
    unsigned GetMessageColour(wxLogLevel level);
private:
    wxConnection* m_connection;
    wxLevelToRGBAMap m_levelRGBAMap;
};

bool wxIPCClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    m_connection = (wxConnection*)MakeConnection(sHost, sService, sTopic);
    return (m_connection != nullptr);
}

void wxIPCClient::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        wxDELETE(m_connection);
    }
}

unsigned wxIPCClient::GetMessageColour(wxLogLevel level)
{
    unsigned clr = 0;
    wxLevelToRGBAMap::iterator iter = m_levelRGBAMap.find(level);
    if (iter != m_levelRGBAMap.end())
        clr = (wxINT32_SWAP_ALWAYS(iter->second & 0x00FFFFFF) >> 8) | 0xFF000000;
    return clr;
}

// ----------------------------------------------------------------------------
// wxLogServer class implementation
// ----------------------------------------------------------------------------

wxLogClient::wxLogClient(const wxString& szPort)
{
    m_client = new wxIPCClient;
    if (!m_client->Connect("localhost", szPort, "LOG_MESSAGE")) wxDELETE(m_client);
}

wxLogClient::~wxLogClient()
{
    delete m_client;
}

void wxLogClient::AddLevelToRGBAMapping(wxLogLevel level, unsigned rgba)
{
    if (m_client) m_client->AddLevelToRGBAMapping(level, rgba);
}

void wxLogClient::RemoveLevelToRGBAMapping(wxLogLevel level)
{
    if (m_client) m_client->RemoveLevelToRGBAMapping(level);
}

void wxLogClient::ClearLevelToRGBAMapping()
{
    if (m_client) m_client->ClearLevelToRGBAMapping();
}

void wxLogClient::DoLogRecord(wxLogLevel level, const wxString& msg,
                              const wxLogRecordInfo& info)
{
    if (m_client)
    {
        wxCriticalSectionLocker lock(synchro);
        wxDateTime ts(info.timestamp);
        wxString packet(wxString::Format("%s\r%.8X\r%s", msg,
                                         m_client->GetMessageColour(level),
                                         ts.FormatTime()));
        m_client->GetConnection()->Poke("LOGPACKET", packet);
    }
}
