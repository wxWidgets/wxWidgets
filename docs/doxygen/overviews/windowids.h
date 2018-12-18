/////////////////////////////////////////////////////////////////////////////
// Name:        windowids.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_windowids Window IDs

@tableofcontents

When creating a new wxWindow-derived class or adding a menu item, its ID must
be specified. An ID is just a unique (at least locally, i.e. inside the same
top level window) integer allowing to find the window or menu item later and to
distinguish between events from different objects.

If applicable, stock IDs such as ::wxID_EXIT (for menu item) or ::wxID_OK (for
a button) should be used, see @ref page_stockitems for the full list of such
IDs.

If the value of an ID is not important, ::wxID_ANY should be used, telling
wxWidgets to allocate a unique ID automatically. All such
automatically-assigned IDs are negative and so won't conflict with any
user-defined IDs as long as they are positive.

If you do care about the ID value but don't want to specify it as a literal in
your code, you can use wxWindow::NewControlId() to create an ID that had never
been returned by this function before. Such IDs are also negative.

Finally, you can just define your own IDs. Typically this is done by using a
C++ enum to automatically ensure their uniqueness. If you do this, please note
that your custom IDs must be positive to avoid clashes with the automatically
assigned IDs discussed above and should @e not have values 0 or 1, that can
result in surprising behaviour under some platforms. Finally, you also need to
avoid defining IDs in the range from ::wxID_LOWEST to ::wxID_HIGHEST which is
reserved for wxWidgets-defined IDs, see ::wxStandardID for more details.

To avoid all these restrictions, it is best to avoid using hard-coded IDs at
all, they are not needed when using wxEvtHandler::Bind() for event handling
(unlike with the previously used event tables).


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
