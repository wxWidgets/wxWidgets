/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/button.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKBUTTONH__
#define __GTKBUTTONH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar *wxButtonNameStr;

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

class wxButton: public wxControl
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
    {
      Create(parent, id, label, pos, size, style, validator, name);
    }
    virtual ~wxButton();

    bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    virtual void SetDefault();
    virtual void SetLabel( const wxString &label );
    virtual bool Enable( bool enable = TRUE );

    static wxSize GetDefaultSize();

    // implementation
    // --------------

    void ApplyWidgetStyle();
    bool IsOwnGtkWindow( GdkWindow *window );

protected:
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS(wxButton)
};

#endif // __GTKBUTTONH__
