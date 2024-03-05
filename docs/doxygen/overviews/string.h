/////////////////////////////////////////////////////////////////////////////
// Name:        string.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_string wxString Overview

@tableofcontents

wxString is used for all strings in wxWidgets. This class is very similar to
the standard string class, and is implemented using it, but provides additional
compatibility functions to allow applications originally written for the much
older versions of wxWidgets to continue to work with the latest ones.

When writing new code, you're encouraged to use wxString as if it were
`std::wstring` and use only functions compatible with the standard class.


@section overview_string_settings wxString Related Compilation Settings

The main build options affecting wxString are `wxUSE_UNICODE_WCHAR` and
`wxUSE_UNICODE_UTF8`, exactly one of which must be set to determine whether
fixed-width `wchar_t` or variable-width `char`-based strings are used
internally. Please see @ref overview_unicode_support_utf for more information
about this choice.

The other options all affect the presence, or absence, of various implicit
conversions provided by this class. By default, wxString can be implicitly
created from `char*`, `wchar_t*`, `std::string` and `std::wstring` and can be
implicitly converted to `char*` or `wchar_t*`. This behaviour is convenient
and compatible with the previous wxWidgets versions, but is dangerous and may
result in unwanted conversions, please see @ref string_conv for how to disable
them.


@section overview_string_iterating Iterating over wxString

It is possible to iterate over strings using indices, but the recommended way
to do it is to use use b iterators, either explicitly:

@code
wxString s = "hello";
wxString::const_iterator i;
for (i = s.begin(); i != s.end(); ++i)
{
    wxUniChar uni_ch = *i;
    // do something with it
}
@endcode

or, even simpler, implicitly, using range for loop:
@code
wxString s = "hello";
for ( auto c : s )
{
    // do something with "c"
}
@endcode

@note wxString iterators have unusual proxy-like semantics and can be used to
    modify the string even when @e not using references, i.e. with just @c
    auto, as in the example above.


@section overview_string_internal wxString Internal Representation

@note This section can be skipped at first reading and is provided solely for
informational purposes.

As mentioned above, wxString may use any of @c UTF-16 (under Windows, using
the native 16 bit @c wchar_t), @c UTF-32 (under Unix, using the native 32
bit @c wchar_t) or @c UTF-8 (under both Windows and Unix) to store its
content. By default, @c wchar_t is used under all platforms, but wxWidgets can
be compiled with <tt>wxUSE_UNICODE_UTF8=1</tt> to use UTF-8 instead.

For simplicity of implementation, wxString uses <em>per code unit indexing</em>
instead of <em>per code point indexing</em> when using UTF-16, i.e. in the
default <tt>wxUSE_UNICODE_WCHAR==1</tt> build under Windows and doesn't know
anything about surrogate pairs. In other words it always considers code points
to be composed by 1 code unit, while this is really true only for characters in
the @e BMP (Basic Multilingual Plane), as explained in more details in the @ref
overview_unicode_encodings section. Thus when iterating over a UTF-16 string
stored in a wxString under Windows, the user code has to take care of
<em>surrogate pairs</em> manually if it needs to handle them (note however that
Windows itself has built-in support for surrogate pairs in UTF-16, such as for
drawing strings on screen, so nothing special needs to be done when just
passing strings containing surrogates to wxWidgets functions).

@remarks
Note that while the behaviour of wxString when <tt>wxUSE_UNICODE_WCHAR==1</tt>
resembles UCS-2 encoding, it's not completely correct to refer to wxString as
UCS-2 encoded since you can encode code points outside the @e BMP in a wxString
as two code units (i.e. as a surrogate pair; as already mentioned however wxString
will "see" them as two different code points)

In <tt>wxUSE_UNICODE_UTF8==1</tt> case, wxString handles UTF-8 multi-bytes
sequences just fine also for characters outside the BMP (it implements <em>per
code point indexing</em>), so that you can use UTF-8 in a completely transparent
way:

Example:
@code
    // first test, using exotic characters outside of the Unicode BMP:

    wxString test = wxString::FromUTF8("\xF0\x90\x8C\x80");
        // U+10300 is "OLD ITALIC LETTER A" and is part of Unicode Plane 1
        // in UTF8 it's encoded as 0xF0 0x90 0x8C 0x80

    // it's a single Unicode code-point encoded as:
    // - a UTF16 surrogate pair under Windows
    // - a UTF8 multiple-bytes sequence under Linux
    // (without considering the final NUL)

    wxPrintf("wxString reports a length of %d character(s)", test.length());
        // prints "wxString reports a length of 1 character(s)" on Linux
        // prints "wxString reports a length of 2 character(s)" on Windows
        // since wxString on Windows doesn't have surrogate pairs support!


    // second test, this time using characters part of the Unicode BMP:

    wxString test2 = wxString::FromUTF8("\x41\xC3\xA0\xE2\x82\xAC");
        // this is the UTF8 encoding of capital letter A followed by
        // 'small case letter a with grave' followed by the 'euro sign'

    // they are 3 Unicode code-points encoded as:
    // - 3 UTF16 code units under Windows
    // - 6 UTF8 code units under Linux
    // (without considering the final NUL)

    wxPrintf("wxString reports a length of %d character(s)", test2.length());
        // prints "wxString reports a length of 3 character(s)" on Linux
        // prints "wxString reports a length of 3 character(s)" on Windows
@endcode

To better explain what stated above, consider the second string of the example
above; it's composed by 3 characters and the final @NUL:

@image html overview_wxstring_encoding.png

As you can see, UTF16 encoding is straightforward (for characters in the @e BMP)
and in this example the UTF16-encoded wxString takes 8 bytes.
UTF8 encoding is more elaborated and in this example takes 7 bytes.

In general, for strings containing many latin characters UTF8 provides a big
advantage with regards to the memory footprint respect UTF16, but requires some
more processing for common operations like e.g. length calculation.

Finally, note that the type used by wxString to store Unicode code units
(@c wchar_t or @c char) is always @c typedef-ined to be ::wxStringCharType.

*/
