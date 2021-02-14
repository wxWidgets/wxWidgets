/////////////////////////////////////////////////////////////////////////////
// Name:        internationalization.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_i18n Internationalization

@tableofcontents

Although internationalization of an application (i18n for short) involves far
more than just translating its text messages to another message - date, time
and currency formats need changing too, some languages are written left to
right and others right to left, character encoding may differ and many other
things may need changing too - it is a necessary first step. wxWidgets provides
facilities for message translation with its wxLocale class and is itself fully
translated into several languages. Please consult wxWidgets home page for the
most up-to-date translations - and if you translate it into one of the
languages not done yet, your translations would be gratefully accepted for
inclusion into future versions of the library!

The wxWidgets approach to i18n closely follows the GNU gettext package.
wxWidgets uses the message catalogs which are binary compatible with gettext
catalogs and this allows to use all of the programs in this package to work
with them as well as using any of the tools working with message catalogs in
this format such as <a href="http://poedit.net/">Poedit</a>.

Because of this, you will need to use the gettext package to work with the
translations during the program development. However no additional libraries
are needed during run-time, so you have only the message catalogs to distribute
and nothing else.

There are two kinds of message catalogs: source catalogs which are text files
with extension .po and binary catalogs which are created from the source ones
with @e msgfmt program (part of gettext package) and have the extension .mo.
Only the binary files are needed during program execution.

Translating your application involves several steps:

@li Translating the strings in the program text using wxGetTranslation or
    equivalently the @c _() macro.
@li Extracting the strings to be translated from the program: this uses the
    work done in the previous step because @c xgettext program used for string
    extraction recognises the standard @c _() as well as (using its @c -k
    option) our wxGetTranslation and extracts all strings inside the calls to
    these functions. Alternatively, you may use @c -a option to extract all the
    strings, but it will usually result in many strings being found which don't
    have to be translated at all. This will create a text message catalog - a
    .po file.
@li Translating the strings extracted in the previous step to other
    language(s). It involves editing the .po file.
@li Compiling the .po file into .mo file to be used by the program.
@li Installing the .mo files with your application in the appropriate location
    for the target system (see @ref overview_i18n_mofiles).
@li Setting the appropriate locale in your program to use the strings for the
    given language: see wxLocale.


@note Under macOS you also need to list all the supported languages under
      @c CFBundleLocalizations key in your application @c Info.plist file
      in order to allow the application to support the corresponding locale.


@section overview_i18n_mofiles Installing translation catalogs

The .mo files with compiled catalogs must be included with the application.
By default, wxFileTranslationsLoader is used to load them from files installed
alongside the application (although you could use wxResourceTranslationsLoader
or some custom loader too).

The files are expected to be in the resources directory (as returned by
wxStandardPaths::GetLocalizedResourcesDir(wxStandardPaths::ResourceCat_Messages).
If the message catalogs are not installed in this default location you may
explicitly use wxFileTranslationsLoader::AddCatalogLookupPathPrefix() to still
allow wxWidgets to find them, but it is recommended to use the default
locations when possible.

Depending on the platform, the default location differs. On Windows, it is
alongside the executable. On Unix, translations are expected to be in
"$prefix/share/locale". On macOS, application bundle's @em Resources subdirectory
is used.

In all cases, translations are searched for in subdirectories named using the
languages codes from ISO 639. The .mo file(s) should be located either directly
in that directory or in LC_MESSAGES subdirectory. On macOS, ".lproj" extension
is used for the per-languages Resources subdirectories.

Here's how an app would typically install the files on Unix:
@code
/usr/bin/myapp
/usr/share/locale/de/LC_MESSAGES/myapp.mo
/usr/share/locale/fr/LC_MESSAGES/myapp.mo
@endcode
And on macOS:
@code
MyApp.app/Contents/MacOS/MyApp
MyApp.app/Contents/Resources/de.lproj/myapp.mo
MyApp.app/Contents/Resources/fr.lproj/myapp.mo
@endcode
And on Windows:
@code
C:\Program Files\MyApp\myapp.exe
C:\Program Files\MyApp\de\myapp.mo
C:\Program Files\MyApp\fr\myapp.mo
@endcode
It is of course possible to use the Unix layout everywhere instead.


@section overview_i18n_menuaccel Translating Menu Accelerators

If you translate the accelerator modifier names (Ctrl, Alt and Shift) in your
menu labels, you may find the accelerators no longer work. In your message
catalogs, you need to provide individual translations of these modifiers from
their lower case names (ctrl, alt, shift) so that the wxWidgets accelerator
code can recognise them even when translated. wxWidgets does not provide
translations for all of these currently. wxWidgets does not yet handle
translated special key names such as Backspace, End, Insert, etc.


@see
@li The gettext Manual: http://www.gnu.org/software/gettext/manual/gettext.html
@li @ref overview_nonenglish - It focuses on handling charsets related problems.
@li @ref page_samples_internat - Shows you how all this looks in practice.

*/
