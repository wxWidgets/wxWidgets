/* Real -*- C++ -*- */
// /////////////////////////////////////////////////////////////////////////////
// Name:       sndsnd.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __WX_SND_SOUND_H__
#define __WX_SND_SOUND_H__
#ifdef __GNUG__
#pragma interface
#endif

#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include <wx/thread.h>
#include "sndfrmt.h"
#include "mmtype.h"

class wxSound;
class wxSndBuffer;

typedef enum {
  wxSND_OUTPUT,
  wxSND_INPUT,
  wxSND_DUPLEX,
  wxSND_OTHER_IO
} /// The possible sound output modes
 wxSndMode;

typedef enum {
  wxSND_NOERROR = 0,
  wxSND_NOCAPS,
  wxSND_CANTOPEN,
  wxSND_NOMEM,
  wxSND_READERR,
  wxSND_WRITEERR,
  wxSND_CANTSET
} /// Sound errors
 wxSndError;

/// Sound callback
typedef void (*wxSndCallback)(wxSound&, wxSndBuffer&, char *);

/// Sound flags
typedef wxUint16 wxSndFlags;

/** @name Sound buffer flags */
/// buffer ready
#define wxSND_BUFREADY   0x0001
/// an error occured
#define wxSND_BUFERR     0x0002
/// buffer is in use
#define wxSND_BUFLOCKED  0x0004
/// the driver mustn't unqueue it
#define wxSND_KEEPQUEUED 0x0008
/// automatic: when BUFREADY is set play the buffer 
#define wxSND_BUFAUTO    0x0010
///
#define wxSND_UNFINISHED 0x0020
/// buffer is nearly being unqueued
#define wxSND_UNQUEUEING 0x0040
/// driver wants the buffer stop
#define wxSND_BUFSTOP    0x0080
/// buffer will loop
#define wxSND_LOOP       0x0100

/** @name Sound data format */
/// little endian
#define wxSND_SAMPLE_LE  0
/// big endian
#define wxSND_SAMPLE_BE  1
/// unsigned samples
#define wxSND_SAMPLE_UNSIGNED 0
/// signed samples
#define wxSND_SAMPLE_SIGNED   1

/** @name wxSndBuffer
  * @memo wxSndBuffer is the basic class for all the sound codec.
  * @author Guilhem Lavaux
  */
class wxSndBuffer : public wxObject {
  /// It is an abstract class
  DECLARE_ABSTRACT_CLASS(wxSndBuffer)
protected:
  wxMutex m_mutex;
  /// output mode
  wxSndMode m_sndmode;
  /// last error
  wxSndError m_snderror;
  /// some flag
  wxSndFlags m_sndflags;
  /// last sound driver used
  wxSound *m_sndoutput;
  /// sound data format
  wxSoundDataFormat m_sndformat;
  /// current sound codec
  wxSoundCodec *m_sndcodec;
public:
  /** @name constructor and destructor */
  //@{
  /// Construct an uninitialized wxSndBuffer
  wxSndBuffer();
  /// Destroy
  virtual ~wxSndBuffer();
  //@}

  /** @name Functions returning the current state */
  //@{
    /// @return current mode
  inline wxSndMode GetMode() const { return m_sndmode; }
    /// @return sound data format
  inline wxSoundDataFormat& GetFormat() { return m_sndformat; }
     /// @return the size of the buffer
  virtual wxUint32 GetSize() const = 0;
    /// @return bytes left
  virtual wxUint32 Available() const = 0;
 
    /** enable the specified flags
      * @param flags
      */
  void Set(wxSndFlags flags);
    /** disable the specified flags
      * @param flags
      */
  inline void Clear(wxSndFlags flags)
      { m_sndflags &= ~flags; }
    /** Check if the specified flags is set
      * @param flags
      * @return TRUE if all flags is set
      */
  inline bool IsSet(wxSndFlags flags) const
      { return ((m_sndflags & flags) == flags); }
    /** Check if the specified flags is not set
      * @param flags
      * @return TRUE if at least one flag is not set
      */
  inline bool IsNotSet(wxSndFlags flags) const
      { return ((m_sndflags & flags) != flags); }
    /** Check if the buffer is currently being played
      * @return TRUE
       if the buffer is being played
      */
  inline bool IsPlaying() const
      { return IsSet(wxSND_BUFLOCKED); }
  //@}

  ///
  inline void SetOutput(wxSound& snd)
      { m_sndoutput = &snd; }
  ///
  inline wxSoundCodec *GetCurrentCodec() const
      { return m_sndcodec; }
  ///
  void HardLock();
  ///
  void HardUnlock();

  ///
  wxSndError GetError();
  ///
  void SetError(wxSndError err);

  ///
  virtual bool Wait();
  ///
  virtual bool RestartBuffer(wxSndMode mode) = 0;
  ///
  virtual bool Abort() { return TRUE; }

  ///
  virtual void OnPlayFinished();

  /** Data exchanging functions */
  //@{
  ///
  virtual void OnNeedOutputData(char *io_buf, wxUint32& size) = 0;
  ///
  virtual void OnBufferOutFinished();
  ///
  virtual void OnBufferInFinished(char *iobuf, wxUint32& size);
  //@}

protected:
  void ChangeCodec(int no);
};

class wxSndSimpleBuffer : public wxSndBuffer {
  DECLARE_DYNAMIC_CLASS(wxSndSimpleBuffer)
protected:
  /// sound buffer
  char *m_sndbuf;
  /// size of the sound buffer
  wxUint32 m_sndsize;
  /// current position in the sound buffer
  wxUint32 m_sndptr;
public:
  wxSndSimpleBuffer(char *buffer = NULL, wxUint32 bufsize = 0,
                    wxSndMode mode = wxSND_OUTPUT);
  virtual ~wxSndSimpleBuffer();

  void SetData(char *buffer, wxUint32 bufsize,
               wxSndMode mode = wxSND_OUTPUT);
  inline void SetSoundFormat(const wxSoundDataFormat& format);

  void OnNeedOutputData(char *io_buf, wxUint32& size);
  void OnNeedInputData(wxUint32& size);

  void OnBufferOutFinished();
  void OnBufferInFinished(char *iobuf, wxUint32& size);

  bool RestartBuffer(wxSndMode mode);
  wxUint32 GetSize() const;
  wxUint32 Available() const;
};

///
class wxSound : public wxObject {
  ///
  DECLARE_ABSTRACT_CLASS(wxSound)
protected:
  friend class wxFragmentBuffer;

  ///
  wxSndBuffer *m_lastbuf;
  ///
  wxList m_buffers;
  ///
  wxSndCallback m_sndcbk;
  ///
  wxSndError m_snderror;
  ///
  char *m_cdata;
public:
  ///
  wxSound();
  ///
  virtual ~wxSound();

  ///
  virtual bool QueueBuffer(wxSndBuffer& buf);
  ///
  virtual bool UnqueueBuffer(wxSndBuffer& buf);
  ///
  inline wxSndBuffer *LastBufferPlayed()
    { return m_lastbuf; }

  ///
  wxSndError GetError() { return m_snderror; }

  ///
  void Callback(wxSndCallback cbk);
  ///
  void SetClientData(char *cdata);
  ///
  virtual void OnPlayBuffer(wxSndBuffer& buf);
protected:
  ///
  virtual bool Wakeup(wxSndBuffer& buf) = 0;
  ///
  virtual void StopBuffer(wxSndBuffer& buf) = 0;

  ///
  virtual inline bool OnSetupDriver(wxSndBuffer& WXUNUSED(buf),
                                      wxSndMode WXUNUSED(mode))
             { return TRUE; }
};

#endif
