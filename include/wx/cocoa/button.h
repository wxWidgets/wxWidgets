/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/button.h
// Purpose:     wxButton class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/29
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_BUTTON_H__
#define __WX_COCOA_BUTTON_H__

#include "wx/cocoa/NSButton.h"

// ========================================================================
// wxButton
// ========================================================================
class WXDLLEXPORT wxButton : public wxButtonBase, protected wxCocoaNSButton
{
    DECLARE_DYNAMIC_CLASS(wxButton)
    DECLARE_EVENT_TABLE()
    WX_DECLARE_COCOA_OWNER(NSButton,NSControl,NSView)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
   wxButton() { }
   wxButton(wxWindow *parent, wxWindowID winid,
             const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Create(parent, winid, label, pos, size, style, validator, name);
    }
    
    wxButton(wxWindow *parent, wxWindowID winid, wxStockItemID stock,
           const wxString& descriptiveLabel = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
    {
        Create(parent, winid, stock, descriptiveLabel, pos, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID winid,
            const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxButtonNameStr);
    
    bool Create(wxWindow *parent, wxWindowID winid, wxStockItemID stock,
           const wxString& descriptiveLabel = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
    {
        return CreateStock(parent, winid, stock, descriptiveLabel,
                           pos, style, validator, name);
    }
    
    virtual ~wxButton();

// ------------------------------------------------------------------------
// Cocoa callbacks
// ------------------------------------------------------------------------
protected:
    virtual void Cocoa_wxNSButtonAction(void);
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    wxString GetLabel() const;
    void SetLabel(const wxString& label);
};

#endif // __WX_COCOA_BUTTON_H__
