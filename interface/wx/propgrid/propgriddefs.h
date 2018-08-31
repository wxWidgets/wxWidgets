/////////////////////////////////////////////////////////////////////////////
// Name:        propgriddefs.h
// Purpose:     various constants, etc. used in documented propgrid API
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------

/** @section propgrid_misc wxPropertyGrid Miscellanous

    This section describes some miscellanous values, types and macros.
    @{
*/

// Used to tell wxPGProperty to use label as name as well
#define wxPG_LABEL              (*wxPGProperty::sm_wxPG_LABEL)

// This is the value placed in wxPGProperty::sm_wxPG_LABEL
#define wxPG_LABEL_STRING       wxS("@!")
#define wxPG_NULL_BITMAP        wxNullBitmap
#define wxPG_COLOUR_BLACK       (*wxBLACK)

/** Convert Red, Green and Blue to a single 32-bit value.
*/
#define wxPG_COLOUR(R,G,B) ((wxUint32)(R+(G<<8)+(B<<16)))


/** If property is supposed to have custom-painted image, then returning
    this in OnMeasureImage() will usually be enough.
*/
#define wxPG_DEFAULT_IMAGE_SIZE             wxSize(-1, -1)


/** This callback function is used for sorting properties.

    Call wxPropertyGrid::SetSortFunction() to set it.

    Sort function should return a value greater than 0 if position of p1 is
    after p2. So, for instance, when comparing property names, you can use
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



typedef wxString wxPGCachedString;

/** @}
*/

// -----------------------------------------------------------------------

// Used to indicate wxPGChoices::Add etc that the value is actually not given
// by the caller.
#define wxPG_INVALID_VALUE      INT_MAX

// -----------------------------------------------------------------------

enum wxPG_GETPROPERTYVALUES_FLAGS
{

/** Flags for wxPropertyGridInterface::GetPropertyValues */
wxPG_KEEP_STRUCTURE               = 0x00000010,

/** Flags for wxPropertyGrid::SetPropertyAttribute() etc */
wxPG_RECURSE                      = 0x00000020,

/** Include attributes for GetPropertyValues. */
wxPG_INC_ATTRIBUTES               = 0x00000040,

/** Used when first starting recursion. */
wxPG_RECURSE_STARTS               = 0x00000080,

/** Force value change. */
wxPG_FORCE                        = 0x00000100,

/** Only sort categories and their immediate children.
    Sorting done by wxPG_AUTO_SORT option uses this.
*/
wxPG_SORT_TOP_LEVEL_ONLY          = 0x00000200

};

/** Flags for wxPropertyGrid::SetPropertyAttribute() etc */
#define wxPG_DONT_RECURSE         0x00000000

// -----------------------------------------------------------------------

// Misc argument flags.
enum wxPG_MISC_ARG_FLAGS
{
    // Get/Store full value instead of displayed value.
    wxPG_FULL_VALUE                     = 0x00000001,

    wxPG_REPORT_ERROR                   = 0x00000002,

    wxPG_PROPERTY_SPECIFIC              = 0x00000004,

    // Get/Store editable value instead of displayed one (should only be
    // different in the case of common values)
    wxPG_EDITABLE_VALUE                 = 0x00000008,

    // Used when dealing with fragments of composite string value
    wxPG_COMPOSITE_FRAGMENT             = 0x00000010,

    // Means property for which final string value is for cannot really be
    // edited.
    wxPG_UNEDITABLE_COMPOSITE_FRAGMENT  = 0x00000020,

    // ValueToString() called from GetValueAsString()
    // (guarantees that input wxVariant value is current own value)
    wxPG_VALUE_IS_CURRENT               = 0x00000040,

    // Value is being set programmatically (ie. not by user)
    wxPG_PROGRAMMATIC_VALUE             = 0x00000080
};

// -----------------------------------------------------------------------

// wxPGProperty::SetValue() flags
enum wxPG_SETVALUE_FLAGS
{
    wxPG_SETVAL_REFRESH_EDITOR      = 0x0001,
    wxPG_SETVAL_AGGREGATED          = 0x0002,
    wxPG_SETVAL_FROM_PARENT         = 0x0004,
    wxPG_SETVAL_BY_USER             = 0x0008  // Set if value changed by user
};

// -----------------------------------------------------------------------

//
// Valid constants for wxPG_UINT_BASE attribute
// (long because of wxVariant constructor)
#define wxPG_BASE_OCT                       (long)8
#define wxPG_BASE_DEC                       (long)10
#define wxPG_BASE_HEX                       (long)16
#define wxPG_BASE_HEXL                      (long)32

//
// Valid constants for wxPG_UINT_PREFIX attribute
#define wxPG_PREFIX_NONE                    (long)0
#define wxPG_PREFIX_0x                      (long)1
#define wxPG_PREFIX_DOLLAR_SIGN             (long)2

// -----------------------------------------------------------------------
