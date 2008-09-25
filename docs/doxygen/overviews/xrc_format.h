/////////////////////////////////////////////////////////////////////////////
// Name:        xrc_format.h
// Purpose:     XRC format specification
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*
    NOTE: to make doxygen happy about <custom-tags> we're forced to
          escape all < and > symbols which appear inside a doxygen comment
*/


/**

@page xrc_format XRC file format

Table of contents:
 - @ref xrc_format_overview
 - @ref xrc_format_root
 - @ref xrc_format_objects
    - @ref xrc_format_object
    - @ref xrc_format_object_ref
 - @ref xrc_format_datatypes
 - @ref xrc_format_windows
    - @ref xrc_format_std_props
    - @ref xrc_format_controls
 - @ref xrc_format_sizers
 - @ref xrc_format_other_objects
 - @ref xrc_format_platform
 - @ref xrc_format_extending
    - @ref xrc_format_extending_subclass
    - @ref xrc_format_extending_unknown
    - @ref xrc_format_extending_custom
 - @ref xrc_format_packed
 - @ref xrc_format_oldversions

This document describes the format of XRC resource files, as used by
wxXmlResource.


<hr>


@section xrc_format_overview Overview

XRC file is a XML file with all of its elements in the
@c http://www.wxwidgets.org/wxxrc namespace. For backward compatibility,
@c http://www.wxwindows.org/wxxrc namespace is accepted as well (and treated
as identical to @c http://www.wxwidgets.org/wxxrc), but it shouldn't be used
in new XRC files.

XRC file contains definitions for one or more @em objects -- typically
windows. The objects may themselves contain child objects.

Objects defined at the top level, under the
@ref xrc_format_root "root element", can be accessed using
wxXmlResource::LoadDialog() and other LoadXXX methods. They must have
@c name attribute that is used as LoadXXX's argument (see
@ref xrc_format_object for details).

Child objects are not directly accessible via wxXmlResource, they can only
be accessed using XRCCTRL().


@section xrc_format_root Root element: \<resource\>

The root element is always @c \<resource\>. It has one optional attribute, @c
version. If set, it specifies version of the file. In absence of @c version
attribute, the default is @c "0.0.0.0".

The version consists of four integers separated by periods.  The first three
components are major, minor and release number of the wxWidgets release when
the change was introduced, the last one is revision number and is 0 for the
first incompatible change in given wxWidgets release, 1 for the second and so
on.  The version changes only if there was an incompatible change introduced;
merely adding new kind of objects does not constitute incompatible change.

At the time of writing, the latest version is @c "2.5.3.0".

Note that even though @c version attribute is optional, it should always be
specified to take advantage of the latest capabilities:

@code
<?xml version="1.0"?>
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
    ...
</resource>
@endcode

@c \<resource\> may have arbitrary number of
@ref xrc_format_objects "object elements" as its children; they are referred
to as @em toplevel objects in the rest of this document. Unlike objects defined
deeper in the hierarchy, toplevel objects @em must have their @c name attribute
set and it must be set to a value unique among root's children.



@section xrc_format_objects Defining objects

@subsection xrc_format_object \<object\>

The @c \<object\> element represents a single object (typically a GUI element)
and it usually maps directly to a wxWidgets class instance. It has one
mandatory attribute, @c class, and optional @c name and @c subclass attributes.

The @c class attribute must always be present, it tells XRC what wxWidgets
object should be created and by which wxXmlResourceHandler.

@c name is the identifier used to identify the object. This name serves three
purposes:

 -# It is used by wxXmlResource's various LoadXXX() methods to find the
    resource by name passed as argument.
 -# wxWindow's name (see wxWindow::GetName()) is set to it.
 -# Numeric ID of a window or menu item is derived from the name.
    If the value represents an integer (in decimal notation), it is used for
    the numeric ID unmodified.  If it is one of the wxID_XXX literals defined
    by wxWidgets (see @ref page_stockitems), its respective value is used.
    Otherwise, the name is transformed into dynamically generated ID. See
    wxXmlResource::GetXRCID() for more information.

Name attributes must be unique at the top level (where the name is used to
load resources) and should be unique among all controls within the same
toplevel window (wxDialog, wxFrame).

The @c subclass attribute optional name of class whose constructor will be
called instead of the constructor for "class".
See @ref xrc_format_extending_subclass for more details.

@c \<object\> element may -- and almost always do -- have children elements.
These come in two varieties:

 -# Object's properties. A @em property is a value describing part of object's
    behaviour, for example the "label" property on wxButton defines its label.
    In the most common form, property is a single element with text content
    ("<label>Cancel</label>"), but they may use nested subelements too (e.g.
    @ref xrc_format_type_font "font property"). A property can only be
    listed once in an object's definition.
 -# Child objects. Window childs, sizers, sizer items or notebook pages
    are all examples of child objects. They are represented using nested
    @c \<object\> elements and are can be repeated more than once. The specifics
    of which object classes are allowed as children are class-specific and
    are documented below in @ref xrc_format_controls.

Example:
@code
<object class="wxDialog" name="example_dialog">
    <!-- properties: -->
    <title>Non-Derived Dialog Example</title>
    <centered>1</centered>
    <!-- child objects: -->
    <object class="wxBoxSizer">
        <orient>wxVERTICAL</orient>
        <cols>1</cols>
        <rows>0</rows>
        ...
    </object>
</object>
@endcode


@subsection xrc_format_object_ref <object_ref>

Anywhere an @c \<object\> element can be used, @c \<object_ref\> may be used
instead. @c \<object_ref\> is a @em reference to another named (i.e. with the
@c name attribute) @c \<object\> element. It has one mandatory attribute,
@c ref, with value containing the name of a named @c \<object\> element. When an
@c \<object_ref\> is encountered, a copy of the referenced @c \<object\> element
is made in place of @c \<object_ref\> occurrence and processed as usual.

For example, the following code:
@code
<object class="wxDialog" name="my_dlg">
    ...
</object>
<object_ref name="my_dlg_alias" ref="my_dlg"/>
@endcode
is equivalent to
@code
<object class="wxDialog" name="my_dlg">
    ...
</object>
<object class="wxDialog" name="my_dlg_alias">
    ... <!-- same as in my_dlg -->
</object>
@endcode

Additionally, it is possible to override some parts of the referenced object
in the @c \<object_ref\> pointing to it. This is useful for putting repetitive
parts of XRC definitions into a template that can be reused and customized in
several places. The two parts are merged as follows:

 -# The referred object is used as the initial content.
 -# All attributes set on @c \<object_ref\> are added to it.
 -# All child elements of @c \<object_ref\> are scanned. If an element with
    the same name (and, if specified, the @c name attribute too) is found
    in the referred object, they are recursively merged.
 -# Child elements in @c \<object_ref\> that do not have a match in the referred
    object are appended to the list of children of the resulting element by
    default. Optionally, they may have @c insert_at attribute with two possible
    values, "begin" or "end". When set to "begin", the element is prepended to
    the list of children instead of appended.

For example, "my_dlg" in this snippet:
@code
<object class="wxDialog" name="template">
    <title>Dummy dialog</title>
    <size>400,400</size>
</object>
<object_ref ref="template" name="my_dlg">
    <title>My dialog</title>
    <centered>1</centered>
</object>
@endcode
is identical to:
@code
<object_ref ref="template" name="my_dlg">
    <title>My dialog</title>
    <size>400,400</size>
    <centered>1</centered>
</object>
@endcode


@section xrc_format_datatypes Data types

There are several property data types that are frequently reused by different
properties. Rather than describing their format in the documentation of
every property, we list commonly used types in this section and document
their format.


@subsection xrc_format_type_bool Boolean

Boolean values are expressed using either "1" literal (true) or "0" (false).


@subsection xrc_format_type_float Floating-point value

Floating point values use POSIX (C locale) formatting -- decimal separator
is "." regardless of the locale.


@subsection xrc_format_type_colour Colour

Colour specification can be either any string colour representation accepted
by wxColour::Set() or any wxSYS_COLOUR_XXX symbolic name accepted by
wxSystemSettings::GetColour(). In particular, the following forms are supported:

@li named colours from wxColourDatabase
@li HTML-like "#rrggbb" syntax (but not "#rgb")
@li CSS-style "rgb(r,g,b)" and "rgba(r,g,b,a)"
@li wxSYS_COLOUR_XXX symbolic names

Some examples:
@code
<fg>red</fg>
<fg>#ff0000</fg>
<fg>rgb(255,0,0)</fg>
<fg>wxSYS_COLOUR_HIGHLIGHT</fg>
@endcode


@subsection xrc_format_type_size Size

Sizes and positions have the form of string with two comma-separated integer
components, with optional "d" suffix. Semi-formally:

  size := x "," y ["d"]

where x and y are integers. Either of the components (or both) may be "-1" to
signify default value. As a shortcut, empty string is equivalent to "-1,-1"
(= wxDefaultSize or wxDefaultPosition).

When the "d" suffix is used, integer values are interpreted as
@ref wxWindow::ConvertDialogToPixels() "dialog units" in the parent window.

Examples:
@code
42,-1
100,100
100,50d
@endcode

@subsection xrc_format_type_pos Position

Same as @ref xrc_format_type_size.


@subsection xrc_format_type_dimension Dimension

Similarly to @ref xrc_format_type_size "sizes", dimensions are expressed
as integers with optional "d" suffix. When "d" suffix is used, the integer
preceding it is interpreted as dialog units in the parent window.


@subsection xrc_format_type_text Text

String properties use several escape sequences that are translated according to
the following table:
@beginDefList
@itemdef{ "_", "&" (used for accelerators in wxWidgets) }
@itemdef{ "__", "_" }
@itemdef{ "\n", line break }
@itemdef{ "\r", carriage return }
@itemdef{ "\t", tab }
@itemdef{ "\\", "\" }
@endDefList

By default, the text is translated using wxLocale::GetTranslation() before
it is used. This can be disabled either globally by not passing
wxXRC_USE_LOCALE to wxXmlResource constructor, or by setting the @c translate
attribute on the property node to "0":
@code
<!-- this is not translated: -->
<label translate="0">_Unix</label>
<!-- but this is: -->
<help>Use Unix-style newlines</help>
@endcode

@note Even though the "_" character is used instead of "&" for accelerators,
      it is still possible to use "&". The latter has to be encoded as "&amp;",
      though, so using "_" is more convenient.

@see @ref xrc_format_pre_v2530, @ref xrc_format_pre_v2301


@subsection xrc_format_type_text_notrans Non-translatable text

Like @ref xrc_format_type_text, but the text is never translated and
@c translate attribute cannot be used.


@subsection xrc_format_type_bitmap Bitmap

Bitmap properties contain specification of a single bitmap or icon. In the most
basic form, their text value is simply a relative filename (or another
wxFileSystem URL) of the bitmap to use. For example:
@code
<object class="tool" name="wxID_NEW">
    <tooltip>New</tooltip>
    <bitmap>new.png</bitmap>
</object>
@endcode
The value is interpreted as path relative to the location of XRC file where the
reference occurs.

Alternatively, it is possible to specify the bitmap using wxArtProvider IDs.
In this case, the property element has no textual value (filename) and instead
has the @c stock_id XML attribute that contains stock art ID as accepted by
wxArtProvider::GetBitmap(). This can be either custom value (if the app uses
app-specific art provider) or one of the predefined wxART_XXX constants.

Optionally, @c stock_client attribute may be specified too and contain one of
the predefined wxArtClient values. If it is not specified, the default client
ID most appropriate in the context where the bitmap is referenced will be used.
In most cases, specifying @c stock_client is not needed.

Examples of stock bitmaps usage:
@code
<bitmap stock_id="fixed-width"/>        <!-- custom app-specific art -->
<bitmap stock_id="wxART_FILE_OPEN"/>    <!-- standard art->
@endcode

Specifying the bitmap directly and using @c stock_id are mutually exclusive.


@subsection xrc_format_type_style Style

Style properties (such as window's style or sizer flags) use syntax similar to
C++: the style value is OR-combination of individual flags. Symbolic names
identical to those used in C++ code are used for the flags. Flags are separated
with "|" (whitespace is allowed but not required around it).

The flags that are allowed for a given property are context-dependent.

Examples:
@code
<style>wxCAPTION|wxSYSTEM_MENU | wxRESIZE_BORDER</style>
<exstyle>wxDIALOG_EX_CONTEXTHELP</exstyle>
@endcode


@subsection xrc_format_type_font Font

XRC uses similar, but more flexible, abstract description of fonts to that
used by wxFont class. A font can be described either in terms of its elementary
properties, or it can be derived from one of system fonts.

The font property element is "composite" element: unlike majority of
properties, it doesn't have text value but contains several child elements
instead. These children are handled in the same way as object properties
and can be one of the following "sub-properties":

@beginTable
@hdr3col{property, type, description}
@row3col{size, unsigned integer,
    Pixel size of the font (default: wxNORMAL_FONT's size or @c sysfont's
    size if the @c sysfont property is used.}
@row3col{style, enum,
    One of "normal", "italic" or "slant" (default: normal).}
@row3col{weight, enum,
    One of "normal", "bold" or "light" (default: normal).}
@row3col{family, enum,
    One of "roman", "script", "decorative", "swiss", "modern" or "teletype"
    (default: roman).}
@row3col{underlined, @ref xrc_format_type_bool,
    Whether the font should be underlined (default: 0).}
@row3col{face, ,
    Comma-separated list of face names; the first one available is used
    (default: unspecified).}
@row3col{encoding, ,
    Charset of the font, unused in Unicode build), as string
    (default: unspecified).}
@row3col{sysfont, ,
    Symbolic name of system standard font(one of wxSYS_*_FONT constants).}
@row3col{relativesize, float,
    Float, font size relative to chosen system font's size; can only be
    used when 'sysfont' is used and when 'size' is not used.}
@endTable

All of them are optional, if they are missing, appropriate wxFont default is
used. If the @c sysfont property is used, then the defaults are taken from it
instead.

Examples:
@code
<font>
    <!-- fixed font: Arial if available, fall back to Helvetica -->
    <face>arial,helvetica</face>
    <size>12</size>
</font>

<font>
    <!-- enlarged, enboldened standard font: -->
    <sysfont>wxSYS_DEFAULT_GUI_FONT</sysfont>
    <weight>bold</weight>
    <relativesize>1.5</relativesize>
</font>
@endcode


@section xrc_format_windows Controls and windows

This section describes support wxWindow-derived classes in XRC format.

@subsection xrc_format_std_props Standard properties

The following properties are always (unless stated otherwise in
control-specific docs) available for @em windows objects. They are omitted
from properties lists below.

@beginTable
@hdr3col{property, type, description}
@row3col{position, @ref xrc_format_type_pos,
    Initial position of the window (default: wxDefaultPosition).}
@row3col{size, @ref xrc_format_type_size,
    Initial size of the window (default: wxDefaultSize).}
@row3col{style, @ref xrc_format_type_style,
    Window style for this control. The allowed values depend on what
    window is being created, consult respective class' constructor
    documentation for details (default: window-dependent default, usually
    wxFOO_DEFAULT_STYLE if defined for class wxFoo, 0 if not).}
@row3col{exstyle, @ref xrc_format_type_style,
    Extra style for the window, if any. See wxWindow::SetExtraStyle()
    (default: not set).}
@row3col{fg, @ref xrc_format_type_colour,
    Foreground colour of the window (default: window's default).}
@row3col{bg, @ref xrc_format_type_colour,
    Background colour of the window (default: window's default).}
@row3col{enabled, @ref xrc_format_type_bool,
    If set to 0, the control is disabled (default: 1).}
@row3col{hidden, @ref xrc_format_type_bool,
    If set to 1, the control is created hidden (default: 0).}
@row3col{tooltip, @ref xrc_format_type_text,
    Tooltip to use for the control (default: not set).}
@row3col{font, @ref xrc_format_type_font,
    Font to use for the control (default: window's default).}
@row3col{help, @ref xrc_format_type_text,
    Context-sensitive help for the control, used by wxHelpProvider
    (default: not set).}
@endTable

All of these properties are optional.


@subsection xrc_format_controls Supported controls

@subsubsection xrc_wxanimationctrl wxAnimationCtrl
FIXME

@subsubsection xrc_wxbitmapbutton wxBitmapButton
FIXME

@subsubsection xrc_wxbitmapcombobox wxBitmapComboBox
FIXME

@subsubsection xrc_wxbutton wxButton
FIXME

@subsubsection xrc_wxcalendarctrl wxCalendarCtrl
FIXME

@subsubsection xrc_wxcheckbox wxCheckBox
FIXME

@subsubsection xrc_wxchecklistbox wxCheckListBox
FIXME

@subsubsection xrc_wxchoice wxChoice
FIXME

@subsubsection xrc_wxchoicebook wxChoicebook
FIXME

@subsubsection xrc_wxcollapsiblepane wxCollapsiblePane
FIXME

@subsubsection xrc_wxcolourpickerctrl wxColourPickerCtrl
FIXME

@subsubsection xrc_wxcombobox wxComboBox
FIXME

@subsubsection xrc_wxdatepickerctrl wxDatePickerCtrl
FIXME

@subsubsection xrc_wxdialog wxDialog
FIXME

@subsubsection xrc_wxdirpickerctrl wxDirPickerCtrl
FIXME

@subsubsection xrc_wxfilepickerctrl wxFilePickerCtrl
FIXME

@subsubsection xrc_wxfontpickerctrl wxFontPickerCtrl
FIXME

@subsubsection xrc_wxfrane wxFrame
FIXME

@subsubsection xrc_wxgauge wxGauge
FIXME

@subsubsection xrc_wxgenericdirctrl wxGenericDirCtrl
FIXME

@subsubsection xrc_wxgrid wxGrid
FIXME

@subsubsection xrc_wxhtmlwindow wxHtmlWindow
FIXME

@subsubsection xrc_wxhyperlinkctrl wxHyperlinkCtrl
FIXME

@subsubsection xrc_wxlistbox wxListBox
FIXME

@subsubsection xrc_wxlistbook wxListbook
FIXME

@subsubsection xrc_wxlistctrl wxListCtrl
FIXME

@subsubsection xrc_wxmdiparentframe wxMDIParentFrame
FIXME

@subsubsection xrc_wxmdichildframe wxMDIChildFrame
FIXME

@subsubsection xrc_wxmenu wxMenu
FIXME

@subsubsection xrc_wxmenubar wxMenuBar
FIXME

@subsubsection xrc_wxnotebook wxNotebook
FIXME

@subsubsection xrc_wxownerdrawncombobox wxOwnerDrawnComboBox
FIXME

@subsubsection xrc_wxpanel wxPanel
FIXME

@subsubsection xrc_wxpropertysheetdialog wxPropertySheetDialog
FIXME

@subsubsection xrc_wxradiobutton wxRadioButton
FIXME

@subsubsection xrc_wxradiobox wxRadioBox
FIXME

@subsubsection xrc_wxrichtextctrl wxRichTextCtrl
FIXME

@subsubsection xrc_wxscrollbar wxScrollBar
FIXME

@subsubsection xrc_wxscrolledwindow wxScrolledWindow
FIXME

@subsubsection xrc_wxsimplehtmllistbox wxSimpleHtmlListBox
FIXME

@subsubsection xrc_wxslider wxSliderq
FIXME

@subsubsection xrc_wxspinctrl wxSpinCtrl
FIXME

@subsubsection xrc_wxsplitterwindow wxSplitterWindow
FIXME

@subsubsection xrc_wxsearchctrl wxSearchCtrl
FIXME

@subsubsection xrc_wxstatusbar wxStatusBar
FIXME

@subsubsection xrc_wxstaticbitmap wxStaticBitmap
FIXME

@subsubsection xrc_wxstaticbox wxStaticBox
FIXME

@subsubsection xrc_wxstaticline wxStaticLine
FIXME

@subsubsection xrc_wxstatictext wxStaticText
FIXME

@subsubsection xrc_wxtextctrl wxTextCtrl
FIXME

@subsubsection xrc_wxtogglebuttton wxToggleButton
FIXME

@subsubsection xrc_wxtoolbar wxToolBar
FIXME

@subsubsection xrc_wxtreectrl wxTreeCtrl
FIXME

@subsubsection xrc_wxtreebook wxTreebook
FIXME

@subsubsection xrc_wxwizard wxWizard
FIXME


@section xrc_format_sizers Sizers

Sizers are handled slightly differently in XRC resources than they are in
wxWindow hierarchy. wxWindow's sizers hierarchy is parallel to the wxWindow
children hieararchy: child windows are children of their parent window and
the sizer (or sizers) form separate hierarchy attached to the window with
wxWindow::SetSizer().

In XRC, the two hierarchies are merged together: sizers are children of other
sizers or windows and they can contain child window objects.

If a sizer is child of a window object in the resource, it must be the only
child and it will be attached to the parent with wxWindow::SetSizer().
Additionally, if the window doesn't have its size explicitly set,
wxSizer::Fit() is used to resize the window. If the parent window is
toplevel window, wxSizer::SetSizeHints() is called to set its hints.

A sizer object can have one or more child objects of one of two pseudo-classes:
@c sizeritem or @c spacer (see @ref xrc_format_wxstddialogbuttonsizer for
an exception). The former specifies an element (another sizer or a window)
to include in the sizer, the latter adds empty space to the sizer.

@c sizeritem objects have exactly one child object: either another sizer
object, or a window object. @c spacer objects don't have any children, but
they have one property:

@beginTable
@hdr3col{property, type, description}
@row3col{size, @ref xrc_format_type_size, Size of the empty space (required).}
@endTable

Both @c sizeritem and @c spacer objects can have any of the following
properties:

@beginTable
@hdr3col{property, type, description}
@row3col{option, integer,
    The "option" value for sizers. Used by wxBoxSizer to set proportion of
    the item in the growable direction (default: 0).}
@row3col{flag, @ref xrc_format_type_style,
    wxSizerItem flags (default: 0).}
@row3col{border, @ref xrc_format_type_dimension,
    Size of the border around the item (directions are specified in flags)
    (default: 0).}
@row3col{minsize, @ref xrc_format_type_size,
    Minimal size of this item (default: no min size).}
@row3col{ratio, @ref xrc_format_type_size,
    Item ratio, see wxSizer::SetRatio() (default: no ratio).}
@row3col{cellpos, @ref xrc_format_type_pos,
    (wxGridBagSizer only) Position, see wxGBSizerItem::SetPos() (required). }
@row3col{cellspan, @ref xrc_format_type_size,
    (wxGridBagSizer only) Span, see wxGBSizerItem::SetSpan() (required). }
@endTable

Example of sizers XRC code:
@code
<object class="wxDialog" name="derived_dialog">
    <title>Derived Dialog Example</title>
    <centered>1</centered>
    <!-- this sizer is set to be this dialog's sizer: -->
    <object class="wxFlexGridSizer">
        <cols>1</cols>
        <rows>0</rows>
        <vgap>0</vgap>
        <hgap>0</hgap>
        <growablecols>0</growablecols>
        <growablerows>0</growablerows>
        <object class="sizeritem">
            <flag>wxALIGN_CENTRE|wxALL</flag>
            <border>5</border>
            <object class="wxButton" name="my_button">
                <label>My Button</label>
            </object>
        </object>
        <object class="sizeritem">
            <flag>wxALIGN_CENTRE|wxALL</flag>
            <border>5</border>
            <object class="wxBoxSizer">
                <orient>wxHORIZONTAL</orient>
                <object class="sizeritem">
                    <flag>wxALIGN_CENTRE|wxALL</flag>
                    <border>5</border>
                    <object class="wxCheckBox" name="my_checkbox">
                        <label>Enable this text control:</label>
                    </object>
                </object>
                <object class="sizeritem">
                    <flag>wxALIGN_CENTRE|wxALL</flag>
                    <border>5</border>
                    <object class="wxTextCtrl" name="my_textctrl">
                        <size>80,-1</size>
                        <value></value>
                    </object>
                </object>
            </object>
        </object>
        ...
    </object>
</object>
@endcode

The sizer classes that can be used are listed below, together with their
class-specific properties. All classes support the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{minsize, @ref xrc_format_type_size,
    Minimal size that this sizer will have, see wxSizer::SetMinSize()
    (default: no min size).}
@endTable

@subsection xrc_format_wxboxsizer wxBoxSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref xrc_format_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (default: wxHORIZONTAL).}
@endTable

@subsection xrc_format_wxstaticsboxizer wxStaticBoxSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref xrc_format_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (default: wxHORIZONTAL).}
@row3col{label, @ref xrc_format_type_text,
    Label to be used for the static box around the sizer (required).}
@endTable

@subsection xrc_format_wxgridsizer wxGridSizer

@beginTable
@hdr3col{property, type, description}
@row3col{rows, integer, Number of rows in the grid (required).}
@row3col{cols, integer, Number of columns in the grid (required).}
@row3col{vgap, integer, Vertical gap between children (default: 0).}
@row3col{hgap, integer, Horizontal gap between children (default: 0).}
@endTable

@subsection xrc_format_wxflexgridsizer wxFlexGridSizer

@beginTable
@hdr3col{property, type, description}
@row3col{rows, integer, Number of rows in the grid (required).}
@row3col{cols, integer, Number of columns in the grid (required).}
@row3col{vgap, integer, Vertical gap between children (default: 0).}
@row3col{hgap, integer, Horizontal gap between children (default: 0).}
@row3col{growablerows, comma-separated integers list,
    Comma-separated list of indexes of rows that are growable
    (default: none).}
@row3col{growablecols, comma-separated integers list,
    Comma-separated list of indexes of columns that are growable
    (default: none).}
@endTable

@subsection xrc_format_wxgridbagsizer wxGridBagSizer

@beginTable
@hdr3col{property, type, description}
@row3col{vgap, integer, Vertical gap between children (default: 0).}
@row3col{hgap, integer, Horizontal gap between children (default: 0).}
@row3col{growablerows, comma-separated integers list,
    Comma-separated list of indexes of rows that are growable
    (default: none).}
@row3col{growablecols, comma-separated integers list,
    Comma-separated list of indexes of columns that are growable
    (default: none).}
@endTable

@subsection xrc_format_wxwrapsizer wxWrapSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref xrc_format_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (required).}
@row3col{flag, @ref xrc_format_type_style, wxWrapSizer flags (default: 0).}
@endTable

@subsection xrc_format_wxstddialogbuttonsizer wxStdDialogButtonSizer

Unlike other sizers, wxStdDialogButtonSizer doesn't have neither @c sizeritem
nor @c spacer children. Instead, it has one or more children of the
@c button pseudo-class. @c button objects have no properties and they must
always have exactly one child of the @c wxButton class or a class derived from
wxButton.

Example:
@code
<object class="wxStdDialogButtonSizer">
    <object class="button">
        <object class="wxButton" name="wxID_OK">
            <label>OK</label>
        </object>
    </object>
    <object class="button">
        <object class="wxButton" name="wxID_CANCEL">
            <label>Cancel</label>
        </object>
    </object>
</object>
@endcode



@section xrc_format_other_objects Other objects

In addition to describing UI elements, XRC files can contain non-windows
objects such as bitmaps or icons. This is a concession to Windows developers
used to storing them in Win32 resources.

Note that unlike Win32 resources, bitmaps included in XRC files are @em not
embedded in the XRC file itself. XRC file only contains a reference to another
file with bitmap data.

@subsection xrc_format_bitmap wxBitmap

Bitmaps are stored in @c \<object\> element with class set to @c wxBitmap. Such
bitmaps can then be loaded using wxXmlResource::LoadBitmap(). The content of
the element is exactly same as in the case of
@ref xrc_format_type_bitmap "bitmap properties", except that toplevel
@c \<object\> is used.

For example, instead of:
@code
<bitmap>mybmp.png</bitmap>
<bitmap stock_id="wxART_NEW"/>
@endcode
toplevel wxBitmap resources would look like:
@code
<object class="wxBitmap" name="my_bitmap">mybmp.png</object>
<object class="wxBitmap" name="my_new_bitmap" stock_id="wxART_NEW"/>
@endcode


@subsection xrc_format_icon wxIcon

wxIcon resources are identical to @ref xrc_format_bitmap "wxBitmap ones",
except that the class is @c wxIcon.


@section xrc_format_platform Platform specific content

It is possible to conditionally process parts of XRC files on some platforms
only and ignore them on other platforms. @em Any element in XRC file, be it
toplevel or arbitrarily nested one, can have the @c platform attribute. When
used, @c platform contains |-separated list of platforms that this element
should be processed on. It is filtered out and ignored on any other platforms.

Possible elemental values are:
@beginDefList
@itemdef{ @c win, Windows }
@itemdef{ @c mac, Mac OS X (or Mac Classic in wxWidgets version supporting it }
@itemdef{ @c unix, Any Unix platform @em except OS X }
@itemdef{ @c os2, OS/2 }
@endDefList

Examples:
@code
<label platform="win">Windows</label>
<label platform="unix">Unix</label>
<label platform="mac">Mac OS X</label>
<help platform="mac|unix">Not a Windows machine</help>
@endcode



@section xrc_format_extending Extending XRC format

The XRC format is designed to be extensible and allows specifying and loading
custom controls. The three available mechanisms are described in the rest of
this section in the order of increasing complexity.

@subsection xrc_format_extending_subclass Subclassing

The simplest way to add custom controls is to set the @c subclass attribute
of @c \<object\> element:

@code
<object name="my_value" class="wxTextCtrl" subclass="MyTextCtrl">
  <style>wxTE_MULTILINE</style>
  ...etc., setup wxTextCtrl as usual...
</object>
@endcode

In that case, wxXmlResource will create an instance of the specified subclass
(@c MyTextCtrl in the example above) instead of the class (@c wxTextCtrl above)
when loading the resource. However, the rest of the object's loading (calling
its Create() method, setting its properties, loading any children etc.)
will proceed in @em exactly the same way as it would without @c subclass
attribute. In other words, this approach is only sufficient when the custom
class is just a small modification (e.g. overridden methods or customized
events handling) of an already supported classes.

The subclass must satisfy a number of requirements:

 -# It must be derived from the class specified in @c class attribute.
 -# It must be visible in wxWidget's pseudo-RTTI mechanism, i.e. there must be
    a DECLARE_DYNAMIC_CLASS() entry for it.
 -# It must support two-phase creation. In particular, this means that it has
    to have default constructor.
 -# It cannot provide custom Create() method and must be constructible using
    base @c class' Create() method (this is because XRC will call Create() of
    @c class, not @c subclass). In other words, @em creation of the control
    must not be customized.


@subsection xrc_format_extending_unknown <object class="unknown">

A more flexible solution is to put a @em placeholder in the XRC file and
replace it with custom control after the resource is loaded. This is done by
using the @c unknown pseudo-class:

@code
<object class="unknown" name="my_placeholder"/>
@endcode

The placeholder is inserted as dummy wxPanel that will hold custom control in
it. At runtime, after the resource is loaded and a window created from it
(using e.g. wxXmlResource::LoadDialog()), use code must call
wxXmlResource::AttachUnknownControl() to insert the desired control into
placeholder container.

This method makes it possible to insert controls that are not known to XRC at
all, but it's also impossible to configure the control in XRC description in
any way. The only properties that can be specified are
the @ref xrc_format_std_props "standard window properties".

@note @c unknown class cannot be combined with @c subclass attribute,
      they are mutually exclusive.


@subsection xrc_format_extending_custom Adding custom classes

Finally, XRC allows adding completely new classes in addition to the ones
listed in this document. A class for which wxXmlResourceHandler is implemented
can be used as first-class object in XRC simply by passing class name as the
value of @c class attribute:

@code
<object name="my_ctrl" class="MyWidget">
  <my_prop>foo</my_prop>
  ...etc., whatever MyWidget handler accepts...
</object>
@endcode

The only requirements on the class are that
 -# the class must derive from wxObject
 -# it must support wxWidget's pseudo-RTTI mechanism

Child elements of @c \<object\> are handled by the custom handler and there are
no limitations on them imposed by XRC format.

This is the only mechanism that works for toplevel objects -- custom controls
are accessible using type-unsafe wxXmlResource::LoadObject() method.



@section xrc_format_packed Packed XRC files

In addition to plain XRC files, wxXmlResource supports (if wxFileSystem support
is compiled in) compressed XRC resources. Compressed resources have either
.zip or .xrs extension and are simply ZIP files that contain arbitrary
number of XRC files and their dependencies (bitmaps, icons etc.).



@section xrc_format_oldversions Older format versions

This section describes differences in older revisions of XRC format (i.e.
files with older values of @c version attribute of @c \<resource\>).


@subsection xrc_format_pre_v2530 Versions before 2.5.3.0

Version 2.5.3.0 introduced C-like handling of "\\" in text. In older versions,
"\n", "\t" and "\r" escape sequences were replaced with respective characters
in the same matter it's done in C, but "\\" was left intact instead of being
replaced with single "\", as one would expect. Starting with 2.5.3.0, all of
them are handled in C-like manner.


@subsection xrc_format_pre_v2301 Versions before 2.3.0.1

Prior to version 2.3.0.1, "$" was used for accelerators instead of "_"
or "&amp;". For example,
@code
<label>$File</label>
@endcode
was used in place of current version's
@code
<label>_File</label>
@endcode
(or "&amp;File").

*/
