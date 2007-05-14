///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gsocketiohandler.cpp
// Purpose:     implementation of wxFDIOHandler for GSocket
// Author:      Angel Vidal, Lukasz Michalski
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

#if wxUSE_SOCKETS

#include "wx/private/gsocketiohandler.h"
#include "wx/unix/private.h"
#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGSocketIOHandler
// ----------------------------------------------------------------------------

wxGSocketIOHandler::wxGSocketIOHandler(GSocket* socket)
                  : m_socket(socket),
                    m_flags(0)
{

}

void wxGSocketIOHandler::OnReadWaiting()
{
    m_socket->Detected_Read();
}

void wxGSocketIOHandler::OnWriteWaiting()
{
    m_socket->Detected_Write();
}

void wxGSocketIOHandler::OnExceptionWaiting()
{
    m_socket->Detected_Read();
}

int wxGSocketIOHandler::GetFlags() const
{
    return m_flags;
}


void wxGSocketIOHandler::RemoveFlag(wxFDIODispatcherEntryFlags flag)
{
    m_flags &= ~flag;
}

void wxGSocketIOHandler::AddFlag(wxFDIODispatcherEntryFlags flag)
{
    m_flags |= flag;
}

// ----------------------------------------------------------------------------
// GSocket interface
// ----------------------------------------------------------------------------

bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{
    return true;
}

bool GSocketGUIFunctionsTableConcrete::OnInit()
{
    return true;
}

void GSocketGUIFunctionsTableConcrete::OnExit()
{
}

bool GSocketGUIFunctionsTableConcrete::Init_Socket(GSocket *socket)
{
  int *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(int)*2);
  m_id = (int *)(socket->m_gui_dependent);

  m_id[0] = -1;
  m_id[1] = -1;

  return true;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void GSocketGUIFunctionsTableConcrete::Install_Callback(GSocket *socket,
                                                        GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
  const int fd = socket->m_fd;

  if ( fd == -1 )
    return;

  int c;
  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  wxSelectDispatcher * const dispatcher = wxSelectDispatcher::Get();
  if ( !dispatcher )
      return;

  wxGSocketIOHandler *
      handler = (wxGSocketIOHandler*)dispatcher->FindHandler(fd);
  if ( !handler )
  {
      handler = new wxGSocketIOHandler(socket);
  }

  if (c == 0)
  {
      m_id[0] = fd;
      handler->AddFlag(wxFDIO_INPUT);
  }
  else
  {
      m_id[1] = fd;
      handler->AddFlag(wxFDIO_OUTPUT);
  }

  dispatcher->RegisterFD(fd, handler, handler->GetFlags());
}

void GSocketGUIFunctionsTableConcrete::Uninstall_Callback(GSocket *socket,
                                                          GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
  int c;

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  if ( m_id[c] == -1 )
      return;

  int fd = m_id[c];
  m_id[c] = -1;

  const wxFDIODispatcherEntryFlags flag = c == 0 ? wxFDIO_INPUT : wxFDIO_OUTPUT;

  wxSelectDispatcher * const dispatcher = wxSelectDispatcher::Get();
  if ( !dispatcher )
      return;

  wxGSocketIOHandler * const
      handler = (wxGSocketIOHandler*)dispatcher->UnregisterFD(fd, flag);
  if ( handler )
  {
      handler->RemoveFlag(flag);

      if ( !handler->GetFlags() )
          delete handler;
  }
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  Install_Callback(socket, GSOCK_INPUT);
  Install_Callback(socket, GSOCK_OUTPUT);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  Uninstall_Callback(socket, GSOCK_INPUT);
  Uninstall_Callback(socket, GSOCK_OUTPUT);
}

#endif // wxUSE_SOCKETS
