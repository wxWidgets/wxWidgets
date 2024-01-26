This is the README file for the internationalization sample for wxWidgets.

Q. What does this stupid program do?
A. It demonstrates how to translate all program messages to a foreign language.
   In any program using wxWidgets there is going to be 3 kinds of messages: the
   messages from the program itself, the messages from the wxWidgets library and
   the messages from the system (e.g. system error messages). This program
   translates the first 2 kinds of messages but the system messages will be only
   translated if your system supports it.

   Brief usage summary: "Test|File" tries to open a non existing file (provoking
   system error message), "Test|Play" shows a message box asking for a number.
   Hint: try some special values like 9 and 17.

Q. Why the error message when I try to open a non existing file is only partly
   translated?
A. Your system doesn't have the translation in the language you use, sorry.

Q. Why the message when I enter '9' is not translated?
A. This is on purpose: the corresponding string wasn't enclosed in _() macro and
   so didn't get into the message catalog when it was created using xgettext.

Q. Why the message when I enter '17' is only partly translated?
A. This will only work under some versions of Linux, don't worry if the second
   half of the sentence is not translated.

Q. I don't speak french, what about translations to <language>?
A. Please write them - see the next question. French is chosen by default
   because it's the only translation which is currently available. To test
   translations to the other languages please run the sample with 2 command line
   arguments: the full language name and the name of the directory where the
   message catalogs for this language are (will be taken as 2 first letters of
   the language name if only 1 argument is given).

Q. How to do translations to other language?
A. First of all, you will need the GNU gettext tools (see the next questions).
   After you've probably installed them, type the following (example is for Unix
   and you should do exactly the same under Windows).

   # all translations forgiven language should be in a separate directory.
   # Please use the standard abbreviation for the language names!
   mkdir <language>
   cd <language>

   # generate the .po file for the program itself
   # see `xgettext --help' for options, "-C" is important!
   xgettext -C -k_ -kwxPLURAL:1,2 -kwxGETTEXT_IN_CONTEXT:1c,2 -kwxGETTEXT_IN_CONTEXT_PLURAL:1c,2,3 -kwxTRANSLATE -kwxTRANSLATE_IN_CONTEXT:1c,2 -kwxGetTranslation -o internat.po ../internat.cpp

   # .po file for wxWidgets might be generated in the same way. An already
   # generated wxstd.pot as well as translations for some languages can be
   # found in the locale directory.
   cp ../../locale/<language>.po ./wxstd.pot
   - or -
   cp ../../locale/wxstd.pot .

   # now edit the files and do translate strings (this isn't done by gettext)
   # you can use another editor if you wish :-) No need to edit wxstd.pot if you
   # already got a translated one.
   vi internat.po wxstd.pot

   # create the message catalog files
   msgfmt -o internat.mo internat.po
   msgfmt -o wxstd.mo wxstd.pot

   # run the sample to test it
   cd ..
   ./internat <language>

Q. How to do update the translation of 'internat' sample for a language?
A. First of all, you will need the GNU gettext tools (see the next question).
   After you've probably installed them, type the following (example is for Unix
   and you should do exactly the same under Windows).

   # enter the directory of an already-existing translations which needs to be updated
   cd <language>

   # the -j flag tells xgettext to merge and not simply overwrite the output file
   xgettext -j -C -k_ -kwxPLURAL:1,2 -kwxGETTEXT_IN_CONTEXT:1c,2 -kwxGETTEXT_IN_CONTEXT_PLURAL:1c,2,3 -kwxTRANSLATE -kwxTRANSLATE_IN_CONTEXT:1c,2 -kwxGetTranslation -o internat.po ../internat.cpp

   # now edit the files and do translate the new strings (this isn't done by gettext)
   vi internat.po

   # update the message catalog:
   msgfmt -o internat.mo internat.po

Q. How to get the gettext tools?
A. For Unix, you should be able to get the source distribution of any GNU mirror
   (see www.gnu.org for a start). gettext() version 0.10 is buggy, try to get at
   least version strictly greater than 0.10. gettext RPMs can be downloaded from
   the standard locations for Linux. For Windows, you can get the precompiled
   binaries from wxWidgets web page.

Q. What's i18n?
A. Count the number of letters in the word "internationalization".

Q. Where to send comments, or additional translations?
A. wxWidgets mailing list <wx-dev@googlegroups.com>.
