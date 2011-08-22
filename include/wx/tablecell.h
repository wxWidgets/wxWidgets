/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tablecell.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABLECELL_H_BASE_
#define _WX_TABLECELL_H_BASE_

#include "wx/dynarray.h"

#include "wx/vscroll.h"

class WXDLLIMPEXP_FWD_BASE wxTableCellContentWindow;
class WXDLLEXPORT wxTableCell;
class WXDLLEXPORT wxTableCtrl;


#pragma mark wxTableCellContentWindow

class WXDLLEXPORT wxTableCellContentWindowBase: public wxWindow
{
public:
    wxTableCellContentWindowBase() { }
    wxTableCellContentWindowBase(wxWindow* parent,
                                 wxWindowID id = wxID_ANY,
                                 const wxPoint& pos = wxDefaultPosition,
                                 const wxSize& sz = wxDefaultSize,
                                 long style = 0);
    
    void SetCell(wxTableCell* cell) { m_cell = cell; }
    wxTableCell* GetCell() const { return m_cell; }
    
    void SetTableCtrl(wxTableCtrl* ctrl) { m_tableCtrl = ctrl; }
    wxTableCtrl* GetTableCtrl() const { return m_tableCtrl; }
    
protected:    
    wxTableCell*  m_cell;
    wxTableCtrl*  m_tableCtrl;
    
    //DECLARE_DYNAMIC_CLASS(wxTableCellContentWindow)
    //DECLARE_EVENT_TABLE()
    
    wxDECLARE_NO_COPY_CLASS(wxTableCellContentWindowBase);
};


#pragma mark wxTableCell

/**
 @class wxTableCell
 
 Represents a cell (row) in a table.
 
 A cell can have text label, and optionally a detail label.
 How the label and detail label are presented is determined by the
 cell style, as follows.
 
 @li CellStyleDefault: only the main text label is shown.
 @li CellStyleValue1: a style for a cell with a label on the left side of the cell
 with left-aligned and black text; on the right side is a label
 that has smaller blue text and is right-aligned. The Settings
 application uses cells in this style.
 @li CellStyleValue2: a style for a cell with a label on the left side of the cell with
 text that is right-aligned and blue; on the right side of the cell
 is another label with larger text that is
 left-aligned and black.
 The Phone/Contacts application uses cells in this style.
 @li CellStyleSubtitle: a style for a cell with a left-aligned label across the top and a
 left-aligned label below it in smaller gray text. The Music application
 uses cells in this style.
 
 You may customize the cell further either by placing child windows on the window
 returned by wxTableCell::GetContentWindow, or by calling
 wxTableCell::SetAccessoryWindow to show a control instead of a standard accessory.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxTableCellBase: public wxObject
{
public:
    /// Text alignment
    enum wxTableCellTextAlignment {
        TextAlignmentLeft,
        TextAlignmentCenter,
        TextAlignmentRight
    };
    
    /// Line break mode
    enum wxTableCellLineBreakMode {
        LineBreakModeWordWrap,
        LineBreakModeCharacterWrap,
        LineBreakModeClip,
        LineBreakModeHeadTruncation,
        LineBreakModeTailTruncation,
        LineBreakModeMiddleTruncation
    };
    
    /// Selection style
    enum wxTableCellSelectionStyle {
        SelectionStyleNone,
        SelectionStyleBlue,
        SelectionStyleGrey
    };
    
    /// Accessory type
    enum wxTableCellAccessoryType {
        AccessoryTypeNone,
        AccessoryTypeDisclosureIndicator,
        AccessoryTypeDetailDisclosureButton,
        AccessoryTypeCheckmark
    };
    
    /// Cell style
    enum wxTableCellStyle {
        CellStyleDefault,
        CellStyleValue1,
        CellStyleValue2,
        CellStyleSubtitle
    };
    
    /// Cell edit style
    enum wxTableCellEditStyle {
        EditStyleNone,
        EditStyleShowDeleteButton,
        EditStyleShowInsertButton
    };
    
    
    /// Default constructor.
    wxTableCellBase() { }
    wxTableCellBase(const wxTableCell& cell) { Copy(cell); }
    
    /// Constructor.
    wxTableCellBase(wxTableCtrl* ctrl,
                    const wxString& reuseName = wxEmptyString,
                    wxTableCellStyle cellStyle = CellStyleDefault) { }
    virtual ~wxTableCellBase() { }
    
    // Assignment operato
    void operator=(const wxTableCell& cell) { Copy(cell); }
    
    /// Copies the cell.
    virtual void Copy(const wxTableCell& cell) = 0;
    
    /// Prepares the cell for reuse
    virtual void PrepareForReuse(wxTableCtrl* tableCtrl) = 0;
    
    /// Creates the content window.
    virtual bool CreateContentWindow(wxTableCtrl* ctrl) = 0;
    
    /// Sets the name used to identify this type of cell for reuse.
    virtual void SetReuseName(const wxString& name) { m_reuseName = name; }
    
    /// Gets the name used to identify this type of cell for reuse.
    virtual const wxString& GetReuseName() const { return m_reuseName; }
        
    /// Sets the main text for the cell.
    virtual void SetText(const wxString& text) { m_text = text; }
    
    /// Gets the main text for the cell.
    virtual const wxString& GetText() const { return m_text; }
    
    /// Sets the detail text for the cell.
    virtual void SetDetailText(const wxString& text) { m_detailText = text; }
    
    /// Gets the detail text for the cell.
    virtual const wxString& GetDetailText() const { return m_detailText; }
    
    /// Sets the cell font
    virtual void SetTextFont(const wxFont& font) { m_font = font; }
    
    /// Gets the cell font
    virtual wxFont GetTextFont() const { return m_font; }
    
    /// Sets the cell detail font
    virtual void SetDetailTextFont(const wxFont& font) { m_detailFont = font; }
    
    /// Gets the cell detail font
    virtual wxFont GetDetailTextFont() const { return m_detailFont; }
    
    /// Sets the text colour.
    virtual void SetTextColour(const wxColour& colour) { m_textColour = colour; }
    
    /// Gets the text colour.
    virtual const wxColour& GetTextColour() const { return m_textColour; }
    
    /// Sets the detail text colour.
    virtual void SetDetailTextColour(const wxColour& colour) { m_detailTextColour = colour; }
    
    /// Gets the text colour.
    virtual const wxColour& GetDetailTextColour() const { return m_detailTextColour; }
    
    /// Sets the selected text colour.
    virtual void SetSelectedTextColour(const wxColour& colour) { m_selectedTextColour = colour; }
    
    /// Gets the selected text colour.
    virtual const wxColour& GetSelectedTextColour() const { return m_selectedTextColour; }

    /// Sets the selected detail text colour.
    virtual void SetSelectedDetailTextColour(const wxColour& colour) { m_selectedDetailTextColour = colour; }
    
    /// Gets the selected detail text colour.
    virtual const wxColour& GetSelectedDetailTextColour() const { return m_selectedDetailTextColour; }
    
    /// Sets the text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    virtual void SetTextAlignment(wxTableCellTextAlignment alignment) { m_textAlignment = alignment; }
    
    /// Gets the text alignment.
    virtual wxTableCellTextAlignment GetTextAlignment() const { return m_textAlignment; }
    
    /// Sets the detail text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    virtual void SetDetailTextAlignment(wxTableCellTextAlignment alignment) { m_detailTextAlignment = alignment; }
    
    /// Gets the detail text alignment.
    virtual wxTableCellTextAlignment GetDetailTextAlignment() const { return m_detailTextAlignment; }
    
    /// Sets the text line break mode. The mode can be one of
    /// LineBreakModeWordWrap, LineBreakModeCharacterWrap,
    /// LineBreakModeClip, LineBreakModeHeadTruncation,
    /// LineBreakModeTailTruncation, LineBreakModeMiddleTruncation.
    /// This is not currently implemented.
    // FIXME rename this in "generic" too.
    virtual void SetTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode) { m_textLineBreakMode = lineBreakMode; }
    
    /// Gets the line break mode.
    virtual wxTableCellLineBreakMode GetTextLineBreakMode() const { return m_textLineBreakMode; }

    /// Sets the detail text line break mode. The mode can be one of
    /// LineBreakModeWordWrap, LineBreakModeCharacterWrap,
    /// LineBreakModeClip, LineBreakModeHeadTruncation,
    /// LineBreakModeTailTruncation, LineBreakModeMiddleTruncation.
    /// This is not currently implemented.
    // FIXME add this in "generic" too.
    virtual void SetDetailTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode) { m_detailTextLineBreakMode = lineBreakMode; }
    
    /// Gets the line break mode.
    virtual wxTableCellLineBreakMode GetDetailTextLineBreakMode() const { return m_detailTextLineBreakMode; }
    
    /// Sets the bitmap on the left side of the cell.
    virtual void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    
    /// Returns the bitmap on the left side of the cell.
    virtual const wxBitmap& GetBitmap() const { return m_bitmap; }
    
    /// Sets the bitmap for the selected state.
    virtual void SetSelectedBitmap(const wxBitmap& bitmap) { m_selectedBitmap = bitmap; }
    
    /// Gets the bitmap for the selected state.
    virtual const wxBitmap& GetSelectedBitmap() const { return m_selectedBitmap; }
    
    /// Sets the selected state.
    virtual void SetSelected(bool selected) { m_selected = selected; }
    
    /// Gets the selected state.
    virtual bool GetSelected() const { return m_selected; }
    
    /// Sets the selection style: SelectionStyleNone, SelectionStyleBlue, or SelectionStyleGrey.
    virtual void SetSelectionStyle(wxTableCellSelectionStyle style) { m_selectionStyle = style; }
    
    /// Gets the selection style.
    virtual wxTableCellSelectionStyle GetSelectionStyle() const { return m_selectionStyle; }
    
    // Sets the event handler for the cell.
    //void SetEventHandler(wxEvtHandler* handler) { m_eventHandler = handler; }
    
    // Gets the event handler for the cell.
    //wxEvtHandler* GetEventHandler() const { return m_eventHandler; }
        
    // Gets the edit style.
    wxTableCellEditStyle GetEditStyle() const { return EditStyleNone; }
    
    /// Sets editing mode (not yet implemented).
    virtual bool SetEditingMode(bool editingMode, bool animated = true) { m_editingMode = editingMode; }
    
    /// Returns true if in edit mode (not implemented).
    virtual bool IsEditing() const { return m_editingMode; }
    
    /// Sets the indentation level.
    virtual void SetIndentationLevel(int indentationLevel) { m_indentationLevel = indentationLevel; }
    
    /// Gets the indentation level.
    virtual int GetIndentationLevel() const { return m_indentationLevel; }
    
    /// Sets the indentation width.
    virtual void SetIndentationWidth(int indentationWidth) { m_indentationWidth = indentationWidth; }
    
    /// Gets the indentation width.
    virtual int GetIndentationWidth() const { return m_indentationWidth; }
    
    /// Sets the width of the detail part of the cell.
    /// This isn't part of the Cocoa Touch API - need to find
    /// how this dimension is found.
    virtual void SetDetailWidth(float width) { m_detailWidth = width; }
    virtual float GetDetailWidth() const { return m_detailWidth; }
    
    /// Sets the content window for the cell.
    virtual void SetContentWindow(wxTableCellContentWindow* win) { m_contentWindow = win; }
    
    /// Gets the content window for the cell.
    virtual wxTableCellContentWindow* GetContentWindow() const { return m_contentWindow; }
    
    /// Sets the accessory type. This can be AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
    /// AccessoryTypeDetailDisclosureButton, or AccessoryTypeCheckmark.
    /// The accessory is the image or button that appears on the right of the row.
    virtual void SetAccessoryType(wxTableCellAccessoryType accessoryType) { m_accessoryType = accessoryType; }
    
    /// Gets the accessor type.
    virtual wxTableCellAccessoryType GetAccessoryType() const { return m_accessoryType; }
    
    /// Sets the window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    virtual void SetAccessoryWindow(wxWindow* win) { m_accessoryWindow = win; }
    
    /// Gets the custom accessory window.
    virtual wxWindow* GetAccessoryWindow() const { return m_accessoryWindow; }
    
    /// Returns true if this cell has an accessory.
    virtual bool HasAccessory() const { return (GetAccessoryType() != AccessoryTypeNone); }
    
    /// Sets the editing accessory type. This can be AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
    /// AccessoryTypeDetailDisclosureButton, or AccessoryTypeCheckmark.
    /// The accessory is the image or button that appears on the right of the row.
    virtual void SetEditingAccessoryType(wxTableCellAccessoryType accessoryType) { m_editingAccessoryType = accessoryType; }
    
    /// Gets the accessor type.
    virtual wxTableCellAccessoryType GetEditingAccessoryType() const { return m_editingAccessoryType; }
    
    /// Sets the editing window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    virtual void SetEditingAccessoryWindow(wxWindow* win) { m_editingAccessoryWindow = win; }
    
    /// Gets the custom editing accessory window.
    virtual wxWindow* GetEditingAccessoryWindow() const { return m_editingAccessoryWindow; }
    
    /// Returns true if this cell has an editing accessory.
    virtual bool HasEditingAccessory() const { return (GetEditingAccessoryType() != AccessoryTypeNone); }
    
    /// There is no SetCellStyle() because the style for the underlying UITableViewCell can't be changed
    /// once the cell has been created.
    
    /// Gets the cell style.
    virtual wxTableCellStyle GetCellStyle() const { return m_cellStyle; }
    
    /// Determines whether the cell should indent during editing.
    virtual void SetIndentWhileEditing(bool indent) { m_shouldIndentWhileEditing = indent; }
    
    /// Returns true if the cell should indent during editing.
    virtual bool GetIndentWhileEditing() const { return m_shouldIndentWhileEditing; }
    
    /// Determines whether the row shows a reordering button in editing mode.
    virtual void SetShowReorderingControl(bool show) { m_showReorderingButton = show; }
    
    /// Returns true if the shows a reordering button in editing mode.
    virtual bool GetShowReorderingControl() const { return m_showReorderingButton; }
    
    /// Is the delete confirmation button showing for this path?
    virtual bool IsDeleteButtonShowing(wxTableCtrl* tableCtrl) const { return false; }
        
protected:
    
    wxString                    m_reuseName;
    wxFont                      m_font;
    wxFont                      m_detailFont;
    wxString                    m_text;
    wxString                    m_detailText;
    wxColour                    m_textColour;
    wxColour                    m_detailTextColour;
    wxColour                    m_selectedTextColour;
    wxColour                    m_selectedDetailTextColour; // FIXME add to "generic" too
    wxTableCellTextAlignment  m_textAlignment;
    wxTableCellTextAlignment  m_detailTextAlignment;
    wxTableCellLineBreakMode  m_textLineBreakMode;
    wxTableCellLineBreakMode  m_detailTextLineBreakMode;
    wxBitmap                    m_bitmap;
    wxBitmap                    m_selectedBitmap;
    bool                        m_selected;
    wxTableCellSelectionStyle m_selectionStyle;
    //wxEvtHandler*             m_eventHandler;
    wxTableCellAccessoryType  m_accessoryType;
    wxTableCellAccessoryType  m_editingAccessoryType;
    int                         m_indentationLevel;
    int                         m_indentationWidth;
    float                         m_detailWidth;
    wxTableCellBase::wxTableCellStyle          m_cellStyle;
    bool                        m_editingMode;
    bool                        m_shouldIndentWhileEditing;
    bool                        m_showReorderingButton;
    wxTableCellContentWindow* m_contentWindow;
    wxWindow*                   m_accessoryWindow;
    wxWindow*                   m_editingAccessoryWindow;
    
    wxDECLARE_NO_COPY_CLASS(wxTableCellBase);
    
};

WX_DEFINE_ARRAY_PTR(wxTableCell*, wxTableCellArray);


// ----------------------------------------------------------------------------
// wxTableCell class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/tablecell.h"
#endif


#endif
    // _WX_TABLECELL_H_BASE_
