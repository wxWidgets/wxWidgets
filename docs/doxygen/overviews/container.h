/////////////////////////////////////////////////////////////////////////////
// Name:        container.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_container Container Classes

@tableofcontents

For historical reasons, wxWidgets uses custom container classes internally.
This was unfortunately unavoidable during a long time when the standard library
wasn't widely available and can't be easily changed even now that it is for
compatibility reasons. However, starting from version 3.3.0 these custom
container classes are implemented in terms of the standard library classes by
default and so are mostly interoperable with them.

This means that even if some wxWidgets function is documented as taking or
returning a legacy container such as wxList<T> or wxArray<T>, you can work with
them as if they were just `std::list<T*>` or `std::vector<T>`. For example,
wxWidgets::GetChildren() is documented as returning a `wxWindowList`, which
inherits from `std::list<wxWindow*>` and so can be used as such, e.g.

@code
    const auto& children = someFrame->GetChildren();
    auto it = std::find(children.begin(), children.end(), someWindow);
    if ( it != children.end() )
        // ... do something with this window ...
@endcode

You may still need to create wxWidgets containers to pass them to some
wxWidgets function, e.g. wxDir::GetAllFiles() requires a wxArrayString to be
passed in. However in this case you may still the object as if it were just a
`std::vector<wxString>` and, other than that, there is no reason to use the
container classes provided by wxWidgets anywhere in the new code, they only
exist for backwards compatibility and standard library classes should be used
by the application.

@see @ref group_class_containers



@section overview_container_legacy Legacy Classes

This section contains a brief overview of the legacy container classes and is
mostly useful if you need to maintain an existing code base using them -- none
of this is really necessary to know if you follow the advice above and treat
all wxWidgets containers as if they were standard containers.

All these classes use macro-based machinery to define type-safe classes based
on the generic implementation. Generally speaking, this consists of using one
macro to declare a particular "instantiation" and then another one to actually
define it, e.g. WX_DECLARE_LIST() and WX_DEFINE_LIST() for the linked lists or
WX_DECLARE_OBJARRAY() and WX_DEFINE_OBJARRAY() for the arrays containing
objects. The array classes storing primitive types, such as `int`, don't need
to be declared and defined separately, and so only WX_DEFINE_ARRAY_INT()
exists, without the matching "DECLARE" macro.

The "DECLARE" macro declares a new container class containing the elements of
given type and can be used in a header file and don't require the full
declaration of the contained type. The "DEFINE" macros must be used in a place
where the @e full declaration of container element class is in scope (i.e. not
just forward declaration).

Finally, wxWidgets predefines several commonly used container classes. wxList
itself is defined for compatibility with very old versions of wxWidgets as a
list containing wxObjects and wxStringList as a list of C-style strings
(`char *`). There are also a few predefined array classes including wxArrayInt,
wxArrayLong, wxArrayPtrVoid and wxArrayString, similar to `std::vector`
instantiations for `int`, `long`, `void*` and `wxString` respectively.



@section overview_container_impls Different Implementations

Since wxWidgets 3.3.0, all legacy container classes are implemented in terms of
the standard library classes. These implementations are mostly, but not
completely, compatible with the implementation provided by wxWidgets itself
which was used by default in the previous versions.

If the existing code doesn't compile with the implementation based on the
standard containers, it's possible to rebuild the library to use the legacy
implementation of them by setting wxUSE_STD_CONTAINERS option to 0. This is
done by:

- Either manually editing `wx/msw/setup.h` file to change the option of this
  value there for wxMSW.
- Or using `--disable-std_containers` option when calling configure when
  building wxWidgets under Unix-like systems.
- Or using `-DwxUSE_STD_CONTAINERS=OFF` when using CMake build system.


However a better approach is to update the legacy code to work with the new
container classes implementation. In most cases this can be done in such a way
that they still continue to work with the legacy implementation as well, thus
allowing the application to compile with the default configurations of both
wxWidgets 3.3 and 3.2 and even earlier versions.

Here are some modifications that may need to be done in order to make the
existing compatible with the default build of wxWidgets 3.3:

 - `wxList::compatibility_iterator` must be used instead of `wxList::Node*` when
   iterating over the list contents. The `compatibility_iterator` class has the
   same semantics as a `Node` pointer but it is an object and not a pointer, so
   the old code doing
        @code
        for ( wxWindowList::Node *n = list.GetFirst(); n; n = n->GetNext() ) {
            // ... do something with *n ...
        }
        @endcode
   can be mechanically rewritten as
        @code
        for ( wxWindowList::compatibility_iterator it = list.GetFirst();
              it;
              it = it->GetNext() ) {
            // ... do something with *it ...
        }
        @endcode
   Of course, if compatibility with the previous wxWidgets versions is not
   important, an even better and simpler way to rewrite it is
        @code
        for ( const auto& elem: list ) {
            // ... do something with elem ...
        }
        @endcode
 - wxSortedArrayString and wxArrayString are separate classes now and the
   former doesn't derive from the latter. If you need to convert a sorted array
   to a normal one, you must copy all the elements. Alternatively, you may
   avoid the use of wxSortedArrayString by using a normal array and calling its
   wxArrayString::Sort() method when needed.
 - `WX_DEFINE_ARRAY_INT(bool)` cannot be used because of the differences in
   `std::vector<bool>` specialization compared with the generic std::vector<>
   class. Please either use `std::vector<bool>` directly or use wxArrayInt
   instead.

*/
