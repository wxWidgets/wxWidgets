// --------------------------------------------------------------------------
// Name: sndwav.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndwav.cpp"
#endif

#include <wx/defs.h>
#include <wx/stream.h>
#include <wx/datstrm.h>
#include <wx/filefn.h>
#include "sndbase.h"
#include "sndcodec.h"
#include "sndfile.h"
#include "sndpcm.h"
#include "sndwav.h"

#define BUILD_SIGNATURE(a,b,c,d) (((wxUint32)a) | (((wxUint32)b) << 8) | (((wxUint32)c) << 16)  | (((wxUint32)d) << 24)) 

#define RIFF_SIGNATURE BUILD_SIGNATURE('R','I','F','F')
#define WAVE_SIGNATURE BUILD_SIGNATURE('W','A','V','E')
#define FMT_SIGNATURE BUILD_SIGNATURE('f','m','t',' ')
#define DATA_SIGNATURE BUILD_SIGNATURE('d','a','t','a')

#define HEADER_SIZE 4+4 + 4+4+16 + 4+4
 // 4+4 => NAME + LEN
 // 16 => fmt size

wxSoundWave::wxSoundWave(wxInputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
}

wxSoundWave::wxSoundWave(wxOutputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
}

wxSoundWave::~wxSoundWave()
{
}

#define FAIL_WITH(condition, err) if (condition) { m_snderror = err; return FALSE; }

bool wxSoundWave::CanRead()
{
  wxUint32 len, signature;
  m_snderror = wxSOUND_NOERR;

  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);

  if (wxUINT32_SWAP_ON_BE(signature) != RIFF_SIGNATURE) {
    m_input->Ungetch(&signature, 4);
    return FALSE;
  }

  m_input->Read(&len, 4);
  FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);

  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
  if (wxUINT32_SWAP_ON_BE(signature) != WAVE_SIGNATURE) {
    m_input->Ungetch(&signature, 4);
    return FALSE;
  }

  m_input->Ungetch("RIFF", 4);
  m_input->Ungetch(&len, 4);
  m_input->Ungetch("WAVE", 4);

  return TRUE;
}

bool wxSoundWave::PrepareToPlay()
{
  wxUint32 signature, len;
  bool end_headers;

  if (!m_input) {
    m_snderror = wxSOUND_INVSTRM;
    return FALSE;
  }

  wxDataInputStream data(*m_input);
  data.BigEndianOrdered(FALSE);

  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
  FAIL_WITH(wxUINT32_SWAP_ON_BE(signature) != RIFF_SIGNATURE, wxSOUND_INVSTRM);
     // "RIFF"

  len = data.Read32();
  FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);
    // dummy len
 
  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
  FAIL_WITH(wxUINT32_SWAP_ON_BE(signature) != WAVE_SIGNATURE, wxSOUND_INVSTRM);
     // "WAVE"
 
  end_headers = FALSE;
  while (!end_headers) {
    FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);

    len = data.Read32();
    FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);

    switch (wxUINT32_SWAP_ON_BE(signature)) {
    case FMT_SIGNATURE: {  // "fmt "
      wxUint16 format, channels, byte_p_spl, bits_p_spl;
      wxUint32 sample_fq, byte_p_sec;
      wxSoundFormatPcm sndformat;

      data >> format >> channels >> sample_fq 
           >> byte_p_sec >> byte_p_spl >> bits_p_spl;
      FAIL_WITH(format != 1, wxSOUND_NOCODEC);
      
      sndformat.SetSampleRate(sample_fq);
      sndformat.SetBPS(bits_p_spl);
      sndformat.SetChannels(channels);
      sndformat.Signed(TRUE);
      sndformat.SetOrder(wxLITTLE_ENDIAN);

      if (!SetSoundFormat(sndformat))
        return FALSE;
      m_input->SeekI(len-16, wxFromCurrent);
      break;
    }
    case DATA_SIGNATURE: // "data"
      end_headers = TRUE;
      break;
    default:
      m_input->SeekI(len, wxFromCurrent);
      break;
    }
  }
  return TRUE;
}

bool wxSoundWave::PrepareToRecord(unsigned long time)
{
#define WRITE_SIGNATURE(sig) \
signature = sig; \
signature = wxUINT32_SWAP_ON_BE(signature); \
FAIL_WITH(m_output->Write(&signature, 4).LastWrite() != 4, wxSOUND_INVSTRM);

  wxUint32 signature, len;

  if (!m_output) {
    m_snderror = wxSOUND_INVSTRM;
    return FALSE;
  }

  wxDataOutputStream data(*m_output);
  data.BigEndianOrdered(FALSE);

  len = m_sndformat->GetBytesFromTime(time);

  len += HEADER_SIZE;

  WRITE_SIGNATURE(RIFF_SIGNATURE);

  data << len;
  FAIL_WITH(m_output->LastWrite() != 4, wxSOUND_INVSTRM);

  WRITE_SIGNATURE(WAVE_SIGNATURE);

  {
    wxUint16 format, channels, byte_p_spl, bits_p_spl;
    wxUint32 sample_fq, byte_p_sec;
    wxSoundFormatPcm *pcm;

    if (m_sndformat->GetType() != wxSOUND_PCM) {
      m_snderror = wxSOUND_NOCODEC;
      return FALSE;
    }

    pcm = (wxSoundFormatPcm *)(m_sndformat->Clone());

    WRITE_SIGNATURE(FMT_SIGNATURE);
    data.Write32(16);

    sample_fq  = pcm->GetSampleRate();
    bits_p_spl = pcm->GetBPS();
    channels   = pcm->GetChannels();
    byte_p_spl = pcm->GetBPS() / 8;
    byte_p_sec = pcm->GetBytesFromTime(1);
    format     = 1;
    data << format << channels << sample_fq
         << byte_p_sec << byte_p_spl << bits_p_spl;

    pcm->Signed(TRUE);
    pcm->SetOrder(wxLITTLE_ENDIAN);
    
    if (!SetSoundFormat(*pcm))
      return FALSE;

    delete pcm;
  }

  WRITE_SIGNATURE(DATA_SIGNATURE);
  data.Write32(m_sndformat->GetBytesFromTime(time));
  return TRUE;
}

bool wxSoundWave::FinishRecording()
{
  // TODO: Update headers when we stop before the specified time (if possible)
  return TRUE;
}

size_t wxSoundWave::GetData(void *buffer, size_t len)
{
  return m_input->Read(buffer, len).LastRead();
}

size_t wxSoundWave::PutData(const void *buffer, size_t len)
{
  return m_output->Write(buffer, len).LastWrite();
}
