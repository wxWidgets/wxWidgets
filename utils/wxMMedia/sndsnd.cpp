////////////////////////////////////////////////////////////////////////////////
// Name:       sndsnd.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndsnd.h"
#endif
#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include "sndsnd.h"
#include "sndfrmt.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#define PROCESS_EVENT() wxYield()
// #define PROCESS_EVENT()

// ----------------------------------------------------------------------------
// wxSndBuffer: base sound buffer class
// ----------------------------------------------------------------------------


wxSndBuffer::wxSndBuffer()
  : m_sndmode(wxSND_OUTPUT), m_sndflags(0), m_sndoutput(NULL), m_sndcodec(NULL)
{
}

wxSndBuffer::~wxSndBuffer()
{
}

void wxSndBuffer::Set(wxSndFlags flags)
{
  m_sndflags |= flags;

  if ((m_sndflags & wxSND_BUFAUTO) && (flags & wxSND_BUFREADY))
    m_sndoutput->QueueBuffer(*this);
}

void wxSndBuffer::SetError(wxSndError error)
{
  if (error == wxSND_NOERROR)
    Clear(wxSND_BUFERR);
  else
    Set(wxSND_BUFERR);

  m_snderror = error;
}

wxSndError wxSndBuffer::GetError()
{
  if (IsNotSet(wxSND_BUFERR))
    return wxSND_NOERROR;
  Clear(wxSND_BUFERR);
  return m_snderror;
}

void wxSndBuffer::OnPlayFinished()
{
}

void wxSndBuffer::OnBufferOutFinished()
{
}

void wxSndBuffer::OnBufferInFinished(char *WXUNUSED(iobuf),
                                     wxUint32& WXUNUSED(size))
{
}

bool wxSndBuffer::Wait()
{
  if (IsNotSet(wxSND_BUFLOCKED))
    return FALSE;

  while (IsSet(wxSND_BUFLOCKED))
    PROCESS_EVENT();

  return IsNotSet(wxSND_BUFERR);
}

void wxSndBuffer::HardLock()
{
  m_mutex.Lock();
}

void wxSndBuffer::HardUnlock()
{
  m_mutex.Unlock();
}

void wxSndBuffer::ChangeCodec(int no)
{
  m_sndformat.SetCodecNo(no);
  m_sndcodec = m_sndformat.GetCodec();
  m_sndcodec->SetIOBuffer(this);
}

// ----------------------------------------------------------------------------
// wxSndSimpleBuffer: the simplest sound buffer
// ----------------------------------------------------------------------------


wxSndSimpleBuffer::wxSndSimpleBuffer(char *buffer, wxUint32 bufsize,
				     wxSndMode mode)
  : wxSndBuffer()
{
  m_sndbuf = buffer;
  m_sndsize = bufsize;
  m_sndmode = mode;

  m_sndptr = 0;
}

wxSndSimpleBuffer::~wxSndSimpleBuffer()
{
}

void wxSndSimpleBuffer::OnNeedOutputData(char *iobuf, wxUint32& size)
{
  char *buf = m_sndbuf + m_sndptr;
  wxUint32 nbdata_left = m_sndsize - m_sndptr;

  if (m_sndptr >= m_sndsize) {
    size = 0;
    return;
  }

  if (size > nbdata_left)
    size = nbdata_left;

  m_sndptr += size;

  memcpy(iobuf, buf, size);
}

void wxSndSimpleBuffer::OnBufferOutFinished()
{
  if (m_sndptr >= m_sndsize)
    OnPlayFinished();
}

void wxSndSimpleBuffer::OnBufferInFinished(char *iobuf, wxUint32& size)
{
  char *raw_buf = m_sndbuf + m_sndptr;
  wxUint32 data_left = m_sndsize - m_sndptr;

  if (!data_left) {
    size = 0;
    return;
  }

  if (size > data_left)
    size = data_left;

  memcpy(raw_buf, iobuf, size);
  m_sndptr += size;
}

void wxSndSimpleBuffer::SetData(char *buffer, wxUint32 bufsize,
				wxSndMode mode)
{
  m_sndbuf = buffer;
  m_sndsize = bufsize;
  m_sndmode = mode;
}

bool wxSndSimpleBuffer::RestartBuffer(wxSndMode mode)
{
  m_sndptr = 0;
  return TRUE;
}

wxUint32 wxSndSimpleBuffer::GetSize() const
{
  return m_sndsize;
}

wxUint32 wxSndSimpleBuffer::Available() const
{
  return m_sndsize - m_sndptr;
}

// ----------------------------------------------------------------------------
// wxSound: base sound driver implementation
// ----------------------------------------------------------------------------

wxSound::wxSound()
  : wxObject(),
    m_lastbuf(NULL), m_sndcbk(NULL), m_snderror(wxSND_NOERROR)
{
  m_buffers.Clear();
}

wxSound::~wxSound()
{
  wxNode *node = m_buffers.First();

  while (node) {
    wxSndBuffer *buf = (wxSndBuffer *)node->Data();

    buf->Clear(wxSND_BUFLOCKED);
  }
}

bool wxSound::QueueBuffer(wxSndBuffer& buf)
{
  if (buf.IsSet(wxSND_BUFLOCKED) || buf.IsNotSet(wxSND_BUFREADY))
    return FALSE;

  buf.Set(wxSND_BUFLOCKED);
  buf.SetOutput(*this);

  m_buffers.Append(&buf);
  return Wakeup(buf);
}

bool wxSound::UnqueueBuffer(wxSndBuffer& buf)
{
  wxNode *node;

  if (buf.IsNotSet(wxSND_BUFLOCKED))
    return FALSE;

  node = m_buffers.Member(&buf);
  if (!node)
    return FALSE;

  StopBuffer(buf);
  node = m_buffers.Member(&buf);
  if (node)
    delete node;

  return TRUE;
}

void wxSound::Callback(wxSndCallback cbk)
{
  m_sndcbk = cbk;
}

void wxSound::SetClientData(char *cdata)
{
  m_cdata = cdata;
}

void wxSound::OnPlayBuffer(wxSndBuffer& buf)
{
  m_lastbuf = &buf;
  if (m_sndcbk)
    m_sndcbk(*this, buf, m_cdata);
}
