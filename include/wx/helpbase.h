/////////////////////////////////////////////////////////////////////////////
// Name:        helpbase.h
// Purpose:     Help system base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __HELPBASEH__
#define __HELPBASEH__

#ifdef __GNUG__
#pragma interface "helpbase.h"
#endif

#include "wx/wx.h"

#if USE_HELP

// Defines the API for help controllers
class WXDLLEXPORT wxHelpControllerBase: public wxObject
{
  DECLARE_CLASS(wxHelpControllerBase)

 public:
  inline wxHelpControllerBase(void) {}
  inline ~wxHelpControllerBase(void) {};

  // Must call this to set the filename and server name.
  // server is only required when implementing TCP/IP-based
  // help controllers.
  virtual bool Initialize(const wxString& file, int server = -1) = 0;

  // If file is "", reloads file given  in Initialize
  virtual bool LoadFile(const wxString& file = "") = 0;
  virtual bool DisplayContents(void) = 0;
  virtual bool DisplaySection(int sectionNo) = 0;
  virtual bool DisplayBlock(long blockNo) = 0;
  virtual bool KeywordSearch(const wxString& k) = 0;

  virtual bool Quit(void) = 0;
  virtual void OnQuit(void) {};
};

#endif // USE_HELP
#endif
    // __HELPBASEH__
