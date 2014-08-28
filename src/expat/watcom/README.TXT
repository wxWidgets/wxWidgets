
                        Expat, Release 2.0.0
                 Open Watcom OS/2-eCS and NT/2000/XP

SUMMARY
=======
This is the first effort of adding Open Watcom (www.openwatcom.org) build
capability to the expat project. The included changes and makefiles are
verified to work with Open Watcom 1.5, however, it might build with
version 1.4.

HISTORY
=======
June 2006 - First successful compile with Open Watcom 1.5
July 2006 - Submit as patch

BUILDING
========
The makefile system is based on a multi-platform system provided by
Mat Nieuwenhoven. In the watcom directory is the main makefile which
accepts the following options:

   os2       - OS/2-ECS Release
   nt        - WinNT/2000/XP Release
   linux     - Linux Release
   os2d      - OS/2-ECS Debug
   ntd       - WinNT/2000/XP Debug
   linuxd    - Linux Debug
   full      - all release
   fulld     - all debug
   fullall   - all release and debug
   cleanall  - remove all
   clean     - remove build keep release
   xmlts.zip - download test suite and unzip

The options for the OW build of expat are set in watcom\buildopts.inc.

The following expat options are available:

XML_DTD (Note 1)
    Include support for using and reporting DTD-based content. If this
    is defined, default attribute values from an external DTD subset are
    reported and attribute value normalization occurs based on the type of
    attributes defined in the external subset. Without this, Expat has a
    smaller memory footprint and can be faster, but will not load external
    entities or process conditional sections. This does not affect the set
    of functions available in the API.

XML_NS (Note 1)
    When defined, support for the Namespaces in XML specification is included.

XML_UNICODE (Note 2)
    When defined, character data reported to the application is encoded
    in UTF-16 using wide characters of the type XML_Char. This is implied
    if XML_UNICODE_WCHAR_T is defined.

XML_UNICODE_WCHAR_T (Note 2)
    If defined, causes the XML_Char character type to be defined using the
    wchar_t type; otherwise, unsigned short is used. Defining this implies
    XML_UNICODE.

XML_LARGE_SIZE (Note 3)
    (OS/2 and Windows) [optional] If defined, causes the XML_Size and XML_Index
    integer types to be at least 64 bits in size. This is intended to support
    processing of very large input streams, where the return values of
    XML_GetCurrentByteIndex, XML_GetCurrentLineNumber and
    XML_GetCurrentColumnNumber could overflow. It may not be supported by all
    compilers, and is turned off by default.

XML_CONTEXT_BYTES (Note 1)
    The number of input bytes of markup context which the parser will ensure are
    available for reporting via XML_GetInputContext. This is normally set to 1024,
    and must be set to a positive interger. If this is not defined, the input
    context will not be available and XML_GetInputContext will always report NULL.
    Without this, Expat has a smaller memory footprint and can be faster.

XML_MIN_SIZE (Note 3)
    (OS/2 and Windows) [optional] Makes a parser that's smaller but that,
    in general, will run slower.

Note 1: Define by default in watcomconfig.h - XML_CONTEXT_BYTES define as 1024.

Note 2: Not yet supported

Note 3: Not defined by default, but supported by Open Watcom


XML_TEST_SUITE
==============
While you are welcome to run the tests, however, it will require downloading and
setting up additional unix type utilitiy/tools on OS/2-ECS and Windows platforms.

I have added the target xmlts.zip to the makefile which will use wget to dowload
and unzip to setup the test files. The tests/xmltest.sh file will have to be
modified, changing XMLWF= to the location of the xmlwwf.exe file.

The ouput from both OS2-ECS and WinNT/2000/XP builds is listed below. I have
built expat 2.0.0 on a Linux gcc system and verified that the output is the same
result for OS/2-ECS and Windows platforms. So, I feel confident that the Open Watcom
build is working as well as the other compiler builds.

The test results should be:

  Output differs: ibm/valid/P02/ibm02v01.xml
  Output differs: ibm/valid/P28/ibm28v02.xml
  Output differs: ibm/valid/P29/ibm29v01.xml
  Output differs: ibm/valid/P29/ibm29v02.xml
  Output differs: ibm/valid/P54/ibm54v01.xml
  Output differs: ibm/valid/P56/ibm56v08.xml
  Output differs: ibm/valid/P57/ibm57v01.xml
  Output differs: ibm/valid/P58/ibm58v01.xml
  Output differs: ibm/valid/P58/ibm58v02.xml
  Output differs: ibm/valid/P70/ibm70v01.xml
  Output differs: ibm/valid/P82/ibm82v01.xml
  E:/expat-2.0.0/tests/XML-Test-Suite/out/ibm/invalid/P29//*.xml: No such file or directory
  ibm49i02.dtd: No such file or directory
  In ibm/invalid/P49/: ibm49i02.xml:4:1: error in processing external entity reference
  Output differs: ibm/invalid/P58/ibm58i01.xml
  Output differs: ibm/invalid/P58/ibm58i02.xml
  Output differs: xmltest/valid/sa/069.xml
  Output differs: xmltest/valid/sa/076.xml
  Output differs: xmltest/valid/sa/090.xml
  Output differs: xmltest/valid/sa/091.xml
  Output differs: sun/valid/not-sa01.xml
  Output differs: sun/valid/not-sa02.xml
  Output differs: sun/valid/not-sa03.xml
  Output differs: sun/valid/not-sa04.xml
  Output differs: sun/valid/notation01.xml
  Output differs: sun/valid/sa02.xml
  Output differs: sun/valid/sa03.xml
  Output differs: sun/valid/sa04.xml
  Output differs: sun/valid/sa05.xml
  Expected well-formed: ibm/not-wf/misc/432gewf.xml
  Expected well-formed: xmltest/not-wf/not-sa/005.xml
  Expected well-formed: sun/not-wf/uri01.xml
  Expected well-formed: oasis/p06fail1.xml
  Expected well-formed: oasis/p08fail1.xml
  Expected well-formed: oasis/p08fail2.xml
  Passed: 1776
  Failed: 34






