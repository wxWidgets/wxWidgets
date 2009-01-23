/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#include "wx/control.h"
#include "wx/gdicmn.h"

WXDLLIMPEXP_DATA_CORE(extern const char) wxButtonNameStr[];

// Pushbutton
class WXDLLIMPEXP_CORE wxButton: public wxButtonBase
{
public:
    wxButton() {}
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    static wxSize GetDefaultSize();

    virtual wxWindow *SetDefault();
    virtual void Command(wxCommandEvent& event);

    // osx specific event handling common for all osx-ports
    
    virtual bool        OSXHandleClicked( double timestampsec );

protected:
    virtual wxSize DoGetBestSize() const ;

    DECLARE_DYNAMIC_CLASS(wxButton)
};

class WXDLLIMPEXP_CORE wxDisclosureTriangle: public wxControl
{
public:
    wxDisclosureTriangle(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    void SetOpen( bool open );
    bool IsOpen() const;

    // osx specific event handling common for all osx-ports
    
    virtual bool        OSXHandleClicked( double timestampsec );

protected:
    virtual wxSize DoGetBestSize() const ;
    
};

#endif
    // _WX_BUTTON_H_
