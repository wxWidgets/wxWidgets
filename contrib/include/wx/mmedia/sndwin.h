// --------------------------------------------------------------------------
// Name: sndwin.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDWIN_H
#define _WX_SNDWIN_H

#include "wx/defs.h"
#include "wx/mmedia/sndbase.h"

typedef struct _wxSoundInternal wxSoundInternal;
typedef struct _wxSoundInfoHeader wxSoundInfoHeader;

class wxSoundInternal;
class WXDLLEXPORT wxSoundStreamWin : public wxSoundStream {
 public:
  wxSoundStreamWin();
  ~wxSoundStreamWin();

  wxSoundStream& Write(const void *buffer, wxUint32 len);
  wxSoundStream& Read(void *buffer, wxUint32 len); 

  bool SetSoundFormat(wxSoundFormatBase& base);

  bool StartProduction(int evt);
  bool StopProduction();

  bool QueueFilled() const;

  // Internal but defined as public
  void NotifyDoneBuffer(wxUint32 dev_handle, int flag);

  wxUint32 GetBestSize() const { return 4096; }

 protected:
  wxSoundInternal *m_internal;
  wxUint32 m_current_frag_in, m_current_frag_out;
  wxUint32 m_input_frag_in, m_output_frag_out;
  wxSoundInfoHeader **m_headers_play, **m_headers_rec;

  bool m_production_started, m_queue_filled, m_waiting_for;

 protected:
  void CreateSndWindow();
  void DestroySndWindow();
  bool OpenDevice(int mode);
  void CloseDevice();

  wxSoundInfoHeader *AllocHeader(int mode);
  void FreeHeader(wxSoundInfoHeader *header, int mode);
  bool AllocHeaders(int mode);
  void FreeHeaders(int mode);

  void WaitFor(wxSoundInfoHeader *info);
  bool AddToQueue(wxSoundInfoHeader *info);
  void ClearHeader(wxSoundInfoHeader *info);

  wxSoundInfoHeader *NextFragmentOutput();
  wxSoundInfoHeader *NextFragmentInput();
};

#endif
