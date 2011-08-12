/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/tabctrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABCTRL_H_
#define _WX_TABCTRL_H_

/**
 @class wxTabCtrl
 
 @category{wxbile}
 */
class WXDLLEXPORT wxTabCtrl: public wxTabCtrlBase
{
    DECLARE_DYNAMIC_CLASS(wxTabCtrl)
    
public:
    
    /// Default constructor.
    wxTabCtrl();
    
    /// Constructor.
    wxTabCtrl(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT|wxTAB_BITMAP,
                const wxString& name = wxT("tabCtrl"));
                
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT|wxTAB_BITMAP,
                const wxString& name = wxT("tabCtrl"));    
    
    virtual ~wxTabCtrl();
        

    // Accessors
    
    /// Get the selection
    virtual int GetSelection() const;
    
    /// Get the tab with the current keyboard focus. Not implemented.
    int GetCurFocus() const;
        
    /// Get the number of items
    virtual int GetItemCount() const;
    
    /// Get the rect corresponding to the tab. Not implemented.
    virtual bool GetItemRect(int item, wxRect& rect) const;
    
    // Get the number of rows
    int GetRowCount() const;
    
    /// Get the item text
    virtual wxString GetItemText(int item) const ;
    
    /// Get the item image
    virtual int GetItemImage(int item) const;
    
    // Get the item data
    //void* GetItemData(int item) const;
    
    /// Set the selection, generating events
    virtual int SetSelection(int item);
    
    /// Set the selection, without generating events
    virtual int ChangeSelection(int item);
    
    /// Set the image list
    void SetImageList(wxImageList* imageList);
    
    /// Assign (own) the image list
    void AssignImageList(wxImageList* imageList);
    
    /// Set the text for an item
    virtual bool SetItemText(int item, const wxString& text);
    
    /// Set the image for an item
    virtual bool SetItemImage(int item, int image);
    
    // Set the data for an item
    // bool SetItemData(int item, void* data);
    
    /// Set the size for a fixed-width tab control. Not implemented.
    virtual void SetItemSize(const wxSize& size);
    
    /// Set the padding between tabs. Not implemented.
    void SetPadding(const wxSize& padding);
    
    // Operations
    
    /// Delete all items
    virtual bool DeleteAllItems();
    
    /// Delete an item
    virtual bool DeleteItem(int item);
    
    /// Hit test. Not implemented.
    int HitTest(const wxPoint& pt, long& flags);
    
    /// Insert an item, passing an optional index into the image list.
    virtual bool InsertItem(int item, const wxString& text, int imageId = -1);
    
    /// Insert an item, passing a bitmap.
    virtual bool InsertItem(int item, const wxString& text, const wxBitmap& bitmap);
    
    /// Add an item, passing an optional index into the image list.
    virtual bool AddItem(const wxString& text, int imageId = -1);
    
    /// Add an item, passing a bitmap.
    virtual bool AddItem(const wxString& text, const wxBitmap& bitmap);
    
    /// Set a text badge for the given item
    virtual bool SetBadge(int item, const wxString& badge);
    
    /// Get the text badge for the given item
    virtual wxString GetBadge(int item) const;
        
    /// Sets the button background colour.
    virtual bool SetButtonBackgroundColour(const wxColour &colour);
        
    
    // Implementation
    
    virtual wxSize DoGetBestSize() const;
    
    virtual void OnInsertItem(wxBarButton* button);
    
    void OnMouseEvent(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
    bool SendCommand(wxEventType eventType, int selection);
    
    void OnSysColourChanged(wxSysColourChangedEvent& event);
            
protected:
    
    void Init();
    
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTabCtrl)
};

#endif
    // _WX_TABCTRL_H_
