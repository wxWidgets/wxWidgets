/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/private.h
// Purpose:     Private wxPropertyGrid declarations
// Author:      Artur Wieczorek, Jaakko Salli
// Created:     2023-03-10
// Copyright:   (c) 2023 wxWidgets development team
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PRIVATE_H_
#define _WX_PROPGRID_PRIVATE_H_

#if !defined(WXBUILDING)
#error This header is intended for internal use
#endif // !WXBUILDING

#include "wx/defs.h"

// -----------------------------------------------------------------------

// space between vertical sides of a custom image
#define wxPG_CUSTOM_IMAGE_SPACINGY      1

// space between caption and selection rectangle,
#define wxPG_CAPRECTXMARGIN             2

// horizontally and vertically
#define wxPG_CAPRECTYMARGIN             1

// -----------------------------------------------------------------------

//
// Here are some platform dependent defines
// NOTE: More in propertygrid.cpp
//
// NB: Only define wxPG_TEXTCTRLXADJUST for platforms that do not
//     (yet) support wxTextEntry::SetMargins() for the left margin.

#if defined(__WXMSW__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // left margin can be set with wxTextEntry::SetMargins()
    #undef wxPG_TEXTCTRLXADJUST

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    // If 1 then controls are refreshed explicitly in a few places
    #define wxPG_REFRESH_CONTROLS 0

#elif defined(__WXGTK__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin can be set with wxTextEntry::SetMargins()
    #undef wxPG_TEXTCTRLXADJUST

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 1

#elif defined(__WXMAC__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin cannot be set with wxTextEntry::SetMargins()
    #define wxPG_TEXTCTRLXADJUST        1

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             11
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 0

#else // defaults

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin cannot be set with wxTextEntry::SetMargins()
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    0

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 0
#endif // platform


#define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#define wxCC_CUSTOM_IMAGE_MARGIN1       4  // before image
#define wxCC_CUSTOM_IMAGE_MARGIN2       5  // after image

#define DEFAULT_IMAGE_OFFSET_INCREMENT \
    (wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2)

#define wxPG_DRAG_MARGIN                30

#if wxPG_NO_CHILD_EVT_MOTION
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#else
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#endif

// Use this macro to generate standard custom image height from
#define wxPG_STD_CUST_IMAGE_HEIGHT(LINEHEIGHT)  ((LINEHEIGHT)-3)

// Undefine wxPG_ICON_WIDTH to use supplied xpm bitmaps instead
// (for tree buttons)
//#undef wxPG_ICON_WIDTH

// -----------------------------------------------------------------------

// Events used only internally
wxDECLARE_EVENT(wxEVT_PG_HSCROLL, wxPropertyGridEvent);
wxDECLARE_EVENT(wxEVT_PG_COLS_RESIZED, wxPropertyGridEvent);

// -----------------------------------------------------------------------

// Flags used only internally

// wxBoolProperty, wxFlagsProperty specific flags
constexpr wxPGPropertyFlags wxPG_PROP_USE_CHECKBOX = wxPG_PROP_RESERVED_1;
// DCC = Double Click Cycles
constexpr wxPGPropertyFlags wxPG_PROP_USE_DCC = wxPG_PROP_RESERVED_2;

// wxStringProperty flag
constexpr wxPGPropertyFlags wxPG_PROP_PASSWORD = wxPG_PROP_RESERVED_2;

#if !WXWIN_COMPATIBILITY_3_2
// wxColourProperty flag - if set, then match from list is searched for a custom colour.
constexpr wxPGPropertyFlags wxPG_PROP_TRANSLATE_CUSTOM = wxPG_PROP_RESERVED_1;

// wxCursorProperty, wxSystemColourProperty - If set, then selection of choices is static
// and should not be changed (i.e. returns nullptr in GetPropertyChoices).
constexpr wxPGPropertyFlags wxPG_PROP_STATIC_CHOICES = wxPG_PROP_RESERVED_1;

// wxSystemColourProperty - wxEnumProperty based classes cannot use wxPG_PROP_RESERVED_1
constexpr wxPGPropertyFlags wxPG_PROP_HIDE_CUSTOM_COLOUR = wxPG_PROP_RESERVED_2;
constexpr wxPGPropertyFlags wxPG_PROP_COLOUR_HAS_ALPHA = wxPG_PROP_RESERVED_3;

// wxFileProperty - if set, full path is shown in wxFileProperty.
constexpr wxPGPropertyFlags wxPG_PROP_SHOW_FULL_FILENAME = wxPG_PROP_RESERVED_1;

// wxLongStringProperty - flag used to mark that edit button
// should be enabled even in the read-only mode.
constexpr wxPGPropertyFlags wxPG_PROP_ACTIVE_BTN = wxPG_PROP_RESERVED_3;
#endif // !WXWIN_COMPATIBILITY_3_2

#endif // _WX_PROPGRID_PRIVATE_H_
