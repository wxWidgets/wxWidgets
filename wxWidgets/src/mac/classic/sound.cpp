/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/sound.cpp
// Purpose:     wxSound class implementation: optional
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOUND

#include "wx/sound.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
#endif

#ifdef __WXMAC__
#include "wx/mac/private.h"
#ifndef __DARWIN__
#include <Sound.h>
#endif
#endif

wxSound::wxSound()
  : m_sndChan(0), m_hSnd(NULL), m_waveLength(0), m_isResource(true)
{
}

wxSound::wxSound(const wxString& sFileName, bool isResource)
  : m_sndChan(0), m_hSnd(NULL), m_waveLength(0), m_isResource(true)
{
    Create(sFileName, isResource);
}


wxSound::~wxSound()
{
    FreeData();
}

wxSound::wxSound(int size, const wxByte* data)
  : m_sndChan(0), m_hSnd(NULL), m_waveLength(0), m_isResource(false)
{
    //TODO convert data
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
    bool ret = false;
    m_sndname = fileName;
    m_isResource = isResource;

    if (m_isResource)
        ret = true;
    else
    { /*
        if (sndChan)
        {                                                                    // we're playing
            FSClose(SndRefNum);
            SndRefNum = 0;
            SndDisposeChannel(sndChan, TRUE);
            free(sndChan);
            sndChan = 0;
            KillTimer(0,timerID);
        }

        if (!lpSnd)
            return true;

        if (_access(lpSnd,0))                            // no file, no service
            return false;

        // Allocate SndChannel
        sndChan = (SndChannelPtr) malloc (sizeof(SndChannel));

        if (!sndChan)
            return false;

        sndChan->qLength = 128;

        if (noErr != SndNewChannel (&sndChan, sampledSynth, initMono | initNoInterp, 0))
        {
            free(sndChan);
            sndChan = 0;
            return false;
        }

        if (!(SndRefNum = MacOpenSndFile ((char *)lpSnd)))
        {
            SndDisposeChannel(sndChan, TRUE);
            free(sndChan);
            sndChan = 0;

            return false;
        }

        bool async = false;

        if (fdwSound & SND_ASYNC)
            async = true;

        if (SndStartFilePlay(sndChan, SndRefNum, 0, 81920, 0, 0, 0, async) != noErr)
        {
            FSClose (SndRefNum);
            SndRefNum = 0;
            SndDisposeChannel (sndChan, TRUE);
            free (sndChan);
            sndChan = 0;
            return false;
        }

        if (async)
        {  // haven't finish yet
            timerID = SetTimer(0, 0, 250, TimerCallBack);
        }
        else
        {
            FSClose (SndRefNum);
            SndRefNum = 0;
            SndDisposeChannel (sndChan, TRUE);
            free (sndChan);
            sndChan = 0;
        }*/
    }

    return ret;
}


//don't know what to do with looped, wth
bool wxSound::DoPlay(unsigned flags) const
{
    bool ret = false;

    if (m_isResource)
    {
        Str255 snd ;
        wxMacStringToPascal( m_sndname , snd ) ;
        SndListHandle hSnd;

        hSnd = (SndListHandle) GetNamedResource('snd ', snd);

        if ((hSnd != NULL) && (SndPlay((SndChannelPtr)m_sndChan, (SndListHandle) hSnd, (flags & wxSOUND_ASYNC)) == noErr))
            ret = true;
    }

    return ret;
}


bool wxSound::FreeData()
{
    bool ret = false;

    if (m_isResource)
    {
        m_sndname.Empty();
        ret = true;
    }
    else
    {
        //TODO,
    }

    return ret;
}


//code below is from an old implementation used for telinfo with MSVC crossplatform support
//technology proceeds, so it would be the wisest to drop this code, but it's left here just
//for the sake of a reference. BTW: Wave files can now be played with QT, starting from V3

/*static short MacOpenSndFile (char * path)
{
    VolumeParam vp;
    FSSpec fspec;
    Str255 name;
    char *c;

    // first, get the volume reference number for the file.  Start by
    // making a Pstring with just the volume name
    strcpy ((char *) name, path);
    if (c = strchr ((char *) name, ':'))
    {
        c++;
        *c = '\0';
    }

    c2pstr ((char *) name);
    vp.ioCompletion = 0;
    vp.ioVolIndex    = -1;
    vp.ioNamePtr    = name;
    vp.ioVRefNum    = 0;

    if (PBGetVInfo((ParamBlockRec *)&vp, 0) != noErr)
        return 0;

    // next, buld an FSSpec for the file
    strcpy ((char *) name, path);
    c2pstr ((char *) name);
    if (FSMakeFSSpec (vp.ioVRefNum, 0, name, &fspec) != noErr)
        return 0;

    short frefnum;
    // now open the file, and return it's reference number
    if (FSpOpenDF(&fspec, fsRdPerm, &frefnum) != noErr)
        return 0;

    return frefnum;
}


void TimerCallBack(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
    if(!sndChan)
    {
        KillTimer(0,timerID);
        return;
    }

    SCStatus    scstat;

    if (noErr == SndChannelStatus (sndChan, sizeof (SCStatus), &scstat)) {
        if (scstat.scChannelPaused || scstat.scChannelBusy)
            return;                            // not done yet
    }

    // either error or done.
    FSClose (SndRefNum);
    SndRefNum = 0;
    SndDisposeChannel (sndChan, TRUE);
    free (sndChan);
    sndChan = 0;
    KillTimer(0,timerID);
}*/

#endif // wxUSE_SOUND
