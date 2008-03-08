/////////////////////////////////////////////////////////////////////////////
// Name:        object.h
// Purpose:     documentation for wxObjectRefData class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxObjectRefData
    @wxheader{object.h}

    This class is used to store reference-counted data. Derive classes from this to
    store your own data. When retrieving information from a @b wxObject's reference
    data,
    you will need to cast to your own derived class.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxObject, wxObjectDataPtrT, @ref overview_trefcount "Reference counting"
*/
class wxObjectRefData
{
public:
    /**
        Default constructor. Initialises the internal reference count to 1.
    */
    wxObjectRefData();

    /**
        Destructor. It's declared @c protected so that wxObjectRefData instances will
        never
        be destroyed directly but only as result of a DecRef() call.
    */
    wxObjectRefData();

    /**
        Decrements the reference count associated with this shared data and, if it
        reaches zero,
        destroys this instance of wxObjectRefData releasing its memory.
        
        Please note that after calling this function, the caller should absolutely
        avoid to use
        the pointer to this instance since it may not be valid anymore.
    */
    void DecRef();

    /**
        Returns the reference count associated with this shared data.
        When this goes to zero during a DecRef() call, the object
        will auto-free itself.
    */
    int GetRefCount();

    /**
        Increments the reference count associated with this shared data.
    */
    void IncRef();
};


/**
    @class wxObject
    @wxheader{object.h}

    This is the root class of many of the wxWidgets classes.
    It declares a virtual destructor which ensures that
    destructors get called for all derived class objects where necessary.

    wxObject is the hub of a dynamic object creation
    scheme, enabling a program to create instances of a class only knowing
    its string class name, and to query the class hierarchy.

    The class contains optional debugging versions
    of @b new and @b delete, which can help trace memory allocation
    and deallocation problems.

    wxObject can be used to implement @ref overview_trefcount "reference counted"
    objects,
    such as wxPen, wxBitmap and others (see @ref overview_refcountlist "this list").

    @library{wxbase}
    @category{rtti}

    @seealso
    wxClassInfo, @ref overview_debuggingoverview "Debugging overview",
    wxObjectRefData
*/
class wxObject
{
public:
    //@{
    /**
        Default and copy constructors.
    */
    wxObject();
    wxObject(const wxObject& other);
    //@}

    /**
        Destructor. Performs dereferencing, for those objects
        that use reference counting.
    */
    wxObject();

    /**
        A virtual function that may be redefined by derived classes to allow dumping of
        memory states.
        
        This function is only defined in debug build and doesn't exist at all if
        @c __WXDEBUG__ is not defined.
        
        @param stream
        Stream on which to output dump information.
        
        @remarks Currently wxWidgets does not define Dump for derived classes,
                   but programmers may wish to use it for their own
                   applications. Be sure to call the Dump member of the
                   class's base class to allow all information to be
                   dumped.
    */
    void Dump(ostream& stream);

    /**
        This virtual function is redefined for every class that requires run-time
        type information, when using DECLARE_CLASS macros.
    */
    wxClassInfo * GetClassInfo();

    /**
        Returns the @b m_refData pointer.
        
        @sa Ref(), UnRef(), wxObject::m_refData, SetRefData(),
              wxObjectRefData
    */
    wxObjectRefData* GetRefData();

    /**
        Determines whether this class is a subclass of (or the same class as)
        the given class.
        
        @param info
        A pointer to a class information object, which may be obtained
        by using the CLASSINFO macro.
        
        @returns @true if the class represented by info is the same class as this
                   one or is derived from it.
    */
    bool IsKindOf(wxClassInfo * info);

    /**
        Returns @true if this object has the same data pointer as @e obj. Notice
        that @true is returned if the data pointers are @NULL in both objects.
        
        This function only does a shallow comparison, i.e. it doesn't compare
        the objects pointed to by the data pointers of these objects.
    */
    bool IsSameAs(const wxObject& obj);

    /**
        Makes this object refer to the data in @e clone.
        
        @param clone
        The object to 'clone'.
        
        @remarks First this function calls UnRef() on itself to decrement
                   (and perhaps free) the data it is currently referring
                   to.
        
        @sa UnRef(), wxObject::m_refData, SetRefData(),
              GetRefData(), wxObjectRefData
    */
#define void Ref(const wxObject& clone)     /* implementation is private */

    /**
        Sets the @b m_refData pointer.
        
        @sa Ref(), UnRef(), wxObject::m_refData, GetRefData(),
              wxObjectRefData
    */
    void SetRefData(wxObjectRefData* data);

    /**
        Decrements the reference count in the associated data, and if it is zero,
        deletes the data.
        The @b m_refData member is set to @NULL.
        
        @sa Ref(), wxObject::m_refData, SetRefData(),
              GetRefData(), wxObjectRefData
    */
    void UnRef();

    /**
        Ensure that this object's data is not shared with any other object.
        
        if we have no
        data, it is created using CreateRefData() below, if we have shared data
        it is copied using CloneRefData(), otherwise nothing is done.
    */
    void UnShare();

    /**
        wxObjectRefData*  m_refData
        
        Pointer to an object which is the object's reference-counted data.
        
        @sa Ref(), UnRef(), SetRefData(),
              GetRefData(), wxObjectRefData
    */


    /**
        The @e delete operator is defined for debugging versions of the library only,
        when
        the identifier __WXDEBUG__ is defined. It takes over memory deallocation,
        allowing
        wxDebugContext operations.
    */
    void delete(void buf);

    /**
        The @e new operator is defined for debugging versions of the library only, when
        the identifier __WXDEBUG__ is defined. It takes over memory allocation, allowing
        wxDebugContext operations.
    */
    void * new(size_t size, const wxString& filename = @NULL,
               int lineNum = 0);
};


/**
    @class wxClassInfo
    @wxheader{object.h}

    This class stores meta-information about classes. Instances of this class are
    not generally defined directly by an application, but indirectly through use
    of macros such as @b DECLARE_DYNAMIC_CLASS and @b IMPLEMENT_DYNAMIC_CLASS.

    @library{wxbase}
    @category{rtti}

    @seealso
    Overview, wxObject
*/
class wxClassInfo
{
public:
    /**
        Constructs a wxClassInfo object. The supplied macros implicitly construct
        objects of this
        class, so there is no need to create such objects explicitly in an application.
    */
    wxClassInfo(const wxChar * className,
                const wxClassInfo * baseClass1,
                const wxClassInfo * baseClass2,
                int size, wxObjectConstructorFn fn);

    /**
        Creates an object of the appropriate kind. Returns @NULL if the class has not
        been declared
        dynamically creatable (typically, it is an abstract class).
    */
    wxObject* CreateObject();

    /**
        Finds the wxClassInfo object for a class of the given string name.
    */
    static wxClassInfo * FindClass(wxChar * name);

    /**
        Returns the name of the first base class (@NULL if none).
    */
    wxChar * GetBaseClassName1();

    /**
        Returns the name of the second base class (@NULL if none).
    */
    wxChar * GetBaseClassName2();

    /**
        Returns the string form of the class name.
    */
    wxChar * GetClassName();

    /**
        Returns the size of the class.
    */
    int GetSize();

    /**
        Initializes pointers in the wxClassInfo objects for fast execution
        of IsKindOf. Called in base wxWidgets library initialization.
    */
    static void InitializeClasses();

    /**
        Returns @true if this class info can create objects of the associated class.
    */
    bool IsDynamic();

    /**
        Returns @true if this class is a kind of (inherits from) the given class.
    */
    bool IsKindOf(wxClassInfo* info);
};


/**
    @class wxObjectDataPtrT
    @wxheader{object.h}

    This is helper template class primarily written to avoid memory
    leaks because of missing calls to wxObjectRefData::DecRef.

    Despite the name this template can actually be used as a
    smart pointer for any class implementing the reference
    counting interface which only consists of the two methods
    @b T::IncRef() and @b T::DecRef().

    The difference to wxSharedPtr is that
    wxObjectDataPtr relies on the reference counting to be in
    the class pointed to where as wxSharedPtr implements the
    reference counting itself.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxObject, wxObjectRefData, @ref overview_trefcount "Reference counting"
*/
class wxObjectDataPtr<T>
{
public:
    //@{
    /**
        This copy constructor increases the count of the reference
        counted object to which @e tocopy points and then this
        class will point to, as well.
    */
    wxObjectDataPtrT(T* ptr = @NULL);
    wxObjectDataPtrT(const wxObjectDataPtr<T>& tocopy);
    //@}

    /**
        Decreases the reference count of the object to which this
        class points.
    */
    ~wxObjectDataPtrT();

    /**
        Gets a pointer to the reference counted object to which
        this class points.
    */
    T* get();

    /**
        Conversion to a boolean expression (in a variant which is not
        convertable to anything but a boolean expression). If this class
        contains a valid pointer it will return @e @true, if it contains
        a @NULL pointer it will return @e @false.
    */
    operator unspecified_bool_type();

    /**
        Returns a reference to the object. If the internal pointer is @NULL
        this method will cause an assert in debug mode.
    */
    T operator*();

    /**
        Returns a pointer to the reference counted object to which
        this class points. If this the internal pointer is @NULL,
        this method will assert in debug mode.
    */
    T* operator-();

    //@{
    /**
        Assignment operators.
    */
    wxObjectDataPtrT& operator operator=(const wxObjectDataPtr<T>& tocopy);
    wxObjectDataPtrT& operator operator=(T* ptr);
    //@}
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    Used inside a class declaration to declare that the class should be
    made known to the class hierarchy, but objects of this class cannot be created
    dynamically. The same as DECLARE_ABSTRACT_CLASS.
*/
#define DECLARE_CLASS()     /* implementation is private */

/**
    Used inside a class declaration to declare that the class should be
    made known to the class hierarchy, but objects of this class cannot be created
    dynamically. The same as DECLARE_CLASS.

    Example:
    @code
    class wxCommand: public wxObject
    {
      DECLARE_ABSTRACT_CLASS(wxCommand)

     private:
      ...
     public:
      ...
    };
    @endcode
*/
#define DECLARE_ABSTRACT_CLASS()     /* implementation is private */

/**
    Returns a pointer to the wxClassInfo object associated with this class.
*/
#define wxClassInfo * CLASSINFO()     /* implementation is private */

/**
    Same as @c reinterpret_castT(x) if the compiler supports reinterpret cast or
    @c (T)x for old compilers.

    @sa wx_const_cast, wx_static_cast
*/
T wx_reinterpret_cast();

/**
    Used in a C++ implementation file to complete the declaration of a
    class that has run-time type information and two base classes. The
    same as IMPLEMENT_ABSTRACT_CLASS2.
*/
#define IMPLEMENT_CLASS2()     /* implementation is private */

/**
    This macro expands into @c const_castclassname *(ptr) if the compiler
    supports @e const_cast or into an old, C-style cast, otherwise.

    @sa wx_const_cast, wxDynamicCast, wxStaticCast
*/
classname * wxConstCast();

/**
    Used in a C++ implementation file to complete the declaration of
    a class that has run-time type information. The same as IMPLEMENT_CLASS.

    Example:
    @code
    IMPLEMENT_ABSTRACT_CLASS(wxCommand, wxObject)

    wxCommand::wxCommand(void)
    {
    ...
    }
    @endcode
*/
#define IMPLEMENT_ABSTRACT_CLASS()     /* implementation is private */

/**
    Used in a C++ implementation file to complete the declaration of
    a class that has run-time type information. The same as
    IMPLEMENT_ABSTRACT_CLASS.
*/
#define IMPLEMENT_CLASS()     /* implementation is private */

/**
    This macro is equivalent to @c wxDynamicCast(this, classname) but the
    latter provokes spurious compilation warnings from some compilers (because it
    tests whether @c this pointer is non-@NULL which is always @true), so
    this macro should be used to avoid them.

    @sa wxDynamicCast
*/
classname * wxDynamicCastThis();

/**
    Used in a C++ implementation file to complete the declaration of
    a class that has run-time type information, and whose instances
    can be created dynamically. Use this for classes derived from two
    base classes.
*/
#define IMPLEMENT_DYNAMIC_CLASS2()     /* implementation is private */

/**
    Creates and returns an object of the given class, if the class has been
    registered with the dynamic class system using DECLARE... and IMPLEMENT...
    macros.
*/
wxObject * wxCreateDynamicObject(const wxString& className);

/**
    Used inside a class declaration to make the class known to wxWidgets RTTI
    system and also declare that the objects of this class should be dynamically
    creatable from run-time type information. Notice that this implies that the
    class should have a default constructor, if this is not the case consider using
    DECLARE_CLASS.

    Example:
    @code
    class wxFrame: public wxWindow
    {
      DECLARE_DYNAMIC_CLASS(wxFrame)

     private:
      const wxString& frameTitle;
     public:
      ...
    };
    @endcode
*/
#define DECLARE_DYNAMIC_CLASS()     /* implementation is private */

/**
    Same as @c const_castT(x) if the compiler supports const cast or
    @c (T)x for old compilers. Unlike wxConstCast,
    the cast it to the type @e T and not to @c T * and also the order of
    arguments is the same as for the standard cast.

    @sa wx_reinterpret_cast, wx_static_cast
*/
T wx_const_cast();

/**
    Used in a C++ implementation file to complete the declaration of
    a class that has run-time type information and two base classes. The same as
    IMPLEMENT_CLASS2.
*/
#define IMPLEMENT_ABSTRACT_CLASS2()     /* implementation is private */

/**
    This macro returns the pointer @e ptr cast to the type @e classname * if
    the pointer is of this type (the check is done during the run-time) or
    @NULL otherwise. Usage of this macro is preferred over obsoleted
    wxObject::IsKindOf() function.

    The @e ptr argument may be @NULL, in which case @NULL will be
    returned.

    Example:
    @code
    wxWindow *win = wxWindow::FindFocus();
        wxTextCtrl *text = wxDynamicCast(win, wxTextCtrl);
        if ( text )
        {
            // a text control has the focus...
        }
        else
        {
            // no window has the focus or it is not a text control
        }
    @endcode

    @sa @ref overview_runtimeclassoverview "RTTI overview", wxDynamicCastThis,
          wxConstCast, wxStaticCast
*/
classname * wxDynamicCast();

/**
    This is defined in debug mode to be call the redefined new operator
    with filename and line number arguments. The definition is:
    @code
    #define WXDEBUG_NEW new(__FILE__,__LINE__)
    @endcode

    In non-debug mode, this is defined as the normal new operator.
*/
#define WXDEBUG_NEW()     /* implementation is private */

/**
    This macro checks that the cast is valid in debug mode (an assert failure will
    result if @c wxDynamicCast(ptr, classname) == @NULL) and then returns the
    result of executing an equivalent of @c static_castclassname *(ptr).

    @sa wx_static_cast, wxDynamicCast, wxConstCast
*/
classname * wxStaticCast();

/**
    Same as @c static_castT(x) if the compiler supports static cast or
    @c (T)x for old compilers. Unlike wxStaticCast,
    there are no checks being done and the meaning of the macro arguments is exactly
    the same as for the standard static cast, i.e. @e T is the full type name and
    star is not appended to it.

    @sa wx_const_cast, wx_reinterpret_cast, wx_truncate_cast
*/
T wx_static_cast();

/**
    Used in a C++ implementation file to complete the declaration of
    a class that has run-time type information, and whose instances
    can be created dynamically.

    Example:
    @code
    IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)

    wxFrame::wxFrame(void)
    {
    ...
    }
    @endcode
*/
#define IMPLEMENT_DYNAMIC_CLASS()     /* implementation is private */

