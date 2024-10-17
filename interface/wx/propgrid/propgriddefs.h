/////////////////////////////////////////////////////////////////////////////
// Name:        propgriddefs.h
// Purpose:     various constants, etc. used in documented propgrid API
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------

/** Used to tell wxPGProperty to use label as name as well
    @hideinitializer
*/
#define wxPG_LABEL              (*wxPGProperty::sm_labelItem)

/** Convert Red, Green and Blue to a single 32-bit value.
    @hideinitializer
*/
#define wxPG_COLOUR(R,G,B) ((wxUint32)(R+(G<<8)+(B<<16)))


/** If property is supposed to have custom-painted image, then returning
    this in wxPGProperty::OnMeasureImage() will usually be enough.
    @hideinitializer
*/
#define wxPG_DEFAULT_IMAGE_SIZE             wxDefaultSize


/** This callback function is used for sorting properties.

    Call wxPropertyGrid::SetSortFunction() to set it.

    Sort function should return a value greater than 0 if position of @a p1 is
    after @a p2. So, for instance, when comparing property names, you can use
    following implementation:

        @code
            int MyPropertySortFunction(wxPropertyGrid* propGrid,
                                       wxPGProperty* p1,
                                       wxPGProperty* p2)
            {
                return p1->GetBaseName().compare( p2->GetBaseName() );
            }
        @endcode
*/
typedef int (*wxPGSortCallback)(wxPropertyGrid* propGrid,
                                wxPGProperty* p1,
                                wxPGProperty* p2);

// -----------------------------------------------------------------------

/** Used to indicate wxPGChoices::Add() etc that the value is actually not given
    by the caller.
    @hideinitializer
*/
constexpr int wxPG_INVALID_VALUE = std::numeric_limits<int>::max();

// -----------------------------------------------------------------------

enum class wxPGPropertyValuesFlags : int
{
    /** Flag for wxPropertyGridInterface::SetProperty* functions,
        wxPropertyGridInterface::HideProperty(), etc.
        Apply changes only for the property in question.
        @hideinitializer
    */
    DontRecurse      = 0x00000000,

    /** Flag for wxPropertyGridInterface::GetPropertyValues().
        Use this flag to retain category structure; each sub-category
        will be its own wxVariantList of wxVariant.
        @hideinitializer
    */
    KeepStructure    = 0x00000010,

    /** Flag for wxPropertyGridInterface::SetProperty* functions,
        wxPropertyGridInterface::HideProperty(), etc.
        Apply changes recursively for the property and all its children.
        @hideinitializer
    */
    Recurse          = 0x00000020,

    /** Flag for wxPropertyGridInterface::GetPropertyValues().
        Use this flag to include property attributes as well.
        @hideinitializer
    */
    IncAttributes    = 0x00000040,

    /** Used when first starting recursion.
        @hideinitializer
    */
    RecurseStarts    = 0x00000080,

    /** Force value change.
        @hideinitializer
    */
    Force            = 0x00000100,

    /** Only sort categories and their immediate children.
        Sorting done by ::wxPG_AUTO_SORT option uses this.
        @hideinitializer
    */
    SortTopLevelOnly = 0x00000200
};

// -----------------------------------------------------------------------

/** Miscellaneous property value format flags.
*/
enum class wxPGPropValFormatFlags : int
{
    /** No flags.
        @hideinitializer
    */
    Null                        = 0,

    /** Get/Store full value instead of displayed value.
        @hideinitializer
    */
    FullValue                   = 0x00000001,

    /** Perform special action in case of unsuccessful conversion.
        @hideinitializer
    */
    ReportError                 = 0x00000002,

    /**
        @hideinitializer
    */
    PropertySpecific            = 0x00000004,

    /** Get/Store editable value instead of displayed one (should only be
        different in the case of common values).
        @hideinitializer
    */
    EditableValue               = 0x00000008,

    /** Used when dealing with fragments of composite string value
        @hideinitializer
    */
    CompositeFragment           = 0x00000010,

    /** Means property for which final string value is for cannot really be
        edited.
        @hideinitializer
    */
    UneditableCompositeFragment = 0x00000020,

    /** wxPGProperty::ValueToString() called from wxPGProperty::GetValueAsString()
        (guarantees that input wxVariant value is current own value)
        @hideinitializer
    */
    ValueIsCurrent              = 0x00000040,

    /** Value is being set programmatically (i.e. not by user)
        @hideinitializer
    */
    ProgrammaticValue           = 0x00000080
};

// -----------------------------------------------------------------------

/** wxPGProperty::SetValue() flags
*/
enum class wxPGSetValueFlags : int
{
    /**
        @hideinitializer
    */
    RefreshEditor = 0x0001,

    /**
        @hideinitializer
    */
    Aggregated    = 0x0002,

    /**
        @hideinitializer
    */
    FromParent    = 0x0004,

    /** Set if value changed by user
        @hideinitializer
    */
    ByUser        = 0x0008
};

// -----------------------------------------------------------------------

/** Constant for ::wxPG_UINT_BASE attribute
    @hideinitializer
*/
constexpr long wxPG_BASE_OCT =   8L;
/** Constant for ::wxPG_UINT_BASE attribute
    @hideinitializer
*/
constexpr long wxPG_BASE_DEC =  10L;
/** Constant for ::wxPG_UINT_BASE attribute
    @hideinitializer
*/
constexpr long wxPG_BASE_HEX =  16L;
/** Constant for ::wxPG_UINT_BASE attribute
    @hideinitializer
*/
constexpr long wxPG_BASE_HEXL = 32L;

/** Constant for ::wxPG_UINT_PREFIX attribute
    @hideinitializer
*/
constexpr long wxPG_PREFIX_NONE        = 0L;
/** Constant for ::wxPG_UINT_PREFIX attribute
    @hideinitializer
*/
constexpr long wxPG_PREFIX_0x          = 1L;
/** Constant for ::wxPG_UINT_PREFIX attribute
    @hideinitializer
*/
constexpr long wxPG_PREFIX_DOLLAR_SIGN = 2L;

// -----------------------------------------------------------------------
