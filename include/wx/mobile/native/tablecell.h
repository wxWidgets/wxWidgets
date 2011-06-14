/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/tablecell.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_TABLECELL_H_
#define _WX_MOBILE_NATIVE_TABLECELL_H_

#include "wx/dynarray.h"

#include "wx/mobile/native/vscroll.h"

class WXDLLEXPORT wxMoTableCell;
class WXDLLEXPORT wxMoTableCtrl;

class WXDLLEXPORT wxMoTableCellContentWindow: public wxWindow
{
public:
    wxMoTableCellContentWindow() { Init(); }
    wxMoTableCellContentWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
                               const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxDefaultSize,
                               long style = 0)
    {
        Create(parent, id, pos, sz, style);
    }
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxDefaultSize,
                long style = 0);
    
    void Init();
    
    void SetCell(wxMoTableCell* cell) { m_cell = cell; }
    wxMoTableCell* GetCell() const { return m_cell; }
    
    void SetTableCtrl(wxMoTableCtrl* ctrl) { m_tableCtrl = ctrl; }
    wxMoTableCtrl* GetTableCtrl() const { return m_tableCtrl; }
    
protected:
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvents(wxMouseEvent& event);
    
    wxMoTableCell*  m_cell;
    wxMoTableCtrl*  m_tableCtrl;
    
    DECLARE_DYNAMIC_CLASS(wxMoTableCellContentWindow)
    DECLARE_EVENT_TABLE()
};

/**
 @class wxMoTableCell
 
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
 returned by wxMoTableCell::GetContentWindow, or by calling
 wxMoTableCell::SetAccessoryWindow to show a control instead of a standard accessory.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoTableCell: public wxObject
{
public:
    /// Text alignment
    enum { TextAlignmentLeft, TextAlignmentCenter, TextAlignmentRight };
    
    /// Line break mode
    enum { LineBreakModeWordWrap,
        LineBreakModeCharacterWrap,
        LineBreakModeClip,
        LineBreakModeHeadTruncation,
        LineBreakModeTailTruncation,
        LineBreakModeMiddleTruncation };
    
    /// Selection style
    enum { SelectionStyleNone, SelectionStyleBlue, SelectionStyleGrey };
    
    /// Accessory type
    enum { AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
        AccessoryTypeDetailDisclosureButton, AccessoryTypeCheckmark };
    
    /// Cell style
    enum { CellStyleDefault, CellStyleValue1, CellStyleValue2, CellStyleSubtitle };
    
    /// Default constructor.
    wxMoTableCell(const wxMoTableCell& cell) { Copy(cell); }
    
    /// Constructor.
    wxMoTableCell(wxMoTableCtrl* ctrl, const wxString& reuseName = wxEmptyString, int cellStyle = CellStyleDefault);
    ~wxMoTableCell();
    
    void Init();
    
    // Assignment operato
    void operator=(const wxMoTableCell& cell) { Copy(cell); }
    
    /// Copies the cell.
    void Copy(const wxMoTableCell& cell);
    
    /// Prepares the cell for reuse
    virtual void PrepareForReuse(wxMoTableCtrl* tableCtrl);
    
    /// Creates the content window.
    virtual bool CreateContentWindow(wxMoTableCtrl* ctrl);
    
    /// Sets the name used to identify this type of cell for reuse.
    void SetReuseName(const wxString& name) { m_reuseName = name; }
    
    /// Gets the name used to identify this type of cell for reuse.
    const wxString& GetReuseName() const { return m_reuseName; }
    
    /// Sets the cell font
    void SetFont(const wxFont& font) { m_font = font; }
    
    /// Gets the cell font
    wxFont GetFont() const { return m_font; }
    
    /// Sets the cell detail font
    void SetDetailFont(const wxFont& font) { m_detailFont = font; }
    
    /// Gets the cell detail font
    wxFont GetDetailFont() const { return m_detailFont; }
    
    /// Sets the main text for the cell.
    void SetText(const wxString& text) { m_text = text; }
    
    /// Gets the main text for the cell.
    const wxString& GetText() const { return m_text; }
    
    /// Sets the detail text for the cell.
    void SetDetailText(const wxString& text) { m_detailText = text; }
    
    /// Gets the detail text for the cell.
    const wxString& GetDetailText() const { return m_detailText; }
    
    /// Sets the text colour.
    void SetTextColour(const wxColour& colour) { m_textColour = colour; }
    
    /// Gets the text colour.
    const wxColour& GetTextColour() const { return m_textColour; }
    
    /// Sets the detail text colour.
    void SetDetailTextColour(const wxColour& colour) { m_detailTextColour = colour; }
    
    /// Gets the text colour.
    const wxColour& GetDetailTextColour() const { return m_detailTextColour; }
    
    /// Sets the selected text colour.
    void SetSelectedTextColour(const wxColour& colour) { m_selectedTextColour = colour; }
    
    /// Gets the selected text colour.
    const wxColour& GetSelectedTextColour() const { return m_selectedTextColour; }
    
    /// Sets the text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    void SetTextAlignment(int alignment) { m_textAlignment = alignment; }
    
    /// Gets the text alignment.
    int GetTextAlignment() const { return m_textAlignment; }
    
    /// Sets the detail text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    void SetDetailTextAlignment(int alignment) { m_detailTextAlignment = alignment; }
    
    /// Gets the detail text alignment.
    int GetDetailTextAlignment() const { return m_detailTextAlignment; }
    
    /// Sets the line break mode. The mode can be one of
    /// LineBreakModeWordWrap, LineBreakModeCharacterWrap,
    /// LineBreakModeClip, LineBreakModeHeadTruncation,
    /// LineBreakModeTailTruncation, LineBreakModeMiddleTruncation.
    /// This is not currently implemented.
    void SetLineBreakMode(int lineBreakMode) { m_lineBreakMode = lineBreakMode; }
    
    /// Gets the line break mode.
    int GetLineBreakMode() const { return m_lineBreakMode; }
    
    /// Sets the bitmap on the left side of the cell.
    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    
    /// Returns the bitmap on the left side of the cell.
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    
    /// Sets the bitmap for the selected state.
    void SetSelectedBitmap(const wxBitmap& bitmap) { m_selectedBitmap = bitmap; }
    
    /// Gets the bitmap for the selected state.
    const wxBitmap& GetSelectedBitmap() const { return m_selectedBitmap; }
    
    /// Sets the selected state.
    void SetSelected(bool selected) { m_selected = selected; }
    
    /// Gets the selected state.
    bool GetSelected() const { return m_selected; }
    
    /// Sets the selection style: SelectionStyleNone, SelectionStyleBlue, or SelectionStyleGrey.
    void SetSelectionStyle(int style) { m_selectionStyle = style; }
    
    /// Gets the selection style.
    int GetSelectionStyle() const { return m_selectionStyle; }
    
    // Sets the event handler for the cell.
    //void SetEventHandler(wxEvtHandler* handler) { m_eventHandler = handler; }
    
    // Gets the event handler for the cell.
    //wxEvtHandler* GetEventHandler() const { return m_eventHandler; }
    
    // Sets the edit style. This can be EditStyleNone,
    // EditStyleShowDeleteButton, or
    // EditStyleShowInsertButton.
    // void SetEditStyle(int editStyle) { m_editStyle = editStyle; }
    
    // Gets the edit style.
    // int GetEditStyle() const { return m_editStyle; }
    
    /// Sets editing mode (not yet implemented).
    bool SetEditingMode(bool editingMode, bool animated = true);
    
    /// Returns true if in edit mode (not implemented).
    bool IsEditing() const { return m_editingMode; }
    
    /// Sets the indentation level.
    void SetIndentationLevel(int indentationLevel) { m_indentationLevel = indentationLevel; }
    
    /// Gets the indentation level.
    int GetIndentationLevel() const { return m_indentationLevel; }
    
    /// Sets the indentation width.
    void SetIndentationWidth(int indentationWidth) { m_indentationWidth = indentationWidth; }
    
    /// Gets the indentation width.
    int GetIndentationWidth() const { return m_indentationWidth; }
    
    /// Sets the width of the detail part of the cell.
    /// This isn't part of the Cocoa Touch API - need to find
    /// how this dimension is found.
    void SetDetailWidth(int width) { m_detailWidth = width; }
    int GetDetailWidth() const { return m_detailWidth; }
    
    /// Sets the content window for the cell.
    void SetContentWindow(wxMoTableCellContentWindow* win) { m_contentWindow = win; }
    
    /// Gets the content window for the cell.
    wxMoTableCellContentWindow* GetContentWindow() const { return m_contentWindow; }
    
    /// Sets the accessory type. This can be AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
    /// AccessoryTypeDetailDisclosureButton, or AccessoryTypeCheckmark.
    /// The accessory is the image or button that appears on the right of the row.
    void SetAccessoryType(int accessoryType) { m_accessoryType = accessoryType; }
    
    /// Gets the accessor type.
    int GetAccessoryType() const { return m_accessoryType; }
    
    /// Sets the window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    void SetAccessoryWindow(wxWindow* win);
    
    /// Gets the custom accessory window.
    wxWindow* GetAccessoryWindow() const { return m_accessoryWindow; }
    
    /// Returns true if this cell has an accessory.
    bool HasAccessory() const { return (GetAccessoryType() != AccessoryTypeNone); }
    
    /// Sets the editing accessory type. This can be AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
    /// AccessoryTypeDetailDisclosureButton, or AccessoryTypeCheckmark.
    /// The accessory is the image or button that appears on the right of the row.
    void SetEditingAccessoryType(int accessoryType) { m_editingAccessoryType = accessoryType; }
    
    /// Gets the accessor type.
    int GetEditingAccessoryType() const { return m_editingAccessoryType; }
    
    /// Sets the editing window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    void SetEditingAccessoryWindow(wxWindow* win);
    
    /// Gets the custom editing accessory window.
    wxWindow* GetEditingAccessoryWindow() const { return m_editingAccessoryWindow; }
    
    /// Returns true if this cell has an editing accessory.
    bool HasEditingAccessory() const { return (GetEditingAccessoryType() != AccessoryTypeNone); }
    
    /// Sets the cell style. The style can be one of CellStyleDefault, CellStyleValue1,
    /// CellStyleValue2, and CellStyleSubtitle.
    void SetCellStyle(int style) { m_cellStyle = style; }
    
    /// Gets the cell style.
    int GetCellStyle() const { return m_cellStyle; }
    
    /// Determines whether the cell should indent during editing.
    void SetIndentWhileEditing(bool indent) { m_shouldIndentWhileEditing = indent; }
    
    /// Returns true if the cell should indent during editing.
    bool GetIndentWhileEditing() const { return m_shouldIndentWhileEditing; }
    
    /// Determines whether the row shows a reordering button in editing mode.
    void SetShowReorderingControl(bool show) { m_showReorderingButton = show; }
    
    /// Returns true if the shows a reordering button in editing mode.
    bool GetShowReorderingControl() const { return m_showReorderingButton; }
    
    /// Is the delete confirmation button showing for this path?
    bool IsDeleteButtonShowing(wxMoTableCtrl* tableCtrl) const;
    
protected:
    
    wxString        m_reuseName;
    wxFont          m_font;
    wxFont          m_detailFont;
    wxString        m_text;
    wxString        m_detailText;
    wxColour        m_textColour;
    wxColour        m_detailTextColour;
    wxColour        m_selectedTextColour;
    int             m_textAlignment;
    int             m_detailTextAlignment;
    int             m_lineBreakMode;
    wxBitmap        m_bitmap;
    wxBitmap        m_selectedBitmap;
    bool            m_selected;
    int             m_selectionStyle;
    //wxEvtHandler*   m_eventHandler;
    int             m_accessoryType;
    int             m_editingAccessoryType;
    //int             m_editStyle;
    int             m_indentationLevel;
    int             m_indentationWidth;
    int             m_detailWidth;
    int             m_cellStyle;
    bool            m_editingMode;
    bool            m_shouldIndentWhileEditing;
    bool            m_showReorderingButton;
    wxMoTableCellContentWindow* m_contentWindow;
    wxWindow*       m_accessoryWindow;
    wxWindow*       m_editingAccessoryWindow;
    
    DECLARE_CLASS(wxMoTableCell)
};

WX_DEFINE_ARRAY_PTR(wxMoTableCell*, wxMoTableCellArray);

#endif
    // _WX_MOBILE_NATIVE_TABLECELL_H_
