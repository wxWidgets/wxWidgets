/////////////////////////////////////////////////////////////////////////////
// Name:        helpxxxx.h
// Purpose:     Help system: native implementation for your system.
// Author:      David Webster
// Modified by:
// Created:      10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPWIN_H_
#define _WX_HELPWIN_H_

#include "wx/wx.h"

#include "wx/helpbase.h"

class WXDLLEXPORT wxWinHelpController: public wxHelpControllerBase
{
  DECLARE_CLASS(wxWinHelpController)

 public:
  wxWinHelpController();
  ~wxWinHelpController();

  // Must call this to set the filename and server name
  virtual bool Initialize(const wxString& file);

  // If file is "", reloads file given  in Initialize
  virtual bool LoadFile(const wxString& file = "");
  virtual bool DisplayContents();
  virtual bool DisplaySection(int sectionNo);
  virtual bool DisplayBlock(long blockNo);
  virtual bool KeywordSearch(const wxString& k);

  virtual bool Quit();
  virtual void OnQuit();

  inline wxString GetHelpFile() const { return m_helpFile; }

protected:
  wxString m_helpFile;
private:
    // virtual function hiding supression :: do not use
    bool Initialize(const wxString& rFile, int WXUNUSED(nServer) ) { return(Initialize(rFile)); }
    bool DisplaySection(const wxString& rSection) { return wxHelpControllerBase::DisplaySection(rSection); }
};

#endif
    // _WX_HELPWIN_H_
