/////////////////////////////////////////////////////////////////////////////
// Name:        container.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_container Container Classes

Classes: wxList<T>, wxArray<T>, wxVector<T>, wxStack<T>, wxHashMap, wxHashSet

@section overview_container_intro Overview

For historical reasons, wxWidgets uses custom container classes internally.
This was unfortunately unavoidable during a long time when the standard library
wasn't widely available and can't be easily changed even now that it is for
compatibility reasons. If you are building your own version of the library and
don't care about compatibility nor slight (less than 5%) size penalty imposed
by the use of STL classes, you may choose to use the "STL" build of wxWidgets
in which these custom classes are replaced with their standard counterparts and
only read the section @ref overview_container_std explaining how to do it.

Otherwise you will need to know about the custom wxWidgets container classes
such as wxList<T> and wxArray<T> if only to use wxWidgets functions that work
with them, e.g. wxWindow::GetChildren(), and you should find the information
about using these classes below useful.

Notice that we recommend that you use standard classes directly in your own
code instead of the container classes provided by wxWidgets in any case as the
standard classes are easier to use and may also be safer because of extra
run-time checks they may perform as well as more efficient.

Finally notice that recent versions of wxWidgets also provide standard-like
classes such as wxVector<T>, wxStack<T> or wxDList which can be used exactly
like the std::vector<T>, std::stack<T> and std::list<T*>, respectively, and
actually are just typedefs for the corresponding types if wxWidgets is compiled
in STL mode. These classes could be useful if you wish to avoid the use of the
standard library in your code for some reason.

To summarize, you should use the standard container classes such as
std::vector<T> and std::list<T> if possible and wxVector<T> or wxDList<T> if
it isn't and only use legacy wxWidgets containers such as wxArray<T> and
wxList<T> when you must, i.e. when you use a wxWidgets function taking or
returning a container of such type.


@section overview_container_legacy Legacy Classes

The list classes in wxWidgets are doubly-linked lists which may either own the
objects they contain (meaning that the list deletes the object when it is
removed from the list or the list itself is destroyed) or just store the
pointers depending on whether or not you called wxList<T>::DeleteContents()
method.

Dynamic arrays resemble C arrays but with two important differences: they
provide run-time range checking in debug builds and they automatically expand
the allocated memory when there is no more space for new items. They come in
two sorts: the "plain" arrays which store either built-in types such as "char",
"int" or "bool" or the pointers to arbitrary objects, or "object arrays" which
own the object pointers to which they store.

For the same portability reasons, the container classes implementation in
wxWidgets don't use templates, but are rather based on C preprocessor i.e. are
implemented using the macros: WX_DECLARE_LIST() and WX_DEFINE_LIST() for the
linked lists and WX_DECLARE_ARRAY(), WX_DECLARE_OBJARRAY() and
WX_DEFINE_OBJARRAY() for the dynamic arrays.

The "DECLARE" macro declares a new container class containing the elements of
given type and is needed for all three types of container classes: lists,
arrays and objarrays. The "DEFINE" classes must be inserted in your program in
a place where the @e full declaration of container element class is in scope
(i.e. not just forward declaration), otherwise destructors of the container
elements will not be called!

As array classes never delete the items they contain anyhow, there is no
WX_DEFINE_ARRAY() macro for them.

Examples of usage of these macros may be found in wxList<T> and wxArray<T>
documentation.

Finally, wxWidgets predefines several commonly used container classes. wxList
is defined for compatibility with previous versions as a list containing
wxObjects and wxStringList as a list of C-style strings (char *), both of these
classes are deprecated and should not be used in new programs. The following
array classes are defined: wxArrayInt, wxArrayLong, wxArrayPtrVoid and
wxArrayString. The first three store elements of corresponding types, but
wxArrayString is somewhat special: it is an optimized version of wxArray which
uses its knowledge about wxString reference counting schema.


@section overview_container_std STL Build

To build wxWidgets with the standard containers you need to set
wxUSE_STD_CONTAINERS option to 1 in @c wx/msw/setup.h for wxMSW builds or
specify @c --enable-std_containers option to configure (which is also
implicitly enabled by @c --enable-stl option) in Unix builds.

The standard container build is mostly, but not quite, compatible with the
default one. Here are the most important differences:
 - wxList::compatibility_iterator must be used instead of wxList::Node* when
   iterating over the list contents. The compatibility_iterator class has the
   same semantics as a Node pointer but it is an object and not a pointer, so
   you need to write
        @code
        for ( wxWindowList::compatibility_iterator it = list.GetFirst();
              it;
              it = it->GetNext() )
            ...
        @endcode
   instead of the old
        @code
        for ( wxWindowList::Node *n = list.GetFirst(); n; n = n->GetNext() )
            ...
        @endcode
 - wxSortedArrayString and wxArrayString are separate classes now and the
   former doesn't derive from the latter. If you need to convert a sorted array
   to a normal one, you must copy all the elements. Alternatively, you may
   avoid the use of wxSortedArrayString by using a normal array and calling its
   Sort() method when needed.
 - WX_DEFINE_ARRAY_INT(bool) cannot be used because of the differences in
   std::vector<bool> specialization compared with the generic std::vector<>
   class. Please either use std::vector<bool> directly or use an integer array
   instead.


*/

