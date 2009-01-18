/////////////////////////////////////////////////////////////////////////////
// Name:        wx/defs.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    No id matches this one when compared to it.
*/
int wxID_NONE = -3;

/**
    Id for a separator line in the menu (invalid for normal item).
*/
int wxID_SEPARATOR = -2;

/**
    Any id: means that we don't care about the id, whether when installing
    an event handler or when creating a new window.
*/
int wxID_ANY = -1;

/**
    Item kinds for use with wxMenu, wxMenuItem, and wxToolBar.

    @see wxMenu::Append(), wxMenuItem::wxMenuItem(), wxToolBar::AddTool()
*/
enum wxItemKind
{
    wxITEM_SEPARATOR = -1,

    /**
        Normal tool button / menu item.

        @see wxToolBar::AddTool(), wxMenu::AppendItem().
    */
    wxITEM_NORMAL,

    /**
        Check (or toggle) tool button / menu item.

        @see wxToolBar::AddCheckTool(), wxMenu::AppendCheckItem().
    */
    wxITEM_CHECK,

    /**
        Radio tool button / menu item.

        @see wxToolBar::AddRadioTool(), wxMenu::AppendRadioItem().
    */
    wxITEM_RADIO,

    /**
        Normal tool button with a dropdown arrow next to it. Clicking the
        dropdown arrow sends a @c wxEVT_COMMAND_TOOL_DROPDOWN_CLICKED event and may
        also display the menu previously associated with the item with
        wxToolBar::SetDropdownMenu(). Currently this type of tools is supported
        under MSW and GTK.
    */
    wxITEM_DROPDOWN,

    wxITEM_MAX
};


/**
    Paper size types for use with the printing framework.

    @see overview_printing, wxPrintData::SetPaperId()
*/
enum wxPaperSize
{
    wxPAPER_NONE,               ///<  Use specific dimensions
    wxPAPER_LETTER,             ///<  Letter, 8 1/2 by 11 inches
    wxPAPER_LEGAL,              ///<  Legal, 8 1/2 by 14 inches
    wxPAPER_A4,                 ///<  A4 Sheet, 210 by 297 millimeters
    wxPAPER_CSHEET,             ///<  C Sheet, 17 by 22 inches
    wxPAPER_DSHEET,             ///<  D Sheet, 22 by 34 inches
    wxPAPER_ESHEET,             ///<  E Sheet, 34 by 44 inches
    wxPAPER_LETTERSMALL,        ///<  Letter Small, 8 1/2 by 11 inches
    wxPAPER_TABLOID,            ///<  Tabloid, 11 by 17 inches
    wxPAPER_LEDGER,             ///<  Ledger, 17 by 11 inches
    wxPAPER_STATEMENT,          ///<  Statement, 5 1/2 by 8 1/2 inches
    wxPAPER_EXECUTIVE,          ///<  Executive, 7 1/4 by 10 1/2 inches
    wxPAPER_A3,                 ///<  A3 sheet, 297 by 420 millimeters
    wxPAPER_A4SMALL,            ///<  A4 small sheet, 210 by 297 millimeters
    wxPAPER_A5,                 ///<  A5 sheet, 148 by 210 millimeters
    wxPAPER_B4,                 ///<  B4 sheet, 250 by 354 millimeters
    wxPAPER_B5,                 ///<  B5 sheet, 182-by-257-millimeter paper
    wxPAPER_FOLIO,              ///<  Folio, 8-1/2-by-13-inch paper
    wxPAPER_QUARTO,             ///<  Quarto, 215-by-275-millimeter paper
    wxPAPER_10X14,              ///<  10-by-14-inch sheet
    wxPAPER_11X17,              ///<  11-by-17-inch sheet
    wxPAPER_NOTE,               ///<  Note, 8 1/2 by 11 inches
    wxPAPER_ENV_9,              ///<  #9 Envelope, 3 7/8 by 8 7/8 inches
    wxPAPER_ENV_10,             ///<  #10 Envelope, 4 1/8 by 9 1/2 inches
    wxPAPER_ENV_11,             ///<  #11 Envelope, 4 1/2 by 10 3/8 inches
    wxPAPER_ENV_12,             ///<  #12 Envelope, 4 3/4 by 11 inches
    wxPAPER_ENV_14,             ///<  #14 Envelope, 5 by 11 1/2 inches
    wxPAPER_ENV_DL,             ///<  DL Envelope, 110 by 220 millimeters
    wxPAPER_ENV_C5,             ///<  C5 Envelope, 162 by 229 millimeters
    wxPAPER_ENV_C3,             ///<  C3 Envelope, 324 by 458 millimeters
    wxPAPER_ENV_C4,             ///<  C4 Envelope, 229 by 324 millimeters
    wxPAPER_ENV_C6,             ///<  C6 Envelope, 114 by 162 millimeters
    wxPAPER_ENV_C65,            ///<  C65 Envelope, 114 by 229 millimeters
    wxPAPER_ENV_B4,             ///<  B4 Envelope, 250 by 353 millimeters
    wxPAPER_ENV_B5,             ///<  B5 Envelope, 176 by 250 millimeters
    wxPAPER_ENV_B6,             ///<  B6 Envelope, 176 by 125 millimeters
    wxPAPER_ENV_ITALY,          ///<  Italy Envelope, 110 by 230 millimeters
    wxPAPER_ENV_MONARCH,        ///<  Monarch Envelope, 3 7/8 by 7 1/2 inches
    wxPAPER_ENV_PERSONAL,       ///<  6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    wxPAPER_FANFOLD_US,         ///<  US Std Fanfold, 14 7/8 by 11 inches
    wxPAPER_FANFOLD_STD_GERMAN, ///<  German Std Fanfold, 8 1/2 by 12 inches
    wxPAPER_FANFOLD_LGL_GERMAN, ///<  German Legal Fanfold, 8 1/2 by 13 inches

    // wxMSW Only

    wxPAPER_ISO_B4,             ///<  B4 (ISO) 250 x 353 mm
    wxPAPER_JAPANESE_POSTCARD,  ///<  Japanese Postcard 100 x 148 mm
    wxPAPER_9X11,               ///<  9 x 11 in
    wxPAPER_10X11,              ///<  10 x 11 in
    wxPAPER_15X11,              ///<  15 x 11 in
    wxPAPER_ENV_INVITE,         ///<  Envelope Invite 220 x 220 mm
    wxPAPER_LETTER_EXTRA,       ///<  Letter Extra 9.5 x 12 in
    wxPAPER_LEGAL_EXTRA,        ///<  Legal Extra 9.5 x 15 in
    wxPAPER_TABLOID_EXTRA,      ///<  Tabloid Extra 11.69 x 18 in
    wxPAPER_A4_EXTRA,           ///<  A4 Extra 9.27 x 12.69 in
    wxPAPER_LETTER_TRANSVERSE,  ///<  Letter Transverse 8.5 x 11 in
    wxPAPER_A4_TRANSVERSE,      ///<  A4 Transverse 210 x 297 mm
    wxPAPER_LETTER_EXTRA_TRANSVERSE, ///<  Letter Extra Transverse 9.5 x 12 in
    wxPAPER_A_PLUS,             ///<  SuperA/SuperA/A4 227 x 356 mm
    wxPAPER_B_PLUS,             ///<  SuperB/SuperB/A3 305 x 487 mm
    wxPAPER_LETTER_PLUS,        ///<  Letter Plus 8.5 x 12.69 in
    wxPAPER_A4_PLUS,            ///<  A4 Plus 210 x 330 mm
    wxPAPER_A5_TRANSVERSE,      ///<  A5 Transverse 148 x 210 mm
    wxPAPER_B5_TRANSVERSE,      ///<  B5 (JIS) Transverse 182 x 257 mm
    wxPAPER_A3_EXTRA,           ///<  A3 Extra 322 x 445 mm
    wxPAPER_A5_EXTRA,           ///<  A5 Extra 174 x 235 mm
    wxPAPER_B5_EXTRA,           ///<  B5 (ISO) Extra 201 x 276 mm
    wxPAPER_A2,                 ///<  A2 420 x 594 mm
    wxPAPER_A3_TRANSVERSE,      ///<  A3 Transverse 297 x 420 mm
    wxPAPER_A3_EXTRA_TRANSVERSE, ///<  A3 Extra Transverse 322 x 445 mm

    wxPAPER_DBL_JAPANESE_POSTCARD, ///< Japanese Double Postcard 200 x 148 mm
    wxPAPER_A6,                 ///< A6 105 x 148 mm
    wxPAPER_JENV_KAKU2,         ///< Japanese Envelope Kaku #2
    wxPAPER_JENV_KAKU3,         ///< Japanese Envelope Kaku #3
    wxPAPER_JENV_CHOU3,         ///< Japanese Envelope Chou #3
    wxPAPER_JENV_CHOU4,         ///< Japanese Envelope Chou #4
    wxPAPER_LETTER_ROTATED,     ///< Letter Rotated 11 x 8 1/2 in
    wxPAPER_A3_ROTATED,         ///< A3 Rotated 420 x 297 mm
    wxPAPER_A4_ROTATED,         ///< A4 Rotated 297 x 210 mm
    wxPAPER_A5_ROTATED,         ///< A5 Rotated 210 x 148 mm
    wxPAPER_B4_JIS_ROTATED,     ///< B4 (JIS) Rotated 364 x 257 mm
    wxPAPER_B5_JIS_ROTATED,     ///< B5 (JIS) Rotated 257 x 182 mm
    wxPAPER_JAPANESE_POSTCARD_ROTATED, ///< Japanese Postcard Rotated 148 x 100 mm
    wxPAPER_DBL_JAPANESE_POSTCARD_ROTATED, ///< Double Japanese Postcard Rotated 148 x 200 mm
    wxPAPER_A6_ROTATED,         ///< A6 Rotated 148 x 105 mm
    wxPAPER_JENV_KAKU2_ROTATED, ///< Japanese Envelope Kaku #2 Rotated
    wxPAPER_JENV_KAKU3_ROTATED, ///< Japanese Envelope Kaku #3 Rotated
    wxPAPER_JENV_CHOU3_ROTATED, ///< Japanese Envelope Chou #3 Rotated
    wxPAPER_JENV_CHOU4_ROTATED, ///< Japanese Envelope Chou #4 Rotated
    wxPAPER_B6_JIS,             ///< B6 (JIS) 128 x 182 mm
    wxPAPER_B6_JIS_ROTATED,     ///< B6 (JIS) Rotated 182 x 128 mm
    wxPAPER_12X11,              ///< 12 x 11 in
    wxPAPER_JENV_YOU4,          ///< Japanese Envelope You #4
    wxPAPER_JENV_YOU4_ROTATED,  ///< Japanese Envelope You #4 Rotated
    wxPAPER_P16K,               ///< PRC 16K 146 x 215 mm
    wxPAPER_P32K,               ///< PRC 32K 97 x 151 mm
    wxPAPER_P32KBIG,            ///< PRC 32K(Big) 97 x 151 mm
    wxPAPER_PENV_1,             ///< PRC Envelope #1 102 x 165 mm
    wxPAPER_PENV_2,             ///< PRC Envelope #2 102 x 176 mm
    wxPAPER_PENV_3,             ///< PRC Envelope #3 125 x 176 mm
    wxPAPER_PENV_4,             ///< PRC Envelope #4 110 x 208 mm
    wxPAPER_PENV_5,             ///< PRC Envelope #5 110 x 220 mm
    wxPAPER_PENV_6,             ///< PRC Envelope #6 120 x 230 mm
    wxPAPER_PENV_7,             ///< PRC Envelope #7 160 x 230 mm
    wxPAPER_PENV_8,             ///< PRC Envelope #8 120 x 309 mm
    wxPAPER_PENV_9,             ///< PRC Envelope #9 229 x 324 mm
    wxPAPER_PENV_10,            ///< PRC Envelope #10 324 x 458 mm
    wxPAPER_P16K_ROTATED,       ///< PRC 16K Rotated
    wxPAPER_P32K_ROTATED,       ///< PRC 32K Rotated
    wxPAPER_P32KBIG_ROTATED,    ///< PRC 32K(Big) Rotated
    wxPAPER_PENV_1_ROTATED,     ///< PRC Envelope #1 Rotated 165 x 102 mm
    wxPAPER_PENV_2_ROTATED,     ///< PRC Envelope #2 Rotated 176 x 102 mm
    wxPAPER_PENV_3_ROTATED,     ///< PRC Envelope #3 Rotated 176 x 125 mm
    wxPAPER_PENV_4_ROTATED,     ///< PRC Envelope #4 Rotated 208 x 110 mm
    wxPAPER_PENV_5_ROTATED,     ///< PRC Envelope #5 Rotated 220 x 110 mm
    wxPAPER_PENV_6_ROTATED,     ///< PRC Envelope #6 Rotated 230 x 120 mm
    wxPAPER_PENV_7_ROTATED,     ///< PRC Envelope #7 Rotated 230 x 160 mm
    wxPAPER_PENV_8_ROTATED,     ///< PRC Envelope #8 Rotated 309 x 120 mm
    wxPAPER_PENV_9_ROTATED,     ///< PRC Envelope #9 Rotated 324 x 229 mm
    wxPAPER_PENV_10_ROTATED     ///< PRC Envelope #10 Rotated 458 x 324 m
};


/** @addtogroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa unconditionally, i.e. independently of the
    current platform.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ALWAYS( wxInt32_value )
#define wxUINT32_SWAP_ALWAYS( wxUint32_value )
#define wxINT16_SWAP_ALWAYS( wxInt16_value )
#define wxUINT16_SWAP_ALWAYS( wxUint16_value )

//@}

/** @addtogroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa if the program is compiled on a big-endian
    architecture (such as Sun work stations). If the program has been compiled
    on a little-endian architecture, the value will be unchanged.

    Use these macros to read data from and write data to a file that stores
    data in little-endian (for example Intel i386) format.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ON_BE( wxInt32_value )
#define wxUINT32_SWAP_ON_BE( wxUint32_value )
#define wxINT16_SWAP_ON_BE( wxInt16_value )
#define wxUINT16_SWAP_ON_BE( wxUint16_value )

//@}

/** @addtogroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa if the program is compiled on a little-endian
    architecture (such as Intel PCs). If the program has been compiled on a
    big-endian architecture, the value will be unchanged.

    Use these macros to read data from and write data to a file that stores
    data in big-endian format.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ON_LE( wxInt32_value )
#define wxUINT32_SWAP_ON_LE( wxUint32_value )
#define wxINT16_SWAP_ON_LE( wxInt16_value )
#define wxUINT16_SWAP_ON_LE( wxUint16_value )

//@}



/** @addtogroup group_funcmacro_misc */
//@{

/**
    This macro can be used in a class declaration to disable the generation of
    default assignment operator.

    Some classes have a well-defined copy constructor but cannot have an
    assignment operator, typically because they can't be modified once created.
    In such case, this macro can be used to disable the automatic assignment
    operator generation.

    @see DECLARE_NO_COPY_CLASS()
 */
#define DECLARE_NO_ASSIGN_CLASS(classname)

/**
    This macro can be used in a class declaration to disable the generation of
    default copy ctor and assignment operator.

    Some classes don't have a well-defined copying semantics. In this case the
    standard C++ convention is to not allow copying them. One way of achieving
    it is to use this macro which simply defines a private copy constructor and
    assignment operator.

    Beware that simply not defining copy constructor and assignment operator is
    @em not enough as the compiler would provide its own automatically-generated
    versions of them -- hence the usefulness of this macro.

    Example of use:
    @code
    class FooWidget
    {
    public:
        FooWidget();
        ...

    private:
        // widgets can't be copied
        DECLARE_NO_COPY_CLASS(FooWidget)
    };
    @endcode

    Notice that a semicolon should not be used after this macro and that it
    changes the access specifier to private internally so it is better to use
    it at the end of the class declaration.
 */
#define DECLARE_NO_COPY_CLASS(classname)

/**
    Equivalent of DECLARE_NO_COPY_CLASS() for template classes.

    This macro can be used for template classes (with a single template
    parameter) for the same purpose as DECLARE_NO_COPY_CLASS() is used with the
    non-template classes.

    @param classname The name of the template class.
    @param arg The name of the template parameter.
 */
#define DECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg)

/**
    A function which deletes and nulls the pointer.

    This function uses operator delete to free the pointer and also sets it to
    @NULL. Notice that this does @em not work for arrays, use wxDELETEA() for
    them.

    @code
        MyClass *ptr = new MyClass;
        ...
        wxDELETE(ptr);
        wxASSERT(!ptr);
    @endcode

    @header{wx/defs.h}
*/
template <typename T> wxDELETE(T*& ptr);

/**
    A function which deletes and nulls the pointer.

    This function uses vector operator delete (@c delete[]) to free the array
    pointer and also sets it to @NULL. Notice that this does @em not work for
    non-array pointers, use wxDELETE() for them.

    @code
        MyClass *array = new MyClass[17];
        ...
        wxDELETEA(array);
        wxASSERT(!array);
    @endcode

    @see wxDELETE()

    @header{wx/defs.h}
*/
template <typename T> wxDELETEA(T*& array);

/**
    This macro can be used around a function declaration to generate warnings
    indicating that this function is deprecated (i.e. obsolete and planned to
    be removed in the future) when it is used. Only Visual C++ 7 and higher and
    g++ compilers currently support this functionality.

    Example of use:

    @code
    // old function, use wxString version instead
    wxDEPRECATED( void wxGetSomething(char *buf, size_t len) );

    // ...
    wxString wxGetSomething();
    @endcode

    @header{wx/defs.h}
*/
#define wxDEPRECATED(function)

/**
    This is a special version of wxDEPRECATED() macro which only does something
    when the deprecated function is used from the code outside wxWidgets itself
    but doesn't generate warnings when it is used from wxWidgets.

    It is used with the virtual functions which are called by the library
    itself -- even if such function is deprecated the library still has to call
    it to ensure that the existing code overriding it continues to work, but
    the use of this macro ensures that a deprecation warning will be generated
    if this function is used from the user code or, in case of Visual C++, even
    when it is simply overridden.

    @header{wx/defs.h}
*/
#define wxDEPRECATED_BUT_USED_INTERNALLY(function)

/**
    This macro is similar to wxDEPRECATED() but can be used to not only declare
    the function @a function as deprecated but to also provide its (inline)
    implementation @a body.

    It can be used as following:

    @code
    class wxFoo
    {
    public:
        // OldMethod() is deprecated, use NewMethod() instead
        void NewMethod();
        wxDEPRECATED_INLINE( void OldMethod(), NewMethod() );
    };
    @endcode

    @header{wx/defs.h}
*/
#define wxDEPRECATED_INLINE(func, body)

/**
    @c wxEXPLICIT is a macro which expands to the C++ @c explicit keyword if
    the compiler supports it or nothing otherwise. Thus, it can be used even in
    the code which might have to be compiled with an old compiler without
    support for this language feature but still take advantage of it when it is
    available.

    @header{wx/defs.h}
*/
#define wxEXPLICIT

/**
    GNU C++ compiler gives a warning for any class whose destructor is private
    unless it has a friend. This warning may sometimes be useful but it doesn't
    make sense for reference counted class which always delete themselves
    (hence destructor should be private) but don't necessarily have any
    friends, so this macro is provided to disable the warning in such case. The
    @a name parameter should be the name of the class but is only used to
    construct a unique friend class name internally.

    Example of using the macro:

    @code
    class RefCounted
    {
    public:
        RefCounted() { m_nRef = 1; }
        void IncRef() { m_nRef++ ; }
        void DecRef() { if ( !--m_nRef ) delete this; }

    private:
        ~RefCounted() { }

        wxSUPPRESS_GCC_PRIVATE_DTOR(RefCounted)
    };
    @endcode

    Notice that there should be no semicolon after this macro.

    @header{wx/defs.h}
*/
#define wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING(name)

/**
    Swaps the contents of two variables.

    This is similar to std::swap() but can be used even on the platforms where
    the standard C++ library is not available (if you don't target such
    platforms, please use std::swap() instead).

    The function relies on type T being copy constructible and assignable.

    Example of use:
    @code
        int x = 3,
            y = 4;
        wxSwap(x, y);
        wxASSERT( x == 4 && y == 3 );
    @endcode
 */
template <typename T> wxSwap(T& first, T& second);

/**
    This macro is the same as the standard C99 @c va_copy for the compilers
    which support it or its replacement for those that don't. It must be used
    to preserve the value of a @c va_list object if you need to use it after
    passing it to another function because it can be modified by the latter.

    As with @c va_start, each call to @c wxVaCopy must have a matching
    @c va_end.

    @header{wx/defs.h}
*/
void wxVaCopy(va_list argptrDst, va_list argptrSrc);

//@}


