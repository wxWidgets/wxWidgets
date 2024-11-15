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

Q. How to do translations to other language?
A. First of all, you will need the GNU gettext tools or, if you prefer GUI applications, a program such as poedit.
   After you've probably installed them, type the following (example is for Unix
   and you should do exactly the same under Windows).

   # all translations for a given language should be in a separate directory.
   # Please use the standard abbreviation for the language names!
   mkdir <language>
   cd <language>

   # generate the .pot (template) file for the program itself
   # see `xgettext --help' for options
   xgettext -k_ -kwxPLURAL:1,2 -kwxGETTEXT_IN_CONTEXT:1c,2 -kwxGETTEXT_IN_CONTEXT_PLURAL:1c,2,3 -kwxTRANSLATE -kwxTRANSLATE_IN_CONTEXT:1c,2 -kwxGetTranslation -o internat.pot ../internat.cpp

   # initialize the translation for your language
   msginit -l <language> -o internat.po

   # now edit the file and do translate strings (this isn't done by gettext)
   # you can use another editor if you wish :-)
   vi internat.po

   # create the message catalog file
   msgfmt -vco internat.mo internat.po

   # run the sample to test it
   cd ..
   ./internat <language>

Q. How to do update the translation of 'internat' sample for a language?
A. First of all, you will need the GNU gettext tools.
   After you've probably installed them, type the following (example is for Unix
   and you should do exactly the same under Windows).

   # enter the directory of an already-existing translations which needs to be updated
   cd <language>

   # generate the .pot (template) file for the program itself
   # see `xgettext --help' for options
   xgettext -k_ -kwxPLURAL:1,2 -kwxGETTEXT_IN_CONTEXT:1c,2 -kwxGETTEXT_IN_CONTEXT_PLURAL:1c,2,3 -kwxTRANSLATE -kwxTRANSLATE_IN_CONTEXT:1c,2 -kwxGetTranslation -o internat.pot ../internat.cpp

   # merge updated strings from the .pot (template) into your translation
   msgmerge -U internat.po internat.pot

   # now edit the file and do translate the new strings (this isn't done by gettext)
   vi internat.po

   # update the message catalog:
   msgfmt -vco internat.mo internat.po

Q. What's i18n?
A. Count the number of letters in the word "internationalization".
