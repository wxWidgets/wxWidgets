/////////////////////////////////////////////////////////////////////////////
// Name:        wave.cpp
// Purpose:     wxWave
// Author:      Marcel Rasche
// Modified by:
// Created:     25/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wave.h"
#endif

#include <wx/setup.h>

#if wxUSE_WAVE

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/wave.h"

//-----------------------------------------------------------------
// wxWave
//-----------------------------------------------------------------

wxWave::wxWave()
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
}

wxWave::wxWave(const wxString& sFileName, bool isResource)
  : m_waveData(NULL), m_waveLength(0), m_isResource(isResource)
{
    Create(sFileName, isResource);
}

wxWave::wxWave(int size, const wxByte* data)
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
    Create(size, data);
}

wxWave::~wxWave()
{
    Free();
}

bool wxWave::Create(const wxString& fileName, bool isResource)
{
    Free();
  
    if (isResource)
    {
        //  todo
        return (m_waveData ? TRUE : FALSE);
    }
    else
    {
        m_isResource = FALSE;

        wxFile fileWave;
        if (!fileWave.Open(fileName, wxFile::read))
	{
            return FALSE;
	}

        m_waveLength = (int) fileWave.Length();
    
        m_waveData = new wxByte[m_waveLength];
        if (!m_waveData)
	{
            return FALSE;
	}
    
        fileWave.Read(m_waveData, m_waveLength);
    
        return TRUE;
    }
}

bool wxWave::Create(int size, const wxByte* data)
{
    Free();
    m_isResource = FALSE;
    m_waveLength=size;
    m_waveData = new wxByte[size];
    if (!m_waveData)
    {
        return FALSE;
    }
  
    for (int i=0; i<size; i++) m_waveData[i] = data[i];
    
    return TRUE;
}

bool wxWave::Play(bool async, bool looped)
{
    if (!IsOk()) return FALSE;

    int dev = OpenDSP();
    
    if (dev<0) return FALSE;

    ioctl(dev,SNDCTL_DSP_SYNC,0);
  
    bool play=TRUE;
    int i,l=0;
    do
    {
        i= (int)((l+m_DSPblkSize) < m_sizeData ? m_DSPblkSize : (m_sizeData-l));
        if ( write(dev,&m_data[l],i) != i )
	{
	    play=FALSE;
	}
        l +=i;
    } while (play == TRUE && l<m_sizeData);

    close(dev);
    return TRUE;
}

bool wxWave::Free()
{
    if (m_waveData)
    {
        delete[] m_waveData;
        m_waveData = NULL;
        m_waveLength = 0;
        return TRUE;
    }
  
    return FALSE;
}

typedef  struct
{ 
  unsigned long   uiSize; 
  unsigned short  uiFormatTag;
  unsigned short  uiChannels;
  unsigned long   ulSamplesPerSec;
  unsigned long   ulAvgBytesPerSec;
  unsigned short  uiBlockAlign;
  unsigned short  uiBitsPerSample;
} WAVEFORMAT;    

#define MONO 1  // and stereo is 2 by wav format
#define WAVE_FORMAT_PCM 1
#define WAVE_INDEX  8
#define FMT_INDEX   12

int wxWave::OpenDSP(void)
{
  wxString str;
  WAVEFORMAT  waveformat;
  int dev=-1;
  unsigned long ul;

  if (m_waveLength < (int)(32+sizeof(WAVEFORMAT)))
    return -1;

  memcpy(&waveformat,&m_waveData[FMT_INDEX+4],sizeof(WAVEFORMAT));

  str= wxString(m_waveData,4);
  if (str != "RIFF")    return -1;
  str= wxString(&m_waveData[WAVE_INDEX],4);
  if (str != "WAVE")    return -1;
  str= wxString(&m_waveData[FMT_INDEX],4);
  if (str != "fmt ")    return -1;
  str= wxString(&m_waveData[FMT_INDEX+waveformat.uiSize+8],4);
  if(str != "data")     return -1;
  memcpy(&ul,&m_waveData[FMT_INDEX+waveformat.uiSize+12],4);
  m_sizeData=ul;
  if ((int)(m_sizeData+FMT_INDEX+waveformat.uiSize+16) != m_waveLength)
    return -1;
  m_data=(char *)(&m_waveData[FMT_INDEX+waveformat.uiSize+8]);

  if (waveformat.uiFormatTag != WAVE_FORMAT_PCM)
    return -1;
  if (waveformat.ulSamplesPerSec != waveformat.ulAvgBytesPerSec/waveformat.uiBlockAlign)
    return -1;
   
  if ((dev = open(AUDIODEV,O_RDWR,0)) <0)
    return -1;
  
  if (!InitDSP(dev,(int)waveformat.uiBitsPerSample,waveformat.uiChannels == MONO ? 0:1,waveformat.ulSamplesPerSec))
    {
      close(dev);
      return -1;
    }

  return dev;
}

bool wxWave::InitDSP(int dev, int iDataBits, int iChannel,unsigned long ulSamplingRate)
{
  if ( ioctl(dev,SNDCTL_DSP_GETBLKSIZE,&m_DSPblkSize) < 0 ) 
    return FALSE;
  if (m_DSPblkSize < 4096 || m_DSPblkSize > 65536)
    return FALSE;
  if ( ioctl(dev,SNDCTL_DSP_SAMPLESIZE,&iDataBits) < 0 )
    return FALSE;
  if ( ioctl(dev,SNDCTL_DSP_STEREO,&iChannel) < 0 )
    return FALSE;
  if ( ioctl(dev,SNDCTL_DSP_SPEED,&ulSamplingRate) < 0 ) 
    return FALSE;
  
  return TRUE;
}
#endif

