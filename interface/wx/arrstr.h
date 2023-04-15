/////////////////////////////////////////////////////////////////////////////
// Name:        arrstr.h
// Purpose:     interface of wxArrayString
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxArrayString

    wxArrayString is a legacy class similar to std::vector<wxString>.

    As all the other legacy @ref overview_container "container classes",
    this class shouldn't normally be used in the new code, but is still needed
    when passing multiple items to various functions in wxWidgets API, notably
    the constructors of various GUI control classes. Usually, even in this case
    it doesn't need to be used explicitly, as wxArrayString will be implicitly
    created if you use either an initializer list or a vector of strings, e.g.
    you can just pass either of those instead of wxArrayString, for example

    @code
        // wxListBox ctor is documented as taking wxArrayString but you can
        // pass an initializer_list to it directly:
        auto listbox = new wxListBox(parent, wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     { "some", "items", "for", "the", "listbox" });

        // Similarly, if you already have a vector filled with strings
        // elsewhere in your program, you can just pass it instead:
        std::vector<std::string> countries = GetListOfCountries();
        auto choices = new wxChoice(parent, wxID_ANY,
                                    wxDefaultPosition, wxDefaultSize,
                                    countries);
    @endcode

    When using a wxWidgets function returning an object of this class, you can
    either use it as if it were a `std::vector<wxString>`, as this class has
    all vector methods, or actually convert it to such vector using its
    AsVector(), e.g.

    @code
    wxArrayString files;
    wxDir::GetAllFiles("/some/path", &files);

    // Can use the usual accessors:
    if ( !files.empty() ) {
        auto first = files[0];
        auto total = files.size();
        ...
    }

    // Can iterate over it like over a vector, too.
    for ( const wxString& file: files ) {
        ...
    }

    // Or can just convert it to the "real" vector:
    const std::vector<wxString>& vec = files.AsVector();
    @endcode

    @library{wxbase}
    @category{containers}

    @see wxSortedArrayString, wxArray<T>, wxString, @ref overview_string
*/
class wxArrayString : public wxArray
{
public:
    /**
        The function type used with wxArrayString::Sort().

        This function uses the same conventions as the standard @c qsort()
        comparison function, that is it should return a negative value if the
        first argument is less than the second one, a positive value if the
        first argument is greater than the second one and 0 if the arguments
        are equal.

        @since 3.1.0
    */
    typedef int (*CompareFunction)(const wxString& first, const wxString& second);

    /**
        Default constructor.
    */
    wxArrayString();

    /**
        Copy constructor.
    */
    wxArrayString(const wxArrayString& array);

    ///@{
    /**
        Constructor from a C string array. Pass a size @a sz and an array @a arr.
    **/
    wxArrayString(size_t sz, const char** arr);
    wxArrayString(size_t sz, const wchar_t** arr);
    ///@}

    /**
        Constructor from a wxString array. Pass a size @a sz and array @a arr.
    */
    wxArrayString(size_t sz, const wxString* arr);

    /**
        Constructs the container with the contents of the initializer_list @a list.

        @since 3.2.3
    */
    template<typename T>
    wxArrayString(std::initializer_list<T> list);

    /**
        Constructs the container with the contents of the vector @a vec.

        Template parameter `T` must be convertible to wxString, i.e. it can be
        wxString itself or `std::string`, `std::wstring` etc.

        @since 3.3.0
    */
    template<typename T>
    wxArrayString(const std::vector<T>& vec);

    /**
        Constructs the container with the contents of the vector @a vec.

        In the default build, in which wxArrayString is implemented using
        `std::vector<>` internally, this constructor is more efficient than the
        overload taking const reference to the vector, as it reuses the
        existing vector data instead of copying it.
        Otherwise it is identical to the other overload, see its documentation
        for more details.

        @since 3.3.0
    */
    template<typename T>
    wxArrayString(std::vector<T>&& vec);

    /**
        Destructor frees memory occupied by the array strings. For performance
        reasons it is not virtual, so this class should not be derived from.
    */
    ~wxArrayString();

    /**
        Appends the given number of @a copies of the new item @a str to the
        array and returns the index of the first new item in the array.

        @see Insert()
    */
    size_t Add(const wxString& str, size_t copies = 1);

    /**
        Preallocates enough memory to store @a nCount items.

        This function may be used to improve array class performance before
        adding a known number of items consecutively.
    */
    void Alloc(size_t nCount);

    /**
        Constructs a std::vector containing the same strings as this array.

        In the default build configuration, this function returns a const
        reference to this object itself, without making a copy. But when using
        the legacy implementation of wxArrayString not based on `std::vector`,
        it has to copy all the strings, making it expensive to call for big
        arrays.

        Note that using it like this:
        @code
        const std::vector<wxString>& vec = array.AsVector();
        @endcode
        works in all build variants as long as you don't need to modify the
        returned vector and doesn't impose any extra overhead.

        @since 3.3.0
    */
    std::vector<wxString> AsVector() const;

    /**
        Clears the array contents and frees memory.

        @see Empty()
    */
    void Clear();

    /**
        Empties the array: after a call to this function GetCount() will return 0.
        However, this function does not free the memory used by the array and so
        should be used when the array is going to be reused for storing other strings.
        Otherwise, you should use Clear() to empty the array and free memory.
    */
    void Empty();

    /**
        Returns the number of items in the array.
    */
    size_t GetCount() const;

    /**
        Searches the array for @a str, starting from the beginning if @a bFromEnd
        is @false or from the end otherwise. If @a bCase, comparison is case sensitive
        (default), otherwise the case is ignored.

        This function uses linear search for wxArrayString.
        Returns the index of the first item matched or @c wxNOT_FOUND if there is no match.
    */
    int Index(const wxString& str, bool bCase = true, bool bFromEnd = false) const;

    /**
        Inserts the given number of @a copies of @a str in the array before the
        array element at the position @a nIndex. Thus, for example, to insert
        the string in the beginning of the array you would write:

        @code
        Insert("foo", 0);
        @endcode

        If @a nIndex is equal to GetCount() this function behaves as Add().
    */
    void Insert(const wxString& str, size_t nIndex, size_t copies = 1);

    /**
        Returns @true if the array is empty, @false otherwise. This function returns the
        same result as GetCount() == 0 but is probably easier to read.
    */
    bool IsEmpty() const;

    /**
        Return the array element at position @a nIndex. An assert failure will
        result from an attempt to access an element beyond the end of array in debug
        mode, but no check is done in release mode.

        @see operator[] for the operator version.
    */
    ///@{
    wxString& Item(size_t nIndex);
    const wxString& Item(size_t nIndex) const;
    ///@}

    /**
        Returns the last element of the array. Attempt to access the last element of
        an empty array will result in assert failure in debug build, however no checks
        are done in release mode.
    */
    ///@{
    wxString& Last();
    const wxString& Last() const;
    ///@}

    /**
        Removes the first item matching this value. An assert failure is provoked by
        an attempt to remove an element which does not exist in debug build.

        @see Index()
    */
    void Remove(const wxString& sz);

    /**
        Removes @a count items starting at position @a nIndex from the array.
    */
    void RemoveAt(size_t nIndex, size_t count = 1);

    /**
        Releases the extra memory allocated by the array.
        This function is useful to minimize the array memory consumption.

        @see Alloc()
    */
    void Shrink();

    /**
        Sorts the array in alphabetical order or in reverse alphabetical order if
        @a reverseOrder is @true. The sort is case-sensitive.
    */
    void Sort(bool reverseOrder = false);

    /**
        Sorts the array using the specified @a compareFunction for item comparison.
        @a CompareFunction is defined as a function taking two <em>const wxString&</em>
        parameters and returning an @e int value less than, equal to or greater
        than 0 if the first string is less than, equal to or greater than the
        second one.

        Example:
        The following example sorts strings by their length.

        @code
        static int CompareStringLen(const wxString& first, const wxString& second)
        {
            return first.length() - second.length();
        }

        ...

        wxArrayString array;

        array.Add("one");
        array.Add("two");
        array.Add("three");
        array.Add("four");

        array.Sort(CompareStringLen);
        @endcode
    */
    void Sort(CompareFunction compareFunction);

    /**
        Compares 2 arrays respecting the case. Returns @true if the arrays have
        different number of elements or if the elements don't match pairwise.
    */
    bool operator !=(const wxArrayString& array) const;

    /**
        Assignment operator.
    */
    wxArrayString& operator=(const wxArrayString&);

    /**
        Compares 2 arrays respecting the case. Returns @true only if the arrays have
        the same number of elements and the same strings in the same order.
    */
    bool operator ==(const wxArrayString& array) const;

    /**
        Returns the array element at position @a nIndex. An assert failure will
        result from an attempt to access an element beyond the end of array in
        debug mode, but no check is done in release mode.

        This is the operator version of the Item() method.
    */
    wxString& operator[](size_t nIndex) const;
};


/**
    @class wxSortedArrayString

    wxSortedArrayString is an efficient container for storing wxString objects
    which always keeps the strings in alphabetical order.

    wxSortedArrayString uses binary search in its wxSortedArrayString::Index() method
    (instead of linear search for wxArrayString::Index()) which makes it much more
    efficient if you add strings to the array rarely (because, of course, you have
    to pay for Index() efficiency by having Add() be slower) but search for them
    often. Several methods should not be used with sorted array (basically, all
    those which break the order of items) which is mentioned in their description.

    @library{wxbase}
    @category{containers}

    @see wxArray, wxString, @ref overview_string
*/
class wxSortedArrayString : public wxArray
{
public:
    /**
        Default constructor.

        The elements of the array are kept sorted in alphabetical order.
     */
    wxSortedArrayString();

    /**
        Constructs a sorted array using the specified @a compareFunction for
        item comparison.

        @see wxStringSortAscending(), wxDictionaryStringSortAscending()

        @since 3.1.0
    */
    wxSortedArrayString(CompareFunction compareFunction);

    /**
        Conversion constructor.

        Constructs a sorted array with the same contents as the (possibly
        unsorted) @a array argument.
    */
    wxSortedArrayString(const wxArrayString& array);

    /**
        @copydoc wxArrayString::Add()

        @warning
        For sorted arrays, the index of the inserted item will not be, in general,
        equal to GetCount() - 1 because the item is inserted at the correct position
        to keep the array sorted and not appended.
    */
    size_t Add(const wxString& str, size_t copies = 1);


    /**
        @copydoc wxArrayString::Index()

        This function uses binary search for wxSortedArrayString, but it ignores
        the @a bCase and @a bFromEnd parameters.
    */
    int Index(const wxString& str, bool bCase = true,
              bool bFromEnd = false) const;

    /**
        @warning This function should not be used with sorted arrays because it
                 could break the order of items and, for example, subsequent calls
                 to Index() would then not work!

        @warning In STL mode, Insert is private and simply invokes wxFAIL_MSG.
    */
    void Insert(const wxString& str, size_t nIndex,
                size_t copies = 1);

    ///@{
    /**
        @warning This function should not be used with sorted array because it could
                 break the order of items and, for example, subsequent calls to Index()
                 would then not work! Also, sorting a wxSortedArrayString doesn't make
                 sense because its elements are always already sorted.

        @warning In STL mode, Sort is private and simply invokes wxFAIL_MSG.
    */
    void Sort(bool reverseOrder = false);
    void Sort(CompareFunction compareFunction);
    ///@}
};

/**
    Comparison function comparing strings in alphabetical order.

    This function can be used with wxSortedArrayString::Sort() or passed as an
    argument to wxSortedArrayString constructor.

    @see wxStringSortDescending(), wxDictionaryStringSortAscending(),
         wxNaturalStringSortAscending()

    @since 3.1.0
 */
int wxStringSortAscending(const wxString& s1, const wxString& s2);

/**
    Comparison function comparing strings in reverse alphabetical order.

    This function can be used with wxSortedArrayString::Sort() or passed as an
    argument to wxSortedArrayString constructor.

    @see wxStringSortAscending(), wxDictionaryStringSortDescending(),
         wxNaturalStringSortDescending()

    @since 3.1.0
 */
int wxStringSortDescending(const wxString& s1, const wxString& s2);

/**
    Comparison function comparing strings in dictionary order.

    The "dictionary order" differs from the alphabetical order in that the
    strings differing not only in case are compared case-insensitively to
    ensure that "Aa" comes before "AB" in the sorted array, unlike with
    wxStringSortAscending().

    This function can be used with wxSortedArrayString::Sort() or passed as an
    argument to wxSortedArrayString constructor.

    @see wxDictionaryStringSortDescending(),
         wxStringSortAscending(),
         wxNaturalStringSortAscending()

    @since 3.1.0
 */
int wxDictionaryStringSortAscending(const wxString& s1, const wxString& s2);

/**
    Comparison function comparing strings in reverse dictionary order.

    See wxDictionaryStringSortAscending() for the dictionary sort description.

    @see wxDictionaryStringSortAscending(),
         wxStringSortDescending(),
         wxNaturalStringSortDescending()

    @since 3.1.0
 */
int wxDictionaryStringSortDescending(const wxString& s1, const wxString& s2);


/**
    Comparison function comparing strings in natural order.

    This function can be used with wxSortedArrayString::Sort()
    or passed as an argument to wxSortedArrayString constructor.

    See wxCmpNatural() for more information about how natural
    sort order is implemented.

    @see wxNaturalStringSortDescending(),
         wxStringSortAscending(), wxDictionaryStringSortAscending()

    @since 3.1.4
*/
int wxNaturalStringSortAscending(const wxString& s1, const wxString& s2);

/**
    Comparison function comparing strings in reverse natural order.

    This function can be used with wxSortedArrayString::Sort()
    or passed as an argument to wxSortedArrayString constructor.

    See wxCmpNatural() for more information about how natural
    sort order is implemented.

    @see wxNaturalStringSortAscending(),
         wxStringSortDescending(), wxDictionaryStringSortDescending()

    @since 3.1.4
*/
int wxNaturalStringSortDescending(const wxString& s1, const wxString& s2);

/**
    This function compares strings using case-insensitive collation and
    additionally, numbers within strings are recognised and compared
    numerically, rather than alphabetically. When used for sorting,
    the result is that e.g. file names containing numbers are sorted
    in a natural way.

    For example, sorting with a simple string comparison results in:
    - file1.txt
    - file10.txt
    - file100.txt
    - file2.txt
    - file20.txt
    - file3.txt

    But sorting the same strings in natural sort order results in:
    - file1.txt
    - file2.txt
    - file3.txt
    - file10.txt
    - file20.txt
    - file100.txt

    wxCmpNatural() uses an OS native natural sort function when available
    (currently only under Microsoft Windows), wxCmpNaturalGeneric() otherwise.

    Be aware that OS native implementations might differ from each other,
    and might change behaviour from release to release.

    @see wxNaturalStringSortAscending(), wxNaturalStringSortDescending()

    @since 3.1.4
*/
int wxCmpNatural(const wxString& s1, const wxString& s2);

/**
    This is wxWidgets' own implementation of the natural sort comparison function.

    @see wxCmpNatural()

    @since 3.1.4
*/
int wxCmpNaturalGeneric(const wxString& s1, const wxString& s2);


// ============================================================================
// Global functions/macros
// ============================================================================

/** @addtogroup group_funcmacro_string */
///@{

/**
    Splits the given wxString object using the separator @a sep and returns the
    result as a wxArrayString.

    If the @a escape character is non-null, then the occurrences of @a sep
    immediately prefixed with @a escape are not considered as separators.
    Note that empty tokens will be generated if there are two or more adjacent
    separators.

    @see wxJoin()

    @header{wx/arrstr.h}
*/
wxArrayString wxSplit(const wxString& str, const wxChar sep,
                      const wxChar escape = '\\');

/**
    Concatenate all lines of the given wxArrayString object using the separator
    @a sep and returns the result as a wxString.

    If the @a escape character is non-null, then it's used as prefix for each
    occurrence of @a sep in the strings contained in @a arr before joining them
    which is necessary in order to be able to recover the original array
    contents from the string later using wxSplit(). The @a escape characters
    themselves are @e not escaped when they occur in the middle of the @a arr
    elements, but @e are escaped when they occur at the end, i.e.
    @code
    wxArrayString arr;
    arr.push_back("foo^");
    arr.push_back("bar^baz");
    wxPuts(wxJoin(arr, ':', '^')); // prints "foo^^:bar^baz"
    @endcode

    In any case, applying wxSplit() to the result of wxJoin() is guaranteed to
    recover the original array.

    @see wxSplit()

    @header{wx/arrstr.h}
*/
wxString wxJoin(const wxArrayString& arr, const wxChar sep,
                const wxChar escape = '\\');

///@}

