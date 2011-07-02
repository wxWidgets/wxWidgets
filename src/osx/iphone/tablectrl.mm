/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/tablectrl.mm
// Purpose:     wxMoTableCtrl implementation
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-06-30
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
#include "wx/osx/iphone/private/tablectrlimpl.h"
#include "wx/osx/iphone/private/tablecellimpl.h"


@implementation wxUITableView

@end


@implementation wxUITableViewController (Private)

- (wxMoTableDataSource *)moDataSource {
    wxASSERT_MSG(moTableCtrl != NULL, "wxMoTableCtrl *moTableCtrl is unset.");
    if (moTableCtrl == NULL) {
        return NULL;
    }
    
    wxMoTableDataSource *dataSource = moTableCtrl->GetDataSource();
    wxASSERT_MSG(dataSource != NULL, "wxMoTableCtrl datasource is unset.");

    return dataSource;
}

// When unable to create a cell, return a dummy cell so that the UI won't crash
- (UITableViewCell *)tableView:(UITableView *)tableView
    dummyCellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *MyIdentifier = @"wxMoTableCtrlDummyCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                       reuseIdentifier:MyIdentifier] autorelease];
    }
    
    cell.textLabel.text = [NSString stringWithFormat:@"Dummy cell for section %d, row %d", indexPath.section, indexPath.row];
 
    return cell;
}

@end


@implementation wxUITableViewController

@synthesize moTableCtrl;

- (id)init {
    if ((self = [super init])) {
        moTableCtrl = NULL;
    }
    
    return self;
}

- (id)initWithTableView:(wxUITableView *)initTableView
          wxMoTableCtrl:(wxMoTableCtrl *)initMoTableCtrl {
    if ((self = [self init])) {
        moTableCtrl = initMoTableCtrl;
        [self setTableView:initTableView];
        [self.tableView setDataSource:self];
        [self.tableView setDelegate:self];
    }
    
    return self;
}

#pragma mark UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {

    wxMoTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return 0;
    }
    
    return dataSource->GetSectionCount(moTableCtrl);
}

- (NSInteger)tableView:(UITableView *)tableView
 numberOfRowsInSection:(NSInteger)section {
    
    wxMoTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return 0;
    }
    
    return dataSource->GetRowCount(moTableCtrl, section);
}

- (NSString *)tableView:(UITableView *)tableView
titleForHeaderInSection:(NSInteger)section {

    wxMoTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return @"";
    }
    
    NSUInteger uSection = (NSUInteger)section;
    
    wxArrayString sectionTitles = dataSource->GetSectionTitles(moTableCtrl);
    if (sectionTitles.Count() < uSection) {
        return @"";
    }
    
    NSString *title = [NSString stringWithString:wxCFStringRef(sectionTitles[uSection]).AsNSString()];
    return title;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    wxMoTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }

    wxTablePath path = wxTablePath(indexPath.section, indexPath.row);
    wxMoTableCell *cell = dataSource->GetCell(moTableCtrl, path);
    
    wxASSERT_MSG(cell, "Unable to get wxMoTableCell:");
    if (! cell) {
        NSLog(@"section %d, row %d", indexPath.section, indexPath.row);
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }
    
    wxUITableViewCell *cocoaCell = (wxUITableViewCell *)(cell->GetCellWidgetImpl()->GetWXWidget());
    wxASSERT_MSG(cocoaCell, "Unable to get wxUITableViewCell from wxMoTableCell");
    if (! cocoaCell) {
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }
    
    // Read properties from wxMoTableCell, commit to Cocoa object
    [cocoaCell commitWxMoTableCellProperties];
    
    return cocoaCell;
}

#pragma mark UITableViewDelegate

- (void)tableView:(UITableView *)tableView
didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    
    
}

@end


wxTableViewCtrlIPhoneImpl::wxTableViewCtrlIPhoneImpl( wxWindowMac* peer,
                          wxUITableViewController* tableViewController,
                          wxUITableView *tableView ) : wxWidgetIPhoneImpl(peer, tableView), wxTableViewControllerImpl()
{
    m_tableViewController = tableViewController;
    m_tableView = tableView;
    m_tableCtrl = NULL;
}
    
bool wxTableViewCtrlIPhoneImpl::ReloadData()
{
    [m_tableView reloadData];
    return true;
}


wxWidgetImplType* wxWidgetImpl::CreateTableViewCtrl( wxWindowMac* wxpeer,
                                                     wxWindowMac* WXUNUSED(parent),
                                                     wxWindowID WXUNUSED(id),
                                                     const wxPoint& pos,
                                                     const wxSize& size,
                                                     long style,
                                                     long extraStyle)
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    
    UITableViewStyle tableViewStyle;
    if (wxpeer->HasFlag(wxTC_PLAIN)) {
        tableViewStyle = UITableViewStylePlain;
    } else if (wxpeer->HasFlag(wxTC_GROUPED)) {
        tableViewStyle = UITableViewStyleGrouped;
    } else {
        tableViewStyle = UITableViewStylePlain;
    }
    
    wxUITableView *tableView = [[wxUITableView alloc] initWithFrame:r
                                                            style:tableViewStyle];
    wxUITableViewController *tableViewController = [[wxUITableViewController alloc] initWithTableView:tableView
                                                                                        wxMoTableCtrl:(wxMoTableCtrl *)wxpeer];
    
    wxWidgetIPhoneImpl* c = new wxTableViewCtrlIPhoneImpl( wxpeer, tableViewController, tableView );
    return c;
}
