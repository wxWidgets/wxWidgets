// --------------------------------------------------------------------------
// Name: sndulaw.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDULAW_H
#define _WX_SNDULAW_H

#ifdef __GNUG__
#pragma interface "sndulaw.h"
#endif

#include "wx/defs.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndbase.h"

//
// ULAW format
//
class WXDLLEXPORT wxSoundFormatUlaw: public wxSoundFormatBase {
public:
    wxSoundFormatUlaw();
    ~wxSoundFormatUlaw();
    
    void SetSampleRate(wxUint32 srate);
    wxUint32 GetSampleRate() const;
    
    void SetChannels(wxUint8 channels);
    wxUint8 GetChannels() const;
    
    wxSoundFormatType GetType() const { return wxSOUND_ULAW; }
    wxSoundFormatBase *Clone() const;
    
    wxUint32 GetTimeFromBytes(wxUint32 bytes) const;
    wxUint32 GetBytesFromTime(wxUint32 time) const;
    
    bool operator !=(const wxSoundFormatBase& frmt2) const;
    
protected:
    wxUint32 m_srate;
    wxUint8 m_channels;
};

//
// ULAW converter class
//

class WXDLLEXPORT wxSoundRouterStream;
class WXDLLEXPORT wxSoundStreamUlaw: public wxSoundStreamCodec {
public:
    wxSoundStreamUlaw(wxSoundStream& sndio);
    ~wxSoundStreamUlaw();
    
    wxSoundStream& Read(void *buffer, wxUint32 len);
    wxSoundStream& Write(const void *buffer, wxUint32 len);
    
    bool SetSoundFormat(const wxSoundFormatBase& format);
    
    wxUint32 GetBestSize() const;
    
protected:
    wxSoundRouterStream *m_router;
};

#endif
