// --------------------------------------------------------------------------
// Name: sndbase.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999, 2000
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndbase.cpp"
#endif

#include <wx/wxprec.h>
#include "sndbase.h"


// ---------------------------------------------------------------------------
// wxSoundFormatBase
// ---------------------------------------------------------------------------

wxSoundFormatBase::wxSoundFormatBase()
{
}

wxSoundFormatBase::~wxSoundFormatBase()
{
}

wxSoundFormatBase *wxSoundFormatBase::Clone() const
{
  return NULL;
}

bool wxSoundFormatBase::operator!=(const wxSoundFormatBase& frmt2) const
{
  return (GetType() != frmt2.GetType());
}

// ---------------------------------------------------------------------------
// wxSoundStream
// ---------------------------------------------------------------------------

wxSoundStream::wxSoundStream()
{
  int i;

  // Reset all variables to their neutral value.
  m_sndformat = NULL;
  m_handler = NULL;
  m_snderror = wxSOUND_NOERROR;
  m_lastcount = 0;
  for (i=0;i<2;i++)
    m_callback[i] = NULL;
}

wxSoundStream::~wxSoundStream()
{
  if (m_sndformat)
    delete m_sndformat;
}

// --------------------------------------------------------------------------
// SetSoundFormat(const wxSoundFormatBase& format) is one of the most
// important function of the wxSoundStream class. It prepares the stream to
// receive or send the data in a strict format. Normally, the sound stream
// should be ready to accept any format it is asked to manage but in certain
// cases, it really cannot: in that case it returns FALSE. To have more
// details in the functionnalities of SetSoundFormat see
// wxSoundRouterStream::SetSoundFormat()
// --------------------------------------------------------------------------
bool wxSoundStream::SetSoundFormat(const wxSoundFormatBase& format)
{
  // delete the previous prepared format
  if (m_sndformat)
    delete m_sndformat;

  // create a new one by cloning the format passed in parameter
  m_sndformat = format.Clone();
  return TRUE;
}


// --------------------------------------------------------------------------
// Register(int evt, ...) registers the callback for a specified async event.
// Warning ! Only one callback by event is supported. It means that if you
// call twice this function the previous registered callback is absolutely
// ignored.
// --------------------------------------------------------------------------
void wxSoundStream::SetCallback(int evt, wxSoundCallback cbk, void *cdata)
{
  int c;

  switch (evt) {
  case wxSOUND_INPUT:
    c = 0;
    break;
  case wxSOUND_OUTPUT:
    c = 1;
    break;
  default:
    return;
  }
  m_callback[c] = cbk;
  m_cdata[c] = cdata;
}

// --------------------------------------------------------------------------
// OnSoundEvent(int evt) is called either when the driver is ready to receive
// a new block to play or when the driver has a new recorded buffer. You
// must be careful here and try not to spend a lot of time: this is a
// real-time call. In the case, an "event handler" was specified previously, 
// it called him before everything.
// --------------------------------------------------------------------------
void wxSoundStream::OnSoundEvent(int evt)
{
  int c;

  if (m_handler) {
    m_handler->OnSoundEvent(evt);
    return;
  }

  switch (evt) {
  case wxSOUND_INPUT:
    c = 0;
    break;
  case wxSOUND_OUTPUT:
    c = 1;
    break;
  default:
    return;
  }
  if (m_callback[c])
    m_callback[c](this, evt, m_cdata[c]);
}
