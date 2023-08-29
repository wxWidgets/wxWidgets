/////////////////////////////////////////////////////////////////////////////
// Name:        xrc_format.h
// Purpose:     XRC format specification
// Author:      Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/*
    NOTE: To make doxygen happy about <custom-tags> we're forced to
          escape all < and > symbols which appear inside a doxygen comment.
          Also, don't use < and > symbols in section titles.
*/


/**

@page overview_xrcformat XRC File Format

@tableofcontents

This document describes the format of XRC resource files, as used by
wxXmlResource.

Formal description in the form of a RELAX NG schema is located in the
@c misc/schema subdirectory of the wxWidgets sources.

XRC file is a XML file with all of its elements in the
@c http://www.wxwidgets.org/wxxrc namespace. For backward compatibility,
@c http://www.wxwindows.org/wxxrc namespace is accepted as well (and treated
as identical to @c http://www.wxwidgets.org/wxxrc), but it shouldn't be used
in new XRC files.

XRC file contains definitions for one or more @em objects -- typically
windows. The objects may themselves contain child objects.

Objects defined at the top level, under the
@ref overview_xrcformat_root "root element", can be accessed using
wxXmlResource::LoadDialog() and other LoadXXX methods. They must have
@c name attribute that is used as LoadXXX's argument (see
@ref overview_xrcformat_object for details).

Child objects are not directly accessible via wxXmlResource, they can only
be accessed using XRCCTRL().



@section overview_xrcformat_root Resource Root Element

The root element is always @c \<resource\>. It has one optional attribute, @c
version which, while optional, should always be set to the latest version. At
the time of writing, it is @c "2.5.3.0", so all XRC documents should look like
the following:

@code
<?xml version="1.0"?>
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
    ...
</resource>
@endcode

The version consists of four integers separated by periods.  The first three
components are major, minor and release number of the wxWidgets release when
the change was introduced, the last one is revision number and is 0 for the
first incompatible change in given wxWidgets release, 1 for the second and so
on.  The version changes only if there was an incompatible change introduced;
merely adding new kind of objects does not constitute incompatible change.

@c \<resource\> may have arbitrary number of
@ref overview_xrcformat_objects "object elements" as its children; they are referred
to as @em toplevel objects in the rest of this document. Unlike objects defined
deeper in the hierarchy, toplevel objects @em must have their @c name attribute
set and it must be set to a value unique among root's children.



@section overview_xrcformat_objects Defining Objects

@subsection overview_xrcformat_object Object Element

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
See @ref overview_xrcformat_extending_subclass for more details.

@c \<object\> element may -- and almost always do -- have children elements.
These come in two varieties:

 -# Object's properties. A @em property is a value describing part of object's
    behaviour, for example the "label" property on wxButton defines its label.
    In the most common form, property is a single element with text content
    ("\<label\>Cancel\</label\>"), but they may use nested subelements too (e.g.
    @ref overview_xrcformat_type_font "font property"). A property can only be
    listed once in an object's definition.
 -# Child objects. Window children, sizers, sizer items or notebook pages
    are all examples of child objects. They are represented using nested
    @c \<object\> elements and are can be repeated more than once. The specifics
    of which object classes are allowed as children are class-specific and
    are documented below in @ref overview_xrcformat_controls.

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


@subsection overview_xrcformat_object_ref Object References

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
</object_ref>
@endcode
is identical to:
@code
<object class="wxDialog" name="my_dlg">
    <title>My dialog</title>
    <size>400,400</size>
    <centered>1</centered>
</object>
@endcode


@section overview_xrcformat_datatypes Data Types

There are several property data types that are frequently reused by different
properties. Rather than describing their format in the documentation of
every property, we list commonly used types in this section and document
their format.


@subsection overview_xrcformat_type_bool Boolean

Boolean values are expressed using either "1" literal (true) or "0" (false).


@subsection overview_xrcformat_type_float Floating-point value

Floating point values use POSIX (C locale) formatting -- decimal separator
is "." regardless of the locale.


@subsection overview_xrcformat_type_colour Colour

A single colour can be either any string colour representation accepted
by wxColour::Set() or any wxSYS_COLOUR_XXX symbolic name accepted by
wxSystemSettings::GetColour(). In particular, the following forms are supported:

@li named colours from wxColourDatabase
@li HTML-like "#rrggbb" syntax (but not "#rgb")
@li CSS-style "rgb(r,g,b)" and "rgba(r,g,b,a)"
@li wxSYS_COLOUR_XXX symbolic names

Moreover, a single colour definition in XRC may contain more than one colour,
separated by `|` (pipe symbol), with the first colour used by default and the
subsequent colours in specific situations. Currently the only supported
alternative colour is the colour to be used in dark mode, which must be
prefixed with "dark:".

It is recommended to provide both light and dark values when not using system
colour names (that already adapt to the dark mode), as it's rare for the same
colour to look well in both light and dark mode.

Some examples:
@code
<fg>red</fg>
<fg>#ff0000</fg>
<fg>rgb(192,192,192)|dark:#404040</fg>
<fg>wxSYS_COLOUR_HIGHLIGHT</fg>
@endcode


@subsection overview_xrcformat_type_size Size

Sizes and positions can be expressed in either @ref wxWindow::FromDIP()
"DPI-independent pixel values" or in @ref wxWindow::ConvertDialogToPixels()
"dialog units". The former is the default, to use the latter "d" suffix can be
added. Semi-formally the format is:

  size := x "," y ["d"]

where x and y are integers. Either of the components (or both) may be "-1" to
signify default value. As a shortcut, empty string is equivalent to "-1,-1"
(= wxDefaultSize or wxDefaultPosition).

Notice that the dialog unit suffix "d" applies to both @c x and @c y if it's
specified and cannot be specified after the first component, but only at the
end.

Examples:
@code
42,-1
100,100
100,50d
@endcode

@subsection overview_xrcformat_type_pos Position

Same as @ref overview_xrcformat_type_size.


@subsection overview_xrcformat_type_dimension Dimension

Similarly to @ref overview_xrcformat_type_size "sizes", dimensions are expressed
as integers with optional "d" suffix. When "d" suffix is used, the integer
preceding it is interpreted as dialog units in the parent window, otherwise
it's a DPI-independent pixel value.


@subsection overview_xrcformat_type_pair_ints Pair of integers

This is similar to @ref overview_xrcformat_type_size size, but for values that
are not expressed in pixels and so doesn't allow "d" suffix nor does any
DPI-dependent scaling, i.e. the format is just

  size := x "," y

and @c x and @c y are just integers which are not interpreted in any way.


@subsection overview_xrcformat_type_text Text

String properties use several escape sequences that are translated according to
the following table:
@beginDefList
@itemdef{ "_", "&" (used for accelerators in wxWidgets) }
@itemdef{ "__", "_" }
@itemdef{ "\n", line break }
@itemdef{ "\r", carriage return }
@itemdef{ "\t", tab }
@itemdef{ \"\\\", backslash }
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

@see @ref overview_xrcformat_pre_v2530, @ref overview_xrcformat_pre_v2301


@subsection overview_xrcformat_type_text_notrans Non-Translatable Text

Like @ref overview_xrcformat_type_text, but the text is never translated and
@c translate attribute cannot be used.


@subsection overview_xrcformat_type_string String

An unformatted string. Unlike with @ref overview_xrcformat_type_text, no escaping
or translations are done.


@subsection overview_xrcformat_type_url URL

Any URL accepted by wxFileSystem (typically relative to XRC file's location,
but can be absolute too). Unlike with @ref overview_xrcformat_type_text, no escaping
or translations are done.


@subsection overview_xrcformat_type_bitmap Bitmap

Bitmap properties contain specification of a single bitmap, icon, a set of bitmaps
or SVG file. In the most basic form, their text value is simply a relative URL of
the bitmap to use.
For example:
@code
<object class="tool" name="wxID_NEW">
    <tooltip>New</tooltip>
    <bitmap>new.png</bitmap>
</object>
@endcode
The value is interpreted as path relative to the location of XRC file where the
reference occurs, but notice that it is still an URL and not just a filename,
which means that the characters special in the URLs, such as @c '#' must be
percent-encoded, e.g. here is the correct way to specify a bitmap with the path
@c "images/#1/tool.png" in XRC:
@code
<object class="tool" name="first">
    <bitmap>images/%231/tool.png</bitmap>
</object>
@endcode

Bitmap file paths can include environment variables that are expanded if
wxXRC_USE_ENVVARS was passed to the wxXmlResource constructor.

It is possible to specify the multi-resolution bitmap by a set of bitmaps or
an SVG file, which are mutually exclusive. The set of bitmaps should contain
one or more relative URLs of a bitmap, separated by @c ';'.
For example, to specify two bitmaps, to be used in standard and 200% DPI
scaling respectively, you could write:
@code
<bitmap>new.png;new_2x.png</bitmap>
@endcode

Here the first bitmap is special, as its size determines the logical size of
the bitmap. In other words, this bitmap is the one used when DPI scaling
is not in effect. Any subsequent bitmaps can come in any order and will be used
when the DPI scaling factor is equal, or at least close, to the ratio of their
size to the size of the first bitmap. Using @c _2x naming convention here is common,
but @e not required, the names of the bitmaps can be arbitrary, e.g.
@code
<bitmap>new_32x32.png;new_64x64.png</bitmap>
@endcode
would work just as well.
When using SVG file you must also specify @c default_size attribute
(even if the size is specified in SVG file, it may be different from the size needed here):
@code
<bitmap default_size="32,32">new.svg</bitmap>
@endcode

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

If both specifications are provided, then @c stock_id is used if it is
recognized by wxArtProvider and the provided bitmap file is used as a fallback.


@subsection overview_xrcformat_type_style Style

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


@subsection overview_xrcformat_type_showeffect Show Effect

One of the @ref wxShowEffect values.

Example:
@code
<showeffect>wxSHOW_EFFECT_EXPAND</showeffect>
@endcode


@subsection overview_xrcformat_type_font Font

XRC uses similar, but more flexible, abstract description of fonts to that
used by wxFont class. A font can be described either in terms of its elementary
properties, or it can be derived from one of system fonts or the parent window
font.

The font property element is a "composite" element: unlike majority of
properties, it doesn't have text value but contains several child elements
instead. These children are handled in the same way as object properties
and can be one of the following "sub-properties":

@beginTable
@hdr3col{property, type, description}
@row3col{size, float,
    Pixel size of the font (default: wxNORMAL_FONT's size or @c sysfont's
    size if the @c sysfont property is used or the current size of the font
    of the enclosing control if the @c inherit property is used. Note that
    versions of wxWidgets until 3.1.2 only supported integer values for the
    font size.}
@row3col{style, enum,
    One of "normal", "italic" or "slant" (default: normal).}
@row3col{weight, enum or integer,
    One of "thin", "extralight", "light", "normal", "medium", "semibold",
    "bold", "extrabold", "heavy", "extraheavy", corresponding to the similarly
    named elements of ::wxFontWeight enum, or a numeric value between 1 and
    1000 (default: normal). Note that versions of wxWidgets until 3.1.2 only
    supported "light", "normal" and "bold" values for weight.}
@row3col{family, enum,
    One of "default", "roman", "script", "decorative", "swiss", "modern" or "teletype"
    (default: default).}
@row3col{underlined, @ref overview_xrcformat_type_bool,
    Whether the font should be underlined (default: 0).}
@row3col{strikethrough, @ref overview_xrcformat_type_bool,
    Whether the strikethrough font should be used (default: 0). @since 3.1.2}
@row3col{face, ,
    Comma-separated list of face names; the first one available is used
    (default: unspecified).}
@row3col{encoding, ,
    Charset of the font, unused in Unicode build), as string
    (default: unspecified).}
@row3col{sysfont, ,
    Symbolic name of system standard font(one of wxSYS_*_FONT constants).}
@row3col{inherit, @ref overview_xrcformat_type_bool,
    If true, the font of the enclosing control is used. If this property and the
    @c sysfont property are specified the @c sysfont property takes precedence.}
@row3col{relativesize, float,
    Float, font size relative to chosen system font's or inherited font's size;
    can only be used when 'sysfont' or 'inherit' is used and when 'size' is not
    used.}
@endTable

All of them are optional, if they are missing, appropriate wxFont default is
used. If the @c sysfont or @c inherit property is used, then the defaults are
taken from it instead.

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

@note You cannot use @c inherit for a font that gets used before the enclosing
      control is created, e.g. if the control gets the font passed as parameter
      for its constructor, or if the control is not derived from wxWindow.


@subsection overview_xrcformat_type_imagelist Image List

Defines a wxImageList.

The imagelist property element is a "composite" element: unlike majority of
properties, it doesn't have text value but contains several child elements
instead. These children are handled similarly to object properties
and can be one of the following "sub-properties":

@beginTable
@hdr3col{property, type, description}
@row3col{mask, @ref overview_xrcformat_type_bool,
     If masks should be created for all images (default: 1).}
@row3col{size, @ref overview_xrcformat_type_size,
     The size of the images in the list (default: the size of the first bitmap).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Adds a new image. Unlike normal object properties, @c bitmap may be used more than once
     to add multiple images to the list. At least one @c bitmap value is required.}
@endTable

Example:
@code
<imagelist>
    <size>32,32</size>
    <bitmap stock_id="wxART_QUESTION"/>
    <bitmap stock_id="wxART_INFORMATION"/>
</imagelist>
@endcode


@subsection overview_xrcformat_type_extra_accels Accelerators List

Defines a list of wxMenuItem's extra accelerators.

The extra-accels property element is a "composite" element:
it contains one or more @c \<accel\> "sub-properties":

@beginTable
@hdr3col{property, type, description}
@row3col{accel, @ref overview_xrcformat_type_text_notrans,
     wxMenuItem's accelerator (default: none).}
@endTable

Example:
@code
<extra-accels>
    <accel>Ctrl-W</accel>
    <accel>Shift-Ctrl-W</accel>
</extra-accels>
@endcode


@section overview_xrcformat_windows Controls and Windows

This section describes support wxWindow-derived classes in XRC format.

@subsection overview_xrcformat_std_props Standard Properties

The following properties are always (unless stated otherwise in
control-specific docs) available for @em windows objects. They are omitted
from properties lists below.

@beginTable
@hdr3col{property, type, description}
@row3col{pos, @ref overview_xrcformat_type_pos,
    Initial position of the window (default: wxDefaultPosition).}
@row3col{size, @ref overview_xrcformat_type_size,
    Initial size of the window (default: wxDefaultSize).}
@row3col{style, @ref overview_xrcformat_type_style,
    Window style for this control. The allowed values depend on what
    window is being created, consult respective class' constructor
    documentation for details (default: window-dependent default, usually
    wxFOO_DEFAULT_STYLE if defined for class wxFoo, 0 if not).}
@row3col{exstyle, @ref overview_xrcformat_type_style,
    Extra style for the window, if any. See wxWindow::SetExtraStyle()
    (default: not set).}
@row3col{fg, @ref overview_xrcformat_type_colour,
    Foreground colour of the window (default: window's default).}
@row3col{ownfg, @ref overview_xrcformat_type_colour,
    Non-inheritable foreground colour of the window, see
    wxWindow::SetOwnForegroundColour() (default: none).}
@row3col{bg, @ref overview_xrcformat_type_colour,
    Background colour of the window (default: window's default).}
@row3col{ownbg, @ref overview_xrcformat_type_colour,
    Non-inheritable background colour of the window, see
    wxWindow::SetOwnBackgroundColour() (default: none).}
@row3col{enabled, @ref overview_xrcformat_type_bool,
    If set to 0, the control is disabled (default: 1).}
@row3col{focused, @ref overview_xrcformat_type_bool,
    If set to 1, the control has focus initially (default: 0).}
@row3col{hidden, @ref overview_xrcformat_type_bool,
    If set to 1, the control is created hidden (default: 0).}
@row3col{tooltip, @ref overview_xrcformat_type_text,
    Tooltip to use for the control (default: not set).}
@row3col{variant, @ref overview_xrcformat_type_string,
    Window variant (see wxWindow::SetWindowVariant()), one of "normal", "small", "mini" or "large" (default: "normal")
    @since 3.0.2}
@row3col{font, @ref overview_xrcformat_type_font,
    Font to use for the control (default: window's default).}
@row3col{ownfont, @ref overview_xrcformat_type_font,
    Non-inheritable font to use for the control, see
    wxWindow::SetOwnFont() (default: none).}
@row3col{help, @ref overview_xrcformat_type_text,
    Context-sensitive help for the control, used by wxHelpProvider
    (default: not set).}
@endTable

All of these properties are optional.


@subsection overview_xrcformat_controls Supported Controls

This section lists all controls supported by default. For each control, its
control-specific properties are listed. If the control can have child objects,
it is documented there too; unless said otherwise, XRC elements for these
controls cannot have children.

@subsubsection xrc_activityindicator wxActivityIndicator

@beginTable
@hdr3col{property, type, description}
@row3col{running, @ref overview_xrcformat_type_bool,
    If true, start the activity indicator after creating it (default: false).}
@endTable

@subsubsection xrc_wxanimationctrl wxAnimationCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{animation, @ref overview_xrcformat_type_url,
    Animation file to load into the control or, since wxWindow 3.3.0, multiple
    semicolon-separated files in order of increasing size, corresponding to
    multiple versions of the animation for different resolutions (default: none).}
@row3col{inactive-bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to use when not playing the animation (default: the default).}
@endTable


@subsubsection xrc_wxauimanager wxAuiManager

Notice that wxAUI support in XRC is available in wxWidgets 3.1.1 and
later only and you need to explicitly register its handler using
@code
    #include <wx/xrc/xh_aui.h>

    AddHandler(new wxAuiXmlHandler);
@endcode
to use it.

A wxAuiManager can have one or more child objects of the
wxAuiPaneInfo class.
wxAuiPaneInfo objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{caption, @ref overview_xrcformat_type_text,
    Sets the caption of the pane.}
@row3col{caption_visible, @ref overview_xrcformat_type_bool,
    Indicates that a pane caption should be visible.}
@row3col{close_button, @ref overview_xrcformat_type_bool,
    Indicates that a close button should be drawn for the pane.}
@row3col{maximize_button, @ref overview_xrcformat_type_bool,
    Indicates that a maximize button should be drawn for the pane.}
@row3col{minimize_button, @ref overview_xrcformat_type_bool,
    Indicates that a minimize button should be drawn for the pane.}
@row3col{pin_button, @ref overview_xrcformat_type_bool,
    Indicates that a pin button should be drawn for the pane.}
@row3col{gripper, @ref overview_xrcformat_type_bool,
    Indicates that a gripper should be drawn for the pane.}
@row3col{pane_border, @ref overview_xrcformat_type_bool,
    Indicates that a border should be drawn for the pane.}
@row3col{dock, ,
    Indicates that a pane should be docked.}
@row3col{float, ,
    Indicates that a pane should be floated.}
@row3col{top_dockable, @ref overview_xrcformat_type_bool,
    Indicates whether a pane can be docked at the top of the frame.}
@row3col{bottom_dockable, @ref overview_xrcformat_type_bool,
    Indicates whether a pane can be docked at the bottom of the frame.}
@row3col{left_dockable, @ref overview_xrcformat_type_bool,
    Indicates whether a pane can be docked on the left of the frame.}
@row3col{right_dockable, @ref overview_xrcformat_type_bool,
    Indicates whether a pane can be docked on the right of the frame.}
@row3col{dock_fixed, @ref overview_xrcformat_type_bool,
    Causes the containing dock to have no resize sash.}
@row3col{resizable, @ref overview_xrcformat_type_bool,
    Allows a pane to be resized if the parameter is @true, and forces it
    to be a fixed size if the parameter is @false.}
@row3col{movable, @ref overview_xrcformat_type_bool,
    Indicates whether a pane can be moved.}
@row3col{floatable, @ref overview_xrcformat_type_bool,
    Sets whether the user will be able to undock a pane and turn it
    into a floating window.}
@row3col{best_size, @ref overview_xrcformat_type_size,
    Sets the ideal size for the pane.}
@row3col{floating_size, @ref overview_xrcformat_type_size,
    Sets the size of the floating pane.}
@row3col{min_size, @ref overview_xrcformat_type_size,
    Sets the minimum size of the pane.}
@row3col{max_size, @ref overview_xrcformat_type_size,
    Sets the maximum size of the pane.}
@row3col{default_pane, ,
    Specifies that the pane should adopt the default pane settings.}
@row3col{toolbar_pane, ,
    Specifies that the pane should adopt the default toolbar pane settings.}
@row3col{layer, , Determines the layer of the docked pane.}
@row3col{row, , Determines the row of the docked pane.}
@row3col{center_pane, ,
    Specifies that the pane should adopt the default center pane settings.}
@row3col{centre_pane, ,
    Same as center_pane.}
@row3col{direction, ,
    Determines the direction of the docked pane.}
@row3col{top, ,
    Sets the pane dock position to the top of the frame.}
@row3col{bottom, ,
    Sets the pane dock position to the bottom side of the frame.}
@row3col{left, ,
    Sets the pane dock position to the left side of the frame.}
@row3col{right, ,
    Sets the pane dock position to the right side of the frame.}
@row3col{center, ,
    Sets the pane dock position to the center of the frame.}
@row3col{centre, ,
    Same as center.}
@endTable


@subsubsection xrc_wxauinotebook wxAuiNotebook

@beginTable
@hdr3col{property, type, description}
@row3col{art-provider, @ref overview_xrcformat_type_string,
    One of @c default for wxAuiDefaultTabArt or @c simple for wxAuiSimpleTabArt
    (default: @c default).
    @since 3.2.0}
@endTable

A wxAuiNotebook can have one or more child objects of the @c notebookpage
pseudo-class.
@c notebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Page label (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c notebookpage must have exactly one non-toplevel window as its child.

Example:
@code
<object class="wxAuiNotebook">
    <style>wxBK_BOTTOM</style>
    <object class="notebookpage">
        <label>Page 1</label>
        <bitmap>bitmap.png</bitmap>
        <object class="wxPanel" name="page_1">
            ...
        </object>
    </object>
</object>
@endcode

@note See @ref xrc_wxauimanager about using wxAUI classes in XRC.

@subsubsection xrc_wxauitoolbar wxAuiToolBar

Building an XRC for wxAuiToolBar is quite similar to wxToolBar.
The only significant differences are:
@li the use of the class name wxAuiToolBar
@li the styles supported are the ones described in the wxAuiToolBar class definition
@li the 'space' pseudo-class has two optional, mutually exclusive,
integer properties: 'proportion' and 'width'. If 'width' is specified, a space
is added using wxAuiToolBar::AddSpacer(); if 'proportion', the value is used in
wxAuiToolBar::AddStretchSpacer(). If neither are provided, the default is a
stretch-spacer with a proportion of 1.
@li there is an additional pseudo-class, 'label', that has a string property.
See wxAuiToolBar::AddLabel().

Refer to the section @ref xrc_wxtoolbar for more details.

@note The XML Handler should be explicitly registered:
@code
    #include <wx/xrc/xh_auitoolb.h>

    AddHandler(new wxAuiToolBarXmlHandler);
@endcode

@since 3.1.0

@subsubsection xrc_wxbannerwindow wxBannerWindow

@beginTable
@hdr3col{property, type, description}
@row3col{direction, @c wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,
    The side along which the banner will be positioned (default: wxLEFT).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to use as the banner background (default: none).}
@row3col{title, @ref overview_xrcformat_type_text,
    Banner title, should be single line (default: none).}
@row3col{message, @ref overview_xrcformat_type_text,
    Possibly multi-line banner message (default: none).}
@row3col{gradient-start, @ref overview_xrcformat_type_colour,
    Starting colour of the gradient used as banner background.
    (Optional. Can't be used if a valid bitmap is specified. If used, both gradient values must be set.)}
@row3col{gradient-end, @ref overview_xrcformat_type_colour,
    End colour of the gradient used as banner background.
    (Optional. Can't be used if a valid bitmap is specified. If used, both gradient values must be set.)}
@endTable


@subsubsection xrc_wxbitmapbutton wxBitmapButton

@beginTable
@hdr3col{property, type, description}
@row3col{default, @ref overview_xrcformat_type_bool,
     Should this button be the default button in dialog (default: 0)?}
@row3col{close, @ref overview_xrcformat_type_bool,
     If set, this is a special "Close" button using system-defined appearance,
     see wxBitmapButton::NewCloseButton(). If this property is set, @c bitmap
     and @c style are ignored and shouldn't be used. @since 3.1.5}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap to show on the button (default: none).}
@row3col{pressed, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is pressed (default: none, same as @c bitmap).
     This property exists since wxWidgets 3.1.6, but the equivalent (and still
     supported) "selected" property can be used in the older versions.}
@row3col{focus, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap).}
@row3col{disabled, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap).}
@row3col{current, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when mouse cursor hovers above the bitmap (default: none, same as @c bitmap).
     This property exists since wxWidgets 3.1.6, but the equivalent (and still
     supported) "hover" property can be used in the older versions.}
@endTable


@subsubsection xrc_wxbitmapcombobox wxBitmapComboBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{value, @ref overview_xrcformat_type_string,
     Initial value in the control (doesn't have to be one of @ content values;
     default: empty).}
@endTable

If both @c value and @c selection are specified and @c selection is not -1,
then @c selection takes precedence.

A wxBitmapComboBox can have one or more child objects of the @c ownerdrawnitem
pseudo-class. @c ownerdrawnitem objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{text, @ref overview_xrcformat_type_text,
     Item's label (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
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


@subsubsection xrc_wxbitmaptogglebutton wxBitmapToggleButton

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Label to display on the button (default: none).}
@row3col{pressed, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is pressed (default: none, same as @c bitmap). @since 3.1.7}
@row3col{focus, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap). @since 3.1.7}
@row3col{disabled, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap). @since 3.1.7}
@row3col{current, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the mouse cursor hovers above the bitmap (default: none, same as @c bitmap). @since 3.1.7}
@row3col{margins, @ref overview_xrcformat_type_size,
    Set the margins between the bitmap and the text of the button.
    This method is currently only implemented under MSW. If it is not called, a default margin is used around the bitmap. @since 3.1.7}
@row3col{checked, @ref overview_xrcformat_type_bool,
     Should the button be checked/pressed initially (default: 0)?}
@endTable


@subsubsection xrc_wxbutton wxButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
    Label to display on the button (may be omitted if only the bitmap or stock ID is used).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to display in the button (optional).}
@row3col{pressed, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is pressed (default: none, same as @c bitmap). @since 3.1.7}
@row3col{focus, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap). @since 3.1.7}
@row3col{disabled, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap). @since 3.1.7}
@row3col{current, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the mouse cursor hovers above the bitmap (default: none, same as @c bitmap). @since 3.1.7}
@row3col{margins, @ref overview_xrcformat_type_size,
    Set the margins between the bitmap and the text of the button.
    This method is currently only implemented under MSW. If it is not called, a default margin is used around the bitmap. @since 3.1.7}
@row3col{bitmapposition, @c wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,
    Position of the bitmap in the button, see wxButton::SetBitmapPosition() (default: wxLEFT).}
@row3col{default, @ref overview_xrcformat_type_bool,
    Should this button be the default button in dialog (default: 0)?}
@endTable


@subsubsection xrc_wxcalendarctrl wxCalendarCtrl

No additional properties.


@subsubsection xrc_wxcheckbox wxCheckBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label to use for the checkbox (default: empty).}
@row3col{checked, integer,
    Sets the initial state of the checkbox. 0 is unchecked (default),
    1 is checked, and since wxWidgets 3.1.7, 2 sets the undetermined
    state of a 3-state checkbox.}
@endTable


@subsubsection xrc_wxchecklistbox wxCheckListBox

@beginTable
@hdr3col{property, type, description}
@row3col{content, items,
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
@row3col{content, items,
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

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none, built implicitly).}
@endTable

Additionally, a choicebook can have one or more child objects of the @c
choicebookpage pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and
its @c notebookpage).

@c choicebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Sheet page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label (default: none, mutually exclusive with @c image).}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    into the image list (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c choicebookpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxcollapsiblepane wxCollapsiblePane

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label to use for the collapsible section (default: empty).}
@row3col{collapsed, @ref overview_xrcformat_type_bool,
     Should the pane be collapsed initially (default: 0)?}
@endTable

wxCollapsiblePane may contain single optional child object of the @c panewindow
pseudo-class type. @c panewindow itself must contain exactly one child that
is a @ref overview_xrcformat_sizers "sizer" or a non-toplevel window
object.


@subsubsection xrc_wxcolourpickerctrl wxColourPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_colour,
    Initial value of the control (default: wxBLACK).}
@endTable


@subsubsection xrc_wxcombobox wxComboBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: not used).}
@row3col{content, items,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@row3col{value, @ref overview_xrcformat_type_string,
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


@subsubsection xrc_wxcomboctrl wxComboCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_string,
     Initial value in the control (default: empty).}
@endTable


@subsubsection xrc_wxcommandlinkbutton wxCommandLinkButton

The wxCommandLinkButton contains a main title-like @c label and an optional
@c note for longer description. The main @c label and the @c note can be
concatenated into a single string using a new line character between them
(notice that the @c note part can have more new lines in it).

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
    First line of text on the button, typically the label of an action that
    will be made when the button is pressed (default: empty). }
@row3col{note, @ref overview_xrcformat_type_text,
    Second line of text describing the action performed when the button is pressed (default: none).  }
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to display in the button (optional). @since 3.1.5}
@row3col{pressed, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is pressed (default: none, same as @c bitmap). @since 3.1.7}
@row3col{focus, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap). @since 3.1.7}
@row3col{disabled, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap). @since 3.1.7}
@row3col{current, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the mouse cursor hovers above the bitmap (default: none, same as @c bitmap). @since 3.1.7}
@row3col{default, @ref overview_xrcformat_type_bool,
    Should this button be the default button in dialog (default: 0)? @since 3.1.5}
@endTable


@subsubsection xrc_wxdataviewctrl wxDataViewCtrl

No additional properties.


@subsubsection xrc_wxdataviewlistctrl wxDataViewListCtrl

No additional properties.


@subsubsection xrc_wxdataviewtreectrl wxDataViewTreeCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none).}
@endTable


@subsubsection xrc_wxdatepickerctrl wxDatePickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{null-text, @ref overview_xrcformat_type_string,
    Set the text to show when there is no valid value (default: empty).
    Only used if the control has wxDP_ALLOWNONE style.
    Currently implemented on MSW, ignored elsewhere. @since 3.1.5.}
@endTable


@subsubsection xrc_wxdialog wxDialog

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref overview_xrcformat_type_text,
     Dialog's title (default: empty).}
@row3col{icon, @ref overview_xrcformat_type_bitmap,
     Dialog's icon (default: not used).}
@row3col{centered, @ref overview_xrcformat_type_bool,
     Whether the dialog should be centered on the screen (default: 0).}
@endTable

wxDialog may have optional children: either exactly one
@ref overview_xrcformat_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.


@subsubsection xrc_wxdirpickerctrl wxDirPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_string,
    Initial value of the control (default: empty).}
@row3col{message, @ref overview_xrcformat_type_text,
    Message shown to the user in wxDirDialog shown by the control (default: empty).}
@endTable


@subsubsection xrc_wxeditablelistbox wxEditableListBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label shown above the list (default: empty).}
@row3col{content, items,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (default: empty).}
@endTable

Example:
@code
<object class="wxEditableListBox" name="controls_listbox">
    <size>250,160</size>
    <label>List of things</label>
    <content>
        <item>Milk</item>
        <item>Pizza</item>
        <item>Bread</item>
        <item>Orange juice</item>
        <item>Paper towels</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxfilectrl wxFileCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{defaultdirectory, @ref overview_xrcformat_type_string,
    Sets the current directory displayed in the control (default: empty). }
@row3col{defaultfilename, @ref overview_xrcformat_type_string,
    Selects a certain file (default: empty).}
@row3col{wildcard, @ref overview_xrcformat_type_string,
    Sets the wildcard, which can contain multiple file types, for example:
    "BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"
    (default: all files).}
@endTable


@subsubsection xrc_wxfilepickerctrl wxFilePickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_string,
    Initial value of the control (default: empty).}
@row3col{message, @ref overview_xrcformat_type_text,
    Message shown to the user in wxDirDialog shown by the control (default: empty).}
@row3col{wildcard, @ref overview_xrcformat_type_string,
    Sets the wildcard, which can contain multiple file types, for example:
    "BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"
    (default: all files).}
@endTable


@subsubsection xrc_wxfontpickerctrl wxFontPickerCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_font,
    Initial value of the control (default: wxNORMAL_FONT).}
@endTable

@subsubsection xrc_wxfrane wxFrame

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref overview_xrcformat_type_text,
     Frame's title (default: empty).}
@row3col{icon, @ref overview_xrcformat_type_bitmap,
     Frame's icon (default: not used).}
@row3col{centered, @ref overview_xrcformat_type_bool,
     Whether the frame should be centered on the screen (default: 0).}
@endTable

wxFrame may have optional children: either exactly one
@ref overview_xrcformat_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.


@subsubsection xrc_wxgauge wxGauge

@beginTable
@hdr3col{property, type, description}
@row3col{range, integer,
    Maximum value of the gauge (default: 100).}
@row3col{value, integer,
    Initial value of the control (default: 0).}
@row3col{shadow, @ref overview_xrcformat_type_dimension,
    Ignored, preserved only for compatibility.}
@row3col{bezel, @ref overview_xrcformat_type_dimension,
    Ignored, preserved only for compatibility.}
@endTable

@subsubsection xrc_wxgenericanimationctrl wxGenericAnimationCtrl

This handler is identical to the one for @ref xrc_wxanimationctrl
"wxAnimationCtrl", please see it for more information. The only difference is
that, for the platforms with a native wxAnimationCtrl implementation, using
this handler creates a generic control rather than a native one.


@subsubsection xrc_wxgenericdirctrl wxGenericDirCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{defaultfolder, @ref overview_xrcformat_type_string,
    Initial folder (default: empty).}
@row3col{filter, @ref overview_xrcformat_type_text,
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
@row3col{url, @ref overview_xrcformat_type_url,
    Page to display in the window (default: none).}
@row3col{htmlcode, @ref overview_xrcformat_type_text,
    HTML markup to display in the window (default: none).}
@row3col{borders, @ref overview_xrcformat_type_dimension,
    Border around HTML content (default: 0).}
@endTable

At most one of @c url and @c htmlcode properties may be specified, they are
mutually exclusive. If neither is set, the window is initialized to show empty
page.


@subsubsection xrc_wxhyperlinkctrl wxHyperlinkCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label to display on the control (default: empty).}
@row3col{url, @ref overview_xrcformat_type_url,
     URL to open when the link is clicked (default: empty).}
@endTable


@subsubsection xrc_wxinfobar wxInfoBar

@beginTable
@hdr3col{property, type, description}
@row3col{showeffect, @ref overview_xrcformat_type_showeffect,
     The effect to use when showing the bar (optional).}
@row3col{hideeffect, @ref overview_xrcformat_type_showeffect,
     The effect to use when hiding the bar (optional).}
@row3col{effectduration, integer,
     The duration of the animation used when showing or hiding the bar
     (optional).}
@row3col{button, object,
     Add a button to be shown in the info bar (see wxInfoBar::AddButton);
     this property is of class "button" has name (can be one of standard
     button ID) and has optional label property. If no buttons are added
     to the info bar, the default "Close" button will be shown.}
@endTable

Example:
@code
<object class="wxInfoBar">
    <effectduration>1000</effectduration>
    <showeffect>wxSHOW_EFFECT_EXPAND</showeffect>
    <hideeffect>wxSHOW_EFFECT_SLIDE_TO_RIGHT</hideeffect>
    <object class="button" name="wxID_UNDO"/>
    <object class="button" name="wxID_REDO">
        <label>Redo Custom Label</label>
    </object>
</object>
@endcode

@since 3.1.3


@subsubsection xrc_wxlistbox wxListBox

@beginTable
@hdr3col{property, type, description}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{content, items,
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

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none, built implicitly).}
@endTable

Additionally, a listbook can have one or more child objects of the @c
listbookpage pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and
its @c notebookpage).

@c listbookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Sheet page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label (default: none, mutually exclusive with @c image).}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    into the image list (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c listbookpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxlistctrl wxListCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     The normal (wxIMAGE_LIST_NORMAL) image list (default: none, built implicitly).}
@row3col{imagelist-small, @ref overview_xrcformat_type_imagelist,
     The small (wxIMAGE_LIST_SMALL) image list (default: none, built implicitly).}
@endTable

A list control can have optional child objects of the @ref xrc_wxlistitem
class.  Report mode list controls (i.e. created with @c wxLC_REPORT style) can
in addition have optional @ref xrc_wxlistcol child objects.

@paragraph xrc_wxlistcol listcol

The @c listcol class can only be used for wxListCtrl children. It can have the
following properties (all of them optional):

@beginTable
@hdr3col{property, type, description}
@row3col{align, wxListColumnFormat,
    The alignment for the item.
    Can be one of @c wxLIST_FORMAT_LEFT, @c wxLIST_FORMAT_RIGHT or
    @c wxLIST_FORMAT_CENTRE.}
@row3col{text, @ref overview_xrcformat_type_text,
    The title of the column. }
@row3col{width, integer,
    The column width. @c wxLIST_DEFAULT_COL_WIDTH is used by default. }
@row3col{image, integer,
    The zero-based index of the image associated with the item in the 'small' image list. }
@endTable

The columns are appended to the control in order of their appearance and may be
referenced by 0-based index in the @c col attributes of subsequent @c listitem
objects.

@paragraph xrc_wxlistitem listitem

The @c listitem is a child object for the class @ref xrc_wxlistctrl.
It can have the following properties (all of them optional):

@beginTable
@hdr3col{property, type, description}
@row3col{align, wxListColumnFormat,
    The alignment for the item.
    Can be one of @c wxLIST_FORMAT_LEFT, @c wxLIST_FORMAT_RIGHT or
    @c wxLIST_FORMAT_CENTRE.}
@row3col{bg, @ref overview_xrcformat_type_colour,
    The background color for the item.}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Add a bitmap to the (normal) @ref overview_xrcformat_type_imagelist associated with the
    @ref xrc_wxlistctrl parent and associate it with this item.
    If the imagelist is not defined it will be created implicitly
    (default: none, mutually exclusive with @c image).}
@row3col{bitmap-small, @ref overview_xrcformat_type_bitmap,
    Add a bitmap in the 'small' @ref overview_xrcformat_type_imagelist associated with the
    @ref xrc_wxlistctrl parent and associate it with this item.
    If the 'small' imagelist is not defined it will be created implicitly
    (default: none, mutually exclusive with @c image-small).}
@row3col{col, integer,
    The zero-based column index.}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    in the (normal) image list
    (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{image-small, integer,
    The zero-based index of the image associated with the item
    in the 'small' image list
    (default: none, mutually exclusive with @c bitmap-small, only if imagelist-small was set).}
@row3col{data, integer,
    The client data for the item.}
@row3col{font, @ref overview_xrcformat_type_font,
    The font for the item.}
@row3col{state, @ref overview_xrcformat_type_style,
    The item state. Can be any combination of the following values:
        - @c wxLIST_STATE_FOCUSED: The item has the focus.
        - @c wxLIST_STATE_SELECTED: The item is selected.}
@row3col{text, @ref overview_xrcformat_type_text,
    The text label for the item. }
@row3col{textcolour, @ref overview_xrcformat_type_colour,
    The text colour for the item. }
@endTable

Notice that the item position can't be specified here, the items are appended
to the list control in order of their appearance.


@subsubsection xrc_wxmdiparentframe wxMDIParentFrame

wxMDIParentFrame supports the same properties that @ref xrc_wxfrane does.

wxMDIParentFrame may have optional children. When used, the child objects
must be of wxMDIChildFrame type.


@subsubsection xrc_wxmdichildframe wxMDIChildFrame

wxMDIChildFrame supports the same properties that @ref xrc_wxfrane and
@ref xrc_wxmdiparentframe do.

wxMDIChildFrame can only be used as immediate child of @ref
xrc_wxmdiparentframe.

wxMDIChildFrame may have optional children: either exactly one
@ref overview_xrcformat_sizers "sizer" child or any number of non-toplevel window
objects. If sizer child is used, it sets
@ref wxSizer::SetSizeHints() "size hints" too.


@subsubsection xrc_wxmenu wxMenu

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Menu's label (default: empty, but required for menus other
     than popup menus).}
@row3col{style, @ref overview_xrcformat_type_style,
    Window style for the menu.}
@row3col{help, @ref overview_xrcformat_type_text,
     Help shown in statusbar when the menu is selected (only for submenus
     of another wxMenu, default: none).}
@row3col{enabled, @ref overview_xrcformat_type_bool,
     Is the submenu item enabled (only for submenus of another wxMenu,
     default: 1)?}
@endTable

Note that unlike most controls, wxMenu does @em not have
@ref overview_xrcformat_std_props, with the exception of @c style.

A menu object can have one or more child objects of the wxMenuItem or wxMenu
classes or @c break or @c separator pseudo-classes.

The @c separator pseudo-class is used to insert separators into the menu and
has neither properties nor children. Likewise, @c break inserts a break (see
wxMenu::Break()).

wxMenuItem objects support the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Item's label (may be omitted if stock ID is used).}
@row3col{accel, @ref overview_xrcformat_type_text_notrans,
     Item's accelerator (default: none).}
@row3col{extra-accels, @ref overview_xrcformat_type_extra_accels,
     List of item's extra accelerators. Such accelerators will not be shown
     in item's label, but still will work. (default: none). @since 3.1.6.}
@row3col{radio, @ref overview_xrcformat_type_bool,
     Item's kind is wxITEM_RADIO (default: 0)?}
@row3col{checkable, @ref overview_xrcformat_type_bool,
     Item's kind is wxITEM_CHECK (default: 0)?}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap to show with the item (default: none).}
@row3col{bitmap2, @ref overview_xrcformat_type_bitmap,
     Bitmap for the checked state (wxMSW, if checkable; default: none).}
@row3col{help, @ref overview_xrcformat_type_text,
     Help shown in statusbar when the item is selected (default: none).}
@row3col{enabled, @ref overview_xrcformat_type_bool,
     Is the item enabled (default: 1)?}
@row3col{checked, @ref overview_xrcformat_type_bool,
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
    <extra-accels>
      <accel>Ctrl-W</accel>
      <accel>Shift-Ctrl-W</accel>
    </extra-accels>
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

@beginTable
@hdr3col{property, type, description}
@row3col{style, @ref overview_xrcformat_type_style,
    Window style for the menu bar.}
@endTable

Note that unlike most controls, wxMenuBar does @em not have
@ref overview_xrcformat_std_props, with the exception of @c style.

A menubar can have one or more child objects of the @ref xrc_wxmenu "wxMenu"
class.


@subsubsection xrc_wxnotebook wxNotebook

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none, built implicitly).}
@endTable

A notebook can have one or more child objects of the @c notebookpage
pseudo-class.

@c notebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label
     (default: none, mutually exclusive with @c image).}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    into the image list
    (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{selected, @ref overview_xrcformat_type_bool,
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
@row3col{buttonsize, @ref overview_xrcformat_type_size,
     Size of the dropdown button (default: default).}
@endTable


@subsubsection xrc_wxpanel wxPanel

No additional properties.

wxPanel may have optional children: either exactly one
@ref overview_xrcformat_sizers "sizer" child or any number of non-toplevel window
objects.


@subsubsection xrc_wxpropertysheetdialog wxPropertySheetDialog

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref overview_xrcformat_type_text,
     Dialog's title (default: empty).}
@row3col{icon, @ref overview_xrcformat_type_bitmap,
     Dialog's icon (default: not used).}
@row3col{centered, @ref overview_xrcformat_type_bool,
     Whether the dialog should be centered on the screen (default: 0).}
@row3col{buttons, @ref overview_xrcformat_type_style,
     Buttons to show, combination of flags accepted by
     wxPropertySheetDialog::CreateButtons() (default: 0).}
@endTable

A sheet dialog can have one or more child objects of the @c propertysheetpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage). @c propertysheetpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Sheet page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label (default: none).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c propertysheetpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxradiobutton wxRadioButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label shown on the radio button (default: empty).}
@row3col{value, @ref overview_xrcformat_type_bool,
    Initial value of the control (default: 0).}
@endTable


@subsubsection xrc_wxradiobox wxRadioBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label for the whole box (default: empty).}
@row3col{dimension, integer,
     Specifies the maximum number of rows (if style contains
     @c wxRA_SPECIFY_ROWS) or columns (if style contains @c wxRA_SPECIFY_COLS)
     for a two-dimensional radiobox (default: 1).}
@row3col{selection, integer,
     Index of the initially selected item or -1 for no selection (default: -1).}
@row3col{content, items,
     Content of the control; this property has any number of @c \<item\> XML
     elements as its children, with the items text as their text values
     (see below; default: empty).}
@endTable

The @c \<item\> elements have radio buttons' labels as their text values. They
can also have some optional XML @em attributes (not properties!):

@beginTable
@hdr3col{attribute, type, description}
@row3col{tooltip, @ref overview_xrcformat_type_string,
     Tooltip to show over this radio button (default: none).}
@row3col{helptext, @ref overview_xrcformat_type_string,
     Contextual help for this radio button (default: none).}
@row3col{enabled, @ref overview_xrcformat_type_bool,
     Is the button enabled (default: 1)?}
@row3col{hidden, @ref overview_xrcformat_type_bool,
     Is the button hidden initially (default: 0)?}
@row3col{label, @ref overview_xrcformat_type_bool,
     Should this item text be interpreted as a label, i.e. escaping underscores
     in it as done for the label properties of other controls? This attribute
     exists since wxWidgets 3.1.1 and was always treated as having the value of
     0, which still remains its default, until then.}
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
        <item hidden="1">Very quiet station</item>
    </content>
</object>
@endcode


@subsubsection xrc_wxribbonbar wxRibbonBar

@beginTable
@hdr3col{property, type, description}
@row3col{art-provider, @ref overview_xrcformat_type_string,
    One of @c default, @c aui or @c msw (default: @c default).}
@endTable

A wxRibbonBar may have @ref xrc_wxribbonpage child objects. The @c page
pseudo-class may be used instead of @c wxRibbonPage when used as wxRibbonBar
children.

Example:
@code
<object class="wxRibbonBar" name="ribbonbar">
    <object class="page" name="FilePage">
        <label>First</label>
        <object class="panel">
            <label>File</label>
            <object class="wxRibbonButtonBar">
                <object class="button" name="Open">
                    <bitmap>open.xpm</bitmap>
                    <label>Open</label>
                </object>
            </object>
        </object>
    </object>
    <object class="page" name="ViewPage">
        <label>View</label>
        <object class="panel">
            <label>Zoom</label>
            <object class="wxRibbonGallery">
                <object class="item">
                    <bitmap>zoomin.xpm</bitmap>
                </object>
                <object class="item">
                    <bitmap>zoomout.xpm</bitmap>
                </object>
            </object>
        </object>
    </object>
</object>
@endcode

Notice that wxRibbonBar support in XRC is available in wxWidgets 2.9.5 and
later only and you need to explicitly register its handler using
@code
    #include <wx/xrc/xh_ribbon.h>

    AddHandler(new wxRibbonXmlHandler);
@endcode
to use it.


@subsubsection xrc_wxribbonbuttonbar wxRibbonButtonBar

No additional properties.

wxRibbonButtonBar can have child objects of the @c button pseudo-class. @c button
objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{hybrid, @ref overview_xrcformat_type_bool,
    If true, the @c wxRIBBON_BUTTON_HYBRID kind is used (default: false).}
@row3col{disabled, @ref overview_xrcformat_type_bool,
    Whether the button should be disabled (default: false).}
@row3col{label, @ref overview_xrcformat_type_text,
    Item's label (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Item's bitmap (default: none).}
@row3col{small-bitmap, @ref overview_xrcformat_type_bitmap,
    Small bitmap (default: none).}
@row3col{disabled-bitmap, @ref overview_xrcformat_type_bitmap,
    Disabled bitmap (default: none).}
@row3col{small-disabled-bitmap, @ref overview_xrcformat_type_bitmap,
    Small disabled bitmap (default: none).}
@row3col{help, @ref overview_xrcformat_type_text,
    Item's help text (default: none).}
@endTable


@subsubsection xrc_wxribboncontrol wxRibbonControl

No additional properties.

Objects of this type *must* be subclassed with the @c subclass attribute.


@subsubsection xrc_wxribbongallery wxRibbonGallery

No additional properties.

wxRibbonGallery can have child objects of the @c item pseudo-class. @c item
objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Item's bitmap (default: none).}
@endTable


@subsubsection xrc_wxribbonpage wxRibbonPage

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
    Label (default: none).}
@row3col{icon, @ref overview_xrcformat_type_bitmap,
    Icon (default: none).}
@endTable

A wxRibbonPage may have children of any type derived from wxRibbonControl.
Most commonly, wxRibbonPanel is used. As a special case, the @c panel
pseudo-class may be used instead of @c wxRibbonPanel when used as wxRibbonPage
children.


@subsubsection xrc_wxribbonpanel wxRibbonPanel

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
    Label (default: none).}
@row3col{icon, @ref overview_xrcformat_type_bitmap,
    Icon (default: none).}
@endTable

A wxRibbonPanel may have children of any type derived from wxRibbonControl or
a single wxSizer child with non-ribbon windows in it.


@subsubsection xrc_wxrichtextctrl wxRichTextCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_text,
    Initial value of the control (default: empty).}
@endTable

Notice that you need to explicitly register the handler using
@code
    #include <wx/xrc/xh_richtext.h>

    AddHandler(new wxRichTextCtrlXmlHandler);
@endcode
to use it.

@since 2.9.5

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
@row3col{scrollrate, @ref overview_xrcformat_type_size,
    Scroll rate in @em x and @em y directions (default: not set;
    required if the window has a sizer child).}
@endTable

wxScrolledWindow may have optional children: either exactly one
@ref overview_xrcformat_sizers "sizer" child or any number of non-toplevel window
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


@subsubsection xrc_wxsimplebook wxSimplebook

wxSimplebook is similar to @ref xrc_wxnotebook "wxNotebook" but simpler: as it
doesn't show any page headers, it uses neither image list nor individual
page bitmaps and while it still accepts page labels, they are optional as they
are not shown to the user either.

So @c simplebookpage child elements, that must occur inside this object, only
have the following properties:

@c choicebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Page's label (default: empty).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

As with all the other book page elements, each @c simplebookpage must have
exactly one non-toplevel window as its child.

@since 3.0.2


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
    Page size; number of steps the slider moves when the user moves
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
@row3col{inc, integer,
    Increment (default: 1). @since 3.1.6.}
@endTable


@subsubsection xrc_wxspinctrl wxSpinCtrl

wxSpinCtrl supports the same properties as @ref xrc_wxspinbutton and, since
wxWidgets 2.9.5, another one:
@beginTable
@row3col{base, integer,
    Numeric base, currently can be only 10 or 16 (default: 10).}
@endTable


@subsubsection xrc_wxspinctrldouble wxSpinCtrlDouble

wxSpinCtrlDouble supports the same properties as @ref xrc_wxspinbutton but @c
value, @c min and @a max are all of type float instead of int. There is one
additional property:
@beginTable
@row3col{inc, float,
    The amount by which the number is changed by a single arrow press.}
@row3col{digits, integer,
    Sets the precision of the value of the spin control (default: 0). @since 3.1.7.}
@endTable

@since  3.1.1.


@subsubsection xrc_wxsplitterwindow wxSplitterWindow

@beginTable
@hdr3col{property, type, description}
@row3col{orientation, @ref overview_xrcformat_type_string,
    Orientation of the splitter, either "vertical" or "horizontal" (default: horizontal).}
@row3col{sashpos, @ref overview_xrcformat_type_dimension,
    Initial position of the sash (default: 0).}
@row3col{minsize, @ref overview_xrcformat_type_dimension,
    Minimum child size (default: not set).}
@row3col{gravity, @ref overview_xrcformat_type_float,
    Sash gravity, see wxSplitterWindow::SetSashGravity() (default: not set).}
@endTable

wxSplitterWindow must have one or two children that are non-toplevel window
objects. If there's only one child, it is used as wxSplitterWindow's only
visible child. If there are two children, the first one is used for left/top
child and the second one for right/bottom child window.


@subsubsection xrc_wxsearchctrl wxSearchCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_text,
    Initial value of the control (default: empty).}
@row3col{hint, @ref overview_xrcformat_type_text,
    Descriptive text shown in the empty control (default: "Search").
    @since 3.1.1.}
@endTable


@subsubsection xrc_wxstatusbar wxStatusBar

@beginTable
@hdr3col{property, type, description}
@row3col{fields, integer,
    Number of status bar fields (default: 1).}
@row3col{widths, @ref overview_xrcformat_type_string,
    Comma-separated list of @em fields integers. Each value specifies width
    of one field; the values are interpreted using the same convention used
    by wxStatusBar::SetStatusWidths() (default: not set).}
@row3col{styles, @ref overview_xrcformat_type_string,
    Comma-separated list of @em fields style values. Each value specifies style
    of one field and can be one of @c wxSB_NORMAL, @c wxSB_FLAT, @c wxSB_RAISED or
    @c wxSB_SUNKEN (default: not set).}
@endTable



@subsubsection xrc_wxstaticbitmap wxStaticBitmap

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap to display (required).}
@endTable

@subsubsection xrc_wxstaticbox wxStaticBox

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Static box's label (default: empty).}
@endTable


@subsubsection xrc_wxstaticline wxStaticLine

No additional properties.


@subsubsection xrc_wxstatictext wxStaticText

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label to display (default: empty).}
@row3col{wrap, @ref overview_xrcformat_type_dimension,
     Wrap the text so that each line is at most the given number of pixels, see
     wxStaticText::Wrap() (default: no wrap).}
@endTable

@subsubsection xrc_wxstyledtextctrl wxStyledTextCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{wrapmode, @ref overview_xrcformat_type_style,
    Set wrapmode to wxSTC_WRAP_WORD to enable wrapping on word or style boundaries,
    wxSTC_WRAP_CHAR to enable wrapping between any characters, wxSTC_WRAP_WHITESPACE
    to enable wrapping on whitespace, and wxSTC_WRAP_NONE to disable line wrapping
    (default: wxSTC_WRAP_NONE).}
@endTable

Notice that wxStyledTextCtrl support in XRC is available in wxWidgets 3.1.6 and
later only and you need to explicitly register its handler using
@code
    #include <wx/xrc/xh_styledtextctrl.h>

    AddHandler(new wxStyledTextCtrlXmlHandler);
@endcode
to use it.

@subsubsection xrc_wxtextctrl wxTextCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{value, @ref overview_xrcformat_type_text,
    Initial value of the control (default: empty).}
@row3col{maxlength, integer,
    Maximum length of the text which can be entered by user (default: unlimited).}
@row3col{forceupper, @ref overview_xrcformat_type_bool,
    If true, use wxTextEntry::ForceUpper() to force the control contents to be
    upper case.}
@row3col{hint, @ref overview_xrcformat_type_text,
    Hint shown in empty control. @since 3.0.1}
@endTable


@subsubsection xrc_wxtimepickerctrl wxTimePickerCtrl

No additional properties.


@subsubsection xrc_wxtogglebutton wxToggleButton

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Label to display on the button (default: empty).}
@row3col{checked, @ref overview_xrcformat_type_bool,
     Should the button be checked/pressed initially (default: 0)?}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to display in the button (optional). @since 3.1.1}
@row3col{pressed, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is pressed (default: none, same as @c bitmap). @since 3.1.7}
@row3col{focus, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button has focus (default: none, same as @c bitmap). @since 3.1.7}
@row3col{disabled, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the button is disabled (default: none, same as @c bitmap). @since 3.1.7}
@row3col{current, @ref overview_xrcformat_type_bitmap,
     Bitmap to show when the mouse cursor hovers above the bitmap (default: none, same as @c bitmap). @since 3.1.7}
@row3col{margins, @ref overview_xrcformat_type_size,
    Set the margins between the bitmap and the text of the button.
    This method is currently only implemented under MSW. If it is not called, a default margin is used around the bitmap. @since 3.1.7}
@row3col{bitmapposition, @c wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,
    Position of the bitmap in the button, see wxButton::SetBitmapPosition() (default: wxLEFT). @since 3.1.1}
@endTable

@subsubsection xrc_wxtoolbar wxToolBar

@beginTable
@hdr3col{property, type, description}
@row3col{bitmapsize, @ref overview_xrcformat_type_pair_ints,
    Size of toolbar bitmaps in pixels. Note that these are physical pixels, as
    they typically correspond to the size of available bitmaps, and @e not
    DIPs, i.e. not depending on the current DPI value. In particular, "d" suffix
    is invalid and cannot be used here (default: not set).}
@row3col{margins, @ref overview_xrcformat_type_size,
    Margins (default: platform default).}
@row3col{packing, integer,
    Packing, see wxToolBar::SetToolPacking() (default: not set).}
@row3col{separation, integer,
    Default separator size, see wxToolBar::SetToolSeparation() (default: not set).}
@row3col{dontattachtoframe, @ref overview_xrcformat_type_bool,
    If set to 0 and the toolbar object is child of a wxFrame,
    wxFrame::SetToolBar() is called; otherwise, you have to add it to a frame
    manually. The toolbar is attached by default, you have to set this property
    to 1 to disable this behaviour (default: 0).}
@endTable

A toolbar can have one or more child objects of any wxControl-derived class or
one of three pseudo-classes: @c separator, @c space or @c tool.

The @c separator pseudo-class is used to insert separators into the toolbar and
has neither properties nor children. Similarly, the @c space pseudo-class is
used for stretchable spaces (see wxToolBar::AddStretchableSpace(), new since
wxWidgets 2.9.1).

The @c tool pseudo-class objects specify toolbar buttons and have the following
properties:

@beginTable
@hdr3col{property, type, description}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Tool's bitmap (default: empty).}
@row3col{bitmap2, @ref overview_xrcformat_type_bitmap,
    Bitmap for disabled tool (default: derived from @c bitmap).}
@row3col{label, @ref overview_xrcformat_type_text,
    Label to display on the tool (default: no label).}
@row3col{radio, @ref overview_xrcformat_type_bool,
     Item's kind is wxITEM_RADIO (default: 0)?}
@row3col{toggle, @ref overview_xrcformat_type_bool,
     Item's kind is wxITEM_CHECK (default: 0)?}
@row3col{dropdown, see below,
     Item's kind is wxITEM_DROPDOWN (default: 0)? @since 2.9.0}
@row3col{tooltip, @ref overview_xrcformat_type_text,
    Tooltip to use for the tool (default: none).}
@row3col{longhelp, @ref overview_xrcformat_type_text,
    Help text shown in statusbar when the mouse is on the tool (default: none).}
@row3col{disabled, @ref overview_xrcformat_type_bool,
     Is the tool initially disabled (default: 0)?}
@row3col{checked, @ref overview_xrcformat_type_bool,
     Is the tool initially checked (default: 0)? @since 2.9.3}
@endTable

The presence of a @c dropdown property indicates that the tool is of type
wxITEM_DROPDOWN. It must be either empty or contain exactly one @ref
xrc_wxmenu child object defining the drop-down button associated menu.

Notice that @c radio, @c toggle and @c dropdown are mutually exclusive.

Children that are not @c tool, @c space or @c separator must be instances of classes
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
    <object class="tool" name="view_auto">
        <bitmap>view.png</bitmap>
        <label>View</label>
        <dropdown>
            <object class="wxMenu">
                <object class="wxMenuItem" name="view_as_text">
                    <label>View as text</label>
                </object>
                <object class="wxMenuItem" name="view_as_hex">
                    <label>View as binary</label>
                </object>
            </object>
        </dropdown>
    </object>
    <object class="space"/>
    <object class="wxComboBox">
        <content>
            <item>Just</item>
            <item>a combobox</item>
            <item>in the toolbar</item>
        </content>
    </object>
</object>

@endcode


@subsubsection xrc_wxtoolbook wxToolbook

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none, built implicitly).}
@endTable

A toolbook can have one or more child objects of the @c toolbookpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage).

@c toolbookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{label, @ref overview_xrcformat_type_text,
     Sheet page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label
     (default: none, mutually exclusive with @c image).}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    into the image list
    (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@endTable

Each @c toolbookpage has exactly one non-toplevel window as its child.


@subsubsection xrc_wxtreectrl wxTreeCtrl

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none).}
@endTable


@subsubsection xrc_wxtreebook wxTreebook

@beginTable
@hdr3col{property, type, description}
@row3col{imagelist, @ref overview_xrcformat_type_imagelist,
     Image list to use for the images (default: none, built implicitly).}
@endTable

A treebook can have one or more child objects of the @c treebookpage
pseudo-class (similarly to @ref xrc_wxnotebook "wxNotebook" and its
@c notebookpage).

@c treebookpage objects have the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{depth, integer,
     Page's depth in the labels tree (default: 0; see below).}
@row3col{label, @ref overview_xrcformat_type_text,
     Sheet page's title (default: empty).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
     Bitmap shown alongside the label (default: none, mutually exclusive with @c image).}
@row3col{image, integer,
    The zero-based index of the image associated with the item
    into the image list
    (default: none, mutually exclusive with @c bitmap, only if imagelist was set).}
@row3col{selected, @ref overview_xrcformat_type_bool,
     Is the page selected initially (only one page can be selected; default: 0)?}
@row3col{expanded, @ref overview_xrcformat_type_bool,
    If set to 1, the page is initially expanded. By default all pages are
    initially collapsed.}
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
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Bitmap to display on the left side of the wizard (default: none).}
@row3col{border, integer, Sets width of border around page area. (default: 0).
    @since 3.2.0}
@row3col{bitmap-placement, @ref overview_xrcformat_type_style,
    Sets the flags indicating how the wizard or page bitmap should be expanded
    and positioned to fit the page height. By default, placement is 0
    (no expansion is done). See wxWizard::SetBitmapPlacement()
    @since 3.2.0}
@row3col{bitmap-minwidth, integer,
    Sets the minimum width for the bitmap that will be constructed to contain
    the actual wizard or page bitmap if a non-zero bitmap placement flag has
    been set.
    @since 3.2.0}
@row3col{bitmap-bg, @ref overview_xrcformat_type_colour,
    Sets the colour that should be used to fill the area not taken up by the
    wizard or page bitmap, if a non-zero bitmap placement flag has been set.
    @since 3.2.0}
@endTable

A wizard object can have one or more child objects of the wxWizardPage or
wxWizardPageSimple classes. They both support the following properties
(in addition to @ref overview_xrcformat_std_props):

@beginTable
@hdr3col{property, type, description}
@row3col{title, @ref overview_xrcformat_type_text,
    Wizard window's title (default: none).}
@row3col{bitmap, @ref overview_xrcformat_type_bitmap,
    Page-specific bitmap (default: none).}
@endTable

wxWizardPage and wxWizardPageSimple nodes may have optional children: either
exactly one @ref overview_xrcformat_sizers "sizer" child or any number of
non-toplevel window objects.

wxWizardPageSimple pages are automatically chained together; wxWizardPage pages
transitions must be handled programmatically.


@section overview_xrcformat_sizers Sizers

Sizers are handled slightly differently in XRC resources than they are in
wxWindow hierarchy. wxWindow's sizers hierarchy is parallel to the wxWindow
children hierarchy: child windows are children of their parent window and
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
@c sizeritem or @c spacer (see @ref overview_xrcformat_wxstddialogbuttonsizer for
an exception). The former specifies an element (another sizer or a window)
to include in the sizer, the latter adds empty space to the sizer.

@c sizeritem objects have exactly one child object: either another sizer
object, or a window object. @c spacer objects don't have any children, but
they have one property:

@beginTable
@hdr3col{property, type, description}
@row3col{size, @ref overview_xrcformat_type_size, Size of the empty space (default: @c wxDefaultSize).}
@endTable

Both @c sizeritem and @c spacer objects can have any of the following
properties:

@beginTable
@hdr3col{property, type, description}
@row3col{option, integer,
    The "option" value for sizers. Used by wxBoxSizer to set proportion of
    the item in the growable direction (default: 0).}
@row3col{flag, @ref overview_xrcformat_type_style,
    wxSizerItem flags (default: 0).}
@row3col{border, @ref overview_xrcformat_type_dimension,
    Size of the border around the item (directions are specified in flags)
    (default: 0).}
@row3col{minsize, @ref overview_xrcformat_type_size,
    Minimal size of this item (default: no min size).}
@row3col{ratio, @ref overview_xrcformat_type_pair_ints,
    Item ratio, see wxSizerItem::SetRatio() (default: no ratio).}
@row3col{cellpos, @ref overview_xrcformat_type_pair_ints,
    (wxGridBagSizer only) Position, see wxGBSizerItem::SetPos() (required). }
@row3col{cellspan, @ref overview_xrcformat_type_pair_ints,
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
        <growablecols>0:1</growablecols>
        <growablerows>0:1</growablerows>
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
class-specific properties. All classes except wxStdDialogButtonSizer
support the following properties:

@beginTable
@hdr3col{property, type, description}
@row3col{minsize, @ref overview_xrcformat_type_size,
    Minimal size that this sizer will have, see wxSizer::SetMinSize()
    (default: no min size).}
@row3col{hideitems, @ref overview_xrcformat_type_bool,
    Whether the sizer will be created with all its items hidden
    (default: 0).}
@endTable

@subsection overview_xrcformat_wxboxsizer wxBoxSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref overview_xrcformat_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (default: wxHORIZONTAL).}
@endTable

@subsection overview_xrcformat_wxstaticsboxizer wxStaticBoxSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref overview_xrcformat_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (default: wxHORIZONTAL).}
@row3col{label, @ref overview_xrcformat_type_text,
    Label to be used for the static box around the sizer (default: empty).}
@row3col{windowlabel, any window,
    Window to be used instead of the plain text label (default: none).
    @since 3.1.1}
@endTable

@subsection overview_xrcformat_wxgridsizer wxGridSizer

@beginTable
@hdr3col{property, type, description}
@row3col{rows, unsigned integer, Number of rows in the grid (default: 0 - determine automatically).}
@row3col{cols, unsigned integer, Number of columns in the grid (default: 0 - determine automatically).}
@row3col{vgap, @ref overview_xrcformat_type_dimension, Vertical gap between children (default: 0).}
@row3col{hgap, @ref overview_xrcformat_type_dimension, Horizontal gap between children (default: 0).}
@endTable

@subsection overview_xrcformat_wxflexgridsizer wxFlexGridSizer

@beginTable
@hdr3col{property, type, description}
@row3col{rows, unsigned integer, Number of rows in the grid (default: 0 - determine automatically).}
@row3col{cols, unsigned integer, Number of columns in the grid (default: 0 - determine automatically).}
@row3col{vgap, @ref overview_xrcformat_type_dimension, Vertical gap between children (default: 0).}
@row3col{hgap, @ref overview_xrcformat_type_dimension, Horizontal gap between children (default: 0).}
@row3col{flexibledirection, @ref overview_xrcformat_type_style,
    Flexible direction, @c wxVERTICAL, @c wxHORIZONTAL or @c wxBOTH (default).
    @since 2.9.5}
@row3col{nonflexiblegrowmode, @ref overview_xrcformat_type_style,
    Grow mode in the non-flexible direction,
    @c wxFLEX_GROWMODE_NONE, @c wxFLEX_GROWMODE_SPECIFIED (default) or
    @c wxFLEX_GROWMODE_ALL.
    @since 2.9.5}
@row3col{growablerows, comma-separated integers list,
    Comma-separated list of indexes of rows that are growable (none by default).
    Since wxWidgets 2.9.5 optional proportion can be appended to each number
    after a colon (@c :).}
@row3col{growablecols, comma-separated integers list,
    Comma-separated list of indexes of columns that are growable (none by default).
    Since wxWidgets 2.9.5 optional proportion can be appended to each number
    after a colon (@c :).}
@endTable

@subsection overview_xrcformat_wxgridbagsizer wxGridBagSizer

@beginTable
@hdr3col{property, type, description}
@row3col{vgap, @ref overview_xrcformat_type_dimension, Vertical gap between children (default: 0).}
@row3col{hgap, @ref overview_xrcformat_type_dimension, Horizontal gap between children (default: 0).}
@row3col{flexibledirection, @ref overview_xrcformat_type_style,
    Flexible direction, @c wxVERTICAL, @c wxHORIZONTAL, @c wxBOTH (default: @c wxBOTH).}
@row3col{nonflexiblegrowmode, @ref overview_xrcformat_type_style,
    Grow mode in the non-flexible direction,
    @c wxFLEX_GROWMODE_NONE, @c wxFLEX_GROWMODE_SPECIFIED, @c wxFLEX_GROWMODE_ALL
    (default: @c wxFLEX_GROWMODE_SPECIFIED).}
@row3col{growablerows, comma-separated integers list,
    Comma-separated list of indexes of rows that are growable,
    optionally the proportion can be appended after each number
    separated by a @c :
    (default: none).}
@row3col{growablecols, comma-separated integers list,
    Comma-separated list of indexes of columns that are growable,
    optionally the proportion can be appended after each number
    separated by a @c :
    (default: none).}
@row3col{empty_cellsize, @ref overview_xrcformat_type_size,
    Size used for cells in the grid with no item. (default: @c wxDefaultSize).
    @since 3.2.0}
@endTable

@subsection overview_xrcformat_wxwrapsizer wxWrapSizer

@beginTable
@hdr3col{property, type, description}
@row3col{orient, @ref overview_xrcformat_type_style,
    Sizer orientation, "wxHORIZONTAL" or "wxVERTICAL" (default: wxHORIZONTAL).}
@row3col{flag, @ref overview_xrcformat_type_style, wxWrapSizer flags (default: 0).}
@endTable

@subsection overview_xrcformat_wxstddialogbuttonsizer wxStdDialogButtonSizer

Unlike other sizers, wxStdDialogButtonSizer has neither @c sizeritem
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



@section overview_xrcformat_other_objects Other Objects

In addition to describing UI elements, XRC files can contain non-windows
objects such as bitmaps or icons. This is a concession to Windows developers
used to storing them in Win32 resources.

Note that unlike Win32 resources, bitmaps included in XRC files are @em not
embedded in the XRC file itself. XRC file only contains a reference to another
file with bitmap data.

@subsection overview_xrcformat_bitmap wxBitmap

Bitmaps are stored in @c \<object\> element with class set to @c wxBitmap. Such
bitmaps can then be loaded using wxXmlResource::LoadBitmap(). The content of
the element is exactly same as in the case of
@ref overview_xrcformat_type_bitmap "bitmap properties", except that toplevel
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


@subsection overview_xrcformat_icon wxIcon

wxIcon resources are identical to @ref overview_xrcformat_bitmap "wxBitmap ones",
except that the class is @c wxIcon.


@section overview_xrcformat_platform Platform Specific Content

It is possible to conditionally process parts of XRC files on some platforms
only and ignore them on other platforms. @em Any element in XRC file, be it
toplevel or arbitrarily nested one, can have the @c platform attribute. When
used, @c platform contains |-separated list of platforms that this element
should be processed on. It is filtered out and ignored on any other platforms.

Possible elemental values are:
@beginDefList
@itemdef{ @c msw, Windows, preferred platform name }
@itemdef{ @c win, Windows, alternative synonym }
@itemdef{ @c mac, macOS or iOS }
@itemdef{ @c unix, Any Unix platform @em except macOS }
@endDefList

Examples:
@code
<label platform="msw">Windows</label>
<label platform="unix">Unix</label>
<label platform="mac">macOS</label>
<help platform="mac|unix">Not a Windows machine</help>
@endcode


@section overview_xrcformat_features Feature-specific Content

Similarly to the platform-specific features, but even more flexibly, XRC
elements can use @c feature attribute. Just as above, the value of this
attribute is a |-separated list of strings but, unlike platforms, these strings
don't have any intrinsic meaning and the program must call
wxXmlResource::EnableFeature() to indicate which features should be considered
to be enabled.

Any element using the @c feature attribute not including any enabled features
is ignored when loading the XRC document. Because by default all features are
disabled, this means that all elements with this attribute are discarded if
wxXmlResource::EnableFeature() is not called at all.

For example, an educational program might exist in several builds targeting
different subjects and enable or disable different features in them:
@code
<object class="wxNotebook">
    <object class="notebookpage"> <!-- No feature, so always present -->
        <label>Overview</label>
        ...
    </object>
    <object class="notebookpage" feature="humanities">
        <label>History</label>
        ...
    </object>
    <object class="notebookpage" feature="science">
        <label>Physics</label>
        ...
    </object>
    <object class="notebookpage" feature="science">
        <label>Chemistry</label>
        ...
    </object>
    <object class="notebookpage" feature="science|humanities">
        <label>Economics</label>
        ...
    </object>
</object>
@endcode

By calling
@code
    wxXmlResource::Get()->EnableFeature("humanities");
@endcode
before loading the XRC document, the program would request loading the first
two and the last pages only.


@section overview_xrcformat_idranges ID Ranges

Usually you won't care what value the XRCID macro returns for the ID of an
object. Sometimes though it is convenient to have a range of IDs that are
guaranteed to be consecutive. An example of this would be connecting a group of
similar controls to the same event handler.

The following XRC fragment 'declares' an ID range called  @em foo and another
called @em bar; each with some items.

@code
    <object class="wxButton" name="foo[start]">
    <object class="wxButton" name="foo[end]">
    <object class="wxButton" name="foo[2]">
    ...
    <object class="wxButton" name="bar[0]">
    <object class="wxButton" name="bar[2]">
    <object class="wxButton" name="bar[1]">
    ...
<ids-range name="foo" />
<ids-range name="bar" size="30" start="10000" />
@endcode

For the range foo, no @em size or @em start parameters were given, so the size
will be calculated from the number of range items, and IDs allocated by
wxWindow::NewControlId (so they'll be negative). Range bar asked for a size of
30, so this will be its minimum size: should it have more items, the range will
automatically expand to fit them. It specified a start ID of 10000, so
XRCID("bar[0]") will be 10000, XRCID("bar[1]") 10001 etc. Note that if you
choose to supply a start value it must be positive, and it's your
responsibility to avoid clashes.

For every ID range, the first item can be referenced either as
<em>rangename</em>[0] or <em>rangename</em>[start]. Similarly
<em>rangename</em>[end] is the last item. Using [start] and [end] is more
descriptive in e.g. a Bind() event range or a @em for loop, and they don't have
to be altered whenever the number of items changes.

Whether a range has positive or negative IDs, [start] is always a smaller
number than [end]; so code like this works as expected:

@code
for (int n=XRCID("foo[start]"); n <= XRCID("foo[end]"); ++n)
    ...
@endcode

ID ranges can be seen in action in the <em>objref</em> dialog section of the
@sample{xrc}.

@note
@li All the items in an ID range must be contained in the same XRC file.
@li You can't use an ID range in a situation where static initialisation
occurs; in particular, they won't work as expected in an event table. This is
because the event table's IDs are set to their integer values before the XRC
file is loaded, and aren't subsequently altered when the XRCID value changes.

@since 2.9.2

@section overview_xrcformat_extending Extending the XRC Format

The XRC format is designed to be extensible and allows specifying and loading
custom controls. The three available mechanisms are described in the rest of
this section in the order of increasing complexity.

@subsection overview_xrcformat_extending_subclass Subclassing

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
    a wxDECLARE_DYNAMIC_CLASS() entry for it.
 -# It must support two-phase creation. In particular, this means that it has
    to have default constructor.
 -# It cannot provide custom Create() method and must be constructible using
    base @c class' Create() method (this is because XRC will call Create() of
    @c class, not @c subclass). In other words, @em creation of the control
    must not be customized.


@subsection overview_xrcformat_extending_unknown Unknown Objects

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
the @ref overview_xrcformat_std_props "standard window properties".

@note @c unknown class cannot be combined with @c subclass attribute,
      they are mutually exclusive.


@subsection overview_xrcformat_extending_custom Adding Custom Classes

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
are accessible using the type-unsafe wxXmlResource::LoadObject() method.



@section overview_xrcformat_packed Packed XRC Files

In addition to plain XRC files, wxXmlResource supports (if wxFileSystem support
is compiled in) compressed XRC resources. Compressed resources have either
.zip or .xrs extension and are simply ZIP files that contain arbitrary
number of XRC files and their dependencies (bitmaps, icons etc.).



@section overview_xrcformat_oldversions Older Format Versions

This section describes differences in older revisions of XRC format (i.e.
files with older values of @c version attribute of @c \<resource\>).


@subsection overview_xrcformat_pre_v2530 Versions Before 2.5.3.0

Version 2.5.3.0 introduced C-like handling of \"\\\\\" in text. In older versions,
"\n", "\t" and "\r" escape sequences were replaced with respective characters
in the same matter as it's done in C, but \"\\\\\" was left intact instead of being
replaced with a single \"\\\", as one would expect. Starting with 2.5.3.0, all of
them are handled in C-like manner.


@subsection overview_xrcformat_pre_v2301 Versions Before 2.3.0.1

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
