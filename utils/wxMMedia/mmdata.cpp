////////////////////////////////////////////////////////////////////////////////
// Name:       mmdata.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#include "mmfile.h"

#include "sndsnd.h"
#include "sndfrmt.h"
#if defined(__UNIX__)
#include "snduss.h"
#endif
#include "sndfrag.h"
#include "sndfile.h"
#include "sndwav.h"
#include "sndaiff.h"
#include "sndau.h"
#include "sndpcm.h"
#include "sndmulaw.h"
#include "vidbase.h"
#if defined(__X__) || defined(__WXGTK__)
#include "vidxanm.h"
#endif
#ifdef __WINDOWS__
#include "sndwin.h"
#include "cdwin.h"
#include "vidwin.h"
#endif
#include "cdbase.h"
#ifdef __UNIX__
#include "cdunix.h"
#endif
#include "mmsolve.h"
// #include "midfile.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

IMPLEMENT_ABSTRACT_CLASS(wxSound, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxSndBuffer, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSndSimpleBuffer, wxSndBuffer)

IMPLEMENT_ABSTRACT_CLASS(wxSoundCodec, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSoundPcmCodec, wxSoundCodec)
IMPLEMENT_DYNAMIC_CLASS(wxSoundMulawCodec, wxSoundCodec)

#ifdef __UNIX__
IMPLEMENT_DYNAMIC_CLASS(wxUssSound, wxSound)
#endif
#ifdef __WINDOWS__
IMPLEMENT_DYNAMIC_CLASS(wxWinSound, wxSound)
#endif

IMPLEMENT_ABSTRACT_CLASS(wxSndFileCodec, wxMMediaFile)
IMPLEMENT_DYNAMIC_CLASS(wxSndWavCodec, wxSndFileCodec)
IMPLEMENT_DYNAMIC_CLASS(wxSndAuCodec, wxSndFileCodec)
IMPLEMENT_DYNAMIC_CLASS(wxSndAiffCodec, wxSndFileCodec)

IMPLEMENT_ABSTRACT_CLASS(wxVideoBaseDriver, wxMMediaFile)
IMPLEMENT_DYNAMIC_CLASS(wxVideoOutput, wxWindow)
#if defined(__X__) || defined(__WXGTK__)
// IMPLEMENT_DYNAMIC_CLASS(wxVideoXANIM, wxVideoBaseDriver)
#endif
#ifdef __WINDOWS__
IMPLEMENT_DYNAMIC_CLASS(wxVideoWindows, wxVideoBaseDriver)
#endif

IMPLEMENT_ABSTRACT_CLASS(wxCDAudio, wxObject)
#ifdef linux
IMPLEMENT_DYNAMIC_CLASS(wxCDAudioLinux, wxCDAudio)
#else
IMPLEMENT_DYNAMIC_CLASS(wxCDAudioWin, wxCDAudio)
#endif

// IMPLEMENT_ABSTRACT_CLASS(wxMidiFile, wxObject)

wxMediaFileSolve::wxMFileList *wxMediaFileSolve::m_first = NULL;
wxUint8 wxMediaFileSolve::m_devnum = 0;

MMD_REGISTER_FILE("audio/x-wav", "Wav Player", wxSndWavCodec, "wav")
MMD_REGISTER_FILE("audio/x-aiff", "Aiff Player", wxSndAiffCodec, "aif")
MMD_REGISTER_FILE("audio/x-au", "Sun Audio File Player", wxSndAuCodec, "au")
#if defined(__X__) || defined(__WXGTK__)
// MMD_REGISTER_FILE("video/*", "Video Player", wxVideoXANIM, "mov")
#else
MMD_REGISTER_FILE("video/avi", "AVI Player", wxVideoWindows, "avi")
#endif
