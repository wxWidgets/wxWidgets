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

// ------------------------------------------------------------------------
// DEFINITIONS

// ---------------------
// Sound streaming mode:
//   - wxSOUND_INPUT: simple recording mode
//   - wxSOUND_OUTPUT: simple playing mode
//   - wxSOUND_DUPLEX: full duplex record/play at the same time
// ---------------------
enum {
  wxSOUND_INPUT = 1,
  wxSOUND_OUTPUT = 2,
  wxSOUND_DUPLEX = wxSOUND_INPUT | wxSOUND_OUTPUT,
};

// ---------------------
// wxSoundFormatType: it specifies the format family of the sound data
// which will be passed to the stream.
// ---------------------
typedef enum {
  wxSOUND_NOFORMAT,
  wxSOUND_PCM,
  wxSOUND_ULAW,
  wxSOUND_G72X,
  wxSOUND_MSADPCM
} wxSoundFormatType;

// ---------------------
// wxSoundError:
//   - wxSOUND_NOERR: No error occured
//   - wxSOUND_IOERR: an input/output error occured, it may concern either
//                    a driver or a file
//   - wxSOUND_INVFRMT: the sound format passed to the function is invalid.
//                      Generally, it means that you passed out of range values
//                      to the codec stream or you don't pass the right sound
//                      format object to the right sound codec stream.
//   - wxSOUND_INVDEV: Invalid device. Generally, it means that the sound stream
//                     didn't manage to open the device driver due to an invalid//                     parameter or to the fact that sound is not supported on
//                     this computer.
//   - wxSOUND_NOEXACT: No exact matching sound codec has been found for
//                      this sound format. It means that the sound driver didn't
//                      manage to setup the sound card with the specified
//                      values.
//   - wxSOUND_NOCODEC: No matching codec has been found. Generally, it 
//                      may happen when you call
//                      wxSoundRouterStream::SetSoundFormat().
//   - wxSOUND_MEMERR:  Not enough memory.
// ---------------------
typedef enum {
  wxSOUND_NOERROR,
  wxSOUND_IOERROR,
  wxSOUND_INVFRMT,
  wxSOUND_INVDEV,
  wxSOUND_NOEXACT,
  wxSOUND_INVSTRM,
  wxSOUND_NOCODEC,
  wxSOUND_MEMERROR
} wxSoundError;

class WXDLLEXPORT wxSoundStream;

// ---------------------
// wxSoundCallback(stream, evt, cdata): C callback for sound event.
//    - stream: current wxSoundStream
//    - evt: the sound event which has occured, it may be wxSOUND_INPUT,
//            wxSOUND_OUTPUT or wxSOUND_DUPLEX
//    - cdata: User callback data
// ---------------------
typedef void (*wxSoundCallback)(wxSoundStream *stream, int evt,
                                void *cdata);

//
// Base class for sound format specification
//

class WXDLLEXPORT wxSoundFormatBase {
 public:
  wxSoundFormatBase();
  virtual ~wxSoundFormatBase();

  // It returns a "standard" format type.
  virtual wxSoundFormatType GetType() const { return wxSOUND_NOFORMAT; }
  // It clones the current format.
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
  void SetCallback(int evt, wxSoundCallback cbk, void *cdata);

  // Starts the async notifier. After this call, the stream begins either 
  // recording or playing or the two at the same time.
  virtual bool StartProduction(int evt) = 0;
  // Stops the async notifier.
  virtual bool StopProduction() = 0;
  // Sets the event handler: if it is non-null, all events are routed to it.
  void SetEventHandler(wxSoundStream *handler) { m_handler = handler; }

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
  void *m_cdata[2];

 protected:
  // Handles event
  virtual void OnSoundEvent(int evt);
};

#endif
