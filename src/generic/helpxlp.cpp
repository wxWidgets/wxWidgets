/////////////////////////////////////////////////////////////////////////////
// Name:        helpxlp.cpp
// Purpose:     Help system: wxHelp implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "helpxlp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/generic/helpxlp.h"

#if USE_HELP
#include <time.h>

#ifdef __X__
#include <netdb.h>

#ifdef SUN_CC
#include <sysent.h>
#endif // SUN_CC
#ifdef ____HPUX__
#include <sys/unistd.h>
#endif // ____HPUX__
#endif // __X__

#include <string.h>

// Timeout in seconds
#define WX_HELP_TIMEOUT 15 /* was 30 */

// MAX path length
#define _MAXPATHLEN 500

// MAX length of Help descriptor
#define _MAX_HELP_LEN 500

#include "wx/generic/helpxlp.h"

#if !USE_SHARED_LIBRARY

#ifdef __WINDOWS__
IMPLEMENT_CLASS(wxXLPHelpClient, wxDDEClient)
IMPLEMENT_CLASS(wxXLPHelpConnection, wxDDEConnection)
#else
IMPLEMENT_CLASS(wxXLPHelpClient, wxTCPClient)
IMPLEMENT_CLASS(wxXLPHelpConnection, wxTCPConnection)
#endif

IMPLEMENT_CLASS(wxXLPHelpController, wxHelpControllerBase)
#endif

wxXLPHelpController::wxXLPHelpController(void):
  helpClient(this)
{
  helpFile = ""; helpServer = -1; helpHost = "";
  helpRunning = FALSE; helpConnection = NULL;
}

wxXLPHelpController::~wxXLPHelpController(void)
{
}

bool wxXLPHelpController::Initialize(const wxString& filename, int server)
{
#ifdef __X__
  char host_buf[255];
  if (wxGetHostName(host_buf, sizeof(host_buf)))
    helpHost = host_buf;
  else helpHost = "";
#endif

  helpFile = filename;
  helpServer = server;
  wxIPCInitialize();
  return TRUE;
}

bool wxXLPHelpController::LoadFile(const wxString& file)
{
  helpFile = file;

  if (!helpRunning)
  {
      if (!Run())
        return FALSE;
  }
  char buf[_MAX_HELP_LEN];
  sprintf(buf, "f %s", (const char*) file);
  if (helpConnection)
    return helpConnection->Execute(buf);
  else return FALSE;
}

bool wxXLPHelpController::DisplayContents(void)
{
	if (!helpRunning)
	{
      if (!Run())
        return FALSE;
    }
    if (helpConnection)
      	return helpConnection->Execute("s -1");
    else
	    return FALSE;
}

bool wxXLPHelpController::DisplaySection(int section)
{
    if (!helpRunning)
	{
      if (!Run())
        return FALSE;
    }
    char buf[_MAX_HELP_LEN];
    sprintf(buf, "s %d", section);
    if (helpConnection)
      return helpConnection->Execute(buf);
    else return FALSE;
}

bool wxXLPHelpController::DisplayBlock(long block)
{
    if (!helpRunning)
	{
      if (!Run())
        return FALSE;
    }
    char buf[_MAX_HELP_LEN];
    sprintf(buf, "b %ld", block);
    if (helpConnection)
      return helpConnection->Execute(buf);
    else return FALSE;
}

bool wxXLPHelpController::KeywordSearch(const wxString& k)
{
    if (!helpRunning)
	{
      if (!Run())
        return FALSE;
    }
    char buf[500];
    sprintf(buf, "k %s", (const char*) k);
    if (helpConnection)
      return helpConnection->Execute(buf);
    else return FALSE;
}

bool wxXLPHelpController::Quit(void)
{
  if (helpConnection)
    return helpConnection->Disconnect(); // Calls OnQuit via OnDisconnect
  else return TRUE;
}

void wxXLPHelpController::OnQuit(void)
{
}

bool wxXLPHelpController::Run(void)
{
#ifdef __X__
  if (!helpFile || !helpHost || helpRunning)
    return FALSE;
#endif
#ifdef __WINDOWS__
  if (!helpFile || helpRunning)
    return FALSE;
#endif

  time_t current_time;
#ifdef __X__
  // Invent a server name that's likely to be unique but different from
  // last time
  (void)time(&current_time);
  if (helpServer == -1)
    helpServer = (int)(4000 + (current_time % 4000));
#else
  // Only one instance of wxHelp at a time
  helpServer = 4000;
#endif

  char server[32];
  sprintf(server, "%d", helpServer);
#ifdef __WINDOWS__
  // Only one instance of wxHelp under Windows.
  // See if there's already an instance of wxHelp
  if ((helpConnection = (wxXLPHelpConnection *)helpClient.MakeConnection(helpHost, server, "WXHELP")))
  {
    helpRunning = TRUE;
    return TRUE;
  }
#endif

  // Start help process in server modus
//  char *argv[] = {"wxhelp", "-server", server, NULL}; // HP compiler complains
  char *argv[4];
  argv[0] = "wxhelp";
  argv[1] = "-server";
  argv[2] = server;
  argv[3] = NULL;

  if (wxExecute((char **)argv) == FALSE)
    return FALSE; // Maybe we should print a message?

  time_t start_time;
  (void)time(&start_time);
  // Give it some time to respond
  do {
    wxSleep(1);
    helpConnection = (wxXLPHelpConnection *)helpClient.MakeConnection(helpHost, server, "WXHELP");
    (void)time(&current_time);
  } while (!helpConnection && ((current_time - start_time) < WX_HELP_TIMEOUT));

  if (helpConnection == NULL) {
    char buf[100];
    sprintf(buf, (const char *) _("Connection to wxHelp timed out in %d seconds"), WX_HELP_TIMEOUT);
    (void)wxMessageBox(buf, _("Error"));
    return FALSE;
  }
  helpRunning = TRUE;
  return TRUE;
}

wxXLPHelpConnection::wxXLPHelpConnection(wxXLPHelpController *instance)
{
  helpInstance = instance;
}

bool wxXLPHelpConnection::OnDisconnect(void)
{
  helpInstance->OnQuit();
  helpInstance->helpRunning = FALSE;
  helpInstance->helpConnection = NULL;
  helpInstance->helpServer = -1;
  delete this;
  return TRUE;
}

#endif // USE_HELP
