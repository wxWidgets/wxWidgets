// --------------------------------------------------------------------------
// Name: sndaiff.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndaiff.cpp"
#endif

#include <wx/wxprec.h>

#include <wx/stream.h>
#include <wx/datstrm.h>
#include <wx/filefn.h>
#include "sndbase.h"
#include "sndcodec.h"
#include "sndfile.h"
#include "sndpcm.h"
#include "sndaiff.h"

#define BUILD_SIGNATURE(a,b,c,d) (((wxUint32)a) | (((wxUint32)b) << 8) | (((wxUint32)c) << 16)  | (((wxUint32)d) << 24))

#define FORM_SIGNATURE BUILD_SIGNATURE('F','O','R','M')
#define AIFF_SIGNATURE BUILD_SIGNATURE('A','I','F','F')
#define AIFC_SIGNATURE BUILD_SIGNATURE('A','I','F','C')
#define COMM_SIGNATURE BUILD_SIGNATURE('C','O','M','M')
#define SSND_SIGNATURE BUILD_SIGNATURE('S','S','N','D')

wxSoundAiff::wxSoundAiff(wxInputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
}

wxSoundAiff::wxSoundAiff(wxOutputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
}

wxSoundAiff::~wxSoundAiff()
{
}

wxString wxSoundAiff::GetCodecName() const
{
    return "wxSoundAiff codec";
}

bool wxSoundAiff::CanRead()
{
  wxUint32 signature1, signature2, len;

  if (m_input->Read(&signature1, 4).LastRead() != 4)
    return FALSE;

  if (wxUINT32_SWAP_ON_BE(signature1) != FORM_SIGNATURE) {
    m_input->Ungetch(&signature1, 4);
    return FALSE;
  }

  m_input->Read(&len, 4);
  if (m_input->LastRead() != 4) {
    m_input->Ungetch(&len, m_input->LastRead());
    m_input->Ungetch(&signature1, 4);
    return FALSE;
  }

  if (m_input->Read(&signature2, 4).LastRead() != 4) {
    m_input->Ungetch(&signature2, m_input->LastRead());
    m_input->Ungetch(&len, 4);
    m_input->Ungetch(&signature1, 4);
    return FALSE;
  }

  m_input->Ungetch(&signature2, 4);
  m_input->Ungetch(&len, 4);
  m_input->Ungetch(&signature1, 4);

  if (
         wxUINT32_SWAP_ON_BE(signature2) != AIFF_SIGNATURE &&
         wxUINT32_SWAP_ON_BE(signature2) != AIFC_SIGNATURE)
    return FALSE;

  return TRUE;
}

#define FAIL_WITH(condition, err) if (condition) { m_snderror = err; return FALSE; }

bool wxSoundAiff::PrepareToPlay()
{
  wxDataInputStream data(*m_input);
  wxUint32 signature, len, ssnd;
  bool end_headers;

  if (!m_input) {
    m_snderror = wxSOUND_INVSTRM;
    return FALSE;
  }

  data.BigEndianOrdered(TRUE);

  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
  FAIL_WITH(wxUINT32_SWAP_ON_BE(signature) != FORM_SIGNATURE, wxSOUND_INVSTRM);
     // "FORM"

  len = data.Read32(); 
  FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);
    // dummy len
 
  FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
  FAIL_WITH(
         wxUINT32_SWAP_ON_BE(signature) != AIFF_SIGNATURE &&
         wxUINT32_SWAP_ON_BE(signature) != AIFC_SIGNATURE, wxSOUND_INVSTRM);
     // "AIFF" / "AIFC"
 
  end_headers = FALSE;
  while (!end_headers) {
    FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);

    len = data.Read32();
    FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);

    switch (wxUINT32_SWAP_ON_BE(signature)) {
    case COMM_SIGNATURE: { // "COMM"
      wxUint16 channels, bps;
      wxUint32 num_samples;
      double srate;
      wxSoundFormatPcm sndformat;

      data >> channels >> num_samples >> bps >> srate; 
      
      sndformat.SetSampleRate((wxUint32) srate);
      sndformat.SetBPS(bps);
      sndformat.SetChannels(channels);
      sndformat.Signed(FALSE);
      sndformat.SetOrder(wxBIG_ENDIAN);

      if (!SetSoundFormat(sndformat))
        return FALSE;
      m_input->SeekI(len-18, wxFromCurrent);
      break;
    }
    case SSND_SIGNATURE: {  // "SSND"
      data >> ssnd;
      // m_input->SeekI(4, wxFromCurrent);  // Pass an INT32
      // m_input->SeekI(len-4, wxFromCurrent); // Pass the rest
      m_input->SeekI(ssnd + 4, wxFromCurrent);
      FinishPreparation(len - 8);
      end_headers = TRUE;
      break;
    }
    default:
      m_input->SeekI(len, wxFromCurrent);
      break;
    }
  }
  return TRUE;
}

bool wxSoundAiff::PrepareToRecord(unsigned long time)
{
  // TODO
  return FALSE;
}

bool wxSoundAiff::FinishRecording()
{
  // TODO
  return FALSE;
}

wxUint32 wxSoundAiff::GetData(void *buffer, wxUint32 len)
{
  return m_input->Read(buffer, len).LastRead();
}

wxUint32 wxSoundAiff::PutData(const void *buffer, wxUint32 len)
{
  return m_output->Write(buffer, len).LastWrite();
}
