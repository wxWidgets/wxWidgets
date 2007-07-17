/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/databrow.cpp
// Purpose:     Classes and functions for the Carbon data browser
// Author:      
// Modified by:
// Created:     2007-05-18
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxUSE_GENERICDATAVIEWCTRL

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/font.h"
#endif

#include "wx/apptrait.h"

#include "wx/mac/uma.h"


#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <ATSUnicode.h>
    #include <TextCommon.h>
    #include <TextEncodingConverter.h>
#endif

#include "wx/mac/private.h"

#if defined(__MWERKS__) && wxUSE_UNICODE
#if __MWERKS__ < 0x4100 || !defined(__DARWIN__)
    #include <wtime.h>
#endif
#endif

#include <limits>

#include "wx/mac/carbon/databrow.h"

#include "wx/dataview.h"

// ============================================================================
// DataBrowser Wrapper
// ============================================================================
//
// basing on DataBrowserItemIDs
//

pascal Boolean wxMacDataBrowserTableViewControl::DataBrowserCompareProc(ControlRef browser, DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty)
{
  wxMacDataBrowserTableViewControl* ControlPtr(dynamic_cast<wxMacDataBrowserTableViewControl*>(wxMacControl::GetReferenceFromNativeControl(browser)));


  if (ControlPtr != NULL)
    return ControlPtr->DataBrowserCompareProc(itemOneID,itemTwoID,sortProperty);
  else
    return errDataBrowserPropertyNotSupported;
} /* wxMacDataBrowserTableViewControl::DataBrowserCompareProc(ControlRef, DataBrowserItemID, DataBrowserItemID, DataBrowserPropertyID) */

pascal void wxMacDataBrowserTableViewControl::DataBrowserDrawItemProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice)
{
  wxMacDataBrowserTableViewControl* ControlPtr(dynamic_cast<wxMacDataBrowserTableViewControl*>(wxMacControl::GetReferenceFromNativeControl(browser)));


  if (ControlPtr != NULL)
    ControlPtr->DataBrowserDrawItemProc(itemID,property,state,rectangle,bitDepth,colorDevice);
} /* wxMacDataBrowserTableViewControl::DataBrowserDrawItemProc(ControlRef, DataBrowserItemID, DataBrowserPropertyID, DataBrowserItemState, Rect const*, SInt16, Boolean) */

pascal OSStatus wxMacDataBrowserTableViewControl::DataBrowserGetSetItemDataProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue)
{
  wxMacDataBrowserTableViewControl* ControlPtr(dynamic_cast<wxMacDataBrowserTableViewControl*>(wxMacControl::GetReferenceFromNativeControl(browser)));


  if (ControlPtr != NULL)
    return ControlPtr->DataBrowserGetSetItemDataProc(itemID,property,itemData,getValue);
  else
    return errDataBrowserPropertyNotSupported;
} /* wxMacDataBrowserTableViewControl::DataBrowserGetSetItemDataProc(ControlRef, DataBrowserItemID, DataBrowserPropertyID, DataBrowserItemDataRef, Boolean) */

pascal void wxMacDataBrowserTableViewControl::DataBrowserItemNotificationProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData)
{
  wxMacDataBrowserTableViewControl* ControlPtr(dynamic_cast<wxMacDataBrowserTableViewControl*>(wxMacControl::GetReferenceFromNativeControl(browser)));


  if (ControlPtr != NULL)
    ControlPtr->DataBrowserItemNotificationProc(itemID,message,itemData);
} /* wxMacDataBrowserTableViewControl::DataBrowserItemNotificationProc(ControlRef, DataBrowserItemID, DataBrowserItemNotification, DataBrowserItemDataRef) */

DataBrowserDrawItemUPP         gDataBrowserTableViewDrawItemUPP         = NULL;
DataBrowserItemCompareUPP      gDataBrowserTableViewItemCompareUPP      = NULL;
DataBrowserItemDataUPP         gDataBrowserTableViewItemDataUPP         = NULL;
DataBrowserItemNotificationUPP gDataBrowserTableViewItemNotificationUPP = NULL;

wxMacDataBrowserTableViewControl::wxMacDataBrowserTableViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style)
                                 :wxMacControl(peer)
{
  Rect bounds = wxMacGetBoundsForControl(peer,pos,size);
  OSStatus err = ::CreateDataBrowserControl(MAC_WXHWND(peer->MacGetTopLevelWindowRef()),&bounds,kDataBrowserListView,&(this->m_controlRef));


  SetReferenceInNativeControl();
  verify_noerr(err);
  if (gDataBrowserTableViewDrawItemUPP         == NULL) gDataBrowserTableViewDrawItemUPP    = NewDataBrowserDrawItemUPP   (DataBrowserDrawItemProc);
  if (gDataBrowserTableViewItemCompareUPP      == NULL) gDataBrowserTableViewItemCompareUPP = NewDataBrowserItemCompareUPP(wxMacDataBrowserTableViewControl::DataBrowserCompareProc);
  if (gDataBrowserTableViewItemDataUPP         == NULL) gDataBrowserTableViewItemDataUPP    = NewDataBrowserItemDataUPP   (wxMacDataBrowserTableViewControl::DataBrowserGetSetItemDataProc);
  if (gDataBrowserTableViewItemNotificationUPP == NULL)
  {
    gDataBrowserTableViewItemNotificationUPP =
#if TARGET_API_MAC_OSX
      (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP(wxMacDataBrowserTableViewControl::DataBrowserItemNotificationProc);
#else
      NewDataBrowserItemNotificationUPP(wxMacDataBrowserTableViewControl::DataBrowserItemNotificationProc);
#endif
  }

  DataBrowserCallbacks       callbacks;
  DataBrowserCustomCallbacks customCallbacks;

  InitializeDataBrowserCallbacks      (&callbacks,      kDataBrowserLatestCallbacks);
  InitializeDataBrowserCustomCallbacks(&customCallbacks,kDataBrowserLatestCallbacks);

  callbacks.u.v1.itemDataCallback         = gDataBrowserTableViewItemDataUPP;
  callbacks.u.v1.itemCompareCallback      = gDataBrowserTableViewItemCompareUPP;
  callbacks.u.v1.itemNotificationCallback = gDataBrowserTableViewItemNotificationUPP;
  this->SetCallbacks(&callbacks);

  customCallbacks.u.v1.drawItemCallback = gDataBrowserTableViewDrawItemUPP;
  this->SetCustomCallbacks(&customCallbacks);

 // style setting:
  this->EnableCellSizeModification();

  DataBrowserSelectionFlags flags; // variable definition

  if (this->GetSelectionFlags(&flags) == noErr) // get default settings
  {
    if ((style & wxDV_MULTIPLE) != 0)
      flags &= ~kDataBrowserSelectOnlyOne;
    else
      flags |= kDataBrowserSelectOnlyOne;
    (void) this->SetSelectionFlags(flags);
  } /* if */
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  OptionBits attributes; // variable definition
  
  if (this->GetAttributes(&attributes) == noErr) // get default settings
  {
    if ((style & wxDV_VERT_RULES) != 0)
      attributes |= kDataBrowserAttributeListViewDrawColumnDividers;
    else
      attributes &= ~kDataBrowserAttributeListViewDrawColumnDividers;
    (void) this->SetAttributes(attributes);
  } /* if */
#endif
  if ((style & wxDV_NO_HEADER) != 0)
    this->SetHeaderButtonHeight(0);
} /* wxMacDataBrowserTableViewControl::wxMacDataBrowserTableViewControl(wxWindow*, wxPoint const&, wxSize const&, long) */

//
// callback handling
//
OSStatus wxMacDataBrowserTableViewControl::SetCallbacks(DataBrowserCallbacks const* callbacks)
{
  return ::SetDataBrowserCallbacks(this->m_controlRef,callbacks);
} /* wxMacDataBrowserTableViewControl::SetCallbacks(DataBrowserCallbacks const*) */

OSStatus wxMacDataBrowserTableViewControl::SetCustomCallbacks(DataBrowserCustomCallbacks const* customCallbacks)
{
  return ::SetDataBrowserCustomCallbacks(this->m_controlRef,customCallbacks);
} /* xMacDataBrowserTableViewControl::SetCustomCallbacks(DataBrowserCustomCallbacks const*) */

//
// header handling
//
OSStatus wxMacDataBrowserTableViewControl::GetHeaderDesc(DataBrowserPropertyID property, DataBrowserListViewHeaderDesc* desc) const
{
  desc->version = kDataBrowserListViewLatestHeaderDesc; // if this statement is missing the next call will fail (NOT DOCUMENTED!!)
  return ::GetDataBrowserListViewHeaderDesc(this->m_controlRef,property,desc);
}

OSStatus wxMacDataBrowserTableViewControl::SetHeaderDesc(DataBrowserPropertyID property, DataBrowserListViewHeaderDesc* desc)
{
  return ::SetDataBrowserListViewHeaderDesc(this->m_controlRef,property,desc);
}

//
// layout handling
//
OSStatus wxMacDataBrowserTableViewControl::AutoSizeColumns()
{
  return AutoSizeDataBrowserListViewColumns(this->m_controlRef);
}

OSStatus wxMacDataBrowserTableViewControl::EnableCellSizeModification(bool enableHeight, bool enableWidth)
{
  return ::SetDataBrowserTableViewGeometry(this->GetControlRef(),enableWidth,enableHeight);
} /* wxMacDataBrowserTableViewControl::EnableCellSizeModification(bool, bool) */

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
OSStatus wxMacDataBrowserTableViewControl::GetAttributes(OptionBits* attributes)
{
  return ::DataBrowserGetAttributes(this->GetControlRef(),attributes);
} /* wxMacDataBrowserTableViewControl::GetAttributes(OptionBits*) */
#endif

OSStatus wxMacDataBrowserTableViewControl::GetColumnWidth( DataBrowserPropertyID column , UInt16 *width ) const
{
    return GetDataBrowserTableViewNamedColumnWidth( m_controlRef , column , width );
}

OSStatus wxMacDataBrowserTableViewControl::GetDefaultRowHeight( UInt16 * height ) const
{
    return GetDataBrowserTableViewRowHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserTableViewControl::GetHeaderButtonHeight(UInt16 *height)
{
    return GetDataBrowserListViewHeaderBtnHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserTableViewControl::GetDefaultColumnWidth( UInt16 *width ) const
{
    return GetDataBrowserTableViewColumnWidth( m_controlRef , width );
}

OSStatus wxMacDataBrowserTableViewControl::GetRowHeight( DataBrowserItemID item , UInt16 *height) const
{
    return GetDataBrowserTableViewItemRowHeight( m_controlRef, item , height);
}

OSStatus wxMacDataBrowserTableViewControl::GetScrollPosition( UInt32 *top , UInt32 *left ) const
{
    return GetDataBrowserScrollPosition( m_controlRef , top , left );
}

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
OSStatus wxMacDataBrowserTableViewControl::SetAttributes(OptionBits attributes)
{
  return ::DataBrowserChangeAttributes(this->GetControlRef(),attributes,~attributes);
} /* wxMacDataBrowserTableViewControl::SetAttributes(OptionBits) */
#endif

OSStatus wxMacDataBrowserTableViewControl::SetColumnWidth( DataBrowserPropertyID column , UInt16 width )
{
    return SetDataBrowserTableViewNamedColumnWidth( m_controlRef , column , width );
}

OSStatus wxMacDataBrowserTableViewControl::SetDefaultColumnWidth( UInt16 width )
{
    return SetDataBrowserTableViewColumnWidth( m_controlRef , width );
}

OSStatus wxMacDataBrowserTableViewControl::SetDefaultRowHeight( UInt16 height )
{
    return SetDataBrowserTableViewRowHeight( m_controlRef , height );
}

OSStatus wxMacDataBrowserTableViewControl::SetHasScrollBars( bool horiz, bool vert )
{
    return SetDataBrowserHasScrollBars( m_controlRef, horiz, vert );
}

OSStatus wxMacDataBrowserTableViewControl::SetHeaderButtonHeight(UInt16 height)
{
    return SetDataBrowserListViewHeaderBtnHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserTableViewControl::SetHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle )
{
    return SetDataBrowserTableViewHiliteStyle( m_controlRef, hiliteStyle );
}

OSStatus wxMacDataBrowserTableViewControl::SetRowHeight(DataBrowserItemID item, UInt16 height)
{
  return ::SetDataBrowserTableViewItemRowHeight(this->m_controlRef,item,height);
}

OSStatus wxMacDataBrowserTableViewControl::SetScrollPosition(UInt32 top, UInt32 left)
{
  return ::SetDataBrowserScrollPosition(this->m_controlRef,top,left);
}

//
// column handling
//
OSStatus wxMacDataBrowserTableViewControl::GetColumnCount(UInt32* numColumns) const
{
  return GetDataBrowserTableViewColumnCount(this->m_controlRef,numColumns);
}

OSStatus wxMacDataBrowserTableViewControl::GetColumnID(DataBrowserTableViewColumnIndex position, DataBrowserTableViewColumnID* id)
{
  return GetDataBrowserTableViewColumnProperty(this->m_controlRef,position,id);
} /* wxMacDataBrowserTableViewControl::GetColumnID(DataBrowserTableViewColumnIndex position, DataBrowserTableViewColumnID* id) */

OSStatus wxMacDataBrowserTableViewControl::GetFreePropertyID(DataBrowserPropertyID* id) const
{
  for (*id=kMinPropertyID; *id<std::numeric_limits<DataBrowserPropertyID>::max(); ++(*id))
    if (this->IsUsedPropertyID(*id) == errDataBrowserPropertyNotFound)
      return noErr;
  return errDataBrowserPropertyNotSupported;
} /* wxMacDataBrowserTableViewControl::GetFreePropertyID(DataBrowserPropertyID*) const */

OSStatus wxMacDataBrowserTableViewControl::GetPropertyColumn(DataBrowserPropertyID propertyID, DataBrowserTableViewColumnIndex* column) const
{
  return GetDataBrowserTableViewColumnPosition(this->m_controlRef,propertyID,column);
} /* wxMacDataBrowserTableViewControl::GetPropertyColumn(DataBrowserPropertyID, DataBrowserTableViewColumnIndex*) const */

OSStatus wxMacDataBrowserTableViewControl::GetPropertyFlags(DataBrowserPropertyID property, DataBrowserPropertyFlags *flags) const
{
    return GetDataBrowserPropertyFlags( m_controlRef , property , flags );
}

OSStatus wxMacDataBrowserTableViewControl::IsUsedPropertyID(DataBrowserPropertyID propertyID) const
{
 // as the Mac interface does not provide a function that checks if the property id is in use or not a function is chosen that should not
 // lead to a large overhead for the call but returns an error code if the property id does not exist, here we use the function that returns
 // the column position for the property id:
  DataBrowserTableViewColumnIndex index;
  
  return ::GetDataBrowserTableViewColumnPosition(this->m_controlRef,propertyID,&index);
} /* wxMacDataBrowserTableViewControl::IsUsedPropertyId(DataBrowserPropertyID) const */

OSStatus wxMacDataBrowserTableViewControl::RemoveColumn(DataBrowserTableViewColumnIndex position)
{
  DataBrowserTableViewColumnID id;
  GetColumnID( position, &id );
  return RemoveDataBrowserTableViewColumn( m_controlRef, id );
}

OSStatus wxMacDataBrowserTableViewControl::SetColumnPosition(DataBrowserPropertyID column, DataBrowserTableViewColumnIndex position)
{
  return ::SetDataBrowserTableViewColumnPosition(this->m_controlRef,column,position);
}

OSStatus wxMacDataBrowserTableViewControl::SetPropertyFlags(DataBrowserPropertyID property, DataBrowserPropertyFlags flags)
{
  return ::SetDataBrowserPropertyFlags(this->m_controlRef,property,flags);
} /* wxMacDataBrowserTableViewControl::SetPropertyFlags(DataBrowserPropertyID, DataBrowserPropertyFlags) */

OSStatus wxMacDataBrowserTableViewControl::SetDisclosureColumn(DataBrowserPropertyID property, Boolean expandableRows)
{
  return SetDataBrowserListViewDisclosureColumn( m_controlRef, property, expandableRows);
}

//
// item handling
//
OSStatus wxMacDataBrowserTableViewControl::AddItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty)
{
  return ::AddDataBrowserItems(this->m_controlRef,container,numItems,items,preSortProperty);
} /* wxMacDataBrowserTableViewControl::AddItems(DataBrowserItemID, UInt32, DataBrowserItemID const*, DataBrowserPropertyID) */

OSStatus wxMacDataBrowserTableViewControl::GetFreeItemID(DataBrowserItemID* id) const
{
  ItemCount NoOfItems;
  
  
  verify_noerr(this->GetItemCount(&NoOfItems));
  if (NoOfItems == 0)
  {
    *id = 1;
    return noErr;
  } /* if */
  else
  {
   // as there might be a lot of items in the data browser and mostly the data is added item by item the largest used ID number is roughly in the order of magnitude
   // as the number of items; therefore, start from the number of items to look for a new ID:
    for (*id=NoOfItems; *id<std::numeric_limits<DataBrowserItemID>::max(); ++(*id))
      if (this->IsUsedItemID(*id) == errDataBrowserItemNotFound)
        return noErr;
   // as the first approach was not successful, try from the beginning:
    for (*id=0; *id<NoOfItems; ++(*id))
      if (this->IsUsedItemID(*id) == errDataBrowserItemNotFound)
        return noErr;
   // sorry, data browser is full:
    return errDataBrowserItemNotAdded;
  } /* if */
} /* wxMacDataBrowserTableViewControl::GetFreeItemID(DataBrowserItemID*) const */

OSStatus wxMacDataBrowserTableViewControl::GetItemCount(DataBrowserItemID container, Boolean recurse, DataBrowserItemState state, ItemCount* numItems) const
{
  return GetDataBrowserItemCount(this->m_controlRef,container,recurse,state,numItems);
} /* wxMacDataBrowserTableViewControl::GetItemCount(DataBrowserItemID, Boolean, DataBrowserItemState, ItemCount*) const */

OSStatus wxMacDataBrowserTableViewControl::GetItemID( DataBrowserTableViewRowIndex row, DataBrowserItemID * item ) const
{
  return GetDataBrowserTableViewItemID(this->m_controlRef,row,item);
}

OSStatus wxMacDataBrowserTableViewControl::GetItems(DataBrowserItemID container, Boolean recurse, DataBrowserItemState state, Handle items) const
{
  return GetDataBrowserItems(this->m_controlRef,container,recurse,state,items);
} /* wxMacDataBrowserTableViewControl::GetItems(DataBrowserItemID, Boolean, DataBrowserItemState, Handle) const */

OSStatus wxMacDataBrowserTableViewControl::GetItemRow(DataBrowserItemID item, DataBrowserTableViewRowIndex* row) const
{
  return GetDataBrowserTableViewItemRow(this->m_controlRef,item,row);
}

OSStatus wxMacDataBrowserTableViewControl::IsUsedItemID(DataBrowserItemID itemID) const
{
 // as the Mac interface does not provide a function that checks if the property id is in use or not a function is chosen that should not
 // lead to a large overhead for the call but returns an error code if the property id does not exist, here we use the function that returns
 // the column position for the property id:
  DataBrowserTableViewColumnIndex index;
  
  return ::GetDataBrowserTableViewItemRow(this->m_controlRef,itemID,&index);
} /* wxMacDataBrowserTableViewControl::IsUsedItemID(DataBrowserItemID) const */

OSStatus wxMacDataBrowserTableViewControl::RemoveItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty)
{
  return ::RemoveDataBrowserItems(this->m_controlRef,container,numItems,items,preSortProperty);
}

OSStatus wxMacDataBrowserTableViewControl::RevealItem(DataBrowserItemID item, DataBrowserPropertyID propertyID, DataBrowserRevealOptions options) const
{
  return ::RevealDataBrowserItem(this->m_controlRef,item,propertyID,options);
} /* wxMacDataBrowserTableViewControl::RevealItem(DataBrowserItemID item, DataBrowserPropertyID propertyID, DataBrowserRevealOptions options) const */

OSStatus wxMacDataBrowserTableViewControl::UpdateItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty,
                                                       DataBrowserPropertyID propertyID) const
{
  return UpdateDataBrowserItems(this->m_controlRef,container,numItems,items,preSortProperty,propertyID);
}

//
// item selection
//
size_t wxMacDataBrowserTableViewControl::GetSelectedItemIDs(wxArrayDataBrowserItemID& itemIDs) const
{
  DataBrowserItemID* itemIDPtr;
  Handle handle(::NewHandle(0));
  
  size_t NoOfItems;


  wxCHECK_MSG(this->GetItems(kDataBrowserNoItem,true,kDataBrowserItemIsSelected,handle) == noErr,0,_("Could not get selected items."));
  NoOfItems = static_cast<size_t>(::GetHandleSize(handle)/sizeof(DataBrowserItemID));
  itemIDs.Empty();
  itemIDs.Alloc(NoOfItems);
  HLock(handle);
  itemIDPtr = (DataBrowserItemID*) (*handle);
  for (size_t i=0; i<NoOfItems; ++i)
  {
    itemIDs.Add(*itemIDPtr);
    ++itemIDPtr;
  } /* for */
  HUnlock(handle);
  DisposeHandle(handle);
  return NoOfItems;
} /* wxMacDataBrowserTableViewControl::GetSelectedItemIDs(wxArrayDataBrowserItemID&) const*/

OSStatus wxMacDataBrowserTableViewControl::GetSelectionAnchor( DataBrowserItemID *first, DataBrowserItemID *last ) const
{
    return GetDataBrowserSelectionAnchor( m_controlRef, first, last );
}

OSStatus wxMacDataBrowserTableViewControl::GetSelectionFlags(DataBrowserSelectionFlags* flags) const
{
  return ::GetDataBrowserSelectionFlags(this->m_controlRef,flags);
} /* wxMacDataBrowserTableViewControl::GetSelectionFlags(DataBrowserSelectionFlags*) const */

bool wxMacDataBrowserTableViewControl::IsItemSelected(DataBrowserItemID item) const
{
  return ::IsDataBrowserItemSelected(this->m_controlRef,item);
} /* wxMacDataBrowserTableViewControl::IsItemSelected(DataBrowserItemID) const */

OSStatus wxMacDataBrowserTableViewControl::SetSelectionFlags(DataBrowserSelectionFlags flags)
{
  return ::SetDataBrowserSelectionFlags(this->m_controlRef,flags);
} /* wxMacDataBrowserTableViewControl::SetSelectionFlags(DataBrowserSelectionFlags) */

OSStatus wxMacDataBrowserTableViewControl::SetSelectedItems(UInt32 numItems, DataBrowserItemID const* items, DataBrowserSetOption operation)
{
  return ::SetDataBrowserSelectedItems( m_controlRef, numItems, items, operation );
} /* wxMacDataBrowserTableViewControl::SetSelectedItems(UInt32, DataBrowserItemID const*, DataBrowserSetOption) */

OSStatus wxMacDataBrowserTableViewControl::GetSortProperty( DataBrowserPropertyID *column ) const
{
    return GetDataBrowserSortProperty( m_controlRef , column );
}

OSStatus wxMacDataBrowserTableViewControl::SetSortProperty( DataBrowserPropertyID column )
{
    return SetDataBrowserSortProperty( m_controlRef , column );
}

OSStatus wxMacDataBrowserTableViewControl::GetSortOrder( DataBrowserSortOrder *order ) const
{
    return GetDataBrowserSortOrder( m_controlRef , order );
}

OSStatus wxMacDataBrowserTableViewControl::SetSortOrder( DataBrowserSortOrder order )
{
    return SetDataBrowserSortOrder( m_controlRef , order );
}

IMPLEMENT_ABSTRACT_CLASS(wxMacDataBrowserTableViewControl,wxMacControl)

// ============================================================================
// Databrowser class for the list view control
// ============================================================================
//
#pragma mark -
//
// column handling
//
OSStatus wxMacDataBrowserListViewControl::AddColumn(DataBrowserListViewColumnDesc *columnDesc, DataBrowserTableViewColumnIndex position)
{
  return AddDataBrowserListViewColumn(this->m_controlRef,columnDesc,position);
} /* wxMacDataBrowserListViewControl::AddColumn(DataBrowserListViewColumnDesc*, DataBrowserTableViewColumnIndex) */

// ============================================================================
// Databrowser class for the data view control
// ============================================================================
//
#pragma mark -
//
// constructors / destructor
//
wxMacDataViewDataBrowserListViewControl::wxMacDataViewDataBrowserListViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style)
                                        :wxMacDataBrowserListViewControl(peer,pos,size,style)
{
} /* wxMacDataViewDataBrowserListViewControl::wxMacDataViewDataBrowserListViewControl(wxWindow* , const wxPoint&, const wxSize&, long) */

//
// item ID and model matching
//
void wxMacDataViewDataBrowserListViewControl::ClearItemIDRowPairs(void)
{
  this->m_itemIDRowNumberMap.clear();
} /* wxMacDataViewDataBrowserListViewControl::ClearItemIDRowPairs(void) */

bool wxMacDataViewDataBrowserListViewControl::DeleteItemIDRowPair(DataBrowserItemID itemID)
{
  return (this->m_itemIDRowNumberMap.erase(itemID) == 1);
} /* wxMacDataViewDataBrowserListViewControl::DeleteItemIDRowPair(DataBrowseritemID) */

bool wxMacDataViewDataBrowserListViewControl::GetRowIndex(unsigned long& rowIndex, DataBrowserItemID itemID) const
{
  ItemIDRowNumberHashMap::const_iterator iteratorHashMap(this->m_itemIDRowNumberMap.find(itemID));
  
  
  if (iteratorHashMap == this->m_itemIDRowNumberMap.end())
    return false;
  else
  {
    rowIndex = iteratorHashMap->second;
    return true;
  } /* if */
} /* wxMacDataViewDataBrowserListViewControl::GetRowIndex(unsigned int& rowIndex, DataBrowserItemID itemID) const */

bool wxMacDataViewDataBrowserListViewControl::InsertItemIDRowPair(DataBrowserItemID itemID, unsigned long rowIndex)
{
  return this->m_itemIDRowNumberMap.insert(ItemIDRowNumberHashMap::value_type(itemID,rowIndex)).second;
} /* wxMacDataViewDataBrowserListViewControl::InsertItemIDRowPair(DataBrowserItemID, unsigned long) */

void wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndices(unsigned int* newIndices)
{
  ItemIDRowNumberHashMap::iterator hashMapIterator(this->m_itemIDRowNumberMap.begin());
  
  
  while (hashMapIterator != this->m_itemIDRowNumberMap.end())
  {
    hashMapIterator->second = newIndices[hashMapIterator->second];
    ++hashMapIterator;
  } /* while */
} /* wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndices(unsigned int*) */

void wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndicesDeletion (unsigned long decreaseFromIndex)
{
  ItemIDRowNumberHashMap::iterator hashMapIterator(this->m_itemIDRowNumberMap.begin());
  
  
  while (hashMapIterator != this->m_itemIDRowNumberMap.end())
  {
    if (hashMapIterator->second >= decreaseFromIndex)
      --(hashMapIterator->second);
    ++hashMapIterator;
  } /* while */
} /* wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndicesDeletion (unsigned int) */

void wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndicesInsertion(unsigned long increaseFromIndex)
{
  ItemIDRowNumberHashMap::iterator hashMapIterator(this->m_itemIDRowNumberMap.begin());
  
  
  while (hashMapIterator != this->m_itemIDRowNumberMap.end())
  {
    if (hashMapIterator->second >= increaseFromIndex)
      ++(hashMapIterator->second);
    ++hashMapIterator;
  } /* while */
} /* wxMacDataViewDataBrowserListViewControl::RenumberItemIDRowIndicesInsertion(unsigned int) */

void wxMacDataViewDataBrowserListViewControl::ReverseRowIndexNumbering(void)
{
  if (this->m_itemIDRowNumberMap.size() > 0)
  {
    size_t const maxIndex = this->m_itemIDRowNumberMap.size()-1;

    ItemIDRowNumberHashMap::iterator hashMapIterator(this->m_itemIDRowNumberMap.begin());
    
    
    while (hashMapIterator != this->m_itemIDRowNumberMap.end())
    {
      hashMapIterator->second = maxIndex-hashMapIterator->second;
      ++hashMapIterator;
    } /* while */
  } /* if */
} /* wxMacDataViewDataBrowserListViewControl::ReverseRowIndexNumbering(void) */

//
// callback functions (inherited from wxMacDataBrowserTableViewControl)
//
Boolean wxMacDataViewDataBrowserListViewControl::DataBrowserCompareProc(DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID WXUNUSED(sortProperty))
{
  unsigned long rowIndex1, rowIndex2;

  bool result = (this->GetRowIndex(rowIndex1,itemOneID) && this->GetRowIndex(rowIndex2,itemTwoID) && (rowIndex1 < rowIndex2));
  return result;
} /* wxMacDataViewDataBrowserListViewControl::DataBrowserCompareProc(DataBrowserItemID, DataBrowserItemID, DataBrowserPropertyID) */

void wxMacDataViewDataBrowserListViewControl::DataBrowserDrawItemProc(DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice)
{
  OSStatus errorStatus;

  unsigned long column;
  unsigned long row;

  wxDataViewColumn* dataViewColumnPtr;

  wxDataViewCtrl* dataViewCtrlPtr;
  
  wxDataViewCustomRenderer* dataViewCustomRendererPtr;

  wxVariant dataToRender;


  wxCHECK_RET(this->GetPropertyColumn(property,&column) == noErr,_("Could not determine column index."));
  dataViewCtrlPtr = dynamic_cast<wxDataViewCtrl*>(this->GetPeer());
  wxCHECK_RET(dataViewCtrlPtr != NULL,            _("Pointer to data view control not set correctly."));
  wxCHECK_RET(dataViewCtrlPtr->GetModel() != NULL,_("Pointer to model not set correctly."));
  if (dynamic_cast<wxDataViewSortedListModel*>(dataViewCtrlPtr->GetModel()) == NULL)
  {
    wxCHECK_RET(this->GetRowIndex(row,itemID),_("Row index does not exist."));
  } /* if */
  else
  {
    wxCHECK_RET(this->GetItemRow(itemID,&row) == noErr,_("Row index does not exist."));
  } /* if */
  dataViewColumnPtr = dataViewCtrlPtr->GetColumn(column);
  wxCHECK_RET(dataViewColumnPtr != NULL,_("No column for the specified column index existing."));
  dataViewCustomRendererPtr = dynamic_cast<wxDataViewCustomRenderer*>(dataViewColumnPtr->GetRenderer());
  wxCHECK_RET(dataViewCustomRendererPtr != NULL,_("No renderer or invalid renderer type specified for custom data column."));
  dataViewCtrlPtr->GetModel()->GetValue(dataToRender,column,row);
  dataViewCustomRendererPtr->SetValue(dataToRender);

 // try to determine the content's size (drawable part):
  Rect      content;
  RgnHandle rgn(NewRgn());
  UInt16    headerHeight;

  if (this->GetRegion(kControlContentMetaPart,rgn) == noErr)
    GetRegionBounds(rgn,&content);
  else
    this->GetRect(&content);
  ::DisposeRgn(rgn);
 // space for the header
  this->GetHeaderButtonHeight(&headerHeight);
  content.top += headerHeight;
 // extra space for the frame (todo: do not how to determine the space automatically from the control)
  content.top    += 5;
  content.left   += 5;
  content.right  -= 3;
  content.bottom -= 3;
 // extra space for the scrollbars: 
  content.bottom -= wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y);
  content.right  -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

 // make sure that 'Render' can draw only in the allowed area:  
  dataViewCustomRendererPtr->GetDC()->SetClippingRegion(content.left,content.top,content.right-content.left+1,content.bottom-content.top+1);
  (void) (dataViewCustomRendererPtr->Render(wxRect(static_cast<int>(rectangle->left),static_cast<int>(rectangle->top),
                                                   static_cast<int>(1+rectangle->right-rectangle->left),static_cast<int>(1+rectangle->bottom-rectangle->top)),
                                            dataViewCustomRendererPtr->GetDC(),((state == kDataBrowserItemIsSelected) ? wxDATAVIEW_CELL_SELECTED : 0)));
  dataViewCustomRendererPtr->GetDC()->DestroyClippingRegion(); // probably not necessary
} /* wxMacDataViewDataBrowserListViewControl::DataBrowserDrawItemProc(DataBrowserItemID, DataBrowserPropertyID, DataBrowserItemState, Rect const*, SInt16, Boolean) */

OSStatus wxMacDataViewDataBrowserListViewControl::DataBrowserGetSetItemDataProc(DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue)
{
  if (getValue)
  {
   // variable definition:
    unsigned long     column;
    unsigned long     row;
    wxDataViewCtrl*   dataViewCtrlPtr;
    wxDataViewColumn* dataViewColumnPtr;
    
    verify_noerr(this->GetPropertyColumn(property,&column));
    dataViewCtrlPtr = dynamic_cast<wxDataViewCtrl*>(this->GetPeer());
    wxCHECK_MSG(dataViewCtrlPtr != NULL,errDataBrowserNotConfigured,_("Pointer to data view control not set correctly."));
    if (dynamic_cast<wxDataViewSortedListModel*>(dataViewCtrlPtr->GetModel()) == NULL)
    {
      wxCHECK_MSG(this->GetRowIndex(row,itemID),errDataBrowserNotConfigured,_("Row index does not exist."));
    } /* if */
    else
    {
      wxCHECK_MSG(this->GetItemRow(itemID,&row) == noErr,errDataBrowserNotConfigured,_("Row index does not exist."));
    } /* if */
    dataViewColumnPtr = dataViewCtrlPtr->GetColumn(column);
    wxCHECK_MSG((dataViewColumnPtr != NULL) && (dataViewColumnPtr->GetRenderer() != NULL),errDataBrowserNotConfigured,_("There is no column for the specified column index."));
    switch (dataViewColumnPtr->GetRenderer()->GetPropertyType())
    {
      case kDataBrowserCheckboxType:
        {
         // variable definitions:
          ThemeButtonValue buttonValue;
          OSStatus         errorStatus;

          errorStatus = ::GetDataBrowserItemDataButtonValue(itemData,&buttonValue);
          if (errorStatus == noErr)
          {
            if (buttonValue == kThemeButtonOn)
            {
             // variable definition and initialization:
              wxVariant modifiedData(true);

              return (dataViewCtrlPtr->GetModel()->SetValue(modifiedData,static_cast<unsigned int>(column),static_cast<unsigned int>(row)) ? noErr : errDataBrowserNotConfigured);
            } /* if */
            else if (buttonValue == kThemeButtonOff)
            {
             // variable definition and initialization:
              wxVariant modifiedData(false);

              return (dataViewCtrlPtr->GetModel()->SetValue(modifiedData,static_cast<unsigned int>(column),static_cast<unsigned int>(row)) ? noErr : errDataBrowserNotConfigured);
            } /* if */
            else
              return errDataBrowserInvalidPropertyData;
          } /* if */
          else
            return errorStatus;
        } /* block */
      case kDataBrowserTextType:
        {
         // variable definitions:
          CFStringRef stringReference;
          OSStatus    errorStatus;

          errorStatus = ::GetDataBrowserItemDataText(itemData,&stringReference);
          if (errorStatus == noErr)
          {
           // variable definitions and initializations:
            wxMacCFStringHolder modifiedString(stringReference);
            wxVariant           modifiedData(modifiedString.AsString());

            if (dataViewCtrlPtr->GetModel()->SetValue(modifiedData,static_cast<unsigned int>(column),static_cast<unsigned int>(row)))
              return noErr;
            else
              return errDataBrowserNotConfigured;
          } /* if */
          else
            return errorStatus;
        } /* block */
      default:
        return errDataBrowserPropertyNotSupported;
    } /* switch */
  } /* if */
  else
  {
    if (property >= kMinPropertyID) // in case of data columns set the data
    {
     // variable definitions:
      OSStatus          errorStatus;
      unsigned long     columnPosition;
      unsigned long     rowPosition;
      wxVariant         variant;
      wxDataViewColumn* dataViewColumnPtr;
      wxDataViewCtrl*   dataViewCtrlPtr;
      
      errorStatus = this->GetPropertyColumn(property,&columnPosition);
      wxCHECK_MSG(errorStatus == noErr,errorStatus,_("Column index does not exist."));
      dataViewCtrlPtr = dynamic_cast<wxDataViewCtrl*>(this->GetPeer());wxCHECK_MSG(dataViewCtrlPtr   != NULL,errDataBrowserNotConfigured,_("Pointer to data view control not set correctly."));
      dataViewColumnPtr = dataViewCtrlPtr->GetColumn(columnPosition);  wxCHECK_MSG(dataViewColumnPtr != NULL,errDataBrowserNotConfigured,_("No column for the specified column position existing."));
      if (dynamic_cast<wxDataViewSortedListModel*>(dataViewCtrlPtr->GetModel()) == NULL)
      {
        wxCHECK_MSG(this->GetRowIndex(rowPosition,itemID),errDataBrowserNotConfigured,_("Row index does not exist."));
      } /* if */
      else
      {
        wxCHECK_MSG(this->GetItemRow(itemID,&rowPosition) == noErr,errDataBrowserNotConfigured,_("Row index does not exist."));
      } /* if */
      dataViewCtrlPtr->GetModel()->GetValue(variant,dataViewColumnPtr->GetModelColumn(),rowPosition);
      wxCHECK_MSG(dataViewColumnPtr->GetRenderer() != NULL,errDataBrowserNotConfigured,_("No renderer specified for column."));
      dataViewColumnPtr->GetRenderer()->SetDataReference(itemData);
      dataViewColumnPtr->GetRenderer()->SetValue(variant);
      wxCHECK_MSG(dataViewColumnPtr->GetRenderer()->Render(),errDataBrowserNotConfigured,_("Rendering failed."));
    } /* if */
    else // react on special system requests
      switch (property)
      {
        case kDataBrowserItemIsContainerProperty:
          return ::SetDataBrowserItemDataBooleanValue(itemData,false);
        case kDataBrowserItemIsEditableProperty:
          return ::SetDataBrowserItemDataBooleanValue(itemData,true);
      } /* switch */
  } /* if */
  return errDataBrowserPropertyNotSupported;
} /* wxMacDataViewDataBrowserListViewControl::DataBrowserGetSetItemDataProc(DataBrowserItemID, DataBrowserPropertyID, DataBrowserItemDataRef, Boolean) */

void wxMacDataViewDataBrowserListViewControl::DataBrowserItemNotificationProc(DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef WXUNUSED(itemData))
{
  switch (message)
  {
    case kDataBrowserItemDoubleClicked:
      {
       // variable definitions:
        unsigned long   row;
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->GetPeer()));
        
        wxCHECK_RET(dataViewCtrlPtr != NULL,      _("Pointer to data view control not set correctly."));
        wxCHECK_RET(this->GetRowIndex(row,itemID),_("Row index does not exist."));
       // initialize wxWidget event:
        wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_ROW_ACTIVATED,dataViewCtrlPtr->GetId()); // variable defintion

        dataViewEvent.SetEventObject(dataViewCtrlPtr);
        dataViewEvent.SetRow(row);
       // finally sent the equivalent wxWidget event:
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
      } /* block */
      break;
    case kDataBrowserItemSelected:
      {
       // variable definitions:
        unsigned long   row;
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->GetPeer()));
        
        wxCHECK_RET(dataViewCtrlPtr != NULL,      _("Pointer to data view control not set correctly."));
        if (dynamic_cast<wxDataViewSortedListModel*>(dataViewCtrlPtr->GetModel()) == NULL)
        {
          wxCHECK_RET(this->GetRowIndex(row,itemID),_("Row index does not exist."));
        } /* if */
        else
        {
          wxCHECK_RET(this->GetItemRow(itemID,&row) == noErr,_("Row index does not exist."));
        } /* if */
       // initialize wxWidget event:
        wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_ROW_SELECTED,dataViewCtrlPtr->GetId()); // variable defintion

        dataViewEvent.SetEventObject(dataViewCtrlPtr);
        dataViewEvent.SetRow(row);
       // finally sent the equivalent wxWidget event:
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
      } /* block */
      break;
    case kDataBrowserUserStateChanged:
      {
        DataBrowserPropertyID propertyId;
        
        if ((this->GetSortProperty(&propertyId) == noErr) && (propertyId >= kMinPropertyID))
        {
          DataBrowserSortOrder sortOrder;
          unsigned long        column;
          
          if ((this->GetSortOrder(&sortOrder) == noErr) && (this->GetPropertyColumn(propertyId,&column) == noErr))
          {
            wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->GetPeer()));

            if (  dataViewCtrlPtr->GetColumn(column)->IsSortOrderAscending()  && (sortOrder == kDataBrowserOrderDecreasing) ||
                !(dataViewCtrlPtr->GetColumn(column)->IsSortOrderAscending()) && (sortOrder == kDataBrowserOrderIncreasing))
            {
             // initialize wxWidget event:
              wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED,dataViewCtrlPtr->GetId()); // variable defintion

              dataViewEvent.SetEventObject(dataViewCtrlPtr);
              dataViewEvent.SetColumn(column);
              dataViewEvent.SetDataViewColumn(dataViewCtrlPtr->GetColumn(column));
             // finally sent the equivalent wxWidget event:
              dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
            } /* if */
          } /* if */
        } /* if */
      } /* block */
      break;
  } /* switch */
} /* wxMacDataViewDataBrowserListViewControl::DataBrowserItemNotificationProc(DataBrowserItemID, DataBrowserItemNotification, DataBrowserItemDataRef) */


#endif
 // wxUSE_GENERICDATAVIEWCTRL
