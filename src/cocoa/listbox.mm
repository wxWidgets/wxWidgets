/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/listbox.mm
// Purpose:     wxListBox
// Author:      David Elliott
// Modified by:
// Created:     2003/03/18
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/listbox.h"
#endif //WX_PRECOMP

#include "wx/cocoa/string.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/NSTableDataSource.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSEnumerator.h>
#import <AppKit/NSTableView.h>
#import <AppKit/NSTableColumn.h>

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
BEGIN_EVENT_TABLE(wxListBox, wxListBoxBase)
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
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    // Provide the data
    m_cocoaItems = [[NSMutableArray arrayWithCapacity:n] retain];
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
    m_cocoaDataSource = [[wxCocoaNSTableDataSource alloc] init];
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

    return true;
}

wxListBox::~wxListBox()
{
    [GetNSTableView() setDataSource: nil];
    [m_cocoaDataSource release];
    [m_cocoaItems release];
    DisassociateNSTableView(GetNSTableView());
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

void wxListBox::SetSelection(int n, bool select)
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

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    for(int i=int(items.GetCount())-1; i >= 0; i--)
    {
        [m_cocoaItems insertObject: wxNSStringWithWxString(items[i])
            atIndex: pos];
        m_itemClientData.Insert(NULL,pos);
    }
    [GetNSTableView() reloadData];
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // Remove everything
    [m_cocoaItems removeAllObjects];
    m_itemClientData.Clear();
    // Provide the data
    for(size_t i=0; i < items.GetCount(); i++)
    {
        [m_cocoaItems addObject: wxNSStringWithWxString(items[i])];
        m_itemClientData.Add(clientData[i]);
    }
    [GetNSTableView() reloadData];
}

void wxListBox::DoSetFirstItem(int n)
{
}


// pure virtuals from wxItemContainer
    // deleting items
void wxListBox::Clear()
{
}

void wxListBox::Delete(int n)
{
}

    // accessing strings
int wxListBox::GetCount() const
{
    return 0;
}

wxString wxListBox::GetString(int n) const
{
    return wxEmptyString;
}

void wxListBox::SetString(int n, const wxString& s)
{
}

int wxListBox::FindString(const wxString& s) const
{
    return 0;
}

    // selection
void wxListBox::Select(int n)
{
}

int wxListBox::GetSelection() const
{
    return 0;
}

int wxListBox::DoAppend(const wxString& item)
{
    return 0;
}

void wxListBox::DoSetItemClientData(int n, void* clientData)
{
}

void* wxListBox::DoGetItemClientData(int n) const
{
    return NULL;
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return NULL;
}

