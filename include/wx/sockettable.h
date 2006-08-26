/////////////////////////////////////////////////////////////////////////////
// Name:        sockettable.h
// Purpose:     Socket Table class
// Authors:     Angel Vidal
// Modified by:
// Created:     August 2006
// Copyright:   (c) Angel Vidal
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SOCKETTABLE_H_
#define _WX_SOCKETTABLE_H_

#include "wx/defs.h"

// Global table object. Will be innitialized by the module.
class wxSocketTable;
extern wxSocketTable* wxTheSocketTable;

#if wxUSE_SOCKETS

// Forward declaration
class wxSocketTableEntry;
  
class GSocket;

typedef enum
{ wxSocketTableInput, wxSocketTableOutput } wxSocketTableType ;

// wxSocketTable declaration

class wxSocketTable: public wxHashTable
{
  public:
    wxSocketTable(): wxHashTable(wxKEY_INTEGER)
    {
    }
    ~wxSocketTable()
    {
        WX_CLEAR_HASH_TABLE(*this)
    }

    void RegisterCallback(int fd, wxSocketTableType socketType, GSocket* socket);

    void UnregisterCallback(int fd, wxSocketTableType socketType);

    void RunLoop(int timeout = 0);

private:
    void AddEvents(fd_set* readset, fd_set* writeset);
    
    int FillSets(fd_set* readset, fd_set* writeset);

    wxSocketTableEntry* FindEntry(int fd);
};

#endif // wxUSE_SOCKETS

#endif // _WX_SOCKETTABLE_H_
