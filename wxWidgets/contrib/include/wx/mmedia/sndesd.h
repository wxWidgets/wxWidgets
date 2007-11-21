// --------------------------------------------------------------------------
// Name: sndesd.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// License:    wxWindows license
// --------------------------------------------------------------------------
#ifndef _WX_SNDESD_H
#define _WX_SNDESD_H

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/mmedia/defs.h"
#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndpcm.h"

//
// ESD output class
//

class WXDLLIMPEXP_MMEDIA wxSoundStreamESD : public wxSoundStream {
public:
    wxSoundStreamESD(const wxString& hostname = wxT("localhost"));
    ~wxSoundStreamESD();
    
    wxSoundStream& Read(void *buffer, wxUint32 len);
    wxSoundStream& Write(const void *buffer, wxUint32 len);
    
    bool SetSoundFormat(const wxSoundFormatBase& format);
    
    bool StartProduction(int evt);
    bool StopProduction();
    
    // You should not call this.
    void WakeUpEvt(int evt);

    bool QueueFilled() const { return m_q_filled; }
protected:
    int m_fd_input, m_fd_output;      // ESD fds
    int m_tag_input, m_tag_output;    // GLIB tags
    bool m_esd_stop;                  // Stream started ?
    bool m_esd_ok;                    // ESD detected
    wxString m_hostname;              // ESD host
    bool m_q_filled;                  // queue filled

private:
    void DetectBest(wxSoundFormatPcm *pcm);
};

#endif
