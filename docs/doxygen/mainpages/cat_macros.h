/////////////////////////////////////////////////////////////////////////////
// Name:        cat_macros.h
// Purpose:     Macros-by-category page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page page_macro_cat Macros by Category

@li @ref page_macro_cat_version
@li @ref page_macro_cat_byteorder
@li @ref page_macro_cat_rtti
@li @ref page_macro_cat_debugging
@li @ref page_macro_cat_misc


<hr>


@section page_macro_cat_version Versioning

The following constants are defined in wxWidgets:

@li wxMAJOR_VERSION is the major version of wxWidgets
@li wxMINOR_VERSION is the minor version of wxWidgets
@li wxRELEASE_NUMBER is the release number
@li wxSUBRELEASE_NUMBER is the subrelease number which is @c 0 for all
    official releases

For example, the values or these constants for wxWidgets 2.8.7
are 2, 8, 7 and 0.

Additionally, wxVERSION_STRING is a user-readable string containing the full
wxWidgets version and wxVERSION_NUMBER is a combination of the three version
numbers above: for 2.1.15, it is 2115 and it is 2200 for wxWidgets 2.2.

The subrelease number is only used for the sources in between official releases
and so normally is not useful.

@header{wx/version.h}
@header{wx/defs.h}

@li wxCHECK_GCC_VERSION
@li wxCHECK_SUNCC_VERSION
@li wxCHECK_VERSION
@li wxCHECK_VERSION_FULL
@li wxCHECK_VISUALC_VERSION
@li wxCHECK_W32API_VERSION


@section page_macro_cat_misc Miscellaneous

@header{FIXME}

@li wxCONCAT
@li wxDECLARE_APP
@li wxDYNLIB_FUNCTION
@li wxDEPRECATED
@li wxDEPRECATED_BUT_USED_INTERNALLY
@li wxDEPRECATED_INLINE
@li wxEXPLICIT
@li wxON_BLOCK_EXIT
@li wxON_BLOCK_EXIT_OBJ
@li wxSTRINGIZE
@li wxSTRINGIZE_T
@li wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING
@li __WXFUNCTION__
@li wxS
@li wxT
@li wxTRANSLATE
@li _
@li wxPLURAL
@li _T
@li WXTRACE
@li WXTRACELEVEL


@section page_macro_cat_byteorder Byte Order

@header{FIXME}

The endian-ness issues (that is the difference between big-endian and
little-endian architectures) are important for the portable programs working
with the external binary data (for example, data files or data coming from
network) which is usually in some fixed, platform-independent format.

The macros are helpful for transforming the data to the correct format.

@li wxINTXX_SWAP_ALWAYS
@li wxINTXX_SWAP_ON_BE
@li wxINTXX_SWAP_ON_LE
@li wxFORCE_LINK_THIS_MODULE
@li wxFORCE_LINK_MODULE
@li wxIMPLEMENT_APP


@section page_macro_cat_rtti Runtime Type Information (RTTI)

wxWidgets uses its own RTTI ("run-time type identification") system which
predates the current standard C++ RTTI and so is kept for backwards
compatibility reasons but also because it allows some things which the standard
RTTI doesn't directly support (such as creating a class from its name). The
standard C++ RTTI can be used in the user code without any problems and in
general you shouldn't need to use the functions and the macros in this section
unless you are thinking of modifying or adding any wxWidgets classes.

Related Overviews: @ref overview_rtti

@li #CLASSINFO
@li #DECLARE_ABSTRACT_CLASS
@li #DECLARE_APP
@li #DECLARE_CLASS
@li #DECLARE_DYNAMIC_CLASS
@li #IMPLEMENT_ABSTRACT_CLASS
@li #IMPLEMENT_ABSTRACT_CLASS2
@li #IMPLEMENT_APP
@li #IMPLEMENT_CLASS
@li #IMPLEMENT_CLASS2
@li #IMPLEMENT_DYNAMIC_CLASS
@li #IMPLEMENT_DYNAMIC_CLASS2
@li #wxConstCast
@li ::wxCreateDynamicObject
@li #WXDEBUG_NEW
@li #wxDynamicCast
@li #wxDynamicCastThis
@li #wxStaticCast
@li #wx_const_cast
@li #wx_reinterpret_cast
@li #wx_static_cast
@li #wx_truncate_cast


@section page_macro_cat_debugging Debugging

Useful macros and functions for error checking and defensive programming.
wxWidgets defines three families of the assert-like macros: the wxASSERT and
wxFAIL macros only do anything if __WXDEBUG__ is defined (in other words, in
the debug build) but disappear completely in the release build. On the other
hand, the wxCHECK macros stay event in release builds but a check failure
doesn't generate any user-visible effects then. Finally, the compile time
assertions don't happen during the run-time but result in the compilation error
messages if the condition they check fail.

@header{wx/debug.h}

@li wxASSERT
@li wxASSERT_MIN_BITSIZE
@li wxASSERT_MSG
@li wxCOMPILE_TIME_ASSERT
@li wxCOMPILE_TIME_ASSERT2
@li wxFAIL
@li wxFAIL_MSG
@li wxCHECK
@li wxCHECK_MSG
@li wxCHECK_RET
@li wxCHECK2
@li wxCHECK2_MSG

*/

