import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.Button

Methods yet to test:
__init__, Create, GetClassDefaultAttributes, SetDefault
"""

def getIdLabelPairs(without_mnemonic=True):
    """ID/Label pairs were copied from the docs"""
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
                #(wx.ID_REPLACE, 'Find and rep&lace'),
                    (wx.ID_REPLACE, 'Replace'), # unknown ampersand position
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

class ButtonTestFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Button(parent=self, id=wx.ID_ANY)
        

class ButtonTest(testControl.ControlTest):
    def __init__(self, arg):
        # superclass setup
        super(ButtonTest, self).__init__(arg)
        # ButtonTest setup
        
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = ButtonTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        self.children = []
        self.children_ids = []
        self.children_names = []
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()

    ##################
    ## Test Methods ##
    ##################
    
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
        

def suite():
    suite = unittest.makeSuite(ButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
