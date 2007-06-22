import os, sys

rootdir = os.path.abspath(sys.path[0])
if not os.path.isdir(rootdir):
    rootdir = os.path.dirname(rootdir)
figfile = os.path.join(rootdir, 'tests.figleaf')

if os.path.exists(figfile):
    os.remove(figfile)

# note figleaf dependency
# may want to revisit this issue in the future
import figleaf
figleaf.start(ignore_python_lib=False)

import unittest
# individual modules of the test suite:
import testButton
import testColor
import testControl
import testControlWithItems
import testFont
import testFrame
import testItemContainer
import testListBox
import testPoint
import testRect
import testSize
import testSizer
import testStaticText
import testTextCtrl
import testTopLevelWindow
import testWindow

modules = (testButton, testColor, testControl, testControlWithItems, testFont, testFrame, 
            testItemContainer, testListBox, testPoint, testRect, testSize, testSizer,
            testStaticText, testTextCtrl, testTopLevelWindow, testWindow)

alltests = unittest.TestSuite([mod.suite() for mod in modules])
maxstr = max(( mod.__name__ for mod in modules ))
maxlen = len(maxstr)

results = unittest.TestResult()
alltests.run(results)

total_successes = 0
total_failures  = 0
total_errors    = 0
for module in modules:
    # run suite
    suite = module.suite()
    results = unittest.TestResult()
    suite.run(results)
    # report on it
    failures  = len(results.failures)
    errors    = len(results.errors)
    successes = results.testsRun - failures - errors
    total_failures  += failures
    total_errors    += errors
    total_successes += successes
    print "%s:\t%d tests passed" % (module.__name__, successes)
    if failures > 0:
        print "%s:\t%d tests failed!\n" % (module.__name__, failures)
        for failure in results.failures:
            print "------ " + str(failure[0]) + " ------"
            print failure[1]
    if errors > 0:
        print "%s:\t%d tests in error!\n" % (module.__name__, errors)
        for error in results.errors:
            print "------ " + str(error[0]) + " ------"
            print error[1]


print "----------------------"
print "%d tests passed in total!" % (total_successes)
if total_failures > 0:
    print "%d tests failed in total!" % (total_failures)
if total_errors > 0:
    print "%d tests erred in total!" % (total_errors)

figleaf.stop()
figleaf.write_coverage(figfile)

# this is broken for some reason
#os.system("figleaf2html -d ./tests_code_coverage %s" % figfile)
