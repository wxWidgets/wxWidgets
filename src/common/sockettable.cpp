///////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/sockettable.cpp
// Purpose:     implements wxSocketTable for platforms with no event notification.
// Author:      Angel Vidal
// Modified by:
// Created:     08.24.06
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Angel vidal
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/sockettable.h"

#ifndef WX_PRECOMP
    #include "wx/hash.h"
#endif

#include "wx/module.h"
#include "wx/unix/private.h"

#include <sys/time.h>
#include <unistd.h>

#if wxUSE_SOCKETS
// ----------------------------------------------------------------------------
// wxSocketTable
// ----------------------------------------------------------------------------

#include "wx/gsocket.h"

wxSocketTable* wxTheSocketTable = NULL;

class wxSocketTableEntry: public wxObject
{
  public:
    wxSocketTableEntry()
    {
        m_fdInput = -1; m_fdOutput = -1;
        m_socket = NULL;
    }

    int m_fdInput;
    int m_fdOutput;
    GSocket* m_socket;
};

wxSocketTableEntry* wxSocketTable::FindEntry(int fd)
{
    wxSocketTableEntry* entry = (wxSocketTableEntry*) Get(fd);
    return entry;
}

void wxSocketTable::RegisterCallback(int fd, wxSocketTableType socketType, GSocket* socket)
{
    wxSocketTableEntry* entry = FindEntry(fd);
    if (!entry)
    {
        entry = new wxSocketTableEntry();
        Put(fd, entry);
    }

    if (socketType == wxSocketTableInput)
        entry->m_fdInput = fd;
    else
        entry->m_fdOutput = fd;
     
    entry->m_socket = socket;
}

void wxSocketTable::UnregisterCallback(int fd, wxSocketTableType socketType)
{
    wxSocketTableEntry* entry = FindEntry(fd);
    if (entry)
    {
        if (socketType == wxSocketTableInput)
            entry->m_fdInput = -1;
        else
            entry->m_fdOutput = -1;
        
        if (entry->m_fdInput == -1 && entry->m_fdOutput == -1)
        {
            entry->m_socket = NULL;          
            Delete(fd);
            delete entry;
        }
    }
}

int wxSocketTable::FillSets(fd_set* readset, fd_set* writeset)
{
    int max_fd = 0;
  
    wxFD_ZERO(readset);
    wxFD_ZERO(writeset);
  
    BeginFind();
    wxHashTable::compatibility_iterator node = Next();
    while (node)
    {
        wxSocketTableEntry* entry = (wxSocketTableEntry*) node->GetData();

        if (entry->m_fdInput != -1)
        {
            wxFD_SET(entry->m_fdInput, readset);
            if (entry->m_fdInput > max_fd)
              max_fd = entry->m_fdInput;
        }

        if (entry->m_fdOutput != -1)
        {
            wxFD_SET(entry->m_fdOutput, writeset);
            if (entry->m_fdOutput > max_fd)
                max_fd = entry->m_fdOutput;
        }

        node = Next();
    }
    
    return max_fd;
}

void wxSocketTable::AddEvents(fd_set* readset, fd_set* writeset)
{
    BeginFind();
    wxHashTable::compatibility_iterator node = Next();
    while (node)
    {
        // We have to store the next node here, because the event processing can 
        // destroy the object before we call Next()

        wxHashTable::compatibility_iterator next_node = Next();	

        wxSocketTableEntry* entry = (wxSocketTableEntry*) node->GetData();

        wxCHECK_RET(entry->m_socket, wxT("Critical: Processing a NULL socket in wxSocketTable"));
      
        if (entry->m_fdInput != -1 && wxFD_ISSET(entry->m_fdInput, readset))
            entry->m_socket->Detected_Read();

        if (entry->m_fdOutput != -1 && wxFD_ISSET(entry->m_fdOutput, writeset))
            entry->m_socket->Detected_Write();;

        node = next_node;
    }
}

void wxSocketTable::RunLoop(int timeout) {

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;
    fd_set readset;
    fd_set writeset;

    int max_fd = FillSets( &readset, &writeset);
    if (select( max_fd+1, &readset, &writeset, NULL, &tv ) == 0)
    {
      // No socket input/output. Don't add events.
      return;
    }
    else
    {
      AddEvents(&readset, &writeset);
    }
}

class wxSocketTableModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxSocketTableModule)
public:
    wxSocketTableModule() {}
    bool OnInit() { wxTheSocketTable = new wxSocketTable; return true; };
    void OnExit() { delete wxTheSocketTable; wxTheSocketTable = NULL; };
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketTableModule, wxModule)

#endif
