/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSLIDERH__
#define __GTKSLIDERH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
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
    wxSlider( wxWindow *parent, const wxWindowID id,
           const int value, const int minValue, const int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxSL_HORIZONTAL,
/*           const wxValidator& validator = wxDefaultValidator, */
           const wxString& name = wxSliderNameStr);
   ~wxSlider(void);
   bool Create(wxWindow *parent, const wxWindowID id,
           const int value, const int minValue, const int maxValue,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxSL_HORIZONTAL,
/*           const wxValidator& validator = wxDefaultValidator, */
           const wxString& name = wxSliderNameStr);
    virtual int GetValue(void) const;
    virtual void SetValue( const int );
    void GetSize( int *x, int *y ) const;
    void SetSize( const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO );
    void GetPosition( int *x, int *y ) const;
    void SetRange( const int minValue, const int maxValue );
    int GetMin(void) const;
    int GetMax(void) const;
    void SetTickFreq( const int n, const int pos );
    int GetTickFreq(void) const;
    void SetPageSize( const int pageSize );
    int GetPageSize(void) const;
    void ClearSel(void);
    void ClearTicks(void);
    void SetLineSize( const int lineSize );
    int GetLineSize(void) const;
    int GetSelEnd(void) const;
    int GetSelStart(void) const;
    void SetSelection( const int minPos, const int maxPos );
    void SetThumbLength( const int len );
    int GetThumbLength(void) const;
    void SetTick( const int tickPos );

  public:
  
    GtkAdjustment  *m_adjust;
    float           m_oldPos;
    
};

#endif // __GTKSLIDERH__
