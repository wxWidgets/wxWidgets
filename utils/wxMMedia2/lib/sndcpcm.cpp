// --------------------------------------------------------------------------
// Name: sndcpcm.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999, 2000
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


#include "converter.def"

// -----------------------------------------------------------------------
// Main PCM stream converter table
// -----------------------------------------------------------------------
wxSoundStreamPcm::ConverterType s_converters[] = {
  NULL,
  Convert_8_8_sign,			/* 8 -> 8 sign */
  NULL,
  NULL,
  NULL,
  NULL,

  Convert_8_16,				/* 8 -> 16 */
  Convert_8_16_sign,			/* 8 -> 16 sign */
  Convert_8_16_swap,			/* 8 -> 16 swapped */
  Convert_8_16_sign_swap,		/* 8 -> 16 sign swapped */
  NULL,
  NULL,

  Convert_16_8,				/* 16 -> 8 */
  Convert_16_8_sign,			/* 16 -> 8 sign */
  Convert_16_swap_8,			/* 16 swapped -> 8 */
  Convert_16_swap_8_sign,		/* 16 swapped -> 8 sign */
  NULL,
  NULL,

  NULL,					/* 16 -> 16 */
  Convert_16_sign,			/* 16 -> 16 sign */
  Convert_16_swap,			/* 16 swapped -> 16 */
  Convert_16_swap_16_sign,		/* 16 swapped -> 16 sign */
  Convert_16_sign_16_swap,		/* 16 sign -> 16 swapped */
  Convert_16_swap_16_sign_swap		/* 16 swapped -> 16 sign swapped */
};

#define CONVERT_BPS 0
#define CONVERT_SIGN 1
#define CONVERT_SWAP 2
#define CONVERT_SIGN_SWAP 3
#define CONVERT_SWAP_SIGN 4
#define CONVERT_SWAP_SIGN_SWAP 5

#define CONVERT_BASE_8_8 0
#define CONVERT_BASE_8_16 6
#define CONVERT_BASE_16_8 12 
#define CONVERT_BASE_16_16 18

//
// TODO: Read() and Write() aren't really safe. If you give it a buffer which
// is not aligned on 2, you may crash (See converter.def).
//

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
  m_function_out((const char *)buffer, tmp_buf, len2);
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

  // ----------------------------------------------------
  // Select table to use:
  //     * 8 bits -> 8 bits
  //     * 16 bits -> 8 bits
  //     * 8 bits -> 16 bits
  //     * 16 bits -> 16 bits

  m_16_to_8 = FALSE;
  if (pcm_format->GetBPS() != pcm_format2->GetBPS()) {
    m_16_to_8 = TRUE;
    if (pcm_format2->GetBPS() == 8) {
      current_table_out = &s_converters[CONVERT_BASE_16_8];
      current_table_in  = &s_converters[CONVERT_BASE_8_16];
    } else {
      current_table_out = &s_converters[CONVERT_BASE_8_16];
      current_table_in  = &s_converters[CONVERT_BASE_16_8];
    }
  } else if (pcm_format->GetBPS() == 16) {
    current_table_out = &s_converters[CONVERT_BASE_16_16];
    current_table_in  = &s_converters[CONVERT_BASE_16_16];
  } else {
    current_table_out = &s_converters[CONVERT_BASE_8_8];
    current_table_in  = &s_converters[CONVERT_BASE_8_8];
  }

  change_sign = (pcm_format2->Signed() != pcm_format->Signed());

#define MY_ORDER wxBYTE_ORDER
#if wxBYTE_ORDER == wxLITTLE_ENDIAN
#define OTHER_ORDER wxBIG_ENDIAN
#else
#define OTHER_ORDER wxLITTLE_ENDIAN
#endif

  // --------------------------------------------------------
  // Find the good converter !


  if (pcm_format->GetOrder() == OTHER_ORDER &&
      pcm_format2->GetOrder() == OTHER_ORDER && change_sign) 
    index = CONVERT_SWAP_SIGN_SWAP;

  else if (pcm_format->GetOrder() == OTHER_ORDER &&
           pcm_format2->GetOrder() == MY_ORDER && change_sign) 
    index = CONVERT_SWAP_SIGN;

  else if (pcm_format->GetOrder() == MY_ORDER &&
           pcm_format->GetOrder() == OTHER_ORDER && change_sign)
    index = CONVERT_SIGN_SWAP;

  else if (change_sign)
    index = CONVERT_SIGN;

  else if (!change_sign &&
           pcm_format->GetOrder() != pcm_format2->GetOrder())
    index = CONVERT_SWAP;

  else
    index = CONVERT_BPS;

  m_function_out = current_table_out[index];
  m_function_in  = current_table_in[index];

  m_sndio->SetSoundFormat(*new_format);
  m_sndformat = new_format;
  return TRUE;
}
