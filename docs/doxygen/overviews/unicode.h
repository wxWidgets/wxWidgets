/////////////////////////////////////////////////////////////////////////////
// Name:        unicode.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_unicode Unicode Support in wxWidgets

This section briefly describes the state of the Unicode support in wxWidgets.
Read it if you want to know more about how to write programs able to work with
characters from languages other than English.

@li @ref overview_unicode_what
@li @ref overview_unicode_ansi
@li @ref overview_unicode_supportin
@li @ref overview_unicode_supportout
@li @ref overview_unicode_settings

<hr>


@section overview_unicode_what What is Unicode?

wxWidgets has support for compiling in Unicode mode on the platforms which
support it. Unicode is a standard for character encoding which addresses the
shortcomings of the previous, 8 bit standards, by using at least 16 (and
possibly 32) bits for encoding each character. This allows to have at least
65536 characters (what is called the BMP, or basic multilingual plane) and
possible 2^32 of them instead of the usual 256 and is sufficient to encode all
of the world languages at once. A different approach is to encode all 
strings in UTF8 which does not require the use of wide characters and
additionally is backwards compatible with 7-bit ASCII. The solution to
use UTF8 is prefered under Linux and partially OS X.

More details about Unicode may be found at <http://www.unicode.org/>.

Writing internationalized programs is much easier with Unicode. Moreover
even a program which uses only standard ASCII can benefit from using Unicode
for string representation because there will be no need to convert all
strings the program uses to/from Unicode each time a system call is made.

@section overview_unicode_ansi Unicode and ANSI Modes

Until wxWidgets 3.0 it was possible to compile the library both in
ANSI (=8-bit) mode as well as in wide char mode (16-bit per character
on Windows and 32-but on most Unix versions, Linux and OS X). This
has been changed in wxWidget with the removal of the ANSI mode,
but much effort has been made so that most of the previous ANSI
code should still compile and work as before.

@section overview_unicode_supportin Unicode Support in wxWidgets

Since wxWidgets 3.0 Unicode support is always enabled meaning
that the wxString class always uses Unicode to encode its content.
Under Windows wxString uses UCS-2 (basically an array of 16-bit
wchar_t). Under Unix, Linux  and OS X however, wxString uses UTF8
to encode its content.

For the programmer, the biggest change is that iterating over
a string can be slower than before since wxString has to parse
the entire string in order to find the n-th character in a 
string, meaning that iterating over a string should no longer
be done by index but using iterators. Old code will still work
but might be less efficient.

Old code like this:

@code
wxString s = wxT("hello");
size_t i;
for (i = 0; i < s.Len(); i++)
{
    wxChar ch = s[i];
    
    // do something with it
}
@endcode

should be replaced (especially in time critical places) with:

@code
wxString s = "hello";
wxString::iterator i;
for (i = s.begin(); i != s.end(); ++i)
{
    wxUniChar uni_ch = *i;
    wxChar ch = uni_ch;
    // same as:   wxChar ch = *i
    
    // do something with it
}
@endcode

If you want to replace individual characters in the string you
need to get a reference to that character:

@code
wxString s = "hello";
wxString::iterator i;
for (i = s.begin(); i != s.end(); ++i)
{
    wxUniCharRef ch = *i;
    ch = 'a';
    // same as:  *i = 'a';
}
@endcode

which will change the content of the wxString s from "hello" to "aaaaa".

String literals are translated to Unicode when they are assigned to
a wxString object so code can be written like this:

@code
wxString s = "Hello, world!";
int len = s.Len();
@endcode

wxWidgets provides wrappers around most Posix C functions (like printf(..))
and the syntax has been adapted to support input with wxString, normal
C-style strings and wchar_t strings:

@code
wxString s;
s.Printf( "%s %s %s", "hello1", L"hello2", wxString("hello3") );
wxPrintf( "Three times hello %s\n", s );
@endcode

@section overview_unicode_supportout Unicode and the Outside World

We have seen that it was easy to write Unicode programs using wxWidgets types
and macros, but it has been also mentioned that it isn't quite enough. Although
everything works fine inside the program, things can get nasty when it tries to
communicate with the outside world which, sadly, often expects ANSI strings (a
notable exception is the entire Win32 API which accepts either Unicode or ANSI
strings and which thus makes it unnecessary to ever perform any conversions in
the program). GTK 2.0 only accepts UTF-8 strings.

To get an ANSI string from a wxString, you may use the mb_str() function which
always returns an ANSI string (independently of the mode - while the usual
c_str() returns a pointer to the internal representation which is either ASCII
or Unicode). More rarely used, but still useful, is wc_str() function which
always returns the Unicode string.

Sometimes it is also necessary to go from ANSI strings to wxStrings. In this
case, you can use the converter-constructor, as follows:

@code
const char* ascii_str = "Some text";
wxString str(ascii_str, wxConvUTF8);
@endcode

For more information about converters and Unicode see the @ref overview_mbconv.


@section overview_unicode_settings Unicode Related Compilation Settings

You should define @c wxUSE_UNICODE to 1 to compile your program in Unicode
mode. Since wxWidgets 3.0 this is always the case. When compiled in UTF8
mode @c wxUSE_UNICODE_UTF8 is also defined.

*/

