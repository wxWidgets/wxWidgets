/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/tablecell.mm
// Purpose:     wxMoTableCell implementation
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

#include "wx/mobile/native/tablectrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private/tablecellimpl.h"


@implementation wxUITableViewCell

@synthesize moTableCell;

- (id)init {
    if ((self = [super init])) {
        moTableCell = NULL;
    }
    
    return self;
}

- (id)initWithWxMoTableCell:(wxMoTableCell *)initMoTableCell {
    
    NSString *cellIdentifier = [NSString stringWithString:wxCFStringRef(initMoTableCell->GetReuseName()).AsNSString()];
    if (! cellIdentifier) {
        return nil;
    }
    
    // Style
    wxMoTableCell::wxMoTableCellStyle cellStyle = initMoTableCell->GetCellStyle();
    UITableViewCellStyle style;
    switch (cellStyle) {
        case wxMoTableCell::CellStyleDefault:   style = UITableViewCellStyleDefault;    break;
        case wxMoTableCell::CellStyleValue1:    style = UITableViewCellStyleValue1;     break;
        case wxMoTableCell::CellStyleValue2:    style = UITableViewCellStyleValue2;     break;
        case wxMoTableCell::CellStyleSubtitle:  style = UITableViewCellStyleSubtitle;   break;
        default:                                style = UITableViewCellStyleDefault;    break;
    };
    
    if ((self = [self initWithStyle:style reuseIdentifier:cellIdentifier])) {
        moTableCell = initMoTableCell;
    }
    
    return self;
}

// Reads properties from wxMoTableCell, stores them to self ("translates" to Cocoa properties)
// Used just before showing the cell
- (void)commitWxMoTableCellProperties {
    
    wxASSERT_MSG(moTableCell, "wxMoTableCell is unset.");
    if (! moTableCell) {
        return;
    }
    
    // Title
    NSString *title = [NSString stringWithString:wxCFStringRef(moTableCell->GetText()).AsNSString()];
    if (title) {
        [self.textLabel setText:title];
    } else {
        [self.textLabel setText:nil];
    }
    
    // Subtitle
    NSString *subtitle = [NSString stringWithString:wxCFStringRef(moTableCell->GetDetailText()).AsNSString()];
    if (subtitle) {
        [self.detailTextLabel setText:subtitle];
    } else {
        [self.detailTextLabel setText:nil];
    }
    
    // Title colour
    UIColor *titleColour = nil;
    wxColour wxTextColour = moTableCell->GetTextColour();
    titleColour = [[[UIColor alloc] initWithCGColor:wxTextColour.GetCGColor()] autorelease];
    if (titleColour) {
        [self.textLabel setTextColor:titleColour];
    } else {
        [self.textLabel setTextColor:[UIColor blackColor]];
    }

    // Subtitle colour
    UIColor *subtitleColour = nil;
    wxColour wxDetailTextColour = moTableCell->GetDetailTextColour();
    subtitleColour = [[[UIColor alloc] initWithCGColor:wxDetailTextColour.GetCGColor()] autorelease];
    if (subtitleColour) {
        [self.detailTextLabel setTextColor:subtitleColour];
    } else {
        [self.detailTextLabel setTextColor:[UIColor grayColor]]; // FIXME might be blue or gray
    }

    // Selected text colour
    // FIXME detailTextLabel too!
    UIColor *selectedTextColour = nil;
    wxColour wxSelectedTextColour = moTableCell->GetSelectedTextColour();
    selectedTextColour = [[[UIColor alloc] initWithCGColor:wxSelectedTextColour.GetCGColor()] autorelease];
    if (selectedTextColour) {
        [self.textLabel setHighlightedTextColor:selectedTextColour];
    } else {
        [self.textLabel setHighlightedTextColor:nil];
    }
    
    // Title alignment
    wxMoTableCell::wxMoTableCellTextAlignment wxTitleAlignment = moTableCell->GetTextAlignment();
    UITextAlignment titleAlignment;
    switch (wxTitleAlignment) {
        case wxMoTableCell::TextAlignmentLeft:      titleAlignment = UITextAlignmentLeft;   break;
        case wxMoTableCell::TextAlignmentCenter:    titleAlignment = UITextAlignmentCenter; break;
        case wxMoTableCell::TextAlignmentRight:     titleAlignment = UITextAlignmentRight;  break;
        default:                                    titleAlignment = UITextAlignmentLeft;   break;
    };
    [self.textLabel setTextAlignment:titleAlignment];

    // Subtitle alignment
    wxMoTableCell::wxMoTableCellTextAlignment wxSubtitleAlignment = moTableCell->GetDetailTextAlignment();
    UITextAlignment subtitleAlignment;
    switch (wxSubtitleAlignment) {
        case wxMoTableCell::TextAlignmentLeft:      subtitleAlignment = UITextAlignmentLeft;   break;
        case wxMoTableCell::TextAlignmentCenter:    subtitleAlignment = UITextAlignmentCenter; break;
        case wxMoTableCell::TextAlignmentRight:     subtitleAlignment = UITextAlignmentRight;  break;
        default:                                    subtitleAlignment = UITextAlignmentLeft;   break;
    };
    [self.detailTextLabel setTextAlignment:subtitleAlignment];
    
    // FIXME various other properties too
    
    // Bitmap
    UIImage *img = nil;
    wxBitmap bitmap = moTableCell->GetBitmap();
    if ((img = bitmap.GetUIImage())) {
        [img retain];
    }
    [self.imageView setImage:img];
}

@end



wxTableViewCellIPhoneImpl::wxTableViewCellIPhoneImpl(wxMoTableCell* peer,
                                                     wxUITableViewCell* cell) : wxWidgetIPhoneImpl(NULL, cell), wxTableViewCellImpl()
{
    m_cell = cell;
}



wxWidgetImplType* wxWidgetImpl::CreateTableViewCell( wxMoTableCell* wxpeer )
{
    // Cell's initializer will figure out string identifier and style from the wxMoTableCell peer
    wxUITableViewCell *cell = [[wxUITableViewCell alloc] initWithWxMoTableCell:wxpeer];
    
    wxWidgetIPhoneImpl* c = new wxTableViewCellIPhoneImpl(wxpeer, cell);
    return c;
}
