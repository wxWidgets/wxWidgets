/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_
#define _WX_SPINBUTT_H_

#include "wx/control.h"
#include "wx/event.h"

class WXDLLEXPORT wxSpinButton: public wxControl
{
public:
    // Construction
    wxSpinButton() { }

    inline wxSpinButton( wxWindow *parent
                        ,wxWindowID id = -1
                        ,const wxPoint& pos = wxDefaultPosition
                        ,const wxSize& size = wxDefaultSize
                        ,long style = wxSP_VERTICAL
                        ,const wxString& name = "wxSpinButton"
                       )
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxSpinButton();

    bool Create( wxWindow *parent
                ,wxWindowID id = -1
                ,const wxPoint& pos = wxDefaultPosition
                ,const wxSize& size = wxDefaultSize
                ,long style = wxSP_VERTICAL
                ,const wxString& name = "wxSpinButton"
               );

    // Accessors
    virtual int  GetValue() const ;
    virtual void SetValue(int val) ;
    virtual void SetRange( int minVal
                          ,int maxVal
                         );

    // Implementation    
    virtual bool OS2Command( WXUINT param
                            ,WXWORD id
                           );
    virtual bool OS2OnNotify( int       idCtrl
                             ,WXLPARAM  lParam
                             ,WXLPARAM* result
                            );
    virtual bool OS2OnScroll( int    orientation
                             ,WXWORD wParam
                             ,WXWORD pos
                             ,WXHWND control
                            );

protected:
   virtual wxSize DoGetBestSize();
private:
    DECLARE_DYNAMIC_CLASS(wxSpinButton)
};
#endif
    // _WX_SPINBUTT_H_
