///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/renderer.h
// Purpose:     wxRenderer class declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

/*
   wxRenderer class is used to draw all wxWindows controls. This is an ABC and
   the look of the application is determined by the concrete derivation of
   wxRenderer used in the program.

 */

#ifndef _WX_UNIX_RENDERER_H_
#define _WX_UNIX_RENDERER_H_

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxRenderer
{
public:
    // draw the label
    virtual void DrawLabel(wxDC& dc, wxWindow *window) = 0;

    // draw the border and adjust the DC
    virtual void DrawBorder(wxDC& dc, wxWindow *window) = 0;
};

#endif // _WX_UNIX_RENDERER_H_

