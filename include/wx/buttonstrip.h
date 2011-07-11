/////////////////////////////////////////////////////////////////////////////
// Name:        wx/buttonstrip.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTONSTRIP_H_BASE_
#define _WX_BUTTONSTRIP_H_BASE_

#include "wx/bitmap.h"
#include "wx/imaglist.h"
#include "wx/dc.h"
#include "wx/dynarray.h"
#include "wx/barbutton.h"

/**
 @class wxButtonStrip
 
 This class is used to implement wxToolBar, wxTabCtrl, and wxSegmentedCtrl.
 
 @category{wxbileImplementation}
 */

class WXDLLEXPORT wxButtonStripBase: public wxObject
{
public:
    typedef enum {
        TabBar,
        ToolBar,
        SegmentedCtrl
    } ButtonStripType;
    
    typedef enum {
        BitmapButton,
        TextButton,
        TextAndBitmapButton
    } ButtonStripFace;
    
    typedef enum {
        NoSelection,
        SingleSelection
    } SelectionStyle;
    
    typedef enum {
        LayoutEvenButtons, // buttons have the same size
        LayoutEvenSeparators, // buttons have differing size, separation is the same
        LayoutStretchingSeparators // buttons have fixed size, but separators determine spacing
    } LayoutStyle;
    
    DECLARE_DYNAMIC_CLASS(wxButtonStrip)
public:
    
    /// Constructor.
    wxButtonStripBase() { }
    
    virtual ~wxButtonStripBase();
    
    /// Returns the bar button array.
    virtual wxBarButtonArray& GetButtonStripArray() { return m_buttonStrip; }
    
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
    
    /// Returns the number of buttons in the strip.
    virtual size_t GetCount() const { return m_buttonStrip.GetCount(); }
    
    /// Returns a pointer to the given button.
    virtual wxBarButton* GetButton(size_t idx) const;
    
    /// Finds the index of the button by its identifier.
    virtual int FindIndexForId(int id);
    
    /// Clears the buttons.
    virtual void Clear();
    
    /// Sets the horizontal margin between the edge of the button and the content.
    virtual void SetButtonMarginX(int margin) { m_buttonMarginX = margin; }
    
    /// Gets the horizontal margin between the edge of the button and the content.
    virtual int GetButtonMarginX() const { return m_buttonMarginX; }
    
    /// Sets the vertical margin between the edge of the button and the content.
    virtual void SetButtonMarginY(int margin) { m_buttonMarginY = margin; }
    
    /// Gets the vertical margin between the edge of the button and the content.
    virtual int GetButtonMarginY() const { return m_buttonMarginY; }
    
    /// Sets the space between the image and label.
    virtual void SetButtonMarginInterItemY(int margin) { m_buttonMarginInterItemY = margin; }
    
    /// Gets the space between the image and label.
    virtual int GetButtonMarginInterItemY() const { return m_buttonMarginInterItemY; }
    
    /// Sets the horizontal spacing between buttons.
    virtual void SetInterButtonSpacing(int spacing) { m_interButtonSpacing = spacing; }
    
    /// Gets the horizontal spacing between buttons.
    virtual int GetInterButtonSpacing() const { return m_interButtonSpacing; }
    
    /// Sets the layout style:
    /// LayoutEvenButtons (buttons have the same size),
    /// LayoutEvenSeparators (buttons have differing size, separation is the same),
    /// or LayoutStretchingSeparators (buttons have fixed size, but separators determine spacing).
    virtual LayoutStyle GetLayoutStyle() const { return m_layoutStyle; }
    virtual void SetLayoutStyle(LayoutStyle style) { m_layoutStyle = style; }
    
    /// Set the margin between left or right side and first or last button.
    virtual void SetEndMargin(int margin) { m_endMargin = margin; }
    
    /// Get the margin between left or right side and first or last button.
    virtual int GetEndMargin() const { return m_endMargin; }
    
    /// Sets the label font.
    virtual void SetLabelFont(const wxFont& font) { m_labelFont = font; }
    
    /// Gets the label font.
    virtual const wxFont& GetLabelFont() const { return m_labelFont; }
    
    /// Sets the label colour.
    virtual void SetLabelColour(const wxColour& colour) { m_labelColour = colour; }
    
    /// Gets the label colour.
    virtual const wxColour& GetLabelColour() const { return m_labelColour; }
    
    /// Sets the button selection colour.
    virtual void SetButtonSelectionColour(const wxColour& colour) { m_buttonSelectionColour = colour; }
    
    /// Gets the button selection colour.
    virtual const wxColour& GetButtonSelectionColour() const { return m_buttonSelectionColour; }
    
    /// Sets the button background colour.
    virtual void SetButtonBackgroundColour(const wxColour& colour) { m_buttonBackgroundColour = colour; }
    
    /// Gets the button background colour.
    virtual const wxColour& GetButtonBackgroundColour() const { return m_buttonBackgroundColour; }
    
    /// Sets the control background colour.
    virtual void SetBackgroundColour(const wxColour& colour) { m_backgroundColour = colour; }
    
    /// Gets the control background colour.
    virtual const wxColour& GetBackgroundColour() const { return m_backgroundColour; }
    
    /// Sets the control border colour.
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }
    
    /// Gets the control border colour.
    virtual wxColour GetBorderColour() const { return m_borderColour; }
    
    /// Returns the image list, if any.
    virtual wxImageList* GetImageList() const { return m_imageList; }
    
    /// Sets the image list: the image will not be deleted when the strip is deleted.
    virtual void SetImageList(wxImageList* imageList) { m_imageList = imageList; m_ownsImageList = false; }
    
    /// Assigns the image list - the image list will be deleted when the strip is deleted.
    virtual void AssignImageList(wxImageList* imageList) { m_imageList = imageList; m_ownsImageList = true; }
    
    /// Sets ownership of the image list - if true, the image list will be deleted when the
    /// strip is deleted.
    virtual void SetOwnsImageList(bool ownsImageList) { m_ownsImageList = ownsImageList; }
    
    /// Returns true if the image list will be deleted when the strip is deleted.
    virtual bool GetOwnsImageList() const { return m_ownsImageList; }
    
    /// Sets the selected button.
    virtual bool SetSelection(int sel);
    
    /// Gets the current selection.
    virtual int GetSelection() const { return m_selection; }
    
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
    
    wxBarButtonArray          m_buttonStrip;
    ButtonStripType             m_controlType;
    ButtonStripFace             m_faceType;
    SelectionStyle              m_selectionStyle;
    int                         m_buttonMarginX; // between content and button edge
    int                         m_buttonMarginY;
    int                         m_buttonMarginInterItemY; // between image and label
    LayoutStyle                 m_layoutStyle;
    int                         m_interButtonSpacing;
    int                         m_endMargin; // margin between left or right side and first or last button
    wxFont                      m_labelFont;
    wxColour                    m_labelColour;
    wxColour                    m_buttonBackgroundColour;
    wxColour                    m_backgroundColour;
    wxColour                    m_borderColour;
    wxColour                    m_buttonSelectionColour;
    wxImageList*                m_imageList;
    bool                        m_ownsImageList;
    int                         m_selection;
    
    // State of highlighted button
    int                         m_buttonState;
    
    // The index of the button for the given state
    int                         m_buttonForState;
    
    wxDECLARE_NO_COPY_CLASS(wxButtonStripBase);
    
};


// ----------------------------------------------------------------------------
// wxButtonStrip class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/buttonstrip.h"
#endif


#endif
    // _WX_BUTTONSTRIP_H_BASE_
