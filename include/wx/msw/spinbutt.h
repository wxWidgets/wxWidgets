/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_
#define _WX_SPINBUTT_H_

#ifdef __GNUG__
#pragma interface "spinbutt.h"
#endif

#include "wx/control.h"
#include "wx/event.h"

#if defined(__WIN95__)


class WXDLLEXPORT wxSpinButton : public wxSpinButtonBase
{
DECLARE_DYNAMIC_CLASS(wxSpinButton)

public:
    /*
     * Public interface
     */
    wxSpinButton() { }

    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = "wxSpinButton")
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxSpinButton();

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                const wxString& name = "wxSpinButton");


    // accessors
    virtual int GetValue() const;
    virtual void SetValue(int val);
    virtual void SetRange(int minVal, int maxVal);

    // implementation
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
    virtual bool MSWOnScroll(int orientation, WXWORD wParam,
                             WXWORD pos, WXHWND control);
};

#endif
    // _WX_WIN95__
#endif
    // _WX_SPINBUTT_H_
