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

#if wxUSE_GUI

#include "wx/osx/private.h"

WX_DEFINE_ARRAY_SIZE_T(size_t,wxArrayDataBrowserItemID);

// ============================================================================
// wxMacDataBrowserTableViewControl
// ============================================================================
//
// this is a wrapper class for the Mac OS X data browser environment,
// it covers all general data brower functionality,
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
  OSStatus GetPartBounds        (DataBrowserItemID item, DataBrowserPropertyID property, DataBrowserPropertyPart part, Rect* bounds);
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
  OSStatus SetIndent(float Indent);
  OSStatus SetRowHeight( DataBrowserItemID item , UInt16 height);
  OSStatus SetScrollPosition( UInt32 top , UInt32 left );

//
// column handling
//
  OSStatus GetColumnCount   (UInt32* numColumns) const;
  OSStatus GetColumnIndex   (DataBrowserPropertyID propertyID, DataBrowserTableViewColumnIndex* index) const; // returns for the passed property the corresponding column index
  OSStatus GetFreePropertyID(DataBrowserPropertyID* propertyID) const; // this method returns a property id that is valid and currently not used; if it cannot be found 'errDataBrowerPropertyNotSupported' is returned
  OSStatus GetPropertyFlags (DataBrowserPropertyID propertyID, DataBrowserPropertyFlags *flags ) const;
  OSStatus GetPropertyID    (DataBrowserItemDataRef itemData, DataBrowserPropertyID* propertyID); // returns for the passed item data reference the corresponding property ID
  OSStatus GetPropertyID    (DataBrowserTableViewColumnIndex index, DataBrowserPropertyID* propertyID); // returns for the passed column index the corresponding property ID
  
  OSStatus IsUsedPropertyID(DataBrowserPropertyID propertyID) const; // checks if passed property id is used by the control; no error is returned if the id exists

  OSStatus RemoveColumnByProperty(DataBrowserTableViewColumnID propertyID);
  OSStatus RemoveColumnByIndex   (DataBrowserTableViewColumnIndex index);

  OSStatus SetColumnIndex     (DataBrowserPropertyID propertyID, DataBrowserTableViewColumnIndex index);
  OSStatus SetDisclosureColumn(DataBrowserPropertyID propertyID, Boolean expandableRows=false);
  OSStatus SetPropertyFlags   (DataBrowserPropertyID propertyID, DataBrowserPropertyFlags flags);

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
  OSStatus GetItemID    (DataBrowserTableViewRowIndex row, DataBrowserItemID* item) const;
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
    return this->UpdateItems(kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty,kDataBrowserItemNoProperty);
  }
  OSStatus UpdateItems(DataBrowserItemID const* item) // updates all columns of item
  {
    return this->UpdateItems(kDataBrowserNoItem,1,item,kDataBrowserItemNoProperty,kDataBrowserItemNoProperty);
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
  OSStatus GetSortProperty(DataBrowserPropertyID* propertyID) const;

  OSStatus Resort(DataBrowserItemID container=kDataBrowserNoItem, Boolean sortChildren=true);

  OSStatus SetSortOrder   (DataBrowserSortOrder  order);
  OSStatus SetSortProperty(DataBrowserPropertyID propertyID);

//
// container handling
//
  OSStatus CloseContainer(DataBrowserItemID containerID);
  
  OSStatus OpenContainer(DataBrowserItemID containerID);

protected :
//
// standard callback functions
//
  static pascal Boolean  DataBrowserCompareProc          (ControlRef browser, DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty);
  static pascal void     DataBrowserGetContextualMenuProc(ControlRef browser, MenuRef* menu, UInt32* helpType, CFStringRef* helpItemString, AEDesc* selection);
  static pascal OSStatus DataBrowserGetSetItemDataProc   (ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue);
  static pascal void     DataBrowserItemNotificationProc (ControlRef browser, DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData);

  virtual Boolean  DataBrowserCompareProc          (DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty) = 0;
  virtual void     DataBrowserGetContextualMenuProc(MenuRef* menu, UInt32* helpType, CFStringRef* helpItemString, AEDesc* selection) = 0;
  virtual OSStatus DataBrowserGetSetItemDataProc   (DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean getValue) = 0;
  virtual void     DataBrowserItemNotificationProc (DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData) = 0;

//
// callback functions for customized types
//
  static pascal void                      DataBrowserDrawItemProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID propertyID, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice);
  static pascal Boolean                   DataBrowserEditItemProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID propertyID, CFStringRef theString, Rect* maxEditTextRect, Boolean* shrinkToFit);
  static pascal Boolean                   DataBrowserHitTestProc (ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Rect const* mouseRect);
  static pascal DataBrowserTrackingResult DataBrowserTrackingProc(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Point startPt, EventModifiers modifiers);

  virtual void                      DataBrowserDrawItemProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice) = 0;
  virtual Boolean                   DataBrowserEditItemProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, CFStringRef theString, Rect* maxEditTextRect, Boolean* shrinkToFit) = 0;
  virtual Boolean                   DataBrowserHitTestProc (DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Rect const* mouseRect) = 0;
  virtual DataBrowserTrackingResult DataBrowserTrackingProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Point startPt, EventModifiers modifiers) = 0;

//
// callback functions for drag & drop
///
  static pascal Boolean DataBrowserAcceptDragProc (ControlRef browser, DragReference dragRef, DataBrowserItemID itemID);
  static pascal Boolean DataBrowserAddDragItemProc(ControlRef browser, DragReference dragRef, DataBrowserItemID itemID, ItemReference* itemRef);
  static pascal Boolean DataBrowserReceiveDragProc(ControlRef browser, DragReference dragRef, DataBrowserItemID itemID);

  virtual Boolean DataBrowserAcceptDragProc (DragReference dragRef, DataBrowserItemID itemID)                         = 0;
  virtual Boolean DataBrowserAddDragItemProc(DragReference dragRef, DataBrowserItemID itemID, ItemReference* itemRef) = 0;
  virtual Boolean DataBrowserReceiveDragProc(DragReference dragRef, DataBrowserItemID itemID)                         = 0;

private:
//
// wxWidget internal stuff
//
    DECLARE_ABSTRACT_CLASS(wxMacDataBrowserTableViewControl)
};

// ============================================================================
// wxMacDataBrowserListViewControl
// ============================================================================
//
// this class is a wrapper for a list view which incorporates all general
// data browser functionality of the inherited table view control class;
// the term list view is in this case Mac OS X specific and is not related
// to any wxWidget naming conventions
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
// wxMacDataViewDataBrowserListViewControl
// ============================================================================
//
// internal interface class between wxDataViewCtrl (wxWidget) and the data
// browser (Mac OS X)
//
class wxMacDataViewDataBrowserListViewControl : public wxMacDataBrowserListViewControl
{
public:
//
// constructors / destructor
//
  wxMacDataViewDataBrowserListViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style);

protected:
//
// standard callback functions (inherited from wxMacDataBrowserTableViewControl)
//
  virtual Boolean  DataBrowserCompareProc          (DataBrowserItemID itemOneID, DataBrowserItemID itemTwoID, DataBrowserPropertyID sortProperty);
  virtual void     DataBrowserItemNotificationProc (DataBrowserItemID itemID, DataBrowserItemNotification message, DataBrowserItemDataRef itemData);
  virtual void     DataBrowserGetContextualMenuProc(MenuRef* menu, UInt32* helpType, CFStringRef* helpItemString, AEDesc* selection);
  virtual OSStatus DataBrowserGetSetItemDataProc   (DataBrowserItemID itemID, DataBrowserPropertyID propertyID, DataBrowserItemDataRef itemData, Boolean getValue);

//
// callback functions for customized types (inherited from wxMacDataBrowserTableViewControl)
//
  virtual void                      DataBrowserDrawItemProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, DataBrowserItemState state, Rect const* rectangle, SInt16 bitDepth, Boolean colorDevice);
  virtual Boolean                   DataBrowserEditItemProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, CFStringRef theString, Rect* maxEditTextRect, Boolean* shrinkToFit);
  virtual Boolean                   DataBrowserHitTestProc (DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Rect const* mouseRect);
  virtual DataBrowserTrackingResult DataBrowserTrackingProc(DataBrowserItemID itemID, DataBrowserPropertyID propertyID, Rect const* theRect, Point startPt, EventModifiers modifiers);

//
// callback functions for drag & drop (inherited from wxMacDataBrowserTableViewControl)
//
  virtual Boolean DataBrowserAcceptDragProc (DragReference dragRef, DataBrowserItemID itemID);
  virtual Boolean DataBrowserAddDragItemProc(DragReference dragRef, DataBrowserItemID itemID, ItemReference* itemRef);
  virtual Boolean DataBrowserReceiveDragProc(DragReference dragRef, DataBrowserItemID itemID);

//
// drag & drop helper methods
//
  wxDataFormat           GetDnDDataFormat(wxDataObjectComposite* dataObjects);
  wxDataObjectComposite* GetDnDDataObjects(DragReference dragRef, ItemReference itemRef) const; // create the data objects from the native dragged object

private:
};

typedef wxMacDataViewDataBrowserListViewControl* wxMacDataViewDataBrowserListViewControlPointer;


#endif // WX_GUI
  
#endif // _WX_DATABROW_H_
