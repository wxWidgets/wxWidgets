// /////////////////////////////////////////////////////////////////////////////
// Name:       sndfrag.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __SND_frag_H__
#define __SND_frag_H__
#ifdef __GNUG__
#pragma interface
#endif

#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include "sndsnd.h"

///
class wxFragmentBuffer {
protected:
  wxSound *m_iodrv;

  ///
  wxUint8 m_maxoq, m_maxiq;

  ///
  typedef enum {
    wxBUFFER_FREE,
    wxBUFFER_FFILLED,
    wxBUFFER_TOFREE,
    wxBUFFER_PLAYING
  } BufState;
public: 
  ///
  typedef struct {
    // Local stream buffer for this fragment.
    wxStreamBuffer *sndbuf;
    // Data the driver would like to pass to the callback.
    char *user_data;
    // Buffers included in this fragment.
    wxList *buffers;
    // State of the fragment.
    BufState state;
  } wxFragBufPtr;
protected:
  //
  wxFragBufPtr *m_optrs, *m_iptrs;
  //
  wxFragBufPtr *m_lstoptrs, *m_lstiptrs;
  //
  bool m_buf2free, m_dontq, m_freeing;
  //
  wxSoundDataFormat m_drvformat;
public:
  wxFragmentBuffer(wxSound& io_drv);
  virtual ~wxFragmentBuffer();

  // These functions initializes the fragments. They must initialize 
  // m_lstoptrs, m_lstiptrs, m_maxoq, m_maxiq.
  virtual void AllocIOBuffer() = 0;
  virtual void FreeIOBuffer() = 0;

  void AbortBuffer(wxSndBuffer *buf);
  
  // Find a free (or partly free) fragment.
  wxFragBufPtr *FindFreeBuffer(wxFragBufPtr *list, wxUint8 max_queue);
  // Add this sound buffer to an "OUTPUT" fragment.
  bool NotifyOutputBuffer(wxSndBuffer *buf);
  // Add this sound buffer to an "INPUT" fragment.
  bool NotifyInputBuffer(wxSndBuffer *buf);

  // Called when a fragment is finished.
  void OnBufferFinished(wxFragBufPtr *ptr);

  // Called when a fragment is full and it should be flushed in the sound card.
  virtual bool OnBufferFilled(wxFragBufPtr *ptr, wxSndMode mode) = 0;

  inline wxSndBuffer *LastBuffer() {
    wxNode *node = m_iodrv->m_buffers.Last();

    if (!node) return NULL;
    return (wxSndBuffer *)node->Data();
  }
  inline wxSndBuffer *FirstBuffer() {
    wxNode *node = m_iodrv->m_buffers.First();

    if (!node) return NULL;
    return (wxSndBuffer *)node->Data();
  }
protected:
  void FreeBufToFree(bool force = FALSE);
  void ClearBuffer(wxFragBufPtr *ptr);
};

#endif
