///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gauge.h
// Purpose:     wxGauge interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 1996-2001 wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GAUGE_H_BASE_
#define _WX_GAUGE_H_BASE_

#ifdef __GNUG__
    #pragma implementation "gaugebase.h"
#endif

#include "wx/defs.h"

#if wxUSE_GAUGE

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxGaugeNameStr;

// ----------------------------------------------------------------------------
// wxGauge: a progress bar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGaugeBase : public wxControl
{
public:
    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxGaugeNameStr);

    // set/get the control range
    virtual void SetRange(int range);
    virtual int GetRange() const;

    // position
    virtual void SetValue(int pos);
    virtual int GetValue() const;

    // appearance params (not implemented for most ports)

    virtual void SetShadowWidth(int w);
    virtual int GetShadowWidth() const;

    virtual void SetBezelFace(int w);
    virtual int GetBezelFace() const;

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    // the max position
    int m_rangeMax;

    // the current position
    int m_gaugePos;
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/gauge.h"
#elif defined(__WXMSW__)
    #ifdef __WIN95__
        #include "wx/msw/gauge95.h"
        #define wxGauge wxGauge95
        #define sm_classwxGauge sm_classwxGauge95
    #else // !__WIN95__
        #include "wx/msw/gaugemsw.h"
        #define wxGauge wxGaugeMSW
        #define sm_classwxGauge sm_classwxGaugeMSW
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/motif/gauge.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/gauge.h"
#elif defined(__WXQT__)
    #include "wx/qt/gauge.h"
#elif defined(__WXMAC__)
    #include "wx/mac/gauge.h"
#elif defined(__WXPM__)
    #include "wx/os2/gauge.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/gauge.h"
#endif

#endif // wxUSE_GAUGE

#endif
    // _WX_GAUGE_H_BASE_
