// /////////////////////////////////////////////////////////////////////////////
// Name:       cdwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __CDA_win_H__
#define __CDA_win_H__

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

#ifdef WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>
typedef struct CDAW_Internal {
  MCIDEVICEID dev_id;
} CDAW_Internal;
#endif

///
class WXDLLEXPORT wxCDAudioWin : public wxCDAudio {
  DECLARE_DYNAMIC_CLASS(wxCDAudioWin)
protected:
  struct CDAW_Internal *m_internal;
  wxCDtime *m_trksize, *m_trkpos;
  CDtoc *m_toc;
  bool m_ok;
public:
  ///
  wxCDAudioWin(void);
  ///
  wxCDAudioWin(const char *dev_name);
  ///
  virtual ~wxCDAudioWin(void);

  ///
  virtual bool Play(const wxCDtime& beg_time, const wxCDtime& end_time);
  ///
  virtual bool Pause(void);
  ///
  virtual bool Resume(void);
  ///
  virtual CDstatus GetStatus(void);
  ///
  virtual wxCDtime GetTime(void);
  ///
  virtual CDtoc& GetToc(void);
  ///
  virtual inline bool Ok(void) const { return m_ok; }
protected:
  void PrepareToc();
};

#endif
