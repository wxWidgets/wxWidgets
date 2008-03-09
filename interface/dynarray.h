/////////////////////////////////////////////////////////////////////////////
// Name:        dynarray.h
// Purpose:     documentation for wxArray<T> class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxArrayT
    @wxheader{dynarray.h}

    This section describes the so called @e dynamic arrays. This is a C
    array-like type safe data structure i.e. the member access time is constant
    (and not
    linear according to the number of container elements as for linked lists).
    However, these
    arrays are dynamic in the sense that they will automatically allocate more
    memory if there is not enough of it for adding a new element. They also perform
    range checking on the index values but in debug mode only, so please be sure to
    compile your application in debug mode to use it (see @ref
    overview_debuggingoverview "debugging overview" for
    details). So, unlike the arrays in some other
    languages, attempt to access an element beyond the arrays bound doesn't
    automatically expand the array but provokes an assertion failure instead in
    debug build and does nothing (except possibly crashing your program) in the
    release build.

    The array classes were designed to be reasonably efficient, both in terms of
    run-time speed and memory consumption and the executable size. The speed of
    array item access is, of course, constant (independent of the number of
    elements)
    making them much more efficient than linked lists (wxList).
    Adding items to the arrays is also implemented in more or less constant time -
    but the price is preallocating the memory in advance. In the @ref
    wxArray::memorymanagement "memory management" section
    you may find some useful hints about optimizing wxArray memory usage. As for
    executable size, all
    wxArray functions are inline, so they do not take @e any space at all.

    wxWidgets has three different kinds of array. All of them derive from
    wxBaseArray class which works with untyped data and can not be used directly.
    The standard macros WX_DEFINE_ARRAY(), WX_DEFINE_SORTED_ARRAY() and
    WX_DEFINE_OBJARRAY() are used to define a new class deriving from it. The
    classes declared will be called in this documentation wxArray, wxSortedArray and
    wxObjArray but you should keep in mind that no classes with such names actually
    exist, each time you use one of WX_DEFINE_XXXARRAY macro you define a class
    with a new name. In fact, these names are "template" names and each usage of one
    of the macros mentioned above creates a template specialization for the given
    element type.

    wxArray is suitable for storing integer types and pointers which it does not
    treat as objects in any way, i.e. the element pointed to by the pointer is not
    deleted when the element is removed from the array. It should be noted that
    all of wxArray's functions are inline, so it costs strictly nothing to define as
    many array types as you want (either in terms of the executable size or the
    speed) as long as at least one of them is defined and this is always the case
    because wxArrays are used by wxWidgets internally. This class has one serious
    limitation: it can only be used for storing integral types (bool, char, short,
    int, long and their unsigned variants) or pointers (of any kind). An attempt
    to use with objects of sizeof() greater than sizeof(long) will provoke a
    runtime assertion failure, however declaring a wxArray of floats will not (on
    the machines where sizeof(float) = sizeof(long)), yet it will @b not work,
    please use wxObjArray for storing floats and doubles.

    wxSortedArray is a wxArray variant which should be used when searching in the
    array is a frequently used operation. It requires you to define an additional
    function for comparing two elements of the array element type and always stores
    its items in the sorted order (according to this function). Thus, it is
     wxArray::Index function execution time is O(log(N)) instead of
    O(N) for the usual arrays but the wxArray::Add method is
    slower: it is O(log(N)) instead of constant time (neglecting time spent in
    memory allocation routine). However, in a usual situation elements are added to
    an array much less often than searched inside it, so wxSortedArray may lead to
    huge performance improvements compared to wxArray. Finally, it should be
    noticed that, as wxArray, wxSortedArray can be only used for storing integral
    types or pointers.

    wxObjArray class treats its elements like "objects". It may delete them when
    they are removed from the array (invoking the correct destructor) and copies
    them using the objects copy constructor. In order to implement this behaviour
    the definition of the wxObjArray arrays is split in two parts: first, you should
    declare the new wxObjArray class using WX_DECLARE_OBJARRAY() macro and then
    you must include the file defining the implementation of template type:
    wx/arrimpl.cpp and define the array class with WX_DEFINE_OBJARRAY() macro
    from a point where the full (as opposed to 'forward') declaration of the array
    elements class is in scope. As it probably sounds very complicated here is an
    example:

    @code
    #include wx/dynarray.h

    // we must forward declare the array because it is used inside the class
    // declaration
    class MyDirectory;
    class MyFile;

    // this defines two new types: ArrayOfDirectories and ArrayOfFiles which can be
    // now used as shown below
    WX_DECLARE_OBJARRAY(MyDirectory, ArrayOfDirectories);
    WX_DECLARE_OBJARRAY(MyFile,      ArrayOfFiles);

    class MyDirectory
    {
    ...
        ArrayOfDirectories m_subdirectories; // all subdirectories
        ArrayOfFiles       m_files;          // all files in this directory
    };

    ...

    // now that we have MyDirectory declaration in scope we may finish the
    // definition of ArrayOfDirectories -- note that this expands into some C++
    // code and so should only be compiled once (i.e., don't put this in the
    // header, but into a source file or you will get linking errors)
    #include wx/arrimpl.cpp // this is a magic incantation which must be done!
    WX_DEFINE_OBJARRAY(ArrayOfDirectories);

    // that's all!
    @endcode

    It is not as elegant as writing

    @code
    typedef std::vectorMyDirectory ArrayOfDirectories;
    @endcode

    but is not that complicated and allows the code to be compiled with any, however
    dumb, C++ compiler in the world.

    Remember to include wx/arrimpl.cpp just before each WX_DEFINE_OBJARRAY
    ocurrence in your code, even if you have several in the same file.

    Things are much simpler for wxArray and wxSortedArray however: it is enough
    just to write

    @code
    WX_DEFINE_ARRAY_INT(int, ArrayOfInts);
    WX_DEFINE_SORTED_ARRAY_INT(int, ArrayOfSortedInts);
    @endcode

    i.e. there is only one @c DEFINE macro and no need for separate
    @c DECLARE one. For the arrays of the primitive types, the macros
    @c WX_DEFINE_ARRAY_CHAR/SHORT/INT/SIZE_T/LONG/DOUBLE should be used
    depending on the sizeof of the values (notice that storing values of smaller
    type, e.g. shorts, in an array of larger one, e.g. @c ARRAY_INT, does
    not work on all architectures!).

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxcontaineroverview, wxListT, wxVectorT
*/
class wxArray<T>
{
public:
    //@{
    /**
        Appends the given number of @a copies of the @a item to the array
        consisting of the elements of type @e T.
        The first version is used with wxArray. The second is used with wxSortedArray,
        returning the index where @a item is stored. The third and the
        fourth ones are used with wxObjArray. There is an important difference between
        them: if you give a pointer to the array, it will take ownership of it, i.e.
        will delete it when the item is deleted from the array. If you give a reference
        to the array, however, the array will make a copy of the item and will not take
        ownership of the original item. Once again, it only makes sense for wxObjArrays
        because the other array types never take ownership of their elements. Also note
        that you cannot append more than one pointer as reusing it would lead to
        deleting it twice (or more) and hence to a crash.
        You may also use WX_APPEND_ARRAY macro to append all
        elements of one array to another one but it is more efficient to use
        @a copies parameter and modify the elements in place later if you plan to
        append a lot of items.
    */
    void Add(T item, size_t copies = 1);
    size_t Add(T item);
    void Add(T* item);
    void Add(T& item, size_t copies = 1);
    //@}

    /**
        Inserts the given @a item into the array in the specified @e index
         position.
        Be aware that you will set out the order of the array if you give a wrong
         position.
        This function is useful in conjunction with
        wxArray::IndexForInsert for a common operation
        of "insert only if not found".
    */
    void AddAt(T item, size_t index);

    /**
        wxArray::Add
        
        wxArray::AddAt
        
        wxArray::Insert
        
        wxArray::SetCount
        
        WX_APPEND_ARRAY
        
        WX_PREPEND_ARRAY
    */


    /**
        Preallocates memory for a given number of array elements. It is worth calling
        when the number of items which are going to be added to the array is known in
        advance because it will save unneeded memory reallocation. If the array already
        has enough memory for the given number of items, nothing happens. In any case,
        the existing contents of the array is not modified.
    */
    void Alloc(size_t count);

    /**
        This function does the same as wxArray::Empty and additionally
        frees the memory allocated to the array.
    */
    void Clear();

    /**
        Array classes are 100% C++ objects and as such they have the appropriate copy
        constructors and assignment operators. Copying wxArray just copies the elements
        but copying wxObjArray copies the arrays items. However, for memory-efficiency
        sake, neither of these classes has virtual destructor. It is not very important
        for wxArray which has trivial destructor anyhow, but it does mean that you
        should avoid deleting wxObjArray through a wxBaseArray pointer (as you would
        never use wxBaseArray anyhow it shouldn't be a problem) and that you should not
        derive your own classes from the array classes.
        @ref wxArray::ctordef "wxArray default constructor"
        
        @ref wxArray::ctorcopy "wxArray copy constructors and assignment operators"
        
        @ref wxArray::dtor ~wxArray
    */


    //@{
    /**
        (T first, T second)@e compareFunction)
        There is no default constructor for wxSortedArray classes - you must initialize
        it
        with a function to use for item comparison. It is a function which is passed
        two arguments of type @e T where @e T is the array element type and which
        should return a negative, zero or positive value according to whether the first
        element passed to it is less than, equal to or greater than the second one.
    */
    wxArray();
    wxObjArray();
    wxSortedArray();
    //@}

    /**
        Removes the element from the array, but, unlike,
        wxArray::Remove doesn't delete it. The function returns the
        pointer to the removed element.
    */
    T* Detach(size_t index);

    /**
        Empties the array. For wxObjArray classes, this destroys all of the array
        elements. For wxArray and wxSortedArray this does nothing except marking the
        array of being empty - this function does not free the allocated memory, use
        wxArray::Clear for this.
    */
    void Empty();

    /**
        Return the number of items in the array.
    */
    size_t GetCount();

    //@{
    /**
        The first version of the function is for wxArray and wxObjArray, the second is
        for wxSortedArray only.
        Searches the element in the array, starting from either beginning or the end
        depending on the value of @a searchFromEnd parameter. @c wxNOT_FOUND is
        returned if the element is not found, otherwise the index of the element is
        returned.
        Linear search is used for the wxArray and wxObjArray classes but binary search
        in the sorted array is used for wxSortedArray (this is why searchFromEnd
        parameter doesn't make sense for it).
        @b NB: even for wxObjArray classes, the operator==() of the elements in the
        array is @b not used by this function. It searches exactly the given
        element in the array and so will only succeed if this element had been
        previously added to the array, but fail even if another, identical, element is
        in the array.
    */
    int Index(T& item, bool searchFromEnd = false);
    int Index(T& item);
    //@}

    /**
        Search for a place to insert @a item into the sorted array (binary search).
        The index returned is just before the first existing item that is greater or
        equal
        (according to the compare function) to the given @e item.
        You have to do extra work to know if the @a item already exists in array.
        This function is useful in conjunction with
        wxArray::AddAt for a common operation
        of "insert only if not found".
    */
    size_t IndexForInsert(T item);

    //@{
    /**
        Insert the given number of @a copies of the @a item into the array before
        the existing item @a n - thus, @e Insert(something, 0u) will insert an
        item in such way that it will become the first array element.
        wxSortedArray doesn't have this function because inserting in wrong place
        would break its sorted condition.
        Please see wxArray::Add for explanation of the differences
        between the overloaded versions of this function.
    */
    void Insert(T item, size_t n, size_t copies = 1);
    void Insert(T* item, size_t n);
    void Insert(T& item, size_t n, size_t copies = 1);
    //@}

    /**
        Returns @true if the array is empty, @false otherwise.
    */
    bool IsEmpty();

    /**
        Returns the item at the given position in the array. If @a index is out of
        bounds, an assert failure is raised in the debug builds but nothing special is
        done in the release build.
        The returned value is of type "reference to the array element type" for all of
        the array classes.
    */
    T Item(size_t index);

    /**
        Returns the last element in the array, i.e. is the same as Item(GetCount() - 1).
        An assert failure is raised in the debug mode if the array is empty.
        The returned value is of type "reference to the array element type" for all of
        the array classes.
    */
    T Last();

    /**
        To use an array you must first define the array class. This is done with the
        help of the macros in this section. The class of array elements must be (at
        least) forward declared for WX_DEFINE_ARRAY, WX_DEFINE_SORTED_ARRAY and
        WX_DECLARE_OBJARRAY macros and must be fully declared before you use
        WX_DEFINE_OBJARRAY macro.
        WX_DEFINE_ARRAY
        
        WX_DEFINE_EXPORTED_ARRAY
        
        WX_DEFINE_USER_EXPORTED_ARRAY
        
        WX_DEFINE_SORTED_ARRAY
        
        WX_DEFINE_SORTED_EXPORTED_ARRAY
        
        WX_DEFINE_SORTED_USER_EXPORTED_ARRAY
        
        WX_DECLARE_EXPORTED_OBJARRAY
        
        WX_DECLARE_USER_EXPORTED_OBJARRAY
        
        WX_DEFINE_OBJARRAY
        
        WX_DEFINE_EXPORTED_OBJARRAY
        
        WX_DEFINE_USER_EXPORTED_OBJARRAY
        To slightly complicate the matters even further, the operator - defined by
        default for the array iterators by these macros only makes sense if the array
        element type is not a pointer itself and, although it still works, this
        provokes warnings from some compilers and to avoid them you should use the
        @c _PTR versions of the macros above. For example, to define an array of
        pointers to @c double you should use:
        
        Note that the above macros are generally only useful for
        wxObject types.  There are separate macros for declaring an array of a simple
        type,
        such as an int.
        The following simple types are supported:
        
        int
        
        long
        
        size_t
        
        double
        To create an array of a simple type, simply append the type you want in CAPS to
        the array definition.
        For example, for an integer array, you'd use one of the following variants:
        WX_DEFINE_ARRAY_INT
        
        WX_DEFINE_EXPORTED_ARRAY_INT
        
        WX_DEFINE_USER_EXPORTED_ARRAY_INT
        
        WX_DEFINE_SORTED_ARRAY_INT
        
        WX_DEFINE_SORTED_EXPORTED_ARRAY_INT
        
        WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_INT
    */


    /**
        Automatic array memory management is quite trivial: the array starts by
        preallocating some minimal amount of memory (defined by
        WX_ARRAY_DEFAULT_INITIAL_SIZE) and when further new items exhaust already
        allocated memory it reallocates it adding 50% of the currently allocated
        amount, but no more than some maximal number which is defined by
        ARRAY_MAXSIZE_INCREMENT constant. Of course, this may lead to some memory
        being wasted (ARRAY_MAXSIZE_INCREMENT in the worst case, i.e. 4Kb in the
        current implementation), so the wxArray::Shrink function is
        provided to deallocate the extra memory. The wxArray::Alloc
        function can also be quite useful if you know in advance how many items you are
        going to put in the array and will prevent the array code from reallocating the
        memory more times than needed.
        wxArray::Alloc
        
        wxArray::Shrink
    */


    /**
        Functions in this section return the total number of array elements and allow to
        retrieve them - possibly using just the C array indexing [] operator which
        does exactly the same as wxArray::Item method.
        wxArray::GetCount
        
        wxArray::IsEmpty
        
        wxArray::Item
        
        wxArray::Last
    */


    /**
        Removes an element from the array by value: the first item of the
        array equal to @a item is removed, an assert failure will result from an
        attempt to remove an item which doesn't exist in the array.
        When an element is removed from wxObjArray it is deleted by the array - use
        Detach if you don't want this to happen. On the
        other hand, when an object is removed from a wxArray nothing happens - you
        should delete it manually if required:
        
        See also WX_CLEAR_ARRAY macro which deletes all
        elements of a wxArray (supposed to contain pointers).
    */
    Remove(T item);

    /**
        Removes @a count elements starting at @a index from the array. When an
        element is removed from wxObjArray it is deleted by the array - use
        Detach if you don't want this to happen. On
        the other hand, when an object is removed from a wxArray nothing happens -
        you should delete it manually if required:
        
        See also WX_CLEAR_ARRAY macro which deletes all
        elements of a wxArray (supposed to contain pointers).
    */
    RemoveAt(size_t index, size_t count = 1);

    /**
        WX_CLEAR_ARRAY
        
        wxArray::Empty
        
        wxArray::Clear
        
        wxArray::RemoveAt
        
        wxArray::Remove
    */


    /**
        wxArray::Index
        
        wxArray::IndexForInsert
        
        wxArray::Sort
    */


    /**
        )
        This function ensures that the number of array elements is at least
        @e count. If the array has already @a count or more items, nothing is
        done. Otherwise, @c count - GetCount() elements are added and initialized to
        the value @e defval.
        
        @see wxArray::GetCount
    */
    void SetCount(size_t count);

    /**
        Frees all memory unused by the array. If the program knows that no new items
        will be added to the array it may call Shrink() to reduce its memory usage.
        However, if a new item is added to the array, some extra memory will be
        allocated again.
    */
    void Shrink();

    /**
        The notation CMPFUNCT should be read as if we had the following declaration:
        
        where @e T is the type of the array elements. I.e. it is a function returning
        @e int which is passed two arguments of type @e T *.
        Sorts the array using the specified compare function: this function should
        return a negative, zero or positive value according to whether the first element
        passed to it is less than, equal to or greater than the second one.
        wxSortedArray doesn't have this function because it is always sorted.
    */
    void Sort(CMPFUNC<T> compareFunction);

    /**
        This macro may be used to append all elements of the @a other array to the
        @e array. The two arrays must be of the same type.
    */
#define void WX_APPEND_ARRAY(wxArray& array, wxArray& other)     /* implementation is private */

    /**
        This macro may be used to delete all elements of the array before emptying it.
        It can not be used with wxObjArrays - but they will delete their elements anyhow
        when you call Empty().
    */
#define void WX_CLEAR_ARRAY(wxArray& array)     /* implementation is private */

    //@{
    /**
        This macro declares a new object array class named @a name and containing
        the elements of type @e T. The second form is used when compiling wxWidgets as
        a DLL under Windows and array needs to be visible outside the DLL.  The third is
        needed for exporting an array from a user DLL.
        Example:
        
        You must use WX_DEFINE_OBJARRAY macro to define
        the array class - otherwise you would get link errors.
    */
    WX_DECLARE_OBJARRAY(T, name);
    WX_DECLARE_EXPORTED_OBJARRAY(T, name);
    WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name);
    //@}

    //@{
    /**
        This macro defines a new array class named @a name and containing the
        elements of type @e T. The second form is used when compiling wxWidgets as
        a DLL under Windows and array needs to be visible outside the DLL.  The third is
        needed for exporting an array from a user DLL.
        Example:
        
        Note that wxWidgets predefines the following standard array classes: @b
        wxArrayInt,
        @b wxArrayLong, @b wxArrayShort, @b wxArrayDouble,  @b wxArrayPtrVoid.
    */
    WX_DEFINE_ARRAY(T, name);
    WX_DEFINE_EXPORTED_ARRAY(T, name);
    WX_DEFINE_USER_EXPORTED_ARRAY(T, name, exportspec);
    //@}

    //@{
    /**
        This macro defines the methods of the array class @a name not defined by the
        WX_DECLARE_OBJARRAY macro. You must include the
        file wx/arrimpl.cpp before using this macro and you must have the full
        declaration of the class of array elements in scope! If you forget to do the
        first, the error will be caught by the compiler, but, unfortunately, many
        compilers will not give any warnings if you forget to do the second - but the
        objects of the class will not be copied correctly and their real destructor will
        not be called.  The latter two forms are merely aliases of the first to satisfy
        some people's sense of symmetry when using the exported declarations.
        Example of usage:
    */
    WX_DEFINE_OBJARRAY(name);
    WX_DEFINE_EXPORTED_OBJARRAY(name);
    WX_DEFINE_USER_EXPORTED_OBJARRAY(name);
    //@}

    //@{
    /**
        This macro defines a new sorted array class named @a name and containing
        the elements of type @e T. The second form is used when compiling wxWidgets as
        a DLL under Windows and array needs to be visible outside the DLL.  The third is
        needed for exporting an array from a user DLL.
        Example:
        
        You will have to initialize the objects of this class by passing a comparison
        function to the array object constructor like this:
    */
    WX_DEFINE_SORTED_ARRAY(T, name);
    WX_DEFINE_SORTED_EXPORTED_ARRAY(T, name);
    WX_DEFINE_SORTED_USER_EXPORTED_ARRAY(T, name);
    //@}

    /**
        This macro may be used to prepend all elements of the @a other array to the
        @e array. The two arrays must be of the same type.
    */
#define void WX_PREPEND_ARRAY(wxArray& array, wxArray& other)     /* implementation is private */

    //@{
    /**
        The copy constructors and assignment operators perform a shallow array copy
        (i.e. they don't copy the objects pointed to even if the source array contains
        the items of pointer type) for wxArray and wxSortedArray and a deep copy (i.e.
        the array element are copied too) for wxObjArray.
    */
    wxArray(const wxArray& array);
    wxSortedArray(const wxSortedArray& array);
    wxObjArray(const wxObjArray& array);
    wxArray operator=(const wxArray& array);
    wxSortedArray operator=(const wxSortedArray& array);
    wxObjArray operator=(const wxObjArray& array);
    //@}

    //@{
    /**
        The wxObjArray destructor deletes all the items owned by the array. This is not
        done by wxArray and wxSortedArray versions - you may use
        WX_CLEAR_ARRAY macro for this.
    */
    ~wxArray();
    ~wxSortedArray();
    ~wxObjArray();
    //@}
};
