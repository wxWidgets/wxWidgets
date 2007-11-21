/////////////////////////////////////////////////////////////////////////////
// Name:        mmboard.h
// Purpose:     Multimedia Board header
// Author:      Guilhem Lavaux, <guilhem.lavaux@libertysurf.fr>
// Modified by:
// Created:     13/02/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MMBOARD_APP_H_
#define _MMBOARD_APP_H_

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

#define MM_SOUND_OSS 0x01
#define MM_SOUND_ESD 0x02
#define MM_SOUND_WIN 0x04

// --------------------------------------------------------------------------
// Class definitions
// --------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MMBoardApp : public wxApp
{
public:
  wxUint8 m_caps;

  // override base class virtuals
  // ----------------------------

  virtual bool OnInit();

  wxUint8 TestMultimediaCaps();
};

#endif
