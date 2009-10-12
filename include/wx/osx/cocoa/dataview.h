/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/carbon/dataview.h
// Purpose:     wxDataViewCtrl native implementation header for carbon
// Author:
// Id:          $Id: dataview.h 57374 2009-01-27
// Copyright:   (c) 2009
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAVIEWCTRL_COCOOA_H_
#define _WX_DATAVIEWCTRL_COCOOA_H_

#include "wx/defs.h"

#if wxUSE_GUI

#ifdef __OBJC__
  #import <Cocoa/Cocoa.h>
#endif

#include "wx/osx/core/dataview.h"
#include "wx/osx/private.h"

// Forward declaration
class wxCocoaDataViewControl;

// ============================================================================
// wxPointerObject
// ============================================================================
//
// This is a helper class to store a pointer in an object. This object just
// stores the pointer but does not take any ownership.
// To pointer objects are equal if the containing pointers are equal. This
// means also that the hash value of a pointer object depends only on the
// stored pointer.
//
@interface wxPointerObject : NSObject
{
  void* pointer;
}

 //
 // object initialization
 //
  -(id) initWithPointer:(void*)initPointer;

 //
 // access to pointer
 //
  -(void*) pointer;
  -(void)  setPointer:(void*)newPointer;

@end

// ============================================================================
// wxSortDescriptorObject
// ============================================================================
//
// This is a helper class to use native sorting facilities.
//
@interface wxSortDescriptorObject : NSSortDescriptor<NSCopying>
{
  wxDataViewColumn* columnPtr; // pointer to the sorting column

  wxDataViewModel* modelPtr; // pointer to model
}

 //
 // initialization
 //
  -(id) initWithModelPtr:(wxDataViewModel*)initModelPtr sortingColumnPtr:(wxDataViewColumn*)initColumnPtr ascending:(BOOL)sortAscending;

 //
 // access to variables
 //
  -(wxDataViewColumn*) columnPtr;
  -(wxDataViewModel*) modelPtr;

  -(void) setColumnPtr:(wxDataViewColumn*)newColumnPtr;
  -(void) setModelPtr:(wxDataViewModel*)newModelPtr;

@end

// ============================================================================
// wxDataViewColumnNativeData
// ============================================================================
class wxDataViewColumnNativeData
{
public:
//
// constructors / destructor
//
  wxDataViewColumnNativeData(void) : m_NativeColumnPtr(NULL)
  {
  }
  wxDataViewColumnNativeData(NSTableColumn* initNativeColumnPtr) : m_NativeColumnPtr(initNativeColumnPtr)
  {
  }

//
// data access methods
//
  NSTableColumn* GetNativeColumnPtr() const
  {
    return m_NativeColumnPtr;
  }

  void SetNativeColumnPtr(NSTableColumn* newNativeColumnPtr)
  {
    m_NativeColumnPtr = newNativeColumnPtr;
  }

protected:
private:
//
// variables
//
  NSTableColumn* m_NativeColumnPtr; // this class does not take over ownership of the pointer nor retains it
};

// ============================================================================
// wxDataViewRendererNativeData
// ============================================================================
class wxDataViewRendererNativeData
{
public:
//
// constructors / destructor
//
  wxDataViewRendererNativeData(void) : m_Object(NULL), m_ColumnCell(NULL)
  {
      Init();
  }
  wxDataViewRendererNativeData(NSCell* initColumnCell) : m_Object(NULL), m_ColumnCell([initColumnCell retain])
  {
      Init();
  }
  wxDataViewRendererNativeData(NSCell* initColumnCell, id initObject) : m_Object([initObject retain]), m_ColumnCell([initColumnCell retain])
  {
      Init();
  }

  ~wxDataViewRendererNativeData()
  {
    [m_ColumnCell release];
    [m_Object     release];

    [m_origFont release];
    [m_origTextColour release];
  }

 //
 // data access methods
 //
  NSCell* GetColumnCell() const
  {
    return m_ColumnCell;
  }
  NSTableColumn* GetColumnPtr() const
  {
    return m_TableColumnPtr;
  }
  id GetItem() const
  {
    return m_Item;
  }
  NSCell* GetItemCell() const
  {
    return m_ItemCell;
  }
  id GetObject() const
  {
    return m_Object;
  }

  void SetColumnCell(NSCell* newCell)
  {
    [newCell retain];
    [m_ColumnCell release];
    m_ColumnCell = newCell;
  }
  void SetColumnPtr(NSTableColumn* newColumnPtr)
  {
    m_TableColumnPtr = newColumnPtr;
  }
  void SetItem(id newItem)
  {
    m_Item = newItem;
  }
  void SetItemCell(NSCell* newCell)
  {
    m_ItemCell = newCell;
  }
  void SetObject(id newObject)
  {
    [newObject retain];
    [m_Object release];
    m_Object = newObject;
  }

  // The original cell font and text colour stored here are NULL by default and
  // are only initialized to the values retrieved from the cell when we change
  // them from wxCocoaOutlineView:willDisplayCell:forTableColumn:item: which
  // calls our SaveOriginalXXX() methods before changing the cell attributes.
  //
  // This allows us to avoid doing anything for the columns without any
  // attributes but still be able to restore the correct attributes for the
  // ones that do.
  NSFont *GetOriginalFont() const { return m_origFont; }
  NSColor *GetOriginalTextColour() const { return m_origTextColour; }

  void SaveOriginalFont(NSFont *font)
  {
      m_origFont = [font retain];
  }

  void SaveOriginalTextColour(NSColor *textColour)
  {
      m_origTextColour = [textColour retain];
  }

private:
  void Init()
  {
      m_origFont = NULL;
      m_origTextColour = NULL;
  }

  id m_Item;   // item NOT owned by renderer
  id m_Object; // object that can be used by renderer for storing special data (owned by renderer)

  NSCell* m_ColumnCell; // column's cell is owned by renderer
  NSCell* m_ItemCell;   // item's cell is NOT owned by renderer

  NSTableColumn* m_TableColumnPtr; // column NOT owned by renderer

  // we own those if they're non-NULL
  NSFont *m_origFont;
  NSColor *m_origTextColour;
};

// ============================================================================
// wxCocoaOutlineDataSource
// ============================================================================
//
// This class implements the data source delegate for the outline view.
// As only an informal protocol exists this class inherits from NSObject only.
//
// As mentioned in the documentation for NSOutlineView the native control does
// not own any data. Therefore, it has to be done by the data source.
// Unfortunately, wxWidget's data source is a C++ data source but
// NSOutlineDataSource requires objects as data. Therefore, the data (or better
// the native item objects) have to be stored additionally in the native data
// source.
// NSOutlineView requires quick access to the item objects and quick linear
// access to an item's children. This requires normally a hash type of storage
// for the item object itself and an array structure for each item's children.
// This means that basically two times the whole structure of wxWidget's model
// class has to be stored.
// This implementation is using a compromise: all items that are in use by the
// control are stored in a set (from there they can be easily retrieved) and
// owned by the set. Furthermore, children of the last parent are stored
// in a linear list.
//
@interface wxCocoaOutlineDataSource : NSObject
{
  NSArray* sortDescriptors; // descriptors specifying the sorting (currently the array only holds one object only)

  NSMutableArray* children; // buffered children

  NSMutableSet* items; // stores all items that are in use by the control

  wxCocoaDataViewControl* implementation;

  wxDataViewModel* model;

  wxPointerObject* currentParentItem; // parent of the buffered children; the object is owned
}

 //
 // methods of informal protocol:
 //
  -(BOOL)            outlineView:(NSOutlineView*)outlineView acceptDrop:(id<NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index;
  -(id)              outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item;
  -(id)              outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn*)tableColumn byItem:(id)item;
  -(BOOL)            outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item;
  -(NSInteger)       outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item;
  -(NSDragOperation) outlineView:(NSOutlineView*)outlineView validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index;
  -(BOOL)            outlineView:(NSOutlineView*)outlineView writeItems:(NSArray*)items toPasteboard:(NSPasteboard*)pasteboard;

 //
 // buffer for items handling
 //
  -(void)             addToBuffer:(wxPointerObject*)item;
  -(void)             clearBuffer;
  -(wxPointerObject*) getDataViewItemFromBuffer:(wxDataViewItem const&)item;  // returns the item in the buffer that has got the same pointer as "item",
  -(wxPointerObject*) getItemFromBuffer:(wxPointerObject*)item;               // if such an item does not exist nil is returned
  -(BOOL)             isInBuffer:(wxPointerObject*)item;
  -(void)             removeFromBuffer:(wxPointerObject*)item;

 //
 // buffered children handling
 //
  -(void)             appendChild:(wxPointerObject*)item;
  -(void)             clearChildren;
  -(wxPointerObject*) getChild:(NSUInteger)index;
  -(NSUInteger)       getChildCount;
  -(void)             removeChild:(NSUInteger)index;

 //
 // buffer handling
 //
  -(void) clearBuffers;

 //
 // sorting
 //
  -(NSArray*) sortDescriptors;
  -(void)     setSortDescriptors:(NSArray*)newSortDescriptors;

 //
 // access to wxWidget's variables
 //
  -(wxPointerObject*)        currentParentItem;
  -(wxCocoaDataViewControl*) implementation;
  -(wxDataViewModel*)        model;
  -(void)                    setCurrentParentItem:(wxPointerObject*)newCurrentParentItem;
  -(void)                    setImplementation:(wxCocoaDataViewControl*)newImplementation;
  -(void)                    setModel:(wxDataViewModel*)newModel;

 //
 // other methods
 //
  -(void) bufferItem:(wxPointerObject*)parentItem withChildren:(wxDataViewItemArray*)dataViewChildrenPtr;

@end

// ============================================================================
// wxCustomCell
// ============================================================================
//
// This is a cell that is used for custom renderers.
//
@interface wxCustomCell : NSTextFieldCell
{
}

 //
 // other methods
 //
  -(NSSize) cellSize;

@end

// ============================================================================
// wxImageTextCell
// ============================================================================
//
// As the native cocoa environment does not have a cell displaying an icon/
// image and text at the same time, it has to be implemented by the user.
// This implementation follows the implementation of Chuck Pisula in Apple's
// DragNDropOutline sample application.
// Although in wxDataViewCtrl icons are used on OSX icons do not exist for
// display. Therefore, the cell is also called wxImageTextCell.
// Instead of displaying images of any size (which is possible) this cell uses
// a fixed size for displaying the image. Larger images are scaled to fit
// into their reserved space. Smaller or not existing images use the fixed
// reserved size and are scaled if necessary.
//
@interface wxImageTextCell : NSTextFieldCell
{
@private
  CGFloat xImageShift;    // shift for the image in x-direction from border
  CGFloat spaceImageText; // space between image and text ("belongs" to the image)

  NSImage* image; // the image itself

  NSSize imageSize; // largest size of the image; default size is (16, 16)

  NSTextAlignment cellAlignment; // the text alignment is used to align the whole
                                 // cell (image and text)
}

 //
 // alignment
 //
  -(NSTextAlignment) alignment;
  -(void) setAlignment:(NSTextAlignment)newAlignment;

 //
 // image access
 //
  -(NSImage*) image;
  -(void) setImage:(NSImage*)newImage;

 //
 // size access
 //
  -(NSSize) imageSize;
  -(void) setImageSize:(NSSize) newImageSize;

 //
 // other methods
 //
  -(NSSize) cellSize;

@end

// ============================================================================
// wxCocoaOutlineView
// ============================================================================
@interface wxCocoaOutlineView : NSOutlineView
{
@private
  BOOL isEditingCell; // flag indicating if a cell is currently being edited

  wxCocoaDataViewControl* implementation;
}

 //
 // access to wxWidget's implementation
 //
  -(wxCocoaDataViewControl*) implementation;
  -(void)                    setImplementation:(wxCocoaDataViewControl*) newImplementation;

@end

// ============================================================================
// wxCocoaDataViewControl
// ============================================================================
//
// This is the internal interface class between wxDataViewCtrl (wxWidget) and
// the native source view (Mac OS X cocoa).
//
class wxCocoaDataViewControl : public wxWidgetCocoaImpl, public wxDataViewWidgetImpl
{
public:
 //
 // constructors / destructor
 //
  wxCocoaDataViewControl(wxWindow* peer, wxPoint const& pos, wxSize const& size, long style);
  ~wxCocoaDataViewControl();

 //
 // column related methods (inherited from wxDataViewWidgetImpl)
 //
  virtual bool              ClearColumns       ();
  virtual bool              DeleteColumn       (wxDataViewColumn* columnPtr);
  virtual void              DoSetExpanderColumn(wxDataViewColumn const* columnPtr);
  virtual wxDataViewColumn* GetColumn          (unsigned int pos) const;
  virtual int               GetColumnPosition  (wxDataViewColumn const* columnPtr) const;
  virtual bool              InsertColumn       (unsigned int pos, wxDataViewColumn* columnPtr);

 //
 // item related methods (inherited from wxDataViewWidgetImpl)
 //
  virtual bool         Add          (wxDataViewItem const& parent, wxDataViewItem const& item);
  virtual bool         Add          (wxDataViewItem const& parent, wxDataViewItemArray const& items);
  virtual void         Collapse     (wxDataViewItem const& item);
  virtual void         EnsureVisible(wxDataViewItem const& item, wxDataViewColumn const* columnPtr);
  virtual void         Expand       (wxDataViewItem const& item);
  virtual unsigned int GetCount     () const;
  virtual wxRect       GetRectangle (wxDataViewItem const& item, wxDataViewColumn const* columnPtr);
  virtual bool         IsExpanded   (wxDataViewItem const& item) const;
  virtual bool         Reload       ();
  virtual bool         Remove       (wxDataViewItem const& parent, wxDataViewItem const& item);
  virtual bool         Remove       (wxDataViewItem const& parent, wxDataViewItemArray const& item);
  virtual bool         Update       (wxDataViewColumn const* columnPtr);
  virtual bool         Update       (wxDataViewItem const& parent, wxDataViewItem const& item);
  virtual bool         Update       (wxDataViewItem const& parent, wxDataViewItemArray const& items);

 //
 // model related methods
 //
  virtual bool AssociateModel(wxDataViewModel* model); // informs the native control that a model is present

 //
 // selection related methods (inherited from wxDataViewWidgetImpl)
 //
  virtual int  GetSelections(wxDataViewItemArray& sel)   const;
  virtual bool IsSelected   (wxDataViewItem const& item) const;
  virtual void Select       (wxDataViewItem const& item);
  virtual void SelectAll    ();
  virtual void Unselect     (wxDataViewItem const& item);
  virtual void UnselectAll  ();

 //
 // sorting related methods
 //
  virtual wxDataViewColumn* GetSortingColumn () const;
  virtual void              Resort           ();

 //
 // other methods (inherited from wxDataViewWidgetImpl)
 //
  virtual void DoSetIndent (int indent);
  virtual void HitTest     (wxPoint const& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const;
  virtual void SetRowHeight(wxDataViewItem const& item, unsigned int height);
  virtual void OnSize      ();

 //
 // other methods
 //
  wxDataViewCtrl* GetDataViewCtrl() const
  {
    return dynamic_cast<wxDataViewCtrl*>(GetWXPeer());
  }

//
// drag & drop helper methods
//
  wxDataFormat           GetDnDDataFormat(wxDataObjectComposite* dataObjects);
  wxDataObjectComposite* GetDnDDataObjects(NSData* dataObject) const; // create the data objects from the native dragged object
protected:
private:
//
// variables
//
  wxCocoaOutlineDataSource* m_DataSource;

  wxCocoaOutlineView* m_OutlineView;
};

typedef wxCocoaDataViewControl* wxCocoaDataViewControlPointer;


#endif // wxUSE_GUI
#endif // _WX_DATAVIEWCTRL_COCOOA_H_
