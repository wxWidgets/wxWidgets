/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKGAUGEH__
#define __GTKGAUGEH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_GAUGE

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxGauge;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar* wxGaugeNameStr;

//-----------------------------------------------------------------------------
// wxGaugeBox
//-----------------------------------------------------------------------------

class wxGauge: public wxControl
{
public:
    wxGauge() { Init(); }

    wxGauge( wxWindow *parent,
             wxWindowID id,
             int range,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxGA_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxGaugeNameStr )
    {
        Init();

        Create(parent, id, range, pos, size, style, validator, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id, int range,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxGA_HORIZONTAL,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxGaugeNameStr );

    void SetShadowWidth( int WXUNUSED(w) ) { }
    void SetBezelFace( int WXUNUSED(w) ) { }
    void SetRange( int r );
    void SetValue( int pos );
    int GetShadowWidth() const { return 0; };
    int GetBezelFace() const { return 0; };
    int GetRange() const;
    int GetValue() const;

    // implementation
    // -------------

    void ApplyWidgetStyle();

    // the max and current gauge values
    int m_rangeMax,
        m_gaugePos;

    // obsolete functions, don't use
#ifdef WXWIN_COMPATIBILITY_2_2
    bool GetProgressBar() const { return TRUE; }
#endif // WXWIN_COMPATIBILITY_2_2

protected:
    // common part of all ctors
    void Init() { m_rangeMax = m_gaugePos = 0; }

    // set the gauge value to the value of m_gaugePos
    void DoSetGauge();

private:
    DECLARE_DYNAMIC_CLASS(wxGauge)
};

#endif

#endif // __GTKGAUGEH__
