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
import testWindow
import testButton
import testFont
import testColor
import testControl
import testRect
import testPoint
import testSize

alltests = unittest.TestSuite(( testWindow.suite(),
                                testButton.suite(),
                                testFont.suite(),
                                testColor.suite(),
                                testControl.suite(),
                                testRect.suite(),
                                testPoint.suite(),
                                testSize.suite()
                                ))

results = unittest.TestResult()
alltests.run(results)

if results.wasSuccessful():
    print "%d tests passed!" % (results.testsRun)
else:
    print "\n%d tests failed!\n" % (len(results.failures))
    for error in results.failures:
        print "------ " + str(error[0]) + " ------"
        print error[1]
    
    for error in results.errors:
        print "------ " + str(error[0]) + " ------"
        print error[1]
    sys.exit(1)

figleaf.stop()
figleaf.write_coverage(figfile)

# this is broken for some reason
#os.system("figleaf2html -d ./tests_code_coverage %s" % figfile)
