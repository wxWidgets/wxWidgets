/////////////////////////////////////////////////////////////////////////////
// Name:        nonenglish
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!
 
 @page nonenglish_overview Writing non-English applications
 
 This article describes how to write applications that communicate with
 the user in a language other than English. Unfortunately many languages use
 different charsets under Unix and Windows (and other platforms, to make
 the situation even more complicated). These charsets usually differ in so
 many characters that it is impossible to use the same texts under all
 platforms.
 The wxWidgets library provides a mechanism that helps you avoid distributing many
 identical, only differently encoded, packages with your application 
 (e.g. help files and menu items in iso8859-13 and windows-1257). Thanks
 to this mechanism you can, for example, distribute only iso8859-13 data 
 and it will be handled transparently under all systems.
 Please read #Internationalization which
 describes the locales concept.
 In the following text, wherever @e iso8859-2 and @e windows-1250 are
 used, any encodings are meant and any encodings may be substituted there.
 @b Locales
 The best way to ensure correctly displayed texts in a GUI across platforms
 is to use locales. Write your in-code messages in English or without 
 diacritics and put real messages into the message catalog (see 
 #Internationalization).
 A standard .po file begins with a header like this:
 
 @code
 # SOME DESCRIPTIVE TITLE.
 # Copyright (C) YEAR Free Software Foundation, Inc.
 # FIRST AUTHOR EMAIL@ADDRESS, YEAR.
 #
 msgid ""
 msgstr ""
 "Project-Id-Version: PACKAGE VERSION\n"
 "POT-Creation-Date: 1999-02-19 16:03+0100\n"
 "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
 "Last-Translator: FULL NAME EMAIL@ADDRESS\n"
 "Language-Team: LANGUAGE LL@li.org\n"
 "MIME-Version: 1.0\n"
 "Content-Type: text/plain; charset=CHARSET\n"
 "Content-Transfer-Encoding: ENCODING\n"
 @endcode
 
 Note this particular line:
 
 @code
 "Content-Type: text/plain; charset=CHARSET\n"
 @endcode
 
 It specifies the charset used by the catalog. All strings in the catalog
 are encoded using this charset.
 You have to fill in proper charset information. Your .po file may look like this
 after doing so: 
 
 @code
 # SOME DESCRIPTIVE TITLE.
 # Copyright (C) YEAR Free Software Foundation, Inc.
 # FIRST AUTHOR EMAIL@ADDRESS, YEAR.
 #
 msgid ""
 msgstr ""
 "Project-Id-Version: PACKAGE VERSION\n"
 "POT-Creation-Date: 1999-02-19 16:03+0100\n"
 "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
 "Last-Translator: FULL NAME EMAIL@ADDRESS\n"
 "Language-Team: LANGUAGE LL@li.org\n"
 "MIME-Version: 1.0\n"
 "Content-Type: text/plain; charset=iso8859-2\n"
 "Content-Transfer-Encoding: 8bit\n"
 @endcode
 
 (Make sure that the header is @b not marked as @e fuzzy.)
 wxWidgets is able to use this catalog under any supported platform
 (although iso8859-2 is a Unix encoding and is normally not understood by
 Windows).
 How is this done? When you tell the wxLocale class to load a message catalog that
 contains a correct header, it checks the charset. The catalog is then converted
 to the charset used (see
 wxLocale::GetSystemEncoding and
 wxLocale::GetSystemEncodingName) by
 the user's operating system. This is the default behaviour of the
 #wxLocale class; you can disable it by @b not passing
 @c wxLOCALE_CONV_ENCODING to wxLocale::Init.
 @b Non-English strings or 8-bit characters in the source code
 By convention, you should only use characters without diacritics (i.e. 7-bit
 ASCII strings) for msgids in the source code and write them in English.
 If you port software to wxWindows, you may be confronted with legacy source
 code containing non-English string literals. Instead of translating the strings
 in the source code to English and putting the original strings into message
 catalog, you may configure wxWidgets to use non-English msgids and translate to
 English using message catalogs:
 
 
 If you use the program @c xgettext to extract the strings from
 the source code, specify the option @c --from-code=source code charset.
 Specify the source code language and charset as arguments to
 wxLocale::AddCatalog. For example:
 
 @code
 locale.AddCatalog(_T("myapp"),
                   wxLANGUAGE_GERMAN, _T("iso-8859-1"));
 @endcode
 
 
 
 
 @b Font mapping
 You can use @ref mbconvclasses_overview and 
 #wxFontMapper to display text:
 
 @code
 if (!wxFontMapper::Get()-IsEncodingAvailable(enc, facename))
 {
    wxFontEncoding alternative;
    if (wxFontMapper::Get()-GetAltForEncoding(enc, ,
                                               facename, @false))
    {
        wxCSConv convFrom(wxFontMapper::Get()-GetEncodingName(enc));
        wxCSConv convTo(wxFontMapper::Get()-GetEncodingName(alternative));
        text = wxString(text.mb_str(convFrom), convTo);
    }
    else
        ...failure (or we may try iso8859-1/7bit ASCII)...
 }
 ...display text...
 @endcode
 
 @b Converting data
 You may want to store all program data (created documents etc.) in
 the same encoding, let's say @c utf-8. You can use
 #wxCSConv class to convert data to the encoding used by the
 system your application is running on (see
 wxLocale::GetSystemEncoding).
 @b Help files
 If you're using #wxHtmlHelpController there is
 no problem at all. You only need to make sure that all the HTML files contain
 the META tag, e.g.
 
 @code
 meta http-equiv="Content-Type" content="text/html; charset=iso8859-2"
 @endcode
 
 and that the hhp project file contains one additional line in the @c OPTIONS
 section:
 
 @code
 Charset=iso8859-2
 @endcode
 
 This additional entry tells the HTML help controller what encoding is used
 in contents and index tables.
 
 */
 
 
