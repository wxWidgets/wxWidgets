// --------------------------------------------------------------------------
// Name: sndaiff.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// wxWindows licence
// --------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/defs.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stream.h"
#include "wx/datstrm.h"
#include "wx/filefn.h"

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndfile.h"
#include "wx/mmedia/sndpcm.h"
#include "wx/mmedia/sndaiff.h"

#define BUILD_SIGNATURE(a,b,c,d) (((wxUint32)a) | (((wxUint32)b) << 8) | (((wxUint32)c) << 16)  | (((wxUint32)d) << 24))

#define FORM_SIGNATURE BUILD_SIGNATURE('F','O','R','M')
#define AIFF_SIGNATURE BUILD_SIGNATURE('A','I','F','F')
#define AIFC_SIGNATURE BUILD_SIGNATURE('A','I','F','C')
#define COMM_SIGNATURE BUILD_SIGNATURE('C','O','M','M')
#define SSND_SIGNATURE BUILD_SIGNATURE('S','S','N','D')

wxSoundAiff::wxSoundAiff(wxInputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
    m_base_offset = wxInvalidOffset;
}

wxSoundAiff::wxSoundAiff(wxOutputStream& stream, wxSoundStream& io_sound)
  : wxSoundFileStream(stream, io_sound)
{
    m_base_offset = wxInvalidOffset;
}

wxSoundAiff::~wxSoundAiff()
{
}

wxString wxSoundAiff::GetCodecName() const
{
    return wxT("wxSoundAiff codec");
}

bool wxSoundAiff::CanRead()
{
    wxUint32 signature1, signature2, len;
    
    if (m_input->Read(&signature1, 4).LastRead() != 4)
        return false;
    
    if (wxUINT32_SWAP_ON_BE(signature1) != FORM_SIGNATURE) {
        m_input->Ungetch(&signature1, 4);
        return false;
    }
    
    m_input->Read(&len, 4);
    if (m_input->LastRead() != 4) {
        m_input->Ungetch(&len, m_input->LastRead());
        m_input->Ungetch(&signature1, 4);
        return false;
    }
    
    if (m_input->Read(&signature2, 4).LastRead() != 4) {
        m_input->Ungetch(&signature2, m_input->LastRead());
        m_input->Ungetch(&len, 4);
        m_input->Ungetch(&signature1, 4);
        return false;
    }
    
    m_input->Ungetch(&signature2, 4);
    m_input->Ungetch(&len, 4);
    m_input->Ungetch(&signature1, 4);
    
    if (
        wxUINT32_SWAP_ON_BE(signature2) != AIFF_SIGNATURE &&
        wxUINT32_SWAP_ON_BE(signature2) != AIFC_SIGNATURE)
        return false;
    
    return true;
}

#define FAIL_WITH(condition, err) if (condition) { m_snderror = err; return false; }

bool wxSoundAiff::PrepareToPlay()
{
    wxDataInputStream data(*m_input);
    wxUint32 signature, len, ssnd;
    bool end_headers;
    
    if (!m_input) {
        m_snderror = wxSOUND_INVSTRM;
        return false;
    }
    m_snderror = wxSOUND_NOERROR;
    
    data.BigEndianOrdered(true);
    
    FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
    FAIL_WITH(wxUINT32_SWAP_ON_BE(signature) != FORM_SIGNATURE, wxSOUND_INVSTRM);
    // "FORM"
    
    len = data.Read32(); 
    wxUnusedVar(len);
    FAIL_WITH(m_input->LastRead() != 4, wxSOUND_INVSTRM);
    // dummy len
    
    FAIL_WITH(m_input->Read(&signature, 4).LastRead() != 4, wxSOUND_INVSTRM);
    FAIL_WITH(
        wxUINT32_SWAP_ON_BE(signature) != AIFF_SIGNATURE &&
        wxUINT32_SWAP_ON_BE(signature) != AIFC_SIGNATURE, wxSOUND_INVSTRM);
    // "AIFF" / "AIFC"
    
    end_headers = false;
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

                // Get sound data informations
                data >> channels >> num_samples >> bps >> srate; 

                // Convert them in a wxSoundFormat object
                sndformat.SetSampleRate((wxUint32) srate);
                sndformat.SetBPS(bps);
                sndformat.SetChannels(channels);
                sndformat.Signed(false);
                sndformat.SetOrder(wxBIG_ENDIAN);
                
                if (!SetSoundFormat(sndformat))
                    return false;
                // We pass all data left
                m_input->SeekI(len-18, wxFromCurrent);
                break;
            }
            case SSND_SIGNATURE: {  // "SSND"
                data >> ssnd;
                // m_input->SeekI(4, wxFromCurrent);  // Pass an INT32
                // m_input->SeekI(len-4, wxFromCurrent); // Pass the rest
                m_input->SeekI(ssnd + 4, wxFromCurrent);
                m_base_offset = m_input->TellI();
                // len-8 bytes of samples
                FinishPreparation(len - 8);
                end_headers = true;
                break;
            }
            default:
                m_input->SeekI(len, wxFromCurrent);
                break;
        }
    }
    return true;
}

bool wxSoundAiff::PrepareToRecord(wxUint32 WXUNUSED(time))
{
    // TODO
    return false;
}

bool wxSoundAiff::FinishRecording()
{
    // TODO
    return false;
}

bool wxSoundAiff::RepositionStream(wxUint32 WXUNUSED(position))
{
    // If the stream is not seekable "TellI() returns wxInvalidOffset" we cannot reposition stream
    if (m_base_offset == wxInvalidOffset)
        return false;
    m_input->SeekI(m_base_offset, wxFromStart);
    return true;
}

wxUint32 wxSoundAiff::GetData(void *buffer, wxUint32 len)
{
    return m_input->Read(buffer, len).LastRead();
}

wxUint32 wxSoundAiff::PutData(const void *buffer, wxUint32 len)
{
    return m_output->Write(buffer, len).LastWrite();
}
