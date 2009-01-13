///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/listbox.mm
// Purpose:     wxListBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: listbox.cpp 54820 2008-07-29 20:04:11Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/arrstr.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"

#include <vector>

// forward decls

class wxListWidgetCocoaImpl;

@interface wxNSTableDataSource : NSObject
{
    wxListWidgetCocoaImpl* impl;
}

- (id)tableView:(NSTableView *)aTableView 
        objectValueForTableColumn:(NSTableColumn *)aTableColumn 
        row:(NSInteger)rowIndex;

- (id)tableView:(NSTableView *)aTableView 
        setObjectValue:(NSTableColumn *)aTableColumn 
        row:(NSInteger)rowIndex;
        
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;

- (void)setImplementation: (wxListWidgetCocoaImpl *) theImplementation;
- (wxListWidgetCocoaImpl*) implementation;

@end

@interface wxNSTableView : NSTableView
{
    wxListWidgetCocoaImpl* impl;
}

- (void)setImplementation: (wxListWidgetCocoaImpl *) theImplementation;
- (wxListWidgetCocoaImpl*) implementation;
- (void)clickedAction: (id) sender;
- (void)doubleClickedAction: (id) sender;

@end

//
// table column
//

class wxCocoaTableColumn;

@interface wxNSTableColumn : NSTableColumn
{
    wxCocoaTableColumn* column;
}

- (void) setColumn: (wxCocoaTableColumn*) col;

- (wxCocoaTableColumn*) column;

@end

class WXDLLIMPEXP_CORE wxCocoaTableColumn : public wxListWidgetColumn
{
public :
    wxCocoaTableColumn( wxNSTableColumn* column, bool editable ) 
        : m_column( column ), m_editable(editable)
    {
    }
    
    ~wxCocoaTableColumn()
    {
    }
    
    wxNSTableColumn* GetNSTableColumn() const { return m_column ; }
    
    bool IsEditable() const { return m_editable; }
    
protected :
    wxNSTableColumn* m_column;
    bool m_editable;
} ;

NSString* column1 = @"1";

class wxListWidgetCocoaImpl : public wxWidgetCocoaImpl, public wxListWidgetImpl
{
public :
    wxListWidgetCocoaImpl( wxWindowMac* peer, NSScrollView* view, wxNSTableView* tableview, wxNSTableDataSource* data );
    
    ~wxListWidgetCocoaImpl();
    
    virtual wxListWidgetColumn*     InsertTextColumn( unsigned pos, const wxString& title, bool editable = false, 
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth = -1)  ;
    virtual wxListWidgetColumn*     InsertCheckColumn( unsigned pos , const wxString& title, bool editable = false, 
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth =  -1)  ;
    
    // add and remove
    
    virtual void            ListDelete( unsigned int n ) ;    
    virtual void            ListInsert( unsigned int n ) ;
    virtual void            ListClear() ;

    // selecting

    virtual void            ListDeselectAll();
    
    virtual void            ListSetSelection( unsigned int n, bool select, bool multi ) ;
    virtual int             ListGetSelection() const ;
    
    virtual int             ListGetSelections( wxArrayInt& aSelections ) const ;
    
    virtual bool            ListIsSelected( unsigned int n ) const ;
    
    // display

    virtual void            ListScrollTo( unsigned int n ) ;

    // accessing content

    virtual unsigned int    ListGetCount() const ;
    
    int                     ListGetColumnType( int col )
    {
        return col;
    }
    virtual void            UpdateLine( unsigned int n, wxListWidgetColumn* col = NULL ) ;
    virtual void            UpdateLineToEnd( unsigned int n);
protected :
    wxNSTableView*          m_tableView ;
    
    wxNSTableDataSource*    m_dataSource;
} ;

//
// implementations
//

@implementation wxNSTableColumn

- (id) init
{
    [super init];
    column = nil;
    return self;
}

- (void) setColumn: (wxCocoaTableColumn*) col
{
    column = col;
}

- (wxCocoaTableColumn*) column
{
    return column;
}

@end

class wxNSTableViewCellValue : public wxListWidgetCellValue
{
public :
    wxNSTableViewCellValue( id &v ) : value(v)
    {
    }
    
    virtual ~wxNSTableViewCellValue() {}

    virtual void Set( CFStringRef v )
    {
        value = [[(NSString*)v retain] autorelease];
    }
    virtual void Set( const wxString& value ) 
    {
        Set( (CFStringRef) wxCFStringRef( value ) );
    }
    virtual void Set( int v ) 
    {
        value = [NSNumber numberWithInt:v];
    }
    
    virtual int GetIntValue() const 
    {
        if ( [value isKindOfClass:[NSNumber class]] )
            return [ (NSNumber*) value intValue ];
            
        return 0;
    }
    
    virtual wxString GetStringValue() const 
    {
        if ( [value isKindOfClass:[NSString class]] )
            return wxCFStringRef( (CFStringRef) [value retain] ).AsString();
            
        return wxEmptyString;
    }
        
protected:
    id& value;
} ;

@implementation wxNSTableDataSource

- (id) init
{
    [super init];
    impl = nil;
    return self;
}

- (void)setImplementation: (wxListWidgetCocoaImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxListWidgetCocoaImpl*) implementation
{
    return impl;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    if ( impl )
        return impl->ListGetCount();
    return 0;
}

- (id)tableView:(NSTableView *)aTableView 
        objectValueForTableColumn:(NSTableColumn *)aTableColumn 
        row:(NSInteger)rowIndex
{
    wxNSTableColumn* tablecol = (wxNSTableColumn *)aTableColumn;
    wxListBox* lb = dynamic_cast<wxListBox*>(impl->GetWXPeer());
    wxCocoaTableColumn* col = [tablecol column];
    id value = nil;
    wxNSTableViewCellValue cellvalue(value);
    lb->GetValueCallback(rowIndex, col, cellvalue);
    return value;
}

- (void)tableView:(NSTableView *)aTableView 
        setObjectValue:(id)value forTableColumn:(NSTableColumn *)aTableColumn 
        row:(NSInteger)rowIndex
{
    wxNSTableColumn* tablecol = (wxNSTableColumn *)aTableColumn;
    wxListBox* lb = dynamic_cast<wxListBox*>(impl->GetWXPeer());
    wxCocoaTableColumn* col = [tablecol column];
    wxNSTableViewCellValue cellvalue(value);
    lb->SetValueCallback(rowIndex, col, cellvalue);
}

@end

@implementation wxNSTableView

- (void)setImplementation: (wxListWidgetCocoaImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxListWidgetCocoaImpl*) implementation
{
    return impl;
}

- (id) init
{
    [super init];
    impl = NULL;
    [self setTarget: self];
    [self setAction: @selector(clickedAction:)];
    [self setDoubleAction: @selector(doubleClickedAction:)];
    return self;
}

- (void) clickedAction: (id) sender
{
    if ( impl )
    {
        wxListBox *list = static_cast<wxListBox*> ( impl->GetWXPeer());
        wxCHECK_RET( list != NULL , wxT("Listbox expected"));

        wxCommandEvent event( wxEVT_COMMAND_LISTBOX_SELECTED, list->GetId() );

        int sel = [self clickedRow];
        if ((sel < 0) || (sel > (int) list->GetCount()))  // OS X can select an item below the last item (why?)
           return;
           
        list->HandleLineEvent( sel, false );
    }
}

- (void) doubleClickedAction: (id) sender
{
    if ( impl )
    {
        wxListBox *list = static_cast<wxListBox*> ( impl->GetWXPeer());
        wxCHECK_RET( list != NULL , wxT("Listbox expected"));

        int sel = [self clickedRow];
        if ((sel < 0) || (sel > (int) list->GetCount()))  // OS X can select an item below the last item (why?)
           return;

        list->HandleLineEvent( sel, true );
    }
}

@end

//
//
//

wxListWidgetCocoaImpl::wxListWidgetCocoaImpl( wxWindowMac* peer, NSScrollView* view, wxNSTableView* tableview, wxNSTableDataSource* data ) :
    wxWidgetCocoaImpl( peer, view ), m_tableView(tableview), m_dataSource(data)
{
}

wxListWidgetCocoaImpl::~wxListWidgetCocoaImpl()
{
    [m_dataSource release];
}

unsigned int wxListWidgetCocoaImpl::ListGetCount() const 
{
    wxListBox* lb = dynamic_cast<wxListBox*> ( GetWXPeer() );
    return lb->GetCount();
}

//
// columns
//

wxListWidgetColumn* wxListWidgetCocoaImpl::InsertTextColumn( unsigned pos, const wxString& title, bool editable, 
                                wxAlignment just, int defaultWidth) 
{
    wxNSTableColumn* col1 = [[wxNSTableColumn alloc] init];
    [col1 setEditable:editable];
    
    unsigned formerColCount = [m_tableView numberOfColumns];
    
    // there's apparently no way to insert at a specific position
    [m_tableView addTableColumn:col1 ];
    if ( pos < formerColCount )
        [m_tableView moveColumn:formerColCount toColumn:pos];
        
    if ( defaultWidth >= 0 )
    {
        [col1 setMaxWidth:defaultWidth];
        [col1 setMinWidth:defaultWidth];
    }
    
    wxCocoaTableColumn* wxcol = new wxCocoaTableColumn( col1, editable );
    [col1 setColumn:wxcol];

    // owned by the tableview
    [col1 release];
    return wxcol;
}

wxListWidgetColumn* wxListWidgetCocoaImpl::InsertCheckColumn( unsigned pos , const wxString& title, bool editable, 
                                wxAlignment just, int defaultWidth ) 
{
   wxNSTableColumn* col1 = [[wxNSTableColumn alloc] init];
    [col1 setEditable:editable];
 
    // set your custom cell & set it up
    NSButtonCell* checkbox = [[NSButtonCell alloc] init];
    [checkbox setTitle:@""];
    [checkbox setButtonType:NSSwitchButton];
    [col1 setDataCell:checkbox] ;
    [checkbox release];
             
    unsigned formerColCount = [m_tableView numberOfColumns];
    
    // there's apparently no way to insert at a specific position
    [m_tableView addTableColumn:col1 ];
    if ( pos < formerColCount )
        [m_tableView moveColumn:formerColCount toColumn:pos];
        
    if ( defaultWidth >= 0 )
    {
        [col1 setMaxWidth:defaultWidth];
        [col1 setMinWidth:defaultWidth];
    }
    
    wxCocoaTableColumn* wxcol = new wxCocoaTableColumn( col1, editable );
    [col1 setColumn:wxcol];

    // owned by the tableview
    [col1 release];
    return wxcol;
}


//
// inserting / removing lines
//

void wxListWidgetCocoaImpl::ListInsert( unsigned int n ) 
{
#if 0
    {
        wxListBoxCocoaLine* line = new wxListBoxCocoaLine();
        line->SetLabel(items[i]);
        if ( m_items.size() <= n+i )
            m_items.push_back( line );
        else
            m_items.insert(m_items.begin()+n, line);
/*
        NSMutableDictionary* line = [[NSMutableDictionary alloc] init];
        [line setObject:wxCFStringRef(items[i]).AsNSString() forKey:column1];
        NSMutableArray* array = [m_dataSource items];
        if ( [array count] <= n+i )
            [array addObject:line];
        else
            [array insertObject:line atIndex:n];
*/
    }
#endif
    [m_tableView reloadData];
}

void wxListWidgetCocoaImpl::ListDelete( unsigned int n ) 
{
    [m_tableView reloadData];
}

void wxListWidgetCocoaImpl::ListClear() 
{
    [m_tableView reloadData];
}

// selecting

void wxListWidgetCocoaImpl::ListDeselectAll()
{
    [m_tableView deselectAll:nil];
}

void wxListWidgetCocoaImpl::ListSetSelection( unsigned int n, bool select, bool multi ) 
{
    // TODO
    if ( select )
        [m_tableView selectRow: n byExtendingSelection:multi];
    else
        [m_tableView deselectRow: n];

}

int wxListWidgetCocoaImpl::ListGetSelection() const 
{
    return [m_tableView selectedRow];
}

int wxListWidgetCocoaImpl::ListGetSelections( wxArrayInt& aSelections ) const 
{
    aSelections.Empty();

    int count = ListGetCount();

    for ( int i = 0; i < count; ++i)
    {
        if ([m_tableView isRowSelected:count])
        aSelections.Add(i);
    }

    return aSelections.Count();
}

bool wxListWidgetCocoaImpl::ListIsSelected( unsigned int n ) const 
{
    return [m_tableView isRowSelected:n];
}

// display

void wxListWidgetCocoaImpl::ListScrollTo( unsigned int n ) 
{
    [m_tableView scrollRowToVisible:n];
}

    
void wxListWidgetCocoaImpl::UpdateLine( unsigned int n, wxListWidgetColumn* col ) 
{
    // TODO optimize
    [m_tableView reloadData];
}

void wxListWidgetCocoaImpl::UpdateLineToEnd( unsigned int n) 
{
    // TODO optimize
    [m_tableView reloadData];
}


// accessing content


wxWidgetImplType* wxWidgetImpl::CreateListBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    NSScrollView* scrollview = [[NSScrollView alloc] initWithFrame:r];
    
    // use same scroll flags logic as msw
    
    [scrollview setHasVerticalScroller:YES];
    
    if ( style & wxLB_HSCROLL )
        [scrollview setHasHorizontalScroller:YES];
    
    [scrollview setAutohidesScrollers: ((style & wxLB_ALWAYS_SB) ? NO : YES)];
    
    // setting up the true table
    
    wxNSTableView* tableview = [[wxNSTableView alloc] init];
    [scrollview setDocumentView:tableview];
    [tableview release];
    
    // only one multi-select mode available
    if ( (style & wxLB_EXTENDED) || (style & wxLB_MULTIPLE) )
        [tableview setAllowsMultipleSelection:YES];
        
    // simple listboxes have no header row
    [tableview setHeaderView:nil];
    
    [tableview setColumnAutoresizingStyle:NSTableViewLastColumnOnlyAutoresizingStyle];
    wxNSTableDataSource* ds = [[ wxNSTableDataSource alloc] init];
    [tableview setDataSource:ds];
    wxListWidgetCocoaImpl* c = new wxListWidgetCocoaImpl( wxpeer, scrollview, tableview, ds );
    [tableview setImplementation:c];
    [ds setImplementation:c];
    return c;
}

int wxListBox::DoListHitTest(const wxPoint& inpoint) const
{
#if wxOSX_USE_CARBON
    OSStatus err;

    // There are few reasons why this is complicated:
    // 1) There is no native HitTest function for Mac
    // 2) GetDataBrowserItemPartBounds only works on visible items
    // 3) We can't do it through GetDataBrowserTableView[Item]RowHeight
    //    because what it returns is basically inaccurate in the context
    //    of the coordinates we want here, but we use this as a guess
    //    for where the first visible item lies

    wxPoint point = inpoint;

    // get column property ID (req. for call to itempartbounds)
    DataBrowserTableViewColumnID colId = 0;
    err = GetDataBrowserTableViewColumnProperty(m_peer->GetControlRef(), 0, &colId);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserTableViewColumnProperty"));

    // OK, first we need to find the first visible item we have -
    // this will be the "low" for our binary search. There is no real
    // easy way around this, as we will need to do a SLOW linear search
    // until we find a visible item, but we can do a cheap calculation
    // via the row height to speed things up a bit
    UInt32 scrollx, scrolly;
    err = GetDataBrowserScrollPosition(m_peer->GetControlRef(), &scrollx, &scrolly);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserScrollPosition"));

    UInt16 height;
    err = GetDataBrowserTableViewRowHeight(m_peer->GetControlRef(), &height);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserTableViewRowHeight"));

    // these indices are 0-based, as usual, so we need to add 1 to them when
    // passing them to data browser functions which use 1-based indices
    int low = scrolly / height,
        high = GetCount() - 1;

    // search for the first visible item (note that the scroll guess above
    // is the low bounds of where the item might lie so we only use that as a
    // starting point - we should reach it within 1 or 2 iterations of the loop)
    while ( low <= high )
    {
        Rect bounds;
        err = GetDataBrowserItemPartBounds(
            m_peer->GetControlRef(), low + 1, colId,
            kDataBrowserPropertyEnclosingPart,
            &bounds); // note +1 to translate to Mac ID
        if ( err == noErr )
            break;

        // errDataBrowserItemNotFound is expected as it simply means that the
        // item is not currently visible -- but other errors are not
        wxCHECK_MSG( err == errDataBrowserItemNotFound, wxNOT_FOUND,
                     wxT("Unexpected error from GetDataBrowserItemPartBounds") );

        low++;
    }

    // NOW do a binary search for where the item lies, searching low again if
    // we hit an item that isn't visible
    while ( low <= high )
    {
        int mid = (low + high) / 2;

        Rect bounds;
        err = GetDataBrowserItemPartBounds(
            m_peer->GetControlRef(), mid + 1, colId,
            kDataBrowserPropertyEnclosingPart,
            &bounds); //note +1 to trans to mac id
        wxCHECK_MSG( err == noErr || err == errDataBrowserItemNotFound,
                     wxNOT_FOUND,
                     wxT("Unexpected error from GetDataBrowserItemPartBounds") );

        if ( err == errDataBrowserItemNotFound )
        {
            // item not visible, attempt to find a visible one
            // search lower
            high = mid - 1;
        }
        else // visible item, do actual hitttest
        {
            // if point is within the bounds, return this item (since we assume
            // all x coords of items are equal we only test the x coord in
            // equality)
            if ((point.x >= bounds.left && point.x <= bounds.right) &&
                (point.y >= bounds.top && point.y <= bounds.bottom) )
            {
                // found!
                return mid;
            }

            if ( point.y < bounds.top )
                // index(bounds) greater then key(point)
                high = mid - 1;
            else
                // index(bounds) less then key(point)
                low = mid + 1;
        }
    }
#endif
    return wxNOT_FOUND;
}

#endif // wxUSE_LISTBOX
