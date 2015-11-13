/////////////////////////////////////////////////////////////////////////////
// Name:        windowids.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_windowids Window IDs

@tableofcontents

Various controls and other parts of wxWidgets need an ID.  Sometimes the ID may
be directly provided by the user or have a predefined value, such as
@c wxID_OPEN. Often, however, the value of the ID is unimportant and in this
case it is enough to use @c wxID_ANY as the ID of an object which tells
wxWidgets to assign an ID automatically. All such automatically-assigned IDs
are negative, so the IDs predefined in the user code should always be positive
to avoid clashes with them.

If you do care about the ID value but don't want to specify it as a literal in
your code, you can use wxWindow::NewControlId() to create an ID that had never
been returned by this function before and, being also negative, never conflicts
with any IDs explicitly defined in the program if the advice above is followed.

@see wxIdManager, wxWindow::NewControlId(), wxWindow::UnreserveControlId()


@section overview_windowids_type Data Types

A wxWindowID is just the integer type for a window ID.  It should be used
almost everywhere.  To help keep track of the count for the automatically
generated IDs, a new type, wxWindowIDRef exists, that can take the place of
wxWindowID where needed. When an ID is first created, it is marked as reserved.
When assigning it to a wxWindowIDRef, the usage count of the ID is increased,
or set to 1 if it is currently reserved.  Assigning the same ID to several
wxWindowIDRefs will keep track of the count. As the wxWindowIDRef gets
destroyed or its value changes, it will decrease the count of the used ID. When
there are no more wxWindowIDRef types with the created ID, the ID is considered
free and can then be used again by wxWindow::NewControlId.

If a created ID is not assigned to a wxWindowIDRef, then it remains reserved
until it is unreserved manually with wxWindow::UnreserveControlId. However, if
it is assigned to a wxWindowIDRef, then it will be unreserved automatically and
will be considered free when the count is 0, and should NOT be manually
unreserved.

wxWindowIDRef can store both automatic IDs from wxWindow::NewControlId and
normal IDs.  Reference counting is only done for the automatic IDs.  Also,
wxWindowIDRef has conversion operators that allow it to be treated just like a
wxWindowID.


@section overview_windowids_using Using wxWindowIDRef

A wxWindowIDRef should be used in place of a wxWindowID where you want to make
sure the ID is not created again by wxWindow::NewControlId at least until the
wxWindowIDRef is destroyed, usually when the associated object is destroyed.
This is done already for windows, menu items, and tool bar items. It should
only be used in the main thread, as it is not thread safe.

*/
