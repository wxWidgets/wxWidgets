/////////////////////////////////////////////////////////////////////////////
// Name:        gaugemsw.h
// Purpose:     wxGauge class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _GAUGEMSW_H_
#define _GAUGEMSW_H_

#ifdef __GNUG__
#pragma interface "gaugemsw.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxGaugeMSW: public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxGaugeMSW)

public:
    wxGaugeMSW(void) { m_rangeMax = 0; m_gaugePos = 0; }

    wxGaugeMSW(wxWindow *parent, wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxGaugeNameStr)
    {
        Create(parent, id, range, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxGaugeNameStr);

    void SetShadowWidth(int w);
    void SetBezelFace(int w);
    void SetRange(int r);
    void SetValue(int pos);

    int GetShadowWidth(void) const ;
    int GetBezelFace(void) const ;
    int GetRange(void) const ;
    int GetValue(void) const ;

    bool SetForegroundColour(const wxColour& col);
    bool SetBackgroundColour(const wxColour& col);

    // Backward compatibility
#if WXWIN_COMPATIBILITY
    void SetButtonColour(const wxColour& col) { SetForegroundColour(col); }
#endif

    virtual void Command(wxCommandEvent& WXUNUSED(event)) {} ;

protected:
    int      m_rangeMax;
    int      m_gaugePos;
};

#endif
    // _GAUGEMSW_H_
