// /////////////////////////////////////////////////////////////////////////////
// Name:       mmfile.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __MMD_file_H__
#define __MMD_file_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/string.h"
#include "wx/stream.h"
#include "mmtype.h"

///
typedef enum {
  wxMMFILE_NOERROR,
  wxMMFILE_INVALID,
  wxMMFILE_EOF
} ///
 wxMMFileError;

typedef struct {
  wxInt8 hours;
  wxUint8 minutes, seconds;
} wxMMtime;

/// Common base class for multimedia file.
class wxMMediaFile {
public:
  wxMMediaFile();
  ///
  wxMMediaFile(wxInputStream& is, bool preload, bool seekable);
  ///
  wxMMediaFile(wxOutputStream& is, bool seekable);
  ///
  wxMMediaFile(const wxString& fname);
  ///
  virtual ~wxMMediaFile();

  ///
  wxInputStream *GetIRawData();
  wxOutputStream *GetORawData();
  ///
  wxString GetCurrentFile();

  ///
  virtual void SetFile(wxInputStream& is,
                       bool preload = FALSE, bool seekable = FALSE);
  ///
  virtual void SetFile(wxOutputStream& is, bool seekable = FALSE);
  ///
  void SetFile(const wxString& fname);

  ///
  wxMMFileError GetFileError() const { return m_mmerror; }
  ///
  virtual wxMMtime GetLength();
  ///
  virtual wxMMtime GetPosition();
  ///
  virtual bool StartPlay() = 0;
  ///
  virtual void StopPlay() = 0;
protected:
  ///
  wxMMFileError m_mmerror;
  ///
  wxInputStream *m_istream, *m_i_temp;
  ///
  wxOutputStream *m_ostream, *m_o_temp;
  ///
  wxString m_tmpfname, m_mfname;
  ///
  bool m_seekable;

  ///
  void CacheIO(); 
  ///
  void CleanUpPrevious();
};

#endif
