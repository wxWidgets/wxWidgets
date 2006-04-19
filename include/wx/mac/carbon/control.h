/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

WXDLLEXPORT_DATA(extern const wxChar) wxControlNameStr[];

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   wxControl(wxWindow *parent, wxWindowID winid,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxControlNameStr)
    {
        Create(parent, winid, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxControlNameStr);
   virtual ~wxControl();

   // Simulates an event
   virtual void Command(wxCommandEvent& event) { ProcessCommand(event); }

   // implementation from now on
   // --------------------------

   // Calls the callback and appropriate event handlers
   bool ProcessCommand(wxCommandEvent& event);

   void                 OnKeyDown( wxKeyEvent &event ) ;
};

#endif
    // _WX_CONTROL_H_
