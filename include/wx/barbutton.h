/////////////////////////////////////////////////////////////////////////////
// Name:        wx/barbutton.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BARBUTTON_H_BASE_
#define _WX_BARBUTTON_H_BASE_

#include "wx/bitmap.h"


/**
 The Button styles supported by wxBarButton.
 */

enum {
    wxBBU_PLAIN=0x0800,
    wxBBU_BORDERED=0x1000,
    wxBBU_DONE=0x2000,
    wxBBU_BACK=0x4000
};

/*
 Bar button or tab bar item. For internal use only.
 */

enum {
    wxMO_BARBUTTON = 1,
    wxMO_TABBUTTON = 2
};

/**
 @class wxBarButton
 
 A button class used by wxNavigationBar, wxNavigationCtrl, wxToolBar,
 wxTabCtrl, and wxSegmentedCtrl.
 
 For most of these classes, you don't need to use wxBarButton directly;
 it's a wxbile implementation detail, and the API hides the use of this class.
 However, if you use custom wxNavigationItem
 objects with wxNavigationBar or wxNavigationCtrl, you may wish
 to create wxBarButton objects explicitly.
 
 A bar button can have the styles:
 
 @li wxBBU_PLAIN: has no border
 @li wxBBU_BORDERED: has a border
 @li wxBBU_DONE: uses a different colour to denote a 'Done' button
 @li wxBBU_BACK: uses an arrow representation for a 'Back' button
 
 Note that wxBarButton isn't derived from wxWindow in the wxbile implementation,
 but it has many of the same functions.
 
 @category{wxbile}
 */

class WXDLLIMPEXP_FWD_CORE wxBarButton;

class WXDLLEXPORT wxBarButtonBase : public wxEvtHandler
{
public:
    /// Default constructor
    wxBarButtonBase() { }
    
    /// Copy constructor
    wxBarButtonBase(const wxBarButton& button) { Copy(button); }
    
    /// Constructor taking a string label.
    wxBarButtonBase(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxT("barbutton"));
    
    /// Constructor taking a bitmap label.
    wxBarButtonBase(wxWindow *parent,
                  wxWindowID id,
                  const wxBitmap& bitmap,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxT("barbutton"));
        
    virtual ~wxBarButtonBase();
    
    /// Copies the item.
    virtual void Copy(const wxBarButton& item) = 0;
    
    /// Assignment operator.
    void operator= (const wxBarButton& item) { Copy(item); }
    
    /// Sets the button background colour.
    virtual bool SetBackgroundColour(const wxColour &colour) { m_backgroundColour = colour; return true; }
    
    /// Gets the button background colour.
    virtual wxColour GetBackgroundColour() const { return m_backgroundColour; }
    
    /// Sets the button foreground (text) colour.
    virtual bool SetForegroundColour(const wxColour &colour) { m_foregroundColour = colour; return true; }
    
    /// Gets the button foreground (text) colour.
    virtual wxColour GetForegroundColour() const { return m_foregroundColour; }
    
    /// Sets the selection colour.
    virtual void SetSelectionColour(const wxColour& colour) { m_selectionColour = colour; }
    
    /// Gets the selection colour.
    virtual wxColour GetSelectionColour() const { return m_selectionColour; }
    
    /// Sets the button font.
    virtual void SetFont(const wxFont& font) { m_font = font; }
    
    /// Gets the button font.
    virtual wxFont GetFont() const { return m_font; }
    
    /// Enables the button.
    virtual bool Enable(bool enable) { SetEnabled(enable); return true; }
    
    /// Returns true if the button is enabled.
    virtual bool IsEnabled() const { return GetEnabled(); }
    
    /// Sets the button border colour.
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }
    
    /// Gets the button border colour.
    virtual wxColour GetBorderColour() const { return m_borderColour; }
    
    /// Gets the position and size of the button.
    const wxRect& GetRect() const { return m_rect; }
    
    /// Gets the position and size of the button.
    wxRect& GetRect() { return m_rect; }
    
    /// Gets the size of the button.
    wxSize GetSize() const { return m_rect.GetSize(); }
    
    /// Sets the size of the button.
    void SetSize(const wxSize& sz) { m_rect.SetSize(sz); }
    
    /// Gets the position of the button.
    wxPoint GetPosition() const { return m_rect.GetPosition(); }
    
    /// Sets the position of the button.
    void SetPosition(const wxPoint& pt) { m_rect.SetPosition(pt); }
    
    /// Sets the normal state bitmap.
    void SetNormalBitmap(const wxBitmap& bitmap) { m_normalBitmap = bitmap; }
    
    /// Gets the normal state bitmap.
    wxBitmap& GetNormalBitmap() { return m_normalBitmap; }
    
    /// Gets the normal state bitmap.
    const wxBitmap& GetNormalBitmap() const { return m_normalBitmap; }
    
    /// Sets the disabled state bitmap.
    void SetDisabledBitmap(const wxBitmap& bitmap) { m_disabledBitmap = bitmap; }
    
    /// Gets the disabled state bitmap.
    wxBitmap& GetDisabledBitmap() { return m_disabledBitmap; }
    
    /// Gets the disabled state bitmap.
    const wxBitmap& GetDisabledBitmap() const { return m_disabledBitmap; }
    
    /// Sets the selected state bitmap.
    void SetSelectedBitmap(const wxBitmap& bitmap) { m_selectedBitmap = bitmap; }
    
    /// Gets the selected state bitmap.
    wxBitmap& GetSelectedBitmap() { return m_selectedBitmap; }
    
    /// Gets the selected state bitmap.
    const wxBitmap& GetSelectedBitmap() const { return m_selectedBitmap; }
    
    /// Sets the highlighted state bitmap.
    void SetHighlightedBitmap(const wxBitmap& bitmap) { m_highlightedBitmap = bitmap; }
    
    /// Gets the highlighted state bitmap.
    wxBitmap& GetHighlightedBitmap() { return m_highlightedBitmap; }
    
    /// Gets the highlighted state bitmap.
    const wxBitmap& GetHighlightedBitmap() const { return m_highlightedBitmap; }
    
    /// Sets the text label.
    void SetLabel(const wxString& label) { m_label = label; }
    
    /// Gets the text label.
    const wxString& GetLabel() const { return m_label; }
    
    /// Sets the button badge - text that appears in a circle on top of the
    /// button.
    void SetBadge(const wxString& badge) { m_badge = badge; }
    
    /// Gets the button badge - text that appears in a circle on top of the
    /// button.
    const wxString& GetBadge() const { return m_badge; }
    
    /// Sets the button identifier.
    void SetId(int id) { m_id = id; }
    
    /// Gets the button identifier.
    int GetId() const { return m_id; }
    
    /// Sets the image identifier (into the image list).
    void SetImageId(int id) { m_imageId = id; }
    
    /// Gets the image identifier (into the image list).
    int GetImageId() const { return m_imageId; }
    
    
    // Not public API
    
    // Sets the horizontal margin between button edge and content.
    void SetMarginX(int margin) { m_marginX = margin; }
    
    // Gets the horizontal margin between button edge and content.
    int GetMarginX() const { return m_marginX; }
    
    // Sets the vertical margin between button edge content.
    void SetMarginY(int margin) { m_marginY = margin; }
    
    // Gets the vertical margin between button edge content.
    int GetMarginY() const { return m_marginY; }
    
    // Sets the enabled flag.
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    // Gets the enabled flag.
    bool GetEnabled() const { return m_enabled; }
    
    // Sets the selected flag.
    void SetSelected(bool selected) { m_selected = selected; }
    
    // Gets the selected flag.
    bool GetSelected() const { return m_selected; }
    
    // Sets the highlighted flag.
    void SetHighlighted(bool hilighted) { m_highlighted = hilighted; }
    
    // Gets the highlighted flag.
    bool GetHighlighted() const { return m_highlighted; }
    
    // Sets the button style.
    void SetStyle(int style) { m_style = style; }
    
    // Gets the button style.
    int GetStyle() const { return m_style; }
    
    // Returns true if the button has the specified flag.
    bool HasFlag(int flag) const { return (m_style & flag) == flag; }
    
    // Sets the parent window.
    void SetParent(wxWindow* parent) { m_parent = parent; }
    
    // Gets the parent window.
    wxWindow* GetParent() const { return m_parent; }
    
    /// Is this a stock item? wxbile Implementation only.
    //static bool IsStockId(int id, int barType);
    
    /// Get the stock label for the given identifier. wxbile Implementation only.
    //static wxString GetStockLabel(int id);
    
    /// Get the stock bitmap for the given identifier. wxbile Implementation only.
    //static wxBitmap GetStockBitmap(int id, int barType);
    
    /// Get the stock help string for the given identifier. wxbile Implementation only.
    //static wxString GetStockHelpString(int id);
    
    /// Get a bitmap, such as a PNG, from raw data. wxbile Implementation only.
    //static wxBitmap GetBitmapFromData(const unsigned char *data, int length);
    
    /// Get the best size. wxbile Implementation only.
    //virtual wxSize GetBestButtonSize(wxDC& dc) const;
    
    /// Get the best size.
    virtual wxSize GetBestSize() const { return DoGetBestSize(); }
    
    /// Draw the  button. wxbile Implementation only.
    //void Draw(wxDC& dc, int flags);
    
protected:
    // send a notification event, return true if processed
    //bool SendClickEvent();
    
    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const;
    
    // function called when any of the bitmaps changes
    virtual void OnSetLabel();
    
    void ProcessMouseEvent(wxMouseEvent& event);
    
private:
    
    wxDECLARE_NO_COPY_CLASS(wxBarButtonBase);
    
    wxColour    m_borderColour;
    wxColour    m_backgroundColour;
    wxColour    m_foregroundColour;
    wxColour    m_selectionColour;
    wxFont      m_font;
    
    wxBitmap    m_normalBitmap;
    wxBitmap    m_disabledBitmap;
    wxBitmap    m_selectedBitmap;
    wxBitmap    m_highlightedBitmap;
    wxRect      m_rect;
    int         m_marginX;
    int         m_marginY;
    wxString    m_label;
    wxString    m_badge;
    int         m_id;
    int         m_style;
    bool        m_enabled;
    bool        m_selected;
    bool        m_highlighted;
    int         m_imageId;
    wxWindow*   m_parent;
    
};

WX_DECLARE_OBJARRAY(wxBarButton, wxBarButtonArray);


// ----------------------------------------------------------------------------
// wxBarButton class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/barbutton.h"
#endif


#endif
    // _WX_BARBUTTON_H_BASE_
