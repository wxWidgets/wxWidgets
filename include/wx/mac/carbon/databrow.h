/////////////////////////////////////////////////////////////////////////////
// Name:        include/mac/carbon/databrow.h
// Purpose:     Classes and functions for the Carbon data browser
// Author:      
// Modified by:
// Created:     2007-05-18
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATABROW_H_
#define _WX_DATABROW_H_

#include "wx/defs.h"

#ifdef __DARWIN__
#    include <Carbon/Carbon.h>
#else
#    include <Debugging.h>
#    include <Appearance.h>
#    include <Controls.h>
#    include <ControlDefinitions.h>
#    include <LowMem.h>
#    include <Gestalt.h>
#    include <CoreServices.h>
#endif

#if UNIVERSAL_INTERFACES_VERSION < 0x0342
    #error "please update to Apple's latest universal headers from http://developer.apple.com/sdk/"
#endif

#ifndef MAC_OS_X_VERSION_10_3
    #define MAC_OS_X_VERSION_10_3 1030
#endif

#ifndef MAC_OS_X_VERSION_10_4
    #define MAC_OS_X_VERSION_10_4 1040
#endif

#ifndef MAC_OS_X_VERSION_10_5
    #define MAC_OS_X_VERSION_10_5 1050
#endif

#ifdef __WXMAC_CARBON__
#  include "wx/mac/corefoundation/cfstring.h"
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
typedef UInt32 URefCon;
typedef SInt32 SRefCon;
#endif

#if wxUSE_GUI

#include "wx/hashmap.h"

#include "wx/mac/private.h"

WX_DEFINE_ARRAY_SIZE_T(size_t,wxArrayDataBrowserItemID);

// ============================================================================
// DataBrowser Wrapper
// ============================================================================
//
// basing on DataBrowserItemIDs
//

// data browser's property IDs have a reserved ID range from 0 - 1023
// therefore, the first usable property ID is 'kMinPropertyID'
DataBrowserPropertyID const kMinPropertyID = 1024;

class wxMacDataBrowserTableViewControl : public wxMacControl
{
public:
//
// constructors / destructor
//
  wxMacDataBrowserTableViewControl(wxWindow* peer, const wxPoint& pos, const wxSize& size, long style);
  wxMacDataBrowserTableViewControl(void)
  {
  }

//
// callback handling
//
  OSStatus SetCallbacks      (DataBrowserCallbacks const*       callbacks);
  OSStatus SetCustomCallbacks(DataBrowserCustomCallbacks const* customCallbacks);

//
// header handling
//
  OSStatus GetHeaderDesc(DataBrowserPropertyID property, DataBrowserListViewHeaderDesc* desc) const;

  OSStatus SetHeaderDesc(DataBrowserPropertyID property, DataBrowserListViewHeaderDesc* desc);

//
// layout handling
//
  OSStatus AutoSizeColumns();

  OSStatus EnableCellSizeModification(bool enableHeight=true, bool enableWidth=true); // enables or disables the column width and row height modification (default: false)

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  OSStatus GetAttributes        (OptionBits* attributes);
#endif
  OSStatus GetColumnWidth       (DataBrowserPropertyID column, UInt16 *width ) const; // returns the column width in pixels
  OSStatus GetDefaultColumnWidth(UInt16 *width ) const; // returns the default column width in pixels
  OSStatus GetDefaultRowHeight  (UInt16 * height ) const;
  OSStatus GetHeaderButtonHeight(UInt16 *height );
  OSStatus GetRowHeight         (DataBrowserItemID item , UInt16 *height) const;
  OSStatus GetScrollPosition    (UInt32* top, UInt32 *left) const;

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  OSStatus SetAttributes (OptionBits attributes);
#endif
  OSStatus SetColumnWidth(DataBrowserPropertyID column, UInt16 width); // sets the column width in pixels
  OSStatus SetDefaultColumnWidth( UInt16 width );
  OSStatus SetDefaultRowHeight( UInt16 height );
  OSStatus SetHasScrollBars( bool horiz, bool vert );
  OSStatus SetHeaderButtonHeight( UInt16 height );
  OSStatus SetHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle );
  OSStatus SetRowHeight( DataBrowserItemID item , UInt16 height);
  OSStatus SetScrollPosition( UInt32 top , UInt32 left );

//
// column handling
//
  OSStatus GetColumnCount   (UInt32* numColumns) const;
  OSStatus GetColumnID      (DataBrowserTableViewColumnIndex position, DataBrowserTableViewColumnID* id);
  OSStatus GetFreePropertyID(DataBrowserPropertyID* id) const; // this method returns a property id that is valid and currently not used; if it cannot be found 'errDataBrowerPropertyNotSupported' is returned
  OSStatus GetPropertyColumn(DataBrowserPropertyID propertyID, DataBrowserTableViewColumnIndex* index) const;
  OSStatus GetPropertyFlags (DataBrowserPropertyID propertyID, DataBrowserPropertyFlags *flags ) const;

  OSStatus IsUsedPropertyID(DataBrowserPropertyID propertyID) const; // checks if passed property id is used by the control; no error is returned if the id exists

  OSStatus RemoveColumn(DataBrowserTableViewColumnIndex position);

  OSStatus SetColumnPosition  (DataBrowserPropertyID column, DataBrowserTableViewColumnIndex position);
  OSStatus SetDisclosureColumn(DataBrowserPropertyID property, Boolean expandableRows );
  OSStatus SetPropertyFlags   (DataBrowserPropertyID property, DataBrowserPropertyFlags flags );

//
// item handling
//
  OSStatus AddItem(DataBrowserItemID container, DataBrowserItemID const* itemID) // adds a single item
  {
    return this->AddItems(container,1,itemID,kDataBrowserItemNoProperty);
  }
  OSStatus AddItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty); // adds items to the data browser

  OSStatus GetFreeItemID(DataBrowserItemID* id) const; // this method returns an item id that is valid and currently not used; if it cannot be found 'errDataBrowserItemNotAdded' is returned
  OSStatus GetItemCount (ItemCount* numItems) const
  {
    return this->GetItemCount(kDataBrowserNoItem,true,kDataBrowserItemAnyState,numItems);
  }
  OSStatus GetItemCount (DataBrowserItemID container, Boolean recurse, DataBrowserItemState state, ItemCount* numItems) const;
  OSStatus GetItemID    (DataBrowserTableViewRowIndex row, DataBrowserItemID * item) const;
  OSStatus GetItems     (DataBrowserItemID container, Boolean recurse, DataBrowserItemState state, Handle items) const;
  OSStatus GetItemRow   (DataBrowserItemID item, DataBrowserTableViewRowIndex* row) const;

  OSStatus IsUsedItemID(DataBrowserItemID itemID) const; // checks if the passed id is in use

  OSStatus RevealItem(DataBrowserItemID item, DataBrowserPropertyID propertyID, DataBrowserRevealOptions options) const;

  OSStatus RemoveItem(DataBrowserItemID container, DataBrowserItemID const* itemID) // removes a single item
  {
    return this->RemoveItems(container,1,itemID,kDataBrowserItemNoProperty);
  }
  OSStatus RemoveItems(void) // removes all items
  {
    return this->RemoveItems(kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
  }
  OSStatus RemoveItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty);

  OSStatus UpdateItems(void) // updates all items
  {
    return this->UpdateItems(kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty,kDataBrowserNoItem);
  }
  OSStatus UpdateItems(DataBrowserItemID const* item) // updates all columns of item
  {
    return this->UpdateItems(kDataBrowserNoItem,1,item,kDataBrowserItemNoProperty,kDataBrowserNoItem);
  }
  OSStatus UpdateItems(DataBrowserItemID container, UInt32 numItems, DataBrowserItemID const* items, DataBrowserPropertyID preSortProperty, DataBrowserPropertyID propertyID) const;

//
// item selection
//
  size_t   GetSelectedItemIDs(wxArrayDataBrowserItemID& itemIDs) const; // returns the number of selected item and the item IDs in the array
  OSStatus GetSelectionAnchor(DataBrowserItemID *first, DataBrowserItemID *last) const;
  OSStatus GetSelectionFlags (DataBrowserSelectionFlags* flags) const;

  bool IsItemSelected(DataBrowserItemID item) const;

  OSStatus SetSelectionFlags(DataBrowserSelectionFlags flags);
  OSStatus SetSelectedItems (UInt32 numItems, DataBrowserItemID const* itemIDs, DataBrowserSetOption operation);

//
// item sorting
//
  OSStatus GetSortOrder   (DataBrowserSortOrder* order) const;
  OSStatus GetSortProperty(DataBrowserPropertyID* column) const;

  OSStatus SetSortOrder   (DataBrowserSortOrder  order);
  OSStatus SetSortProperty(DataBrowserPropertyID column);

protected :
//
// callback functions
//
  static pascal Boolean  DataBrowserCompareProc         (ControlRef browser, DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty);
  static pascal void     DataBrowserDrawItemProc        (ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice);
  static pascal OSStatus DataBrowserGetSetItemDataProc  (ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue);
  static pascal void     DataBrowserItemNotificationProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData);

  virtual Boolean  DataBrowserCompareProc         (DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty) = 0;
  virtual void     DataBrowserDrawItemProc        (DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice) = 0;
  virtual OSStatus DataBrowserGetSetItemDataProc  (DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue) = 0;
  virtual void     DataBrowserItemNotificationProc(DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData) = 0;
private:
//
// wxWidget internal stuff
//
    DECLARE_ABSTRACT_CLASS(wxMacDataBrowserTableViewControl)
};

// ============================================================================
// Databrowser class for the list view control
// ============================================================================
//
class wxMacDataBrowserListViewControl : public wxMacDataBrowserTableViewControl
{
public:
//
// constructors / destructor
//
  wxMacDataBrowserListViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style) : wxMacDataBrowserTableViewControl(peer,pos,size,style)
  {
  }

//
// column handling
//
  OSStatus AddColumn(DataBrowserListViewColumnDesc *columnDesc, DataBrowserTableViewColumnIndex position);

protected:
private:
};


// ============================================================================
// Databrowser class for the data view list control model
// ============================================================================
//

//
// Hash maps used by the data browser for the data view model
//
WX_DECLARE_HASH_MAP(DataBrowserItemID,unsigned long,wxIntegerHash,wxIntegerEqual,ItemIDRowNumberHashMap); // stores for each item ID the model's row number

class wxMacDataViewDataBrowserListViewControl : public wxMacDataBrowserListViewControl
{
public:
//
// constructors / destructor
  wxMacDataViewDataBrowserListViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style);

//
// item ID and model matching
//
  void ClearItemIDRowPairs(void); // removes all ID row pairs

  bool DeleteItemIDRowPair(DataBrowserItemID itemID); // if the id has been removed, 'true' is returned, 'false' is returned if the id did not exist or another error occurred

  bool GetRowIndex(unsigned long& rowIndex, DataBrowserItemID itemID) const; // returns 'true' and the rowIndex if the id has been found in the map, otherwise 'false' is returned and rowIndex remains untouched

  bool InsertItemIDRowPair(DataBrowserItemID itemID, unsigned long rowIndex); // the pair will only be inserted if the item ID does not exist before the call

  void RenumberItemIDRowIndices         (unsigned int* newIndices);        // for each item ID - row index pair the row index is changed
  void RenumberItemIDRowIndicesDeletion (unsigned long decreaseFromIndex); // decreases all row indices by one that are equal or larger than 'decreaseFromIndex'
  void RenumberItemIDRowIndicesInsertion(unsigned long increaseFromIndex); // increases all row indices by one that are equal or larger than 'increaseFromIndex'

  void ReverseRowIndexNumbering(void); // reverses the order of the indices

protected:
//
// callback functions (inherited from wxMacDataBrowserTableViewControl)
//
  virtual Boolean  DataBrowserCompareProc         (DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty);
  virtual void     DataBrowserDrawItemProc        (DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice);
  virtual void     DataBrowserItemNotificationProc(DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData);
  virtual OSStatus DataBrowserGetSetItemDataProc  (DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue);

private:
//
// variables
//
  ItemIDRowNumberHashMap m_itemIDRowNumberMap; // contains for each data browser ID the corresponding line number in the model
};

typedef wxMacDataViewDataBrowserListViewControl* wxMacDataViewDataBrowserListViewControlPointer;


#endif // WX_GUI
  
#endif // _WX_DATABROW_H_
