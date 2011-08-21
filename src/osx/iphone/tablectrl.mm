/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/tablectrl.mm
// Purpose:     wxTableCtrl implementation
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

#include "wx/mstream.h"
#include "wx/dcbuffer.h"

#include "wx/tablectrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private/tablectrlimpl.h"
#include "wx/osx/iphone/private/tablecellimpl.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreGraphics/CoreGraphics.h>


#pragma mark -
#pragma mark Cocoa class

@implementation wxUITableView

@end


@implementation wxUITableViewController (Private)

- (wxTableDataSource *)moDataSource {
    wxASSERT_MSG(moTableCtrl != NULL, "wxTableCtrl *moTableCtrl is unset.");
    if (moTableCtrl == NULL) {
        return NULL;
    }
    
    wxTableDataSource *dataSource = moTableCtrl->GetDataSource();

    return dataSource;
}

// When unable to create a cell, return a dummy cell so that the UI won't crash
- (UITableViewCell *)tableView:(UITableView *)tableView
    dummyCellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *MyIdentifier = @"wxTableCtrlDummyCell";
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
          wxTableCtrl:(wxTableCtrl *)initMoTableCtrl {
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

    wxTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return 1;
    }
    
    int sectionCount = dataSource->GetSectionCount(moTableCtrl);
    wxASSERT_MSG(sectionCount >= 1, "Table section count must be 1 or more");
    
    return sectionCount;
}

- (NSInteger)tableView:(UITableView *)tableView
 numberOfRowsInSection:(NSInteger)section {
    
    wxTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return 0;
    }
    
    return dataSource->GetRowCount(moTableCtrl, section);
}

- (NSString *)tableView:(UITableView *)tableView
titleForHeaderInSection:(NSInteger)section {

    wxTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return @"";
    }
    
    NSUInteger uSection = (NSUInteger)section;
    
    wxArrayString sectionTitles = dataSource->GetSectionTitles(moTableCtrl);
    if (sectionTitles.Count() < uSection+1) {
        return @"";
    }
    
    NSString *title = [NSString stringWithString:wxCFStringRef(sectionTitles[uSection]).AsNSString()];
    return title;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    wxTableDataSource *dataSource = [self moDataSource];
    if (dataSource == NULL) {
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }

    wxTablePath path = wxTablePath(indexPath.section, indexPath.row);
    wxTableCell *cell = dataSource->GetCell(moTableCtrl, path);
    
    wxASSERT_MSG(cell, "Unable to get wxTableCell:");
    if (! cell) {
        NSLog(@"section %d, row %d", indexPath.section, indexPath.row);
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }
    
    wxOSXWidgetImpl* cellWidgetImpl = cell->GetCellWidgetImpl();
    wxASSERT_MSG(cellWidgetImpl, "Unable to get wxUITableViewCell from wxTableCell");
    if ( !cellWidgetImpl ) {
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }

    wxUITableViewCell* cocoaCell = (wxUITableViewCell *)(cellWidgetImpl->GetWXWidget());
    wxASSERT_MSG(cocoaCell, "Unable to get wxUITableViewCell from wxTableCell");
    if (! cocoaCell) {
        return [self tableView:tableView dummyCellForRowAtIndexPath:indexPath];
    }
    
    // Read properties from wxTableCell, commit to Cocoa object
    [cocoaCell commitWxTableCellProperties];
    
    return cocoaCell;
}

#pragma mark UITableViewDelegate

- (void)tableView:(UITableView *)tableView
didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    wxTablePath path = wxTablePath(indexPath.section, indexPath.row);
    
    wxTableDataSource *dataSource = [self moDataSource];
    
    // Both table controller and data source get informed
    dataSource->OnSelectRow(moTableCtrl, path);

    wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_ROW_SELECTED, moTableCtrl->GetId(), moTableCtrl);
    event.SetPath(path);
    event.SetEventObject(moTableCtrl);
    moTableCtrl->GetEventHandler()->ProcessEvent(event);
}

@end


#pragma mark -
#pragma mark Peer implementation

wxTableViewCtrlIPhoneImpl::wxTableViewCtrlIPhoneImpl( wxWindowMac* peer,
                          wxUITableViewController* tableViewController,
                          wxUITableView *tableView ) : wxWidgetIPhoneImpl(peer, tableView), wxTableViewControllerImpl()
{
    m_tableViewController = tableViewController;
    m_tableView = tableView;
    m_tableCtrl = (wxTableCtrl *)peer;
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
                                                                                        wxTableCtrl:(wxTableCtrl *)wxpeer];
    
    wxWidgetIPhoneImpl* c = new wxTableViewCtrlIPhoneImpl( wxpeer, tableViewController, tableView );
    return c;
}


#pragma mark -
#pragma mark wxTableCtrl implementation


BEGIN_EVENT_TABLE(wxTableCtrl, wxTableCtrlBase)
END_EVENT_TABLE()


/// Constructor.
wxTableCtrl::wxTableCtrl(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, name);
}

bool wxTableCtrl::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxWindow::Create( parent, id, pos, size, style, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateTableViewCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;    
}

wxTableCtrl::~wxTableCtrl()
{
    
}

wxTableCell* wxTableCtrl::GetReusableCell(const wxString& reuseName)
{
    // FIXME
    return NULL;
    
    // Let the Cocoa part take care of the reusability.
    
    wxUITableView *tableView = (wxUITableView *)GetPeer()->GetWXWidget();
    if (! tableView) {
        return NULL;
    }
    
    NSString *cellIdentifier = wxCFStringRef(reuseName).AsNSString();
    if (! cellIdentifier) {
        return NULL;
    }
    
    wxUITableViewCell *cell = (wxUITableViewCell *)[tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (! cell) {
        return NULL;
    }
    
    return [cell tableCell];
}

// Clears all data
void wxTableCtrl::Clear()
{
    m_sections.Clear();
    m_indexTitles.Clear();
    
    size_t i;
    for (i = 0; i < m_reusableCells.GetCount(); i++)
    {
        wxTableCell* cell = m_reusableCells[i];
        delete cell;
    }
    m_reusableCells.Clear();
    m_totalTableHeight = 0;
}

bool wxTableCtrl::ReloadData(bool resetScrollbars)
{
    Clear();
    
    wxTableViewCtrlIPhoneImpl *peer = (wxTableViewCtrlIPhoneImpl *)GetPeer();
    return peer->ReloadData();
}

/// Starts batching of operations within which no visual updates are performed.
/// You can nest Freeze/Thaw operations.
void wxTableCtrl::Freeze()
{
    // FIXME stub
}

/// Ends batching of operations, refetches the data, and refreshes the window.
void wxTableCtrl::Thaw()
{
    // FIXME stub
}

/// Finds the path for the cell.
bool wxTableCtrl::FindPathForCell(wxTableCell* cell, wxTablePath& path) const
{
    // FIXME stub
    return false;
}

/// Finds the cell for the path.
wxTableCell* wxTableCtrl::FindCellForPath(const wxTablePath& path) const
{
    // FIXME stub
    return NULL;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::ReloadRows(const wxTablePathArray& WXUNUSED(paths),
                             RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

// Sets editing mode (not yet implemented).
bool wxTableCtrl::SetEditingMode(bool editingMode,
                                 bool WXUNUSED(animated))
{
    // FIXME stub
    
    return true;
}

// Inserts the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::InsertSections(const wxArrayInt& WXUNUSED(sections),
                                 RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

// Deletes the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::DeleteRows(const wxTablePathArray& WXUNUSED(paths),
                             RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::InsertRows(const wxTablePathArray& WXUNUSED(paths),
                             RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

// Refreshes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::ReloadSections(const wxArrayInt& WXUNUSED(sections),
                                 RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

// Deletes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxTableCtrl::DeleteSections(const wxArrayInt& WXUNUSED(sections),
                                 RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub
    
    return true;
}

/// Scroll to the given path (section/row)
bool wxTableCtrl::ScrollToPath(const wxTablePath& path)
{
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:path.GetRow()
                                                inSection:path.GetSection()];
    if ( !indexPath ) {
        return false;
    }
    
    wxUITableView *tableView = (wxUITableView *)GetPeer()->GetWXWidget();
    if (! tableView) {
        return false;
    }
    
    [tableView scrollToRowAtIndexPath:indexPath
                     atScrollPosition:UITableViewScrollPositionTop
                             animated:YES];
    return true;
}

void wxTableCtrl::SetSelection(const wxTablePath& path)
{
    // FIXME stub
}

/// Removes the selection at the given path.
void wxTableCtrl::Deselect(const wxTablePath& path)
{
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:path.GetRow()
                                                inSection:path.GetSection()];
    if ( !indexPath ) {
        return;
    }

    wxUITableView *tableView = (wxUITableView *)GetPeer()->GetWXWidget();
    if (! tableView) {
        return;
    }
    
    [tableView deselectRowAtIndexPath:indexPath
                             animated:YES];
}

wxTablePath* wxTableCtrl::GetSelection() const
{
    // FIXME stub
    
    return NULL;
}

// Loads the data within the specified rectangle.
bool wxTableCtrl::LoadVisibleData(const wxRect& rect1)
{
    // FIXME stub
    
    return true;
}

#pragma mark wxTableDataSource implementation

/// Called by the table control to commit an insertion requested by the user. This function should
/// then call InsertRows in response.
bool wxTableDataSource::CommitInsertRow(wxTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->InsertRows(paths, wxTableCtrlBase::RowAnimationNone);
}

/// Called by the table control to commit a deletion requested by the user. This function should
/// then call DeleteRows in response.
bool wxTableDataSource::CommitDeleteRow(wxTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->DeleteRows(paths, wxTableCtrlBase::RowAnimationNone);
}
