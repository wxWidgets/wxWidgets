// --------------------------------------------------------------------------
// Name: sndbase.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDBASE_H
#define _WX_SNDBASE_H

#ifdef __GNUG__
#pragma interface "sndbase.h"
#endif

#include <wx/defs.h>

enum {
  wxSOUND_INPUT = 1,
  wxSOUND_OUTPUT = 2,
  wxSOUND_DUPLEX = wxSOUND_INPUT | wxSOUND_OUTPUT,
};

typedef enum {
  wxSOUND_NOFORMAT,
  wxSOUND_PCM,
  wxSOUND_ULAW,
  wxSOUND_G72X
} wxSoundFormatType;

typedef enum {
  wxSOUND_NOERR,
  wxSOUND_IOERR,
  wxSOUND_INVFRMT,
  wxSOUND_INVDEV,
  wxSOUND_NOTEXACT,
  wxSOUND_INVSTRM,
  wxSOUND_NOCODEC,
  wxSOUND_MEMERR
} wxSoundError;

class WXDLLEXPORT wxSoundStream;

typedef void (*wxSoundCallback)(wxSoundStream *stream, int evt,
                                char *cdata);

//
// Base class for sound format specification
//

class WXDLLEXPORT wxSoundFormatBase {
 public:
  wxSoundFormatBase();
  virtual ~wxSoundFormatBase();

  virtual wxSoundFormatType GetType() const { return wxSOUND_NOFORMAT; }
  virtual wxSoundFormatBase *Clone() const;

  virtual wxUint32 GetTimeFromBytes(wxUint32 bytes) const = 0;
  virtual wxUint32 GetBytesFromTime(wxUint32 time) const = 0;
 
  virtual bool operator !=(const wxSoundFormatBase& frmt2) const;
};

//
// Base class for sound streams
//

class wxSoundStream {
 public:
  wxSoundStream();
  virtual ~wxSoundStream();

  // Reads "len" bytes from the sound stream.
  virtual wxSoundStream& Read(void *buffer, wxUint32 len) = 0;
  // Writes "len" byte to the sound stream.
  virtual wxSoundStream& Write(const void *buffer, wxUint32 len) = 0;
  // Returns the best size for IO calls
  virtual wxUint32 GetBestSize() const { return 1024; }

  // SetSoundFormat returns TRUE when the format can be handled.
  virtual bool SetSoundFormat(const wxSoundFormatBase& format);

  // GetSoundFormat returns the current sound format.
  wxSoundFormatBase& GetSoundFormat() const { return *m_sndformat; }

  // Register a callback for a specified async event.
  void Register(int evt, wxSoundCallback cbk, char *cdata);

  // Starts the async notifier.
  virtual bool StartProduction(int evt) = 0;
  // Stops the async notifier.
  virtual bool StopProduction() = 0;
  // Sets the event handler: if it is non-null, all events are routed to it.
  void SetEventHandler(wxSoundStream *handler) { m_handler = handler; }

  // Initializes the full duplex mode.
  virtual void SetDuplexMode(bool duplex) = 0;

  wxSoundError GetError() const { return m_snderror; }
  wxUint32 GetLastAccess() const { return m_lastcount; }

  // This is only useful for device (I think).
  virtual bool QueueFilled() const { return TRUE; }

 protected:
  // Current sound format
  wxSoundFormatBase *m_sndformat;

  // Last error
  wxSoundError m_snderror;

  // Last access
  wxUint32 m_lastcount;

  // Event handler
  wxSoundStream *m_handler;

  wxSoundCallback m_callback[2];
  char *m_cdata[2];

 protected:
  // Handles event
  virtual void OnSoundEvent(int evt);
};

#endif
