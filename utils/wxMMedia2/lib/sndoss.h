// --------------------------------------------------------------------------
// Name: sndoss.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDOSS_H
#define _WX_SNDOSS_H

#ifdef __GNUG__
#pragma interface "sndoss.h"
#endif

#include <wx/string.h>
#include "sndbase.h"
#include "sndpcm.h"

//
// OSS output class
//

class wxSoundStreamOSS : public wxSoundStream {
 public:
  wxSoundStreamOSS(const wxString& dev_name = _T("/dev/dsp"));
  ~wxSoundStreamOSS();

  wxSoundStream& Read(void *buffer, wxUint32 len);
  wxSoundStream& Write(const void *buffer, wxUint32 len);
  wxUint32 GetBestSize() const;

  bool SetSoundFormat(const wxSoundFormatBase& format);

  bool StartProduction(int evt);
  bool StopProduction();

  void SetDuplexMode(bool duplex) {}
  bool QueueFilled() const;

  // You should not call this.
  void WakeUpEvt(int evt);
 protected:
  int m_fd;
  wxUint32 m_bufsize;
  int m_tag;
  bool m_oss_stop, m_q_filled;
  wxString m_devname;

 private:
  bool SetupFormat(wxSoundFormatPcm *pcm);
  void DetectBest(wxSoundFormatPcm *pcm);
};

#endif
