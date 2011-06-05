/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/buttonstrip.h
// Purpose:     wxMoButtonStrip helper class for tab control,
//              toolbar and segmented control.
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_BUTTONSTRIP_H_
#define _WX_MOBILE_GENERIC_BUTTONSTRIP_H_

#include "wx/bitmap.h"
#include "wx/imaglist.h"
#include "wx/dc.h"
#include "wx/dynarray.h"
#include "wx/mobile/generic/utils.h"
#include "wx/mobile/generic/barbutton.h"

/**
    @class wxMoButtonStrip

    This class is used to implement wxMoToolBar, wxMoTabCtrl, and wxMoSegmentedCtrl.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoButtonStrip: public wxObject
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

    DECLARE_DYNAMIC_CLASS(wxMoButtonStrip)
public:

    /// Constructor.
    wxMoButtonStrip() { Init(); }

    ~wxMoButtonStrip();

    void Init();

    /// Returns the bar button array.
    wxMoBarButtonArray& GetButtonStripArray() { return m_buttonStrip; }

    /// Draws the strip in the given window and rectangle.
    void Draw(wxWindow* win, const wxRect& rect, wxDC& dc);

    /// Draws a single button.
    void DrawButton(wxWindow* win, wxDC& dc, wxMoBarButton& item, int i);

    /// Draws the strip background.
    void DrawBackground(wxWindow* win, wxDC& dc);

    /// Returns a button index for the given point.
    int HitTest(const wxPoint& pt) const;

    /// Returns a positive integer if a button was pressed, otherwise -1.
    int ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event);

    /// Returns the current control type, which changes the way the strip is drawn.
    ButtonStripType GetControlType() { return m_controlType; }

    /// Sets the current control type, which changes the way the strip is drawn.
    void SetControlType(ButtonStripType type) { m_controlType = type; }

    // Gets the face type. Not currently used but might be used in future if we decide to
    // let the programmer choose between text and bitmaps on toolbars.
    ButtonStripFace GetFaceType() const { return m_faceType; }
    void SetFaceType(ButtonStripFace faceType) { m_faceType = faceType; }

    /// Returns the selection style: NoSelection, or SingleSelection.
    SelectionStyle GetSelectionStyle() const { return m_selectionStyle; }

    /// Sets the selection style: NoSelection, or SingleSelection.
    void SetSelectionStyle(SelectionStyle selectionStyle) { m_selectionStyle = selectionStyle; }

    /// Adds a text button.
    int AddTextButton(wxWindow* parent, int id, const wxString& text, const wxString& badge = wxEmptyString);

    /// Adds a bitmap and text button, passing an explicit bitmap.
    int AddBitmapButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);

    /// Adds a bitmap index, passing an index into an image list.
    int AddBitmapButton(wxWindow* parent, int id, int bitmapIndex, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);

    /// Adds a flexible separator.
    int AddFlexibleSeparator();

    /// Adds a fixed separator.
    int AddFixedSeparator(int width);

    /// Inserts a text button.
    int InsertTextButton(wxWindow* parent, int id, int insertBefore, const wxString& text, const wxString& badge = wxEmptyString);

    /// Inserts a bitmap and text button, passing an explicit bitmap.
    int InsertBitmapButton(wxWindow* parent, int id, int insertBefore, const wxBitmap& bitmap, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);

    /// Inserts a bitmap and text button, passing an index into an image list.
    int InsertBitmapButton(wxWindow* parent, int id, int insertBefore, int bitmapIndex, const wxString& text = wxEmptyString, const wxString& badge = wxEmptyString);

    /// Inserts a flexible separator.
    int InsertFlexibleSeparator(int insertBefore);

    /// Inserts a fixed separator.
    int InsertFixedSeparator(int insertBefore, int width);

    /// Deletes the button by index.
    bool DeleteButton(size_t idx);

    /// Sets the button text by index.
    bool SetButtonText(size_t idx, const wxString& text);

    /// Sets the button badge by index.
    bool SetButtonBadge(size_t idx, const wxString& badge);

    /// Sets the button normal bitmap by index.
    bool SetButtonNormalBitmap(size_t idx, const wxBitmap& bitmap);

    /// Sets the button disabled bitmap by index.
    bool SetButtonDisabledBitmap(size_t idx, const wxBitmap& bitmap);

    /// Sets the button selected bitmap by index.
    bool SetButtonSelectedBitmap(size_t idx, const wxBitmap& bitmap);

    /// Sets the button highlighted bitmap by index.
    bool SetButtonHighlightedBitmap(size_t idx, const wxBitmap& bitmap);

    /// Enables the button by index.
    bool EnableButton(size_t idx, bool enable);

    /// Returns true if the given button is enabled.
    bool IsButtonEnabled(size_t idx) const;

    /// Selects the button by index.
    bool SelectButton(size_t idx, bool selected);

    /// Returns true if the given button is selected.
    bool IsButtonSelected(size_t idx) const;

    /// Returns the number of buttons in the strip.
    size_t GetCount() const { return m_buttonStrip.GetCount(); }

    /// Returns a pointer to the given button.
    wxMoBarButton* GetButton(size_t idx) const;

    /// Finds the index of the button by its identifier.
    int FindIndexForId(int id);

    /// Clears the buttons.
    void Clear();

    /// Sets the horizontal margin between the edge of the button and the content.
    void SetButtonMarginX(int margin) { m_buttonMarginX = margin; }

    /// Gets the horizontal margin between the edge of the button and the content.
    int GetButtonMarginX() const { return m_buttonMarginX; }

    /// Sets the vertical margin between the edge of the button and the content.
    void SetButtonMarginY(int margin) { m_buttonMarginY = margin; }

    /// Gets the vertical margin between the edge of the button and the content.
    int GetButtonMarginY() const { return m_buttonMarginY; }

    /// Sets the space between the image and label.
    void SetButtonMarginInterItemY(int margin) { m_buttonMarginInterItemY = margin; }

    /// Gets the space between the image and label.
    int GetButtonMarginInterItemY() const { return m_buttonMarginInterItemY; }

    /// Sets the horizontal spacing between buttons.
    void SetInterButtonSpacing(int spacing) { m_interButtonSpacing = spacing; }

    /// Gets the horizontal spacing between buttons.
    int GetInterButtonSpacing() const { return m_interButtonSpacing; }

    /// Sets the layout style:
    /// LayoutEvenButtons (buttons have the same size),
    /// LayoutEvenSeparators (buttons have differing size, separation is the same),
    /// or LayoutStretchingSeparators (buttons have fixed size, but separators determine spacing).
    LayoutStyle GetLayoutStyle() const { return m_layoutStyle; }
    void SetLayoutStyle(LayoutStyle style) { m_layoutStyle = style; }

    /// Set the margin between left or right side and first or last button.
    void SetEndMargin(int margin) { m_endMargin = margin; }

    /// Get the margin between left or right side and first or last button.
    int GetEndMargin() const { return m_endMargin; }

    /// Sets the label font.
    void SetLabelFont(const wxFont& font) { m_labelFont = font; }

    /// Gets the label font.
    const wxFont& GetLabelFont() const { return m_labelFont; }

    /// Sets the label colour.
    void SetLabelColour(const wxColour& colour) { m_labelColour = colour; }

    /// Gets the label colour.
    const wxColour& GetLabelColour() const { return m_labelColour; }

    /// Sets the button selection colour.
    void SetButtonSelectionColour(const wxColour& colour) { m_buttonSelectionColour = colour; }

    /// Gets the button selection colour.
    const wxColour& GetButtonSelectionColour() const { return m_buttonSelectionColour; }

    /// Sets the button background colour.
    void SetButtonBackgroundColour(const wxColour& colour) { m_buttonBackgroundColour = colour; }

    /// Gets the button background colour.
    const wxColour& GetButtonBackgroundColour() const { return m_buttonBackgroundColour; }

    /// Sets the control background colour.
    void SetBackgroundColour(const wxColour& colour) { m_backgroundColour = colour; }

    /// Gets the control background colour.
    const wxColour& GetBackgroundColour() const { return m_backgroundColour; }

    /// Sets the control border colour.
    virtual void SetBorderColour(const wxColour &colour) { m_borderColour = colour; }

    /// Gets the control border colour.
    virtual wxColour GetBorderColour() const { return m_borderColour; }

    /// Returns the image list, if any.
    wxImageList* GetImageList() const { return m_imageList; }

    /// Sets the image list: the image will not be deleted when the strip is deleted.
    void SetImageList(wxImageList* imageList) { m_imageList = imageList; m_ownsImageList = false; }

    /// Assigns the image list - the image list will be deleted when the strip is deleted.
    void AssignImageList(wxImageList* imageList) { m_imageList = imageList; m_ownsImageList = true; }

    /// Sets ownership of the image list - if true, the image list will be deleted when the
    /// strip is deleted.
    void SetOwnsImageList(bool ownsImageList) { m_ownsImageList = ownsImageList; }

    /// Returns true if the image list will be deleted when the strip is deleted.
    bool GetOwnsImageList() const { return m_ownsImageList; }

    /// Sets the selected button.
    bool SetSelection(int sel);

    /// Gets the current selection.
    int GetSelection() const { return m_selection; }

    /// Returns the best size for the control.
    virtual wxSize GetBestSize() const;

    /// Gets the space taking up by just the buttons.
    virtual wxSize GetTotalButtonSize(wxSize& maxButtonSize, wxSize& totalIncluding) const;

    /// Gets the best button size.
    wxSize GetBestButtonSize(wxDC& dc, const wxMoBarButton& item) const;

    /// Computes the button sizes taking into account control size, layout style,
    /// and individual button properties.
    bool ComputeSizes(wxWindow* win);

    /// Resizes the content of the control (calls ComputeSizes).
    void DoResize(wxWindow* win);

protected:

    wxMoBarButtonArray          m_buttonStrip;
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
};

#endif
    // _WX_MOBILE_GENERIC_BUTTONSTRIP_H_
