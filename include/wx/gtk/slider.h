/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSLIDERH__
#define __GTKSLIDERH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_SLIDER

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxSlider;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxSliderNameStr;

//-----------------------------------------------------------------------------
// wxSlider
//-----------------------------------------------------------------------------

class wxSlider: public wxControl
{
public:
    wxSlider() {}
    inline wxSlider( wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxSliderNameStr )
    {
      Create( parent, id, value, minValue, maxValue, pos, size, style, validator, name );
    }
    bool Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator, 
           const wxString& name = wxSliderNameStr );
    virtual int GetValue() const;
    virtual void SetValue( int );
    void SetRange( int minValue, int maxValue );
    int GetMin() const;
    int GetMax() const;
    void SetTickFreq( int n, int pos );
    int GetTickFreq() const;
    void SetPageSize( int pageSize );
    int GetPageSize() const;
    void ClearSel();
    void ClearTicks();
    void SetLineSize( int lineSize );
    int GetLineSize() const;
    int GetSelEnd() const;
    int GetSelStart() const;
    void SetSelection( int minPos, int maxPos );
    void SetThumbLength( int len );
    int GetThumbLength() const;
    void SetTick( int tickPos );

    // implementation    
    // --------------
    
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
    
    GtkAdjustment  *m_adjust;
    float           m_oldPos;
    
private:
    DECLARE_DYNAMIC_CLASS(wxSlider)
};

#endif

#endif // __GTKSLIDERH__
