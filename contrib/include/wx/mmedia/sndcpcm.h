// --------------------------------------------------------------------------
// Name: sndcpcm.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDCPCM_H
#define _WX_SNDCPCM_H

#ifdef __GNUG__
#pragma interface "sndcpcm.h"
#endif

#include "wx/defs.h"
#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndcodec.h"

//
// PCM converter class
//

class wxSoundStreamPcm: public wxSoundStreamCodec {
public:
    typedef void (*ConverterType)(const void *buf_in, void *buf_out,
                                  wxUint32 len);
    
    wxSoundStreamPcm(wxSoundStream& sndio);
    ~wxSoundStreamPcm();
    
    wxSoundStream& Read(void *buffer, wxUint32 len);
    wxSoundStream& Write(const void *buffer, wxUint32 len);
    
    bool SetSoundFormat(const wxSoundFormatBase& format);
    
    wxUint32 GetBestSize() const;
    
protected:
    wxUint32 GetReadSize(wxUint32 len) const;
    wxUint32 GetWriteSize(wxUint32 len) const;

protected:
    ConverterType m_function_out, m_function_in;

    // Static temporary buffer
    char *m_prebuffer;
    wxUint32 m_prebuffer_size;
    // Estimated best size to fit into the static buffer
    wxUint32 m_best_size;
    // Multiplier for IO buffer size
    float m_multiplier_in, m_multiplier_out;
};

#endif
