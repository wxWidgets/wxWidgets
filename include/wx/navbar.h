/////////////////////////////////////////////////////////////////////////////
// Name:        wx/navbar.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NAVBAR_H_BASE_
#define _WX_NAVBAR_H_BASE_

#include "wx/dynarray.h"
#include "wx/bitmap.h"
#include "wx/control.h"

#include "wx/barbutton.h"
#include "wx/navitem.h"

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationBarNameStr[];

/**
 Bar style
 */

enum {
    wxNAVBAR_NORMAL_BG =            0x00000000,
    wxNAVBAR_BLACK_OPAQUE_BG =      0x00010000,
    wxNAVBAR_BLACK_TRANSLUCENT_BG = 0x00020000
};


/**
 @class wxNavigationBar
 
 A navigation bar manages a stack of wxNavigationItem objects.
 It shows a centred title, and a Back button on the left of the bar if
 there is one or more item under the current item.
 
 The bar can also be customised by adding a user-defined wxBarButton to the
 left or right of the bar.
 
 Note that in most cases, it's better to use wxNavigationCtrl in preference
 to this control, since wxNavigationCtrl helps you manage content as well
 as the bar appearance.
 
 Control styles can be:
 
 @li wxNAVBAR_NORMAL_BG: a normal, opaque background colour
 @li wxNAVBAR_BLACK_OPAQUE_BG: an opaque black background
 @li wxNAVBAR_BLACK_TRANSLUCENT_BG: a translucent black background
 
 To use this class, call wxNavigationBar::PushItem to push a
 wxNavigationItem object onto the stack. The wxNavigationItem should
 be prepared by setting the title to show, and optionally left
 and right wxBarButton objects to use. If no buttons are specified, the
 default back button will be shown, displaying the title of the item below
 as its label.
 
 The application can call wxNavigationBar::PopItem to pop the top
 item. The application should delete the returned item if non-NULL.
 
 Regular button events are emitted for the Back button (wxID_BACK)
 and other buttons placed on the bar. By default, the handler for wxID_BACK
 causes the top item to be popped from the stack and deleted.
 
 Popping and pushing items, whether handled by wxNavigationBar or
 initiated by the application, causes wxNavigationBarEvent events to be
 generated. Event types are:
 
 @li wxEVT_COMMAND_NAVBAR_POPPED: an item was popped
 @li wxEVT_COMMAND_NAVBAR_PUSHED: an item was pushed
 @li wxEVT_COMMAND_NAVBAR_POPPING: an item is about to be popped. This can be vetoed.
 @li wxEVT_COMMAND_NAVBAR_PUSHING: an item is about to be pushed. This can be vetoed.
 
 @category{mobile}
 */

class WXDLLEXPORT wxNavigationBarBase: public wxControl
{
public:
    /// Default constructor.
    wxNavigationBarBase() { }
    
    /// Constructor.
    wxNavigationBarBase(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxNavigationBarNameStr);
    
    virtual ~wxNavigationBarBase();
    
    /// Pushes an item onto the stack. If this function returns false, it may have
    /// been vetoed in an event handler and the application must delete the item
    /// to avoid a memory leak.
    virtual bool PushItem(wxNavigationItem* item) = 0;
    
    /// Pops an item off the stack. The item must then be deleted or stored by the application,
    /// if the return value is non-NULL.
    virtual wxNavigationItem* PopItem() = 0;
    
    /// Returns the top item.
    virtual wxNavigationItem* GetTopItem() const = 0;
    
    /// Returns the back item (the item below the top item).
    virtual wxNavigationItem* GetBackItem() const = 0;
    
    /// Returns the item stack.
    const wxNavigationItemArray& GetItems() const { return m_items; }
    
    /// Sets the item stack.
    virtual void SetItems(const wxNavigationItemArray& items) = 0;
    
    /// Clears the item stack, deleting the items.
    virtual void ClearItems(bool deleteItems = true) = 0;
    
    // IMPLEMENTATION
    
    virtual wxBarButton* GetLeftButton() = 0;
    virtual wxBarButton* GetRightButton() = 0;
    virtual wxString GetCurrentTitle() = 0;
    
    // Position the buttons
    virtual void PositionButtons() = 0;
    
    // Gets the best size for this button
    virtual wxSize GetBestButtonSize(wxDC& dc, const wxBarButton& item) const = 0;
    
    void SetButtonMarginX(int margin) { m_buttonMarginX = margin; }
    int GetButtonMarginX() const { return m_buttonMarginX; }
    
    void SetButtonMarginY(int margin) { m_buttonMarginY = margin; }
    int GetButtonMarginY() const { return m_buttonMarginY; }
    
    // Space between image and label
    void SetButtonMarginInterItemY(int margin) { m_buttonMarginInterItemY = margin; }
    int GetButtonMarginInterItemY() const { return m_buttonMarginInterItemY; }
    
    void SetInterButtonSpacing(int spacing) { m_interButtonSpacing = spacing; }
    int GetInterButtonSpacing() const { return m_interButtonSpacing; }
    
    /// Set the margin between left or right side and first or last button.
    void SetEndMargin(int margin) { m_endMargin = margin; }
    
    /// Get the margin between left or right side and first or last button.
    int GetEndMargin() const { return m_endMargin; }
    
    virtual bool SetBackgroundColour(const wxColour &colour) = 0;
    virtual bool SetForegroundColour(const wxColour &colour) = 0;
    virtual bool SetFont(const wxFont& font) = 0;
    virtual bool Enable(bool enable) = 0;
    
protected:    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
    wxNavigationItemArray m_items;
    
private:
    
    // Default back button
    wxBarButton           m_backButton;
    
    int                     m_buttonMarginX;            // between content and button edge
    int                     m_buttonMarginY;
    int                     m_buttonMarginInterItemY;   // between image and label
    int                     m_interButtonSpacing;
    int                     m_endMargin;                // margin between left or right side and first or last button
    
    wxDECLARE_NO_COPY_CLASS(wxNavigationBarBase);
};


/**
 @class wxNavigationBarEvent
 
 Popping and pushing items, whether handled by wxNavigationBar or
 initiated by the application, causes wxNavigationBarEvent events to be
 generated. Event types are:
 
 @li wxEVT_COMMAND_NAVBAR_POPPED: an item was popped
 @li wxEVT_COMMAND_NAVBAR_PUSHED: an item was pushed
 @li wxEVT_COMMAND_NAVBAR_POPPING: an item is about to be popped. This can be vetoed.
 @li wxEVT_COMMAND_NAVBAR_PUSHING: an item it about to be pushed. This can be vetoed.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxNavigationBarEvent: public wxNotifyEvent
{
public:
    wxNavigationBarEvent()
    {
        m_oldItem = NULL;
        m_newItem = NULL;
    }
    wxNavigationBarEvent(wxEventType commandType, int id,
                         wxNavigationItem* oldItem, wxNavigationItem* newItem)
    : wxNotifyEvent(commandType, id)
    {
        m_oldItem = oldItem;
        m_newItem = newItem;
    }
    
    /// Set the new item
    void SetNewItem(wxNavigationItem* newItem) { m_newItem = newItem; }
    
    /// Get the new item
    wxNavigationItem* GetNewItem() const { return m_newItem; }
    
    /// Set the old item
    void SetOldItem(wxNavigationItem* oldItem) { m_oldItem = oldItem; }
    
    /// Get the old item
    wxNavigationItem* GetOldItem() const { return m_oldItem; }
    
private:
    wxNavigationItem* m_oldItem;
    wxNavigationItem* m_newItem;
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxNavigationBarEvent)
};

typedef void (wxEvtHandler::*wxNavigationBarEventFunction)(wxNavigationBarEvent&);

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPED,     850)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHED,     851)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPING,    852)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHING,    853)
END_DECLARE_EVENT_TYPES()

#define wxNavigationBarEventHandler(func) \
(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxNavigationEventFunction, &func)

#define EVT_NAVBAR_POPPED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVBAR_POPPED, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVBAR_POPPING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVBAR_POPPING, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVBAR_PUSHED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVBAR_PUSHED, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVBAR_PUSHING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVBAR_PUSHING, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),


// ----------------------------------------------------------------------------
// wxNavigationBar class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/navbar.h"
#endif


#endif
    // _WX_NAVBAR_H_BASE_
