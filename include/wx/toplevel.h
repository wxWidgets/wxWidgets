///////////////////////////////////////////////////////////////////////////////
// Name:        wx/toplevel.h
// Purpose:     declares wxTopLevelWindow class, the base class for all
//              top level windows (such as frames and dialogs)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOPLEVEL_H_
#define _WX_TOPLEVEL_H_

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxTopLevelWindow: a top level (as opposed to child) window
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindow : public wxWindow
{
public:
    /*
       for now we already have them in wxWindow, but this is wrong: these
       methods really only make sense for wxTopLevelWindow!

    virtual void SetTitle( const wxString& title) = 0;
    virtual wxString GetTitle() const = 0;
     */
};

#endif // _WX_TOPLEVEL_H_
