// --------------------------------------------------------------------------
// Name: sndcpcm.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndcpcm.cpp"
#endif

#include "sndbase.h"
#include "sndpcm.h"
#include "sndcpcm.h"

wxSoundStreamPcm::wxSoundStreamPcm(wxSoundStream& sndio)
 : wxSoundStreamCodec(sndio)
{
  m_function_in = NULL;
  m_function_out = NULL;
}

wxSoundStreamPcm::~wxSoundStreamPcm()
{
}


#define SWAP_BYTES 0
#include "converter.def"
#undef SWAP_BYTES

#define SWAP_BYTES 1
#include "converter.def"
#undef SWAP_BYTES

wxSoundStreamPcm::ConverterType s_convert_16_to_8[] = {
  Convert_16to8_16_no,
  Convert_16to8_U2S_16_no,
  NULL,
  NULL,
  Convert_16to8_U2S_16_yes,
  Convert_16to8_16_yes,
};

wxSoundStreamPcm::ConverterType s_convert_16[] = {
  NULL,
  Convert_U2S_16_no,
  Convert_U2S_SWAP_16_no,
  Convert_U2S_SWAP_16_yes,
  Convert_U2S_16_yes,
  Convert_SWAP_16_no
};

wxSoundStreamPcm::ConverterType s_convert_8[] = {
  NULL,
  Convert_U2S_8,
  Convert_U2S_8,
  Convert_U2S_8,
  Convert_U2S_8,
  NULL
};

#define CONVERTER 0
#define CONVERTER_SIGN 1
#define CONVERTER_SIGN_SWAP 2
#define CONVERTER_SWAP_SIGN_SWAP 3
#define CONVERTER_SWAP_SIGN 4
#define CONVERTER_SWAP 5

wxSoundStream& wxSoundStreamPcm::Read(void *buffer, size_t len)
{
  if (!m_function_in) {
    m_sndio->Read(buffer, len);
    m_lastcount = m_sndio->GetLastAccess();
    m_snderror = m_sndio->GetError();
    return *this;
  }

  // TODO
  m_sndio->Read(buffer, len);
  m_lastcount = m_sndio->GetLastAccess();
  m_snderror = m_sndio->GetError();
  return *this;
}

wxSoundStream& wxSoundStreamPcm::Write(const void *buffer, size_t len)
{
  char *tmp_buf;
  size_t len2;

  if (!m_function_out)
    return m_sndio->Write(buffer, len);

  len2 = (m_16_to_8) ? len / 2 : len;

  tmp_buf = new char[len2];
  m_function_out((const char *)buffer, tmp_buf, len);
  m_sndio->Write(tmp_buf, len);
  delete[] tmp_buf;

  m_lastcount = (m_16_to_8) ? 
                    (m_sndio->GetLastAccess() * 2) : m_sndio->GetLastAccess();

  return *this;
}

bool wxSoundStreamPcm::SetSoundFormat(const wxSoundFormatBase& format)
{
  wxSoundFormatBase *new_format;
  wxSoundFormatPcm *pcm_format, *pcm_format2;
  ConverterType *current_table;
  int index;
  bool change_sign;

  if (m_sndio->SetSoundFormat(format)) {
    m_function_out = NULL;
    m_function_in = NULL;
    return TRUE;
  }
  if (format.GetType() != wxSOUND_PCM) {
    m_snderror = wxSOUND_INVFRMT;
    return FALSE;
  }
  if (m_sndformat)
    delete m_sndformat;

  new_format = m_sndio->GetSoundFormat().Clone();
  pcm_format = (wxSoundFormatPcm *)&format;
  pcm_format2 = (wxSoundFormatPcm *)new_format;

  m_16_to_8 = FALSE;
  if (pcm_format->GetBPS() == 16 && pcm_format2->GetBPS() == 8) {
    m_16_to_8 = TRUE;
    current_table = s_convert_16_to_8;
  } else if (pcm_format->GetBPS() == 16)
    current_table = s_convert_16;
  else
    current_table = s_convert_8;

  change_sign = (pcm_format2->Signed() != pcm_format->Signed());

#define MY_ORDER wxBYTE_ORDER
#if wxBYTE_ORDER == wxLITTLE_ENDIAN
#define OTHER_ORDER wxBIG_ENDIAN
#else
#define OTHER_ORDER wxLITTLE_ENDIAN
#endif

  if (pcm_format->GetOrder() == OTHER_ORDER &&
      pcm_format2->GetOrder() == OTHER_ORDER && change_sign) 
    index = CONVERTER_SWAP_SIGN_SWAP;

  else if (pcm_format->GetOrder() == OTHER_ORDER &&
           pcm_format2->GetOrder() == MY_ORDER && change_sign) 
    index = CONVERTER_SWAP_SIGN;

  else if (pcm_format->GetOrder() == MY_ORDER &&
           pcm_format->GetOrder() == OTHER_ORDER && change_sign)
    index = CONVERTER_SIGN_SWAP;

  else if (!change_sign &&
           pcm_format->GetOrder() != pcm_format2->GetOrder())
    index = CONVERTER_SWAP;

  else
    index = CONVERTER;

  m_function_out = current_table[index];
//  m_function_in = current_table[index+1];

  m_sndio->SetSoundFormat(*new_format);
  m_sndformat = new_format;
  return TRUE;
}
