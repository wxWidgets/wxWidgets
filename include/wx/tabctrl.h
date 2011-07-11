/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tabctrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABCTRL_H_BASE_
#define _WX_TABCTRL_H_BASE_

class WXDLLIMPEXP_CORE wxImageList;

#include "wx/buttonstrip.h"

#define wxTAB_TEXT   0x00010000
#define wxTAB_BITMAP 0x00020000

/*
 * Flags returned by HitTest
 */

#define wxTAB_HITTEST_NOWHERE           1
#define wxTAB_HITTEST_ONICON            2
#define wxTAB_HITTEST_ONLABEL           4
#define wxTAB_HITTEST_ONITEM            6

class WXDLLEXPORT wxTabEvent : public wxNotifyEvent
{
public:
    wxTabEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
               int nSel = -1, int nOldSel = -1)
    : wxNotifyEvent(commandType, id)
    {
        m_nSel = nSel;
        m_nOldSel = nOldSel;
    }
    
    /// Get the currently selected page (-1 if none)
    int GetSelection() const { return m_nSel; }
    
    /// Set the currently selected page (-1 if none)
    void SetSelection(int nSel) { m_nSel = nSel; }
    
    /// Get the page that was selected before the change (-1 if none)
    int GetOldSelection() const { return m_nOldSel; }
    
    /// Set the page that was selected before the change (-1 if none)
    void SetOldSelection(int nOldSel) { m_nOldSel = nOldSel; }
    
private:
    int m_nSel,     // currently selected page
    m_nOldSel;  // previously selected page
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTabEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED, 800)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING, 801)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxTabEventFunction)(wxTabEvent&);

#define EVT_TAB_SEL_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TAB_SEL_CHANGED, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTabEventFunction, & fn ), NULL),
#define EVT_TAB_SEL_CHANGING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TAB_SEL_CHANGING, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTabEventFunction, & fn ), NULL),


/**
 @class wxTabCtrl
 
 A tab control. Images can be referenced either using an image list,
 as per the standard wxWidgets tab/notebook API, or by passing
 bitmaps directly.
 
 TODO: customisation of the set of tabs.
 
 @category{wxbile}
 */
class WXDLLEXPORT wxTabCtrlBase: public wxControl
{
public:
    
    /// Default constructor.
    wxTabCtrlBase() { }
    
    /// Constructor.
    wxTabCtrlBase(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT|wxTAB_BITMAP,
                const wxString& name = wxT("tabCtrl"));
                
    virtual bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT|wxTAB_BITMAP,
                const wxString& name = wxT("tabCtrl")) = 0;    
    
    virtual ~wxTabCtrlBase() { }
    
    // Accessors
    
    /// Get the selection
    virtual int GetSelection() const = 0;
    
    /// Get the tab with the current keyboard focus. Not implemented.
    virtual int GetCurFocus() const = 0;
    
    /// Get the associated image list
    virtual wxImageList* GetImageList() const { return m_buttonStrip.GetImageList(); }
    
    /// Get the number of items
    virtual int GetItemCount() const = 0;
    
    /// Get the rect corresponding to the tab. Not implemented.
    //virtual bool GetItemRect(int item, wxRect& rect) const;
    
    // Get the number of rows
    virtual int GetRowCount() const = 0;
    
    /// Get the item text
    virtual wxString GetItemText(int item) const = 0;
    
    /// Get the item image
    virtual int GetItemImage(int item) const = 0;
    
    // Get the item data
    //void* GetItemData(int item) const;
    
    /// Set the selection, generating events
    virtual int SetSelection(int item) = 0;
    
    /// Set the selection, without generating events
    virtual int ChangeSelection(int item) = 0;
    
    /// Set the image list
    virtual void SetImageList(wxImageList* imageList) = 0;
    
    /// Assign (own) the image list
    virtual void AssignImageList(wxImageList* imageList) = 0;
    
    /// Set the text for an item
    virtual bool SetItemText(int item, const wxString& text) = 0;
    
    /// Set the image for an item
    virtual bool SetItemImage(int item, int image) = 0;
    
    // Set the data for an item
    // bool SetItemData(int item, void* data);
    
    /// Set the size for a fixed-width tab control. Not implemented.
    virtual void SetItemSize(const wxSize& size) = 0;
    
    /// Set the padding between tabs. Not implemented.
    virtual void SetPadding(const wxSize& padding) = 0;
    
    // Operations
    
    /// Delete all items
    virtual bool DeleteAllItems() = 0;
    
    /// Delete an item
    virtual bool DeleteItem(int item) = 0;
        
    /// Insert an item, passing an optional index into the image list.
    virtual bool InsertItem(int item, const wxString& text, int imageId = -1) = 0;
    
    /// Insert an item, passing a bitmap.
    virtual bool InsertItem(int item, const wxString& text, const wxBitmap& bitmap) = 0;
    
    /// Add an item, passing an optional index into the image list.
    virtual bool AddItem(const wxString& text, int imageId = -1) = 0;
    
    /// Add an item, passing a bitmap.
    virtual bool AddItem(const wxString& text, const wxBitmap& bitmap) = 0;
    
    /// Set a text badge for the given item
    virtual bool SetBadge(int item, const wxString& badge) = 0;
    
    /// Get the text badge for the given item
    virtual wxString GetBadge(int item) const = 0;
    
    // Implementation
    
    virtual wxSize DoGetBestSize() const = 0;
    
    virtual void OnInsertItem(wxBarButton* button) = 0;
    
    virtual void OnMouseEvent(wxMouseEvent& event) = 0;
    virtual void OnPaint(wxPaintEvent& event) = 0;
    virtual void OnSize(wxSizeEvent& event) = 0;
    virtual void OnEraseBackground(wxEraseEvent& event) = 0;
    
    virtual bool SendCommand(wxEventType eventType, int selection) = 0;
    
    virtual void OnSysColourChanged(wxSysColourChangedEvent& event) = 0;
    
    wxButtonStrip& GetButtonStrip() { return m_buttonStrip; }
    const wxButtonStrip& GetButtonStrip() const { return m_buttonStrip; }
    
    /// Sets the border colour.
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }
    
    /// Gets the border colour.
    virtual wxColour GetBorderColour() const { return m_borderColour; }
    
    virtual bool SetBackgroundColour(const wxColour &colour) { m_backgroundColour = colour; }
    virtual wxColour GetBackgroundColour() const { return m_backgroundColour; }
    
    /// Sets the button background colour.
    virtual bool SetButtonBackgroundColour(const wxColour &colour) { m_buttonBackgroundColour = colour; }
    
    /// Gets the button background colour.
    virtual wxColour GetButtonBackgroundColour() const { return m_buttonBackgroundColour; }
    
    virtual bool SetForegroundColour(const wxColour &colour) { m_foregroundColour = colour; }
    virtual wxColour GetForegroundColour() const { return m_foregroundColour; }
    
    virtual bool SetFont(const wxFont& font) { m_font = font; }
    virtual wxFont GetFont() const { return m_font; }
    
protected:
    wxButtonStrip   m_buttonStrip;
    wxColour        m_borderColour;
    wxColour        m_backgroundColour;
    wxColour        m_buttonBackgroundColour;
    wxColour        m_foregroundColour;
    wxFont          m_font;
    
    wxDECLARE_NO_COPY_CLASS(wxTabCtrlBase);
};


// ----------------------------------------------------------------------------
// wxTabCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/tabctrl.h"
#endif


#endif
    // _WX_TABCTRL_H_BASE_
