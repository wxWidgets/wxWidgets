///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/window.h
// Purpose:     wxUniversalWindow class which is the base class for all
//              wxUniv port controls, it supports the customization of the
//              window drawing and input processing.
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_WINDOW_H_
#define _WX_UNIX_WINDOW_H_

#ifdef __GNUG__
    #pragma interface "univwindow.h"
#endif

class WXDLLEXPORT wxRenderer;

// ----------------------------------------------------------------------------
// wxUniversalWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxUniversalWindow : public wxWindow
{
public:
    // ctor
    wxUniversalWindow(wxWindowBase *parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPanelNameStr)
        : wxWindow(parent, id, pos, size, style, validator, name)
        { }

protected:
    // draw the controls contents: the base class version draws the border as
    // required (depending on the window style flags and enabled state) and
    // adjusts the DC so that subsequent drawing is clipped to the area inside
    // the border
    virtual void DoDraw(wxDC& dc, wxRenderer *renderer);

    // event handlers
    void OnPaint(wxPaintEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIX_WINDOW_H_
