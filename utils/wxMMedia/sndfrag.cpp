////////////////////////////////////////////////////////////////////////////////
// Name:       sndfrag.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndfrag.h"
#endif
#include <stdio.h>
#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include "sndfrag.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxFragmentBuffer::wxFragmentBuffer(wxSound& io_drv)
 : m_iodrv(&io_drv), m_maxoq(0), m_maxiq(0),
   m_optrs(NULL), m_iptrs(NULL), m_lstoptrs(NULL), m_lstiptrs(NULL),
   m_buf2free(FALSE), m_dontq(FALSE), m_freeing(FALSE)
{
}

wxFragmentBuffer::~wxFragmentBuffer()
{
}

void wxFragmentBuffer::AbortBuffer(wxSndBuffer *buf)
{
  for (wxUint8 i=0;i<m_maxoq;i++)
    if (m_lstoptrs[i].buffers->Member(buf)) {
      if (m_lstoptrs[i].state == wxBUFFER_PLAYING)
        // TODO: Do something.
        ;
      m_lstoptrs[i].state = wxBUFFER_TOFREE;
    }

  for (wxUint8 i=0;i<m_maxiq;i++)
    if (m_lstiptrs[i].buffers->Member(buf)) {
      if (m_lstiptrs[i].state == wxBUFFER_PLAYING)
        // Do something.
        ;
      m_lstiptrs[i].state = wxBUFFER_TOFREE;
    }
}

wxFragmentBuffer::wxFragBufPtr *wxFragmentBuffer::FindFreeBuffer(
				                   xFragBufPtr *list, wxUint8 max_queue)
{
  if (!list)
    return NULL;

  for (wxUint8 i=0;i<max_queue;i++) {
    if (list[i].state == wxBUFFER_FREE)
      return &list[i];
  }

  return NULL;
}

bool wxFragmentBuffer::NotifyOutputBuffer(wxSndBuffer *buf)
{
  wxFragBufPtr *ptr;
  char *raw_buf;
  wxUint32 rawbuf_size;
  wxSoundCodec *codec = buf->GetCurrentCodec();

  if (!m_iodrv->OnSetupDriver(*buf, wxSND_OUTPUT))
    return FALSE;

  while (1) {
    // Find the first free (at least partly free) output buffer
    ptr = FindFreeBuffer(m_lstoptrs, m_maxoq);
    // No free : go out !
    if (!ptr)
      return FALSE;
    
    codec->SetOutStream(ptr->sndbuf);
    codec->InitIO(m_drvformat);
    
    // Fill it up
    codec->Decode();

    // No data to fill the buffer: dequeue the current wxSndBuffer
    if (!codec->Available()) {
      if (buf->IsNotSet(wxSND_KEEPQUEUED)) {
        buf->Set(wxSND_UNQUEUEING);
        m_iodrv->m_buffers.DeleteObject(buf);
      }
      return TRUE;
    }

    // Data: append it to the list
    ptr->buffers->Append(buf);

    // Output buffer full: send it to the driver
    if (ptr->sndbuf->GetDataLeft()) {
      ptr->state = wxBUFFER_FFILLED;
      OnBufferFilled(ptr, wxSND_OUTPUT);
    }
  }
}

bool wxFragmentBuffer::NotifyInputBuffer(wxSndBuffer *buf)
{
  /*
    wxFragBufPtr *ptr;
    char *raw_buf;
    wxUint32 rawbuf_size;
    
    if (!m_iodrv->OnSetupDriver(*buf, wxSND_INPUT))
      return FALSE;
    
    while (1) {
      ptr = FindFreeBuffer(m_lstiptrs, m_maxiq);
      if (!ptr)
        return FALSE;
    
    raw_buf = ptr->data + ptr->ptr;
    rawbuf_size = ptr->size - ptr->ptr;
    
    rawbuf_size = (buf->Available() < rawbuf_size) ? buf->Available() : rawbuf_size;

    if (!rawbuf_size) {

      if (buf->IsNotSet(wxSND_KEEPQUEUED)) {
        buf->Set(wxSND_UNQUEUEING);
        m_iodrv->m_buffers.DeleteObject(buf);
      }

      if (!LastBuffer() && ptr->ptr) {
        ptr->state = wxBUFFER_FFILLED;
        if (!OnBufferFilled(ptr, wxSND_INPUT))
          return FALSE;
      }
      return TRUE;
    }
    ptr->buffers->Append(buf);

    ptr->ptr += rawbuf_size;


    if (ptr->ptr == ptr->size) {
      ptr->state = wxBUFFER_FFILLED;
      if (!OnBufferFilled(ptr, wxSND_INPUT))
        return FALSE;
    }
  }
  */

  return TRUE;
}

void wxFragmentBuffer::FreeBufToFree(bool force)
{
  wxUint8 i;
  // Garbage collecting

  m_dontq = TRUE;
  m_buf2free = FALSE;

  for (i=0;i<m_maxoq;i++) {
    if ((m_lstoptrs[i].state == wxBUFFER_TOFREE) ||
       (force && m_lstoptrs[i].state == wxBUFFER_FFILLED))
      ClearBuffer(&m_lstoptrs[i]);
  }

  for (i=0;i<m_maxiq;i++) {
    if ((m_lstiptrs[i].state == wxBUFFER_TOFREE) ||
       (force && m_lstoptrs[i].state == wxBUFFER_FFILLED))
      ClearBuffer(&m_lstiptrs[i]);
  }

  m_dontq = FALSE;
}

void wxFragmentBuffer::ClearBuffer(wxFragBufPtr *ptr)
{
  wxNode *node;
  wxSndBuffer *buf;
  char *data;
  wxUint32 size, data_read;

  data = ptr->data;
  size = ptr->size;

  node = ptr->buffers->First();

  while (node) {
    buf = (wxSndBuffer *)node->Data();

    if (buf->GetMode() == wxSND_OUTPUT) {
      buf->OnBufferOutFinished();
    } else {
      data_read = buf->OnBufferInFinished(data, size);

      data += data_read;
      size -= data_read;
    }

    if (buf->IsSet(wxSND_UNQUEUEING))
      buf->Clear(wxSND_UNQUEUEING | wxSND_BUFLOCKED | wxSND_BUFREADY);

    delete node;
    node = ptr->buffers->First();
  }

  ptr->sndbuf->ResetBuffer();
  ptr->state = wxBUFFER_FREE;
}

void wxFragmentBuffer::OnBufferFinished(wxFragBufPtr *ptr)
{
  wxNode *node;
  wxSndBuffer *buf;
  bool ret = TRUE;

  if (m_freeing) {
    ptr->state = wxBUFFER_TOFREE;
    m_buf2free = TRUE;
    return;
  }
  m_freeing = TRUE;

  // Clean up the old buffer.
  if (ptr && ptr->state != wxBUFFER_FREE)
    ClearBuffer(ptr);

  // Garbage collecting ...
  if (m_buf2free)
    FreeBufToFree();

  // If we are queueing, return immediately.
  if (m_dontq) {
    m_freeing = FALSE;
    return;
  }

  node = m_iodrv->m_buffers.First();

  while (node && ret) {
    buf = (wxSndBuffer *)node->Data();
    node = node->Next();

    buf->HardLock();

    // Stop request on this buffer.
    if (buf->IsSet(wxSND_BUFSTOP)) {
      buf->Clear(wxSND_BUFSTOP);
      continue;
    }
    if (buf->GetMode() == wxSND_OUTPUT)
      ret = NotifyOutputBuffer(buf);
    else
      ret = NotifyInputBuffer(buf);

    buf->HardUnlock();
  }
  m_freeing = FALSE;
}
