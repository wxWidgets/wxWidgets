/////////////////////////////////////////////////////////////////////////////
// Name:        wave.h
// Purpose:     wxWave class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WAVEH__
#define __WAVEH__

#ifdef __GNUG__
#pragma interface "wave.h"
#endif

#include <wx/object.h>

class wxWave : public wxObject
{
public:
  wxWave(void);
  wxWave(const wxString& fileName, bool isResource = FALSE);
  ~wxWave(void);

public:
  bool  Create(const wxString& sFileName, bool isResource = FALSE);
  bool  IsOk(void) const { return (m_waveData ? TRUE : FALSE); };
  bool  Play(bool async = TRUE, bool looped = FALSE) const;

protected:
  bool  Free(void);

private:
  byte* m_waveData;
  int   m_waveLength;
  bool  m_isResource;
};

#endif

