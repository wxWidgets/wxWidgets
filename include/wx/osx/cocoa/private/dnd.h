#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD

#ifndef WX_PRECOMP
#include "wx/object.h"
#endif

#include "wx/dnd.h"

#include "wx/osx/private.h"
#include "wx/osx/private/datatransfer.h"

@interface DropSourceDelegate : NSObject<NSDraggingSource>
{
    BOOL dragFinished;
    int resultCode;
    wxDropSource* impl;

    // Flags for drag and drop operations (wxDrag_* ).
    int m_dragFlags;
    NSImage* m_copy_cursor;
    NSImage* m_move_cursor;
    NSImage* m_none_cursor;
}

- (void)setImplementation:(nonnull wxDropSource *)dropSource flags:(int)flags;
- (BOOL)finished;
- (NSDragOperation)code;
- (NSDragOperation)draggingSession:(nonnull NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context;
- (void)draggedImage:(nonnull NSImage *)anImage movedTo:(NSPoint)aPoint;
- (void)draggedImage:(nonnull NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation;
- (nullable NSImage*)cursorForStatus:(wxDragResult)status;
@end

#endif