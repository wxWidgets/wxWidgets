
/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/dataview.h
// Purpose:     wxDataViewCtrl native implementation header for OSX
// Author:
// Copyright:   (c) 2009
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAVIEWCTRL_CORE_H_
#define _WX_DATAVIEWCTRL_CORE_H_

#include "wx/dataview.h"

typedef wxWidgetImpl wxWidgetImplType;

// ---------------------------------------------------------
// Helper functions for dataview implementation on OSX
// ---------------------------------------------------------
wxWidgetImplType* CreateDataView(wxWindowMac* wxpeer, wxWindowMac* parent,  wxWindowID id,
                                 wxPoint const& pos, wxSize const& size,
                                 long style, long extraStyle);
wxString ConcatenateDataViewItemValues(wxDataViewCtrl const* dataViewCtrlPtr, wxDataViewItem const& dataViewItem); // concatenates all data of the visible columns of the passed control
                                                                                                                   // and item TAB separated into a string and returns it

// ---------------------------------------------------------
// wxDataViewWidgetImpl
// Common interface of the native dataview implementation
// for the carbon and cocoa environment.
// ATTENTION
//  All methods assume that the passed column pointers are
//  valid (unless a NULL pointer is explicitly allowed
//  to be passed)!
// ATTENTION
// ---------------------------------------------------------
class WXDLLIMPEXP_CORE wxDataViewWidgetImpl
{
public:
 //
 // constructors / destructor
 //
  virtual ~wxDataViewWidgetImpl()
  {
  }

 //
 // column related methods
 //
  virtual bool              ClearColumns()                                                     = 0; // deletes all columns in the native control
  virtual bool              DeleteColumn       (wxDataViewColumn* columnPtr)                   = 0; // deletes the column in the native control
  virtual void              DoSetExpanderColumn(wxDataViewColumn const* columnPtr)             = 0; // sets the disclosure column in the native control
  virtual wxDataViewColumn* GetColumn          (unsigned int pos) const                        = 0; // returns the column belonging to 'pos' in the native control
  virtual int               GetColumnPosition  (wxDataViewColumn const* columnPtr) const       = 0; // returns the position of the passed column in the native control
  virtual bool              InsertColumn       (unsigned int pos, wxDataViewColumn* columnPtr) = 0; // inserts a column at pos in the native control;
                                                                                                    // the method can assume that the column's owner is already set
  virtual void              FitColumnWidthToContent(unsigned int pos)                          = 0; // resizes column to fit its content

 //
 // item related methods
 //
  virtual bool         Add          (wxDataViewItem const& parent, wxDataViewItem const& item)       = 0; // adds an item to the native control
  virtual bool         Add          (wxDataViewItem const& parent, wxDataViewItemArray const& items) = 0; // adds items to the native control
  virtual void         Collapse     (wxDataViewItem const& item)                                     = 0; // collapses the passed item in the native control
  virtual void         EnsureVisible(wxDataViewItem const& item, wxDataViewColumn const* columnPtr)  = 0; // ensures that the passed item's value in the passed column is visible (column pointer can be NULL)
  virtual unsigned int GetCount() const                                                              = 0; // returns the number of items in the native control
  virtual int          GetCountPerPage() const                                                       = 0; // get number of items that fit into a single page
  virtual wxRect       GetRectangle (wxDataViewItem const& item, wxDataViewColumn const* columnPtr)  = 0; // returns the rectangle that is used by the passed item and column in the native control
  virtual wxDataViewItem GetTopItem() const                                                          = 0; // get top-most visible item
  virtual bool         IsExpanded   (wxDataViewItem const& item) const                               = 0; // checks if the passed item is expanded in the native control
  virtual bool         Reload()                                                                      = 0; // clears the native control and reloads all data
  virtual bool         Remove       (wxDataViewItem const& parent)                                   = 0; // removes one or more items under the given parent from the native control
  virtual bool         Update       (wxDataViewColumn const* columnPtr)                              = 0; // updates the items in the passed column of the native control
  virtual bool         Update       (wxDataViewItem const& parent, wxDataViewItem const& item)       = 0; // updates the passed item in the native control
  virtual bool         Update       (wxDataViewItem const& parent, wxDataViewItemArray const& items) = 0; // updates the passed items in the native control

 //
 // model related methods
 //
  virtual bool AssociateModel(wxDataViewModel* model) = 0; // informs the native control that a model is present

 //
 // selection related methods
 //
  virtual wxDataViewItem GetCurrentItem() const = 0;
  virtual void SetCurrentItem(const wxDataViewItem& item) = 0;

  virtual wxDataViewColumn *GetCurrentColumn() const = 0;

  virtual int  GetSelectedItemsCount() const = 0;
  virtual int  GetSelections(wxDataViewItemArray& sel)   const = 0; // returns all selected items in the native control
  virtual bool IsSelected   (wxDataViewItem const& item) const = 0; // checks if the passed item is selected in the native control
  virtual void Select       (wxDataViewItem const& item)       = 0; // selects the passed item in the native control
  virtual void Select       (wxDataViewItemArray const& items) = 0; // selects the passed items in the native control
  virtual void SelectAll()                                     = 0; // selects all items in the native control
  virtual void Unselect     (wxDataViewItem const& item)       = 0; // unselects the passed item in the native control
  virtual void UnselectAll()                                   = 0; // unselects all items in the native control

 //
 // sorting related methods
 //
  virtual wxDataViewColumn* GetSortingColumn() const      = 0; // returns the column that is primarily responsible for sorting in the native control
  virtual void              Resort()                      = 0; // asks the native control to start a resorting process

 //
 // other methods
 //
  virtual void DoSetIndent (int indent)                                                                     = 0; // sets the indentation in the native control
  virtual void DoExpand    (wxDataViewItem const& item)                                                     = 0; // expands the passed item in the native control

  virtual void HitTest     (wxPoint const& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const = 0; // return the item and column pointer that contains with the passed point
  virtual void SetRowHeight(int height)                                                                     = 0; // sets the height of all rows
  virtual void SetRowHeight(wxDataViewItem const& item, unsigned int height)                                = 0; // sets the height of the row containing the passed item in the native control
  virtual void OnSize()                                                                                     = 0; // updates the layout of the native control after a size event
  virtual void StartEditor( const wxDataViewItem & item, unsigned int column )                              = 0; // starts editing the passed in item and column
};

#endif // _WX_DATAVIEWCTRL_CORE_H_
