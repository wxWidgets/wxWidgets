/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/defs.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_DEFS_H_
#define _WX_MOBILE_NATIVE_DEFS_H_

// States for drawing; might also be used for
// setting values in new API.
// Separate from wxRenderer defs so we can
// save memory and not include renderer code (TBD).

enum
{
    wxCTRL_STATE_NORMAL     = 0x00000000,
    wxCTRL_STATE_DISABLED   = 0x00000001,  // control is disabled
    wxCTRL_STATE_FOCUSED    = 0x00000002,  // currently has keyboard focus
    wxCTRL_STATE_PRESSED    = 0x00000004,  // (button) is pressed
    wxCTRL_STATE_HIGHLIGHTED= 0x00000008,  // highlighted
    wxCTRL_STATE_SELECTED   = 0x00000020,  // selected item in e.g. listbox
    wxCTRL_STATE_CHECKED    = 0x00000040,  // (check/radio button) is checked
    wxCTRL_STATE_UNDETERMINED = 0x00000080, // (check) undetermined state
    
    wxCTRL_STATE_FLAGS_MASK = 0x000000ff
};

/**
 
 New standard identifiers
 */

enum {
    
    // Bar button item
    
    wxID_DONE = wxID_OK,
    wxID_SEARCH = wxID_FIND,
    
    wxID_FLEXIBLESPACE = wxID_HIGHEST+1,
    wxID_FIXEDSPACE,
    wxID_BACK,
    wxID_COMPOSE,
    wxID_REPLY,
    wxID_ACTION,
    wxID_ORGANIZE,
    wxID_BOOKMARKS,
    wxID_CAMERA,
    wxID_TRASH,
    wxID_PLAY,
    wxID_PAUSE,
    wxID_REWIND,
    wxID_FASTFORWARD,
    wxID_PAGECURL,
    
    // Tab bar item
    
    wxID_FAVORITES,
    wxID_FEATURED,
    wxID_TOPRATED,
    wxID_RECENTS,
    wxID_CONTACTS,
    wxID_HISTORY,
    // wxID_BOOKMARKS,
    // wxID_SEARCH,
    wxID_DOWNLOADS,
    wxID_MOSTRECENT,
    wxID_MOSTVIEWED,
    
    wxID_EXTENDED_HIGHEST
    
};

/*
 Recognised as bar button ids but already defined by wxWidgets
 
 wxID_CANCEL
 wxID_SAVE
 wxID_EDIT
 wxID_ADD
 wxID_REFRESH
 wxID_STOP
 */

/*
 Recognised as tab bar item ids but already defined by wxWidgets
 
 wxID_MORE
 */


#define wxMO_BACK_BUTTON_ARROW_SIZE 10

#endif
// _WX_MOBILE_NATIVE_DEFS_H_
