// --------------------------------------------------------------------------
// Name: sndesd.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDESD_H
#define _WX_SNDESD_H

#ifdef __GNUG__
#pragma interface "sndesd.h"
#endif

#include <wx/string.h>
#include "sndbase.h"
#include "sndpcm.h"

//
// ESD output class
//

class wxSoundStreamESD : public wxSoundStream {
 public:
  wxSoundStreamESD(const wxString& hostname = _T("localhost"));
  ~wxSoundStreamESD();

  wxSoundStream& Read(void *buffer, size_t len);
  wxSoundStream& Write(const void *buffer, size_t len);

  bool SetSoundFormat(const wxSoundFormatBase& format);

  bool StartProduction(int evt);
  bool StopProduction();

  void SetDuplexMode(bool duplex) {}

  // You should not call this.
  void WakeUpEvt(int evt);
 protected:
  int m_fd;
  int m_tag;
  bool m_esd_stop;
  wxString m_hostname;

 private:
  void DetectBest(wxSoundFormatPcm *pcm);
};

#endif
