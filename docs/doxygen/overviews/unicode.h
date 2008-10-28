/////////////////////////////////////////////////////////////////////////////
// Name:        unicode.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_unicode Unicode Support in wxWidgets

This section describes how does wxWidgets support Unicode and how can it affect
your programs.

Notice that Unicode support has changed radically in wxWidgets 3.0 and a lot of
existing material pertaining to the previous versions of the library is not
correct any more. Please see @ref overview_changes_unicode for the details of
these changes.

You can skip the first two sections if you're already familiar with Unicode and
wish to jump directly in the details of its support in the library:
@li @ref overview_unicode_what
@li @ref overview_unicode_encodings
@li @ref overview_unicode_supportin
@li @ref overview_unicode_pitfalls
@li @ref overview_unicode_supportout
@li @ref overview_unicode_settings

<hr>


@section overview_unicode_what What is Unicode?

Unicode is a standard for character encoding which addresses the shortcomings
of the previous, 8 bit standards, by using at least 16 (and possibly 32) bits
for encoding each character. This allows to have at least 65536 characters
(in what is called the BMP, or basic multilingual plane) and possible 2^32 of
them instead of the usual 256 and is sufficient to encode all of the world
languages at once. More details about Unicode may be found at
http://www.unicode.org/.

From a practical point of view, using Unicode is almost a requirement when
writing applications for international audience. Moreover, any application
reading files which it didn't produce or receiving data from the network from
other services should be ready to deal with Unicode.


@section overview_unicode_encodings Unicode Representations

Unicode provides a unique code to identify every character, however in practice
these codes are not always used directly but encoded using one of the standard
UTF or Unicode Transformation Formats which are algorithms mapping the Unicode
codes to byte code sequences. The simplest of them is UTF-32 which simply maps
the Unicode code to a 4 byte sequence representing this 32 bit number (although
this is still not completely trivial as the mapping is different for little and
big-endian architectures). UTF-32 is commonly used under Unix systems for
internal representation of Unicode strings. Another very widespread standard is
UTF-16 which is used by Microsoft Windows: it encodes the first (approximately)
64 thousands of Unicode characters using only 2 bytes and uses a pair of 16-bit
codes to encode the characters beyond this. Finally, the most widespread
encoding used for the external Unicode storage (e.g. files and network
protocols) is UTF-8 which is byte-oriented and so avoids the endianness
ambiguities of UTF-16 and UTF-32. However UTF-8 uses a variable number of bytes
for representing Unicode characters which makes it less efficient than UTF-32
for internal representation.

From the C/C++ programmer perspective the situation is further complicated by
the fact that the standard type @c wchar_t which is used to represent the
Unicode ("wide") strings in C/C++ doesn't have the same size on all platforms.
It is 4 bytes under Unix systems, corresponding to the tradition of using
UTF-32, but only 2 bytes under Windows which is required by compatibility with
the OS which uses UTF-16.


@section overview_unicode_supportin Unicode Support in wxWidgets

Since wxWidgets 3.0 Unicode support is always enabled and building the library
without it is not recommended any longer and will cease to be supported in the
near future. This means that internally only Unicode strings are used and that,
under Microsoft Windows, Unicode system API is used which means that wxWidgets
programs require the Microsoft Layer for Unicode to run on Windows 95/98/ME.

However, unlike Unicode build mode in the previous versions of wxWidgets, this
support is mostly transparent: you can still continue to work with the narrow
(i.e. @c char*) strings even if wide (i.e. @c wchar_t*) strings are also
supported. Any wxWidgets function accepts arguments of either type as both
kinds of strings are implicitly converted to wxString, so both
@code
wxMessageBox("Hello, world!");
@endcode
and somewhat less usual
@code
wxMessageBox(L"Salut \u00e0 toi!"); // 00E0 is "Latin Small Letter a with Grave"
@endcode
work as expected.

Notice that the narrow strings used with wxWidgets are @e always assumed to be
in the current locale encoding, so writing
@code
wxMessageBox("Salut à toi!");
@endcode
wouldn't work if the encoding used on the user system is incompatible with
ISO-8859-1 (or even if the sources were compiled under different locale
in the case of gcc). In particular, the most common encoding used under
modern Unix systems is UTF-8 and as the string above is not a valid UTF-8 byte
sequence, nothing would be displayed at all in this case. Thus it is important
to never use 8 bit characters directly in the program source but use wide
strings or, alternatively, write
@code
wxMessageBox(wxString::FromUTF8("Salut \xc3\xa0 toi!"));
@endcode

In a similar way, wxString provides access to its contents as either wchar_t or
char character buffer. Of course, the latter only works if the string contains
data representable in the current locale encoding. This will always be the case
if the string had been initially constructed from a narrow string or if it
contains only 7-bit ASCII data but otherwise this conversion is not guaranteed
to succeed. And as with wxString::FromUTF8() example above, you can always use
wxString::ToUTF8() to retrieve the string contents in UTF-8 encoding -- this,
unlike converting to @c char* using the current locale, never fails.

To summarize, Unicode support in wxWidgets is mostly transparent for the
application and if you use wxString objects for storing all the character data
in your program there is really nothing special to do. However you should be
aware of the potential problems covered by the following section.


@section overview_unicode_pitfalls Potential Unicode Pitfalls

The problems can be separated into three broad classes:

@subsection overview_unicode_compilation_errors Unicode-Related Compilation Errors

Because of the need to support implicit conversions to both @c char and @c
wchar_t, wxString implementation is rather involved and many of its operators
don't return the types which they could be naively expected to return. For
example, the @c operator[] doesn't return neither a @c char nor a @c wchar_t
but an object of a helper class wxUniChar or wxUniCharRef which is implicitly
convertible to either. Usually you don't need to worry about this as the
conversions do their work behind the scenes however in some cases it doesn't
work. Here are some examples, using a wxString object @c s and some integer @c
n:

 - Writing @code switch ( s[n] ) @endcode doesn't work because the argument of
   the switch statement must an integer expression so you need to replace
   @c s[n] with @code s[n].GetValue() @endcode. You may also force the
   conversion to char or wchar_t by using an explicit cast but beware that
   converting the value to char uses the conversion to current locale and may
   return 0 if it fails. Finally notice that writing @code (wxChar)s[n] @endcode
   works both with wxWidgets 3.0 and previous library versions and so should be
   used for writing code which should be compatible with both 2.8 and 3.0.

 - Similarly, @code &s[n] @endcode doesn't yield a pointer to char so you may
   not pass it to functions expecting @c char* or @c wchar_t*. Consider using
   string iterators instead if possible or replace this expression with
   @code s.c_str() + n @endcode otherwise.

Another class of problems is related to the fact that the value returned by
@c c_str() itself is also not just a pointer to a buffer but a value of helper
class wxCStrData which is implicitly convertible to both narrow and wide
strings. Again, this mostly will be unnoticeable but can result in some
problems:

 - You shouldn't pass @c c_str() result to vararg functions such as standard
   @c printf(). Some compilers (notably g++) warn about this but even if they
   don't, this @code printf("Hello, %s", s.c_str()) @endcode is not going to
   work. It can be corrected in one of the following ways:

  - Preferred: @code wxPrintf("Hello, %s", s) @endcode (notice the absence
    of @c c_str(), it is not needed at all with wxWidgets functions)
  - Compatible with wxWidgets 2.8: @code wxPrintf("Hello, %s", s.c_str()) @endcode
  - Using an explicit conversion to narrow, multibyte, string:
    @code printf("Hello, %s", (const char *)s.mb_str()) @endcode
  - Using a cast to force the issue (listed only for completeness):
    @code printf("Hello, %s", (const char *)s.c_str()) @endcode

 - The result of @c c_str() can not be cast to @c char* but only to @c const @c
   @c char*. Of course, modifying the string via the pointer returned by this
   method has never been possible but unfortunately it was occasionally useful
   to use a @c const_cast here to pass the value to const-incorrect functions.
   This can be done either using new wxString::char_str() (and matching
   wchar_str()) method or by writing a double cast:
   @code (char *)(const char *)s.c_str() @endcode

 - One of the unfortunate consequences of the possibility to pass wxString to
   @c wxPrintf() without using @c c_str() is that it is now impossible to pass
   the elements of unnamed enumerations to @c wxPrintf() and other similar
   vararg functions, i.e.
   @code
        enum { Red, Green, Blue };
        wxPrintf("Red is %d", Red);
   @endcode
   doesn't compile. The easiest workaround is to give a name to the enum.

Other unexpected compilation errors may arise but they should happen even more
rarely than the above-mentioned ones and the solution should usually be quite
simple: just use the explicit methods of wxUniChar and wxCStrData classes
instead of relying on their implicit conversions if the compiler can't choose
among them.


@subsection overview_unicode_data_loss Data Loss due To Unicode Conversion Errors

wxString API provides implicit conversion of the internal Unicode string
contents to narrow, char strings. This can be very convenient and is absolutely
necessary for backwards compatibility with the existing code using wxWidgets
however it is a rather dangerous operation as it can easily give unexpected
results if the string contents isn't convertible to the current locale.

To be precise, the conversion will always succeed if the string was created
from a narrow string initially. It will also succeed if the current encoding is
UTF-8 as all Unicode strings are representable in this encoding. However
initializing the string using wxString::FromUTF8() method and then accessing it
as a char string via its wxString::c_str() method is a recipe for disaster as the
program may work perfectly well during testing on Unix systems using UTF-8 locale
but completely fail under Windows where UTF-8 locales are never used because
wxString::c_str() would return an empty string.

The simplest way to ensure that this doesn't happen is to avoid conversions to
@c char* completely by using wxString throughout your program. However if the
program never manipulates 8 bit strings internally, using @c char* pointers is
safe as well. So the existing code needs to be reviewed when upgrading to
wxWidgets 3.0 and the new code should be used with this in mind and ideally
avoiding implicit conversions to @c char*.


@subsection overview_unicode_performance Unicode Performance Implications

Under Unix systems wxString class uses variable-width UTF-8 encoding for
internal representation and this implies that it can't guarantee constant-time
access to N-th element of the string any longer as to find the position of this
character in the string we have to examine all the preceding ones. Usually this
doesn't matter much because most algorithms used on the strings examine them
sequentially anyhow and because wxString implements a cache for iterating over
the string by index but it can have serious consequences for algorithms
using random access to string elements as they typically acquire O(N^2) time
complexity instead of O(N) where N is the length of the string.

Even despite caching the index, indexed access should be replaced with
sequential access using string iterators. For example a typical loop:
@code
wxString s("hello");
for ( size_t i = 0; i < s.length(); i++ )
{
    wchar_t ch = s[i];

    // do something with it
}
@endcode
should be rewritten as
@code
wxString s("hello");
for ( wxString::const_iterator i = s.begin(); i != s.end(); ++i )
{
    wchar_t ch = *i

    // do something with it
}
@endcode

Another, similar, alternative is to use pointer arithmetic:
@code
wxString s("hello");
for ( const wchar_t *p = s.wc_str(); *p; p++ )
{
    wchar_t ch = *i

    // do something with it
}
@endcode
however this doesn't work correctly for strings with embedded @c NUL characters
and the use of iterators is generally preferred as they provide some run-time
checks (at least in debug build) unlike the raw pointers. But if you do use
them, it is better to use wchar_t pointers rather than char ones to avoid the
data loss problems due to conversion as discussed in the previous section.


@section overview_unicode_supportout Unicode and the Outside World

Even though wxWidgets always uses Unicode internally, not all the other
libraries and programs do and even those that do use Unicode may use a
different encoding of it. So you need to be able to convert the data to various
representations and the wxString methods wxString::ToAscii(), wxString::ToUTF8()
(or its synonym wxString::utf8_str()), wxString::mb_str(), wxString::c_str() and
wxString::wc_str() can be used for this.
The first of them should be only used for the string containing 7-bit ASCII characters
only, anything else will be replaced by some substitution character.
wxString::mb_str() converts the string to the encoding used by the current locale
and so can return an empty string if the string contains characters not representable in
it as explained in @ref overview_unicode_data_loss. The same applies to wxString::c_str()
if its result is used as a narrow string. Finally, wxString::ToUTF8() and wxString::wc_str()
functions never fail and always return a pointer to char string containing the
UTF-8 representation of the string or wchar_t string.

wxString also provides two convenience functions: wxString::From8BitData() and
wxString::To8BitData(). They can be used to create a wxString from arbitrary binary
data without supposing that it is in current locale encoding, and then get it back,
again, without any conversion or, rather, undoing the conversion used by
wxString::From8BitData(). Because of this you should only use wxString::From8BitData()
for the strings created using wxString::To8BitData(). Also notice that in spite
of the availability of these functions, wxString is not the ideal class for storing
arbitrary binary data as they can take up to 4 times more space than needed
(when using @c wchar_t internal representation on the systems where size of
wide characters is 4 bytes) and you should consider using wxMemoryBuffer
instead.

Final word of caution: most of these functions may return either directly the
pointer to internal string buffer or a temporary wxCharBuffer or wxWCharBuffer
object. Such objects are implicitly convertible to char and wchar_t pointers,
respectively, and so the result of, for example, wxString::ToUTF8() can always be
passed directly to a function taking @c const @c char*. However code such as
@code
const char *p = s.ToUTF8();
...
puts(p); // or call any other function taking const char *
@endcode
does @b not work because the temporary buffer returned by wxString::ToUTF8() is
destroyed and @c p is left pointing nowhere. To correct this you may use
@code
wxCharBuffer p(s.ToUTF8());
puts(p);
@endcode
which does work but results in an unnecessary copy of string data in the build
configurations when wxString::ToUTF8() returns the pointer to internal string buffer.
If this inefficiency is important you may write
@code
const wxUTF8Buf p(s.ToUTF8());
puts(p);
@endcode
where @c wxUTF8Buf is the type corresponding to the real return type of wxString::ToUTF8().
Similarly, wxWX2WCbuf can be used for the return type of wxString::wc_str().
But, once again, none of these cryptic types is really needed if you just pass
the return value of any of the functions mentioned in this section to another
function directly.

@section overview_unicode_settings Unicode Related Compilation Settings

@c wxUSE_UNICODE is now defined as 1 by default to indicate Unicode support.
If UTF-8 is used for the internal storage in wxString, @c wxUSE_UNICODE_UTF8 is
also defined, otherwise @c wxUSE_UNICODE_WCHAR is.

*/

