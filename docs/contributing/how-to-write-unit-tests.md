How to write unit tests for wxWidgets
=====================================

wxWidgets unit tests use [Catch](http://catch-lib.net/) framework. It is
included in wxWidgets as a submodule, so you will need to run

    $ git submodule update --init 3rdparty/catch

to get it before the first use. Catch is header-only and doesn't need to be
compiled.

Testing with Catch
------------------

**WARNING**: Most of the existing tests are currently still written in the
CppUnit style, please do _not_ follow them when writing new tests, the old
style is too complex and unnecessary.

Writing tests with Catch is almost embarrassingly simple: you need to just
add a new test case and use Catch assertion macros inside it, e.g.

    TEST_CASE("MyNewTest", "[my][new][another-tag]")
    {
        wxString s("Hello, world!");
        CHECK( s.BeforeFirst(",") == "Hello" );
        CHECK( s.AfterLast(" ") == "world!" );
    }

This is all, the new test will be automatically run when you run the full test
suite or you can run just it using

    $ ./test MyNewTest

(see below for more about running tests).

See [Catch tutorial](https://github.com/philsquared/Catch/blob/v1.11.0/docs/tutorial.md)
for more information.


Tests physical structure
------------------------

All (i.e. both GUI and non-GUI) unit tests are under `tests` subdirectory. When
adding a new test, try to find an existing file to add it to. If there are no
applicable files, try to add a new file to an existing directory. If there is
no applicable directory neither, create a new one and put the new file there
(i.e. do _not_ put new files directly under `tests`). If your test is small,
consider adding it to `tests/misc/misctests.cpp`.

If you add a new file, you need to update `tests/test.bkl` and add a
`<sources>` tag for your new file.bkl. Make sure it's in the correct section:
the one starting `<exe id="test_gui"` for a gui test, the one starting `<exe
id="test" template="wx_sample_console` otherwise. After modifying this file,
rerun bakefile to regenerate the tests make- and project files:

    $ cd build/bakefiles
    $ bakefile_gen -b ../../tests/test.bkl


Writing GUI-specific tests
--------------------------

`wxUIActionSimulator` can be used when user input is required, for example
clicking buttons or typing text. A simple example of this can be found in
`tests/controls/buttontest.cpp`. After simulating some user input always
call `wxYield()` to allow event processing. When writing a test using
`wxUIActionSimulator` wrap it in `#if wxUSE_UIACTIONSIMULATOR` block.

There are a number of classes that are available to help with testing GUI
elements. Firstly throughout the test run there is a frame of type
`wxTestableFrame` that you can access through `wxTheApp->GetTopWindow()`. This
class adds two new functions, `GetEventCount()`, which takes an optional
`wxEventType`. It then returns the number of events of that type that it has
received since the last call. Passing nothing returns the total number of event
received since the last call. Also there is `OnEvent()`, which counts the events
based on type that are passed to it. To make it easy to count events there is
also a new class called `EventCounter` which takes a window and event type and
connects the window to the top level `wxTestableFrame` with the specific event
type. It disconnects again once it is out of scope. It simply reduces the
amount of typing required to count events.


Running the tests
-----------------

Run the main test suite by using the command `test` for the console tests,
or `test_gui` for the GUI ones. With no arguments, all the default set of tests
(all those registered without `[hide]` tag) are run.

To list the test suites without running them use `-l` command-line option.

To run a particular test case, use `./test NameTestCase`. To run all tests
using the specified tag, use `./test [tag_name]` (the square brackets may need
to be escaped from your shell).
