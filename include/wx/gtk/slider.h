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
  DECLARE_DYNAMIC_CLASS(wxSlider)

  public:
    wxSlider(void);
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
    ~wxSlider(void);
    bool Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSL_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator, 
           const wxString& name = wxSliderNameStr );
    virtual int GetValue(void) const;
    virtual void SetValue( int );
    void SetRange( int minValue, int maxValue );
    int GetMin(void) const;
    int GetMax(void) const;
    void SetTickFreq( int n, int pos );
    int GetTickFreq(void) const;
    void SetPageSize( int pageSize );
    int GetPageSize(void) const;
    void ClearSel(void);
    void ClearTicks(void);
    void SetLineSize( int lineSize );
    int GetLineSize(void) const;
    int GetSelEnd(void) const;
    int GetSelStart(void) const;
    void SetSelection( int minPos, int maxPos );
    void SetThumbLength( int len );
    int GetThumbLength(void) const;
    void SetTick( int tickPos );

  // implementation    
    
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
    
    GtkAdjustment  *m_adjust;
    float           m_oldPos;
    
};

#endif

#endif // __GTKSLIDERH__
