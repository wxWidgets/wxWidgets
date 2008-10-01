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


@subsection xrc_format_type_string URL

An unformatted string. Unlike with @ref xrc_format_type_text, no escaping
or translations are done.


@subsection xrc_format_type_url URL

Any URL accepted by wxFileSystem (typically relative to XRC file's location,
but can be absolute too). Unlike with @ref xrc_format_type_text, no escaping
or translations are done.


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
<bitmap stock_id="wxART_FILE_OPEN"/>    <!-- standard art -->
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
@row3col{pos, @ref xrc_format_type_pos,
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

This section lists all controls supported by default. For each control, its
control-specific properties are listed. If the control can have child objects,
it is documented there too; unless said otherwise, XRC elements for these
controls cannot have children.

@subsubsection xrc_wxanimationctrl wxAnimationCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{animation, @ref xrc_format_type_url,
    Animation file to load into the control (required).}
@endTable


@subsubsection xrc_wxbitmapbutton wxBitmapButton

@beginTable
@hdr3col{property, type, description}
@row3col{default, @ref xrc_format_type_bool,
     Should this button be the default button in dialog (default: 0)?}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap to show on the button (required).}
@row3col{selected, @ref xrc_format_type_bitmap,
     Bitmap to show when the button is selected (default: none, same as @c bitmap).}
@row3col{focus, @ref xrc_format_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap).}
@row3col{disabled, @ref xrc_format_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap).}
@row3col{hover, @ref xrc_format_type_bitmap,
     Bitmap to show when mouse cursor hovers above the bitmap (default: none, same as @c bitmap).}
@endTable


@subsubsection xrc_wxbitmapcombobox wxBitmapComboBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{value, @ref xrc_format_type_string,
     Initial value in the control (doesn't have to be one of @ content values;
     default: empty).}
@endTable

If both @c value and @c selection are specified and @c selection is not -1,
then @c selection takes precedence.

A wxBitmapComboBox can have one or more child objects of the @c ownerdrawnitem
pseudo-class. @c ownerdrawnitem objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{text, @ref xrc_format_type_text,
     Item's label (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Item's bitmap (default: no bitmap).}
@endTable

Example:
@code
<object class="wxBitmapComboBox">
    <selection>1</selection>
    <object class="ownerdrawnitem">
        <text>Foo</text>
        <bitmap>foo.png</bitmap>
    </object>
    <object class="ownerdrawnitem">
        <text>Bar</text>
        <bitmap>bar.png</bitmap>
    </object>
</object>
@endcode


@subsubsection xrc_wxbutton wxButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to display on the button (required).}
@row3col{default, @ref xrc_format_type_bool,
     Should this button be the default button in dialog (default: 0)?}
@endTable


@subsubsection xrc_wxcalendarctrl wxCalendarCtrl

No additional properties.


@subsubsection xrc_wxcheckbox wxCheckBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to use for the checkbox (required).}
@row3col{checked, @ref xrc_format_type_bool,
     Should the checkbox be checked initially (default: 0)?}
@endTable


@subsubsection xrc_wxchecklistbox wxCheckListBox

@beginTable
@hdr3col{property, type, description}
@row3col{content, ,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@endTable

The @c \<item\> elements have listbox items' labels as their text values. They
can also have optional @c checked XML attribute -- if set to "1", the value is
initially checked.

Example:
@code
<object class="wxCheckListBox">
    <content>
        <item checked="1">Download library</item>
        <item checked="1">Compile samples</item>
        <item checked="1">Skim docs</item>
        <item checked="1">Finish project</item>
        <item>Wash car</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxchoice wxChoice

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{content, ,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@endTable

Example:
@code
<object class="wxChoice" name="controls_choice">
    <content>
        <item>See</item>
        <item>Hear</item>
        <item>Feel</item>
        <item>Smell</item>
        <item>Taste</item>
        <item>The Sixth Sense!</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxchoicebook wxChoicebook

No additional properties.

A choicebook can have one or more child objects of the @c choicebookpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage). @c choicebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Sheet page's title (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref xrc_format_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c choicebookpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxcollapsiblepane wxCollapsiblePane

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to use for the collapsible section (required).}
@row3col{collapsed, @ref xrc_format_type_bool,
     Should the pane be collapsed initially (default: 0)?}
@endTable

wxCollapsiblePane may contain single optional child object of the @c panewindow
pseudo-class type. @c panewindow itself must contain exactly one child that
is a @ref xrc_format_sizers "sizer" or a non-toplevel window
object.


@subsubsection xrc_wxcolourpickerctrl wxColourPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_colour,
    Initial value of the control (default: wxBLACK).}
@endTable


@subsubsection xrc_wxcombobox wxComboBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: not used).}
@row3col{content, ,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@row3col{value, @ref xrc_format_type_string,
     Initial value in the control (doesn't have to be one of @ content values;
     default: empty).}
@endTable

If both @c value and @c selection are specified and @c selection is not -1,
then @c selection takes precedence.

Example:
@code
<object class="wxComboBox" name="controls_combobox">
    <style>wxCB_DROPDOWN</style>
    <value>nedit</value>
    <content>
        <item>vim</item>
        <item>emacs</item>
        <item>notepad.exe</item>
        <item>bbedit</item>
    </content>
</object>
@endcode

@subsubsection xrc_wxdatepickerctrl wxDatePickerCtrl

No additional properties.


@subsubsection xrc_wxdialog wxDialog

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref xrc_format_type_text,
     Dialog's title (default: empty).}
@row3col{icon, @ref xrc_format_type_bitmap,
     Dialog's icon (default: not used).}
@row3col{centered, @ref xrc_format_type_bool,
     Whether the dialog should be centered on the screen (default: 0).}
@endTable

wxDialog may have optional children: either exactly one
@ref xrc_format_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.

@subsubsection xrc_wxdirpickerctrl wxDirPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_string,
    Initial value of the control (default: empty).}
@row3col{message, @ref xrc_format_type_text,
    Message shown to the user in wxDirDialog shown by the control (required).}
@endTable


@subsubsection xrc_wxfilepickerctrl wxFilePickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_string,
    Initial value of the control (default: empty).}
@row3col{message, @ref xrc_format_type_text,
    Message shown to the user in wxDirDialog shown by the control (required).}
@row3col{wildcard, @ref xrc_format_type_string,
    Message shown to the user in wxDirDialog shown by the control (required).}
@endTable


@subsubsection xrc_wxfontpickerctrl wxFontPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_font,
    Initial value of the control (default: wxNORMAL_FONT).}
@endTable

@subsubsection xrc_wxfrane wxFrame

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref xrc_format_type_text,
     Frame's title (default: empty).}
@row3col{icon, @ref xrc_format_type_bitmap,
     Frame's icon (default: not used).}
@row3col{centered, @ref xrc_format_type_bool,
     Whether the frame should be centered on the screen (default: 0).}
@endTable

wxFrame may have optional children: either exactly one
@ref xrc_format_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.


@subsubsection xrc_wxgauge wxGauge

@beginTable
@hdr3col{property, type, description}
@row3col{range, integer,
    Maximum value of the gauge (default: 100).}
@row3col{value, integer,
    Initial value of the control (default: 0).}
@row3col{shadow, @ref xrc_format_type_dimension,
    Rendered shadow size (default: none; ignored by most platforms).}
@row3col{bezel, @ref xrc_format_type_dimension,
    Rendered bezel size (default: none; ignored by most platforms).}
@endTable

@subsubsection xrc_wxgenericdirctrl wxGenericDirCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{defaultfolder, @ref xrc_format_type_text,
    Initial folder (default: empty).}
@row3col{filter, @ref xrc_format_type_text,
    Filter string, using the same syntax as used by wxFileDialog, e.g.
    "All files (*.*)|*.*|JPEG files (*.jpg)|*.jpg" (default: empty).}
@row3col{defaultfilter, integer,
    Zero-based index of default filter (default: 0).}
@endTable

@subsubsection xrc_wxgrid wxGrid

No additional properties.


@subsubsection xrc_wxhtmlwindow wxHtmlWindow

@beginTable
@hdr3col{property, type, description}
@row3col{url, @ref xrc_format_type_url,
    Page to display in the window.}
@row3col{htmlcode, @ref xrc_format_type_text,
    HTML markup to display in the window.}
@row3col{borders, @ref xrc_format_type_dimension,
    Border around HTML content (default: 0).}
@endTable

At most one of @c url and @c htmlcode properties may be specified, they are
mutually exclusive. If neither is set, the window is initialized to show empty
page.


@subsubsection xrc_wxhyperlinkctrl wxHyperlinkCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to display on the control (required).}
@row3col{url, @ref xrc_format_type_url,
     URL to open when the link is clicked (required).}
@endTable


@subsubsection xrc_wxlistbox wxListBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{content, ,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@endTable

Example:
@code
<object class="wxListBox" name="controls_listbox">
    <size>250,160</size>
    <style>wxLB_SINGLE</style>
    <content>
        <item>Milk</item>
        <item>Pizza</item>
        <item>Bread</item>
        <item>Orange juice</item>
        <item>Paper towels</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxlistbook wxListbook

No additional properties.

A listbook can have one or more child objects of the @c listbookpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage). @c listbookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Sheet page's title (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref xrc_format_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c listbookpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxlistctrl wxListCtrl

No additional properties.


@subsubsection xrc_wxmdiparentframe wxMDIParentFrame

wxMDIParentFrame supports the same properties that @ref xrc_wxfrane does.

wxMDIParentFrame may have optional children. When used, the child objects
must be of wxMDIChildFrame type.


@subsubsection xrc_wxmdichildframe wxMDIChildFrame

wxMDIChildFrame supports the same properties that @ref xrc_wxfrane and
@ref xrc_wxmdiparentframe do.

wxMDIChildFrame can only be used as as immediate child of @ref
xrc_wxmdiparentframe.

wxMDIChildFrame may have optional children: either exactly one
@ref xrc_format_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.


@subsubsection xrc_wxmenu wxMenu

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Menu's label (default: empty, but required for menus other
     than popup menus).}
@row3col{help, @ref xrc_format_type_text,
     Help shown in statusbar when the menu is selected (only for submenus
     of another wxMenu, default: none).}
@row3col{enabled, @ref xrc_format_type_bool,
     Is the submenu item enabled (only for submenus of another wxMenu,
     default: 1)?}
@endTable

Note that unlike most controls, wxMenu does @em not have
@ref xrc_format_std_props.

A menu object can have one or more child objects of the wxMenuItem or wxMenu
classes or @c break or @c separator pseudo-classes.

The @c separator pseudo-class is used to insert separators into the menu and
has neither properties nor children. Likewise, @c break inserts a break (see
wxMenu::Break()).

wxMenuItem objects support the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Item's label (required).}
@row3col{accel, @ref xrc_format_type_text_notrans,
     Item's accelerator (default: none).}
@row3col{radio, @ref xrc_format_type_bool,
     Item's kind is wxITEM_RADIO (default: 0)?}
@row3col{checkable, @ref xrc_format_type_bool,
     Item's kind is wxITEM_CHECK (default: 0)?}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap to show with the item (default: none).}
@row3col{bitmap2, @ref xrc_format_type_bitmap,
     Bitmap for the checked state (wxMSW, if checkable; default: none).}
@row3col{help, @ref xrc_format_type_text,
     Help shown in statusbar when the item is selected (default: none).}
@row3col{enabled, @ref xrc_format_type_bool,
     Is the item enabled (default: 1)?}
@row3col{checked, @ref xrc_format_type_bool,
     Is the item checked initially (default: 0)?}
@endTable

Example:
@code
<object class="wxMenu" name="menu_edit">
  <style>wxMENU_TEAROFF</style>
  <label>_Edit</label>
  <object class="wxMenuItem" name="wxID_FIND">
    <label>_Find...</label>
    <accel>Ctrl-F</accel>
  </object>
  <object class="separator"/>
  <object class="wxMenuItem" name="menu_fuzzy">
    <label>Translation is _fuzzy</label>
    <checkable>1</checkable>
  </object>
  <object class="wxMenu" name="submenu">
    <label>A submenu</label>
    <object class="wxMenuItem" name="foo">...</object>
    ...
  </object>
  <object class="separator" platform="unix"/>
  <object class="wxMenuItem" name="wxID_PREFERENCES" platform="unix">
    <label>_Preferences</label>
  </object>
</object>
@endcode

@subsubsection xrc_wxmenubar wxMenuBar

No properties. Note that unlike most controls, wxMenuBar does @em not have
@ref xrc_format_std_props.

A menubar can have one or more child objects of the @ref xrc_wxmenu "wxMenu"
class.


@subsubsection xrc_wxnotebook wxNotebook

No additional properties.

A notebook can have one or more child objects of the @c notebookpage
pseudo-class. @c notebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Page's title (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref xrc_format_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c notebookpage has exactly one non-toplevel window as its child.

Example:
@code
<object class="wxNotebook">
    <style>wxBK_BOTTOM</style>
    <object class="notebookpage">
        <label>Page 1</label>
        <object class="wxPanel" name="page_1">
            ...
        </object>
    </object>
    <object class="notebookpage">
        <label>Page 2</label>
        <object class="wxPanel" name="page_2">
            ...
        </object>
    </object>
</object>
@endcode


@subsubsection xrc_wxownerdrawncombobox wxOwnerDrawnComboBox

wxOwnerDrawnComboBox has the same properties as
@ref xrc_wxcombobox "wxComboBox", plus the following additional properties:

@beginTable
@hdr3col{property, type, description}
@row3col{buttonsize, @ref xrc_format_type_size,
     Size of the dropdown button (default: default).}
@endTable


@subsubsection xrc_wxpanel wxPanel

No additional properties.

wxPanel may have optional children: either exactly one
@ref xrc_format_sizers "sizer" child or any number of non-toplevel window
objects.


@subsubsection xrc_wxpropertysheetdialog wxPropertySheetDialog

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref xrc_format_type_text,
     Dialog's title (default: empty).}
@row3col{icon, @ref xrc_format_type_bitmap,
     Dialog's icon (default: not used).}
@row3col{centered, @ref xrc_format_type_bool,
     Whether the dialog should be centered on the screen (default: 0).}
@row3col{buttons, @ref xrc_format_type_style,
     Buttons to show, combination of flags accepted by
     wxPropertySheetDialog::CreateButtons() (default: 0).}
@endTable

A sheet dialog can have one or more child objects of the @c propertysheetpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage). @c propertysheetpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Sheet page's title (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref xrc_format_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c propertysheetpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxradiobutton wxRadioButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label shown on the radio button (required).}
@row3col{value, @ref xrc_format_type_bool,
    Initial value of the control (default: 0).}
@endTable


@subsubsection xrc_wxradiobox wxRadioBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label for the whole box (required).}
@row3col{dimension, integer,
     Specifies the maximum number of rows (if style contains
     @c wxRA_SPECIFY_ROWS) or columns (if style contains @c wxRA_SPECIFY_COLS)
     for a two-dimensional radiobox (default: 1).}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{content, ,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (see below; default: empty).}
@endTable

The @c \<item\> elements have radio buttons' labels as their text values. They
can also have some optional XML @em attributes (not properties!):

@beginTable
@hdr3col{attribute, type, description}
@row3col{tooltip, @ref xrc_format_type_string,
     Tooltip to show over this ratio button (default: none).}
@row3col{helptext, @ref xrc_format_type_string,
     Contextual help for this radio button (default: none).}
@row3col{enabled, @ref xrc_format_type_bool,
     Is the button enabled (default: 1)?}
@row3col{hidden, @ref xrc_format_type_bool,
     Is the button hidden initially (default: 0)?}
@endTable

Example:
@code
<object class="wxRadioBox" name="controls_radiobox">
    <style>wxRA_SPECIFY_COLS</style>
    <label>Radio stations</label>
    <dimension>1</dimension>
    <selection>0</selection>
    <content>
        <item tooltip="Powerful radio station" helptext="This station is for amateurs of hard rock and heavy metal">Power 108</item>
        <item tooltip="Disabled radio station" enabled="0">Power 0</item>
        <item tooltip="">WMMS 100.7</item>
        <item tooltip="E=mc^2">Energy 98.3</item>
        <item helptext="Favourite chukcha's radio">CHUM FM</item>
        <item>92FM</item>
        <item hidden="1">Very quit station</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxrichtextctrl wxRichTextCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_text,
    Initial value of the control (default: empty).}
@row3col{maxlength, integer,
    Maximum length of the text entered (default: unlimited).}
@endTable


@subsubsection xrc_wxscrollbar wxScrollBar

@beginTable
@hdr3col{property, type, description}
@row3col{value, integer,
    Initial position of the scrollbar (default: 0).}
@row3col{range, integer,
    Maximum value of the gauge (default: 10).}
@row3col{thumbsize, integer,
    Size of the thumb (default: 1).}
@row3col{pagesize, integer,
    Page size (default: 1).}
@endTable

@subsubsection xrc_wxscrolledwindow wxScrolledWindow

@beginTable
@hdr3col{property, type, description}
@row3col{scrollrate, @ref xrc_format_type_size,
    Scroll rate in @em x and @em y directions (default: not set;
    required if the window has a sizer child).}
@endTable

wxScrolledWindow may have optional children: either exactly one
@ref xrc_format_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, wxSizer::FitInside() is used (instead of
wxSizer::Fit() as usual) and so the children don't determine scrolled window's
minimal size, they only affect @em virtual size. Usually, both @c scrollrate
and either @c size or @c minsize on containing sizer item should be used
in this case.


@subsubsection xrc_wxsimplehtmllistbox wxSimpleHtmlListBox

wxSimpleHtmlListBox has same properties as @ref xrc_wxlistbox "wxListBox".
The only difference is that the text contained in @c \<item\> elements is
HTML markup. Note that the markup has to be escaped:

@code
<object class="wxSimpleHtmlListBox">
    <content>
        <item>&lt;b&gt;Bold&lt;/b&gt; Milk</item>
    </content>
</object>
@endcode

(X)HTML markup elements cannot be included directly:

@code
<object class="wxSimpleHtmlListBox">
    <content>
        <!-- This is incorrect, doesn't work! -->
        <item><b>Bold</b> Milk</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxslider wxSlider

@beginTable
@hdr3col{property, type, description}
@row3col{value, integer,
    Initial value of the control (default: 0).}
@row3col{min, integer,
    Minimum allowed value (default: 0).}
@row3col{max, integer,
    Maximum allowed value (default: 100).}
@row3col{pagesize, integer,
    Line size; number of steps the slider moves when the user moves
    pages up or down (default: unset).}
@row3col{linesize, integer,
    Line size; number of steps the slider moves when the user moves it
    up or down a line (default: unset).}
@row3col{tickfreq, integer,
    Tick marks frequency (Windows only; default: unset).}
@row3col{tick, integer,
    Tick position (Windows only; default: unset).}
@row3col{thumb, integer,
    Thumb length (Windows only; default: unset).}
@row3col{selmin, integer,
    Selection start position (Windows only; default: unset).}
@row3col{selmax, integer,
    Selection end position (Windows only; default: unset).}
@endTable


@subsubsection xrc_wxspinbutton wxSpinButton

@beginTable
@hdr3col{property, type, description}
@row3col{value, integer,
    Initial value of the control (default: 0).}
@row3col{min, integer,
    Minimum allowed value (default: 0).}
@row3col{max, integer,
    Maximum allowed value (default: 100).}
@endTable


@subsubsection xrc_wxspinctrl wxSpinCtrl

wxSpinCtrl supports the properties as @ref xrc_wxspinbutton.


@subsubsection xrc_wxsplitterwindow wxSplitterWindow

@beginTable
@hdr3col{property, type, description}
@row3col{orientation, @ref xrc_format_type_string,
    Orientation of the splitter, either "vertical" or "horizontal" (default: horizontal).}
@row3col{sashpos, integer,
    Initial position of the sash (default: 0).}
@row3col{minsize, integer,
    Minimum child size (default: not set).}
@row3col{minsize, @ref xrc_format_type_float,
    Sash gravity, see wxSplitterWindow::SetSashGravity() (default: not set).}
@endTable

wxSplitterWindow must have one or two children that are non-toplevel window
objects. If there's only one child, it is used as wxSplitterWindow's only
visible child. If there are two children, the first one is used for left/top
child and the second one for right/bottom child window.


@subsubsection xrc_wxsearchctrl wxSearchCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_text,
    Initial value of the control (default: empty).}
@endTable


@subsubsection xrc_wxstatusbar wxStatusBar

@beginTable
@hdr3col{property, type, description}
@row3col{fields, integer,
    Number of status bar fields (default: 1).}
@row3col{widths, @ref xrc_format_type_string,
    Comma-separated list of @em fields integers. Each value specifies width
    of one field; the values are interpreted using the same convention used
    by wxStatusBar::SetStatusWidths().}
@row3col{styles, @ref xrc_format_type_string,
    Comma-separated list of @em fields flags. Each value specifies status bar
    fieldd style and can be one of @c  wxSB_NORMAL, @c wxSB_FLAT or
    @c wxSB_RAISED. See wxStatusBar::SetStatusStyles() for their description.}
@endTable



@subsubsection xrc_wxstaticbitmap wxStaticBitmap

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap to display (required).}
@endTable

@subsubsection xrc_wxstaticbox wxStaticBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Static box's label (required).}
@endTable


@subsubsection xrc_wxstaticline wxStaticLine

No additional properties.


@subsubsection xrc_wxstatictext wxStaticText

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to display (required).}
@row3col{wrap, integer,
     Number of characters per line to wrap the text for, see
     wxStaticText::Wrap() (default: no wrap).}
@endTable

@subsubsection xrc_wxtextctrl wxTextCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref xrc_format_type_text,
    Initial value of the control (default: empty).}
@row3col{maxlength, integer,
    Maximum length of the text which can be entered by user (default: unlimited).}
@endTable


@subsubsection xrc_wxtogglebuttton wxToggleButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref xrc_format_type_text,
     Label to display on the button (required).}
@row3col{checked, @ref xrc_format_type_bool,
     Should the button be checked/pressed initially (default: 0)?}
@endTable

@subsubsection xrc_wxtoolbar wxToolBar

@beginTable
@hdr3col{property, type, description}
@row3col{bitmapsize, @ref xrc_format_type_size,
    Size of toolbar bitmaps (default: not set).}
@row3col{margins, @ref xrc_format_type_size,
    Margins (default: platform default).}
@row3col{packing, integer,
    Packing, see wxToolBar::SetToolPacking() (default: not set).}
@row3col{separation, integer,
    Default separator size, see wxToolBar::SetToolSeparation() (default: not set).}
@row3col{dontattachtoframe, @ref xrc_format_type_bool,
    If set to 0 and the toolbar object is child of a wxFrame,
    wxFrame::SetToolBar() is called; otherwise, you have to add it to a frame
    manually. The toolbar is attached by default, you have to set this property
    to 1 to disable this behaviour (default: 0).}
@endTable

A toolbar can have one or more child objects of any wxControl-derived class or
one of two pseudo-classes: @c separator or @c tool.

The @c separator pseudo-class is used to insert separators into the toolbar and
has neither properties nor children.

The @c tool pseudo-class objects specify toolbar buttons and have the following
properties:

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref xrc_format_type_bitmap,
    Tool's bitmap (required).}
@row3col{bitmap2, @ref xrc_format_type_bitmap,
    Bitmap for disabled tool (default: derived from @c bitmap).}
@row3col{label, @ref xrc_format_type_text,
    Label to display on the tool (default: no label).}
@row3col{radio, @ref xrc_format_type_bool,
     Item's kind is wxITEM_RADIO (default: 0)?}
@row3col{toggle, @ref xrc_format_type_bool,
     Item's kind is wxITEM_CHECK (default: 0)?}
@row3col{tooltip, @ref xrc_format_type_text,
    Tooltip to use for the tool (default: none).}
@row3col{longhelp, @ref xrc_format_type_text,
    Help text shown in statusbar when the mouse is on the tool (default: none).}
@row3col{disabled, @ref xrc_format_type_bool,
     Is the tool initially disabled (default: 0)?}
@endTable

@c radio and @c toggle are mutually exclusive.

Children that are neither @c tool nor @c separator must be instances of classes
derived from wxControl and are added to the toolbar using
wxToolBar::AddControl().

Example:
@code
<object class="wxToolBar">
    <style>wxTB_FLAT|wxTB_NODIVIDER</style>
    <object class="tool" name="foo">
        <bitmap>foo.png</bitmap>
        <label>Foo</label>
    </object>
    <object class="tool" name="bar">
        <bitmap>bar.png</bitmap>
        <label>Bar</label>
    </object>
    <object class="separator"/>
    <object class="wxComboBox">
        <content>
            <item>Just</item>
            <item>a combobox</item>
            <item>in the toolbar</item>
        </content>
    </object>
</object>

@endcode


@subsubsection xrc_wxtreectrl wxTreeCtrl

No additional properties.


@subsubsection xrc_wxtreebook wxTreebook

No additional properties.

A treebook can have one or more child objects of the @c treebookpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage). @c treebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{depth, integer,
     Page's depth in the labels tree (required; see below).}
@row3col{label, @ref xrc_format_type_text,
     Sheet page's title (required).}
@row3col{bitmap, @ref xrc_format_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref xrc_format_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c treebookpage has exactly one non-toplevel window as its child.

The tree of labels is not described using nested @c treebookpage objects, but
using the @em depth property. Toplevel pages have depth 0, their child pages
have depth 1 and so on. A @c treebookpage's label is inserted as child of
the latest preceding page with depth equal to @em depth-1. For example, this
XRC markup:

@code
<object class="wxTreebook">
  ...
  <object class="treebookpage">
    <depth>0</depth>
    <label>Page 1</label>
    <object class="wxPanel">...</object>
  </object>
  <object class="treebookpage">
    <depth>1</depth>
    <label>Subpage 1A</label>
    <object class="wxPanel">...</object>
  </object>
  <object class="treebookpage">
    <depth>2</depth>
    <label>Subsubpage 1</label>
    <object class="wxPanel">...</object>
  </object>
  <object class="treebookpage">
    <depth>1</depth>
    <label>Subpage 1B</label>
    <object class="wxPanel">...</object>
  </object>
  <object class="treebookpage">
    <depth>2</depth>
    <label>Subsubpage 2</label>
    <object class="wxPanel">...</object>
  </object>
  <object class="treebookpage">
    <depth>0</depth>
    <label>Page 2</label>
    <object class="wxPanel">...</object>
  </object>
</object>
@endcode

corresponds to the following tree of labels:

 - Page 1
   - Subpage 1A
     - Subsubpage 1
   - Subpage 1B
     - Subsubpage 2
 - Page 2


@subsubsection xrc_wxwizard wxWizard

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref xrc_format_type_bitmap,
    Bitmap to display on the left side of the wizard (default: none).}
@endTable

A wizard object can have one or more child objects of the wxWizardPage or
wxWizardPageSimple classes. They both support the following properties
(in addition to @ref xrc_format_std_props):

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref xrc_format_type_bitmap,
    Page-specific bitmap (default: none).}
@endTable

wxWizardPageSimple pages are automatically chained together; wxWizardPage pages
transitions must be handled programatically.


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
