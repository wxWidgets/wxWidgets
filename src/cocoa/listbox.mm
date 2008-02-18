/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/listbox.mm
// Purpose:     wxListBox
// Author:      David Elliott
// Modified by:
// Created:     2003/03/18
// Id:          $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/cocoa/string.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/ObjcRef.h"
#include "wx/cocoa/private/scrollview.h"
#include "wx/cocoa/NSTableDataSource.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSEnumerator.h>
#import <AppKit/NSTableView.h>
#import <AppKit/NSTableColumn.h>
#import <AppKit/NSScrollView.h>
#import <AppKit/NSCell.h>

// ============================================================================
// @class wxCocoaListBoxNSTableDataSource
// ============================================================================
// 2.8 hack: We can't add an i-var to wxListBox so we add one here
@interface wxCocoaListBoxNSTableDataSource : wxCocoaNSTableDataSource
{
    BOOL m_needsUpdate;
}

@end
WX_DECLARE_GET_OBJC_CLASS(wxCocoaListBoxNSTableDataSource,wxCocoaNSTableDataSource)

@implementation wxCocoaListBoxNSTableDataSource
// No methods
@end
WX_IMPLEMENT_GET_OBJC_CLASS_WITH_UNIQUIFIED_SUPERCLASS(wxCocoaListBoxNSTableDataSource,wxCocoaNSTableDataSource)


// ============================================================================
// helper functions
// ============================================================================

static CGFloat _TableColumnMaxWidthForItems(NSTableColumn *tableColumn, NSArray *items)
{
    wxAutoNSAutoreleasePool pool;

    NSCell *dataCell = [[[tableColumn dataCell] copy] autorelease];
    CGFloat width = 0.0f;
    NSEnumerator *itemEnum = [items objectEnumerator];
    NSString *item;
    while( (item = [itemEnum nextObject]) != nil )
    {
        [dataCell setStringValue: item];
        NSSize itemSize = [dataCell cellSize];
        CGFloat itemWidth = itemSize.width;
        if(itemWidth > width)
            width = itemWidth;
    }
    return width;
}

static void _SetWidthOfTableColumnToFitItems(NSTableColumn *tableColumn, NSArray *items)
{
    CGFloat width = _TableColumnMaxWidthForItems(tableColumn, items);
    [tableColumn setWidth:width];
    [tableColumn setMinWidth:width];
}

// ============================================================================
// class wxListBox
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
BEGIN_EVENT_TABLE(wxListBox, wxListBoxBase)
    EVT_IDLE(wxListBox::_WxCocoa_OnIdle)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxListBox,NSTableView,NSControl,NSView)

bool wxListBox::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, winid, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

bool wxListBox::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
/*
wxLB_SINGLE
Single-selection list.

wxLB_MULTIPLE
Multiple-selection list: the user can toggle multiple items on and off.

wxLB_EXTENDED
Extended-selection list: the user can select multiple items using the SHIFT key and the mouse or special key combinations.

wxLB_HSCROLL
Create horizontal scrollbar if contents are too wide (Windows only).

wxLB_ALWAYS_SB
Always show a vertical scrollbar.

wxLB_NEEDED_SB
Only create a vertical scrollbar if needed.

wxLB_SORT
The listbox contents are sorted in alphabetical order.
*/
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    // Provide the data
    m_cocoaItems = wxGCSafeRetain([NSMutableArray arrayWithCapacity:n]);
    for(int i=0; i < n; i++)
    {
        [m_cocoaItems addObject: wxNSStringWithWxString(choices[i])];
    }
    // Remove everything
    m_itemClientData.Clear();
    // Initialize n elements to NULL
    m_itemClientData.SetCount(n,NULL);

    SetNSTableView([[NSTableView alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    [GetNSTableView() setHeaderView: nil];

    // Set up the data source
    m_cocoaDataSource = [[WX_GET_OBJC_CLASS(wxCocoaListBoxNSTableDataSource) alloc] init];
    [GetNSTableView() setDataSource:m_cocoaDataSource];

    // Add the single column
    NSTableColumn *tableColumn = [[NSTableColumn alloc] initWithIdentifier:nil];
    [GetNSTableView() addTableColumn: tableColumn];
    [tableColumn release];

    [GetNSTableView() sizeToFit];
    // Finish
    if(m_parent)
        m_parent->CocoaAddChild(this);
    // NSTableView does WEIRD things with sizes.  Wrapping it in an
    // NSScrollView seems to be the only reasonable solution.
    CocoaCreateNSScrollView();
    SetInitialFrameRect(pos,size);

    [m_wxCocoaScrollView->GetNSScrollView() setHasVerticalScroller:YES];
    // Pre-10.3: Always show vertical scroller, never show horizontal scroller
    // Post-10.3: Show scrollers dynamically (turn them both on, set auto-hide)
    if([m_wxCocoaScrollView->GetNSScrollView() respondsToSelector:@selector(setAutohidesScrollers:)])
    {
        [m_wxCocoaScrollView->GetNSScrollView() setHasHorizontalScroller:YES];
        [m_wxCocoaScrollView->GetNSScrollView() setAutohidesScrollers:YES];
    }

    // Set up extended/multiple selection flags
    if ((style & wxLB_EXTENDED) || (style & wxLB_MULTIPLE))
        //diff is that mult requires shift down for multi selection
        [GetNSTableView() setAllowsMultipleSelection:true];

    [GetNSTableView() setAllowsColumnSelection:false];
    _SetWidthOfTableColumnToFitItems(tableColumn, m_cocoaItems);
    return true;
}

wxSize wxListBox::DoGetBestSize() const
{
    wxSize size = wxControlWithItems::DoGetBestSize();
    // Limit best size to 100x100. It can be smaller if none of the items are very
    // wide or if there aren't many items, but anything bigger than 100x100 ought
    // to be asked for by the programmer. The 100x100 size is based on being barely
    // enough for a scroller to be usable.
    if(size.GetWidth() > 100)
        size.SetWidth(100);
    if(size.GetHeight() > 100)
        size.SetHeight(100);
    return size;
}

wxListBox::~wxListBox()
{
    [GetNSTableView() setDataSource: nil];
    [m_cocoaDataSource release];
    wxGCSafeRelease(m_cocoaItems);
    m_cocoaItems = nil;
    DisassociateNSTableView(GetNSTableView());
}

bool wxListBox::_WxCocoa_GetNeedsUpdate()
{
    return static_cast<wxCocoaListBoxNSTableDataSource*>(m_cocoaDataSource)->m_needsUpdate;
}

void wxListBox::_WxCocoa_SetNeedsUpdate(bool needsUpdate)
{
    static_cast<wxCocoaListBoxNSTableDataSource*>(m_cocoaDataSource)->m_needsUpdate = needsUpdate;
}

void wxListBox::_WxCocoa_OnIdle(wxIdleEvent &event)
{
    event.Skip();
    if(_WxCocoa_GetNeedsUpdate())
    {
        _SetWidthOfTableColumnToFitItems([[GetNSTableView() tableColumns] objectAtIndex:0], m_cocoaItems);
        [GetNSTableView() tile];
        [GetNSTableView() reloadData];
        _WxCocoa_SetNeedsUpdate(false);
    }
}

int wxListBox::CocoaDataSource_numberOfRows()
{
    return [m_cocoaItems count];
}

struct objc_object* wxListBox::CocoaDataSource_objectForTableColumn(
        WX_NSTableColumn tableColumn, int rowIndex)
{
    return [m_cocoaItems objectAtIndex:rowIndex];
}

// pure virtuals from wxListBoxBase
bool wxListBox::IsSelected(int n) const
{
    return [GetNSTableView() isRowSelected: n];
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if(select)
        [GetNSTableView() selectRow: n byExtendingSelection:NO];
    else
        [GetNSTableView() deselectRow: n];
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Clear();
    NSEnumerator *enumerator = [GetNSTableView() selectedRowEnumerator];
    while(NSNumber *num = [enumerator nextObject])
    {
        aSelections.Add([num intValue]);
    }
    return [GetNSTableView() numberOfSelectedRows];
}

void wxListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    wxAutoNSAutoreleasePool pool;

    for(int i=int(items.GetCount())-1; i >= 0; i--)
    {
        [m_cocoaItems insertObject: wxNSStringWithWxString(items[i])
            atIndex: pos];
        m_itemClientData.Insert(NULL,pos);
    }
    _WxCocoa_SetNeedsUpdate(true);
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    wxAutoNSAutoreleasePool pool;

    // Remove everything
    [m_cocoaItems removeAllObjects];
    m_itemClientData.Clear();
    // Provide the data
    for(unsigned int i=0; i < items.GetCount(); i++)
    {
        [m_cocoaItems addObject: wxNSStringWithWxString(items[i])];
        m_itemClientData.Add(clientData[i]);
    }
    _WxCocoa_SetNeedsUpdate(true);
}

void wxListBox::DoSetFirstItem(int n)
{
    [m_cocoaItems exchangeObjectAtIndex:0 withObjectAtIndex:n];
    void* pOld = m_itemClientData[n];
    m_itemClientData[n] = m_itemClientData[0];
    m_itemClientData[0] = pOld;
    [GetNSTableView() reloadData];
}


// pure virtuals from wxItemContainer
    // deleting items
void wxListBox::Clear()
{
    [m_cocoaItems removeAllObjects];
    m_itemClientData.Clear();
    [GetNSTableView() reloadData];
}

void wxListBox::Delete(unsigned int n)
{
    [m_cocoaItems removeObjectAtIndex:n];
    m_itemClientData.RemoveAt(n);
    _WxCocoa_SetNeedsUpdate(true);
}

    // accessing strings
unsigned int wxListBox::GetCount() const
{
    return (unsigned int)[m_cocoaItems count];
}

wxString wxListBox::GetString(unsigned int n) const
{
    return wxStringWithNSString([m_cocoaItems objectAtIndex:n]);
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxAutoNSAutoreleasePool pool;
    [m_cocoaItems removeObjectAtIndex:n];
    [m_cocoaItems insertObject: wxNSStringWithWxString(s) atIndex: n];
    [GetNSTableView() reloadData];
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    // FIXME: use wxItemContainerImmutable::FindString for bCase parameter
    wxAutoNSAutoreleasePool pool;
    return [m_cocoaItems indexOfObject:wxNSStringWithWxString(s)];
}

    // selection
int wxListBox::GetSelection() const
{
    return [GetNSTableView() selectedRow];
}

int wxListBox::DoAppend(const wxString& item)
{
    wxAutoNSAutoreleasePool pool;
    [m_cocoaItems addObject:wxNSStringWithWxString(item)];
    _WxCocoa_SetNeedsUpdate(true);
    m_itemClientData.Add(NULL);
    return [m_cocoaItems count];
}

void wxListBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_itemClientData[n] = clientData;
}

void* wxListBox::DoGetItemClientData(unsigned int n) const
{
    return m_itemClientData[n];
}

void wxListBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    m_itemClientData[n] = (void*) clientData;
}

wxClientData* wxListBox::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData*) m_itemClientData[n];
}

#endif
