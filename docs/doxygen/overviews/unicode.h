/////////////////////////////////////////////////////////////////////////////
// Name:        unicode
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page overview_unicode Unicode support in wxWidgets

 This section briefly describes the state of the Unicode support in wxWidgets.
 Read it if you want to know more about how to write programs able to work with
 characters from languages other than English.

 @li @ref overview_whatisunicode
 @li @ref overview_unicodeandansi
 @li @ref overview_unicodeinsidewxw
 @li @ref overview_unicodeoutsidewxw
 @li @ref overview_unicodesettings
 @li @ref overview_topic8


 @section overview_whatisunicode What is Unicode?

 wxWidgets has support for compiling in Unicode mode
 on the platforms which support it. Unicode is a standard for character
 encoding which addresses the shortcomings of the previous, 8 bit standards, by
 using at least 16 (and possibly 32) bits for encoding each character. This
 allows to have at least 65536 characters (what is called the BMP, or basic
 multilingual plane) and possible 2^32 of them instead of the usual 256 and
 is sufficient to encode all of the world languages at once. More details about
 Unicode may be found at #http://www.unicode.org.

 As this solution is obviously preferable to the previous ones (think of
 incompatible encodings for the same language, locale chaos and so on), many
 modern operating systems support it. The probably first example is Windows NT
 which uses only Unicode internally since its very first version.

 Writing internationalized programs is much easier with Unicode and, as the
 support for it improves, it should become more and more so. Moreover, in the
 Windows NT/2000 case, even the program which uses only standard ASCII can profit
 from using Unicode because they will work more efficiently - there will be no
 need for the system to convert all strings the program uses to/from Unicode
 each time a system call is made.

 @section overview_unicodeandansi Unicode and ANSI modes

 As not all platforms supported by wxWidgets support Unicode (fully) yet, in
 many cases it is unwise to write a program which can only work in Unicode
 environment. A better solution is to write programs in such way that they may
 be compiled either in ANSI (traditional) mode or in the Unicode one.

 This can be achieved quite simply by using the means provided by wxWidgets.
 Basically, there are only a few things to watch out for:


 - Character type (@c char or @c wchar_t)
 - Literal strings (i.e. @c "Hello, world!" or @c '*')
 - String functions (@c strlen(), @c strcpy(), ...)
 - Special preprocessor tokens (@c __FILE__, @c __DATE__
 and @c __TIME__)


 Let's look at them in order. First of all, each character in an Unicode
 program takes 2 bytes instead of usual one, so another type should be used to
 store the characters (@c char only holds 1 byte usually). This type is
 called @c wchar_t which stands for @e wide-character type.

 Also, the string and character constants should be encoded using wide
 characters (@c wchar_t type) which typically take 2 or 4 bytes instead
 of @c char which only takes one. This is achieved by using the standard C
 (and C++) way: just put the letter @c 'L' after any string constant and it
 becomes a @e long constant, i.e. a wide character one. To make things a bit
 more readable, you are also allowed to prefix the constant with @c 'L'
 instead of putting it after it.

 Of course, the usual standard C functions don't work with @c wchar_t
 strings, so another set of functions exists which do the same thing but accept
 @c wchar_t * instead of @c char *. For example, a function to get the
 length of a wide-character string is called @c wcslen() (compare with
 @c strlen() - you see that the only difference is that the "str" prefix
 standing for "string" has been replaced with "wcs" standing for "wide-character
 string").

 And finally, the standard preprocessor tokens enumerated above expand to ANSI
 strings but it is more likely that Unicode strings are wanted in the Unicode
 build. wxWidgets provides the macros @c __TFILE__, @c __TDATE__
 and @c __TTIME__ which behave exactly as the standard ones except that
 they produce ANSI strings in ANSI build and Unicode ones in the Unicode build.

 To summarize, here is a brief example of how a program which can be compiled
 in both ANSI and Unicode modes could look like:

 @code
 #ifdef __UNICODE__
     wchar_t wch = L'*';
     const wchar_t *ws = L"Hello, world!";
     int len = wcslen(ws);

     wprintf(L"Compiled at %s\n", __TDATE__);
 #else // ANSI
     char ch = '*';
     const char *s = "Hello, world!";
     int len = strlen(s);

     printf("Compiled at %s\n", __DATE__);
 #endif // Unicode/ANSI
 @endcode

 Of course, it would be nearly impossibly to write such programs if it had to
 be done this way (try to imagine the number of @c #ifdef UNICODE an average
 program would have had!). Luckily, there is another way - see the next
 section.

 @section overview_unicodeinsidewxw Unicode support in wxWidgets

 In wxWidgets, the code fragment from above should be written instead:

 @code
 wxChar ch = wxT('*');
     wxString s = wxT("Hello, world!");
     int len = s.Len();
 @endcode

 What happens here? First of all, you see that there are no more @c #ifdefs
 at all. Instead, we define some types and macros which behave differently in
 the Unicode and ANSI builds and allow us to avoid using conditional
 compilation in the program itself.

 We have a @c wxChar type which maps either on @c char or @c wchar_t
 depending on the mode in which program is being compiled. There is no need for
 a separate type for strings though, because the standard
 #wxString supports Unicode, i.e. it stores either ANSI or
 Unicode strings depending on the compile mode.

 Finally, there is a special #wxT() macro which should enclose all
 literal strings in the program. As it is easy to see comparing the last
 fragment with the one above, this macro expands to nothing in the (usual) ANSI
 mode and prefixes @c 'L' to its argument in the Unicode mode.

 The important conclusion is that if you use @c wxChar instead of
 @c char, avoid using C style strings and use @c wxString instead and
 don't forget to enclose all string literals inside #wxT() macro, your
 program automatically becomes (almost) Unicode compliant!

 Just let us state once again the rules:

 - Always use @c wxChar instead of @c char
 - Always enclose literal string constants in #wxT() macro
 unless they're already converted to the right representation (another standard
 wxWidgets macro #_() does it, for example, so there is no
 need for @c wxT() in this case) or you intend to pass the constant directly
 to an external function which doesn't accept wide-character strings.
 - Use @c wxString instead of C style strings.

 @section overview_unicodeoutsidewxw Unicode and the outside world

 We have seen that it was easy to write Unicode programs using wxWidgets types
 and macros, but it has been also mentioned that it isn't quite enough.
 Although everything works fine inside the program, things can get nasty when
 it tries to communicate with the outside world which, sadly, often expects
 ANSI strings (a notable exception is the entire Win32 API which accepts either
 Unicode or ANSI strings and which thus makes it unnecessary to ever perform
 any conversions in the program). GTK 2.0 only accepts UTF-8 strings.

 To get an ANSI string from a wxString, you may use the
 mb_str() function which always returns an ANSI
 string (independently of the mode - while the usual
 #c_str() returns a pointer to the internal
 representation which is either ASCII or Unicode). More rarely used, but still
 useful, is wc_str() function which always returns
 the Unicode string.

 Sometimes it is also necessary to go from ANSI strings to wxStrings.
 In this case, you can use the converter-constructor, as follows:


 @code
 const char* ascii_str = "Some text";
    wxString str(ascii_str, wxConvUTF8);
 @endcode

 This code also compiles fine under a non-Unicode build of wxWidgets,
 but in that case the converter is ignored.

 For more information about converters and Unicode see
 the @ref overview_mbconvclasses.

 @section overview_unicodesettings Unicode-related compilation settings

 You should define @c wxUSE_UNICODE to 1 to compile your program in
 Unicode mode. This currently works for wxMSW, wxGTK, wxMac and wxX11. If you
 compile your program in ANSI mode you can still define @c wxUSE_WCHAR_T
 to get some limited support for @c wchar_t type.

 This will allow your program to perform conversions between Unicode strings and
 ANSI ones (using @ref overview_mbconvclasses)
 and construct wxString objects from Unicode strings (presumably read
 from some external file or elsewhere).

 @section overview_topic8 Traps for the unwary

 - Casting c_str() to void* is now char*, not wxChar*
 - Passing c_str(), mb_str() or wc_str() to variadic functions
 doesn't work

 */


