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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

class WXDLLIMPEXP_CORE wxGauge;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern WXDLLIMPEXP_CORE const wxChar* wxGaugeNameStr;

//-----------------------------------------------------------------------------
// wxGaugeBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGauge: public wxControl
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

    bool IsVertical() const { return HasFlag(wxGA_VERTICAL); }
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    // implementation
    // -------------

    // the max and current gauge values
    int m_rangeMax,
        m_gaugePos;

    // obsolete functions, don't use
#if WXWIN_COMPATIBILITY_2_2
    bool GetProgressBar() const { return true; }
#endif // WXWIN_COMPATIBILITY_2_2

protected:
    // common part of all ctors
    void Init() { m_rangeMax = m_gaugePos = 0; }

    // set the gauge value to the value of m_gaugePos
    void DoSetGauge();

    virtual wxSize DoGetBestSize() const;

    virtual wxVisualAttributes GetDefaultAttributes() const;

private:
    DECLARE_DYNAMIC_CLASS(wxGauge)
};

#endif

#endif // __GTKGAUGEH__
