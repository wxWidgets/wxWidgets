/////////////////////////////////////////////////////////////////////////////
// Name:        helpwin.h
// Purpose:     Help system: WinHelp implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPWIN_H_
#define _WX_HELPWIN_H_

#ifdef __GNUG__
#pragma interface "helpwin.h"
#endif

#include "wx/wx.h"

#if USE_HELP

#include "wx/helpbase.h"

class WXDLLEXPORT wxWinHelpController: public wxHelpControllerBase
{
  DECLARE_CLASS(wxWinHelpController)

 public:
  wxWinHelpController(void);
  ~wxWinHelpController(void);

  // Must call this to set the filename and server name
  virtual bool Initialize(const wxString& file);

  // If file is "", reloads file given  in Initialize
  virtual bool LoadFile(const wxString& file = "");
  virtual bool DisplayContents(void);
  virtual bool DisplaySection(int sectionNo);
  virtual bool DisplayBlock(long blockNo);
  virtual bool KeywordSearch(const wxString& k);

  virtual bool Quit(void);
  virtual void OnQuit(void);

  inline wxString GetHelpFile(void) const { return m_helpFile; }

protected:
  wxString m_helpFile;
};

#endif // USE_HELP
#endif
    // _WX_HELPWIN_H_
