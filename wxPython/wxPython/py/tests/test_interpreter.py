#!/usr/bin/env python

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import unittest

# Import from this module's parent directory.
import os
import sys
sys.path.insert(0, os.pardir)
import interpreter
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
        module = interpreter
        self.assert_(module.__author__)
        self.assert_(module.__cvsid__)
        self.assert_(module.__revision__)
        self.assert_(module.Interpreter)
        self.assert_(module.Interpreter.push)
        self.assert_(module.Interpreter.runsource)
        self.assert_(module.Interpreter.getAutoCompleteList)
        self.assert_(module.Interpreter.getCallTip)
        self.assert_(module.InterpreterAlaCarte)


class InterpreterTestCase(unittest.TestCase):

    def setUp(self):
        self.output = ''
        self.i = interpreter.Interpreter(stdout=self)

    def write(self, text):
        """Capture output from self.i.push()."""
        self.output += text

    def tearDown(self):
        self.output = ''
        self.i = None
        del self.i

    def test_more(self):
        self.assertEqual(self.i.push('dir()'), 0)
        self.assertEqual(self.i.push('for n in range(3):'), 1)

    def test_push(self):
        values = (
        ('dir', '<built-in function dir>'),
        ('dir()', "['__builtins__', '__doc__', '__name__']"),
        ('2 + 2', '4'),
        ('d = {}', ''),
        ('d', '{}'),
        ('del d', ''),
        ('len([4,5,6])', '3'),
        )
        for input, output in values:
            if output: output += '\n'
            self.i.push(input)
            self.assertEqual(self.output, output)
            self.output = ''


if __name__ == '__main__':
    unittest.main()
