/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrl.h
// Purpose:     wxSpinCtrlBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINCTRL_H_
#define _WX_SPINCTRL_H_

#include "wx/control.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
// a spin ctrl is a text control with a spin button which is usually used to
// prompt the user for a numeric input
// ----------------------------------------------------------------------------

/* there is no generic base class for this control because it's imlpemented
   very differently under MSW and other platforms

class WXDLLEXPORT wxSpinCtrlBase : public wxControl
{
public:
    wxSpinCtrlBase() { Init(); }

    // accessors
    virtual int GetValue() const = 0;
    virtual int GetMin() const { return m_min; }
    virtual int GetMax() const { return m_max; }

    // operations
    virtual void SetValue(int val) = 0;
    virtual void SetRange(int minVal, int maxVal) = 0;

protected:
    // initialize m_min/max with the default values
    void Init() { m_min = 0; m_max = 100; }

    int   m_min;
    int   m_max;
};
*/

// ----------------------------------------------------------------------------
// include the platform-dependent class implementation
// ----------------------------------------------------------------------------

#if defined(__WXMSW__) && defined(__WIN32__)
    #include "wx/msw/spinctrl.h"
#elif defined(__WXPM__)
    #include "wx/os2/spinctrl.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/spinctrl.h"
#else // Win16 || !Win
    #include "wx/generic/spinctrl.h"
#endif // platform

#endif // _WX_SPINCTRL_H_

