/////////////////////////////////////////////////////////////////////////////
// Name:        wx/segctrl.h
// Purpose:     Segmented control (NSSegmentedControl - OS X, UISegmentedControl - iOS)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEGCTRL_H_BASE_
#define _WX_SEGCTRL_H_BASE_

class WXDLLIMPEXP_CORE wxImageList;

/**
 @class wxSegmentedCtrl
 
 A segmented control.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxSegmentedCtrlBase: public wxControl
{    
public:
    /// Default constructor.
    wxSegmentedCtrlBase() { }
    
    /// Constructor.
    wxSegmentedCtrlBase(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxT("segCtrl")) { }
    
    /// Add an item, passing an optional index into the image list.
    virtual bool AddItem(const wxString& text, int imageId = -1) {
        return InsertItem(GetItemCount(), text, imageId);
    }
    
    /// Add an item, passing a bitmap.
    virtual bool AddItem(const wxString& text, const wxBitmap& bitmap) {
        return InsertItem(GetItemCount(), text, bitmap);
    }
    
    /// Insert an item, passing an optional index into the image list.
    virtual bool InsertItem(int item, const wxString& text, int imageId = -1) = 0;
    
    /// Insert an item, passing a bitmap.
    virtual bool InsertItem(int item, const wxString& text, const wxBitmap& bitmap) = 0;
        
    /// Get the number of items
    virtual int GetItemCount() const = 0;
    
    /// Get the selection
    virtual int GetSelection() const = 0;
    
    /// Set the selection, generating events
    virtual int SetSelection(int item) = 0;
    
    /// Set the selection, without generating events
    virtual int ChangeSelection(int item) = 0;

    /// Get the associated image list
    virtual wxImageList* GetImageList() const { return m_imageList; }
    
    /// Set the image list
    virtual void SetImageList(wxImageList* imageList)
    {
        m_imageList = imageList;
        m_ownsImageList = false;
    }
    
    /// Assign (own) the image list
    virtual void AssignImageList(wxImageList* imageList)
    {
        m_imageList = imageList;
        m_ownsImageList = true;
    }
    
    /// Get the item text
    virtual wxString GetItemText(int item) const = 0;
    
    /// Set the text for an item
    virtual bool SetItemText(int item, const wxString& text) = 0;
    
    /// Get the item image
    virtual wxBitmap GetItemImage(int item) const = 0;
    
    /// Set the image for an item
    virtual bool SetItemImage(int item, wxBitmap& image) = 0;
    
    /// Get the content offset
    virtual wxSize GetContentOffset(int item) const = 0;
    
    /// Set the content offset
    virtual void SetContentOffset(int item, const wxSize& size) = 0;
    
    /// Get the segment width
    virtual float GetWidth(int item) const = 0;
    
    /// Set the segment width
    virtual void SetWidth(int item, const float width) = 0;
    
    /// Set button background ("tint") colour
    virtual void SetButtonBackgroundColour(const wxColour& colour) = 0;
    
    /// Get button background ("tint") colour
    virtual const wxColour& GetButtonBackgroundColour() const = 0;
    
    /// Delete an item
    virtual bool DeleteItem(int item) = 0;
    
    /// Delete all items
    virtual bool DeleteAllItems() = 0;
                    
protected:
    
    virtual void Init() { m_ownsImageList = false; }
    
    wxImageList*    m_imageList;
    bool            m_ownsImageList;
    
    wxDECLARE_NO_COPY_CLASS(wxSegmentedCtrlBase);
};


// ----------------------------------------------------------------------------
// wxSegmentedCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/segctrl.h"
#endif


#endif
    // _WX_SEGCTRL_H_BASE_
