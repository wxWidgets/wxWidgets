/////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/automtn.h
// Purpose:     interface of wxAutomationObject
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Automation object creation flags.

    These flags can be used with wxAutomationObject::GetInstance().

    @since 2.9.2
*/
enum wxAutomationInstanceFlags
{
    /**
        Only use the existing instance, never create a new one.

        This flag can be used to forbid the creation of a new instance if none
        is currently running.
     */
    wxAutomationInstance_UseExistingOnly = 0,

    /**
        Create a new instance if there are no existing ones.

        This flag corresponds to the default behaviour of
        wxAutomationObject::GetInstance() and means that if getting an existing
        instance failed, we should call wxAutomationObject::CreateInstance() to
        create a new one.
     */
    wxAutomationInstance_CreateIfNeeded = 1,

    /**
        Do not show an error message if no existing instance is currently
        running.

        All other errors will still be reported as usual.
     */
    wxAutomationInstance_SilentIfNone = 2
};

/**
    Flags used for conversions between wxVariant and OLE @c VARIANT.

    These flags are used by wxAutomationObject for its wxConvertOleToVariant()
    calls. They can be obtained by wxAutomationObject::GetConvertVariantFlags()
    and set by wxAutomationObject::SetConvertVariantFlags().

    @since 3.0

    @header{wx/msw/ole/oleutils.h}
*/
enum wxOleConvertVariantFlags
{
    /**
        Default value.
    */
    wxOleConvertVariant_Default = 0,

    /**
        If this flag is used, SAFEARRAYs contained in OLE @c VARIANTs will be
        returned as wxVariants with wxVariantDataSafeArray type instead of
        wxVariants with the list type containing the (flattened) SAFEARRAY's
        elements.
    */
    wxOleConvertVariant_ReturnSafeArrays = 1
};


/**
    @class wxVariantDataCurrency

    This class represents a thin wrapper for Microsoft Windows @c CURRENCY type.

    It is used for converting between wxVariant and OLE @c VARIANT
    with type set to @c VT_CURRENCY. When wxVariant stores
    wxVariantDataCurrency, it returns "currency" as its type.

    An example of setting and getting @c CURRENCY value to and from wxVariant:
    @code
    CURRENCY cy;
    wxVariant variant;

    // set wxVariant to currency type
    if ( SUCCEEDED(VarCyFromR8(123.45, &cy)) )  // set cy to 123.45
    {
        variant.SetData(new wxVariantDataCurrency(cy));

        // or instead of the line above you could write:
        // wxVariantDataCurrency wxCy;
        // wxCy.SetValue(cy);
        // variant.SetData(wxCy.Clone());
    }

    // get CURRENCY value from wxVariant
    if ( variant.GetType() == "currency" )
    {
        wxVariantDataCurrency*
            wxCy = wxDynamicCastVariantData(variant.GetData(), wxVariantDataCurrency);
        cy = wxCy->GetValue();
    }
    @endcode

    @onlyfor{wxmsw}
    @since 2.9.5

    @library{wxcore}
    @category{data}

    @see wxAutomationObject, wxVariant, wxVariantData, wxVariantDataErrorCode

    @header{wx/msw/ole/oleutils.h}
*/
class wxVariantDataCurrency : public wxVariantData
{
public:
    /**
        Default constructor initializes the object to 0.0.
    */
    wxVariantDataCurrency();

    /**
        Constructor from CURRENCY.
    */
    wxVariantDataCurrency(CURRENCY value);

    /**
        Returns the stored CURRENCY value.
    */
    CURRENCY GetValue() const;

    /**
        Sets the stored value to @a value.
    */
    void SetValue(CURRENCY value);

    /**
        Returns @true if @a data is of wxVariantDataCurency type
        and contains the same CURRENCY value.
    */
    virtual bool Eq(wxVariantData& data) const;

    /**
        Fills the provided string with the textual representation of this
        object.

        The implementation of this method uses @c VarBstrFromCy() Windows API
        function with @c LOCALE_USER_DEFAULT.
    */
    virtual bool Write(wxString& str) const;

    /**
        Returns a copy of itself.
    */
    wxVariantData* Clone() const;

    /**
        Returns "currency".
    */
    virtual wxString GetType() const;

    /**
        Converts the value of this object to wxAny.
    */
    virtual bool GetAsAny(wxAny* any) const;
};


/**
    @class wxVariantDataErrorCode

    This class represents a thin wrapper for Microsoft Windows @c SCODE type
    (which is the same as @c HRESULT).

    It is used for converting between a wxVariant and OLE @c VARIANT with type set
    to @c VT_ERROR. When wxVariant stores wxVariantDataErrorCode, it returns
    "errorcode" as its type. This class can be used for returning error codes
    of automation calls or exchanging values with other applications: e.g.
    Microsoft Excel returns VARIANTs with @c VT_ERROR type for cell values with
    errors (one of XlCVError constants, displayed as e.g. "#DIV/0!" or "#REF!"
    there) etc. See wxVariantDataCurrency for an example of how to  exchange
    values between wxVariant and a native type not directly supported by it.

    @onlyfor{wxmsw}
    @since 2.9.5

    @library{wxcore}
    @category{data}

    @see wxAutomationObject, wxVariant, wxVariantData, wxVariantDataCurrency

    @header{wx/msw/ole/oleutils.h}
*/
class wxVariantDataErrorCode : public wxVariantData
{
public:
    /**
        Constructor initializes the object to @a value or @c S_OK if no value was
        passed.
    */
    wxVariantDataErrorCode(SCODE value = S_OK);

    /**
        Returns the stored @c SCODE value.
    */
    SCODE GetValue() const;

    /**
        Set the stored value to @a value.
    */
    void SetValue(SCODE value);

    /**
        Returns @true if @a data is of wxVariantDataErrorCode type
        and contains the same @c SCODE value.
    */
    virtual bool Eq(wxVariantData& data) const;

    /**
        Fills the provided string with the textual representation of this
        object.

        The error code is just a number, so it's output as such.
    */
    virtual bool Write(wxString& str) const;

    /**
        Returns a copy of itself.
    */
    wxVariantData* Clone() const;

    /**
        Returns "errorcode".
    */
    virtual wxString GetType() const { return wxS("errorcode"); }

    /**
        Converts the value of this object to wxAny.
    */
    virtual bool GetAsAny(wxAny* any) const;
};

/**
    @class wxVariantDataSafeArray

    This class stores @c SAFEARRAY in a wxVariant. It can be used
    to pass arrays having more than one dimension with wxAutomationObject methods.

    When wxVariant stores wxVariantDataSafeArray, it returns "safearray" as its type.

    wxVariantDataSafeArray does NOT manage the @c SAFEARRAY it points to.
    If you want to pass it to a wxAutomationObject as a parameter:
        -# Create and fill a @c SAFEARRAY.
        -# Assign the @c SAFEARRAY pointer to it and store it in a wxVariant.
        -# Call a wxAutomationObject method (such as CallMethod() or PutProperty()) with the wxVariant as a parameter.
        -# wxAutomationObject will destroy the array after the automation call.

    An example of creating a two-dimensional @c SAFEARRAY containing <tt>VARIANT</tt>s
    and storing it in a wxVariant, using a utility class wxSafeArray<varType>.
    @code
    const size_t dimensions = 2;
    const long rowCount = 1000;
    const long columnCount = 20;

    SAFEARRAYBOUND bounds[dimensions];
    wxSafeArray<VT_VARIANT> safeArray;    

    bounds[0].lLbound = 0; // elements start at 0
    bounds[0].cElements = rowCount;
    bounds[1].lLbound = 0; // elements start at 0
    bounds[1].cElements = columnCount;

    if ( !safeArray.Create(bounds, dimensions) )
        return false;

    long indices[dimensions];

    for ( long row = 0; row < rowCount; ++row )
    {
        indices[0] = row;

        for ( long column = 0; column < columnCount; ++column )
        {
            indices[1] = column;
            if ( !safeArray.SetElement(indices, wxString::Format("R%u C%u", row, column)) )
               return false;
        }
    }

    range.PutProperty("Value", wxVariant(new wxVariantDataSafeArray(safeArray.Detach())));
    @endcode

    If you want to receive a @c SAFEARRAY in a wxVariant as a result of an wxAutomationObject
    call: 
        -# Call wxAutomationObject::SetConvertVariantFlags() with parameter ::wxOleConvertVariant_ReturnSafeArrays,
           (otherwise the data would be sent as a flattened one-dimensional list).
        -# Call a wxAutomationObject method (such as CallMethod() or GetProperty()).
        -# Retrieve the @c SAFEARRAY from the returned wxVariant.
        -# Process the data in the @c SAFEARRAY.
        -# Destroy the @c SAFEARRAY when you no longer need it.

    The following example shows how to process a two-dimensional @c SAFEARRAY
    with @c VT_VARIANT type received from a wxAutomationObject call,
    using a utility class wxSafeArray<varType>.
    @code
    const size_t dimensions = 2;

    wxVariant result;

    range.SetConvertVariantFlags(wxOleConvertVariant_ReturnSafeArrays);
    result = range.GetProperty("Value");

    if ( !result.IsType("safearray") )
       return false;

    wxSafeArray<VT_VARIANT> safeArray;
    wxVariantDataSafeArray* const
        sa = wxStaticCastVariantData(result.GetData(), wxVariantDataSafeArray);

    if ( !safeArray.Attach(sa->GetValue()) )
    {
        if ( !safeArray.HasArray() )
            ::SafeArrayDestroy(sa->GetValue()); // we have to dispose the SAFEARRAY ourselves
        return false;
    }

    if ( safeArray.GetDim() != dimensions ) // we are expecting 2 dimensions
        return false; // SAFEARRAY will be disposed by safeArray's dtor

    long rowStart, columnStart;
    long rowCount, columnCount;
    long indices[dimensions];
    wxVariant value;

    // get start indices and item counts for rows and columns
    safeArray.GetLBound(1, rowStart);
    safeArray.GetLBound(2, columnStart);
    safeArray.GetUBound(1, rowCount);
    safeArray.GetUBound(2, columnCount);

    for ( long row = rowStart; row <= rowCount; ++row )
    {
        indices[0] = row;

        for ( long column = columnStart; column <= columnCount; ++column )
        {
            indices[1] = column;
            if ( !safeArray.GetElement(indices, value) )
                return false;
            // do something with value
        }
    }
    // SAFEARRAY will be disposed by safeArray's dtor
    @endcode

    @onlyfor{wxmsw}
    @since 2.9.5

    @library{wxcore}
    @category{data}

    @see wxAutomationObject, wxSafeArray<varType>, wxVariant, wxVariantData

    @header{wx/msw/ole/oleutils.h}
*/
class wxVariantDataSafeArray : public wxVariantData
{
public:
    /**
        Constructor initializes the object to @a value.
    */
    explicit wxVariantDataSafeArray(SAFEARRAY* value = NULL);

    /**
        Returns the stored array.
    */
    SAFEARRAY* GetValue() const;

    /**
        Set the stored array.
    */
    void SetValue(SAFEARRAY* value);

    /**
        Returns @true if @a data is of wxVariantDataSafeArray type
        and contains the same SAFEARRAY* value.
    */
    virtual bool Eq(wxVariantData& data) const;

    /**
        Fills the provided string with the textual representation of this
        object.

        Only the address of @c SAFEARRAY pointer is output.
    */
    virtual bool Write(wxString& str) const;

    /**
        Returns a copy of itself.
    */
    wxVariantData* Clone() const;

    /**
        Returns "safearray".
    */
    virtual wxString GetType() const;

    /**
        Converts the value of this object to wxAny.
    */
    virtual bool GetAsAny(wxAny* any) const;
};

/**
    @class wxSafeArray<varType>

    wxSafeArray<varType> is wxWidgets wrapper for working with MS Windows @c
    SAFEARRAY used in Component Object Model (COM) and OLE Automation APIs.

    It also has convenience functions for converting between @c SAFEARRAY and
    wxVariant with list type or wxArrayString.

    wxSafeArray is a template class which must be created with an appropriate
    type matching the underlying @c VARIANT type (such as @c VT_VARIANT or @c
    VT_BSTR).

    See wxVariantDataSafeArray documentation for examples of using it.

    @onlyfor{wxmsw}
    @since 3.0

    @library{wxcore}
    @category{data}

    @see wxAutomationObject, wxVariantDataSafeArray, wxVariant

    @header{wx/msw/ole/safearray.h}
*/
template <VARTYPE varType>
class wxSafeArray<varType>
{
public:

    /**
        The default constructor.
    */
    wxSafeArray();

    /**
        The destructor unlocks and destroys the owned @c SAFEARRAY.
    */
    ~wxSafeArray();

    /**
        Creates and locks a zero-based one-dimensional @c SAFEARRAY with the
        given number of elements.
    */
    bool Create(size_t count);

    /**
        Creates and locks a @c SAFEARRAY.

        See @c SafeArrayCreate() in MSDN documentation for more information.
    */
    bool Create(SAFEARRAYBOUND* bound, size_t dimensions);

    /**
        Creates a zero-based one-dimensional @c SAFEARRAY from wxVariant
        with the list type.

        Can be called only for wxSafeArray<@c VT_VARIANT>.
    */
    bool CreateFromListVariant(const wxVariant& variant);

    /**
        Creates a zero-based one-dimensional @c SAFEARRAY from wxArrayString.

        Can be called only for wxSafeArray<@c VT_BSTR>.
    */
    bool CreateFromArrayString(const wxArrayString& strings);

    /**
        Attaches and locks an existing @c SAFEARRAY.

        The array must have the same @c VARTYPE as this wxSafeArray was
        instantiated with.
    */
    bool Attach(SAFEARRAY* array);

    /**
        Unlocks the owned @c SAFEARRAY, returns it and gives up its ownership.
    */
    SAFEARRAY* Detach();

    /**
        Unlocks and destroys the owned @c SAFEARRAY.
    */
    void Destroy();

    /**
        Returns @true if it has a valid @c SAFEARRAY.
    */
    bool HasArray() const { return m_array != NULL; }

    /**
        Returns the number of dimensions.
    */
    size_t GetDim() const;

    /**
        Returns lower bound for dimension @a dim in @a bound.

        Dimensions start at @c 1.
    */
    bool GetLBound(size_t dim, long& bound) const;

    /**
        Returns upper bound for dimension @a dim in @a bound.

        Dimensions start at @c 1.
    */
    bool GetUBound(size_t dim, long& bound) const;

    /**
        Returns element count for dimension @a dim. Dimensions start at @c 1.
    */
    size_t GetCount(size_t dim) const;

    /**
        Change the value of the specified element.

        @a indices have the same row-column order as @c rgIndices i
        @c SafeArrayPutElement(), i.e., the right-most dimension is
        <tt>rgIndices[0]</tt> and the left-most dimension is stored at
        <tt>rgIndices[</tt>GetDim()<tt> – 1]</tt>.

        @a element must be of type matching @c varType this wxSafeArray was
        created with. For example, wxString for wxSafeArray<@c VT_BSTR>,
        wxVariant for wxSafeArray<@c VT_VARIANT>, or @c double for
        wxSafeArray<@c VT_R8>.
    */
    bool SetElement(long* indices, const externT& element);

    /**
        Retrieve the value of the specified element.

        @a indices have the same row-column order as @c rgIndices in
        @c SafeArrayGetElement(), i.e., the right-most dimension is
        <tt>rgIndices[0]</tt> and the left-most dimension is stored at
        <tt>rgIndices[</tt>GetDim()<tt> – 1]</tt>.

        @a element must be of type matching @c varType this wxSafeArray was
        created with. For example, wxString for wxSafeArray<@c VT_BSTR>,
        wxVariant for wxSafeArray<@c VT_VARIANT>, or @c double for
        wxSafeArray<@c VT_R8>.
    */
    bool GetElement(long* indices, externT& element) const;

    /**
        Converts the array to a wxVariant with the list type, regardless of the
        underlying @c SAFEARRAY type.

        If the array is multidimensional, it is flattened using the algorithm
        originally employed in wxConvertOleToVariant().
    */
    bool ConvertToVariant(wxVariant& variant) const;

    /**
        Converts an array to wxArrayString.

        Can be called only for wxSafeArray<@c VT_BSTR>. If the array is
        multidimensional, it is flattened using the algorithm originally
        employed in wxConvertOleToVariant().
    */
    bool ConvertToArrayString(wxArrayString& strings) const;

    /**
        Converts @a psa to wxVariant.

        @see wxSafeArray<varType>::ConvertToVariant(wxVariant&) const
    */
    static bool ConvertToVariant(SAFEARRAY* psa, wxVariant& variant);

    /**
        Converts @a psa to wxArrayString.

        @see wxSafeArray<varType>::ConvertToArrayString(wxArrayString&) const

    */
    static bool ConvertToArrayString(SAFEARRAY* psa, wxArrayString& strings);
};

/**
    @class wxAutomationObject

    The @b wxAutomationObject class represents an OLE automation object containing
    a single data member,
    an @c IDispatch pointer. It contains a number of functions that make it easy to
    perform
    automation operations, and set and get properties. The class makes heavy use of
    the wxVariant class.

    The usage of these classes is quite close to OLE automation usage in Visual
    Basic. The API is
    high-level, and the application can specify multiple properties in a single
    string. The following example
    gets the current Microsoft Excel instance, and if it exists, makes the active cell bold.

    @code
    wxAutomationObject excelObject;

    if ( excelObject.GetInstance("Excel.Application") )
        excelObject.PutProperty("ActiveCell.Font.Bold", true);
    @endcode

    Note that this class obviously works under Windows only.

    @onlyfor{wxmsw}

    @library{wxcore}
    @category{data}

    @see wxVariant, wxVariantDataCurrency, wxVariantDataErrorCode, wxVariantDataSafeArray
*/
class wxAutomationObject : public wxObject
{
public:
    /**
        Constructor, taking an optional @c IDispatch pointer which will be released when
        the
        object is deleted.
    */
    wxAutomationObject(WXIDISPATCH* dispatchPtr = NULL);

    /**
        Destructor. If the internal @c IDispatch pointer is non-null, it will be released.
    */
    ~wxAutomationObject();

    //@{
    /**
        Calls an automation method for this object. The first form takes a method name,
        number of
        arguments, and an array of variants. The second form takes a method name and
        zero to six
        constant references to variants. Since the variant class has constructors for
        the basic
        data types, and C++ provides temporary objects automatically, both of the
        following lines
        are syntactically valid:
        @code
        wxVariant res = obj.CallMethod("Sum", wxVariant(1.2), wxVariant(3.4));
        wxVariant res = obj.CallMethod("Sum", 1.2, 3.4);
        @endcode

        Note that @a method can contain dot-separated property names, to save the
        application
        needing to call GetProperty() several times using several temporary objects. For
        example:
        @code
        object.CallMethod("ActiveWorkbook.ActiveSheet.Protect", "MyPassword");
        @endcode
    */
    wxVariant CallMethod(const wxString& method, int noArgs,
                         wxVariant args[]) const;
    wxVariant CallMethod(const wxString& method, ... ) const;
    //@}

    /**
        Creates a new object based on the @a progID, returning @true if the object was
        successfully created,
        or @false if not.

        @see GetInstance()
    */
    bool CreateInstance(const wxString& progId) const;

    /**
        Checks if the object is in a valid state.

        Returns @true if the object was successfully initialized or @false if
        it has no valid @c IDispatch pointer.

        @see GetDispatchPtr()
     */
    bool IsOk() const;

    /**
        Gets the @c IDispatch pointer.

        Notice that the return value of this function is an untyped pointer but
        it can be safely cast to @c IDispatch.
    */
    WXIDISPATCH* GetDispatchPtr() const;

    /**
        Retrieves the current object associated with the specified ProgID, and
        attaches the @c IDispatch pointer to this object.

        If attaching to an existing object failed and @a flags includes
        ::wxAutomationInstance_CreateIfNeeded flag, a new object will be created.
        Otherwise this function will normally log an error message which may be
        undesirable if the object may or may not exist. The
        ::wxAutomationInstance_SilentIfNone flag can be used to prevent the error
        from being logged in this case.

        Returns @true if a pointer was successfully retrieved, @false
        otherwise.

        Note that this cannot cope with two instances of a given OLE object being
        active simultaneously,
        such as two copies of Microsoft Excel running. Which object is referenced cannot
        currently be specified.

        @param progId COM ProgID, e.g. "Excel.Application"
        @param flags The creation flags (this parameters was added in wxWidgets
            2.9.2)

        @see CreateInstance()
    */
    bool GetInstance(const wxString& progId,
                     int flags = wxAutomationInstance_CreateIfNeeded) const;

    /**
        Retrieves a property from this object, assumed to be a dispatch pointer, and
        initialises @a obj with it.
        To avoid having to deal with @c IDispatch pointers directly, use this function in
        preference
        to GetProperty() when retrieving objects
        from other objects.
        Note that an @c IDispatch pointer is stored as a @c void* pointer in wxVariant
        objects.

        @see GetProperty()
    */
    bool GetObject(wxAutomationObject& obj, const wxString& property,
                   int noArgs = 0,
                   wxVariant args[] = NULL) const;

    //@{
    /**
        Gets a property value from this object. The first form takes a property name,
        number of
        arguments, and an array of variants. The second form takes a property name and
        zero to six
        constant references to variants. Since the variant class has constructors for
        the basic
        data types, and C++ provides temporary objects automatically, both of the
        following lines
        are syntactically valid:
        @code
        wxVariant res = obj.GetProperty("Range", wxVariant("A1"));
        wxVariant res = obj.GetProperty("Range", "A1");
        @endcode

        Note that @a property can contain dot-separated property names, to save the
        application
        needing to call GetProperty several times using several temporary objects.

        @see GetObject(), PutProperty()
    */
    wxVariant GetProperty(const wxString& property, int noArgs,
                          wxVariant args[]) const;
    wxVariant GetProperty(const wxString& property, ... ) const;
    //@}

    /**
        This function is a low-level implementation that allows access to the @c IDispatch
        Invoke function.
        It is not meant to be called directly by the application, but is used by other
        convenience functions.

        @param member
            The member function or property name.
        @param action
            Bitlist: may contain @c DISPATCH_PROPERTYPUT, @c DISPATCH_PROPERTYPUTREF,
            @c DISPATCH_PROPERTYGET, @c DISPATCH_METHOD.
        @param retValue
            Return value (ignored if there is no return value)
        @param noArgs
            Number of arguments in args or ptrArgs.
        @param args
            If non-null, contains an array of variants.
        @param ptrArgs
            If non-null, contains an array of constant pointers to variants.

        @return @true if the operation was successful, @false otherwise.

        @remarks Two types of argument array are provided, so that when possible
                 pointers are used for efficiency.
    */
    bool Invoke(const wxString& member, int action,
                wxVariant& retValue, int noArgs,
                wxVariant args[],
                const wxVariant* ptrArgs[] = 0) const;

    //@{
    /**
        Puts a property value into this object. The first form takes a property name,
        number of
        arguments, and an array of variants. The second form takes a property name and
        zero to six
        constant references to variants. Since the variant class has constructors for
        the basic
        data types, and C++ provides temporary objects automatically, both of the
        following lines
        are syntactically valid:
        @code
        obj.PutProperty("Value", wxVariant(23));
        obj.PutProperty("Value", 23);
        @endcode

        Note that @a property can contain dot-separated property names, to save the
        application
        needing to call GetProperty() several times using several temporary objects.
    */
    bool PutProperty(const wxString& property, int noArgs,
                     wxVariant args[]);
    bool PutProperty(const wxString& property, ... );
    //@}

    /**
        Sets the @c IDispatch pointer, does not check if there is already one.
        You may need to cast from @c IDispatch* to @c WXIDISPATCH* when calling this function.
    */
    void SetDispatchPtr(WXIDISPATCH* dispatchPtr);

    /**
        Returns the locale identifier used in automation calls.

        The default is @c LOCALE_SYSTEM_DEFAULT but the objects obtained by
        GetObject() inherit the locale identifier from the one that created
        them.

        @since 2.9.5
    */
    WXLCID GetLCID() const;

    /**
        Sets the locale identifier to be used in automation calls performed by
        this object.

        The default value is @c LOCALE_SYSTEM_DEFAULT.

        Notice that any automation objects created by this one inherit the same
        @c LCID.

        @since 2.9.5
    */
    void SetLCID(WXLCID lcid);

    /**
        Returns the flags used for conversions between wxVariant and OLE
        @c VARIANT, see #wxOleConvertVariantFlags.

        The default value is ::wxOleConvertVariant_Default for compatibility but
        it can be changed using SetConvertVariantFlags().

        Notice that objects obtained by GetObject() inherit the flags from the
        one that created them.

        @since 3.0
    */
    long GetConvertVariantFlags() const;

    /**
        Sets the flags used for conversions between wxVariant and OLE @c VARIANT,
        see #wxOleConvertVariantFlags.

        The default value is ::wxOleConvertVariant_Default.

        @since 3.0
    */
    void SetConvertVariantFlags(long flags);
};

