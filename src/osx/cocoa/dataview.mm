///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dataview.mm
// Purpose:     wxDataView
// Author:
// Modified by:
// Created:     2009-01-31
// Copyright:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#if !defined(wxHAS_GENERIC_DATAVIEWCTRL)

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/font.h"
    #include "wx/settings.h"
    #include "wx/utils.h"
#endif

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"
#include "wx/osx/cocoa/dataview.h"
#include "wx/renderer.h"
#include "wx/stopwatch.h"
#include "wx/dcgraph.h"

#if wxUSE_MARKUP
    #include "wx/osx/cocoa/private/markuptoattr.h"
#endif // wxUSE_MARKUP

// ============================================================================
// Constants used locally
// ============================================================================

#define DataViewPboardType @"OutlineViewItem"

static const int MINIMUM_NATIVE_ROW_HEIGHT = 17;


// ============================================================================
// Classes used locally in dataview.mm
// ============================================================================

// ============================================================================
// wxPointerObject
// ============================================================================

@implementation wxPointerObject

-(id) init
{
    self = [super init];
    if (self != nil)
        self->pointer = NULL;
    return self;
}

-(id) initWithPointer:(void*) initPointer
{
    self = [super init];
    if (self != nil)
        self->pointer = initPointer;
    return self;
}

//
// inherited methods from NSObject
//
-(BOOL) isEqual:(id)object
{
    return (object != nil) &&
             ([object isKindOfClass:[wxPointerObject class]]) &&
                 (pointer == [((wxPointerObject*) object) pointer]);
}

-(NSUInteger) hash
{
    return (NSUInteger) pointer;
}

-(void*) pointer
{
    return pointer;
}

-(void) setPointer:(void*) newPointer
{
    pointer = newPointer;
}

@end

namespace
{

inline wxDataViewItem wxDataViewItemFromItem(id item)
{
    return wxDataViewItem([static_cast<wxPointerObject *>(item) pointer]);
}

inline wxDataViewItem wxDataViewItemFromMaybeNilItem(id item)
{
    return item == nil ? wxDataViewItem() : wxDataViewItemFromItem(item);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxCustomRendererObject
// ----------------------------------------------------------------------------

@interface wxCustomRendererObject : NSObject <NSCopying>
{
@public
    wxDataViewCustomRenderer* customRenderer; // not owned by the class
}

    -(id) init;
    -(id) initWithRenderer:(wxDataViewCustomRenderer*)renderer;
@end

@implementation wxCustomRendererObject

-(id) init
{
    self = [super init];
    if (self != nil)
    {
        customRenderer = NULL;
    }
    return self;
}

-(id) initWithRenderer:(wxDataViewCustomRenderer*)renderer
{
    self = [super init];
    if (self != nil)
    {
        customRenderer = renderer;
    }
    return self;
}

-(id) copyWithZone:(NSZone*)zone
{
    wxCustomRendererObject* copy;

    copy = [[[self class] allocWithZone:zone] init];
    copy->customRenderer = customRenderer;

    return copy;
}
@end

// ----------------------------------------------------------------------------
// wxDVCNSTableColumn: exists only to override NSTableColumn:dataCellForRow:
// ----------------------------------------------------------------------------

@interface wxDVCNSTableColumn : NSTableColumn
{
}

    // Get the identifier we use for the specified column. This should be used
    // for finding columns from identifier only, to initialize the identifier
    // of a new column use initWithColumnPointer below instead.
    +(NSString*) identifierForColumnPointer:(const wxDataViewColumn*)column;

    // Initialize the column with the given pointer to the associated
    // wxDataViewColumn. This pointer can later be retrieved using
    // getColumnPointer.
    -(id) initWithColumnPointer:(const wxDataViewColumn*)column;

    // Retrieve the associated column.
    -(wxDataViewColumn*) getColumnPointer;

    -(id) dataCellForRow:(NSInteger)row;
@end

@implementation wxDVCNSTableColumn

+(NSString*) identifierForColumnPointer:(const wxDataViewColumn*)column
{
    // Starting from OS X 10.7 the column identifier must be an NSString and
    // not just some arbitrary object, so we serialize the pointer into the
    // string. Notice the use of NSInteger which is big enough to store a
    // pointer in both 32 and 64 bit builds.
    return [NSString stringWithFormat:@"%lu",
                (unsigned long)reinterpret_cast<NSUInteger>(column)];
}

-(id) initWithColumnPointer:(const wxDataViewColumn*)column
{
    self = [self initWithIdentifier: [wxDVCNSTableColumn identifierForColumnPointer:column]];
    return self;
}

-(wxDataViewColumn*) getColumnPointer
{
    // The case to NSString is needed for OS X < 10.7.
    return reinterpret_cast<wxDataViewColumn*>(
            [static_cast<NSString*>([self identifier]) integerValue]);
}

-(id) dataCellForRow:(NSInteger)row
{
    // what we want to do here is to simply return nil for the cells which
    // shouldn't show anything as otherwise we would show e.g. empty combo box
    // or progress cells in the columns using the corresponding types even for
    // the container rows which is wrong

    const wxDataViewColumn * const dvCol = [self getColumnPointer];

    const wxDataViewCtrl * const dvc = dvCol->GetOwner();
    const wxCocoaDataViewControl * const
        peer = static_cast<wxCocoaDataViewControl *>(dvc->GetPeer());


    // once we do have everything, simply ask NSOutlineView for the item...
    const id item = peer->GetItemAtRow(row);
    if ( item )
    {
        // ... and if it succeeded, ask the model whether it has any value
        wxDataViewItem dvItem(wxDataViewItemFromItem(item));

        if ( !dvc->GetModel()->HasValue(dvItem, dvCol->GetModelColumn()) )
            return nil;
    }

    return [super dataCellForRow:row];
}

@end

// ============================================================================
// local helpers
// ============================================================================

namespace
{

// convert from NSObject to different C++ types: all these functions check
// that the conversion really makes sense and assert if it doesn't
wxString ObjectToString(NSObject *object)
{
    wxCHECK_MSG( [object isKindOfClass:[NSString class]], "",
                 wxString::Format
                 (
                    "string expected but got %s",
                    wxCFStringRef::AsString([object className])
                 ));

    return wxCFStringRef([((NSString*) object) retain]).AsString();
}

bool ObjectToBool(NSObject *object)
{
    // actually the value must be of NSCFBoolean class but it's private so we
    // can't check for it directly
    wxCHECK_MSG( [object isKindOfClass:[NSNumber class]], false,
                 wxString::Format
                 (
                    "number expected but got %s",
                    wxCFStringRef::AsString([object className])
                 ));

    return [(NSNumber *)object boolValue];
}

long ObjectToLong(NSObject *object)
{
    wxCHECK_MSG( [object isKindOfClass:[NSNumber class]], -1,
                 wxString::Format
                 (
                    "number expected but got %s",
                    wxCFStringRef::AsString([object className])
                 ));

    return [(NSNumber *)object longValue];
}

wxDateTime ObjectToDate(NSObject *object)
{
    wxCHECK_MSG( [object isKindOfClass:[NSDate class]], wxInvalidDateTime,
                 wxString::Format
                 (
                    "date expected but got %s",
                    wxCFStringRef::AsString([object className])
                 ));

    // get the number of seconds since 1970-01-01 UTC and this is the only
    // way to convert a double to a wxLongLong
    const wxLongLong seconds = [((NSDate*) object) timeIntervalSince1970];

    wxDateTime dt(1, wxDateTime::Jan, 1970);
    dt.Add(wxTimeSpan(0,0,seconds));

    // the user has entered a date in the local timezone but seconds
    // contains the number of seconds from date in the local timezone
    // since 1970-01-01 UTC; therefore, the timezone information has to be
    // transferred to wxWidgets, too:
    dt.MakeFromTimezone(wxDateTime::UTC);

    return dt;
}

NSInteger CompareItems(id item1, id item2, void* context)
{
    NSArray* const sortDescriptors = (NSArray*) context;

    NSUInteger const count = [sortDescriptors count];

    NSInteger result = NSOrderedSame;
    for ( NSUInteger i = 0; i < count && result == NSOrderedSame; ++i )
    {
        wxSortDescriptorObject* const
            sortDescriptor = (wxSortDescriptorObject*)
                [sortDescriptors objectAtIndex:i];

        int rc = [sortDescriptor modelPtr]->Compare
                 (
                     wxDataViewItemFromItem(item1),
                     wxDataViewItemFromItem(item2),
                     [sortDescriptor columnPtr]->GetModelColumn(),
                     [sortDescriptor ascending] == YES
                 );

        if ( rc < 0 )
            result = NSOrderedAscending;
        else if ( rc > 0 )
            result = NSOrderedDescending;
    }

    return result;
}

NSTextAlignment ConvertToNativeHorizontalTextAlignment(int alignment)
{
    if (alignment & wxALIGN_CENTER_HORIZONTAL)
        return NSCenterTextAlignment;
    else if (alignment & wxALIGN_RIGHT)
        return NSRightTextAlignment;
    else
        return NSLeftTextAlignment;
}

NSTableColumn* CreateNativeColumn(const wxDataViewColumn *column)
{
    wxDataViewRenderer * const renderer = column->GetRenderer();

    wxCHECK_MSG( renderer, NULL, "column should have a renderer" );

    wxDVCNSTableColumn * const nativeColumn(
        [[wxDVCNSTableColumn alloc] initWithColumnPointer: column]
    );

    // setting the size related parameters:
    int resizingMask;
    if (column->IsResizeable())
    {
        resizingMask = NSTableColumnUserResizingMask | NSTableColumnAutoresizingMask;
        [nativeColumn setMinWidth:column->GetMinWidth()];
        [nativeColumn setMaxWidth:column->GetMaxWidth()];
    }
    else // column is not resizable [by user]
    {
        // if the control doesn't show a header, make the columns resize
        // automatically, this is particularly important for the single column
        // controls (such as wxDataViewTreeCtrl) as their unique column should
        // always take up all the available splace
        resizingMask = column->GetOwner()->HasFlag(wxDV_NO_HEADER)
                            ? NSTableColumnAutoresizingMask
                            : NSTableColumnNoResizing;
    }
    [nativeColumn setResizingMask:resizingMask];

    // setting the visibility:
    [nativeColumn setHidden:static_cast<BOOL>(column->IsHidden())];

    wxDataViewRendererNativeData * const renderData = renderer->GetNativeData();

    // setting the header:
    [[nativeColumn headerCell] setAlignment:
        ConvertToNativeHorizontalTextAlignment(column->GetAlignment())];
    [[nativeColumn headerCell] setStringValue:
        wxCFStringRef(column->GetTitle()).AsNSString()];
    renderData->ApplyLineBreakMode([nativeColumn headerCell]);

    // setting data cell's properties:
    [[nativeColumn dataCell] setWraps:NO];
    // setting the default data cell:
    [nativeColumn setDataCell:renderData->GetColumnCell()];

    if (!renderData->HasCustomFont())
        [renderData->GetColumnCell() setFont:column->GetOwner()->GetFont().OSXGetNSFont()];

    // setting the editablility:
    const bool isEditable = renderer->GetMode() == wxDATAVIEW_CELL_EDITABLE;

    [nativeColumn setEditable:isEditable];
    [[nativeColumn dataCell] setEditable:isEditable];

    return nativeColumn;
}

} // anonymous namespace

// ============================================================================
// Public helper functions for dataview implementation on OSX
// ============================================================================

wxWidgetImplType* CreateDataView(wxWindowMac* wxpeer,
                                 wxWindowMac* WXUNUSED(parent),
                                 wxWindowID WXUNUSED(id),
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 long WXUNUSED(extraStyle))
{
    return new wxCocoaDataViewControl(wxpeer,pos,size,style);
}

// ============================================================================
// wxSortDescriptorObject
// ============================================================================

@implementation wxSortDescriptorObject
-(id) init
{
    self = [super init];
    if (self != nil)
    {
        columnPtr = NULL;
        modelPtr  = NULL;
    }
    return self;
}

-(id)
initWithModelPtr:(wxDataViewModel*)initModelPtr
    sortingColumnPtr:(wxDataViewColumn*)initColumnPtr
    ascending:(BOOL)sortAscending
{
    self = [super initWithKey:@"dummy" ascending:sortAscending];
    if (self != nil)
    {
        columnPtr = initColumnPtr;
        modelPtr  = initModelPtr;
    }
    return self;
}

-(id) copyWithZone:(NSZone*)zone
{
    wxSortDescriptorObject* copy;


    copy = [super copyWithZone:zone];
    copy->columnPtr = columnPtr;
    copy->modelPtr  = modelPtr;

    return copy;
}

//
// access to model column's index
//
-(wxDataViewColumn*) columnPtr
{
    return columnPtr;
}

-(wxDataViewModel*) modelPtr
{
    return modelPtr;
}

-(void) setColumnPtr:(wxDataViewColumn*)newColumnPtr
{
    columnPtr = newColumnPtr;
}

-(void) setModelPtr:(wxDataViewModel*)newModelPtr
{
    modelPtr = newModelPtr;
}

@end

// ============================================================================
// wxCocoaOutlineDataSource
// ============================================================================
@implementation wxCocoaOutlineDataSource

//
// constructors / destructor
//
-(id) init
{
    self = [super init];
    if (self != nil)
    {
        implementation = NULL;
        model          = NULL;

        currentParentItem = nil;

        sortDescriptors = nil;

        children = [[NSMutableArray alloc] init];
        items    = [[NSMutableSet   alloc] init];
    }
    return self;
}

-(void) dealloc
{
    [sortDescriptors release];

    [currentParentItem release];

    [children release];
    [items    release];

    [super dealloc];
}

//
// methods of informal protocol:
//
-(BOOL)
outlineView:(NSOutlineView*)outlineView
    acceptDrop:(id<NSDraggingInfo>)info
    item:(id)item childIndex:(NSInteger)index
{
    wxUnusedVar(outlineView);
    
    return [self setupAndCallDataViewEvents:wxEVT_DATAVIEW_ITEM_DROP dropInfo:info item:item proposedChildIndex:index] != NSDragOperationNone;
}

-(id) outlineView:(NSOutlineView*)outlineView
    child:(NSInteger)index
    ofItem:(id)item
{
    wxUnusedVar(outlineView);

    if ((item == currentParentItem) &&
            (index < ((NSInteger) [self getChildCount])))
        return [self getChild:index];

    wxDataViewItemArray dataViewChildren;

    wxCHECK_MSG( model, 0, "Valid model in data source does not exist." );
    model->GetChildren(wxDataViewItemFromMaybeNilItem(item), dataViewChildren);
    [self bufferItem:item withChildren:&dataViewChildren];
    if ([sortDescriptors count] > 0)
        [children sortUsingFunction:CompareItems context:sortDescriptors];
    return [self getChild:index];
}

-(BOOL) outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item
{
    wxUnusedVar(outlineView);

    wxCHECK_MSG( model, 0, "Valid model in data source does not exist." );
    return model->IsContainer(wxDataViewItemFromItem(item));
}

-(NSInteger) outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item
{
    wxUnusedVar(outlineView);

    NSInteger noOfChildren;

    wxDataViewItemArray dataViewChildren;


    wxCHECK_MSG( model, 0, "Valid model in data source does not exist." );
    noOfChildren = model->GetChildren(wxDataViewItemFromMaybeNilItem(item),
                                      dataViewChildren);
    [self bufferItem:item withChildren:&dataViewChildren];
    if ([sortDescriptors count] > 0)
        [children sortUsingFunction:CompareItems context:sortDescriptors];
    return noOfChildren;
}

-(id)
outlineView:(NSOutlineView*)outlineView
    objectValueForTableColumn:(NSTableColumn*)tableColumn
    byItem:(id)item
{
    wxUnusedVar(outlineView);

    wxCHECK_MSG( model, nil, "Valid model in data source does not exist." );

    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);
    const unsigned colIdx = col->GetModelColumn();

    wxDataViewItem dataViewItem(wxDataViewItemFromItem(item));

    if ( model->HasValue(dataViewItem, colIdx) )
    {
        wxVariant value;
        model->GetValue(value,dataViewItem, colIdx);
        col->GetRenderer()->SetValue(value);
    }

    return nil;
}

-(void)
outlineView:(NSOutlineView*)outlineView
    setObjectValue:(id)object
    forTableColumn:(NSTableColumn*)tableColumn
    byItem:(id)item
{
    wxUnusedVar(outlineView);

    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);

    col->GetRenderer()->
        OSXOnCellChanged(object, wxDataViewItemFromItem(item), col->GetModelColumn());
}

-(void) outlineView:(NSOutlineView*)outlineView sortDescriptorsDidChange:(NSArray*)oldDescriptors
{
    wxUnusedVar(oldDescriptors);
    
    // Warning: the new sort descriptors are guaranteed to be only of type
    // NSSortDescriptor! Therefore, the sort descriptors for the data source
    // have to be converted.
    NSArray* newDescriptors;

    NSMutableArray* wxSortDescriptors;

    NSUInteger noOfDescriptors;

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();


    // convert NSSortDescriptors to wxSortDescriptorObjects:
    newDescriptors    = [outlineView sortDescriptors];
    noOfDescriptors   = [newDescriptors count];
    wxSortDescriptors = [NSMutableArray arrayWithCapacity:noOfDescriptors];
    for (NSUInteger i=0; i<noOfDescriptors; ++i)
    {
        NSSortDescriptor* const newDescriptor = [newDescriptors objectAtIndex:i];
        wxDataViewColumn* const sortingColumn = dvc->GetColumn([[newDescriptor key] intValue]);

        sortingColumn->SetSortOrderVariable([newDescriptor ascending]);

        [wxSortDescriptors addObject:[[[wxSortDescriptorObject alloc] initWithModelPtr:model
            sortingColumnPtr:sortingColumn
            ascending:[newDescriptor ascending]] autorelease]];
    }
    [(wxCocoaOutlineDataSource*)[outlineView dataSource] setSortDescriptors:wxSortDescriptors];

    // send first the event to wxWidgets that the sorting has changed so that
    // the program can do special actions before the sorting actually starts:
    wxDataViewColumn* const col = noOfDescriptors > 0
                                    ? [[wxSortDescriptors objectAtIndex:0] columnPtr]
                                    : NULL;
    wxDataViewEvent event(wxEVT_DATAVIEW_COLUMN_SORTED, dvc, col);
    dvc->GetEventHandler()->ProcessEvent(event);

    // start re-ordering the data;
    // children's buffer must be cleared first because it contains the old order:
    [self clearChildren];
    // sorting is done while reloading the data:
    [outlineView reloadData];
}

-(NSDragOperation) outlineView:(NSOutlineView*)outlineView validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index
{
    wxUnusedVar(outlineView);

    return [self setupAndCallDataViewEvents:wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE dropInfo:info item:item proposedChildIndex:index];
}

-(NSDragOperation) callDataViewEvents:(wxEventType)eventType dataObjects:(wxDataObjectComposite*)dataObjects item:(id)item
                   proposedChildIndex:(NSInteger)index
{
    NSDragOperation dragOperation = NSDragOperationNone;
    wxDataViewCtrl* const dvc(implementation->GetDataViewCtrl());
    wxDataViewEvent event(eventType, dvc, wxDataViewItemFromItem(item));
    if (dataObjects && (dataObjects->GetFormatCount() > 0))
    {
        // copy data into data object:
        event.SetDataObject(dataObjects);
        event.SetDataFormat(implementation->GetDnDDataFormat(dataObjects));
        event.SetProposedDropIndex(index);
        if (index == -1)
        {
            event.SetDropEffect(wxDragCopy);
        }
        else
        {
            //if index is not -1, we're going to set the default
            //for the drop effect to None to be compatible with
            //the other wxPlatforms that don't support it.  In the
            //user code for for the event, they can set this to
            //copy/move or similar to support it.
            event.SetDropEffect(wxDragNone);
        }
        wxDataFormatId formatId = event.GetDataFormat().GetType();
        wxMemoryBuffer buffer;
        
        // copy data into buffer:
        if ( formatId != wxDF_INVALID)
        {
            size_t size = dataObjects->GetDataSize(formatId);
            
            event.SetDataSize(size);
            dataObjects->GetDataHere(formatId,buffer.GetWriteBuf(size));
            buffer.UngetWriteBuf(size);
            event.SetDataBuffer(buffer.GetData());
        }
        
        // finally, send event:
        if (dvc->HandleWindowEvent(event) && event.IsAllowed())
        {
            switch (event.GetDropEffect())
            {
                case wxDragCopy:
                    dragOperation = NSDragOperationCopy;
                    break;
                case wxDragMove:
                    dragOperation = NSDragOperationMove;
                    break;
                case wxDragLink:
                    dragOperation = NSDragOperationLink;
                    break;
                case wxDragNone:
                case wxDragCancel:
                case wxDragError:
                    dragOperation = NSDragOperationNone;
                    break;
                default:
                    dragOperation = NSDragOperationEvery;
            }
        }
        else
        {
            dragOperation = NSDragOperationNone;
        }
    }
    else
    {
        dragOperation = NSDragOperationNone;
    }

    return dragOperation;
}

-(NSDragOperation) setupAndCallDataViewEvents:(wxEventType)eventType dropInfo:(id<NSDraggingInfo>)info item:(id)item
                           proposedChildIndex:(NSInteger)index
{
    NSArray* supportedTypes(
                            [NSArray arrayWithObjects:DataViewPboardType,NSStringPboardType,nil]
                            );
    
    NSPasteboard* pasteboard([info draggingPasteboard]);
    
    NSString* bestType([pasteboard availableTypeFromArray:supportedTypes]);
    
    if ( bestType == nil )
        return NSDragOperationNone;
    
    NSDragOperation dragOperation = NSDragOperationNone;
    wxDataViewCtrl* const dvc(implementation->GetDataViewCtrl());
    
    wxCHECK_MSG(dvc, false, "Pointer to data view control not set correctly.");
    wxCHECK_MSG(dvc->GetModel(), false, "Pointer to model not set correctly.");
    
    // wxDataViewEvent event(eventType, dvc, wxDataViewItemFromItem(item));
    if ([bestType compare:DataViewPboardType] == NSOrderedSame)
    {
        NSArray*               dataArray((NSArray*)[pasteboard propertyListForType:DataViewPboardType]);
        NSUInteger             indexDraggedItem, noOfDraggedItems([dataArray count]);
        
        indexDraggedItem = 0;
        while (indexDraggedItem < noOfDraggedItems)
        {
            wxDataObjectComposite* dataObjects(implementation->GetDnDDataObjects((NSData*)[dataArray objectAtIndex:indexDraggedItem]));
            
            dragOperation = [self callDataViewEvents:eventType dataObjects:dataObjects item:item proposedChildIndex:index];
            
            if ( dragOperation != NSDragOperationNone )
                ++indexDraggedItem;
            else
                indexDraggedItem = noOfDraggedItems;
            
            // clean-up:
            delete dataObjects;
        }
    }
    else
    {
        // needed to convert internally used UTF-16 representation to a UTF-8
        // representation
        CFDataRef              osxData;
        wxDataObjectComposite* dataObjects   (new wxDataObjectComposite());
        wxTextDataObject*      textDataObject(new wxTextDataObject());
        
        osxData = ::CFStringCreateExternalRepresentation(kCFAllocatorDefault,(CFStringRef)[pasteboard stringForType:NSStringPboardType],
#if defined(wxNEEDS_UTF16_FOR_TEXT_DATAOBJ)
                                                         kCFStringEncodingUTF16,
#else
                                                         kCFStringEncodingUTF8,
#endif
                                                         32);
        if (textDataObject->SetData(::CFDataGetLength(osxData),::CFDataGetBytePtr(osxData)))
            dataObjects->Add(textDataObject);
        else
            delete textDataObject;
        // send event if data could be copied:
        
        dragOperation = [self callDataViewEvents:eventType dataObjects:dataObjects item:item proposedChildIndex:index];

        // clean up:
        ::CFRelease(osxData);
        delete dataObjects;
    }
    
    return dragOperation;
}

-(BOOL) outlineView:(NSOutlineView*)outlineView writeItems:(NSArray*)writeItems toPasteboard:(NSPasteboard*)pasteboard
{
    wxUnusedVar(outlineView);

    // the pasteboard will be filled up with an array containing the data as
    // returned by the events (including the data type) and a concatenation of
    // text (string) data; the text data will only be put onto the pasteboard
    // if for all items a string representation exists
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    wxDataViewItemArray dataViewItems;


    wxCHECK_MSG(dvc, false,"Pointer to data view control not set correctly.");
    wxCHECK_MSG(dvc->GetModel(),false,"Pointer to model not set correctly.");

    if ([writeItems count] > 0)
    {
        bool            dataStringAvailable(true); // a flag indicating if for all items a data string is available
        NSMutableArray* dataArray = [NSMutableArray arrayWithCapacity:[writeItems count]]; // data of all items
        wxString        dataString; // contains the string data of all items

        // send a begin drag event for all selected items and proceed with
        // dragging unless the event is vetoed:
        for (size_t itemCounter=0; itemCounter<[writeItems count]; ++itemCounter)
        {
            bool                   itemStringAvailable(false);              // a flag indicating if for the current item a string is available
            wxDataObjectComposite* itemObject(new wxDataObjectComposite()); // data object for current item
            wxString               itemString;                              // contains the TAB concatenated data of an item

            const wxDataViewItem item = wxDataViewItemFromItem([writeItems objectAtIndex:itemCounter]);
            wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, dvc, item);
            itemString = ::ConcatenateDataViewItemValues(dvc, item);
            itemObject->Add(new wxTextDataObject(itemString));
            event.SetDataObject(itemObject);
            // check if event has not been vetoed:
            if (dvc->HandleWindowEvent(event) && event.IsAllowed() && (event.GetDataObject()->GetFormatCount() > 0))
            {
                size_t const noOfFormats = event.GetDataObject()->GetFormatCount();
                wxDataFormat* dataFormats(new wxDataFormat[noOfFormats]);

                event.GetDataObject()->GetAllFormats(dataFormats,wxDataObject::Get);
                for (size_t formatCounter=0; formatCounter<noOfFormats; ++formatCounter)
                {
                    // constant definitions for abbreviational purposes:
                    wxDataFormatId const idDataFormat = dataFormats[formatCounter].GetType();
                    size_t const dataSize       = event.GetDataObject()->GetDataSize(idDataFormat);
                    size_t const dataBufferSize = sizeof(wxDataFormatId)+dataSize;
                    // variable definitions (used in all case statements):
                    // give additional headroom for trailing NULL
                    wxMemoryBuffer dataBuffer(dataBufferSize+4);

                    dataBuffer.AppendData(&idDataFormat,sizeof(wxDataFormatId));
                    switch (idDataFormat)
                    {
                        case wxDF_TEXT:
                            // otherwise wxDF_UNICODETEXT already filled up
                            // the string; and the UNICODE representation has
                            // priority
                            if (!itemStringAvailable)
                            {
                                event.GetDataObject()->GetDataHere(wxDF_TEXT,dataBuffer.GetAppendBuf(dataSize));
                                dataBuffer.UngetAppendBuf(dataSize);
                                [dataArray addObject:[NSData dataWithBytes:dataBuffer.GetData() length:dataBufferSize]];
                                itemString = wxString(static_cast<char const*>(dataBuffer.GetData())+sizeof(wxDataFormatId),wxConvLocal);
                                itemStringAvailable = true;
                            }
                            break;
                        case wxDF_UNICODETEXT:
                            {
                                event.GetDataObject()->GetDataHere(wxDF_UNICODETEXT,dataBuffer.GetAppendBuf(dataSize));
                                dataBuffer.UngetAppendBuf(dataSize);
                                if (itemStringAvailable) // does an object already exist as an ASCII text (see wxDF_TEXT case statement)?
                                    [dataArray replaceObjectAtIndex:itemCounter withObject:[NSData dataWithBytes:dataBuffer.GetData() length:dataBufferSize]];
                                else
                                    [dataArray addObject:[NSData dataWithBytes:dataBuffer.GetData() length:dataBufferSize]];
                                itemString = wxString::FromUTF8(static_cast<char const*>(dataBuffer.GetData())+sizeof(wxDataFormatId),dataSize);
                                itemStringAvailable = true;
                            } /* block */
                            break;
                        default:
                            wxFAIL_MSG("Data object has invalid or unsupported data format");
                            return NO;
                    }
                }
                delete[] dataFormats;
                delete itemObject;
                if (dataStringAvailable)
                {
                    if (itemStringAvailable)
                    {
                        if (itemCounter > 0)
                            dataString << wxT('\n');
                        dataString << itemString;
                    }
                    else
                        dataStringAvailable = false;
                }
            }
            else
            {
                delete itemObject;
                return NO; // dragging was vetoed or no data available
            }
        }
        if (dataStringAvailable)
        {
            wxCFStringRef osxString(dataString);

            [pasteboard declareTypes:[NSArray arrayWithObjects:DataViewPboardType,NSStringPboardType,nil] owner:nil];
            [pasteboard setPropertyList:dataArray forType:DataViewPboardType];
            [pasteboard setString:osxString.AsNSString() forType:NSStringPboardType];
        }
        else
        {
            [pasteboard declareTypes:[NSArray arrayWithObject:DataViewPboardType] owner:nil];
            [pasteboard setPropertyList:dataArray forType:DataViewPboardType];
        }
        return YES;
    }
    else
        return NO; // no items to drag (should never occur)
}

//
// buffer handling
//
-(void) addToBuffer:(wxPointerObject*)item
{
    [items addObject:item];
}

-(void) clearBuffer
{
    [items removeAllObjects];
}

-(wxPointerObject*) getDataViewItemFromBuffer:(const wxDataViewItem&)item
{
    return [items member:[[[wxPointerObject alloc] initWithPointer:item.GetID()] autorelease]];
}

-(wxPointerObject*) getItemFromBuffer:(wxPointerObject*)item
{
    return [items member:item];
}

-(BOOL) isInBuffer:(wxPointerObject*)item
{
    return [items containsObject:item];
}

-(void) removeFromBuffer:(wxPointerObject*)item
{
    [items removeObject:item];
}

//
// children handling
//
-(void) clearChildren
{
    [children removeAllObjects];
}

-(wxPointerObject*) getChild:(NSUInteger)index
{
    return [children objectAtIndex:index];
}

-(NSUInteger) getChildCount
{
    return [children count];
}

//
// buffer handling
//
-(void) clearBuffers
{
    [self clearBuffer];
    [self clearChildren];
    [self setCurrentParentItem:nil];
}

//
// sorting
//
-(NSArray*) sortDescriptors
{
    return sortDescriptors;
}

-(void) setSortDescriptors:(NSArray*)newSortDescriptors
{
    [newSortDescriptors retain];
    [sortDescriptors release];
    sortDescriptors = newSortDescriptors;
}

//
// access to wxWidget's implementation
//
-(wxPointerObject*) currentParentItem
{
    return currentParentItem;
}

-(wxCocoaDataViewControl*) implementation
{
    return implementation;
}

-(wxDataViewModel*) model
{
    return model;
}

-(void) setCurrentParentItem:(wxPointerObject*)newCurrentParentItem
{
    [newCurrentParentItem retain];
    [currentParentItem release];
    currentParentItem = newCurrentParentItem;
}

-(void) setImplementation:(wxCocoaDataViewControl*) newImplementation
{
    implementation = newImplementation;
}

-(void) setModel:(wxDataViewModel*) newModel
{
    model = newModel;
}

//
// other methods
//
-(void) bufferItem:(wxPointerObject*)parentItem withChildren:(wxDataViewItemArray*)dataViewChildrenPtr
{
    NSInteger const noOfChildren = (*dataViewChildrenPtr).GetCount();

    [self setCurrentParentItem:parentItem];
    [self clearChildren];
    for (NSInteger indexChild=0; indexChild<noOfChildren; ++indexChild)
    {
        wxPointerObject* bufferedPointerObject;
        wxPointerObject* newPointerObject([[wxPointerObject alloc] initWithPointer:(*dataViewChildrenPtr)[indexChild].GetID()]);

        // The next statement and test looks strange but there is
        // unfortunately no workaround: due to the fact that two pointer
        // objects are identical if their pointers are identical - because the
        // method isEqual has been overloaded - the set operation will only
        // add a new pointer object if there is not already one in the set
        // having the same pointer. On the other side the children's array
        // would always add the new pointer object. This means that different
        // pointer objects are stored in the set and array. This will finally
        // lead to a crash as objects diverge. To solve this issue it is first
        // tested if the child already exists in the set and if it is the case
        // the sets object is going to be appended to the array, otheriwse the
        // new pointer object is added to the set and array:
        bufferedPointerObject = [self getItemFromBuffer:newPointerObject];
        if (bufferedPointerObject == nil)
        {
            [items    addObject:newPointerObject];
            [children addObject:newPointerObject];
        }
        else
            [children addObject:bufferedPointerObject];
        [newPointerObject release];
    }
}

@end

// ============================================================================
// wxCustomCell
// ============================================================================

@implementation wxCustomCell

#if 0 // starting implementation for custom cell clicks

- (id)init
{
    self = [super init];
    [self setAction:@selector(clickedAction)];
    [self setTarget:self];
    return self;
}

- (void) clickedAction: (id) sender
{
    wxUnusedVar(sender);
}

#endif

-(NSSize) cellSize
{
    wxCustomRendererObject * const
        obj = static_cast<wxCustomRendererObject *>([self objectValue]);


    const wxSize size = obj->customRenderer->GetSize();
    return NSMakeSize(size.x, size.y);
}

//
// implementations
//
-(void) drawWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
    wxCustomRendererObject * const
        obj = static_cast<wxCustomRendererObject *>([self objectValue]);
    if ( !obj )
    {
        // this may happen for the custom cells in container rows: they don't
        // have any values
        return;
    }

    wxDataViewCustomRenderer * const renderer = obj->customRenderer;

    // if this method is called everything is already setup correctly, 
    CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState( context );
    
    if ( ![controlView isFlipped] )
    {
        CGContextTranslateCTM( context, 0,  [controlView bounds].size.height );
        CGContextScaleCTM( context, 1, -1 );
    }
        
    wxGCDC dc;
    wxGraphicsContext* gc = wxGraphicsContext::CreateFromNative(context);
    dc.SetGraphicsContext(gc);

    int state = 0;
    if ( [self backgroundStyle] == NSBackgroundStyleDark )
        state |= wxDATAVIEW_CELL_SELECTED;

    renderer->WXCallRender(wxFromNSRect(controlView, cellFrame), &dc, state);

    CGContextRestoreGState( context );
}

-(NSRect) imageRectForBounds:(NSRect)cellFrame
{
    return cellFrame;
}

-(NSRect) titleRectForBounds:(NSRect)cellFrame
{
    return cellFrame;
}

@end

// ============================================================================
// wxImageCell
// ============================================================================
@implementation wxImageCell

-(NSSize) cellSize
{
    if ([self image] != nil)
        return [[self image] size];
    else
        return NSZeroSize;
}

@end

// ============================================================================
// wxTextFieldCell
// ============================================================================

@implementation wxTextFieldCell

- (void)setWXAlignment:(int)alignment
{
    alignment_ = alignment;
    adjustRect_ = (alignment & (wxALIGN_CENTRE_VERTICAL | wxALIGN_BOTTOM)) != 0;
}

// These three overrides implement vertical alignment of text cells.
// The solution is described by Daniel Jalkut at
// https://red-sweater.com/blog/148/what-a-difference-a-cell-makes

- (NSRect)drawingRectForBounds:(NSRect)theRect
{
    // Get the parent's idea of where we should draw
    NSRect r = [super drawingRectForBounds:theRect];

    if (!adjustRect_)
        return r;
    if (theRect.size.height <= MINIMUM_NATIVE_ROW_HEIGHT)
        return r;  // don't mess with default-sized rows as they are centered

    NSSize bestSize = [self cellSizeForBounds:theRect];
    if (bestSize.height < r.size.height)
    {
        if (alignment_ & wxALIGN_CENTER_VERTICAL)
        {
            r.origin.y += int(r.size.height - bestSize.height) / 2;
            r.size.height = bestSize.height;
        }
        else if (alignment_ & wxALIGN_BOTTOM)
        {
            r.origin.y += r.size.height - bestSize.height;
            r.size.height = bestSize.height;
        }
    }

    return r;
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength
{
    BOOL oldAdjustRect = adjustRect_;
    if (oldAdjustRect)
    {
        aRect = [self drawingRectForBounds:aRect];
        adjustRect_ = NO;
    }
    [super selectWithFrame:aRect inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
    adjustRect_ = oldAdjustRect;
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent
{
    BOOL oldAdjustRect = adjustRect_;
    if (oldAdjustRect)
    {
        aRect = [self drawingRectForBounds:aRect];
        adjustRect_ = NO;
    }
    [super editWithFrame:aRect inView:controlView editor:textObj delegate:anObject event:theEvent];
    adjustRect_ = oldAdjustRect;
}

@end


// ============================================================================
// wxImageTextCell
// ============================================================================
@implementation wxImageTextCell
//
// initialization
//
-(id) init
{
    self = [super init];
    if (self != nil)
    {
        // initializing the text part:
        [self setSelectable:YES];
        // initializing the image part:
        image       = nil;
        imageSize   = NSMakeSize(16,16);
        spaceImageText = 5.0;
        xImageShift    = 5.0;
    }
    return self;
}

-(id) copyWithZone:(NSZone*)zone
{
    wxImageTextCell* cell;


    cell = (wxImageTextCell*) [super copyWithZone:zone];
    cell->image          = [image retain];
    cell->imageSize      = imageSize;
    cell->spaceImageText = spaceImageText;
    cell->xImageShift    = xImageShift;

    return cell;
}

-(void) dealloc
{
    [image release];

    [super dealloc];
}

//
// alignment
//
-(NSTextAlignment) alignment
{
    return cellAlignment;
}

-(void) setAlignment:(NSTextAlignment)newAlignment
{
    cellAlignment = newAlignment;
    switch (newAlignment)
    {
        case NSCenterTextAlignment:
        case NSLeftTextAlignment:
        case NSJustifiedTextAlignment:
        case NSNaturalTextAlignment:
            [super setAlignment:NSLeftTextAlignment];
            break;
        case NSRightTextAlignment:
            [super setAlignment:NSRightTextAlignment];
            break;
        default:
            wxFAIL_MSG("Unknown alignment type.");
    }
}

//
// image access
//
-(NSImage*) image
{
    return image;
}

-(void) setImage:(NSImage*)newImage
{
    [newImage retain];
    [image release];
    image = newImage;
}

-(NSSize) imageSize
{
    return imageSize;
}

-(void) setImageSize:(NSSize) newImageSize
{
    imageSize = newImageSize;
}

//
// other methods
//
-(NSSize) cellImageSize
{
    return NSMakeSize(imageSize.width+xImageShift+spaceImageText,imageSize.height);
}

-(NSSize) cellSize
{
    NSSize cellSize([super cellSize]);


    if (imageSize.height > cellSize.height)
        cellSize.height = imageSize.height;
    cellSize.width += imageSize.width+xImageShift+spaceImageText;

    return cellSize;
}

-(NSSize) cellTextSize
{
    return [super cellSize];
}

//
// implementations
//
-(void) determineCellParts:(NSRect)cellFrame imagePart:(NSRect*)imageFrame textPart:(NSRect*)textFrame
{
    switch (cellAlignment)
    {
        case NSCenterTextAlignment:
            {
                CGFloat const cellSpace = cellFrame.size.width-[self cellSize].width;

                // if the cell's frame is smaller than its contents (at least
                // in x-direction) make sure that the image is visible:
                if (cellSpace <= 0)
                    NSDivideRect(cellFrame,imageFrame,textFrame,xImageShift+imageSize.width+spaceImageText,NSMinXEdge);
                else // otherwise center the image and text in the cell's frame
                    NSDivideRect(cellFrame,imageFrame,textFrame,xImageShift+imageSize.width+spaceImageText+0.5*cellSpace,NSMinXEdge);
            }
            break;
        case NSJustifiedTextAlignment:
        case NSLeftTextAlignment:
        case NSNaturalTextAlignment: // how to determine the natural writing direction? TODO
            NSDivideRect(cellFrame,imageFrame,textFrame,xImageShift+imageSize.width+spaceImageText,NSMinXEdge);
            break;
        case NSRightTextAlignment:
            {
                CGFloat const cellSpace = cellFrame.size.width-[self cellSize].width;

                // if the cell's frame is smaller than its contents (at least
                // in x-direction) make sure that the image is visible:
                if (cellSpace <= 0)
                    NSDivideRect(cellFrame,imageFrame,textFrame,xImageShift+imageSize.width+spaceImageText,NSMinXEdge);
                else // otherwise right align the image and text in the cell's frame
                    NSDivideRect(cellFrame,imageFrame,textFrame,xImageShift+imageSize.width+spaceImageText+cellSpace,NSMinXEdge);
            }
            break;
        default:
            *imageFrame = NSZeroRect;
            *textFrame  = NSZeroRect;
            wxFAIL_MSG("Unhandled alignment type.");
    }
}

-(void) drawWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
    NSRect textFrame, imageFrame;


    [self determineCellParts:cellFrame imagePart:&imageFrame textPart:&textFrame];
    // draw the image part by ourselves;
    // check if the cell has to draw its own background (checking is done by
    // the parameter of the textfield's cell):
    if ([self drawsBackground])
    {
        [[self backgroundColor] set];
        NSRectFill(imageFrame);
    }
    if (image != nil)
    {
        // the image is slightly shifted (xImageShift) and has a fixed size
        // but the image's frame might be larger and starts currently on the
        // left side of the cell's frame; therefore, the origin and the
        // image's frame size have to be adjusted:
        if (imageFrame.size.width >= xImageShift+imageSize.width+spaceImageText)
        {
            imageFrame.origin.x += imageFrame.size.width-imageSize.width-spaceImageText;
            imageFrame.size.width = imageSize.width;
        }
        else
        {
            imageFrame.origin.x   += xImageShift;
            imageFrame.size.width -= xImageShift+spaceImageText;
        }
        // ...and the image has to be centered in the y-direction:
        if (imageFrame.size.height > imageSize.height)
            imageFrame.size.height = imageSize.height;
        imageFrame.origin.y += ceil(0.5*(cellFrame.size.height-imageFrame.size.height));

        // according to the documentation the coordinate system should be
        // flipped for NSTableViews (y-coordinate goes from top to bottom); to
        // draw an image correctly the coordinate system has to be transformed
        // to a bottom-top coordinate system, otherwise the image's
        // content is flipped:
        NSAffineTransform* coordinateTransform([NSAffineTransform transform]);

        if ([controlView isFlipped])
        {
            [coordinateTransform scaleXBy: 1.0 yBy:-1.0]; // first the coordinate system is brought back to bottom-top orientation
            [coordinateTransform translateXBy:0.0 yBy:(-2.0)*imageFrame.origin.y-imageFrame.size.height]; // the coordinate system has to be moved to compensate for the
            [coordinateTransform concat];                                                                 // other orientation and the position of the image's frame
        }
        [image drawInRect:imageFrame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0]; // suggested method to draw the image
        // instead of compositeToPoint:operation:
        // take back previous transformation (if the view is not flipped the
        // coordinate transformation matrix contains the identity matrix and
        // the next two operations do not change the content's transformation
        // matrix):
        [coordinateTransform invert];
        [coordinateTransform concat];
    }
    // let the textfield cell draw the text part:
    if (textFrame.size.width > [self cellTextSize].width)
    {
        // for unknown reasons the alignment of the text cell is ignored;
        // therefore change the size so that alignment does not influence the
        // visualization anymore
        textFrame.size.width = [self cellTextSize].width;
    }
    [super drawWithFrame:textFrame inView:controlView];
}

-(void) editWithFrame:(NSRect)aRect inView:(NSView*)controlView editor:(NSText*)textObj delegate:(id)anObject event:(NSEvent*)theEvent
{
    NSRect textFrame, imageFrame;


    [self determineCellParts:aRect imagePart:&imageFrame textPart:&textFrame];
    [super editWithFrame:textFrame inView:controlView editor:textObj delegate:anObject event:theEvent];
}

-(NSUInteger) hitTestForEvent:(NSEvent*)event inRect:(NSRect)cellFrame ofView:(NSView*)controlView
{
    NSPoint point = [controlView convertPoint:[event locationInWindow] fromView:nil];

    NSRect imageFrame, textFrame;


    [self determineCellParts:cellFrame imagePart:&imageFrame textPart:&textFrame];
    if (image != nil)
    {
        // the image is shifted...
        if (imageFrame.size.width >= xImageShift+imageSize.width+spaceImageText)
        {
            imageFrame.origin.x += imageFrame.size.width-imageSize.width-spaceImageText;
            imageFrame.size.width = imageSize.width;
        }
        else
        {
            imageFrame.origin.x   += xImageShift;
            imageFrame.size.width -= xImageShift+spaceImageText;
        }
        // ...and centered:
        if (imageFrame.size.height > imageSize.height)
            imageFrame.size.height = imageSize.height;
        imageFrame.origin.y += ceil(0.5*(cellFrame.size.height-imageFrame.size.height));
        // If the point is in the image rect, then it is a content hit (see
        // documentation for hitTestForEvent:inRect:ofView):
        if (NSMouseInRect(point, imageFrame, [controlView isFlipped]))
            return NSCellHitContentArea;
    }
    // if the image was not hit let's try the text part:
    if (textFrame.size.width > [self cellTextSize].width)
    {
        // for unknown reasons the alignment of the text cell is ignored;
        // therefore change the size so that alignment does not influence the
        // visualization anymore
        textFrame.size.width = [self cellTextSize].width;
    }

    return [super hitTestForEvent:event inRect:textFrame ofView:controlView];
}

-(NSRect) imageRectForBounds:(NSRect)cellFrame
{
    NSRect textFrame, imageFrame;


    [self determineCellParts:cellFrame imagePart:&imageFrame textPart:&textFrame];
    if (imageFrame.size.width >= xImageShift+imageSize.width+spaceImageText)
    {
        imageFrame.origin.x += imageFrame.size.width-imageSize.width-spaceImageText;
        imageFrame.size.width = imageSize.width;
    }
    else
    {
        imageFrame.origin.x   += xImageShift;
        imageFrame.size.width -= xImageShift+spaceImageText;
    }
    // ...and centered:
    if (imageFrame.size.height > imageSize.height)
        imageFrame.size.height = imageSize.height;
    imageFrame.origin.y += ceil(0.5*(cellFrame.size.height-imageFrame.size.height));

    return imageFrame;
}

-(void) selectWithFrame:(NSRect)aRect inView:(NSView*)controlView editor:(NSText*)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength
{
    NSRect textFrame, imageFrame;


    [self determineCellParts:aRect imagePart:&imageFrame textPart:&textFrame];
    [super selectWithFrame:textFrame inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
}

-(NSRect) titleRectForBounds:(NSRect)cellFrame
{
    NSRect textFrame, imageFrame;


    [self determineCellParts:cellFrame imagePart:&imageFrame textPart:&textFrame];
    return textFrame;
}

@end

// ============================================================================
// wxCocoaOutlineView
// ============================================================================
@implementation wxCocoaOutlineView

//
// initializers / destructor
//
-(id) init
{
    self = [super init];
    if (self != nil)
    {
        currentlyEditedColumn =
            currentlyEditedRow = -1;

        [self registerForDraggedTypes:[NSArray arrayWithObjects:DataViewPboardType,NSStringPboardType,nil]];
        [self setDelegate:self];
        [self setDoubleAction:@selector(actionDoubleClick:)];
        [self setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
        [self setDraggingSourceOperationMask:NSDragOperationEvery forLocal:YES];
        [self setTarget:self];
    }
    return self;
}

//
// access to wxWidget's implementation
//
-(wxCocoaDataViewControl*) implementation
{
    return implementation;
}

-(void) setImplementation:(wxCocoaDataViewControl*) newImplementation
{
    implementation = newImplementation;
}

//
// actions
//
-(void) actionDoubleClick:(id)sender
{
    wxUnusedVar(sender);

    // actually the documentation (NSTableView 2007-10-31) for doubleAction:
    // and setDoubleAction: seems to be wrong as this action message is always
    // sent whether the cell is editable or not
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    const wxDataViewItem item = wxDataViewItemFromItem([self itemAtRow:[self clickedRow]]);
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_ACTIVATED, dvc, item);
    event.SetColumn( [self clickedColumn] );
    dvc->GetEventHandler()->ProcessEvent(event);
}

// Default enter key behaviour is to begin cell editing. Subclass keyDown to 
// provide a keyboard wxEVT_DATAVIEW_ITEM_ACTIVATED event and allow the NSEvent
// to pass if the wxEvent is not processed.
- (void)keyDown:(NSEvent *)event
{
    if( [[event charactersIgnoringModifiers]
         characterAtIndex: 0] == NSCarriageReturnCharacter )
    {
        wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();
        const wxDataViewItem item = wxDataViewItem( [[self itemAtRow:[self selectedRow]] pointer]);
        wxDataViewEvent eventDV(wxEVT_DATAVIEW_ITEM_ACTIVATED, dvc, item);
        if ( !dvc->GetEventHandler()->ProcessEvent(eventDV) )
            [super keyDown:event];
    }
    else
    {
        [super keyDown:event];  // all other keys
    }
}

//
// contextual menus
//
-(NSMenu*) menuForEvent:(NSEvent*)theEvent
{
    wxUnusedVar(theEvent);

    // this method does not do any special menu event handling but only sends
    // an event message; therefore, the user has full control if a context
    // menu should be shown or not
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    // get the item information;
    // theoretically more than one ID can be returned but the event can only
    // handle one item, therefore only the first item of the array is
    // returned:
    wxDataViewItem item;
    wxDataViewItemArray selectedItems;
    if (dvc->GetSelections(selectedItems) > 0)
        item = selectedItems[0];

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, dvc, item);
    dvc->GetEventHandler()->ProcessEvent(event);
    // nothing is done:
    return nil;
}

//
// delegate methods
//
-(void) outlineView:(NSOutlineView*)outlineView didClickTableColumn:(NSTableColumn*)tableColumn
{
    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    // first, send an event that the user clicked into a column's header:
    wxDataViewEvent event(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, dvc, col);
    dvc->HandleWindowEvent(event);

    // now, check if the click may have had an influence on sorting, too;
    // the sorting setup has to be done only if the clicked table column is
    // sortable and has not been used for sorting before the click; if the
    // column is already responsible for sorting the native control changes
    // the sorting direction automatically and informs the data source via
    // outlineView:sortDescriptorsDidChange:
    if (col->IsSortable() && ([tableColumn sortDescriptorPrototype] == nil))
    {
        // remove the sort order from the previously sorted column table (it
        // can also be that no sorted column table exists):
        UInt32 const noOfColumns = [outlineView numberOfColumns];

        for (UInt32 i=0; i<noOfColumns; ++i)
            [[[outlineView tableColumns] objectAtIndex:i] setSortDescriptorPrototype:nil];
        // make column table sortable:
        NSArray*          sortDescriptors;
        NSSortDescriptor* sortDescriptor;

        sortDescriptor = [[NSSortDescriptor alloc] initWithKey:[NSString stringWithFormat:@"%ld",(long)[outlineView columnWithIdentifier:[tableColumn identifier]]]
            ascending:YES];
        sortDescriptors = [NSArray arrayWithObject:sortDescriptor];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
        [outlineView setSortDescriptors:sortDescriptors];
        [sortDescriptor release];
    }
}

-(void) outlineView:(NSOutlineView *)outlineView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn
{
    // Implements per-column reordering in NSTableView per Apple's Q&A:
    // https://developer.apple.com/library/content/qa/qa1503/_index.html
    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);
    [outlineView setAllowsColumnReordering:col->IsReorderable()];
}

-(BOOL) outlineView:(NSOutlineView*)outlineView shouldCollapseItem:(id)item
{
    wxUnusedVar(outlineView);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_COLLAPSING, dvc, wxDataViewItemFromItem(item));
    dvc->GetEventHandler()->ProcessEvent(event);
    // opening the container is allowed if not vetoed:
    return event.IsAllowed();
}

-(BOOL) outlineView:(NSOutlineView*)outlineView shouldExpandItem:(id)item
{
    wxUnusedVar(outlineView);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_EXPANDING, dvc, wxDataViewItemFromItem(item));
    dvc->GetEventHandler()->ProcessEvent(event);
    // opening the container is allowed if not vetoed:
    return event.IsAllowed();
}

-(BOOL) outlineView:(NSOutlineView*)outlineView shouldSelectTableColumn:(NSTableColumn*)tableColumn
{
    wxUnusedVar(tableColumn);
    wxUnusedVar(outlineView);

    return NO;
}

-(void) outlineView:(NSOutlineView*)outlineView
    willDisplayCell:(id)cell
    forTableColumn:(NSTableColumn*)tableColumn
    item:(id)item
{
    wxUnusedVar(outlineView);

    wxDataViewCtrl * const dvc = implementation->GetDataViewCtrl();
    wxDataViewModel * const model = dvc->GetModel();

    wxDataViewColumn* const
        dvCol([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);
    const unsigned colIdx = dvCol->GetModelColumn();

    wxDataViewItem dvItem(wxDataViewItemFromItem(item));

    if ( !model->HasValue(dvItem, colIdx) )
        return;

    wxDataViewRenderer * const renderer = dvCol->GetRenderer();
    wxDataViewRendererNativeData * const data = renderer->GetNativeData();

    // let the renderer know about what it's going to render next
    data->SetColumnPtr(tableColumn);
    data->SetItem(item);
    data->SetItemCell(cell);

    // check if we have anything to render
    if ( renderer->PrepareForItem(model, dvItem, colIdx) )
    {
        // and do render it in this case
        renderer->MacRender();
    }
}

//
// notifications
//
-(void) outlineViewColumnDidMove:(NSNotification*)notification
{
    int const newColumnPosition = [[[notification userInfo] objectForKey:@"NSNewColumn"] intValue];

    NSTableColumn*
        tableColumn = [[self tableColumns] objectAtIndex:newColumnPosition];
    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    wxDataViewEvent event(wxEVT_DATAVIEW_COLUMN_REORDERED, dvc, col);
    event.SetColumn(newColumnPosition);
    dvc->GetEventHandler()->ProcessEvent(event);
}

-(void) outlineViewItemDidCollapse:(NSNotification*)notification
{
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    const wxDataViewItem item = wxDataViewItemFromItem(
                    [[notification userInfo] objectForKey:@"NSObject"]);
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_COLLAPSED, dvc, item);
    dvc->GetEventHandler()->ProcessEvent(event);
}

-(void) outlineViewItemDidExpand:(NSNotification*)notification
{
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    const wxDataViewItem item = wxDataViewItemFromItem(
                    [[notification userInfo] objectForKey:@"NSObject"]);
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_EXPANDED, dvc, item);
    dvc->GetEventHandler()->ProcessEvent(event);
}

-(void) outlineViewSelectionDidChange:(NSNotification*)notification
{
    wxUnusedVar(notification);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

    wxDataViewEvent event(wxEVT_DATAVIEW_SELECTION_CHANGED, dvc, dvc->GetSelection());
    dvc->GetEventHandler()->ProcessEvent(event);
}

-(BOOL) textShouldBeginEditing:(NSText*)textEditor
{
    currentlyEditedColumn = [self editedColumn];
    currentlyEditedRow = [self editedRow];
    
    wxDataViewItem item = wxDataViewItemFromItem([self itemAtRow:currentlyEditedRow]);
    
    NSTableColumn* tableColumn = [[self tableColumns] objectAtIndex:currentlyEditedColumn];
    wxDataViewColumn* const col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);
    
    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();
    // Before doing anything we send an event asking if editing of this item is really wanted.
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_START_EDITING, dvc, col, item);
    dvc->GetEventHandler()->ProcessEvent( event );
    if( !event.IsAllowed() )
        return NO;
    
    return YES;
}

-(void) textDidBeginEditing:(NSNotification*)notification
{
    // this notification is only sent if the user started modifying the cell
    // (not when the user clicked into the cell and the cell's editor is
    // called!)

    // call method of superclass (otherwise editing does not work correctly -
    // the outline data source class is not informed about a change of data):
    [super textDidBeginEditing:notification];

    // remember the column being edited, it will be used in textDidEndEditing:
    currentlyEditedColumn = [self editedColumn];
    currentlyEditedRow = [self editedRow];

    NSTableColumn*
        tableColumn = [[self tableColumns] objectAtIndex:currentlyEditedColumn];
    wxDataViewColumn* const
        col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);

    wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();


    // stop editing of a custom item first (if necessary)
    dvc->FinishCustomItemEditing();

    // now, send the event:
    wxDataViewRenderer* const renderer = col->GetRenderer();
    if ( renderer )
    {
        renderer->NotifyEditingStarted
                  (
                    wxDataViewItemFromItem([self itemAtRow:currentlyEditedRow])
                  );
    }
    //else: we should always have a renderer but don't crash if for some
    //      unfathomable reason we don't have it
}

-(void) textDidEndEditing:(NSNotification*)notification
{
    // call method of superclass (otherwise editing does not work correctly -
    // the outline data source class is not informed about a change of data):
    [super textDidEndEditing:notification];

    // under OSX an event indicating the end of an editing session can be sent
    // even if no event indicating a start of an editing session has been sent
    // (see Documentation for NSControl controlTextDidEndEditing:); this is
    // not expected by a user of the wxWidgets library and therefore an
    // wxEVT_DATAVIEW_ITEM_EDITING_DONE event is only sent if a
    // corresponding wxEVT_DATAVIEW_ITEM_EDITING_STARTED has been sent
    // before; to check if a wxEVT_DATAVIEW_ITEM_EDITING_STARTED has
    // been sent the last edited column/row are valid:
    if ( currentlyEditedColumn != -1 && currentlyEditedRow != -1 )
    {
        NSTableColumn*
            tableColumn = [[self tableColumns] objectAtIndex:currentlyEditedColumn];
        wxDataViewColumn* const
            col([static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer]);

        wxDataViewCtrl* const dvc = implementation->GetDataViewCtrl();

        const wxDataViewItem
            item = wxDataViewItemFromItem([self itemAtRow:currentlyEditedRow]);

        // send event to wxWidgets:
        wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_EDITING_DONE, dvc, col, item);
        dvc->GetEventHandler()->ProcessEvent(event);


        // we're not editing any more
        currentlyEditedColumn =
            currentlyEditedRow = -1;
    }
}

-(BOOL) becomeFirstResponder
{
    BOOL r = [super becomeFirstResponder];
    if ( r )
        implementation->DoNotifyFocusSet();
    return r;
}

@end

// ============================================================================
// wxCocoaDataViewControl
// ============================================================================

wxCocoaDataViewControl::wxCocoaDataViewControl(wxWindow* peer,
                                               const wxPoint& pos,
                                               const wxSize& size,
                                               long style)
    : wxWidgetCocoaImpl
      (
        peer,
        [[NSScrollView alloc] initWithFrame:wxOSXGetFrameForControl(peer,pos,size)]
      ),
      m_DataSource(NULL),
      m_OutlineView([[wxCocoaOutlineView alloc] init])
{
    // initialize scrollview (the outline view is part of a scrollview):
    NSScrollView* scrollview = (NSScrollView*) GetWXWidget();

    [scrollview setBorderType:NSNoBorder];
    [scrollview setHasVerticalScroller:YES];
    [scrollview setHasHorizontalScroller:YES];
    [scrollview setAutohidesScrollers:YES];
    [scrollview setDocumentView:m_OutlineView];

    // initialize the native control itself too
    InitOutlineView(style);
}

void wxCocoaDataViewControl::InitOutlineView(long style)
{
    // we cannot call InstallHandler(m_OutlineView) here, because we are handling
    // our action:s ourselves, only associate the view with this impl
    Associate(m_OutlineView,this);

    [m_OutlineView setImplementation:this];
    [m_OutlineView setFocusRingType:NSFocusRingTypeNone];
    [m_OutlineView setColumnAutoresizingStyle:NSTableViewLastColumnOnlyAutoresizingStyle];
    [m_OutlineView setIndentationPerLevel:GetDataViewCtrl()->GetIndent()];
    NSUInteger maskGridStyle(NSTableViewGridNone);
    if (style & wxDV_HORIZ_RULES)
        maskGridStyle |= NSTableViewSolidHorizontalGridLineMask;
    if (style & wxDV_VERT_RULES)
        maskGridStyle |= NSTableViewSolidVerticalGridLineMask;
    [m_OutlineView setGridStyleMask:maskGridStyle];
    [m_OutlineView setAllowsMultipleSelection:           (style & wxDV_MULTIPLE)  != 0];
    [m_OutlineView setUsesAlternatingRowBackgroundColors:(style & wxDV_ROW_LINES) != 0];

    if ( style & wxDV_NO_HEADER )
        [m_OutlineView setHeaderView:nil];
}

wxCocoaDataViewControl::~wxCocoaDataViewControl()
{
    [m_DataSource  release];
    [m_OutlineView release];
}

//
// column related methods (inherited from wxDataViewWidgetImpl)
//
bool wxCocoaDataViewControl::ClearColumns()
{
    CGFloat rowHeight = [m_OutlineView rowHeight];

    // as there is a bug in NSOutlineView version (OSX 10.5.6 #6555162) the
    // columns cannot be deleted if there is an outline column in the view;
    // therefore, the whole view is deleted and newly constructed:
    RemoveAssociation(m_OutlineView); // undo InitOutlineView's association

    [m_OutlineView removeFromSuperviewWithoutNeedingDisplay];
    [m_OutlineView release];
    m_OutlineView = [[wxCocoaOutlineView alloc] init];
    [((NSScrollView*) GetWXWidget()) setDocumentView:m_OutlineView];
    [m_OutlineView setDataSource:m_DataSource];

    InitOutlineView(GetDataViewCtrl()->GetWindowStyle());

    [m_OutlineView setRowHeight:rowHeight];

    return true;
}

bool wxCocoaDataViewControl::DeleteColumn(wxDataViewColumn* columnPtr)
{
    if ([m_OutlineView outlineTableColumn] == columnPtr->GetNativeData()->GetNativeColumnPtr())
        [m_OutlineView setOutlineTableColumn:nil]; // due to a bug this does not work
    [m_OutlineView removeTableColumn:columnPtr->GetNativeData()->GetNativeColumnPtr()]; // due to a confirmed bug #6555162 the deletion does not work for
    // outline table columns (... and there is no workaround)
    return (([m_OutlineView columnWithIdentifier:[wxDVCNSTableColumn identifierForColumnPointer:columnPtr]]) == -1);
}

void wxCocoaDataViewControl::DoSetExpanderColumn(const wxDataViewColumn *columnPtr)
{
    [m_OutlineView setOutlineTableColumn:columnPtr->GetNativeData()->GetNativeColumnPtr()];
}

wxDataViewColumn* wxCocoaDataViewControl::GetColumn(unsigned int pos) const
{
    NSTableColumn* tableColumn = [[m_OutlineView tableColumns] objectAtIndex:pos];
    return [static_cast<wxDVCNSTableColumn*>(tableColumn) getColumnPointer];
}

int wxCocoaDataViewControl::GetColumnPosition(const wxDataViewColumn *columnPtr) const
{
    return [m_OutlineView columnWithIdentifier:[wxDVCNSTableColumn identifierForColumnPointer:columnPtr]];
}

bool wxCocoaDataViewControl::InsertColumn(unsigned int pos, wxDataViewColumn* columnPtr)
{
    // create column and set the native data of the dataview column:
    NSTableColumn *nativeColumn = ::CreateNativeColumn(columnPtr);
    columnPtr->GetNativeData()->SetNativeColumnPtr(nativeColumn);
    // as the native control does not allow the insertion of a column at a
    // specified position the column is first appended and - if necessary -
    // moved to its final position:
    [m_OutlineView addTableColumn:nativeColumn];

    // it is owned, and kepy alive, by m_OutlineView now
    [nativeColumn release];

    if (pos != static_cast<unsigned int>([m_OutlineView numberOfColumns]-1))
        [m_OutlineView moveColumn:[m_OutlineView numberOfColumns]-1 toColumn:pos];

    // set columns width now that it can be computed even for autosized columns:
    columnPtr->SetWidth(columnPtr->GetWidthVariable());

    // done:
    return true;
}

void wxCocoaDataViewControl::FitColumnWidthToContent(unsigned int pos)
{
    const int count = GetCount();
    NSTableColumn *column = GetColumn(pos)->GetNativeData()->GetNativeColumnPtr();

    class MaxWidthCalculator
    {
    public:
        MaxWidthCalculator(wxCocoaOutlineView *view,
                           NSTableColumn *column, unsigned columnIndex)
            : m_width(0),
              m_view(view),
              m_column(columnIndex),
              m_indent(0)
        {
            // account for indentation in the column with expander
            if ( column == [m_view outlineTableColumn] )
                m_indent = [m_view indentationPerLevel];
        }

        void UpdateWithWidth(int width)
        {
            m_width = wxMax(m_width, width);
        }

        void UpdateWithRow(int row)
        {
            NSCell *cell = [m_view preparedCellAtColumn:m_column row:row];
            unsigned cellWidth = [cell cellSize].width + 1/*round the float up*/;

            if ( m_indent )
                cellWidth += m_indent * ([m_view levelForRow:row] + 1);

            m_width = wxMax(m_width, cellWidth);
        }

        int GetMaxWidth() const { return m_width; }

    private:
        int m_width;
        wxCocoaOutlineView *m_view;
        unsigned m_column;
        int m_indent;
    };

    MaxWidthCalculator calculator(m_OutlineView, column, pos);

    if ( [column headerCell] )
    {
        calculator.UpdateWithWidth([[column headerCell] cellSize].width + 1/*round the float up*/);
    }

    // The code below deserves some explanation. For very large controls, we
    // simply can't afford to calculate sizes for all items, it takes too
    // long. So the best we can do is to check the first and the last N/2
    // items in the control for some sufficiently large N and calculate best
    // sizes from that. That can result in the calculated best width being too
    // small for some outliers, but it's better to get slightly imperfect
    // result than to wait several seconds after every update. To avoid highly
    // visible miscalculations, we also include all currently visible items
    // no matter what.  Finally, the value of N is determined dynamically by
    // measuring how much time we spent on the determining item widths so far.

#if wxUSE_STOPWATCH
    int top_part_end = count;
    static const long CALC_TIMEOUT = 20/*ms*/;
    // don't call wxStopWatch::Time() too often
    static const unsigned CALC_CHECK_FREQ = 100;
    wxStopWatch timer;
#else
    // use some hard-coded limit, that's the best we can do without timer
    int top_part_end = wxMin(500, count);
#endif // wxUSE_STOPWATCH/!wxUSE_STOPWATCH

    int row = 0;

    for ( row = 0; row < top_part_end; row++ )
    {
#if wxUSE_STOPWATCH
        if ( row % CALC_CHECK_FREQ == CALC_CHECK_FREQ-1 &&
             timer.Time() > CALC_TIMEOUT )
            break;
#endif // wxUSE_STOPWATCH
        calculator.UpdateWithRow(row);
    }

    // row is the first unmeasured item now; that's our value of N/2

    if ( row < count )
    {
        top_part_end = row;

        // add bottom N/2 items now:
        const int bottom_part_start = wxMax(row, count - row);
        for ( row = bottom_part_start; row < count; row++ )
            calculator.UpdateWithRow(row);

        // finally, include currently visible items in the calculation:
        const NSRange visible = [m_OutlineView rowsInRect:[m_OutlineView visibleRect]];
        const int first_visible = wxMax(visible.location, top_part_end);
        const int last_visible = wxMin(first_visible + visible.length, bottom_part_start);

        for ( row = first_visible; row < last_visible; row++ )
            calculator.UpdateWithRow(row);

        wxLogTrace("dataview",
                   "determined best size from %d top, %d bottom plus %d more visible items out of %d total",
                   top_part_end,
                   count - bottom_part_start,
                   wxMax(0, last_visible - first_visible),
                   count);
    }

    [column setWidth:calculator.GetMaxWidth()];
}

//
// item related methods (inherited from wxDataViewWidgetImpl)
//
bool wxCocoaDataViewControl::Add(const wxDataViewItem& parent, const wxDataViewItem& WXUNUSED(item))
{
    if (parent.IsOk())
        [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:parent] reloadChildren:YES];
    else
        [m_OutlineView reloadData];
    return true;
}

bool wxCocoaDataViewControl::Add(const wxDataViewItem& parent, const wxDataViewItemArray& WXUNUSED(items))
{
    if (parent.IsOk())
        [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:parent] reloadChildren:YES];
    else
        [m_OutlineView reloadData];
    return true;
}

void wxCocoaDataViewControl::Collapse(const wxDataViewItem& item)
{
    [m_OutlineView collapseItem:[m_DataSource getDataViewItemFromBuffer:item]];
}

void wxCocoaDataViewControl::EnsureVisible(const wxDataViewItem& item, const wxDataViewColumn *columnPtr)
{
    if (item.IsOk())
    {
        [m_OutlineView scrollRowToVisible:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]]];
        if (columnPtr)
            [m_OutlineView scrollColumnToVisible:GetColumnPosition(columnPtr)];
    }
}

void wxCocoaDataViewControl::DoExpand(const wxDataViewItem& item)
{
    [m_OutlineView expandItem:[m_DataSource getDataViewItemFromBuffer:item]];
}

unsigned int wxCocoaDataViewControl::GetCount() const
{
    return [m_OutlineView numberOfRows];
}

int wxCocoaDataViewControl::GetCountPerPage() const
{
    NSScrollView *scrollView = [m_OutlineView enclosingScrollView];
    NSTableHeaderView *headerView = [m_OutlineView headerView];
    NSRect visibleRect = scrollView.contentView.visibleRect;
    if ( headerView )
        visibleRect.size.height -= headerView.visibleRect.size.height;
    return (int) (visibleRect.size.height / [m_OutlineView rowHeight]);
}

wxDataViewItem wxCocoaDataViewControl::GetTopItem() const
{
    NSScrollView *scrollView = [m_OutlineView enclosingScrollView];
    NSTableHeaderView *headerView = [m_OutlineView headerView];
    NSRect visibleRect = scrollView.contentView.visibleRect;
    if ( headerView )
        visibleRect.origin.y += headerView.visibleRect.size.height;
    NSRange range = [m_OutlineView rowsInRect:visibleRect];
    return wxDataViewItem([[m_OutlineView itemAtRow:range.location] pointer]);
}

wxRect wxCocoaDataViewControl::GetRectangle(const wxDataViewItem& item, const wxDataViewColumn *columnPtr)
{
    NSView* const parent = [m_osxView superview];

    wxRect r = wxFromNSRect(parent, [m_OutlineView frameOfCellAtColumn:GetColumnPosition(columnPtr)
            row:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]]]);

    // For hidden items, i.e. items not shown because their parent is
    // collapsed, the native method returns rectangles with negative width, but
    // we're supposed to just return an empty rectangle in this case. To be on
    // the safe side, also check for the height as well, even if it seems to be
    // always 0 in this case.
    if ( r.width < 0 || r.height < 0 )
        return wxRect();

    // Also adjust the vertical coordinates to use physical window coordinates
    // instead of the logical ones returned by frameOfCellAtColumn:row:
    NSScrollView* const scrollView = [m_OutlineView enclosingScrollView];
    const wxRect
        visible = wxFromNSRect(parent, scrollView.contentView.visibleRect);

    // We are also supposed to return empty rectangle if the item is not
    // visible because it is scrolled out of view.
    if ( r.GetBottom() < visible.GetTop() || r.GetTop() > visible.GetBottom() )
        return wxRect();

    r.y -= visible.y;

    return r;
}

bool wxCocoaDataViewControl::IsExpanded(const wxDataViewItem& item) const
{
    return [m_OutlineView isItemExpanded:[m_DataSource getDataViewItemFromBuffer:item]];
}

bool wxCocoaDataViewControl::Reload()
{
    [m_DataSource clearBuffers];
    [m_OutlineView reloadData];
    [m_OutlineView scrollColumnToVisible:0];
    [m_OutlineView scrollRowToVisible:0];
    return true;
}

bool wxCocoaDataViewControl::Remove(const wxDataViewItem& parent, const wxDataViewItem& WXUNUSED(item))
{
    if (parent.IsOk())
        [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:parent] reloadChildren:YES];
    else
        [m_OutlineView reloadData];
    return true;
}

bool wxCocoaDataViewControl::Remove(const wxDataViewItem& parent, const wxDataViewItemArray& WXUNUSED(item))
{
    if (parent.IsOk())
        [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:parent] reloadChildren:YES];
    else
        [m_OutlineView reloadData];
    return true;
}

bool wxCocoaDataViewControl::Update(const wxDataViewColumn *columnPtr)
{
    wxUnusedVar(columnPtr);
    
    return false;
}

bool wxCocoaDataViewControl::Update(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItem& item)
{
    [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:item]];
    return true;
}

bool wxCocoaDataViewControl::Update(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItemArray& items)
{
    for (size_t i=0; i<items.GetCount(); ++i)
        [m_OutlineView reloadItem:[m_DataSource getDataViewItemFromBuffer:items[i]]];
    return true;
}

//
// model related methods
//
bool wxCocoaDataViewControl::AssociateModel(wxDataViewModel* model)
{
    [m_DataSource release];
    if (model)
    {
        m_DataSource = [[wxCocoaOutlineDataSource alloc] init];
        [m_DataSource setImplementation:this];
        [m_DataSource setModel:model];
    }
    else
        m_DataSource = NULL;
    [m_OutlineView setDataSource:m_DataSource]; // if there is a data source the data is immediately going to be requested

    // By default, the first column is indented to leave enough place for the
    // expanders, but this looks bad if there are no expanders, so don't use
    // indent in this case.
    if ( model && model->IsListModel() )
    {
        DoSetIndent(0);
    }

    return true;
}

//
// selection related methods (inherited from wxDataViewWidgetImpl)
//

wxDataViewItem wxCocoaDataViewControl::GetCurrentItem() const
{
    return wxDataViewItem([[m_OutlineView itemAtRow:[m_OutlineView selectedRow]] pointer]);
}

wxDataViewColumn *wxCocoaDataViewControl::GetCurrentColumn() const
{
    int col = [m_OutlineView selectedColumn];
    if ( col == -1 )
        return NULL;
    return GetColumn(col);
}

void wxCocoaDataViewControl::SetCurrentItem(const wxDataViewItem& item)
{
    // We can't have unselected current item in a NSTableView, as the
    // documentation of its deselectRow method explains, the control will
    // automatically change the current item to the closest still selected item
    // if the current item is deselected. So we have no choice but to select
    // the item in the same time as making it current.
    Select(item);
}

int wxCocoaDataViewControl::GetSelectedItemsCount() const
{
    return [m_OutlineView numberOfSelectedRows];
}

int wxCocoaDataViewControl::GetSelections(wxDataViewItemArray& sel) const
{
    NSIndexSet* selectedRowIndexes([m_OutlineView selectedRowIndexes]);

    NSUInteger indexRow;


    sel.Empty();
    sel.Alloc([selectedRowIndexes count]);
    indexRow = [selectedRowIndexes firstIndex];
    while (indexRow != NSNotFound)
    {
        sel.Add(wxDataViewItem([[m_OutlineView itemAtRow:indexRow] pointer]));
        indexRow = [selectedRowIndexes indexGreaterThanIndex:indexRow];
    }
    return sel.GetCount();
}

bool wxCocoaDataViewControl::IsSelected(const wxDataViewItem& item) const
{
    return [m_OutlineView isRowSelected:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]]];
}

void wxCocoaDataViewControl::Select(const wxDataViewItem& item)
{
    if (item.IsOk())
        [m_OutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]]]
            byExtendingSelection:GetDataViewCtrl()->HasFlag(wxDV_MULTIPLE) ? YES : NO];
}

void wxCocoaDataViewControl::SelectAll()
{
    [m_OutlineView selectAll:m_OutlineView];
}

void wxCocoaDataViewControl::Unselect(const wxDataViewItem& item)
{
    if (item.IsOk())
        [m_OutlineView deselectRow:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]]];
}

void wxCocoaDataViewControl::UnselectAll()
{
    [m_OutlineView deselectAll:m_OutlineView];
    [m_OutlineView setNeedsDisplay:YES];
}

//
// sorting related methods
//
wxDataViewColumn* wxCocoaDataViewControl::GetSortingColumn() const
{
    NSArray* const columns = [m_OutlineView tableColumns];

    UInt32 const noOfColumns = [columns count];


    for (UInt32 i=0; i<noOfColumns; ++i)
        if ([[columns objectAtIndex:i] sortDescriptorPrototype] != nil)
            return GetColumn(i);
    return NULL;
}

void wxCocoaDataViewControl::Resort()
{
    [m_DataSource clearChildren];
    [m_OutlineView reloadData];
}

void wxCocoaDataViewControl::StartEditor( const wxDataViewItem & item, unsigned int column )
{
    [m_OutlineView editColumn:column row:[m_OutlineView rowForItem:[m_DataSource getDataViewItemFromBuffer:item]] withEvent:nil select:YES];
}

//
// other methods (inherited from wxDataViewWidgetImpl)
//
void wxCocoaDataViewControl::DoSetIndent(int indent)
{
    [m_OutlineView setIndentationPerLevel:static_cast<CGFloat>(indent)];
}

void wxCocoaDataViewControl::HitTest(const wxPoint& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const
{
    NSPoint const nativePoint = wxToNSPoint((NSScrollView*) GetWXWidget(),point);

    int indexColumn;
    int indexRow;


    indexColumn = [m_OutlineView columnAtPoint:nativePoint];
    indexRow    = [m_OutlineView rowAtPoint:   nativePoint];
    if ((indexColumn >= 0) && (indexRow >= 0))
    {
        columnPtr = GetColumn(indexColumn);
        item      = wxDataViewItem([[m_OutlineView itemAtRow:indexRow] pointer]);
    }
    else
    {
        columnPtr = NULL;
        item      = wxDataViewItem();
    }
}

void wxCocoaDataViewControl::SetRowHeight(int height)
{
    [m_OutlineView setRowHeight:wxMax(height, GetDefaultRowHeight())];
}

int wxCocoaDataViewControl::GetDefaultRowHeight() const
{
    // Custom setup of NSLayoutManager is necessary to match NSTableView sizing.
    // See https://stackoverflow.com/questions/17095927/dynamically-changing-row-height-after-font-size-of-entire-nstableview-nsoutlin
    NSLayoutManager *lm = [[NSLayoutManager alloc] init];
    [lm setTypesetterBehavior:NSTypesetterBehavior_10_2_WithCompatibility];
    [lm setUsesScreenFonts:NO];
    CGFloat height = [lm defaultLineHeightForFont:GetWXPeer()->GetFont().OSXGetNSFont()];
    [lm release];
    return wxMax(MINIMUM_NATIVE_ROW_HEIGHT, int(height));
}

void wxCocoaDataViewControl::SetRowHeight(const wxDataViewItem& WXUNUSED(item), unsigned int WXUNUSED(height))
    // Not supported by the native control
{
}

void wxCocoaDataViewControl::OnSize()
{
    if ([m_OutlineView numberOfColumns] == 1)
        [m_OutlineView sizeLastColumnToFit];
}

//
// drag & drop helper methods
//
wxDataFormat wxCocoaDataViewControl::GetDnDDataFormat(wxDataObjectComposite* dataObjects)
{
    wxDataFormat resultFormat;
    if ( !dataObjects )
        return resultFormat;

    bool compatible(true);

    size_t const noOfFormats = dataObjects->GetFormatCount();
    size_t       indexFormat;

    wxDataFormat* formats;

    // get all formats and check afterwards if the formats are compatible; if
    // they are compatible the preferred format is returned otherwise
    // wxDF_INVALID is returned;
    // currently compatible types (ordered by priority are):
    //  - wxDF_UNICODETEXT - wxDF_TEXT
    formats = new wxDataFormat[noOfFormats];
    dataObjects->GetAllFormats(formats);
    indexFormat = 0;
    while ((indexFormat < noOfFormats) && compatible)
    {
        switch (resultFormat.GetType())
        {
            case wxDF_INVALID:
                resultFormat.SetType(formats[indexFormat].GetType()); // first format (should only be reached if indexFormat == 0)
                break;
            case wxDF_TEXT:
                if (formats[indexFormat].GetType() == wxDF_UNICODETEXT)
                    resultFormat.SetType(wxDF_UNICODETEXT);
                else // incompatible
                {
                    resultFormat.SetType(wxDF_INVALID);
                    compatible = false;
                }
                break;
            case wxDF_UNICODETEXT:
                if (formats[indexFormat].GetType() != wxDF_TEXT)
                {
                    resultFormat.SetType(wxDF_INVALID);
                    compatible = false;
                }
                break;
            default:
                resultFormat.SetType(wxDF_INVALID); // not (yet) supported format
                compatible = false;
        }
        ++indexFormat;
    }

    delete[] formats;

    return resultFormat;
}

wxDataObjectComposite* wxCocoaDataViewControl::GetDnDDataObjects(NSData* dataObject) const
{
    wxDataFormatId dataFormatID;


    [dataObject getBytes:&dataFormatID length:sizeof(wxDataFormatId)];
    switch (dataFormatID)
    {
        case wxDF_TEXT:
        case wxDF_UNICODETEXT:
            {
                wxTextDataObject* textDataObject(new wxTextDataObject());

                if (textDataObject->SetData(wxDataFormat(dataFormatID),[dataObject length]-sizeof(wxDataFormatId),static_cast<char const*>([dataObject bytes])+sizeof(wxDataFormatId)))
                {
                    wxDataObjectComposite* dataObjectComposite(new wxDataObjectComposite());

                    dataObjectComposite->Add(textDataObject);
                    return dataObjectComposite;
                }
                else
                {
                    delete textDataObject;
                    return NULL;
                }
            }
            break;
        default:
            return NULL;
    }
}

id wxCocoaDataViewControl::GetItemAtRow(int row) const
{
    return [m_OutlineView itemAtRow:row];
}

void wxCocoaDataViewControl::SetFont(const wxFont& font, const wxColour& foreground, long windowStyle, bool ignoreBlack)
{
    wxWidgetCocoaImpl::SetFont(font, foreground, windowStyle, ignoreBlack);
    SetRowHeight(0/*will use default/minimum height*/);
}


// ----------------------------------------------------------------------------
// wxDataViewRendererNativeData
// ----------------------------------------------------------------------------

void wxDataViewRendererNativeData::Init()
{
    m_origFont = NULL;
    m_origTextColour = NULL;
    m_ellipsizeMode = wxELLIPSIZE_MIDDLE;
    m_hasCustomFont = false;

    if ( m_ColumnCell )
        ApplyLineBreakMode(m_ColumnCell);
}

void wxDataViewRendererNativeData::ApplyLineBreakMode(NSCell *cell)
{
    NSLineBreakMode nsMode = NSLineBreakByWordWrapping;
    switch ( m_ellipsizeMode )
    {
        case wxELLIPSIZE_NONE:
            nsMode = NSLineBreakByClipping;
            break;

        case wxELLIPSIZE_START:
            nsMode = NSLineBreakByTruncatingHead;
            break;

        case wxELLIPSIZE_MIDDLE:
            nsMode = NSLineBreakByTruncatingMiddle;
            break;

        case wxELLIPSIZE_END:
            nsMode = NSLineBreakByTruncatingTail;
            break;
    }

    wxASSERT_MSG( nsMode != NSLineBreakByWordWrapping, "unknown wxEllipsizeMode" );

    [cell setLineBreakMode: nsMode];
}

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

wxDataViewRenderer::wxDataViewRenderer(const wxString& varianttype,
                                       wxDataViewCellMode mode,
                                       int align)
    : wxDataViewRendererBase(varianttype, mode, align),
      m_alignment(align),
      m_mode(mode),
      m_NativeDataPtr(NULL)
{
}

wxDataViewRenderer::~wxDataViewRenderer()
{
    delete m_NativeDataPtr;
}

void wxDataViewRenderer::SetAlignment(int align)
{
    m_alignment = align;
    OSXUpdateAlignment();
}

void wxDataViewRenderer::OSXUpdateAlignment()
{
    int align = GetEffectiveAlignment();
    NSCell *cell = GetNativeData()->GetColumnCell();
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment(align)];
    if ([cell respondsToSelector:@selector(setWXAlignment:)])
        [(wxTextFieldCell*)cell setWXAlignment:align];
}

void wxDataViewRenderer::SetMode(wxDataViewCellMode mode)
{
    m_mode = mode;
    if ( GetOwner() )
        [GetOwner()->GetNativeData()->GetNativeColumnPtr() setEditable:(mode == wxDATAVIEW_CELL_EDITABLE)];
}

void wxDataViewRenderer::SetNativeData(wxDataViewRendererNativeData* newNativeDataPtr)
{
    delete m_NativeDataPtr;
    m_NativeDataPtr = newNativeDataPtr;
}

void wxDataViewRenderer::EnableEllipsize(wxEllipsizeMode mode)
{
    // we need to store this value to apply it to the columns headerCell in
    // CreateNativeColumn()
    GetNativeData()->SetEllipsizeMode(mode);

    // but we may already apply it to the column cell which will be used for
    // this column
    GetNativeData()->ApplyLineBreakMode(GetNativeData()->GetColumnCell());
}

wxEllipsizeMode wxDataViewRenderer::GetEllipsizeMode() const
{
    return GetNativeData()->GetEllipsizeMode();
}

bool wxDataViewRenderer::IsHighlighted() const
{
    return [GetNativeData()->GetColumnCell() backgroundStyle] == NSBackgroundStyleDark;
}

void
wxDataViewRenderer::OSXOnCellChanged(NSObject *object,
                                     const wxDataViewItem& item,
                                     unsigned col)
{
    // TODO: This code should really be removed and this function be made pure
    //       virtual. We just need to decide what to do with custom renderers
    //       (i.e. wxDataViewCustomRenderer), currently OS X "in place" editing
    //       which doesn't really create an editor control is not compatible
    //       with the in place editing under other platforms.

    wxVariant value;
    if ( [object isKindOfClass:[NSString class]] )
        value = ObjectToString(object);
    else if ( [object isKindOfClass:[NSNumber class]] )
        value = ObjectToLong(object);
    else if ( [object isKindOfClass:[NSDate class]] )
        value = ObjectToDate(object);
    else
    {
        wxFAIL_MSG( wxString::Format
                    (
                     "unknown value type %s",
                     wxCFStringRef::AsString([object className])
                    ));
        return;
    }

    if ( !Validate(value) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(value, item, col);
}

void wxDataViewRenderer::SetAttr(const wxDataViewItemAttr& attr)
{
    wxDataViewRendererNativeData * const data = GetNativeData();
    NSCell * const cell = data->GetItemCell();

    // set the font and text colour to use: we need to do it if we had ever
    // changed them before, even if this item itself doesn't have any special
    // attributes as otherwise it would reuse the attributes from the previous
    // cell rendered using the same renderer
    NSFont *font = NULL;
    NSColor *colText = NULL;

    if ( attr.HasFont() )
    {
        font = data->GetOriginalFont();
        if ( !font )
        {
            // this is the first time we're setting the font, remember the
            // original one before changing it
            font = [cell font];
            data->SaveOriginalFont(font);
        }

        if ( font )
        {
            // FIXME: using wxFont methods here doesn't work for some reason
            NSFontManager * const fm = [NSFontManager sharedFontManager];
            if ( attr.GetBold() )
                font = [fm convertFont:font toHaveTrait:NSBoldFontMask];
            if ( attr.GetItalic() )
                font = [fm convertFont:font toHaveTrait:NSItalicFontMask];
        }
        //else: can't change font if the cell doesn't have any
    }

    if ( attr.HasColour() && [cell backgroundStyle] == NSBackgroundStyleLight )
    {
        // we can set font for any cell but only NSTextFieldCell provides
        // a method for setting text colour so check that this method is
        // available before using it
        if ( [cell respondsToSelector:@selector(setTextColor:)] &&
                [cell respondsToSelector:@selector(textColor)] )
        {
            if ( !data->GetOriginalTextColour() )
            {
                // the cast to (untyped) id is safe because of the check above
                data->SaveOriginalTextColour([(id)cell textColor]);
            }

            colText = attr.GetColour().OSXGetNSColor();
        }
    }

    if ( !font )
        font = data->GetOriginalFont();
    if ( !colText )
        colText = data->GetOriginalTextColour();

    if ( font )
        [cell setFont:font];

    if ( colText )
        [(id)cell setTextColor:colText];
}

void wxDataViewRenderer::SetEnabled(bool enabled)
{
    [GetNativeData()->GetItemCell() setEnabled:enabled];
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer, wxDataViewRendererBase);

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------
wxDataViewCustomRenderer::wxDataViewCustomRenderer(const wxString& varianttype,
                                                   wxDataViewCellMode mode,
                                                   int align)
    : wxDataViewCustomRendererBase(varianttype, mode, align),
      m_editorCtrlPtr(NULL),
      m_DCPtr(NULL)
{
    wxCustomCell* cell = [[wxCustomCell alloc] init];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

bool wxDataViewCustomRenderer::MacRender()
{
    [GetNativeData()->GetItemCell() setObjectValue:[[[wxCustomRendererObject alloc] initWithRenderer:this] autorelease]];
    return true;
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------
wxDataViewTextRenderer::wxDataViewTextRenderer(const wxString& varianttype,
                                               wxDataViewCellMode mode,
                                               int align)
    : wxDataViewRenderer(varianttype,mode,align)
{
#if wxUSE_MARKUP
    m_useMarkup = false;
#endif // wxUSE_MARKUP

    NSTextFieldCell* cell;


    cell = [[wxTextFieldCell alloc] init];
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment(align)];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

#if wxUSE_MARKUP

void wxDataViewTextRenderer::EnableMarkup(bool enable)
{
    m_useMarkup = enable;
}

#endif // wxUSE_MARKUP

bool wxDataViewTextRenderer::MacRender()
{
    NSCell *cell = GetNativeData()->GetItemCell();
#if wxUSE_MARKUP
    if ( m_useMarkup )
    {
        wxItemMarkupToAttrString toAttr(wxFont([cell font]), GetValue().GetString());
        NSMutableAttributedString *str = toAttr.GetNSAttributedString();

        if ( [cell lineBreakMode] != NSLineBreakByClipping )
        {
            NSMutableParagraphStyle *par = [[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy];
            [par setLineBreakMode:[cell lineBreakMode]];
            // Tightening looks very ugly when combined with non-tightened rows,
            // so disabled it on OS X version where it's used:
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_11
            if ( WX_IS_MACOS_AVAILABLE(10, 11) )
            {
                [par setAllowsDefaultTighteningForTruncation:NO];
            }
            else
#endif
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_10
            if ( WX_IS_MACOS_AVAILABLE(10, 10) )
            {
                [par setTighteningFactorForTruncation:0.0];
            }
#endif

            [str addAttribute:NSParagraphStyleAttributeName
                        value:par
                        range:NSMakeRange(0, [str length])];
            [par release];
        }

        if ( [cell backgroundStyle] == NSBackgroundStyleDark )
        {
            [str removeAttribute:NSForegroundColorAttributeName range:NSMakeRange(0, [str length])];
        }

        [cell setAttributedStringValue:str];
        return true;
    }
#endif // wxUSE_MARKUP

    [cell setObjectValue:wxCFStringRef(GetValue().GetString()).AsNSString()];
    return true;
}

void
wxDataViewTextRenderer::OSXOnCellChanged(NSObject *value,
                                         const wxDataViewItem& item,
                                         unsigned col)
{
    wxVariant valueText(ObjectToString(value));
    if ( !Validate(valueText) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueText, item, col);
}

wxIMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------
wxDataViewBitmapRenderer::wxDataViewBitmapRenderer(const wxString& varianttype,
                                                   wxDataViewCellMode mode,
                                                   int align)
    : wxDataViewRenderer(varianttype,mode,align)
{
    NSCell* cell = [[wxImageCell alloc] init];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

// This method returns 'true' if
//  - the passed bitmap is valid and it could be assigned to the native data
//  browser;
//  - the passed bitmap is invalid (or is not initialized); this case
//  simulates a non-existing bitmap.
// In all other cases the method returns 'false'.
bool wxDataViewBitmapRenderer::MacRender()
{
    if (GetValue().GetType() == wxS("wxBitmap"))
    {
        wxBitmap bitmap;
        bitmap << GetValue();
        if (bitmap.IsOk())
            [GetNativeData()->GetItemCell() setObjectValue:bitmap.GetNSImage()];
    }
    else if (GetValue().GetType() == wxS("wxIcon"))
    {
        wxIcon icon;
        icon << GetValue();
        if (icon.IsOk())
            [GetNativeData()->GetItemCell() setObjectValue:icon.GetNSImage()];
    }
    return true;
}

wxIMPLEMENT_CLASS(wxDataViewBitmapRenderer, wxDataViewRenderer);

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------
wxDataViewChoiceRenderer::wxDataViewChoiceRenderer(const wxArrayString& choices,
                                                   wxDataViewCellMode mode,
                                                   int alignment)
    : wxOSXDataViewDisabledInertRenderer(wxT("string"), mode, alignment),
      m_choices(choices)
{
    NSPopUpButtonCell* cell;


    cell = [[NSPopUpButtonCell alloc] init];
    [cell setControlSize:NSMiniControlSize];
    [cell setFont:[NSFont fontWithName:[[cell font] fontName] size:[NSFont systemFontSizeForControlSize:NSMiniControlSize]]];
    for (size_t i=0; i<choices.GetCount(); ++i)
        [cell addItemWithTitle:wxCFStringRef(choices[i]).AsNSString()];
    wxDataViewRendererNativeData *data = new wxDataViewRendererNativeData(cell);
    data->SetHasCustomFont(true);
    SetNativeData(data);
    [cell release];
}

void
wxDataViewChoiceRenderer::OSXOnCellChanged(NSObject *value,
                                           const wxDataViewItem& item,
                                           unsigned col)
{
    // At least under OS X 10.7 we get the index of the item selected and not
    // its string.
    const long choiceIndex = ObjectToLong(value);

    // We can receive -1 if the selection was cancelled, just ignore it.
    if ( choiceIndex == -1 )
        return;

    // If it's not -1, it must be valid, but avoid crashing in GetChoice()
    // below if it isn't, for some reason.
    wxCHECK_RET( choiceIndex >= 0 && (size_t)choiceIndex < GetChoices().size(),
                 wxS("Choice index out of range.") );

    wxVariant valueChoice(GetChoice(choiceIndex));
    if ( !Validate(valueChoice) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueChoice, item, col);
}

bool wxDataViewChoiceRenderer::MacRender()
{
    [((NSPopUpButtonCell*) GetNativeData()->GetItemCell()) selectItemWithTitle:wxCFStringRef(GetValue().GetString()).AsNSString()];
    return true;
}

wxIMPLEMENT_CLASS(wxDataViewChoiceRenderer, wxDataViewRenderer);

// ----------------------------------------------------------------------------
// wxDataViewChoiceByIndexRenderer
// ----------------------------------------------------------------------------

wxDataViewChoiceByIndexRenderer::wxDataViewChoiceByIndexRenderer(const wxArrayString& choices,
                                                                 wxDataViewCellMode mode,
                                                                 int alignment)
    : wxDataViewChoiceRenderer(choices, mode, alignment)
{
    m_variantType = wxS("long");
}

void
wxDataViewChoiceByIndexRenderer::OSXOnCellChanged(NSObject *value,
                                                  const wxDataViewItem& item,
                                                  unsigned col)
{
    wxVariant valueLong(ObjectToLong(value));
    if ( !Validate(valueLong) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueLong, item, col);
}

bool
wxDataViewChoiceByIndexRenderer::SetValue(const wxVariant& value)
{
    const wxVariant valueStr = GetChoice(value.GetLong());
    return wxDataViewChoiceRenderer::SetValue(valueStr);
}

bool
wxDataViewChoiceByIndexRenderer::GetValue(wxVariant& value) const
{
    wxVariant valueStr;
    if ( !wxDataViewChoiceRenderer::GetValue(valueStr) )
         return false;

    value = (long) GetChoices().Index(valueStr.GetString());
    return true;
}

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

wxDataViewDateRenderer::wxDataViewDateRenderer(const wxString& varianttype,
                                               wxDataViewCellMode mode,
                                               int align)
    : wxDataViewRenderer(varianttype,mode,align)
{
    NSTextFieldCell* cell;

    NSDateFormatter* dateFormatter;


    dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setFormatterBehavior:NSDateFormatterBehavior10_4];
    [dateFormatter setDateStyle:NSDateFormatterShortStyle];
    cell = [[wxTextFieldCell alloc] init];
    [cell setFormatter:dateFormatter];
    SetNativeData(new wxDataViewRendererNativeData(cell,[NSDate dateWithString:@"2000-12-30 20:00:00 +0000"]));
    [cell          release];
    [dateFormatter release];
}

bool wxDataViewDateRenderer::MacRender()
{
    if (!GetValue().GetDateTime().IsValid())
        return true;

    // -- find best fitting style to show the date --
    // as the style should be identical for all cells a reference date
    // instead of the actual cell's date value is used for all cells;
    // this reference date is stored in the renderer's native data
    // section for speed purposes; otherwise, the reference date's
    // string has to be recalculated for each item that may become
    // timewise long if a lot of rows using dates exist; the algorithm
    // has the preference to display as much information as possible
    // in the first instance; but as this is often impossible due to
    // space restrictions the style is shortened per loop; finally, if
    // the shortest time and date format does not fit into the cell
    // the time part is dropped

    // GetObject() returns a date for testing the size of a date object
    [GetNativeData()->GetItemCell() setObjectValue:GetNativeData()->GetObject()];

    bool formatFound = false;
    int  dateFormatterStyle = kCFDateFormatterFullStyle;
    while ( !formatFound && (dateFormatterStyle > 0) )
    {
        int timeFormatterStyle = dateFormatterStyle;

        while ( !formatFound && (timeFormatterStyle >= dateFormatterStyle - 1) )
        {
            [[GetNativeData()->GetItemCell() formatter] setDateStyle:(NSDateFormatterStyle)dateFormatterStyle];
            [[GetNativeData()->GetItemCell() formatter] setTimeStyle:(NSDateFormatterStyle)timeFormatterStyle];
            if ( [GetNativeData()->GetItemCell() cellSize].width <= [GetNativeData()->GetColumnPtr() width] )
                formatFound = true;
            else
                --timeFormatterStyle;
        }
        --dateFormatterStyle;
    }
    // set data (the style is set by the previous loop); on OSX the
    // date has to be specified with respect to UTC; in wxWidgets the
    // date is always entered in the local timezone; so, we have to do
    // a conversion from the local to UTC timezone when adding the
    // seconds to 1970-01-01 UTC:
    [GetNativeData()->GetItemCell() setObjectValue:[NSDate dateWithTimeIntervalSince1970:GetValue().GetDateTime().ToUTC().Subtract(wxDateTime(1,wxDateTime::Jan,1970)).GetSeconds().ToDouble()]];

    return true;
}

void
wxDataViewDateRenderer::OSXOnCellChanged(NSObject *value,
                                         const wxDataViewItem& item,
                                         unsigned col)
{
    wxVariant valueDate(ObjectToDate(value));
    if ( !Validate(valueDate) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueDate, item, col);
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewDateRenderer, wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------
wxDataViewIconTextRenderer::wxDataViewIconTextRenderer(const wxString& varianttype,
                                                       wxDataViewCellMode mode,
                                                       int align)
     : wxDataViewRenderer(varianttype,mode)
{
    wxImageTextCell* cell;


    cell = [[wxImageTextCell alloc] init];
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment(align)];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

bool wxDataViewIconTextRenderer::MacRender()
{
    wxDataViewIconText iconText;

    wxImageTextCell* cell;

    cell = (wxImageTextCell*) GetNativeData()->GetItemCell();
    iconText << GetValue();
    if (iconText.GetIcon().IsOk())
        [cell setImage:wxBitmap(iconText.GetIcon()).GetNSImage()];
    else
        [cell setImage:nil];
    [cell setStringValue:wxCFStringRef(iconText.GetText()).AsNSString()];
    return true;
}

void wxDataViewIconTextRenderer::OSXOnCellChanged(NSObject *value,
                                             const wxDataViewItem& item,
                                             unsigned col)
{
    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    
    // The icon can't be edited so get its old value and reuse it.
    wxVariant valueOld;
    model->GetValue(valueOld, item, col);
    
    wxDataViewIconText iconText;
    iconText << valueOld;
    
    // But replace the text with the value entered by user.
    iconText.SetText(ObjectToString(value));
    
    wxVariant valueIconText;
    valueIconText << iconText;
    
    if ( !Validate(valueIconText) )
        return;
    
    model->ChangeValue(valueIconText, item, col);
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewIconTextRenderer,wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------
wxDataViewToggleRenderer::wxDataViewToggleRenderer(const wxString& varianttype,
                                                   wxDataViewCellMode mode,
                                                   int align)
    : wxOSXDataViewDisabledInertRenderer(varianttype, mode, align)
{
    DoInitButtonCell(NSSwitchButton);
}

void wxDataViewToggleRenderer::DoInitButtonCell(int buttonType)
{
    NSButtonCell* cell;


    cell = [[NSButtonCell alloc] init];
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment(GetAlignment())];
    [cell setButtonType: static_cast<NSButtonType>(buttonType)];
    [cell setImagePosition:NSImageOnly];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

void wxDataViewToggleRenderer::ShowAsRadio()
{
    // This is a bit wasteful, as we always create the cell using
    // NSSwitchButton in the ctor and recreate it here, but modifying the
    // existing cell doesn't seem to work well and delaying the creation of the
    // cell until it's used doesn't seem to be worth it, so just recreate it.
    DoInitButtonCell(NSRadioButton);
}

bool wxDataViewToggleRenderer::MacRender()
{
    [GetNativeData()->GetItemCell() setIntValue:GetValue().GetLong()];
    return true;
}

void
wxDataViewToggleRenderer::OSXOnCellChanged(NSObject *value,
                                           const wxDataViewItem& item,
                                           unsigned col)
{
    wxVariant valueToggle(ObjectToBool(value));
    if ( !Validate(valueToggle) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueToggle, item, col);
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer, wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------
wxDataViewProgressRenderer::wxDataViewProgressRenderer(const wxString& label,
                                                       const wxString& varianttype,
                                                       wxDataViewCellMode mode,
                                                       int align)
    : wxDataViewRenderer(varianttype,mode,align)
{
    wxUnusedVar(label);
    
    NSLevelIndicatorCell* cell;

    cell = [[NSLevelIndicatorCell alloc] initWithLevelIndicatorStyle:NSContinuousCapacityLevelIndicatorStyle];
    [cell setMinValue:0];
    [cell setMaxValue:100];
    SetNativeData(new wxDataViewRendererNativeData(cell));
    [cell release];
}

bool wxDataViewProgressRenderer::MacRender()
{
    [GetNativeData()->GetItemCell() setIntValue:GetValue().GetLong()];
    return true;
}

void
wxDataViewProgressRenderer::OSXOnCellChanged(NSObject *value,
                                             const wxDataViewItem& item,
                                             unsigned col)
{
    wxVariant valueProgress(ObjectToLong(value));
    if ( !Validate(valueProgress) )
        return;

    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    model->ChangeValue(valueProgress, item, col);
}

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer, wxDataViewRenderer);

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

wxDataViewColumn::wxDataViewColumn(const wxString& title,
                                   wxDataViewRenderer* renderer,
                                   unsigned int model_column,
                                   int width,
                                   wxAlignment align,
                                   int flags)
     : wxDataViewColumnBase(renderer, model_column),
       m_NativeDataPtr(new wxDataViewColumnNativeData()),
       m_title(title)
{
    InitCommon(width, align, flags);
    if (renderer && !renderer->IsCustomRenderer() &&
        (renderer->GetAlignment() == wxDVR_DEFAULT_ALIGNMENT))
        renderer->OSXUpdateAlignment();
    SetResizeable((flags & wxDATAVIEW_COL_RESIZABLE) != 0);
}

wxDataViewColumn::wxDataViewColumn(const wxBitmap& bitmap,
                                   wxDataViewRenderer* renderer,
                                   unsigned int model_column,
                                   int width,
                                   wxAlignment align,
                                   int flags)
    : wxDataViewColumnBase(bitmap, renderer, model_column),
      m_NativeDataPtr(new wxDataViewColumnNativeData())
{
    InitCommon(width, align, flags);
    if (renderer && !renderer->IsCustomRenderer() &&
        (renderer->GetAlignment() == wxDVR_DEFAULT_ALIGNMENT))
        renderer->OSXUpdateAlignment();
}

wxDataViewColumn::~wxDataViewColumn()
{
    delete m_NativeDataPtr;
}

int wxDataViewColumn::GetWidth() const
{
    return [m_NativeDataPtr->GetNativeColumnPtr() width];
}

bool wxDataViewColumn::IsSortKey() const
{
    NSTableColumn *nsCol = GetNativeData()->GetNativeColumnPtr();
    return nsCol && ([nsCol sortDescriptorPrototype] != nil);
}

void wxDataViewColumn::SetAlignment(wxAlignment align)
{
    m_alignment = align;
    [[m_NativeDataPtr->GetNativeColumnPtr() headerCell] setAlignment:ConvertToNativeHorizontalTextAlignment(align)];
    if (m_renderer && !m_renderer->IsCustomRenderer() &&
        (m_renderer->GetAlignment() == wxDVR_DEFAULT_ALIGNMENT))
        m_renderer->OSXUpdateAlignment();
}

void wxDataViewColumn::SetBitmap(const wxBitmap& bitmap)
{
    // bitmaps and titles cannot exist at the same time - if the bitmap is set
    // the title is removed:
    m_title.clear();
    wxDataViewColumnBase::SetBitmap(bitmap);
    [[m_NativeDataPtr->GetNativeColumnPtr() headerCell] setImage:bitmap.GetNSImage()];
}

void wxDataViewColumn::SetMaxWidth(int maxWidth)
{
    m_maxWidth = maxWidth;
    [m_NativeDataPtr->GetNativeColumnPtr() setMaxWidth:maxWidth];
}

void wxDataViewColumn::SetMinWidth(int minWidth)
{
    m_minWidth = minWidth;
    [m_NativeDataPtr->GetNativeColumnPtr() setMinWidth:minWidth];
}

void wxDataViewColumn::SetReorderable(bool reorderable)
{
    wxUnusedVar(reorderable);
}

void wxDataViewColumn::SetHidden(bool hidden)
{
    // How to set flag here?

    [m_NativeDataPtr->GetNativeColumnPtr() setHidden:hidden];
}

bool wxDataViewColumn::IsHidden() const
{
    return [m_NativeDataPtr->GetNativeColumnPtr() isHidden];
}

void wxDataViewColumn::SetResizeable(bool resizable)
{
    wxDataViewColumnBase::SetResizeable(resizable);
    if (resizable)
        [m_NativeDataPtr->GetNativeColumnPtr() setResizingMask:NSTableColumnUserResizingMask | NSTableColumnAutoresizingMask];
    else
        [m_NativeDataPtr->GetNativeColumnPtr() setResizingMask:NSTableColumnNoResizing];
}

void wxDataViewColumn::UnsetAsSortKey()
{
    NSTableColumn* const tableColumn = m_NativeDataPtr->GetNativeColumnPtr();
    if ( tableColumn )
        [tableColumn setSortDescriptorPrototype:nil];
}

void wxDataViewColumn::SetSortable(bool sortable)
{
    // wxDataViewColumnBase::SetSortable(sortable);
    // Avoid endless recursion and just set the flag here
    if (sortable)
        m_flags |= wxDATAVIEW_COL_SORTABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_SORTABLE;
}

void wxDataViewColumn::SetSortOrder(bool ascending)
{
    NSTableColumn* const tableColumn = m_NativeDataPtr->GetNativeColumnPtr();
    NSTableView* tableView = [tableColumn tableView];

    wxCHECK_RET( tableView, wxS("Column has to be associated with a table view when the sorting order is set") );

    if ( (m_ascending != ascending) || ([tableColumn sortDescriptorPrototype] == nil) )
    {
        m_ascending = ascending;

        // change sorting order for the native implementation (this will
        // trigger a call to outlineView:sortDescriptorsDidChange: where the
        // wxWidget's sort descriptors are going to be set):
        NSSortDescriptor* const
            sortDescriptor = [[NSSortDescriptor alloc]
                                initWithKey:[NSString stringWithFormat:@"%ld",(long)[tableView columnWithIdentifier:[tableColumn identifier]]]
                                ascending:m_ascending];

        NSArray* sortDescriptors = [NSArray arrayWithObject:sortDescriptor];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
        [tableView setSortDescriptors:sortDescriptors];
        [sortDescriptor release];
    }
}

void wxDataViewColumn::SetTitle(const wxString& title)
{
    // bitmaps and titles cannot exist at the same time - if the title is set
    // the bitmap is removed:
    wxDataViewColumnBase::SetBitmap(wxBitmap());
    m_title = title;
    [[m_NativeDataPtr->GetNativeColumnPtr() headerCell] setStringValue:wxCFStringRef(title).AsNSString()];
}

void wxDataViewColumn::SetWidth(int width)
{
    m_width = width;

    if ( !GetOwner() )
    {
        // can't set the real width yet
        return;
    }

    switch ( width )
    {
        case wxCOL_WIDTH_AUTOSIZE:
            {
                wxCocoaDataViewControl *peer = static_cast<wxCocoaDataViewControl*>(GetOwner()->GetPeer());
                peer->FitColumnWidthToContent(GetOwner()->GetColumnPosition(this));
                break;
            }

        case wxCOL_WIDTH_DEFAULT:
            width = wxDVC_DEFAULT_WIDTH;
            wxFALLTHROUGH;

        default:
            [m_NativeDataPtr->GetNativeColumnPtr() setWidth:width];
            break;
    }
}

void wxDataViewColumn::SetNativeData(wxDataViewColumnNativeData* newNativeDataPtr)
{
    delete m_NativeDataPtr;
    m_NativeDataPtr = newNativeDataPtr;
}

#endif // !defined(wxHAS_GENERIC_DATAVIEWCTRL)

#endif // wxUSE_DATAVIEWCTRL
