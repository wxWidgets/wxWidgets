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

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/debug.h"
    #include "wx/log.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndpcm.h"
#include "wx/mmedia/sndcpcm.h"

wxSoundStreamPcm::wxSoundStreamPcm(wxSoundStream& sndio)
        : wxSoundStreamCodec(sndio)
{
    m_function_in = NULL;
    m_function_out = NULL;
    m_prebuffer = NULL;
    m_prebuffer_size = 0;
    m_best_size = 0;
}

wxSoundStreamPcm::~wxSoundStreamPcm()
{
    if (m_prebuffer)
        delete[] m_prebuffer;
}

wxUint32 wxSoundStreamPcm::GetBestSize() const
{
    return m_best_size;
}

// -----------------------------------------------------------------------
// "Converters" definitions/implementations
// -----------------------------------------------------------------------

#define DEFINE_CONV(name, input_type, output_type, convert) \
static void Convert_##name##(const void *buf_in, void *buf_out, wxUint32 len) \
{\
  register input_type src; \
  register const input_type *t_buf_in = (input_type *)buf_in; \
  register output_type *t_buf_out = (output_type *)buf_out; \
\
  while (len > 0) { \
    src = *t_buf_in++; \
    *t_buf_out++ = convert; \
    len -= sizeof(input_type); \
  } \
}

// TODO: define converters for all other formats (32, 24)

DEFINE_CONV(8_8_sign, wxUint8, wxUint8, (src ^ 0x80))

DEFINE_CONV(8_16, wxUint8, wxUint16, (((wxUint16)src) << 8))
DEFINE_CONV(8_16_swap, wxUint8, wxUint16, (src))
DEFINE_CONV(8_16_sign, wxUint8, wxUint16, (((wxUint16)(src ^ 0x80)) << 8))
DEFINE_CONV(8_16_sign_swap, wxUint8, wxUint16, (src ^ 0x80))

DEFINE_CONV(16_8, wxUint16, wxUint8, (wxUint8)(src >> 8))
DEFINE_CONV(16_8_sign, wxUint16, wxUint8, (wxUint8)((src >> 8) ^ 0x80))
DEFINE_CONV(16_swap_8, wxUint16, wxUint8, (wxUint8)(src & 0xff))
DEFINE_CONV(16_swap_8_sign, wxUint16, wxUint8, (wxUint8)((src & 0xff) ^ 0x80))

//DEFINE_CONV(24_8, wxUint32, wxUint8, (wxUint8)(src >> 16))
//DEFINE_CONV(24_8_sig, wxUint32, wxUint8, (wxUint8)((src >> 16) ^ 0x80))

//DEFINE_CONV(32_8, wxUint32, wxUint8, (wxUint8)(src >> 24))

DEFINE_CONV(16_sign, wxUint16, wxUint16, (src ^ 0x8000))
DEFINE_CONV(16_swap, wxUint16, wxUint16, (((src & 0xff) << 8) | ((src >> 8) & 0xff)))
// Problem.
DEFINE_CONV(16_swap_16_sign, wxUint16, wxUint16, ((((src & 0xff) << 8) | ((src >> 8) & 0xff)) ^ 0x80))
// DEFINE_CONV(16_sign_16_swap, wxUint16, wxUint16, ((((src & 0xff) << 8) | ((src >> 8) & 0xff)) ^ 0x8000))
DEFINE_CONV(16_swap_16_sign_swap, wxUint16, wxUint16, (src ^ 0x80))

// -----------------------------------------------------------------------
// Main PCM stream converter table
// -----------------------------------------------------------------------
// Definition
//   XX -> YY
//   XX -> YY sign
//
//   XX swapped -> YY
//   XX swapped -> YY sign
//
//   XX swapped -> YY swapped
//   XX swapped -> YY swapped sign
//
//   XX stereo -> YY mono
//   XX stereo -> YY mono sign
//
//   XX swapped stereo -> YY swapped mono
//   XX swapped stereo -> YY swapped mono sign
//
//   XX swapped stereo -> YY swapped mono
//   XX swapped stereo -> YY swapped mono sign

static wxSoundStreamPcm::ConverterType s_converters[4][3][2] = { 
    {
        {
            NULL,
            Convert_8_8_sign                    /* 8 -> 8 sign */
        },
        {
            NULL,
            NULL
        },
        {
            NULL,
            NULL
        }
    },
    {
        {
            Convert_8_16,                       /* 8 -> 16 */
            Convert_8_16_sign                   /* 8 -> 16 sign */
        },
        {
            Convert_8_16_swap,                  /* 8 -> 16 swapped */
            Convert_8_16_sign_swap              /* 8 -> 16 sign swapped */
        },
        {
            NULL,
            NULL
        }
    },
    {
        {
            Convert_16_8,                       /* 16 -> 8 */
            Convert_16_8_sign                   /* 16 -> 8 sign */
        },
        {
            Convert_16_swap_8,                  /* 16 swapped -> 8 */
            Convert_16_swap_8_sign              /* 16 swapped -> 8 sign */
        },
        {
            NULL,
            NULL 
        },
    },
    
    {
        {
            NULL,                               /* 16 -> 16 */
            Convert_16_sign                     /* 16 -> 16 sign */
        },
        {
            Convert_16_swap,                    /* 16 swapped -> 16 */
            Convert_16_swap_16_sign             /* 16 swapped -> 16 sign */
        },
        {
            NULL,
            Convert_16_swap_16_sign_swap        /* 16 swapped -> 16 sign swapped */
        }
    }
};

// This is the buffer size multiplier. It gives the needed size of the output size.
static float s_converters_multip[] = {1, 2, 0.5, 1};

//
// TODO: Read() and Write() aren't really safe. If you give it a buffer which
// is not aligned on 2, you may crash (See converter.def).
//

wxSoundStream& wxSoundStreamPcm::Read(void *buffer, wxUint32 len)
{
    wxUint32 in_bufsize;

    // We must have a multiple of 2
    len &= 0x01;
    
    if (!m_function_in) {
        m_sndio->Read(buffer, len);
        m_lastcount = m_sndio->GetLastAccess();
        m_snderror = m_sndio->GetError();
        return *this;
    }

    in_bufsize = GetReadSize(len);
    
    if (len <= m_best_size) {
        m_sndio->Read(m_prebuffer, in_bufsize);
        m_snderror  = m_sndio->GetError();
        if (m_snderror != wxSOUND_NOERROR) {
            m_lastcount = 0;
            return *this;
        }
        
        m_function_in(m_prebuffer, buffer, m_sndio->GetLastAccess());
    } else {
        char *temp_buffer;
        
        temp_buffer = new char[in_bufsize];
        m_sndio->Read(temp_buffer, in_bufsize);

        m_snderror =  m_sndio->GetError();
        if (m_snderror != wxSOUND_NOERROR) {
            m_lastcount = 0;
            return *this;
        }
        
        m_function_in(temp_buffer, buffer, m_sndio->GetLastAccess());
        
        delete[] temp_buffer;
    }
    
    m_lastcount = (wxUint32)(m_sndio->GetLastAccess() * m_multiplier_in);
    
    return *this;
}

wxSoundStream& wxSoundStreamPcm::Write(const void *buffer, wxUint32 len)
{
    wxUint32 out_bufsize;
    
    if (!m_function_out) {
        m_sndio->Write(buffer, len);
        m_lastcount = m_sndio->GetLastAccess();
        m_snderror  = m_sndio->GetError();
        return *this;
    }

    out_bufsize = GetWriteSize(len);

    if (len <= m_best_size) {
        out_bufsize = GetWriteSize(len);

        m_function_out(buffer, m_prebuffer, len);
        m_sndio->Write(m_prebuffer, out_bufsize);
        m_snderror  = m_sndio->GetError();
        if (m_snderror != wxSOUND_NOERROR) {
            m_lastcount = 0;
            return *this;
        }
    } else {
        char *temp_buffer;
        
        temp_buffer = new char[out_bufsize];
        m_function_out(buffer, temp_buffer, len);
        
        m_sndio->Write(temp_buffer, out_bufsize);
        m_snderror =  m_sndio->GetError();
        if (m_snderror != wxSOUND_NOERROR) {
            m_lastcount = 0;
            return *this;
        }
        
        delete[] temp_buffer;
    }

    m_lastcount = (wxUint32)(m_sndio->GetLastAccess() / m_multiplier_out);

    return *this;
}

bool wxSoundStreamPcm::SetSoundFormat(const wxSoundFormatBase& format)
{
    wxSoundFormatBase *new_format;
    wxSoundFormatPcm *pcm_format, *pcm_format2;
    
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

#if 0
    // ----------------------------------------------------
    // Test whether we need to resample
    if (pcm_format->GetSampleRate() != pcm_format2->GetSampleRate()) {
        wxUint32 src_rate, dst_rate;

        src_rate = pcm_format->GetSampleRate();
        dst_rate = pcm_format2->GetSampleRate();
        m_needResampling = TRUE;
        if (src_rate < dst_rate)
            m_expandSamples = TRUE;
        else
            m_expandSamples = FALSE;
        m_pitch = (src_rate << FLOATBITS) / dst_rate;
    }
#endif
    // ----------------------------------------------------
    // Select table to use:
    //     * 8 bits -> 8 bits
    //     * 16 bits -> 8 bits
    //     * 8 bits -> 16 bits
    //     * 16 bits -> 16 bits

    int table_no, table_no2;
    int i_sign, i_swap;
    
    switch (pcm_format->GetBPS()) {
        case 8:
            table_no = 0;
            break;
        case 16:
            table_no = 1;
            break;
        default:
            // TODO: Add something here: error, log, ...
            return FALSE;
    }
    switch (pcm_format2->GetBPS()) {
        case 8:
            table_no2 = 0;
            break;
        case 16:
            table_no2 = 1;
            break;
        default:
            // TODO: Add something here: error, log, ...
            return FALSE;
    }
    
    if (pcm_format2->Signed() != pcm_format->Signed())
        i_sign = 1;
    else
        i_sign = 0;

#define MY_ORDER wxBYTE_ORDER
#if wxBYTE_ORDER == wxLITTLE_ENDIAN
#define OTHER_ORDER wxBIG_ENDIAN
#else
#define OTHER_ORDER wxLITTLE_ENDIAN
#endif

    // --------------------------------------------------------
    // Find the good converter !

    if (pcm_format->GetOrder() == OTHER_ORDER) {
        if (pcm_format->GetOrder() == pcm_format2->GetOrder())
            i_swap = 2;
        else
            i_swap = 1;
    } else {
        if (pcm_format->GetOrder() == pcm_format2->GetOrder())
            i_swap = 0;
        else
            i_swap = 1;
    }

    m_function_out = s_converters[table_no*2+table_no2][i_swap][i_sign];
    m_function_in  = s_converters[table_no2*2+table_no][i_swap][i_sign];
    m_multiplier_out = s_converters_multip[table_no*2+table_no2];
    m_multiplier_in  = s_converters_multip[table_no2*2+table_no2];

    if (m_prebuffer)
        delete[] m_prebuffer;

    // We try to minimize the need of dynamic memory allocation by preallocating a buffer. But
    // to be sure it will be efficient we minimize the best size.
    if (m_multiplier_in < m_multiplier_out) {
        m_prebuffer_size = (wxUint32)(m_sndio->GetBestSize() * m_multiplier_out);
        m_best_size = (wxUint32)(m_sndio->GetBestSize() * m_multiplier_in);
    } else {
        m_prebuffer_size = (wxUint32)(m_sndio->GetBestSize() * m_multiplier_in);
        m_best_size = (wxUint32)(m_sndio->GetBestSize() * m_multiplier_out);
    }
    
    m_prebuffer = new char[m_prebuffer_size];
    
    bool SetSoundFormatReturn;

    SetSoundFormatReturn = m_sndio->SetSoundFormat(*new_format);
    wxASSERT( SetSoundFormatReturn );
    
    m_sndformat = new_format;
    return TRUE;
}

wxUint32 wxSoundStreamPcm::GetWriteSize(wxUint32 len) const
{
    // For the moment, it is simple but next time it will become more complicated
    // (Resampling)
    return (wxUint32)(len * m_multiplier_out);
}

wxUint32 wxSoundStreamPcm::GetReadSize(wxUint32 len) const
{
    return (wxUint32)(len / m_multiplier_in);
}

// Resampling engine. NOT FINISHED and NOT INCLUDED but this is a first DRAFT.

#if 0

#define FLOATBITS 16
#define INTBITS 16
#define FLOATMASK 0xffff
#define INTMASK 0xffff0000

void ResamplingShrink_##DEPTH##(const void *source, void *destination, wxUint32 len)
{
    wxUint##DEPTH## *source_data, *dest_data;
    wxUint32 pos;

    source_data = (wxUint##DEPTH## *)source;
    dest_data   = (wxUint##DEPTH## *)destination;
    
    pos = m_saved_pos;
    while (len > 0) {
        // Increment the position in the input buffer
        pos += m_pitch;
        if (pos & INTMASK) {
            pos &= FLOATMASK;
            
            *dest_data ++ = *source_data;
        }
        len--;
        source_data++;
    }
    m_saved_pos = pos;
}
#endif
