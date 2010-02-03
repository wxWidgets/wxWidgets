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

bool wxDropTarget::CurrentDragHasSupportedFormat()
{
    if (m_dataObject == NULL)
        return false;

    return m_dataObject->HasDataInPasteboard( m_currentDragPasteboard );
}

bool wxDropTarget::GetData()
{
    if (m_dataObject == NULL)
        return false;

    if ( !CurrentDragHasSupportedFormat() )
        return false;

    return m_dataObject->GetFromPasteboard( m_currentDragPasteboard );
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
    }


    return result;
}

#endif // wxUSE_DRAG_AND_DROP

