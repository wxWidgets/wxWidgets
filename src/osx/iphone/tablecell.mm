/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/tablecell.mm
// Purpose:     wxTableCell implementation
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-02
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/tablecell.h"
#include "wx/tablectrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private/tablecellimpl.h"


#pragma mark -
#pragma mark Cocoa class

@implementation wxUITableViewCell

@synthesize tableCell;

- (id)init {
    if ((self = [super init])) {
        tableCell = NULL;
    }
    
    return self;
}

- (id)initWithWxTableCell:(wxTableCell *)initTableCell {
    
    NSString *cellIdentifier = [NSString stringWithString:wxCFStringRef(initTableCell->GetReuseName()).AsNSString()];
    if (! cellIdentifier) {
        return nil;
    }
    
    // Style
    wxTableCell::wxTableCellStyle cellStyle = initTableCell->GetCellStyle();
    UITableViewCellStyle style;
    switch (cellStyle) {
        case wxTableCell::CellStyleDefault:   style = UITableViewCellStyleDefault;    break;
        case wxTableCell::CellStyleValue1:    style = UITableViewCellStyleValue1;     break;
        case wxTableCell::CellStyleValue2:    style = UITableViewCellStyleValue2;     break;
        case wxTableCell::CellStyleSubtitle:  style = UITableViewCellStyleSubtitle;   break;
        default:                              style = UITableViewCellStyleDefault;    break;
    };
    
    if ((self = [self initWithStyle:style reuseIdentifier:cellIdentifier])) {
        tableCell = initTableCell;
    }
    
    return self;
}

// Reads properties from wxTableCell, stores them to self ("translates" to Cocoa properties)
// Used just before showing the cell
- (void)commitWxTableCellProperties {
    
    wxASSERT_MSG(tableCell, "wxTableCell is unset.");
    if (! tableCell) {
        return;
    }
    
    // Use setters as they set the actual "native" widget properties
    // FIXME do they really need to be re-commited to native widget? I guess the original idea
    // was that these are re-set because a cell might be taken from the reuse pool.
    tableCell->SetText(tableCell->GetText());
    tableCell->SetDetailText(tableCell->GetDetailText());
    tableCell->SetTextFont(tableCell->GetTextFont());
    tableCell->SetDetailTextFont(tableCell->GetDetailTextFont());
    tableCell->SetTextColour(tableCell->GetTextColour());
    tableCell->SetDetailTextColour(tableCell->GetDetailTextColour());
    tableCell->SetSelectedTextColour(tableCell->GetSelectedTextColour());
    tableCell->SetSelectedDetailTextColour(tableCell->GetSelectedDetailTextColour());
    tableCell->SetTextAlignment(tableCell->GetTextAlignment());
    tableCell->SetDetailTextAlignment(tableCell->GetDetailTextAlignment());
    tableCell->SetTextLineBreakMode(tableCell->GetTextLineBreakMode());
    tableCell->SetDetailTextLineBreakMode(tableCell->GetDetailTextLineBreakMode());
    tableCell->SetAccessoryWindow(tableCell->GetAccessoryWindow());
    tableCell->SetAccessoryType(tableCell->GetAccessoryType());
    tableCell->SetBitmap(tableCell->GetBitmap());
    tableCell->SetSelectionStyle(tableCell->GetSelectionStyle());
    tableCell->SetIndentationLevel(tableCell->GetIndentationLevel());
    tableCell->SetIndentationWidth(tableCell->GetIndentationWidth());
    
    // FIXME other properties (font, selectedBitmap, event handler, ...)
    
}

@end


#pragma mark -
#pragma mark Peer implementation

wxTableViewCellIPhoneImpl::wxTableViewCellIPhoneImpl(wxTableCell* peer,
                                                     wxUITableViewCell* cell) : wxWidgetIPhoneImpl(NULL, cell), wxTableViewCellImpl()
{
    m_cell = cell;
}



wxWidgetImplType* wxWidgetImpl::CreateTableViewCell( wxTableCell* wxpeer )
{
    // Cell's initializer will figure out string identifier and style from the wxTableCell peer
    wxUITableViewCell *cell = [[wxUITableViewCell alloc] initWithWxTableCell:wxpeer];
    
    wxWidgetIPhoneImpl* c = new wxTableViewCellIPhoneImpl(wxpeer, cell);
    return c;
}



#pragma mark -
#pragma mark wxTableCell implementation

IMPLEMENT_CLASS(wxTableCell, wxTableCellBase)


wxTableCell::wxTableCell(wxTableCtrl* ctrl, const wxString& reuseName, wxTableCellStyle cellStyle)
{
    Init();
    
    m_reuseName = reuseName;
    m_cellStyle = cellStyle;
    
    SetCellWidgetImpl(wxWidgetImpl::CreateTableViewCell( this ));
}

wxTableCell::~wxTableCell()
{
    NSLog(@"cell is being freed");
    // FIXME stub
}

void wxTableCell::Init()
{
    m_widgetImpl = NULL;
    
    m_textAlignment = TextAlignmentLeft;
    m_detailTextAlignment = TextAlignmentLeft;
    m_textLineBreakMode = LineBreakModeWordWrap;
    m_detailTextLineBreakMode = LineBreakModeWordWrap;
    m_selected = false;
    m_selectionStyle = SelectionStyleBlue;
    //m_eventHandler = NULL;
    m_accessoryType = AccessoryTypeNone;
    m_accessoryWindow = NULL;
    m_editingAccessoryType = AccessoryTypeNone;
    m_editingAccessoryWindow = NULL;
    m_indentationLevel = 0;
    m_indentationWidth = 0;
    m_contentWindow = NULL;
    m_cellStyle = CellStyleDefault;
    m_detailWidth = 80;
    m_editingMode = false;
    m_shouldIndentWhileEditing = true;
    m_showReorderingButton = false;    
}

void wxTableCell::Copy(const wxTableCell& cell)
{
    // FIXME stub
}

bool wxTableCell::CreateContentWindow(wxTableCtrl* ctrl)
{
    // FIXME stub
    
    return true;
}

void wxTableCell::SetText(const wxString& text)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    // Title
    NSString *title = [NSString stringWithString:wxCFStringRef(text).AsNSString()];
    if (title) {
        [cell.textLabel setText:title];
    } else {
        [cell.textLabel setText:nil];
    }

    m_text = text;
}

void wxTableCell::SetDetailText(const wxString& text)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Subtitle
    NSString *subtitle = [NSString stringWithString:wxCFStringRef(text).AsNSString()];
    if (subtitle) {
        [cell.detailTextLabel setText:subtitle];
    } else {
        [cell.detailTextLabel setText:nil];
    }
    
    m_detailText = text;
}

void wxTableCell::SetTextFont(const wxFont& font)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    if (font.IsOk()) {
        UIFont *cocoaFont = font.OSXGetUIFont();
        [cell.textLabel setFont:cocoaFont];
    } else {
        [cell.textLabel setFont:nil];
    }
    
    m_font = font;
}

void wxTableCell::SetDetailTextFont(const wxFont& font)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    if (font.IsOk()) {
        UIFont *cocoaFont = font.OSXGetUIFont();
        [cell.detailTextLabel setFont:cocoaFont];
    } else {
        [cell.detailTextLabel setFont:nil];
    }
    
    m_detailFont = font;
}

void wxTableCell::SetTextColour(const wxColour& colour)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Title colour
    UIColor *titleColour = nil;
    titleColour = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    if (titleColour) {
        [cell.textLabel setTextColor:titleColour];
    } else {
        [cell.textLabel setTextColor:[UIColor blackColor]];
    }
    
    m_textColour = colour;
}

void wxTableCell::SetDetailTextColour(const wxColour& colour)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Subtitle colour
    UIColor *subtitleColour = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    if (subtitleColour) {
        [cell.detailTextLabel setTextColor:subtitleColour];
    } else {
        [cell.detailTextLabel setTextColor:[UIColor grayColor]]; // FIXME might be blue or gray
    }
    
    m_detailTextColour = colour;
}

void wxTableCell::SetSelectedTextColour(const wxColour& colour)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Selected title colour
    UIColor *selectedTitleColour = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    if (selectedTitleColour) {
        [cell.textLabel setHighlightedTextColor:selectedTitleColour];
    } else {
        [cell.textLabel setHighlightedTextColor:nil];
    }
    
    m_selectedTextColour = colour;
}

void wxTableCell::SetSelectedDetailTextColour(const wxColour& colour)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Selected subtitle colour
    UIColor *selectedSubtitleColour = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    if (selectedSubtitleColour) {
        [cell.detailTextLabel setHighlightedTextColor:selectedSubtitleColour];
    } else {
        [cell.detailTextLabel setHighlightedTextColor:nil];
    }
    
    m_selectedDetailTextColour = colour;
}

void wxTableCell::SetTextAlignment(wxTableCellTextAlignment alignment)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Title alignment
    UITextAlignment titleAlignment;
    switch (alignment) {
        case wxTableCell::TextAlignmentLeft:      titleAlignment = UITextAlignmentLeft;   break;
        case wxTableCell::TextAlignmentCenter:    titleAlignment = UITextAlignmentCenter; break;
        case wxTableCell::TextAlignmentRight:     titleAlignment = UITextAlignmentRight;  break;
        default:                                  titleAlignment = UITextAlignmentLeft;   break;
    };
    [cell.textLabel setTextAlignment:titleAlignment];
    
    m_textAlignment = alignment;
}

void wxTableCell::SetDetailTextAlignment(wxTableCellTextAlignment alignment)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    // Subtitle alignment
    UITextAlignment subtitleAlignment;
    switch (alignment) {
        case wxTableCell::TextAlignmentLeft:      subtitleAlignment = UITextAlignmentLeft;   break;
        case wxTableCell::TextAlignmentCenter:    subtitleAlignment = UITextAlignmentCenter; break;
        case wxTableCell::TextAlignmentRight:     subtitleAlignment = UITextAlignmentRight;  break;
        default:                                  subtitleAlignment = UITextAlignmentLeft;   break;
    };
    [cell.detailTextLabel setTextAlignment:subtitleAlignment];
    
    m_detailTextAlignment = alignment;
}

// Property conversion helper
UILineBreakMode WxTableCellGetCocoaLineBreakMode(wxTableCell::wxTableCellLineBreakMode lineBreakMode)
{
    UILineBreakMode cocoaLineBreakMode;
    switch (lineBreakMode) {
        case wxTableCell::LineBreakModeWordWrap:         cocoaLineBreakMode = UILineBreakModeWordWrap;           break;
        case wxTableCell::LineBreakModeCharacterWrap:    cocoaLineBreakMode = UILineBreakModeCharacterWrap;      break;
        case wxTableCell::LineBreakModeClip:             cocoaLineBreakMode = UILineBreakModeClip;               break;
        case wxTableCell::LineBreakModeHeadTruncation:   cocoaLineBreakMode = UILineBreakModeHeadTruncation;     break;
        case wxTableCell::LineBreakModeTailTruncation:   cocoaLineBreakMode = UILineBreakModeTailTruncation;     break;
        case wxTableCell::LineBreakModeMiddleTruncation: cocoaLineBreakMode = UILineBreakModeMiddleTruncation;   break;
        default:                                         cocoaLineBreakMode = UILineBreakModeTailTruncation;     break;
    };
    
    return cocoaLineBreakMode;
}

void wxTableCell::SetTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }    
    
    // Title line break mode
    [cell.textLabel setLineBreakMode:WxTableCellGetCocoaLineBreakMode(lineBreakMode)];

    m_textLineBreakMode = lineBreakMode;
}

void wxTableCell::SetDetailTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }    
    
    // Title line break mode
    [cell.detailTextLabel setLineBreakMode:WxTableCellGetCocoaLineBreakMode(lineBreakMode)];
    
    m_detailTextLineBreakMode = lineBreakMode;
}

void wxTableCell::SetBitmap(const wxBitmap& bitmap)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }    

    // Bitmap
    UIImage *img = nil;
    if (bitmap.IsOk() && (img = bitmap.GetUIImage())) {
        [img retain];
        [cell.imageView setImage:img];
    } else {
        [cell.imageView setImage:nil];
    }
    
    m_bitmap = bitmap;
}

void wxTableCell::SetSelectedBitmap(const wxBitmap& bitmap)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }    
    
    // Selection bitmap
    UIImage *img = nil;
    if (bitmap.IsOk() && (img = bitmap.GetUIImage())) {
        [img retain];
        [cell.imageView setHighlightedImage:img];
    } else {
        [cell.imageView setHighlightedImage:nil];
    }
    
    m_selectedBitmap = bitmap;
}

void wxTableCell::SetSelectionStyle(wxTableCellSelectionStyle style)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }    

    UITableViewCellSelectionStyle selectionStyle;
    switch (style) {
        case wxTableCell::SelectionStyleNone:     selectionStyle = UITableViewCellSelectionStyleNone;     break;
        case wxTableCell::SelectionStyleBlue:     selectionStyle = UITableViewCellSelectionStyleBlue;     break;
        case wxTableCell::SelectionStyleGrey:     selectionStyle = UITableViewCellSelectionStyleGray;     break;
        default:                                  selectionStyle = UITableViewCellSelectionStyleBlue;     break;
    };
    [cell setSelectionStyle:selectionStyle];
    
    m_selectionStyle = style;
}

void wxTableCell::SetSelected(bool selected)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setSelected:selected animated:NO];

    m_selected = selected;
}

bool wxTableCell::GetSelected() const
{
    // Check if selected straight from the native widget
    // as the local property might be off sync.
    
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return false;
    }
    
    return cell.selected;
}

// Property conversion helper
UITableViewCellAccessoryType WxTableCellGetCocoaUITableViewCellAccessoryType(wxTableCell::wxTableCellAccessoryType accessoryType)
{
    UITableViewCellAccessoryType cocoaAccessoryType;
    switch (accessoryType) {
        case wxTableCell::AccessoryTypeNone:                      cocoaAccessoryType = UITableViewCellAccessoryNone;                   break;
        case wxTableCell::AccessoryTypeDisclosureIndicator:       cocoaAccessoryType = UITableViewCellAccessoryDisclosureIndicator;    break;
        case wxTableCell::AccessoryTypeDetailDisclosureButton:    cocoaAccessoryType = UITableViewCellAccessoryDetailDisclosureButton; break;
        case wxTableCell::AccessoryTypeCheckmark:                 cocoaAccessoryType = UITableViewCellAccessoryCheckmark;              break;
        default:                                                  cocoaAccessoryType = UITableViewCellAccessoryNone;                   break;
    };
    
    return cocoaAccessoryType;
}

// Sets the accessory window
void wxTableCell::SetAccessoryWindow(wxWindow* win)
{
    if ( !win ) {
        return;
    }
    
    wxOSXWidgetImpl* peer = win->GetPeer();
    if ( !peer ) {
        return;
    }
    
    UIView *accessoryView = peer->GetWXWidget();
    if ( !accessoryView ) {
        return;
    }
    
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setAccessoryView:accessoryView];
    m_accessoryWindow = win;
}

void wxTableCell::SetAccessoryType(wxTableCellAccessoryType accessoryType)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setAccessoryType:WxTableCellGetCocoaUITableViewCellAccessoryType(accessoryType)];    

    m_accessoryType = accessoryType;
}

void wxTableCell::SetEditingAccessoryWindow(wxWindow* win)
{
    if ( !win ) {
        return;
    }
    
    wxOSXWidgetImpl* peer = win->GetPeer();
    if ( !peer ) {
        return;
    }
    
    UIView *accessoryView = peer->GetWXWidget();
    if ( !accessoryView ) {
        return;
    }
    
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setEditingAccessoryView:accessoryView];
    m_editingAccessoryWindow = win;
}

void wxTableCell::SetEditingAccessoryType(wxTableCellAccessoryType accessoryType)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setEditingAccessoryType:WxTableCellGetCocoaUITableViewCellAccessoryType(accessoryType)];
    
    m_editingAccessoryType = accessoryType;    
}

/// Sets the indentation level.
void wxTableCell::SetIndentationLevel(int indentationLevel)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setIndentationLevel:indentationLevel];
    
    m_indentationLevel = indentationLevel;
}

/// Sets the indentation width.
void wxTableCell::SetIndentationWidth(int indentationWidth)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }
    
    [cell setIndentationWidth:indentationWidth];
    
    m_indentationWidth = indentationWidth;
}

void wxTableCell::SetIndentWhileEditing(bool indent)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    [cell setShouldIndentWhileEditing:indent];
    
    m_shouldIndentWhileEditing = indent;
}

void wxTableCell::SetShowReorderingControl(bool show)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    [cell setShowsReorderControl:show];
    
    m_showReorderingButton = show;
}

bool wxTableCell::IsDeleteButtonShowing(wxTableCtrl* tableCtrl) const
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return false;
    }

    return [cell showingDeleteConfirmation];
}

bool wxTableCell::SetEditingMode(bool editingMode, bool animated)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return false;
    }

    [cell setEditing:editingMode
            animated:animated];
    
    m_editingMode = editingMode;
    
    return true;
}

bool wxTableCell::IsEditing() const
{
    // Check if editing straight from the native widget
    // as the local property might be off sync.

    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return false;
    }

    return cell.editing;
}

wxTableCell::wxTableCellEditStyle wxTableCell::GetEditStyle() const
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return EditStyleNone;
    }

    UITableViewCellEditingStyle style = cell.editingStyle;
    switch (style) {
        case UITableViewCellEditingStyleNone:   return EditStyleNone;               break;
        case UITableViewCellEditingStyleDelete: return EditStyleShowDeleteButton;   break;
        case UITableViewCellEditingStyleInsert: return EditStyleShowInsertButton;   break;
        default:                                return EditStyleNone;               break;
    };
}

void wxTableCell::SetDetailWidth(float width)
{
    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return;
    }

    CGRect frame = cell.detailTextLabel.frame;
    frame.size.width = width;
    [cell.detailTextLabel setFrame:frame];
    
    m_detailWidth = width;
}

float wxTableCell::GetDetailWidth() const
{
    // Get detail width straight from the native widget
    // as the local property might be off sync.

    UITableViewCell *cell = (UITableViewCell *)m_widgetImpl->GetWXWidget();
    if ( !cell ) {
        return 0.0f;
    }
    
    return cell.detailTextLabel.frame.size.width;
}

// Prepares the cell for reuse
void wxTableCell::PrepareForReuse(wxTableCtrl* WXUNUSED(tableCtrl))
{
    // FIXME stub
}
