/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/barbutton.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BARBUTTON_H_
#define _WX_BARBUTTON_H_

#include "wx/bitmap.h"


/**
 @class wxBarButton
 
 @category{wxbile}
 */

class WXDLLEXPORT wxBarButton : public wxBarButtonBase
{
public:
    /// Default constructor
    wxBarButton() { Init(); }
    
    /// Copy constructor
    wxBarButton(const wxBarButton& button) { Init(); Copy(button); }
    
    /// Constructor taking a string label.
    wxBarButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxT("barbutton"))
    {
        Init();
        Create(parent, id, label, pos, size, style, validator, name);
    }
    
    /// Constructor taking a bitmap label.
    wxBarButton(wxWindow *parent,
                  wxWindowID id,
                  const wxBitmap& bitmap,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxT("barbutton"))
    {
        Init();
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }
    
    /// Creation function taking a text label.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("barbutton"));
    
    /// Creation function taking a bitmap label.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("barbutton"));
    
    virtual ~wxBarButton();
        
    /// Copies the item.
    void Copy(const wxBarButton& item);
    
    /// Assignment operator.
    void operator= (const wxBarButton& item) { Copy(item); }
    
protected:
    
    void Init();
        
private:
    DECLARE_DYNAMIC_CLASS(wxBarButton)
    DECLARE_EVENT_TABLE()    
};

#endif
    // _WX_BARBUTTON_H_
