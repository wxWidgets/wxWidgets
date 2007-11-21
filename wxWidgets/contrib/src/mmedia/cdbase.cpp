// ---------------------------------------------------------------------------
// Name:       cdbase.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1999
// Copyright:  (C) 1997, 1998, 1999 Guilhem Lavaux
// License:    wxWindows license
// ---------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/defs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ---------------------------------------------------------------------------
// MMedia headers
// ---------------------------------------------------------------------------

#include "wx/mmedia/cdbase.h"

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxCDAudio, wxObject)

wxCDtime wxCDAudio::CDtoc::GetTrackTime(wxUint8 track) const
{
  if (track > total_time.track) {
    wxCDtime dummy_time = {0, 0, 0, 0};
    return dummy_time;
  }
  return tracks_time[track];
}

wxCDtime wxCDAudio::CDtoc::GetTrackPos(wxUint8 track) const
{
  if (track > total_time.track) {
    wxCDtime dummy_time = {0, 0, 0, 0};
    return dummy_time;
  }
  return tracks_pos[track];
}

bool wxCDAudio::Play(const wxCDtime& beg_play)
{
  return Play(beg_play, GetToc().GetTotalTime());
}

bool wxCDAudio::Play(wxUint8 beg_track, wxUint8 end_track)
{
  wxCDtime beg_play = GetToc().GetTrackPos(beg_track);
  wxCDtime end_play;

  if (end_track)
    end_play = GetToc().GetTrackPos(end_track);
  else
    end_play = GetToc().GetTotalTime();
  return Play(beg_play, end_play);
}
