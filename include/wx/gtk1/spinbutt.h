/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSPINBUTTH__
#define __GTKSPINBUTTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_SPINBTN

#include "wx/control.h"

//-----------------------------------------------------------------------------
// wxSpinButton
//-----------------------------------------------------------------------------

class wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton() { }
    wxSpinButton( wxWindow *parent, wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxSP_VERTICAL, const wxString& name = "wxSpinButton")
    {
        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent, wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxSP_VERTICAL, const wxString& name = "wxSpinButton" );

    virtual int GetValue() const;
    virtual void SetValue( int value );
    virtual void SetRange( int minVal, int maxVal );
    virtual int GetMin() const;
    virtual int GetMax() const;

    // implementation
    void OnSize( wxSizeEvent &event );

    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();

    GtkAdjustment  *m_adjust;
    float           m_oldPos;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxSpinButton)
};

#endif
  // wxUSE_SPINBTN

#endif
    // __GTKSPINBUTTH__
