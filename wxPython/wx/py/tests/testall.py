#!/usr/bin/env python

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


import unittest
import glob
import os


def suite():
    """Return a test suite containing all test cases in all test modules. 
    Searches the current directory for any modules matching test_*.py."""
    suite = unittest.TestSuite()
    for filename in glob.glob('test_*.py'):
        module = __import__(os.path.splitext(filename)[0])
        suite.addTest(unittest.defaultTestLoader.loadTestsFromModule(module))
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')

