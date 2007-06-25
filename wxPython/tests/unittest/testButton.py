import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.Button

Methods yet to test:
__init__, Create, GetClassDefaultAttributes
"""

def getIdLabelPairs(without_mnemonic=True):
    """ID/Label pairs were copied from the docs.
    Upon inspection of the wx package, there appear to be more IDs than
    are documented here.
    TODO: determine if the below (in the docs) are the only IDs applying to
        instances of wx.Button, or if there are omissions"""
    pairs = (
                (wx.ID_ADD,         'Add'),
                (wx.ID_APPLY,       '&Apply'),
                (wx.ID_BOLD,        '&Bold'),
                (wx.ID_CLEAR,       '&Clear'),
                (wx.ID_CLOSE,       '&Close'),
                (wx.ID_COPY,        '&Copy'),
                (wx.ID_CUT,         'Cu&t'),
                (wx.ID_DELETE,      '&Delete'),
                (wx.ID_FIND,        '&Find'),
                # TODO: fix docs or code!
                (wx.ID_REPLACE, 'Find and rep&lace'),
                # here's what wx.ID_REPLACE actually is (on Windows at least)
                #(wx.ID_REPLACE, 'Replace'), # unknown ampersand position
                (wx.ID_BACKWARD,    '&Back'),
                (wx.ID_DOWN,        '&Down'),
                (wx.ID_FORWARD,     '&Forward'),
                (wx.ID_UP,          '&Up'),
                (wx.ID_HELP,        '&Help'),
                (wx.ID_HOME,        '&Home'),
                (wx.ID_INDENT,      'Indent'),
                (wx.ID_INDEX,       '&Index'),
                (wx.ID_ITALIC,      '&Italic'),
                (wx.ID_JUSTIFY_CENTER, 'Centered'),
                (wx.ID_JUSTIFY_FILL, 'Justified'),
                (wx.ID_JUSTIFY_LEFT, 'Align Left'),
                (wx.ID_JUSTIFY_RIGHT, 'Align Right'),
                (wx.ID_NEW,         '&New'),
                (wx.ID_NO,          '&No'),
                (wx.ID_OK,          '&OK'),
                (wx.ID_OPEN,        '&Open'),
                (wx.ID_PASTE,       '&Paste'),
                (wx.ID_PREFERENCES, '&Preferences'),
                (wx.ID_PRINT,       '&Print'),
                (wx.ID_PREVIEW,     'Print previe&w'),
                (wx.ID_PROPERTIES,  '&Properties'),
                (wx.ID_EXIT,        '&Quit'),
                (wx.ID_REDO,        '&Redo'),
                (wx.ID_REFRESH,     'Refresh'),
                (wx.ID_REMOVE,      'Remove'),
                (wx.ID_REVERT_TO_SAVED, 'Revert to Saved'),
                (wx.ID_SAVE,        '&Save'),
                (wx.ID_SAVEAS,      'Save &As...'),
                (wx.ID_STOP,        '&Stop'),
                (wx.ID_UNDELETE,    'Undelete'),
                (wx.ID_UNDERLINE,   '&Underline'),
                (wx.ID_UNDO,        '&Undo'),
                (wx.ID_UNINDENT,    '&Unindent'),
                (wx.ID_YES,         '&Yes'),
                (wx.ID_ZOOM_100,    '&Actual Size'),
                (wx.ID_ZOOM_FIT,    'Zoom to &Fit'),
                (wx.ID_ZOOM_IN,     'Zoom &In'),
                (wx.ID_ZOOM_OUT,    'Zoom &Out')
        )
    if without_mnemonic:
        return tuple( (id,label.replace('&','')) for id,label in pairs )
    else:
        return pairs
        

# -----------------------------------------------------------

class ButtonTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Button(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
    
    def testGetDefaultSize(self):
        """GetDefaultSize
        (Static method)"""
        sz = wx.Button.GetDefaultSize()
        self.assert_(isinstance(sz, wx.Size))
        self.assert_(sz.IsFullySpecified())
        
    def testIdLabelPairs(self):
        """GetLabelText (from wx.Control)
        Test the ID/Label pairs"""
        for id,label in getIdLabelPairs():
            b = wx.Button(self.frame, id)
            self.assertEquals(label, b.GetLabelText())
    
    def testSetDefault(self):
        """SetDefault"""
        self.testControl.SetDefault()
        self.assertEquals(self.testControl, self.frame.DefaultItem)
        

def suite():
    suite = unittest.makeSuite(ButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
