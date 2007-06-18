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
import testFont
import testFrame
import testItemContainer
import testListBox
import testPoint
import testRect
import testSize
import testSizer
import testTextCtrl
import testTopLevelWindow
import testWindow

modules = (testButton, testColor, testControl, testFont, testFrame, testItemContainer,
            testListBox, testPoint, testRect, testSize, testSizer, testTextCtrl,
            testTopLevelWindow, testWindow)

alltests = unittest.TestSuite([mod.suite() for mod in modules])
maxstr = max(( mod.__name__ for mod in modules ))
maxlen = len(maxstr)

results = unittest.TestResult()
alltests.run(results)

total = 0
for module in modules:
    suite = module.suite()
    results = unittest.TestResult()
    suite.run(results)
    
    if results.wasSuccessful():
        print "%s:\t%d tests passed" % (module.__name__, results.testsRun)
        total += results.testsRun
    else:
        print "\n%d tests failed!\n" % (len(results.failures))
        for error in results.failures:
            print "------ " + str(error[0]) + " ------"
            print error[1]

        for error in results.errors:
            print "------ " + str(error[0]) + " ------"
            print error[1]

# TODO: add reporting for failures as well
print "----------------------"
print "%d tests passed in total!" % (total)

figleaf.stop()
figleaf.write_coverage(figfile)

# this is broken for some reason
#os.system("figleaf2html -d ./tests_code_coverage %s" % figfile)
