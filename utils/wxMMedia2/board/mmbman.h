/////////////////////////////////////////////////////////////////////////////
// Name:        mmbman.h
// Purpose:     Multimedia Board manager
// Author:      Guilhem Lavaux, <guilhem.lavaux@libertysurf.fr>
// Modified by:
// Created:     13/02/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MMBMAN_APP_H_
#define _MMBMAN_APP_H_

#ifdef __GNUG__
  #pragma interface "mmbman.cpp"
#endif

#include "wx/stream.h"
#include "sndbase.h"

// -------------------------------------------------------------------------
// Base structure definitions
// -------------------------------------------------------------------------

typedef struct {
  wxUint8 seconds, minutes, hours;
} MMBoardTime;

// -------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------

#define MMBoard_NoError 0
#define MMBoard_UnknownFile 1

// -------------------------------------------------------------------------
// Interface definition: MMBoardFile
// -------------------------------------------------------------------------

class MMBoardFile {
 public:
  MMBoardFile();
  virtual ~MMBoardFile();

  virtual bool NeedWindow() = 0;

  virtual void SetWindow(wxWindow *window) = 0;
  
  virtual void Play() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void Stop() = 0;

  virtual MMBoardTime GetPosition() = 0;
  virtual MMBoardTime GetLength() = 0;

  virtual bool IsStopped() = 0;

  virtual wxString GetStringType() = 0;
  virtual wxString GetStringInformation() = 0;

  void SetError(wxUint8 error) { m_error = error; }
  wxUint8 GetError() const { return m_error; }

 protected:
  wxUint8 m_error;
};

// -------------------------------------------------------------------------
// Main manager
// -------------------------------------------------------------------------

class MMBoardManager {
 public:
  static MMBoardFile *Open(const wxString& filename);

  static wxSoundStream *OpenSoundStream();
  static void UnrefSoundStream(wxSoundStream *stream);
};

#endif
