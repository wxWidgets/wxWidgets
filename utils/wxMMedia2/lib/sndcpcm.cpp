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

#include <wx/wxprec.h>
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

wxSoundStreamPcm::ConverterType s_convert_out_16_to_8[] = {
  Convert_16to8_16_no,
  Convert_16to8_U2S_16_no,
  NULL,
  NULL,
  Convert_16to8_U2S_16_yes,
  Convert_16to8_16_yes
};

wxSoundStreamPcm::ConverterType s_convert_out_16[] = {
  NULL,
  Convert_U2S_16_no,
  Convert_U2S_SWAP_16_no,
  Convert_U2S_SWAP_16_yes,
  Convert_U2S_16_yes,
  Convert_SWAP_16_no
};

wxSoundStreamPcm::ConverterType s_convert_out_8[] = {
  NULL,
  Convert_U2S_8,
  Convert_U2S_8,
  Convert_U2S_8,
  Convert_U2S_8,
  NULL
};

wxSoundStreamPcm::ConverterType s_convert_in_8_to_16[] = {
  Convert_8to16_8,
  Convert_8to16_U2S_8,
  Convert_8to16_U2S_SWAP_8,
  NULL,
  NULL,
  Convert_8to16_SWAP_8
};

wxSoundStreamPcm::ConverterType *s_convert_in_16 = s_convert_out_16;

wxSoundStreamPcm::ConverterType *s_convert_in_8 = s_convert_out_8;

#define CONVERTER 0
#define CONVERTER_SIGN 1
#define CONVERTER_SIGN_SWAP 2
#define CONVERTER_SWAP_SIGN_SWAP 3
#define CONVERTER_SWAP_SIGN 4
#define CONVERTER_SWAP 5

wxSoundStream& wxSoundStreamPcm::Read(void *buffer, wxUint32 len)
{
  wxUint32 real_len;
  char *tmp_buf;

  if (!m_function_in) {
    m_sndio->Read(buffer, len);
    m_lastcount = m_sndio->GetLastAccess();
    m_snderror = m_sndio->GetError();
    return *this;
  }

  real_len = (m_16_to_8) ? len / 2 : len;

  tmp_buf = new char[real_len];

  m_sndio->Read(tmp_buf, real_len);
  m_lastcount = m_sndio->GetLastAccess();
  m_snderror = m_sndio->GetError();
  if (m_snderror != wxSOUND_NOERR)
    return *this;

  m_function_in(tmp_buf, (char *)buffer, m_lastcount);

  delete[] tmp_buf;

  if (m_16_to_8)
    m_lastcount *= 2;

  return *this;
}

wxSoundStream& wxSoundStreamPcm::Write(const void *buffer, wxUint32 len)
{
  char *tmp_buf;
  wxUint32 len2;

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
  ConverterType *current_table_out, *current_table_in;
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
  if (pcm_format->GetBPS() != pcm_format2->GetBPS()) {
    m_16_to_8 = TRUE;
    current_table_out = s_convert_out_16_to_8;
    current_table_in  = s_convert_in_8_to_16;
  } else if (pcm_format->GetBPS() == 16) {
    current_table_out = s_convert_out_16;
    current_table_in  = s_convert_in_16;
  } else {
    current_table_out = s_convert_out_8;
    current_table_in  = s_convert_in_8;
  }

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

  m_function_out = current_table_out[index];
  m_function_in  = current_table_in[index];

  m_sndio->SetSoundFormat(*new_format);
  m_sndformat = new_format;
  return TRUE;
}
