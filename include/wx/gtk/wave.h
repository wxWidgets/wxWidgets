/////////////////////////////////////////////////////////////////////////////
// Name:        wave.h
// Purpose:     wxWave class
// Author:      Julian Smart
// Modified by:
// Created:     25/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WAVE_H_
#define _WX_WAVE_H_

#ifdef __GNUG__
#pragma interface "wave.h"
#endif

#ifndef byte
#define byte unsigned char
#endif

#include "wx/object.h"

#ifndef AUDIODEV
#define AUDIODEV   "/dev/dsp"    // Default path for audio device
#endif

class wxWave : public wxObject
{
public:
  wxWave();
  wxWave(const wxString& fileName, bool isResource = FALSE);
  wxWave(int size, const byte* data);
  ~wxWave();

public:
  // Create from resource or file
  bool  Create(const wxString& fileName, bool isResource = FALSE);
  // Create from data
  bool Create(int size, const byte* data);

  bool  IsOk() const { return (m_waveData ? TRUE : FALSE); };
  bool  Play(bool async = TRUE, bool looped = FALSE);

protected:
  bool  Free();

private:
  byte* m_waveData;
  int   m_waveLength;
  bool  m_isResource;


  int OpenDSP(void);
  bool InitDSP(int dev, int iDataBits, int iChannel,unsigned long ulSamplingRate);
  int m_DSPblkSize;        // Size of the DSP buffer
  char *m_data;
  int m_sizeData;
};

#endif

