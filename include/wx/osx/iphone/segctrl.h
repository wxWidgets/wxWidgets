/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/segctrl.h
// Purpose:     wxSegmentedCtrl
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEGCTRL_H_
#define _WX_SEGCTRL_H_


/**
 @class wxSegmentedCtrl
 
 @category{wxbile}
 */

class WXDLLEXPORT wxSegmentedCtrl: public wxSegmentedCtrlBase
{
    DECLARE_DYNAMIC_CLASS(wxSegmentedCtrl)
public:
    /// Default constructor.
    wxSegmentedCtrl();
    
    /// Constructor.
    wxSegmentedCtrl(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxString& name = wxT("segmentedCtrl"));
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT("segmentedCtrl"));
        
    /// Insert an item, passing an optional index into the image list.
    bool InsertItem(int item, const wxString& text, int imageId = -1);
    
    /// Insert an item, passing a bitmap.
    bool InsertItem(int item, const wxString& text, const wxBitmap& bitmap);
        
    /// Get the number of items
    int GetItemCount() const;
    
    /// Get the selection
    int GetSelection() const;
    
    /// Set the selection, generating events
    int SetSelection(int item);
    
    /// Set the selection, without generating events
    int ChangeSelection(int item);
    
    /// Get the item text
    wxString GetItemText(int item) const;
    
    /// Set the text for an item
    bool SetItemText(int item, const wxString& text);
    
    /// Get the item image
    wxBitmap GetItemImage(int item) const;
    
    /// Set the image for an item
    bool SetItemImage(int item, wxBitmap& image);
    
    /// Get the content offset
    wxSize GetContentOffset(int item) const;
    
    /// Set the content offset
    void SetContentOffset(int item, const wxSize& size);
    
    /// Get the segment width
    float GetWidth(int item) const;
    
    /// Set the segment width
    void SetWidth(int item, const float width);
    
    /// Set button background ("tint") colour
    void SetButtonBackgroundColour(const wxColour& colour);
    
    /// Get button background ("tint") colour
    const wxColour& GetButtonBackgroundColour() const;
    
    /// Delete an item
    bool DeleteItem(int item);
    
    /// Delete all items
    bool DeleteAllItems();
            
protected:
    
    DECLARE_NO_COPY_CLASS(wxSegmentedCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SEGCTRL_H_
