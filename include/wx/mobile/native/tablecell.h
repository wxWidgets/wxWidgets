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


#pragma mark wxMoTableCellContentWindow

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


#pragma mark wxMoTableCell

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
    enum wxMoTableCellTextAlignment {
        TextAlignmentLeft,
        TextAlignmentCenter,
        TextAlignmentRight
    };
    
    /// Line break mode
    enum wxMoTableCellLineBreakMode {
        LineBreakModeWordWrap,
        LineBreakModeCharacterWrap,
        LineBreakModeClip,
        LineBreakModeHeadTruncation,
        LineBreakModeTailTruncation,
        LineBreakModeMiddleTruncation
    };
    
    /// Selection style
    enum wxMoTableCellSelectionStyle {
        SelectionStyleNone,
        SelectionStyleBlue,
        SelectionStyleGrey
    };
    
    /// Accessory type
    enum wxMoTableCellAccessoryType {
        AccessoryTypeNone,
        AccessoryTypeDisclosureIndicator,
        AccessoryTypeDetailDisclosureButton,
        AccessoryTypeCheckmark
    };
    
    /// Cell style
    enum wxMoTableCellStyle {
        CellStyleDefault,
        CellStyleValue1,
        CellStyleValue2,
        CellStyleSubtitle
    };
    
    /// Default constructor.
    wxMoTableCell(const wxMoTableCell& cell) { Copy(cell); }
    
    /// Constructor.
    wxMoTableCell(wxMoTableCtrl* ctrl,
                  const wxString& reuseName = wxEmptyString,
                  wxMoTableCellStyle cellStyle = CellStyleDefault);
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
    bool SetFont(const wxFont& font) { m_font = font; return true; }
    
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

    /// Sets the selected detail text colour.
    void SetSelectedDetailTextColour(const wxColour& colour) { m_selectedDetailTextColour = colour; }
    
    /// Gets the selected detail text colour.
    const wxColour& GetSelectedDetailTextColour() const { return m_selectedDetailTextColour; }
    
    /// Sets the text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    void SetTextAlignment(wxMoTableCellTextAlignment alignment) { m_textAlignment = alignment; }
    
    /// Gets the text alignment.
    wxMoTableCellTextAlignment GetTextAlignment() const { return m_textAlignment; }
    
    /// Sets the detail text alignment. Alignment can be TextAlignmentLeft, TextAlignmentCenter,
    /// or TextAlignmentRight.s
    void SetDetailTextAlignment(wxMoTableCellTextAlignment alignment) { m_detailTextAlignment = alignment; }
    
    /// Gets the detail text alignment.
    wxMoTableCellTextAlignment GetDetailTextAlignment() const { return m_detailTextAlignment; }
    
    /// Sets the text line break mode. The mode can be one of
    /// LineBreakModeWordWrap, LineBreakModeCharacterWrap,
    /// LineBreakModeClip, LineBreakModeHeadTruncation,
    /// LineBreakModeTailTruncation, LineBreakModeMiddleTruncation.
    /// This is not currently implemented.
    // FIXME rename this in "generic" too.
    void SetTextLineBreakMode(wxMoTableCellLineBreakMode lineBreakMode) { m_textLineBreakMode = lineBreakMode; }
    
    /// Gets the line break mode.
    wxMoTableCellLineBreakMode GetTextLineBreakMode() const { return m_textLineBreakMode; }

    /// Sets the detail text line break mode. The mode can be one of
    /// LineBreakModeWordWrap, LineBreakModeCharacterWrap,
    /// LineBreakModeClip, LineBreakModeHeadTruncation,
    /// LineBreakModeTailTruncation, LineBreakModeMiddleTruncation.
    /// This is not currently implemented.
    // FIXME add this in "generic" too.
    void SetDetailTextLineBreakMode(wxMoTableCellLineBreakMode lineBreakMode) { m_detailTextLineBreakMode = lineBreakMode; }
    
    /// Gets the line break mode.
    wxMoTableCellLineBreakMode GetDetailTextLineBreakMode() const { return m_detailTextLineBreakMode; }
    
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
    void SetSelectionStyle(wxMoTableCellSelectionStyle style) { m_selectionStyle = style; }
    
    /// Gets the selection style.
    wxMoTableCellSelectionStyle GetSelectionStyle() const { return m_selectionStyle; }
    
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
    void SetIndentationLevel(int indentationLevel);
    
    /// Gets the indentation level.
    int GetIndentationLevel() const;
    
    /// Sets the indentation width.
    void SetIndentationWidth(int indentationWidth);
    
    /// Gets the indentation width.
    int GetIndentationWidth() const;
    
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
    void SetAccessoryType(wxMoTableCellAccessoryType accessoryType) { m_accessoryType = accessoryType; }
    
    /// Gets the accessor type.
    wxMoTableCellAccessoryType GetAccessoryType() const { return m_accessoryType; }
    
    /// Sets the window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    void SetAccessoryWindow(wxWindow* win);
    
    /// Gets the custom accessory window.
    wxWindow* GetAccessoryWindow() const;
    
    /// Returns true if this cell has an accessory.
    bool HasAccessory() const { return (GetAccessoryType() != AccessoryTypeNone); }
    
    /// Sets the editing accessory type. This can be AccessoryTypeNone, AccessoryTypeDisclosureIndicator,
    /// AccessoryTypeDetailDisclosureButton, or AccessoryTypeCheckmark.
    /// The accessory is the image or button that appears on the right of the row.
    void SetEditingAccessoryType(wxMoTableCellAccessoryType accessoryType) { m_editingAccessoryType = accessoryType; }
    
    /// Gets the accessor type.
    wxMoTableCellAccessoryType GetEditingAccessoryType() const { return m_editingAccessoryType; }
    
    /// Sets the editing window to be shown in place of a standard accessory.
    /// If there is already an accessory window, it is destroyed.
    void SetEditingAccessoryWindow(wxWindow* win);
    
    /// Gets the custom editing accessory window.
    wxWindow* GetEditingAccessoryWindow() const { return m_editingAccessoryWindow; }
    
    /// Returns true if this cell has an editing accessory.
    bool HasEditingAccessory() const { return (GetEditingAccessoryType() != AccessoryTypeNone); }
    
    /// There is no SetCellStyle() because the style for the underlying UITableViewCell can't be changed
    /// once the cell has been created.
    
    /// Gets the cell style.
    wxMoTableCellStyle GetCellStyle() const { return m_cellStyle; }
    
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
    
    // Variants of SetPeer()/GetPeer()
    void                SetCellWidgetImpl(wxOSXWidgetImpl* widgetImpl) { m_widgetImpl = widgetImpl; }
    wxOSXWidgetImpl*    GetCellWidgetImpl() { return m_widgetImpl; }
    
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
    wxMoTableCellTextAlignment  m_textAlignment;
    wxMoTableCellTextAlignment  m_detailTextAlignment;
    wxMoTableCellLineBreakMode  m_textLineBreakMode;
    wxMoTableCellLineBreakMode  m_detailTextLineBreakMode;
    wxBitmap                    m_bitmap;
    wxBitmap                    m_selectedBitmap;
    bool                        m_selected;
    wxMoTableCellSelectionStyle m_selectionStyle;
    //wxEvtHandler*             m_eventHandler;
    wxMoTableCellAccessoryType  m_accessoryType;
    wxMoTableCellAccessoryType  m_editingAccessoryType;
    //int                       m_editStyle;
    int                         m_indentationLevel;
    int                         m_indentationWidth;
    int                         m_detailWidth;
    wxMoTableCell::wxMoTableCellStyle          m_cellStyle;
    bool                        m_editingMode;
    bool                        m_shouldIndentWhileEditing;
    bool                        m_showReorderingButton;
    wxMoTableCellContentWindow* m_contentWindow;
    wxWindow*                   m_accessoryWindow;
    wxWindow*                   m_editingAccessoryWindow;
    
    DECLARE_CLASS(wxMoTableCell)
    
    
private:
    wxOSXWidgetImpl*    m_widgetImpl;
    
};

WX_DEFINE_ARRAY_PTR(wxMoTableCell*, wxMoTableCellArray);

#endif
    // _WX_MOBILE_NATIVE_TABLECELL_H_
