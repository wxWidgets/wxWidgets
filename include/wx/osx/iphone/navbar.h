/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/navbar.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NAVBAR_H_
#define _WX_NAVBAR_H_

#include "wx/dynarray.h"
#include "wx/bitmap.h"
#include "wx/control.h"


/**
 @class wxNavigationBar
 @category{mobile}
 */

class WXDLLEXPORT wxNavigationBar: public wxNavigationBarBase
{
public:
    /// Default constructor.
    wxNavigationBar() { Init(); }
    
    /// Constructor.
    wxNavigationBar(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxNavigationBarNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxNavigationBarNameStr);
    
    virtual ~wxNavigationBar();
    
    /// Pushes an item onto the stack. If this function returns false, it may have
    /// been vetoed in an event handler and the application must delete the item
    /// to avoid a memory leak.
    bool PushItem(wxNavigationItem* item);
    
    /// Pops an item off the stack. The item must then be deleted or stored by the application,
    /// if the return value is non-NULL.
    wxNavigationItem* PopItem();
    
    /// Returns the top item.
    wxNavigationItem* GetTopItem() const;
    
    /// Returns the back item (the item below the top item).
    wxNavigationItem* GetBackItem() const;
    
    /// Sets the item stack.
    void SetItems(const wxNavigationItemArray& items);
    
    /// Clears the item stack, deleting the items.
    void ClearItems(bool deleteItems = true);
    
    // IMPLEMENTATION
    
    wxBarButton* GetLeftButton();
    wxBarButton* GetRightButton();
    wxString GetCurrentTitle();
    
    // Position the buttons
    void PositionButtons();
    
    // Gets the best size for this button
    wxSize GetBestButtonSize(wxDC& dc, const wxBarButton& item) const;
        
    bool SetBackgroundColour(const wxColour &colour);
    bool SetForegroundColour(const wxColour &colour);
    bool SetFont(const wxFont& font);
    bool Enable(bool enable);
    
protected:
    void Init();
    
    // send a notification event, return true if processed
    bool SendClickEvent(wxBarButton* button);
    
    // Draw background
    void DrawBackground(wxDC& dc);
    
    // Hit test
    int HitTest(const wxPoint& pt, wxBarButton** button);
    
    // Draw a button
    void DrawButton(wxDC& dc, wxBarButton& item);
    
    // Draw the title
    void DrawTitle(wxDC& dc);
    
    // Draw all items
    void DrawItems(wxDC& dc);
    
    // Get the best size
    wxSize DoGetBestSize() const;
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
private:
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxNavigationBar)
    DECLARE_EVENT_TABLE()
    
};

#endif
    // _WX_NAVBAR_H_
