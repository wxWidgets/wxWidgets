// ---------------------------------------------------------------------------
// Name:       cdbase.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998, 1999, 2000
// Copyright:  (C) 1997, 1998, 1999, 2000 Guilhem Lavaux
// License:    wxWindows license
// ---------------------------------------------------------------------------
#ifndef __CDA_base_H__
#define __CDA_base_H__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/mmedia/defs.h"

typedef struct wxCDtime {
  wxUint8 track;
  wxUint8 hour, min, sec;
} wxCDtime;

class WXDLLIMPEXP_MMEDIA wxCDAudio : public wxObject {
  DECLARE_ABSTRACT_CLASS(wxCDAudio)
public:
  typedef enum { PLAYING, PAUSED, STOPPED } CDstatus;
  // Table of contents manager
  class WXDLLIMPEXP_MMEDIA CDtoc {
  protected:
    wxCDtime *tracks_time, *tracks_pos;
    wxCDtime total_time;
  public:
    //
    CDtoc(wxCDtime& tot_tm, wxCDtime *trks_tm, wxCDtime *trks_pos)
      { tracks_time = trks_tm; total_time = tot_tm; tracks_pos = trks_pos; }

    // Returns the length of the specified track
    // track: track to get length
    wxCDtime GetTrackTime(wxUint8 track) const;
    // Returns the position of the specified track
    // track: track to get position
    wxCDtime GetTrackPos(wxUint8 track) const;
    // Returns the total time
    inline wxCDtime GetTotalTime() const { return total_time; }
  };
public:
  //
  wxCDAudio() : wxObject() {}
  //
  virtual ~wxCDAudio() {}

  // Play audio at the specified position
  virtual bool Play(const wxCDtime& beg_play, const wxCDtime& end_play) = 0;
  // Play audio from the specified to the end of the CD audio
  bool Play(const wxCDtime& beg_play);
  //
  bool Play(wxUint8 beg_track, wxUint8 end_track = 0);
  // Pause the audio playing
  virtual bool Pause() = 0;
  // Resume a paused audio playing
  virtual bool Resume() = 0;
  // Get the current CD status
  virtual CDstatus GetStatus() = 0;
  // Get the current playing time
  virtual wxCDtime GetTime() = 0;
  // Returns the table of contents
  virtual const CDtoc& GetToc() = 0;
  // CD ok
  virtual bool Ok() const = 0;
};

#endif
