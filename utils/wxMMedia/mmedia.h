// /////////////////////////////////////////////////////////////////////////////
// Name:       mmedia.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __MMEDIA_H__
#define __MMEDIA_H__

#ifdef __WINDOWS__

#include "sndwin.h"
#define wxSoundDevice wxWinSound

#include "vidwin.h"
#define wxVideoDevice wxVideoWindows

#else

#include "snduss.h"
#define wxSoundDevice wxUssSound

#include "vidxanm.h"
#define wxVideoDevice wxVideoXANIM

#endif

#endif
