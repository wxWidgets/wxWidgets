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

#ifdef __GNUG__
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

extern const char* wxGaugeNameStr;

//-----------------------------------------------------------------------------
// wxGaugeBox
//-----------------------------------------------------------------------------

class wxGauge: public wxControl
{
public:
    inline wxGauge() { m_rangeMax = 0; m_gaugePos = 0; m_useProgressBar = TRUE; }

    inline wxGauge( wxWindow *parent, wxWindowID id, int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr )
    {
      Create(parent, id, range, pos, size, style, validator, name);
    }
    bool Create( wxWindow *parent, wxWindowID id, int range,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxGA_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxGaugeNameStr );
    void SetShadowWidth( int WXUNUSED(w) ) {};
    void SetBezelFace( int WXUNUSED(w) ) {};
    void SetRange( int r );
    void SetValue( int pos );
    int GetShadowWidth() const { return 0; };
    int GetBezelFace() const { return 0; };
    int GetRange() const;
    int GetValue() const;

    // Are we a Win95/GTK progress bar, or a normal gauge?
    inline bool GetProgressBar() const { return m_useProgressBar; }
    
    // implementation
    // -------------
    
    void ApplyWidgetStyle();
    int      m_rangeMax;
    int      m_gaugePos;
    bool     m_useProgressBar;
   
private:
    DECLARE_DYNAMIC_CLASS(wxGauge)
};

#endif

#endif // __GTKGAUGEH__
