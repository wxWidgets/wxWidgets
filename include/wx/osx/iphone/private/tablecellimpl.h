/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private/tablecellimpl.h
// Purpose:     wxMoTableCell implementation classes that have to be exposed
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-02
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_IPHONE_PRIVATE_TABLECELL_H_
#define _WX_OSX_IPHONE_PRIVATE_TABLECELL_H_

#include "wx/osx/private.h"
#include "wx/mobile/native/tablectrl.h"
#include "wx/mobile/native/tablecell.h"


#pragma mark Cocoa

@interface wxUITableViewCell : UITableViewCell
{
@private
    wxMoTableCell *moTableCell;
}

@property (nonatomic) wxMoTableCell *moTableCell;

- (id)initWithWxMoTableCell:(wxMoTableCell *)initMoTableCell;

// Reads properties from wxMoTableCell, stores them to self ("translates" to Cocoa properties)
// Used just before showing the cell
- (void)commitWxMoTableCellProperties;

@end



class wxTableViewCellIPhoneImpl : public wxWidgetIPhoneImpl, public wxTableViewCellImpl
{
public:
    wxTableViewCellIPhoneImpl(wxMoTableCell* peer,
                              wxUITableViewCell *cell );
        
private:
    wxUITableViewCell *m_cell;
};

#endif // _WX_OSX_IPHONE_PRIVATE_TABLECELL_H_
