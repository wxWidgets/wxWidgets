/////////////////////////////////////////////////////////////////////////////
// Name:        mbconvclasses.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_mbconv wxMBConv Overview

@tableofcontents

The wxMBConv classes in wxWidgets enable the application to easily convert
between Unicode and the variety of 8-bit encoding systems if you still need to
use them.

@see @ref group_class_conv



@section overview_mbconv_need Background: The Need for Conversion

wxWidgets always uses Unicode internally, for many good reasons already
abundantly described everywhere. By default is uses UTF-16 under Microsoft
Windows and UTF-32 under all the other systems, but a special build option can
be enabled to use UTF-8 for the internal representation.

Unless this option is enabled, the most common conversion operation is
converting between this internal representation and UTF-8. This can be done
using wxString::utf8_str() and wxString::FromUTF8() functions and, in many
cases, they are the only conversion-related functions that you will need and
you can stop reading this document now.

But if you need to support conversions to other, legacy, encodings, such as
Shift-JIS or KOI8-R, you will need to use the conversion classes directly, in
which case please read on.


@section overview_mbconv_classes wxMBConv Classes

The base class for all these conversions is the wxMBConv class (which itself
implements standard libc locale conversion). Derived classes include
wxMBConvLibc, several different wxMBConvUTFxxx classes, and wxCSConv, which
implement different kinds of conversions. You can also derive your own class
for your own custom encoding and use it, should you need it. All you need to do
is override the `ToWChar()` and `FromWChar()` methods. Note that these methods
are not used when using these classes, however: even if this would be possible,
it is more convenient to use the helper `cMB2WC()` and `cWC2MB()` ones wrapping
them (they read as "convert multi-byte to wide-character" and vice versa,
respectively).


@section overview_mbconv_objects wxMBConv Objects

Several of the wxWidgets-provided wxMBConv classes have predefined instances
(wxConvLibc, wxConvFileName, wxConvUTF7, wxConvUTF8, wxConvLocal). You can use
these predefined objects directly, or you can instantiate your own objects.

A variable, wxConvCurrent, points to the conversion object that is used if the
conversion is not specified. By default, it points to wxConvLibc or
wxConvLocal, depending on which works best on the current platform.


@section overview_mbconv_csconv wxCSConv

The wxCSConv class is special because when it is instantiated, you can tell it
which character set it should use, which makes it meaningful to keep many
instances of them around, each with a different character set (or you can
create a wxCSConv instance on the fly).

The predefined wxCSConv instance, wxConvLocal, is preset to use the default
user character set, but you should rarely need to use it directly, it is better
to go through wxConvCurrent.


@section overview_mbconv_converting Converting Strings

Once you have chosen which object you want to use to convert your text, here is
how you would use them with wxString. These examples all assume that you are
using a Unicode build of wxWidgets, although they will still compile in a
non-Unicode build (they just won't convert anything).

Example 1: Constructing a wxString from input in current encoding.

@code
wxString str(input_data, *wxConvCurrent);
@endcode

Example 2: Input in UTF-8 encoding.

@code
wxString str(input_data, wxConvUTF8);
@endcode

Note that the same task can be accomplished more clearly by using
wxString::FromUTF8().

Example 3: Input in KOI8-R. Construction of wxCSConv instance on the fly.

@code
wxString str(input_data, wxCSConv(wxT("koi8-r")));
@endcode

Example 4: Printing a wxString to stdout in UTF-8 encoding.

@code
puts(str.mb_str(wxConvUTF8));
@endcode

Note that the same can be done more easily by using
@code
puts(str.utf8_str());
@endcode

Example 5: Printing a wxString to stdout in custom encoding. Using
preconstructed wxCSConv instance.

@code
wxCSConv cust(user_encoding);
printf("Data: %s\n", (const char*) str.mb_str(cust));
@endcode

@note Since mb_str() returns a temporary wxCharBuffer to hold the result of the
conversion, you need to explicitly cast it to const char* if you use it in a
vararg context (like with printf).


@section overview_mbconv_buffers Converting Buffers

If you have specialized needs, or just don't want to use wxString, you can also
use the conversion methods of the conversion objects directly, e.g. you could
print a wide character buffer to a non-Unicode stdout:

@code
printf("Data: %s\n", (const char*) wxConvCurrent->cWC2MB(unicode_data));
@endcode

If you need to do more complex processing on the converted data, you may want
to store the temporary buffer in a local variable:

@code
const wxCharBuffer tmp_buf = wxConvCurrent->cWC2MB(unicode_data);
const char *tmp_str = (const char*) tmp_buf;
printf("Data: %s\n", tmp_str);
process_data(tmp_str);
@endcode

*/
