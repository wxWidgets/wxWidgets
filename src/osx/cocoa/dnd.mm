///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dnd.mm
// Purpose:     wxDropTarget, wxDropSource implementations
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: dnd.cpp 61724 2009-08-21 10:41:26Z VZ $
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/evtloop.h"
    #include "wx/toplevel.h"
    #include "wx/gdicmn.h"
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <AppKit/AppKit.h>
#include "wx/osx/private.h"

wxDropSource* gCurrentSource = NULL;

wxDragResult NSDragOperationToWxDragResult(NSDragOperation code)
{
    switch (code)
    {
        case NSDragOperationCopy:
            return wxDragCopy;
        case NSDragOperationMove:
            return wxDragMove;
        case NSDragOperationLink:
            return wxDragLink;
        case NSDragOperationNone:
            return wxDragNone;
        default:
            wxFAIL_MSG("Unexpected result code");
    }
}

@interface DropSourceDelegate : NSObject
{
    BOOL dragFinished;
    int resultCode;
    wxDropSource* impl;
}

- (void)setImplementation: (wxDropSource *)dropSource;
- (BOOL)finished;
- (NSDragOperation)code;
- (void)draggedImage:(NSImage *)anImage movedTo:(NSPoint)aPoint;
- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation;
@end

@implementation DropSourceDelegate

- (id)init
{
    [super init];
    dragFinished = NO;
    resultCode = NSDragOperationNone;
    impl = 0;
    return self;
}

- (void)setImplementation: (wxDropSource *)dropSource
{
    impl = dropSource;
}

- (BOOL)finished
{
    return dragFinished;
}

- (NSDragOperation)code
{
    return resultCode;
}

- (void)draggedImage:(NSImage *)anImage movedTo:(NSPoint)aPoint
{
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
                wxCursor cursor( cursorID );
                cursor.MacInstall();
            }
        }
    }
}

- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation
{
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

wxDragResult wxDropSource::DoDragDrop(int flags)
{
    wxASSERT_MSG( m_data, wxT("Drop source: no data") );

    wxDragResult result = wxDragNone;
    if ((m_data == NULL) || (m_data->GetFormatCount() == 0))
        return result;

    NSView* view = m_window->GetPeer()->GetWXWidget();
    if (view)
    {
        NSPasteboard *pboard;
 
        pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
    
        OSStatus err = noErr;
        PasteboardRef pboardRef;
        PasteboardCreate((CFStringRef)[pboard name], &pboardRef);
        
        err = PasteboardClear( pboardRef );
        if ( err != noErr )
        {
            CFRelease( pboardRef );
            return wxDragNone;
        }
        PasteboardSynchronize( pboardRef );
        
        m_data->AddToPasteboard( pboardRef, 1 );
        
        NSEvent* theEvent = (NSEvent*)wxTheApp->MacGetCurrentEvent();
        wxASSERT_MSG(theEvent, "DoDragDrop must be called in response to a mouse down or drag event.");
        
        gCurrentSource = this;
        NSImage* image = [[NSImage alloc] initWithSize: NSMakeSize(16,16)];
        DropSourceDelegate* delegate = [[DropSourceDelegate alloc] init];
        [delegate setImplementation: this];
        [view dragImage:image at:NSMakePoint(0.0, 16.0) offset:NSMakeSize(0.0,0.0) 
            event: theEvent pasteboard: pboard source:delegate slideBack: NO];
        
        wxEventLoopBase * const loop = wxEventLoop::GetActive();
        while ( ![delegate finished] )
            loop->Dispatch();
        
        result = NSDragOperationToWxDragResult([delegate code]);
        [delegate release];
        [image release];
        gCurrentSource = NULL;
    }


    return result;
}

#endif // wxUSE_DRAG_AND_DROP

