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

#if WXWIN_COMPATIBILITY_2_4
    #warning "wx/wave.h header is deprecated, use wx/sound.h and wxSound"
    #include "wx/sound.h"
#else
    #error "wx/wave.h is only available in compatibility mode"
#endif

#endif
