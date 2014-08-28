How to write unit tests for wxWidgets
=====================================

Unit tests for wxWidgets are written using small cppunit framework. To compile
(but not to run) them you need to have it installed. Hence the first part of
this note explains how to do it while the second one explains how to write the
test.

I. CppUnit Installation
-----------------------

1. Get it from http://www.sourceforge.net/projects/cppunit
   (latest version as of the time of this writing is 1.10.2)

2. Build the library:
 - Under Windows using VC++:
    - build everything in CppUnitLibraries.dsw work space
    - add include and lib subdirectories of the directory
      where you installed cppunit to the compiler search path
      using "Tools|Options" menu in VC IDE

 - Under Unix: run `configure && make && make install` as usual


II. Writing tests with CppUnit
------------------------------

1. Create a new directory tests/foo

2. Write a cpp file for the test copying, if you want,
   from one of the existing tests. The things to look for:

 a) #include "wx/cppunit.h" instead of directly including CppUnit headers

 b) don't put too many things in one test case nor in one method of a test
    case as it makes understanding what exactly failed harder later

 c) 'register' your tests as follows so that the test program will find and
    execute them:

    // register in the unnamed registry so that these tests are run by default
    CPPUNIT_TEST_SUITE_REGISTRATION(MBConvTestCase);

    // also include in its own registry so that these tests can be run alone
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(MBConvTestCase, "MBConvTestCase");

    Read CppUnit documentation for more.

 d) wxUIActionSimulator can be used when user input is required, for example
    clicking buttons or typing text. A simple example of this can be found
    in controls/buttontest.cpp. After simulating some user input always
    wxYield to allow event processing. When writing a test using
    wxUIActionSimulator always add the test using WXUISIM_TEST rather than
    CPPUNIT_TEST as then it won't run on unsupported platforms. The test itself
    must also be wrapped in a #if wxUSE_UIACTIONSIMULATOR block.

 e) There are a number of classes that are available to help with testing GUI
    elements. Firstly throughout the test run there is a frame of type
    wxTestableFrame that you can access through `wxTheApp->GetTopWindow()`. This
    class adds two new functions, GetEventCount, which takes an optional
    wxEventType. It then returns the number of events of that type that it has
    received since the last call. Passing nothing returns the total number of
    event received since the last call. Also there is OnEvent, which counts the
    events based on type that are passed to it. To make it easy to count events
    there is also a new class called EventCounter which takes a window and event
    type and connects the window to the top level wxTestableFrame with the specific
    event type. It disconnects again once it is out of scope. It simply reduces
    the amount of typing required to count events.

3. add a `<sources>` tag for your source file to tests/test.bkl. Make sure it's
   in the correct section: the one starting `<exe id="test_gui"` for a gui test,
   the one starting `<exe id="test" template="wx_sample_console` otherwise.


III. Running the tests
----------------------

1. Regenerate the make/project files from test.bkl using bakefile_gen, e.g.:
        cd build/bakefiles
        bakefile_gen -b ../../tests/test.bkl
   and if you're on a unix system re-run configure.

2. Build the test program using one of the make/project files in the tests
   subdirectory.

3. Run the test program by using the command 'test' for the console tests,
   'test_gui' for the gui ones. With no arguments, all the default set of tests
   (all those registered with CPPUNIT_TEST_SUITE_REGISTRATION) are run.
   Or to list the test suites without running them:
      test -l   or   test_gui -l

4. Tests that have been registered under a name using
   CPPUNIT_TEST_SUITE_NAMED_REGISTRATION can also be run separately. For
   example:
      test_gui ButtonTestCase
   or to list the tests done by a particular testcase:
      test -L MBConvTestCase

5. Fault navigation.
   VC++ users can run the programs as a post build step (Projects/Settings/
   Post-build step) to see the test results in an IDE window. This allows
   errors to be jumped to in the same way as for compiler errors, for
   example by pressing F4 or highlighting the error and pressing return.
   
   Similarly for makefile users: makefiles can be modified to execute the
   test programs as a final step. Then you can navigate to any errors in the
   same way as for compiler errors, if your editor supports that.

   Another alternative is to run the tests manually, redirecting the output
   to a file. Then use your editor to jump to any failures. Using Vim, for
   example, ':cf test.log' would take you to the first error in test.log, and
   ':cn' to the next.

   If you would like to set a breakpoint on a failing test using a debugger,
   put the breakpoint on the function 'CppUnit::Asserter::fail()'. This will
   stop on each failing test.


IV. Notes
---------

1. You can register your tests (or a subset of them) just under a name, and not
   in the unnamed registry if you don't want them to be executed by default.

2. If you are going to register your tests both in the unnamed registry
   and under a name, then use the name that the tests have in the 'test -l'
   listing.

3. Tests which fail can be temporarily registered under "fixme" while the
   problems they expose are fixed, instead of the unnamed registry. That
   way they can easily be run, but they do not make regression testing with
   the default suite more difficult. E.g.:

    // register in the unnamed registry so that these tests are run by default
    //CPPUNIT_TEST_SUITE_REGISTRATION(wxRegExTestCase);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(wxRegExTestCase, "fixme");

    // also include in its own registry so that these tests can be run alone
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(wxRegExTestCase, "wxRegExTestCase");

4. Tests which take a long time to execute can be registered under "advanced"
   instead of the unnamed registry. The default suite should execute reasonably
   quickly. To run the default and advanced tests together:
    test "" advanced
