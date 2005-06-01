#!/usr/bin/env python

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import unittest

import types

# Import from this module's parent directory.
import os
import sys
sys.path.insert(0, os.pardir)
import version
del sys.path[0]
del sys
del os


"""
These unittest methods are preferred:
-------------------------------------
self.assert_(expr, msg=None)
self.assertEqual(first, second, msg=None)
self.assertRaises(excClass, callableObj, *args, **kwargs)
self.fail(msg=None)
self.failIf(expr, msg=None)
"""


class ModuleTestCase(unittest.TestCase):

    def test_module(self):
        module = version
        self.assert_(module.__author__)
        self.assert_(module.__cvsid__)
        self.assert_(module.__revision__)
        self.assert_(module.VERSION)


class VersionTestCase(unittest.TestCase):

    def test_VERSION(self):
        self.assert_(type(version.VERSION) is types.StringType)


if __name__ == '__main__':
    unittest.main()
