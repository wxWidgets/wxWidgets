/////////////////////////////////////////////////////////////////////////////
// Name:        wave.h
// Purpose:     wxWave class
// Author:      Julian Smart
// Modified by: Guilhem Lavaux for wxMMedia (02/05/1998)
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WAVEH__
#define __WAVEH__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/object.h>
#include <wx/string.h>
#include "sndwav.h"

class wxWave : public wxObject
{
public:
  wxWave();
  wxWave(const wxString& fileName, bool isResource = FALSE);
  ~wxWave();

public:
  bool  Create(const wxString& sFileName, bool isResource = FALSE);
  bool  IsOk() const { return (m_wave ? TRUE : FALSE); };
  bool  Play(bool async = TRUE, bool looped = FALSE) const;

protected:
  bool  Free();

protected:
  wxInputStream *m_iowave;
  wxSndWavCodec *m_wave;
};

#endif
