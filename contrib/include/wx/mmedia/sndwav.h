// --------------------------------------------------------------------------
// Name: sndwav.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDWAV_H
#define _WX_SNDWAV_H

#ifdef __GNUG__
#pragma interface "sndwav.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/stream.h"
#include "wx/datstrm.h"
#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndfile.h"

//
// WAVE codec
//

class wxSoundWave: public wxSoundFileStream {
public:
    wxSoundWave(wxInputStream& stream, wxSoundStream& io_sound);
    wxSoundWave(wxOutputStream& stream, wxSoundStream& io_sound);
    ~wxSoundWave();
    
    bool CanRead();
    wxString GetCodecName() const;
    
protected:
    bool PrepareToPlay(); 
    bool PrepareToRecord(wxUint32 time);
    bool FinishRecording();
    bool RepositionStream(wxUint32 position);
    
    wxUint32 GetData(void *buffer, wxUint32 len);
    wxUint32 PutData(const void *buffer, wxUint32 len);
    
    bool HandleOutputPCM(wxDataInputStream& data, wxUint32 len,
                         wxUint16 channels, wxUint32 sample_fq,
                         wxUint32 byte_p_sec, wxUint16 byte_p_spl,
                         wxUint16 bits_p_spl);
    bool HandleOutputMSADPCM(wxDataInputStream& data, wxUint32 len,
                             wxUint16 channels, wxUint32 sample_fq,
                             wxUint32 byte_p_sec, wxUint16 byte_p_spl,
                             wxUint16 bits_p_spl);
    bool HandleOutputG721(wxDataInputStream& data, wxUint32 len,
                          wxUint16 channels, wxUint32 sample_fq,
                          wxUint32 byte_p_sec, wxUint16 byte_p_spl,
                          wxUint16 bits_p_spl);
    wxSoundFormatBase *HandleInputPCM(wxDataOutputStream& data);
    wxSoundFormatBase *HandleInputG72X(wxDataOutputStream& data);

protected:
    off_t m_base_offset;
};

#endif
