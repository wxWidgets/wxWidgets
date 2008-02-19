/////////////////////////////////////////////////////////////////////////////
// Name:        dnd
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page dnd_overview Drag and drop overview

 Classes: #wxDataObject,
 #wxTextDataObject,
 #wxDropSource,
 #wxDropTarget,
 #wxTextDropTarget,
 #wxFileDropTarget
 Note that wxUSE_DRAG_AND_DROP must be defined in setup.h in order
 to use drag and drop in wxWidgets.
 See also: @ref dataobject_overview and @ref samplednd_overview
 It may be noted that data transfer to and from the clipboard is quite
 similar to data transfer with drag and drop and the code to implement
 these two types is almost the same. In particular, both data transfer
 mechanisms store data in some kind of #wxDataObject
 and identify its format(s) using the #wxDataFormat
 class.
 To be a @e drag source, i.e. to provide the data which may be dragged by
 the user elsewhere, you should implement the following steps:


  @b Preparation: First of all, a data object must be created and
 initialized with the data you wish to drag. For example:

 @code
 wxTextDataObject my_data("This text will be dragged.");
 @endcode

 @b Drag start: To start the dragging process (typically in response to a
 mouse click) you must call wxDropSource::DoDragDrop
 like this:

 @code
 wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop( TRUE );
 @endcode

  @b Dragging: The call to DoDragDrop() blocks the program until the user releases the
 mouse button (unless you override the #GiveFeedback function
 to do something special). When the mouse moves in a window of a program which understands the
 same drag-and-drop protocol (any program under Windows or any program supporting the
 XDnD protocol under X Windows), the corresponding #wxDropTarget methods
 are called - see below.
  @b Processing the result: DoDragDrop() returns an @e effect code which
 is one of the values of @c wxDragResult enum (explained #here):

 @code
 switch (result)
  {
      case wxDragCopy: /* copy the data */ break;
case wxDragMove: /* move the data */
break;
default:         /* do nothing */
break;
}
@endcode



To be a @e drop target, i.e. to receive the data dropped by the user you should
follow the instructions below:


@b Initialization: For a window to be a drop target, it needs to have
an associated #wxDropTarget object. Normally, you will
call wxWindow::SetDropTarget during window
creation associating your drop target with it. You must derive a class from
            wxDropTarget and override its pure virtual methods. Alternatively, you may
            derive from #wxTextDropTarget or
#wxFileDropTarget and override their OnDropText()
            or OnDropFiles() method.
            @b Drop: When the user releases the mouse over a window, wxWidgets
            asks the associated wxDropTarget object if it accepts the data. For this,
            a #wxDataObject must be associated with the drop target
            and this data object will be responsible for the format negotiation between
                the drag source and the drop target. If all goes well, then #OnData
                will get called and the wxDataObject belonging to the drop target can get
                filled with data.
                @b The end: After processing the data, DoDragDrop() returns either
                    wxDragCopy or wxDragMove depending on the state of the keys Ctrl, Shift
                    and Alt at the moment of the drop. There is currently no way for the drop
                    target to change this return code.

                                                 */


