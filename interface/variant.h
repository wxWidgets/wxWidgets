/////////////////////////////////////////////////////////////////////////////
// Name:        variant.h
// Purpose:     interface of wxVariant
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxVariant
    @wxheader{variant.h}

    The @b wxVariant class represents a container for any type. A variant's value
    can be changed at run time, possibly to a different type of value.

    As standard, wxVariant can store values of type bool, wxChar, double, long,
    string, string list, time, date, void pointer, list of strings, and list of variants.
    However, an application can extend wxVariant's capabilities by deriving from the
    class wxVariantData and using the wxVariantData form of the wxVariant constructor
    or assignment operator to assign this data to a variant.
    Actual values for user-defined types will need to be accessed via the wxVariantData
    object, unlike the case for basic data types where convenience functions such as
    wxVariant::GetLong can be used.

    Pointers to any wxObject derived class can also easily be stored in a wxVariant.
    wxVariant will then use wxWidgets' built-in RTTI system to set the type name 
    (returned by wxVariant::GetType) and to perform type-safety checks at runtime.

    This class is useful for reducing the programming for certain tasks, such as
    an editor for different data types, or a remote procedure call protocol.

    An optional name member is associated with a wxVariant. This might be used,
    for example, in CORBA or OLE automation classes, where named parameters are required.

    Note that as of wxWidgets 2.7.1, wxVariant is @ref overview_trefcount
    "reference counted".
    Additionally, the convenience macros @b DECLARE_VARIANT_OBJECT and
    @b IMPLEMENT_VARIANT_OBJECT were added so that adding (limited) support
    for conversion to and from wxVariant can be very easily implemented
    without modifying either wxVariant or the class to be stored by wxVariant.
    Since assignment operators cannot be declared outside the class, the shift
    left operators are used like this:

    @code
    // in the header file
    DECLARE_VARIANT_OBJECT(MyClass)

    // in the implementation file
    IMPLEMENT_VARIANT_OBJECT(MyClass)

    // in the user code
    wxVariant variant;
    MyClass value;
    variant << value;

    // or
    value << variant;
    @endcode

    For this to work, MyClass must derive from wxObject, implement
    the @ref overview_runtimeclassoverview "wxWidgets RTTI system"
    and support the assignment operator and equality operator for itself. Ideally,
    it
    should also be reference counted to make copying operations cheap and fast. This
    can be most easily implemented using the reference counting support offered by
    wxObject itself. By default, wxWidgets already implements
    the shift operator conversion for a few of its drawing related classes:

    @code
    IMPLEMENT_VARIANT_OBJECT(wxColour)
    IMPLEMENT_VARIANT_OBJECT(wxImage)
    IMPLEMENT_VARIANT_OBJECT(wxIcon)
    IMPLEMENT_VARIANT_OBJECT(wxBitmap)
    @endcode

    Note that as of wxWidgets 2.9.0, wxVariantData no longer inherits from wxObject
    and wxVariant no longer uses the type-unsafe wxList class for list
    operations but the type-safe wxVariantList class. Also, wxVariantData now
    supports the Clone function for implementing the wxVariant::Unshare function.
    Clone is implemented automatically by IMPLEMENT_VARIANT_OBJECT.

    Since wxVariantData no longer derives from wxObject, any code that tests the
    type of the data using wxDynamicCast will require adjustment. You can use the
    macro wxDynamicCastVariantData with the same arguments as wxDynamicCast, to
    use C++ RTTI type information instead of wxWidgets RTTI.

    @library{wxbase}
    @category{data}

    @see wxVariantData
*/
class wxVariant : public wxObject
{
public:
    /**
        Default constructor.
    */
    wxVariant();
    
    /**
        Constructs a variant directly with a wxVariantData
        object. wxVariant will take ownership of the wxVariantData
        and will not increase its reference count.
    */
    wxVariant(wxVariantData* data, const wxString& name = "");
    
    /**
        Constructs a variant from another variant by increasing the
        reference count.
    */
    wxVariant(const wxVariant& variant);
    
    /**
        Constructs a variant from a wide string literal.
    */
    wxVariant(const wxChar* value, const wxString& name = "");
    
    /**
        Constructs a variant from a string.
    */
    wxVariant(const wxString& value, const wxString& name = "");
    
    /**
        Constructs a variant from a wide char.
    */
    wxVariant(wxChar value, const wxString& name = "");
    
    /**
        Constructs a variant from a long.
    */
    wxVariant(long value, const wxString& name = "");
    
    /**
        Constructs a variant from a bool.
    */
    wxVariant(bool value, const wxString& name = "");
    
    /**
        Constructs a variant from a double.
    */
    wxVariant(double value, const wxString& name = "");
    
    /**
        Constructs a variant from a list of variants
    */
    wxVariant(const wxVariantList& value,
              const wxString& name = "");
    
    /**
        Constructs a variant from a void pointer.
    */
    wxVariant(void* value, const wxString& name = "");
    
    /**
        Constructs a variant from a pointer to an wxObject
        derived class.
    */
    wxVariant(wxObject* value, const wxString& name = "");
    
    /**
        Constructs a variant from a wxDateTime.
    */
    wxVariant(wxDateTime& val, const wxString& name = "");
    
    /**
        Constructs a variant from a wxArrayString.
    */
    wxVariant(wxArrayString& val, const wxString& name = "");

    /**
        Destructor.
        Note that destructor is protected, so wxVariantData cannot usually
        be deleted. Instead, wxVariantData::DecRef should be called.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
    */
    ~wxVariant();

    /**
        @name List functionality
    */
    //@{
    /**
        Returns the value at @a idx (zero-based).
    */
    wxVariant operator [](size_t idx);
    /**
        Returns a reference to the value at @a idx (zero-based). This can be used
        to change the value at this index.
    */
    const wxVariant& operator [](size_t idx);
    /**
        Appends a value to the list.
    */
    void Append(const wxVariant& value);
    /**
        Deletes the contents of the list.
    */
    void ClearList();
    /**
        Deletes the zero-based @a item from the list.
    */
    bool Delete(size_t item);
    /**
        Returns the number of elements in the list.
    */
    size_t GetCount() const;
    /**
        Returns a reference to the wxVariantList class used by
        wxVariant if this wxVariant is currently a list of variants.
    */
    wxVariantList& GetList() const;
    /**
        Makes the variant null by deleting the internal data and
        set the name to @e wxEmptyString.
    */
    void Clear();
    /**
        Inserts a value at the front of the list.
    */
    void Insert(const wxVariant& value);
    /**
        Makes an empty list. This differs from a null variant which has no data;
        a null list is of type list, but the number of elements in the list is zero.
    */
    void NullList();
    //@}

    //@{
    /**
        Retrieves and converts the value of this variant to the type that @a value is.
    */
    bool Convert(long* value) const;
    const bool Convert(bool* value) const;
    const bool Convert(double* value) const;
    const bool Convert(wxString* value) const;
    const bool Convert(wxChar* value) const;
    const bool Convert(wxDateTime* value) const;
    //@}

    /**
        Returns the string array value.
    */
    wxArrayString GetArrayString() const;

    /**
        Returns the boolean value.
    */
    bool GetBool() const;

    /**
        Returns the character value.
    */
    wxChar GetChar() const;

    /**
        Returns a pointer to the internal variant data. To take ownership
        of this data, you must call its wxVariantData::IncRef
        method. When you stop using it, wxVariantData::DecRef
        must be likewise called.
    */
    wxVariantData* GetData() const;

    /**
        Returns the date value.
    */
    wxDateTime GetDateTime() const;

    /**
        Returns the floating point value.
    */
    double GetDouble() const;

    /**
        Returns the integer value.
    */
    long GetLong() const;

    /**
        Returns a constant reference to the variant name.
    */
    const wxString GetName() const;

    /**
        Gets the string value.
    */
    wxString GetString() const;

    /**
        Returns the value type as a string. The built-in types are: bool, char,
        datetime, double, list, long, string, arrstring, void*.
        If the variant is null, the value type returned is the string "null" (not the
        empty string).
    */
    wxString GetType() const;

    /**
        Gets the void pointer value.
    */
    void* GetVoidPtr() const;

    /**
        Gets the wxObject pointer value.
    */
    wxObject* GetWxObjectPtr() const;

    /**
        Returns @true if there is no data associated with this variant, @false if there
        is data.
    */
    bool IsNull() const;

    /**
        Returns @true if @a type matches the type of the variant, @false otherwise.
    */
    bool IsType(const wxString& type) const;

    /**
        Returns @true if the data is derived from the class described by @e type, @false
        otherwise.
    */
    bool IsValueKindOf(const wxClassInfo* type type) const;

    /**
        Makes the variant null by deleting the internal data.
    */
    void MakeNull();

    /**
        Makes a string representation of the variant value (for any type).
    */
    wxString MakeString() const;

    /**
        Returns @true if @a value matches an element in the list.
    */
    bool Member(const wxVariant& value) const;

    /**
        Sets the internal variant data, deleting the existing data if there is any.
    */
    void SetData(wxVariantData* data);

    /**
        Makes sure that any data associated with this variant is not shared with other
        variants. For this to work, wxVariantData::Clone must
        be implemented for the data types you are working with. Clone is implemented
        for all the default data types.
    */
    bool Unshare();

    //@{
    /**
        Inequality test operators.
    */
    bool operator !=(const wxVariant& value) const;
    const bool operator !=(const wxString& value) const;
    const bool operator !=(const wxChar* value) const;
    const bool operator !=(wxChar value) const;
    const bool operator !=(const long value) const;
    const bool operator !=(const bool value) const;
    const bool operator !=(const double value) const;
    const bool operator !=(void* value) const;
    const bool operator !=(wxObject* value) const;
    const bool operator !=(const wxVariantList& value) const;
    const bool operator !=(const wxArrayString& value) const;
    const bool operator !=(const wxDateTime& value) const;
    //@}

    //@{
    /**
        Assignment operators, using @ref overview_trefcount "reference counting" when
        possible.
    */
    void operator =(const wxVariant& value);
    void operator =(wxVariantData* value);
    void operator =(const wxString& value);
    void operator =(const wxChar* value);
    void operator =(wxChar value);
    void operator =(const long value);
    void operator =(const bool value);
    void operator =(const double value);
    void operator =(void* value);
    void operator =(wxObject* value);
    void operator =(const wxVariantList& value);
    void operator =(const wxDateTime& value);
    void operator =(const wxArrayString& value);
    void operator =(const DATE_STRUCT* value);
    void operator =(const TIME_STRUCT* value);
    void operator =(const TIMESTAMP_STRUCT* value);
    //@}

    //@{
    /**
        Equality test operators.
    */
    bool operator ==(const wxVariant& value) const;
    const bool operator ==(const wxString& value) const;
    const bool operator ==(const wxChar* value) const;
    const bool operator ==(wxChar value) const;
    const bool operator ==(const long value) const;
    const bool operator ==(const bool value) const;
    const bool operator ==(const double value) const;
    const bool operator ==(void* value) const;
    const bool operator ==(wxObject* value) const;
    const bool operator ==(const wxVariantList& value) const;
    const bool operator ==(const wxArrayString& value) const;
    const bool operator ==(const wxDateTime& value) const;
    //@}

    //@{
    /**
        Operator for implicit conversion to a long, using GetLong().
    */
    double operator double() const;
    const long operator long() const;
    //@}

    /**
        Operator for implicit conversion to a pointer to a void, using GetVoidPtr().
    */
    void* operator void*() const;

    /**
        Operator for implicit conversion to a wxChar, using GetChar().
    */
    char operator wxChar() const;

    /**
        Operator for implicit conversion to a pointer to a wxDateTime, using
        GetDateTime().
    */
    void* operator wxDateTime() const;

    /**
        Operator for implicit conversion to a string, using MakeString().
    */
    wxString operator wxString() const;
};



/**
    @class wxVariantData
    @wxheader{variant.h}

    The @b wxVariantData class is used to implement a new type for wxVariant.
    Derive from wxVariantData, and override the pure virtual functions.

    wxVariantData is @ref overview_refcount "reference counted", but you don't
    normally have to care about this,
    as wxVariant manages the count automatically. However, in case your application
    needs to take
    ownership of wxVariantData, be aware that the object is created with reference
    count of 1,
    and passing it to wxVariant will not increase this. In other words,
    wxVariantData::IncRef
    needs to be called only if you both take ownership of wxVariantData and pass it
    to a wxVariant.
    Also note that the destructor is protected, so you can never explicitly delete
    a wxVariantData
    instance. Instead, wxVariantData::DecRef will delete the object automatically
    when the reference count reaches zero.

    @library{wxbase}
    @category{FIXME}

    @see wxVariant
*/
class wxVariantData
{
public:
    /**
        Default constructor.
    */
    wxVariantData();

    /**
        This function can be overridden to clone the data.
        Implement Clone if you wish wxVariant::Unshare to work
        for your data. This function is implemented for all built-in data types.
    */
    wxVariantData* Clone() const;

    /**
        Decreases reference count. If the count reaches zero, the object is
        automatically deleted.
        Note that destructor of wxVariantData is protected, so delete
        cannot be used as normal. Instead, DecRef() should be called.
    */
    void DecRef();

    /**
        Returns @true if this object is equal to @e data.
    */
    bool Eq(wxVariantData& data) const;

    /**
        Returns the string type of the data.
    */
    wxString GetType() const;

    /**
        If the data is a wxObject returns a pointer to the objects wxClassInfo
        structure, if
        the data isn't a wxObject the method returns @NULL.
    */
    wxClassInfo* GetValueClassInfo() const;

    /**
        Increases reference count. Note that initially wxVariantData has reference
        count of 1.
    */
    void IncRef();

    //@{
    /**
        Reads the data from @a stream or @e string.
    */
    bool Read(ostream& stream);
    bool Read(wxString& string);
    //@}

    //@{
    /**
        Writes the data to @a stream or @e string.
    */
    bool Write(ostream& stream) const;
    const bool Write(wxString& string) const;
    //@}

    /**
        This macro returns the data stored in @e variant cast to the type @e classname
        * if
        the data is of this type (the check is done during the run-time) or
        @NULL otherwise.
    */
    classname* wxGetVariantCast();
};

