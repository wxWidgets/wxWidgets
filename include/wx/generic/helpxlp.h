/////////////////////////////////////////////////////////////////////////////
// Name:        helpxlp.h
// Purpose:     Help system: wxHelp implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

/* sccsid[] = "@(#)wx_help.h	1.2 5/9/94" */

#ifndef __HELPXLPH__
#define __HELPXLPH__

#ifdef __GNUG__
#pragma interface "helpxlp.h"
#endif

#include <stdio.h>
#include "wx/wx.h"

#if USE_HELP

#include "wx/helpbase.h"

#ifdef __WXMSW__
#include "wx/dde.h"
#else
// Or whatever it'll be called
#include "wx/ipctcp.h"
#endif

class WXDLLEXPORT wxXLPHelpController;

// Connection class for implementing the connection between the
// wxHelp process and the application
class WXDLLEXPORT wxXLPHelpConnection: public

#ifdef __WXMSW__
 wxDDEConnection
#else
 wxTCPConnection
#endif

{
  friend class wxXLPHelpController;

  DECLARE_DYNAMIC_CLASS(wxXLPHelpConnection)

 public:

  wxXLPHelpConnection(wxXLPHelpController *instance);
  bool OnDisconnect(void);

 private:
  wxXLPHelpController *helpInstance;
};

// Connection class for implementing the client process
// controlling the wxHelp process
class WXDLLEXPORT wxXLPHelpClient: public

#ifdef __WXMSW__
 wxDDEClient
#else
 wxTCPClient
#endif

{
DECLARE_CLASS(wxXLPHelpClient)

	friend class WXDLLEXPORT wxXLPHelpController;
public:
  wxXLPHelpClient(wxXLPHelpController* c) { m_controller = c; }

  wxConnectionBase *OnMakeConnection(void)
    { return new wxXLPHelpConnection(m_controller);
    }
protected:
  wxXLPHelpController* m_controller;
};

// An application can have one or more instances of wxHelp,
// represented by an object of this class.
// Nothing happens on initial creation; the application
// must call a member function to display help.
// If the instance of wxHelp is already active, that instance
// will be used for subsequent help.

class WXDLLEXPORT wxXLPHelpController: public wxHelpControllerBase
{
  friend class WXDLLEXPORT wxXLPHelpConnection;
  DECLARE_CLASS(wxXLPHelpController)

 public:
  wxXLPHelpController(void);
  ~wxXLPHelpController(void);

  // Must call this to set the filename and server name
  virtual bool Initialize(const wxString& file, int server = -1);
  // If file is "", reloads file given in Initialize
  virtual bool LoadFile(const wxString& file = "");
  virtual bool DisplayContents(void);
  virtual bool DisplaySection(int sectionNo);
  virtual bool DisplayBlock(long blockNo);
  virtual bool KeywordSearch(const wxString& k);

  virtual bool Quit(void);
  virtual void OnQuit(void);

  // Private
  bool Run(void);

 protected:
  wxString				helpFile;
  wxString				helpHost;
  int					helpServer;
  bool					helpRunning;
  wxXLPHelpConnection*	helpConnection;
  wxXLPHelpClient		helpClient;
};

#endif // USE_HELP
#endif
    // __HELPXLPH__
