///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dnd.mm
// Purpose:     wxDropTarget, wxDropSource implementations
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD

#ifndef WX_PRECOMP
#include "wx/object.h"
#endif

#include "wx/dnd.h"
#include "wx/clipbrd.h"
#include "wx/filename.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/gdicmn.h"
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/evtloop.h"

#include "wx/osx/private.h"
#include "wx/osx/private/datatransfer.h"

wxOSXDataSinkItem::~wxOSXDataSinkItem()
{
}

void wxOSXDataSinkItem::SetFilename(const wxString& filename)
{
    wxCFRef<CFURLRef> url(wxOSXCreateURLFromFileSystemPath(filename));
    wxCFRef<CFDataRef> data(CFURLCreateData(NULL,url,kCFStringEncodingUTF8,true));
    DoSetData( kUTTypeFileURL, data);
}

wxOSXDataSourceItem::~wxOSXDataSourceItem()
{
}

bool wxOSXDataSource::IsSupported(const wxDataFormat &dataFormat)
{
    wxCFMutableArrayRef<CFStringRef> typesarray;
    dataFormat.AddSupportedTypesForSetting(typesarray);
    return HasData(typesarray);
}

bool wxOSXDataSource::IsSupported(const wxDataObject &dataobj)
{
    wxCFMutableArrayRef<CFStringRef> typesarray;
    dataobj.AddSupportedTypes(typesarray, wxDataObjectBase::Direction::Get);
    return HasData(typesarray);
}

class WXDLLIMPEXP_CORE wxOSXPasteboardSinkItem : public wxOSXDataSinkItem
{
public:
    wxOSXPasteboardSinkItem(NSPasteboardItem* item): m_item(item)
    {
    }

    ~wxOSXPasteboardSinkItem()
    {

    }

    virtual void SetData(const wxDataFormat& format, const void *buf, size_t datasize)
    {
        SetData( format.GetFormatId(), buf, datasize);
    }

    virtual void SetData(wxDataFormat::NativeFormat format, const void *buf, size_t datasize)
    {
        wxCFRef<CFDataRef> data(CFDataCreate( kCFAllocatorDefault, (UInt8*)buf, datasize ));
        DoSetData(format, data);
    }

    virtual void DoSetData(wxDataFormat::NativeFormat format, CFDataRef data)
    {
        [m_item setData:(NSData*) data forType:(NSString*) format];
    }

    NSPasteboardItem* GetNative() { return m_item; }
private:
    NSPasteboardItem* m_item;
};

class WXDLLIMPEXP_CORE wxOSXPasteboardSourceItem : public wxOSXDataSourceItem
{
public:
    wxOSXPasteboardSourceItem(NSPasteboardItem* item, NSPasteboard* board): m_item(item), m_pasteboard(board)
    {
    }

    virtual wxDataFormat::NativeFormat AvailableType(CFArrayRef types) const
    {
        return (wxDataFormat::NativeFormat)[m_item availableTypeFromArray:(NSArray*)types];
    }

    virtual bool GetData( const wxDataFormat& dataFormat, wxMemoryBuffer& target)
    {
        return GetData(dataFormat.GetFormatId(), target);
    }


    virtual bool GetData( wxDataFormat::NativeFormat type, wxMemoryBuffer& target)
    {
        bool success = false;

        target.Clear();

        CFDataRef flavorData = DoGetData( type );

        if ( flavorData )
        {
            CFIndex flavorDataSize = CFDataGetLength(flavorData);
            size_t allocatedSize = flavorDataSize+4;
            void * buf = target.GetWriteBuf(allocatedSize);
            if ( buf )
            {
                memset(buf, 0, allocatedSize);
                memcpy(buf, CFDataGetBytePtr(flavorData), flavorDataSize);
                target.UngetWriteBuf(flavorDataSize);
                success = true;
            }
        }
        return success;
    }

    virtual CFDataRef DoGetData(wxDataFormat::NativeFormat type) const
    {
        // before a file promise can be resolved, we must pass a paste location
        if ( UTTypeConformsTo((CFStringRef)type, kPasteboardTypeFileURLPromise ) )
        {
            wxString tempdir = wxFileName::GetTempDir() + wxFILE_SEP_PATH + "wxtemp.XXXXXX";
            char* result = mkdtemp((char*)tempdir.fn_str().data());

            wxCFRef<CFURLRef> dest(CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8*)result, strlen(result), true));
            PasteboardRef pboardRef = NULL;
            PasteboardCreate((CFStringRef)[m_pasteboard name], &pboardRef);
            if (pboardRef != NULL) {
                PasteboardSynchronize(pboardRef);
                PasteboardSetPasteLocation(pboardRef, (CFURLRef)dest);
                CFRelease(pboardRef);
            }
        }

        return (CFDataRef) [m_item dataForType:(NSString*) type];
    }

    NSPasteboardItem* GetNative() { return m_item; }
private:
    NSPasteboardItem* m_item;
    NSPasteboard* m_pasteboard;
};

wxOSXPasteboard::wxOSXPasteboard(OSXPasteboard native)
{
    m_pasteboard = native;
}

wxOSXPasteboard::~wxOSXPasteboard()
{
    DeleteSinkItems();
}

void wxOSXPasteboard::DeleteSinkItems()
{
    for ( wxVector<wxOSXDataSinkItem*>::iterator it = m_sinkItems.begin();
     it != m_sinkItems.end();
     ++it)
    {
        delete (*it);
    }
    m_sinkItems.clear();
}

// data sink methods

void wxOSXPasteboard::Clear()
{
    [m_pasteboard clearContents];
    DeleteSinkItems();    
}

void wxOSXPasteboard::Flush()
{
    NSMutableArray* nsarray = [[NSMutableArray alloc] init];
    for ( wxVector<wxOSXDataSinkItem*>::iterator it = m_sinkItems.begin();
         it != m_sinkItems.end();
         ++it)
    {
        wxOSXPasteboardSinkItem* item = dynamic_cast<wxOSXPasteboardSinkItem*>(*it);
        [nsarray addObject:item->GetNative()];
        delete item;
    }
    m_sinkItems.clear();
    [m_pasteboard writeObjects:nsarray];
    [nsarray release];
}

wxOSXDataSinkItem* wxOSXPasteboard::CreateItem()
{
    NSPasteboardItem* nsitem = [[NSPasteboardItem alloc] init];
    wxOSXPasteboardSinkItem* item = new wxOSXPasteboardSinkItem(nsitem);
    m_sinkItems.push_back(item);

    return item;
}

// data source methods

bool wxOSXPasteboard::HasData(CFArrayRef types) const
{
    return [m_pasteboard canReadItemWithDataConformingToTypes:(NSArray*) types];
}

const wxOSXDataSourceItem* wxOSXPasteboard::GetItem(size_t pos) const
{
    return new wxOSXPasteboardSourceItem([[m_pasteboard pasteboardItems] objectAtIndex: pos], m_pasteboard);
}
 
// data source methods

wxOSXPasteboard* wxOSXPasteboard::GetGeneralClipboard()
{
    static wxOSXPasteboard clipboard( [NSPasteboard pasteboardWithName:NSGeneralPboard]);
    return &clipboard;
}

size_t wxOSXPasteboard::GetItemCount() const
{
    return [[m_pasteboard pasteboardItems] count];
}

#if wxUSE_DRAG_AND_DROP

wxDropSource* gCurrentSource = NULL;

wxDragResult NSDragOperationToWxDragResult(NSDragOperation code)
{
    switch (code)
    {
        case NSDragOperationGeneric:
            return wxDragCopy;
        case NSDragOperationCopy:
            return wxDragCopy;
        case NSDragOperationMove:
            return wxDragMove;
        case NSDragOperationLink:
            return wxDragLink;
        case NSDragOperationNone:
            return wxDragNone;
        case NSDragOperationDelete:
            return wxDragNone;
        default:
            wxFAIL_MSG("Unexpected result code");
    }
    return wxDragNone;
}

@interface DropSourceDelegate : NSObject<NSDraggingSource>
{
    BOOL dragFinished;
    int resultCode;
    wxDropSource* impl;

    // Flags for drag and drop operations (wxDrag_* ).
    int m_dragFlags;
}

- (void)setImplementation:(wxDropSource *)dropSource flags:(int)flags;
- (BOOL)finished;
- (NSDragOperation)code;
- (NSDragOperation)draggingSession:(nonnull NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context;
- (void)draggedImage:(NSImage *)anImage movedTo:(NSPoint)aPoint;
- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation;
@end

@implementation DropSourceDelegate

- (id)init
{
    if ( self = [super init] )
    {
        dragFinished = NO;
        resultCode = NSDragOperationNone;
        impl = 0;
        m_dragFlags = wxDrag_CopyOnly;
    }
    return self;
}

- (void)setImplementation:(wxDropSource *)dropSource flags:(int)flags
{
    impl = dropSource;
    m_dragFlags = flags;
}

- (BOOL)finished
{
    return dragFinished;
}

- (NSDragOperation)code
{
    return resultCode;
}

- (NSDragOperation)draggingSession:(nonnull NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context
{
    wxUnusedVar(session);
    wxUnusedVar(context);

    NSDragOperation allowedDragOperations = NSDragOperationEvery;

    // NSDragOperationGeneric also makes a drag to the trash possible
    // resulting in something we don't support (NSDragOperationDelete)

    allowedDragOperations &= ~(NSDragOperationDelete | NSDragOperationGeneric);

    if (m_dragFlags == wxDrag_CopyOnly)
    {
        allowedDragOperations &= ~NSDragOperationMove;
    }

    // we might adapt flags here in the future
    // context can be NSDraggingContextOutsideApplication or NSDraggingContextWithinApplication

    return allowedDragOperations;
}

- (void)draggedImage:(NSImage *)anImage movedTo:(NSPoint)aPoint
{
    wxUnusedVar( anImage );
    wxUnusedVar( aPoint );

    bool optionDown = GetCurrentKeyModifiers() & optionKey;
    wxDragResult result = optionDown ? wxDragCopy : wxDragMove;

    if (wxDropSource* source = impl)
    {
        if (!source->GiveFeedback(result))
        {
            wxStockCursor cursorID = wxCURSOR_NONE;

            switch (result)
            {
                case wxDragCopy:
                    cursorID = wxCURSOR_COPY_ARROW;
                    break;

                case wxDragMove:
                    cursorID = wxCURSOR_ARROW;
                    break;

                case wxDragNone:
                    cursorID = wxCURSOR_NO_ENTRY;
                    break;

                case wxDragError:
                case wxDragLink:
                case wxDragCancel:
                default:
                    // put these here to make gcc happy
                    ;
            }

            if (cursorID != wxCURSOR_NONE)
            {
                // TODO under 10.6 the os itself deals with the cursor, remove if things
                // work properly everywhere
#if 0
                wxCursor cursor( cursorID );
                cursor.MacInstall();
#endif
            }
        }
    }
}

- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation
{
    wxUnusedVar( anImage );
    wxUnusedVar( aPoint );

    resultCode = operation;
    dragFinished = YES;
}

@end

wxDropTarget::wxDropTarget( wxDataObject *data )
            : wxDropTargetBase( data )
{

}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

wxDropSource::wxDropSource(wxWindow *win,
                           const wxCursor &cursorCopy,
                           const wxCursor &cursorMove,
                           const wxCursor &cursorStop)
            : wxDropSourceBase(cursorCopy, cursorMove, cursorStop)
{
    m_window = win;
}

wxDropSource::wxDropSource(wxDataObject& data,
                           wxWindow *win,
                           const wxCursor &cursorCopy,
                           const wxCursor &cursorMove,
                           const wxCursor &cursorStop)
            : wxDropSourceBase(cursorCopy, cursorMove, cursorStop)
{
    SetData( data );
    m_window = win;
}

wxDropSource* wxDropSource::GetCurrentDropSource()
{
    return gCurrentSource;
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_13
typedef NSString* NSPasteboardType;
#endif

@interface wxPasteBoardWriter : NSObject<NSPasteboardWriting>
{
    wxDataObject* m_data;
}

- (id) initWithDataObject:(wxDataObject*) obj;
@end

@implementation wxPasteBoardWriter

- (id) initWithDataObject:(wxDataObject*) obj
{
    m_data = obj;
    return self;
}

- (void) clearDataObject
{
    m_data = NULL;
}
- (nullable id)pasteboardPropertyListForType:(nonnull NSPasteboardType)type
{
    if ( m_data )
    {
        wxDataFormat format((wxDataFormat::NativeFormat) type);
        size_t size = m_data->GetDataSize(format);
        CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault,size );
        m_data->GetDataHere(format, CFDataGetMutableBytePtr(data));
        CFDataSetLength(data, size);
        return (id) data;
    }
    return nil;
}

- (nonnull NSArray<NSPasteboardType> *)writableTypesForPasteboard:(nonnull NSPasteboard *)pasteboard
{
    wxUnusedVar(pasteboard);
    wxCFMutableArrayRef<CFStringRef> typesarray;
    if ( m_data )
        m_data->AddSupportedTypes(typesarray, wxDataObjectBase::Direction::Get);
    return (NSArray<NSPasteboardType>*) typesarray.autorelease();
}

@end

wxDragResult wxDropSource::DoDragDrop(int flags)
{
    wxASSERT_MSG( m_data, wxT("Drop source: no data") );

    wxDragResult result = wxDragNone;
    if ((m_data == NULL) || (m_data->GetFormatCount() == 0))
        return result;

    NSView* view = m_window->GetPeer()->GetWXWidget();
    if (view)
    {
        NSEvent* theEvent = (NSEvent*)wxTheApp->MacGetCurrentEvent();
        wxASSERT_MSG(theEvent, "DoDragDrop must be called in response to a mouse down or drag event.");

        gCurrentSource = this;

        DropSourceDelegate* delegate = [[DropSourceDelegate alloc] init];
        [delegate setImplementation:this flags:flags];

        // add a dummy square as dragged image for the moment,
        // TODO: proper drag image for data
        NSSize sz = NSMakeSize(16,16);
        NSRect fillRect = NSMakeRect(0, 0, 16, 16);
        NSImage* image = [[NSImage alloc] initWithSize: sz];

        [image lockFocus];

        [[[NSColor whiteColor] colorWithAlphaComponent:0.8] set];
        NSRectFill(fillRect);
        [[NSColor blackColor] set];
        NSFrameRectWithWidthUsingOperation(fillRect,1.0f,NSCompositeDestinationOver);

        [image unlockFocus];

        NSPoint down = [theEvent locationInWindow];
        NSPoint p = [view convertPoint:down fromView:nil];

        wxPasteBoardWriter* writer = [[wxPasteBoardWriter alloc] initWithDataObject:m_data];
        wxCFMutableArrayRef<NSDraggingItem*> items;
        NSDraggingItem* item = [[NSDraggingItem alloc] initWithPasteboardWriter:writer];
        [item setDraggingFrame:NSMakeRect(p.x, p.y, 16, 16) contents:image];
        items.push_back(item);
        [view beginDraggingSessionWithItems:items event:theEvent source:delegate];

        wxEventLoopBase * const loop = wxEventLoop::GetActive();
        while ( ![delegate finished] )
            loop->Dispatch();

        result = NSDragOperationToWxDragResult([delegate code]);
        [delegate release];
        [image release];
        [writer clearDataObject];
        [writer release];

        wxWindow* mouseUpTarget = wxWindow::GetCapture();

        if ( mouseUpTarget == NULL )
        {
            mouseUpTarget = m_window;
        }

        if ( mouseUpTarget != NULL )
        {
            wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
            ((wxWidgetCocoaImpl*)mouseUpTarget->GetPeer())->SetupMouseEvent(wxevent , theEvent) ;
            wxevent.SetEventType(wxEVT_LEFT_UP);

            mouseUpTarget->HandleWindowEvent(wxevent);
        }

        gCurrentSource = NULL;
    }


    return result;
}

#endif // wxUSE_DRAG_AND_DROP
#endif // wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD
