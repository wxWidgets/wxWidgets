/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/buttonstrip.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTONSTRIP_H_
#define _WX_BUTTONSTRIP_H_

/**
 @class wxButtonStrip
 
 @category{wxbileImplementation}
 */

class WXDLLEXPORT wxButtonStrip: public wxButtonStripBase
{
    DECLARE_DYNAMIC_CLASS(wxButtonStrip)
public:
    
    /// Constructor.
    wxButtonStrip() { Init(); }
    virtual ~wxButtonStrip();
    
    /// Draws the strip in the given window and rectangle.
    virtual void Draw(wxWindow* win, const wxRect& rect, wxDC& dc);
    
    /// Draws a single button.
    virtual void DrawButton(wxWindow* win, wxDC& dc, wxBarButton& item, int i);
    
    /// Draws the strip background.
    virtual void DrawBackground(wxWindow* win, wxDC& dc);
    
    /// Returns a button index for the given point.
    virtual int HitTest(const wxPoint& pt) const;
    
    /// Returns a positive integer if a button was pressed, otherwise -1.
    virtual int ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event);
    
    /// Returns the current control type, which changes the way the strip is drawn.
    virtual ButtonStripType GetControlType() { return m_controlType; }
    
    /// Sets the current control type, which changes the way the strip is drawn.
    virtual void SetControlType(ButtonStripType type) { m_controlType = type; }
    
    // Gets the face type. Not currently used but might be used in future if we decide to
    // let the programmer choose between text and bitmaps on toolbars.
    virtual ButtonStripFace GetFaceType() const { return m_faceType; }
    virtual void SetFaceType(ButtonStripFace faceType) { m_faceType = faceType; }
    
    /// Returns the selection style: NoSelection, or SingleSelection.
    virtual SelectionStyle GetSelectionStyle() const { return m_selectionStyle; }
    
    /// Sets the selection style: NoSelection, or SingleSelection.
    virtual void SetSelectionStyle(SelectionStyle selectionStyle) { m_selectionStyle = selectionStyle; }
    
    /// Adds a text button.
    virtual int AddTextButton(wxWindow* parent, int id, const wxString& text, const wxString& badge = wxEmptyString);
    
    /// Adds a bitmap and text button, passing an explicit bitmap.
    virtual int AddBitmapButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);
    
    /// Adds a bitmap index, passing an index into an image list.
    virtual int AddBitmapButton(wxWindow* parent, int id, int bitmapIndex, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);
    
    /// Adds a flexible separator.
    virtual int AddFlexibleSeparator();
    
    /// Adds a fixed separator.
    virtual int AddFixedSeparator(int width);
    
    /// Inserts a text button.
    virtual int InsertTextButton(wxWindow* parent, int id, int insertBefore, const wxString& text, const wxString& badge = wxEmptyString);
    
    /// Inserts a bitmap and text button, passing an explicit bitmap.
    virtual int InsertBitmapButton(wxWindow* parent, int id, int insertBefore, const wxBitmap& bitmap, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);
    
    /// Inserts a bitmap and text button, passing an index into an image list.
    virtual int InsertBitmapButton(wxWindow* parent, int id, int insertBefore, int bitmapIndex, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);
    
    /// Inserts a flexible separator.
    virtual int InsertFlexibleSeparator(int insertBefore);
    
    /// Inserts a fixed separator.
    virtual int InsertFixedSeparator(int insertBefore, int width);
    
    /// Deletes the button by index.
    virtual bool DeleteButton(size_t idx);
    
    /// Sets the button text by index.
    virtual bool SetButtonText(size_t idx, const wxString& text);
    
    /// Sets the button badge by index.
    virtual bool SetButtonBadge(size_t idx, const wxString& badge);
    
    /// Sets the button normal bitmap by index.
    virtual bool SetButtonNormalBitmap(size_t idx, const wxBitmap& bitmap);
    
    /// Sets the button disabled bitmap by index.
    virtual bool SetButtonDisabledBitmap(size_t idx, const wxBitmap& bitmap);
    
    /// Sets the button selected bitmap by index.
    virtual bool SetButtonSelectedBitmap(size_t idx, const wxBitmap& bitmap);
    
    /// Sets the button highlighted bitmap by index.
    virtual bool SetButtonHighlightedBitmap(size_t idx, const wxBitmap& bitmap);
    
    /// Enables the button by index.
    virtual bool EnableButton(size_t idx, bool enable);
    
    /// Returns true if the given button is enabled.
    virtual bool IsButtonEnabled(size_t idx) const;
    
    /// Selects the button by index.
    virtual bool SelectButton(size_t idx, bool selected);
    
    /// Returns true if the given button is selected.
    virtual bool IsButtonSelected(size_t idx) const;
        
    /// Returns a pointer to the given button.
    virtual wxBarButton* GetButton(size_t idx) const;
    
    /// Finds the index of the button by its identifier.
    virtual int FindIndexForId(int id);
    
    /// Clears the buttons.
    virtual void Clear();
            
    /// Sets the selected button.
    virtual bool SetSelection(int sel);
        
    /// Returns the best size for the control.
    virtual wxSize GetBestSize() const;
    
    /// Gets the space taking up by just the buttons.
    virtual wxSize GetTotalButtonSize(wxSize& maxButtonSize, wxSize& totalIncluding) const;
    
    /// Gets the best button size.
    wxSize GetBestButtonSize(wxDC& dc, const wxBarButton& item) const;
    
    /// Computes the button sizes taking into account control size, layout style,
    /// and individual button properties.
    virtual bool ComputeSizes(wxWindow* win);
    
    /// Resizes the content of the control (calls ComputeSizes).
    virtual void DoResize(wxWindow* win);
        
protected:
    
    void Init();    
};

#endif
    // _WX_BUTTONSTRIP_H_
