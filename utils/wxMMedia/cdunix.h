// /////////////////////////////////////////////////////////////////////////////
// Name:       cdunix.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __CDUNIXH__
#define __CDUNIXH__

#ifdef __GNUG__
#pragma interface
#endif

#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include "mmtype.h"
#include "cdbase.h"

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
  wxCDAudioLinux(const char *dev_name);
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
  void OpenDevice(const char *dev_name);
};

#endif
