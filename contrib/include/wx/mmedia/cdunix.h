// ---------------------------------------------------------------------------
// Name:       cdunix.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    2000
// Copyright:  (C) 1997, 1998, 1999, 2000 Guilhem Lavaux
// License:    wxWindows license
// ---------------------------------------------------------------------------
#ifndef __CDUNIXH__
#define __CDUNIXH__

#ifdef __GNUG__
#pragma interface "cdunix.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/mmedia/cdbase.h"

///
class WXDLLEXPORT wxCDAudioLinux : public wxCDAudio {
  DECLARE_DYNAMIC_CLASS(wxCDAudioLinux)
protected:
  wxCDtime m_time;
  CDstatus m_status;
  CDtoc *m_toc;
  int m_fd;
  wxCDtime *m_trksize, *m_trkpos;
public:
  ///
  wxCDAudioLinux();
  ///
  wxCDAudioLinux(const wxString& dev_name);
  ///
  virtual ~wxCDAudioLinux();

  ///
  virtual bool Play(const wxCDtime& beg_time, const wxCDtime& end_time);
  ///
  virtual bool Pause();
  ///
  virtual bool Resume();
  ///
  virtual CDstatus GetStatus();
  ///
  virtual wxCDtime GetTime();
  ///
  virtual CDtoc& GetToc();
  ///
  virtual inline bool Ok() const { return (m_fd != -1); }
protected:
  ///
  void OpenDevice(const wxString& dev_name);
};

#endif
