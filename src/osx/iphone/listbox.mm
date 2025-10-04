///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/IPhone/listbox.mm
// Purpose:     wxListBox
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor, Robert Roebling
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
#include "wx/osx/private/available.h"

// forward decls

class wxListWidgetIPhoneImpl;

@interface wxUITableDataSource : NSObject <UITableViewDataSource>
{
    wxListWidgetIPhoneImpl* impl;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;


- (void)setImplementation: (wxListWidgetIPhoneImpl *) theImplementation;
- (wxListWidgetIPhoneImpl*) implementation;

@end

@interface wxUITableView : UITableView <UITableViewDelegate>
{
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath;

@end


class wxListWidgetIPhoneImpl : public wxWidgetIPhoneImpl, public wxListWidgetImpl
{
public :
    wxListWidgetIPhoneImpl( wxWindowMac* peer, UIScrollView* view, wxUITableView* tableview, wxUITableDataSource* data );

    ~wxListWidgetIPhoneImpl();

    virtual wxListWidgetColumn*     InsertTextColumn( unsigned pos, const wxString& title, bool editable = false,
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth = -1) override  ;
    virtual wxListWidgetColumn*     InsertCheckColumn( unsigned pos , const wxString& title, bool editable = false,
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth =  -1) override  ;

    // add and remove

    virtual void            ListDelete( unsigned int n ) override ;
    virtual void            ListInsert( unsigned int n ) override ;
    virtual void            ListClear() override ;

    // selecting

    virtual void            ListDeselectAll() override;

    virtual void            ListSetSelection( unsigned int n, bool select, bool multi ) override ;
    virtual int             ListGetSelection() const override ;

    virtual int             ListGetSelections( wxArrayInt& aSelections ) const override ;

    virtual bool            ListIsSelected( unsigned int n ) const override ;

    // display

    virtual void            ListScrollTo( unsigned int n ) override ;

    virtual int             ListGetTopItem() const override;
    virtual int             ListGetCountPerPage() const override;

    // accessing content

    virtual unsigned int    ListGetCount() const override ;
    virtual int             DoListHitTest( const wxPoint& inpoint ) const override;

    virtual void            UpdateLine( unsigned int n, wxListWidgetColumn* col = nullptr ) override ;
    virtual void            UpdateLineToEnd( unsigned int n) override;


protected :
    void                    AdaptColumnWidth ( int w );

    wxUITableView*          m_tableView ;

    wxUITableDataSource*    m_dataSource;

    NSMutableArray*         m_widths;
    int                     m_maxWidth;
    bool                    m_autoSize;
} ;



@implementation wxUITableDataSource

- (id) init
{
    if ( self = [super init] )
    {
        impl = nil;
    }
    return self;
}

- (void)setImplementation: (wxListWidgetIPhoneImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxListWidgetIPhoneImpl*) implementation
{
    return impl;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if ( impl )
        return impl->ListGetCount();
    return 0;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *cellIdentifier = @"wxListBoxCellIdentifier";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    
    if (cell == nil)
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];    
    }

    wxListBox* lb = dynamic_cast<wxListBox*> ( impl->GetWXPeer() );
    wxString text = lb->GetString( indexPath.row );
    wxCFStringRef cftext( text );
    cell.textLabel.text =  cftext.AsNSString();

    return cell;
}

@end

@implementation wxUITableView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    int row = indexPath.row;

    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    wxListBox* const list = dynamic_cast<wxListBox*> ( impl->GetWXPeer() );

    list->MacHandleSelectionChange(row);
}


@end

//
//
//

wxListWidgetIPhoneImpl::wxListWidgetIPhoneImpl( wxWindowMac* peer, UIScrollView* view, wxUITableView* tableview, wxUITableDataSource* data ) :
    wxWidgetIPhoneImpl( peer, view ), m_tableView(tableview), m_dataSource(data)
{
    InstallEventHandler( tableview );
}

wxListWidgetIPhoneImpl::~wxListWidgetIPhoneImpl()
{
    [m_dataSource release];
}

unsigned int wxListWidgetIPhoneImpl::ListGetCount() const
{
    wxListBox* lb = dynamic_cast<wxListBox*> ( GetWXPeer() );
    return lb->GetCount();
}


wxListWidgetColumn*   wxListWidgetIPhoneImpl::InsertTextColumn( unsigned pos, const wxString& title, bool editable,
                                wxAlignment just, int defaultWidth)
{
    return nullptr;
}

wxListWidgetColumn*   wxListWidgetIPhoneImpl::InsertCheckColumn( unsigned pos , const wxString& title, bool editable,
                                wxAlignment just, int defaultWidth )
{
    return nullptr;
}

//
// inserting / removing lines
//

void wxListWidgetIPhoneImpl::ListInsert( unsigned int n )
{
    [m_tableView reloadData];
}

void wxListWidgetIPhoneImpl::ListDelete( unsigned int n )
{
    if (ListIsSelected(n))
        ListSetSelection( n, false, false );

    [m_tableView reloadData];
}

void wxListWidgetIPhoneImpl::ListClear()
{
    [m_tableView reloadData];
}

// selecting

void wxListWidgetIPhoneImpl::ListDeselectAll()
{
    [m_tableView selectRowAtIndexPath: nil animated:YES  scrollPosition:UITableViewScrollPositionNone];
}

void wxListWidgetIPhoneImpl::ListSetSelection( unsigned int n, bool select, bool multi )
{
    NSIndexPath *ip = [NSIndexPath indexPathForRow:n inSection:0];
    if (select)
    {
        [m_tableView selectRowAtIndexPath: ip animated:YES  scrollPosition:UITableViewScrollPositionNone];
    }
    else
    {
        [m_tableView deselectRowAtIndexPath: ip animated:NO ];
    }
}

int wxListWidgetIPhoneImpl::ListGetSelection() const
{
    NSIndexPath *ip = [m_tableView indexPathForSelectedRow];
    if (ip == nil)
        return -1;
    return ip.row;
}

int wxListWidgetIPhoneImpl::ListGetSelections( wxArrayInt& aSelections ) const
{
    aSelections.Empty();

    NSArray<NSIndexPath*> *ips = [m_tableView indexPathsForSelectedRows];
    for (NSIndexPath *ip in ips)
    {
        aSelections.Add(ip.row);
    }

    return aSelections.Count();
}

bool wxListWidgetIPhoneImpl::ListIsSelected( unsigned int n ) const
{
    NSArray<NSIndexPath*> *ips = [m_tableView indexPathsForSelectedRows];
    for (NSIndexPath *ip in ips)
    {
        if (ip.row == n)
          return true;
    }
    return false;
}

// display

void wxListWidgetIPhoneImpl::ListScrollTo( unsigned int n )
{
    NSIndexPath *ip = [NSIndexPath indexPathForRow:n inSection:0];
    [m_tableView scrollToRowAtIndexPath: ip atScrollPosition: UITableViewScrollPositionTop animated: NO];
}

int wxListWidgetIPhoneImpl::ListGetTopItem() const
{
    return 0;
}

int wxListWidgetIPhoneImpl::ListGetCountPerPage() const
{
    return 0;
}

void wxListWidgetIPhoneImpl::UpdateLine( unsigned int WXUNUSED(n), wxListWidgetColumn* WXUNUSED(col) )
{
    // TODO optimize
    [m_tableView reloadData];
}

void wxListWidgetIPhoneImpl::UpdateLineToEnd( unsigned int WXUNUSED(n))
{
    // TODO optimize
    [m_tableView reloadData];
}

// accessing content


wxWidgetImplType* wxWidgetImpl::CreateListBox( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;

    wxUITableView* tableview = [[wxUITableView alloc] initWithFrame:r];
    [tableview setDelegate:tableview];

    // only one multi-select mode available
    if ( (style & wxLB_EXTENDED) || (style & wxLB_MULTIPLE) )
        [tableview setAllowsMultipleSelection:YES];


    wxUITableDataSource* ds = [[ wxUITableDataSource alloc] init];
    [tableview setDataSource:ds];

    wxListWidgetIPhoneImpl* impl = new wxListWidgetIPhoneImpl( wxpeer, tableview, tableview, ds );

    [ds setImplementation:impl];
    return impl;
}

int wxListWidgetIPhoneImpl::DoListHitTest(const wxPoint& inpoint) const
{
    return -1;
}

#endif // wxUSE_LISTBOX
