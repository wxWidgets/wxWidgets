/////////////////////////////////////////////////////////////////////////////
// Name:        sound.cpp
// Purpose:     wxSound class implementation: optional
// Author:      Ryan Norton
// Modified by: 
// Created:     2004-10-02
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "sound.h"
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/sound.h"

#if wxUSE_SOUND

#include "wx/app.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/AppKit.h>

//
// NB:  Vaclav's new wxSound API is really tricky -
// Basically, we need to make sure that if the wxSound
// object is still in scope we don't release it's NSSound
//

WX_NSSound lastSound=NULL;
bool isLastSoundLooping = false;
bool isLastSoundInScope = false;

// ========================================================================
// wxNSSoundDelegate
// ========================================================================
@interface wxNSSoundDelegate : NSObject
{
}

// Delegate methods
- (void)sound:(NSSound *)theSound didFinishPlaying:(BOOL)bOK;
@end // interface wxNSSoundDelegate : NSObject

@implementation wxNSSoundDelegate : NSObject

- (void)sound:(NSSound *)theSound didFinishPlaying:(BOOL)bOK
{
    if (bOK && isLastSoundLooping)
        [lastSound play];
    else if (!isLastSoundInScope)
    {
        [lastSound release];
        [self release];
    }
}

@end // wxNSSoundDelegate

// ------------------------------------------------------------------
//          wxSound
// ------------------------------------------------------------------

wxSound::wxSound()
:   m_hSnd(NULL)
,   m_waveLength(0)
{
}

wxSound::wxSound(const wxString& sFileName, bool isResource)
:   m_hSnd(NULL)
,   m_waveLength(0)
{
    Create(sFileName, isResource);
}

wxSound::wxSound(int size, const wxByte* data)
:   m_hSnd(NULL)
,   m_waveLength(size)
{
    NSData* theData = [[NSData alloc] dataWithBytesNoCopy:(void*)data length:size];
    m_hSnd = [[NSSound alloc] initWithData:theData];

    m_cocoaSoundDelegate = [[wxNSSoundDelegate alloc] init];
}

wxSound::~wxSound()
{
    if (m_hSnd != lastSound)
    {
        [m_hSnd release];
        [m_cocoaSoundDelegate release];
    }
    else
        isLastSoundInScope = false;
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
    wxAutoNSAutoreleasePool thePool;

    Stop();

    if (isResource)
    {
        //oftype could be @"snd" @"wav" or @"aiff"; nil or @"" autodetects (?)
        m_hSnd = [[NSSound alloc]
            initWithContentsOfFile:[[NSBundle mainBundle]
                    pathForResource:wxNSStringWithWxString(fileName)
                    ofType:nil]
            byReference:YES];
    }
    else
            m_hSnd = [[NSSound alloc] initWithContentsOfFile:wxNSStringWithWxString(fileName) byReference:YES];

    m_cocoaSoundDelegate = [[wxNSSoundDelegate alloc] init];

    m_sndname = fileName;
    return m_hSnd != nil;
}

bool wxSound::DoPlay(unsigned flags) const
{
    wxASSERT_MSG(!( (flags & wxSOUND_SYNC) && (flags & wxSOUND_LOOP)),
                wxT("Invalid flag combination passed to wxSound::Play"));

    Stop();

    if (flags & wxSOUND_ASYNC)
    {
        lastSound = m_hSnd;
        isLastSoundLooping = (flags & wxSOUND_LOOP) == wxSOUND_LOOP;
        isLastSoundInScope = true;
        [m_hSnd setDelegate:m_cocoaSoundDelegate];
        return [m_hSnd play];
    }
    else
    {
        [m_hSnd setDelegate:nil];

        //play until done
        bool bOK = [m_hSnd play];

        while ([m_hSnd isPlaying])
        {
            wxTheApp->Yield(false);
        }
        return bOK;
    }
}

bool wxSound::IsPlaying()
{
    return [lastSound isPlaying];
}

void wxSound::Stop()
{
    if (isLastSoundInScope)
    {
        isLastSoundInScope = false;

        //remember that even though we're
        //programatically stopping it, the
        //delegate will still be called -
        //so it will free the memory here
        [((NSSound*&)lastSound) stop];
    }

    lastSound = nil;
}

#endif //wxUSE_SOUND
