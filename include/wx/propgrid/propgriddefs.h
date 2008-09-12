/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/propgriddefs.h
// Purpose:     wxPropertyGrid miscellaneous definitions
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-31
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PROPGRIDDEFS_H_
#define _WX_PROPGRID_PROPGRIDDEFS_H_

#include "wx/dynarray.h"
#include "wx/hashmap.h"
#include "wx/variant.h"
#include "wx/longlong.h"
#include "wx/clntdata.h"

// -----------------------------------------------------------------------

//
// Here are some platform dependent defines
// NOTE: More in propertygrid.cpp
//

#ifndef SWIG

#if defined(__WXMSW__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    0

    // 1 to use double-buffer that guarantees flicker-free painting
    #define wxPG_DOUBLE_BUFFER          1

    // Enable tooltips
    #define wxPG_SUPPORT_TOOLTIPS       1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 to create controls out of sight, hide them, and then move them into
    // correct position
    #define wxPG_CREATE_CONTROLS_HIDDEN 0

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    // If 1, then setting empty tooltip actually hides it
    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1

    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0

#elif defined(__WXGTK__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    0

    // 1 to use double-buffer that guarantees flicker-free painting
    #define wxPG_DOUBLE_BUFFER          1

    // Enable tooltips
    #define wxPG_SUPPORT_TOOLTIPS       1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 to create controls out of sight, hide them, and then move them into
    // correct position
    #define wxPG_CREATE_CONTROLS_HIDDEN 0

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    // If 1, then setting empty tooltip actually hides it
    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1

#elif defined(__WXMAC__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             11
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // 1 to use double-buffer that guarantees flicker-free painting
    #define wxPG_DOUBLE_BUFFER          0

    // Enable tooltips
    #define wxPG_SUPPORT_TOOLTIPS       1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 to create controls out of sight, hide them, and then move them into
    // correct position
    #define wxPG_CREATE_CONTROLS_HIDDEN 0

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    // If 1, then setting empty tooltip actually hides it
    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0

#else // defaults

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    0

    // 1 to use double-buffer that guarantees flicker-free painting
    #define wxPG_DOUBLE_BUFFER          0

    // Enable tooltips
    #define wxPG_SUPPORT_TOOLTIPS       0

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 to create controls out of sight, hide them, and then move them into
    // correct position
    #define wxPG_CREATE_CONTROLS_HIDDEN 0

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    // If 1, then setting empty tooltip actually hides it
    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1
#endif // platform


#define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#define wxCC_CUSTOM_IMAGE_MARGIN1       4  // before image
#define wxCC_CUSTOM_IMAGE_MARGIN2       5  // after image

#define wxPG_DRAG_MARGIN                30

#if wxPG_NO_CHILD_EVT_MOTION
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#else
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#endif

// Use this macro to generate standard custom image height from
#define wxPG_STD_CUST_IMAGE_HEIGHT(LINEHEIGHT)  (LINEHEIGHT-3)


#if defined(__WXWINCE__)
    #define wxPG_SMALL_SCREEN       1
    #undef wxPG_DOUBLE_BUFFER
    #define wxPG_DOUBLE_BUFFER      0
#else
    #define wxPG_SMALL_SCREEN       0
#endif

#endif // #ifndef SWIG

// Undefine wxPG_ICON_WIDTH to use supplied xpm bitmaps instead
// (for tree buttons)
//#undef wxPG_ICON_WIDTH

// Need to force disable tooltips?
#if !wxUSE_TOOLTIPS
    #undef wxPG_SUPPORT_TOOLTIPS
    #define wxPG_SUPPORT_TOOLTIPS       0
#endif

// Set 1 to include advanced properties (wxFontProperty, wxColourProperty, etc.)
#ifndef wxPG_INCLUDE_ADVPROPS
    #define wxPG_INCLUDE_ADVPROPS           1
#endif

// Set 1 to include checkbox editor class
#define wxPG_INCLUDE_CHECKBOX           1

// -----------------------------------------------------------------------


// wxPG_CHECK_FOO_DBG - debug check with message that does not
//                      interrupt program flow.
#define wxPG_CHECK_RET_DBG(A,B) \
    if ( !(A) ) { wxLogWarning(B); return; }
#define wxPG_CHECK_MSG_DBG(A,B,C) \
    if ( !(A) ) { wxLogWarning(C); return B; }


class wxPGEditor;
class wxPGProperty;
class wxPropertyCategory;
class wxPGChoices;
class wxPropertyGridPageState;
class wxPGCell;
class wxPGCellRenderer;
class wxPGChoiceEntry;
class wxPGPropArgCls;
class wxPropertyGridInterface;
class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPropertyGridManager;
class wxPGOwnerDrawnComboBox;
class wxPGCustomComboControl;
class wxPGEditorDialogAdapter;
struct wxPGValidationInfo;


// -----------------------------------------------------------------------

/** @section propgrid_misc wxPropertyGrid Miscellanous

    This section describes some miscellanous values, types and macros.
    @{
*/

#ifndef SWIG
    // Used to tell wxPGProperty to use label as name as well
    #define wxPG_LABEL              (*wxPGProperty::sm_wxPG_LABEL)
    // This is the value placed in wxPGProperty::sm_wxPG_LABEL
    #define wxPG_LABEL_STRING       wxS("@!")
    #define wxPG_NULL_BITMAP        wxNullBitmap
    #define wxPG_COLOUR_BLACK       (*wxBLACK)
#else
    #define wxPG_NULL_BITMAP        wxBitmap_NULL
    #define wxPG_COLOUR_BLACK       wxColour_BLACK
#endif // #ifndef SWIG

/** Convert Red, Green and Blue to a single 32-bit value.
*/
#define wxPG_COLOUR(R,G,B) ((wxUint32)(R+(G<<8)+(B<<16)))


/** If property is supposed to have custom-painted image, then returning
    this in OnMeasureImage() will usually be enough.
*/
#define wxPG_DEFAULT_IMAGE_SIZE             wxSize(-1, -1)


typedef wxString wxPGCachedString;

/** @}
*/

// -----------------------------------------------------------------------

// Used to indicate wxPGChoices::Add etc that the value is actually not given
// by the caller.
#define wxPG_INVALID_VALUE      INT_MAX

// -----------------------------------------------------------------------

#ifndef SWIG

#if wxUSE_STL
typedef WXDLLIMPEXP_PROPGRID std::vector<wxPGProperty*>  wxArrayPGProperty;
#else
WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(wxPGProperty*, wxArrayPGProperty,
                                  wxBaseArrayPtrVoid,
                                  class WXDLLIMPEXP_PROPGRID);
#endif

// Always use wxString based hashmap with unicode, stl, swig and GCC 4.0+
WX_DECLARE_STRING_HASH_MAP_WITH_DECL(void*,
                                     wxPGHashMapS2P,
                                     class WXDLLIMPEXP_PROPGRID);


WX_DECLARE_VOIDPTR_HASH_MAP_WITH_DECL(void*,
                                      wxPGHashMapP2P,
                                      class WXDLLIMPEXP_PROPGRID);

WX_DECLARE_HASH_MAP_WITH_DECL(wxInt32,
                              wxInt32,
                              wxIntegerHash,
                              wxIntegerEqual,
                              wxPGHashMapI2I,
                              class WXDLLIMPEXP_PROPGRID);

#endif // #ifndef SWIG

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
wxPG_FORCE                        = 0x00000100

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

    // Means property for which final string value is for can not really be
    // edited.
    wxPG_UNEDITABLE_COMPOSITE_FRAGMENT  = 0x00000020
};

// -----------------------------------------------------------------------

// wxPGProperty::SetValue() flags
enum wxPG_SETVALUE_FLAGS
{
    wxPG_SETVAL_REFRESH_EDITOR      = 0x0001,
    wxPG_SETVAL_AGGREGATED          = 0x0002,
    wxPG_SETVAL_FROM_PARENT         = 0x0004
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
// Editor class.

#ifndef SWIG

// Editor accessor.
#define wxPG_EDITOR(T)          wxPGEditor_##T

// Declare editor class, with optional part.
#define WX_PG_DECLARE_EDITOR_WITH_DECL(EDITOR,DECL) \
extern DECL wxPGEditor* wxPGEditor_##EDITOR; \
extern DECL wxPGEditor* wxPGConstruct##EDITOR##EditorClass();

// Declare editor class.
#define WX_PG_DECLARE_EDITOR(EDITOR) \
extern wxPGEditor* wxPGEditor_##EDITOR; \
extern wxPGEditor* wxPGConstruct##EDITOR##EditorClass();

// Declare builtin editor classes.
WX_PG_DECLARE_EDITOR_WITH_DECL(TextCtrl,WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_EDITOR_WITH_DECL(Choice,WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_EDITOR_WITH_DECL(ComboBox,WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_EDITOR_WITH_DECL(TextCtrlAndButton,WXDLLIMPEXP_PROPGRID)
#if wxPG_INCLUDE_CHECKBOX
WX_PG_DECLARE_EDITOR_WITH_DECL(CheckBox,WXDLLIMPEXP_PROPGRID)
#endif
WX_PG_DECLARE_EDITOR_WITH_DECL(ChoiceAndButton,WXDLLIMPEXP_PROPGRID)

#endif  // !SWIG

// -----------------------------------------------------------------------

#ifndef SWIG

/** @class wxPGVariantData
    @ingroup classes
    wxVariantData with additional functionality.

    It is usually enough to use supplied to macros to automatically generate
    variant data class. Like so:

    @code

    // In header
    WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataMyClass,  // Name of the class
                               MyClass,  // Name of the data type
                               wxEMPTY_PARAMETER_VALUE)  // Declaration

    // In source
    WX_PG_IMPLEMENT_VARIANT_DATA(wxPGVariantDataMyClass, MyClass)

    @endcode

    If your class is derived from wxObject, it is recommended that you use
    wxObject-versions of the macros (WX_PG_DECLARE_WXOBJECT_VARIANT_DATA and
    WX_PG_IMPLEMENT_WXOBJECT_VARIANT_DATA).

*/
class WXDLLIMPEXP_PROPGRID wxPGVariantData : public wxVariantData
{
public:
    virtual void* GetValuePtr() = 0;
    virtual wxVariant GetDefaultValue() const = 0;
protected:
};


//
// With wxWidgets 2.9 and later we demand native C++ RTTI support
#ifdef wxNO_RTTI
    #error "You need to enable compiler RTTI support for wxPropertyGrid"
#endif
#define WX_PG_DECLARE_DYNAMIC_CLASS_VARIANTDATA(A)
#define WX_PG_IMPLEMENT_DYNAMIC_CLASS_VARIANTDATA(A, B)
typedef const std::type_info* wxPGVariantDataClassInfo;
#define wxPGVariantDataGetClassInfo(A) (&typeid(*A))
#define wxPG_VARIANT_EQ(A, B) \
    ( ((A).GetData() && \
       (B).GetData() && typeid(*(A).GetData()) == typeid(*(B).GetData()) && \
       (A == B)) || !((A).GetData() && (B).GetData()) )

#ifndef wxDynamicCastVariantData
    #define wxDynamicCastVariantData wxDynamicCast
#endif


inline void wxPGDoesNothing() {}


#define _WX_PG_IMPLEMENT_VARIANT_DATA_CLASS(CLASSNAME, \
                                            DATATYPE, \
                                            AS_ARG, \
                                            AS_ARG_CONST, \
                                            CTOR_CODE, \
                                            DEFAULT_VALUE, \
                                            SET_CODE) \
    WX_PG_DECLARE_DYNAMIC_CLASS_VARIANTDATA(CLASSNAME) \
protected: \
    DATATYPE m_value; \
public: \
    CLASSNAME() { CTOR_CODE; } \
    CLASSNAME(AS_ARG_CONST value) { CTOR_CODE; SET_CODE; m_value = value; } \
    DATATYPE GetValue() const { return m_value; } \
    AS_ARG_CONST GetValueRef() const { return m_value; } \
    AS_ARG GetValueRef() { return m_value; } \
    void SetValue(AS_ARG_CONST value) { SET_CODE; m_value = value; } \
    virtual bool Eq(wxVariantData&) const { return false; } \
    virtual wxString GetType() const { return wxS(#DATATYPE); } \
    virtual wxVariantData* Clone() { return new CLASSNAME; } \
    virtual bool Read(wxString &) { return false; } \
    virtual bool Write(wxString &) const { return true; } \
    virtual wxVariant GetDefaultValue() const { return DEFAULT_VALUE; }

//
// Macro WXVARIANT allows creation of wxVariant from any type supported by
// wxWidgets internally, and of all types created using
// WX_PG_DECLARE_VARIANT_DATA.
template<class T>
wxVariant WXVARIANT( const T& value )
{
    return wxVariant((void*)&value);
}

template<> inline wxVariant WXVARIANT( const int& value )
    { return wxVariant((long)value); }
template<> inline wxVariant WXVARIANT( const long& value )
    { return wxVariant(value); }
template<> inline wxVariant WXVARIANT( const bool& value )
    { return wxVariant(value); }
template<> inline wxVariant WXVARIANT( const double& value )
    { return wxVariant(value); }
template<> inline wxVariant WXVARIANT( const wxArrayString& value )
    { return wxVariant(value); }
template<> inline wxVariant WXVARIANT( const wxString& value )
    { return wxVariant(value); }
#if wxUSE_DATETIME
template<> inline wxVariant WXVARIANT( const wxDateTime& value )
    { return wxVariant(value); }
#endif

#define _WX_PG_VARIANT_DATA_CLASSINFO_CONTAINER_DECL(CLASSNAME) \
    extern int CLASSNAME##_d_;
#define _WX_PG_VARIANT_DATA_CLASSINFO_CONTAINER(CLASSNAME) \
    int CLASSNAME##_d_;

#define _WX_PG_IMPLEMENT_VARIANT_DATA(CLASSNAME, \
                                      DATATYPE, \
                                      AS_ARG, \
                                      AS_CONST_ARG, \
                                      NULLVAL, \
                                      BASECLASS) \
_WX_PG_VARIANT_DATA_CLASSINFO_CONTAINER(CLASSNAME) \
WX_PG_IMPLEMENT_DYNAMIC_CLASS_VARIANTDATA(CLASSNAME, BASECLASS) \
AS_ARG operator <<( AS_ARG value, const wxVariant &variant ) \
{ \
    CLASSNAME *data = wxDynamicCastVariantData( variant.GetData(), CLASSNAME ); \
    wxASSERT( data ); \
    value = data->GetValue(); \
    return value; \
} \
wxVariant& operator <<( wxVariant &variant, AS_CONST_ARG value ) \
{ \
    CLASSNAME *data = new CLASSNAME( value ); \
    variant.SetData( data ); \
    return variant; \
} \
AS_ARG DATATYPE##FromVariant( const wxVariant& v ) \
{ \
    CLASSNAME *data = wxDynamicCastVariantData( v.GetData(), CLASSNAME ); \
    if ( !data ) \
        return NULLVAL; \
    return data->GetValueRef(); \
} \
wxVariant DATATYPE##ToVariant( AS_CONST_ARG value ) \
{ \
    wxVariant variant( new CLASSNAME( value ) ); \
    return variant; \
}

#define WX_PG_IMPLEMENT_VARIANT_DATA(CLASSNAME, DATATYPE) \
class CLASSNAME : public wxPGVariantData \
{ \
_WX_PG_IMPLEMENT_VARIANT_DATA_CLASS(CLASSNAME, DATATYPE, DATATYPE&, \
                                    const DATATYPE&, wxPGDoesNothing(), \
                                    wxVariant(new CLASSNAME(DATATYPE())), \
                                    wxPGDoesNothing()) \
public: \
    virtual void* GetValuePtr() { return (void*)&m_value; } \
}; \
_WX_PG_IMPLEMENT_VARIANT_DATA(CLASSNAME, \
                              DATATYPE, \
                              DATATYPE&, \
                              const DATATYPE&, \
                              (DATATYPE&)*((DATATYPE*)NULL), \
                              wxPGVariantData)

#define WX_PG_IMPLEMENT_WXOBJECT_VARIANT_DATA(CLASSNAME, DATATYPE) \
class CLASSNAME : public wxPGVariantData \
{ \
_WX_PG_IMPLEMENT_VARIANT_DATA_CLASS(CLASSNAME, DATATYPE, DATATYPE&, \
                                    const DATATYPE&, wxPGDoesNothing(), \
                                    wxVariant(new CLASSNAME(DATATYPE())), \
                                    wxPGDoesNothing()) \
public: \
    virtual void* GetValuePtr() { return (void*)&m_value; } \
    virtual wxClassInfo* GetValueClassInfo() \
        { return m_value.GetClassInfo(); } \
}; \
_WX_PG_IMPLEMENT_VARIANT_DATA(CLASSNAME, DATATYPE, DATATYPE&, \
                              const DATATYPE&, \
                              (DATATYPE&)*((DATATYPE*)NULL), \
                              wxPGVariantData)


#define WX_PG_DECLARE_VARIANT_DATA(CLASSNAME, DATATYPE, DECL) \
DECL DATATYPE& operator <<( DATATYPE& value, const wxVariant &variant ); \
DECL wxVariant& operator <<( wxVariant &variant, const DATATYPE& value ); \
DECL DATATYPE& DATATYPE##FromVariant( const wxVariant& variant ); \
DECL wxVariant DATATYPE##ToVariant( const DATATYPE& value ); \
template<> inline wxVariant WXVARIANT( const DATATYPE& value ) \
    { return DATATYPE##ToVariant(value); } \
DECL _WX_PG_VARIANT_DATA_CLASSINFO_CONTAINER_DECL(CLASSNAME);

#define WX_PG_DECLARE_WXOBJECT_VARIANT_DATA WX_PG_DECLARE_VARIANT_DATA

#define WX_PG_DECLARE_PTR_VARIANT_DATA(CLASSNAME, DATATYPE, DECL) \
DECL DATATYPE* operator <<( DATATYPE* value, const wxVariant &variant ); \
DECL wxVariant& operator <<( wxVariant &variant, DATATYPE* value ); \
DECL DATATYPE* DATATYPE##FromVariant( const wxVariant& variant ); \
DECL wxVariant DATATYPE##ToVariant( DATATYPE* value ); \
DECL _WX_PG_VARIANT_DATA_CLASSINFO_CONTAINER_DECL(CLASSNAME);


#define WX_PG_IMPLEMENT_PTR_VARIANT_DATA(CLASSNAME, DATATYPE, DEFAULT) \
class CLASSNAME : public wxPGVariantData \
{ \
_WX_PG_IMPLEMENT_VARIANT_DATA_CLASS(CLASSNAME, DATATYPE*, DATATYPE*, \
                                    DATATYPE*, m_value = NULL, \
                                    DEFAULT, \
                                    if (m_value) Py_DECREF(m_value); \
                                    if (!value) value = Py_None; \
                                    Py_INCREF(value) ) \
    ~CLASSNAME() { if (m_value) Py_DECREF(m_value); } \
public: \
    virtual void* GetValuePtr() { return (void*)m_value; } \
}; \
_WX_PG_IMPLEMENT_VARIANT_DATA(CLASSNAME, DATATYPE, DATATYPE*, DATATYPE*, \
                              NULL, wxPGVariantData)


WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataPoint, wxPoint, WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataSize, wxSize, WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataArrayInt,
                           wxArrayInt, WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataLongLong,
                           wxLongLong, WXDLLIMPEXP_PROPGRID)
WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataULongLong,
                           wxULongLong, WXDLLIMPEXP_PROPGRID)

WX_PG_DECLARE_WXOBJECT_VARIANT_DATA(wxPGVariantDataFont,
                                    wxFont, WXDLLIMPEXP_PROPGRID)
template<> inline wxVariant WXVARIANT( const wxColour& value )
{
    wxVariant variant;
    variant << value;
    return variant;
}

#define WX_PG_VARIANT_GETVALUEREF(P, T) \
    (*((T*)((wxPGVariantData*)(P.GetData()))->GetValuePtr()))

// Safely converts a wxVariant to (long) int. Supports converting from string
// and boolean as well.
WXDLLIMPEXP_PROPGRID
long wxPGVariantToInt( const wxVariant& variant, long defVal = 1 );

// Safely converts a wxVariant to wxLongLong_t. Returns true on success.
WXDLLIMPEXP_PROPGRID
bool wxPGVariantToLongLong( const wxVariant& variant, wxLongLong_t* pResult );

// Safely converts a wxVariant to wxULongLong_t. Returns true on success.
WXDLLIMPEXP_PROPGRID
bool wxPGVariantToULongLong( const wxVariant& variant, wxULongLong_t* pResult );

// Safely converts a wxVariant to double. Supports converting from string and
// wxLongLong as well.
WXDLLIMPEXP_PROPGRID
bool wxPGVariantToDouble( const wxVariant& variant, double* pResult );

#endif // !SWIG

// -----------------------------------------------------------------------

#ifndef SWIG

WXDLLIMPEXP_PROPGRID
wxObject*
wxPG_VariantToWxObject( const wxVariant& variant, wxClassInfo* classInfo );

//
// Redefine wxGetVariantCast to also take propertygrid variantdata
// classes into account.
//
#undef wxGetVariantCast
#define wxGetVariantCast(var,classname) \
    (classname*)wxPG_VariantToWxObject(var,&classname::ms_classInfo)

// TODO: After a while, remove this.
#define WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) \
    (CLASSNAME*)wxPG_VariantToWxObject(VARIANT,&CLASSNAME::ms_classInfo)

#endif // !SWIG

// -----------------------------------------------------------------------

#ifndef SWIG

//
// Tokenizer macros.
// NOTE: I have made two versions - worse ones (performance and consistency
//   wise) use wxStringTokenizer and better ones (may have unfound bugs)
//   use custom code.
//

#include "wx/tokenzr.h"

// TOKENIZER1 can be done with wxStringTokenizer
#define WX_PG_TOKENIZER1_BEGIN(WXSTRING,DELIMITER) \
    wxStringTokenizer tkz(WXSTRING,DELIMITER,wxTOKEN_RET_EMPTY); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken(); \
        token.Trim(true); \
        token.Trim(false);

#define WX_PG_TOKENIZER1_END() \
    }


//
// 2nd version: tokens are surrounded by DELIMITERs (for example, C-style
// strings). TOKENIZER2 must use custom code (a class) for full compliance with
// " surrounded strings with \" inside.
//
// class implementation is in propgrid.cpp
//

class WXDLLIMPEXP_PROPGRID wxPGStringTokenizer
{
public:
    wxPGStringTokenizer( const wxString& str, wxChar delimeter );
    ~wxPGStringTokenizer();

    bool HasMoreTokens(); // not const so we can do some stuff in it
    wxString GetNextToken();

protected:
    const wxString*             m_str;
    wxString::const_iterator    m_curPos;
    wxString                    m_readyToken;
    wxUniChar                   m_delimeter;
};

#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    wxPGStringTokenizer tkz(WXSTRING,DELIMITER); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken();

#define WX_PG_TOKENIZER2_END() \
    }

#endif

// -----------------------------------------------------------------------

#endif // _WX_PROPGRID_PROPGRIDDEFS_H_
