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
    wxBUFFER_TOFREE
  } BufState;
public: 
  ///
  typedef struct {
    char *data;
    ///
    char *user_data;
    ///
    wxUint32 size, ptr;
    ///
    wxList *buffers;
    ///
    BufState state;
  } wxFragBufPtr;
protected:
  ///
  wxFragBufPtr *m_optrs, *m_iptrs;
  ///
  wxFragBufPtr *m_lstoptrs, *m_lstiptrs;
  ///
  bool m_buf2free, m_dontq, m_freeing;
public:
  ///
  wxFragmentBuffer(wxSound& io_drv);
  ///
  virtual ~wxFragmentBuffer();

  ///
  virtual void AllocIOBuffer() = 0;
  ///
  virtual void FreeIOBuffer() = 0;

  ///
  void AbortBuffer(wxSndBuffer *buf);
  
  ///
  wxFragBufPtr *FindFreeBuffer(wxFragBufPtr *list, wxUint8 max_queue);
  ///
  bool NotifyOutputBuffer(wxSndBuffer *buf);
  ///
  bool NotifyInputBuffer(wxSndBuffer *buf);

  ///
  void OnBufferFinished(wxFragBufPtr *ptr);

  ///
  virtual bool OnBufferFilled(wxFragBufPtr *ptr, wxSndMode mode) = 0;

  ///
  inline wxSndBuffer *LastBuffer() {
    wxNode *node = m_iodrv->m_buffers.Last();

    if (!node) return NULL;
    return (wxSndBuffer *)node->Data();
  }
  ///
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
