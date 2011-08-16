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
    
    
    /// Sets the button background colour.
    virtual bool SetBackgroundColour(const wxColour &colour);

    /// Sets the button foreground (text) colour.
    virtual bool SetForegroundColour(const wxColour &colour);

    /// Sets the selection colour.
    virtual void SetSelectionColour(const wxColour& colour);

    /// Sets the button font.
    virtual void SetFont(const wxFont& font);

    /// Sets the button border colour.
    virtual void SetBorderColour(const wxColour &colour);

    /// Sets the size of the button.
    virtual void SetSize(const wxSize& sz);

    /// Sets the position of the button.
    virtual void SetPosition(const wxPoint& pt);

    /// Sets the normal state bitmap.
    virtual void SetNormalBitmap(const wxBitmap& bitmap);

    /// Sets the disabled state bitmap.
    virtual void SetDisabledBitmap(const wxBitmap& bitmap);

    /// Sets the selected state bitmap.
    virtual void SetSelectedBitmap(const wxBitmap& bitmap);

    /// Sets the highlighted state bitmap.
    virtual void SetHighlightedBitmap(const wxBitmap& bitmap);

    /// Sets the text label.
    virtual void SetLabel(const wxString& label);

    // Sets the horizontal margin between button edge and content.
    virtual void SetMarginX(int margin);

    // Sets the vertical margin between button edge content.
    virtual void SetMarginY(int margin);

    // Sets the enabled flag.
    virtual void SetEnabled(bool enabled);

    // Sets the selected flag.
    virtual void SetSelected(bool selected);

    // Sets the highlighted flag.
    virtual void SetHighlighted(bool highlighted);

    // Sets the button style.
    virtual void SetStyle(int style);
    
    
    // Implementation: get Cocoa's UINavigationItem
    void* GetNativeBarButtonItem() { return m_nativeBarButtonItem; }
    
protected:
    
    void Init();
        
private:
    DECLARE_DYNAMIC_CLASS(wxBarButton)
    DECLARE_EVENT_TABLE()
    
    // Implementation: Cocoa's UIBarButtonItem object
    void*   m_nativeBarButtonItem;
};

#endif
    // _WX_BARBUTTON_H_
