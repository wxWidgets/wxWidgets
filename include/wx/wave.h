/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wave.h
// Purpose:     wxSound compatibility header
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/02/01
// RCS-ID:      $Id$
// Copyright:   (c) 2004, Vaclav Slavik
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WAVE_H_BASE_
#define _WX_WAVE_H_BASE_

#include "wx/setup.h"

#if wxUSE_SOUND

#if WXWIN_COMPATIBILITY_2_4
#ifdef __DMC__
    #pragma message "wx/wave.h header is deprecated, use wx/sound.h and wxSound"
#else
    #warning "wx/wave.h header is deprecated, use wx/sound.h and wxSound"
#endif
    #include "wx/sound.h"
    // wxSound used to be called wxWave before wxWindows 2.5.1:
    typedef wxSound wxWave;
#else
    #error "wx/wave.h is only available in compatibility mode"
#endif

#endif

#endif
