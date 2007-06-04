--------------------------------------------------
            wxPython Unit Test Suite
--------------------------------------------------

The 'tests' folder contains the wxPython Unit Test Suite,
as well as additional test scripts.  When the wxWidgets
repository transitions to Subversion, these two bodies of code
will likely be separated.

Any script beginning with 'test_' is meant to be run manually, 
as is 'TreeMixinTest.py'.  These files were here first.

The Unit Test Suite comprises all scripts beginning with 'test'
which do not contain an underscore, as well as the 
'runAllTests.py' script.  If you need something more explicit,
'runAllTests.py imports every Unit Test module, so you can find
the complete list there.

Running all tests is straightfoward enough.  Typing
'python runAllTests.py' at the command line should do the trick.

It is entirely possible that the existing tests can and should 
be folded into this Test Suite, but that is another topic for
another day.
