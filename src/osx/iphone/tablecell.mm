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
    
    // Title
    NSString *title = [NSString stringWithString:wxCFStringRef(tableCell->GetText()).AsNSString()];
    if (title) {
        [self.textLabel setText:title];
    } else {
        [self.textLabel setText:nil];
    }
    
    // Subtitle
    NSString *subtitle = [NSString stringWithString:wxCFStringRef(tableCell->GetDetailText()).AsNSString()];
    if (subtitle) {
        [self.detailTextLabel setText:subtitle];
    } else {
        [self.detailTextLabel setText:nil];
    }
    
    // Title colour
    UIColor *titleColour = nil;
    wxColour wxTextColour = tableCell->GetTextColour();
    titleColour = [[[UIColor alloc] initWithCGColor:wxTextColour.GetCGColor()] autorelease];
    if (titleColour) {
        [self.textLabel setTextColor:titleColour];
    } else {
        [self.textLabel setTextColor:[UIColor blackColor]];
    }

    // Subtitle colour
    UIColor *subtitleColour = nil;
    wxColour wxDetailTextColour = tableCell->GetDetailTextColour();
    subtitleColour = [[[UIColor alloc] initWithCGColor:wxDetailTextColour.GetCGColor()] autorelease];
    if (subtitleColour) {
        [self.detailTextLabel setTextColor:subtitleColour];
    } else {
        [self.detailTextLabel setTextColor:[UIColor grayColor]]; // FIXME might be blue or gray
    }

    // Selected title colour
    UIColor *selectedTitleColour = nil;
    wxColour wxSelectedTitleColour = tableCell->GetSelectedTextColour();
    selectedTitleColour = [[[UIColor alloc] initWithCGColor:wxSelectedTitleColour.GetCGColor()] autorelease];
    if (selectedTitleColour) {
        [self.textLabel setHighlightedTextColor:selectedTitleColour];
    } else {
        [self.textLabel setHighlightedTextColor:nil];
    }

    // Selected subtitle colour
    UIColor *selectedSubtitleColour = nil;
    wxColour wxSelectedSubtitleColour = tableCell->GetSelectedTextColour();
    selectedSubtitleColour = [[[UIColor alloc] initWithCGColor:wxSelectedSubtitleColour.GetCGColor()] autorelease];
    if (selectedSubtitleColour) {
        [self.detailTextLabel setHighlightedTextColor:selectedSubtitleColour];
    } else {
        [self.detailTextLabel setHighlightedTextColor:nil];
    }
    
    // Title alignment
    wxTableCell::wxTableCellTextAlignment wxTitleAlignment = tableCell->GetTextAlignment();
    UITextAlignment titleAlignment;
    if (wxTitleAlignment != wxTableCell::TextAlignmentLeft) {
        NSLog(@"something's wrong");
    }
    switch (wxTitleAlignment) {
        case wxTableCell::TextAlignmentLeft:      titleAlignment = UITextAlignmentLeft;   break;
        case wxTableCell::TextAlignmentCenter:    titleAlignment = UITextAlignmentCenter; break;
        case wxTableCell::TextAlignmentRight:     titleAlignment = UITextAlignmentRight;  break;
        default:                                  titleAlignment = UITextAlignmentLeft;   break;
    };
    [self.textLabel setTextAlignment:titleAlignment];

    // Subtitle alignment
    wxTableCell::wxTableCellTextAlignment wxSubtitleAlignment = tableCell->GetDetailTextAlignment();
    UITextAlignment subtitleAlignment;
    switch (wxSubtitleAlignment) {
        case wxTableCell::TextAlignmentLeft:      subtitleAlignment = UITextAlignmentLeft;   break;
        case wxTableCell::TextAlignmentCenter:    subtitleAlignment = UITextAlignmentCenter; break;
        case wxTableCell::TextAlignmentRight:     subtitleAlignment = UITextAlignmentRight;  break;
        default:                                  subtitleAlignment = UITextAlignmentLeft;   break;
    };
    [self.detailTextLabel setTextAlignment:subtitleAlignment];
    
    // Accessory type
    wxTableCell::wxTableCellAccessoryType wxAccessoryType = tableCell->GetAccessoryType();
    UITableViewCellAccessoryType accessoryType;
    switch (wxAccessoryType) {
        case wxTableCell::AccessoryTypeNone:                      accessoryType = UITableViewCellAccessoryNone;                   break;
        case wxTableCell::AccessoryTypeDisclosureIndicator:       accessoryType = UITableViewCellAccessoryDisclosureIndicator;    break;
        case wxTableCell::AccessoryTypeDetailDisclosureButton:    accessoryType = UITableViewCellAccessoryDetailDisclosureButton; break;
        case wxTableCell::AccessoryTypeCheckmark:                 accessoryType = UITableViewCellAccessoryCheckmark;              break;
        default:                                                  accessoryType = UITableViewCellAccessoryNone;                   break;
    };
    [self setAccessoryType:accessoryType];
    
    // Selection style
    wxTableCell::wxTableCellSelectionStyle wxSelectionStyle = tableCell->GetSelectionStyle();
    UITableViewCellSelectionStyle selectionStyle;
    switch (wxSelectionStyle) {
        case wxTableCell::SelectionStyleNone:     selectionStyle = UITableViewCellSelectionStyleNone;     break;
        case wxTableCell::SelectionStyleBlue:     selectionStyle = UITableViewCellSelectionStyleBlue;     break;
        case wxTableCell::SelectionStyleGrey:     selectionStyle = UITableViewCellSelectionStyleGray;     break;
        default:                                  selectionStyle = UITableViewCellSelectionStyleBlue;     break;
    };
    [self setSelectionStyle:selectionStyle];
    
    // Title line break mode (FIXME merge with detail line break mode to avoid duplicating the code)
    wxTableCell::wxTableCellLineBreakMode wxTitleLineBreakMode = tableCell->GetTextLineBreakMode();
    UILineBreakMode titleLineBreakMode;
    switch (wxTitleLineBreakMode) {
        case wxTableCell::LineBreakModeWordWrap:         titleLineBreakMode = UILineBreakModeWordWrap;           break;
        case wxTableCell::LineBreakModeCharacterWrap:    titleLineBreakMode = UILineBreakModeCharacterWrap;      break;
        case wxTableCell::LineBreakModeClip:             titleLineBreakMode = UILineBreakModeClip;               break;
        case wxTableCell::LineBreakModeHeadTruncation:   titleLineBreakMode = UILineBreakModeHeadTruncation;     break;
        case wxTableCell::LineBreakModeTailTruncation:   titleLineBreakMode = UILineBreakModeTailTruncation;     break;
        case wxTableCell::LineBreakModeMiddleTruncation: titleLineBreakMode = UILineBreakModeMiddleTruncation;   break;
        default:                                         titleLineBreakMode = UILineBreakModeTailTruncation;     break;
    };
    [self.textLabel setLineBreakMode:titleLineBreakMode];

    // Subtitle line break mode (FIXME merge with title line break mode to avoid duplicating the code)
    wxTableCell::wxTableCellLineBreakMode wxSubtitleLineBreakMode = tableCell->GetDetailTextLineBreakMode();
    UILineBreakMode subtitleLineBreakMode;
    switch (wxSubtitleLineBreakMode) {
        case wxTableCell::LineBreakModeWordWrap:         subtitleLineBreakMode = UILineBreakModeWordWrap;           break;
        case wxTableCell::LineBreakModeCharacterWrap:    subtitleLineBreakMode = UILineBreakModeCharacterWrap;      break;
        case wxTableCell::LineBreakModeClip:             subtitleLineBreakMode = UILineBreakModeClip;               break;
        case wxTableCell::LineBreakModeHeadTruncation:   subtitleLineBreakMode = UILineBreakModeHeadTruncation;     break;
        case wxTableCell::LineBreakModeTailTruncation:   subtitleLineBreakMode = UILineBreakModeTailTruncation;     break;
        case wxTableCell::LineBreakModeMiddleTruncation: subtitleLineBreakMode = UILineBreakModeMiddleTruncation;   break;
        default:                                           subtitleLineBreakMode = UILineBreakModeTailTruncation;     break;
    };
    [self.detailTextLabel setLineBreakMode:subtitleLineBreakMode];
    
    // Bitmap
    UIImage *img = nil;
    wxBitmap bitmap = tableCell->GetBitmap();
    if ((img = bitmap.GetUIImage())) {
        [img retain];
    }
    [self.imageView setImage:img];

    
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
    //m_editStyle = EditStyleNone;
    m_indentationLevel = 0;
    m_indentationWidth = 0;
    m_contentWindow = NULL;
    m_cellStyle = CellStyleDefault;
    m_detailWidth = 80;
    m_editingMode = false;
    m_shouldIndentWhileEditing = true;
    m_showReorderingButton = false;    
}

// Sets the accessory window
void wxTableCell::SetAccessoryWindow(wxWindow* win)
{
    UIView *accessoryView = win->GetPeer()->GetWXWidget();
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
