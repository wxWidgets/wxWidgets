import unittest
import wx

'''
This file contains classes and methods for unit testing the API of wx.Window.

TODO: use this test class as the base of a unittest inheritance hierarchy,
    so that each class derived from wx.Window will have all the WindowTests
    run against it.

To find out where a particular method is tested, search for the name of that
method.  Each test contains, as a docstring, the names of the methods tested
within them.  Additionally, the following methods have yet to have tests
written for them:
    
AcceptsFocus, AcceptsFocusFromKeyboard, AddChild, AdjustForLayoutDirection, AssociateHandle, 
CacheBestSize, CanSetTransparent, CaptureMouse, CenterOnParent, CentreOnParent, 
ClearBackground, ClientToScreen, ClientToScreenXY, Close, ConvertDialogPointToPixels, 
ConvertDialogSizeToPixels, ConvertPixelPointToDialog, ConvertPixelSizeToDialog, 
Create, Destroy, DestroyChildren, DissociateHandle, DLG_PNT, DLG_SZE, DragAcceptFiles, 
FindFocus, Fit, FitInside, GetAutoLayout, GetBestSize, 
GetBestSizeTuple, GetBestVirtualSize, GetBorder, GetCapture, GetCaret, GetCharHeight, 
GetCharWidth, GetChildren, GetClassDefaultAttributes, GetClientAreaOrigin, GetClientRect, 
GetClientSize, GetClientSizeTuple, GetConstraints, GetContainingSizer, GetCursor, 
GetDefaultAttributes, GetDropTarget, GetEffectiveMinSize, GetEventHandler, GetExtraStyle, 
GetFullTextExtent, GetHandle, GetHelpText, 
GetHelpTextAtPoint, GetId, GetLabel, GetLayoutDirection, GetPosition, GetPositionTuple, 
GetScreenPosition, GetScreenPositionTuple, GetScreenRect, GetScrollPos, GetScrollRange, 
GetScrollThumb, GetSizer, GetSizeTuple, GetTextExtent, GetThemeEnabled, GetToolTip, 
GetTopLevelParent, GetUpdateClientRect, GetUpdateRegion, GetValidator, GetVirtualSize, 
GetVirtualSizeTuple, GetWindowBorderSize, GetWindowStyle, GetWindowStyleFlag, GetWindowVariant, 
HasCapture, HasFlag, HasMultiplePages, HasScrollbar, HasTransparentBackground, HitTest, 
HitTestXY, InheritAttributes, InheritsBackgroundColour, InitDialog, InvalidateBestSize, 
IsBeingDeleted, IsDoubleBuffered, IsExposed, IsExposedPoint, IsExposedRect, IsRetained, 
IsShownOnScreen, IsTopLevel, Layout, LineDown, LineUp, Lower, MakeModal, Move, 
MoveAfterInTabOrder, MoveBeforeInTabOrder, MoveXY, Navigate, NewControlId, NextControlId, 
PageDown, PageUp, PopEventHandler, PopupMenu, PopupMenuXY, PostCreate, PrepareDC, PrevControlId, 
PushEventHandler, Raise, Refresh, RefreshRect, RegisterHotKey, ReleaseMouse, RemoveChild, 
RemoveEventHandler, ScreenToClient, ScreenToClientXY, ScrollLines, ScrollPages, 
ScrollWindow, SendSizeEvent, SetAutoLayout, SetCaret, SetClientRect, SetClientSize, 
SetClientSizeWH, SetConstraints, SetContainingSizer, SetCursor, SetDimensions, SetDoubleBuffered, 
SetDropTarget, SetEventHandler, SetExtraStyle, SetFocus, SetFocusFromKbd, 
SetHelpText, SetHelpTextForId, SetId, SetInitialSize, SetLabel, 
SetLayoutDirection, SetOwnBackgroundColour, SetOwnFont, SetOwnForegroundColour, SetPosition, 
SetScrollbar, SetScrollPos, SetSizeHintsSz, SetSizer, SetSizerAndFit, SetSizeWH, SetThemeEnabled, 
SetToolTip, SetToolTipString, SetTransparent, SetValidator, SetVirtualSize, SetVirtualSizeHints, 
SetVirtualSizeHintsSz, SetVirtualSizeWH, SetWindowStyle, SetWindowStyleFlag, SetWindowVariant, 
ShouldInheritColours, ToggleWindowStyle, TransferDataFromWindow, TransferDataToWindow, 
UnregisterHotKey, Update, UpdateWindowUI, UseBgCol, Validate, WarpPointer

GetAdjustedBestSize -> Use GetEffectiveMinSize instead.
GetBestFittingSize(*args, **kwargs) -> Use GetEffectiveMinSize instead.
SetBestFittingSize -> Use SetInitialSize
'''

class WindowTestFrame(wx.Frame):
    '''A simple frame class to test wx.Window'''
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Window(self, wx.ID_ANY)
        self.children_ids = (42, 43, 44)
        self.children_names = ('Child One', 'Child Two', 'Child Three' )
        self.children = ( WindowTestChild(self.testControl, id, name=name)
                            for id, name in zip(self.children_ids, self.children_names) )

class WindowTestChild(wx.Frame):
    '''Test out methods relating to children of windows'''
    def __init__(self, parent, id, name):
        wx.Frame.__init__(self, parent=parent, id=id, name=name,
                size=(20,20))


class WindowTest(unittest.TestCase):
    def __init__(self, arg):
        # superclass setup
        super(WindowTest,self).__init__(arg)
        # WindowTest setup
        self.app = wx.PySimpleApp()
        self.anotherFrame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.yetAnotherFrame = wx.Frame(parent=self.anotherFrame, id=wx.ID_ANY)
        self.RED = wx.Color(255,0,0)
        self.GREEN = wx.Color(0,255,0)
        self.BLUE = wx.Color(0,0,255)
        # TODO: delegate to testColour module
        self.COLOUR_TESTS = (
                        (wx.Color(255,0,0), self.RED), 
                        (wx.Color(0,255,0), self.GREEN),
                        (wx.Color(0,0,255), self.BLUE),
                        ('RED', self.RED),
                        ('GREEN', self.GREEN),
                        ('BLUE', self.BLUE),
                        ('#FF0000', self.RED),
                        ('#00FF00', self.GREEN),
                        ('#0000FF', self.BLUE),
                        ((255,0,0), self.RED),
                        ((0,255,0), self.GREEN),
                        ((0,0,255), self.BLUE)
                    )
        # TODO: delegate to testSize module
        self.SIZES = ( 
                        wx.Size(1,1), wx.Size(5,5),
                        wx.Size(10,10), wx.Size(10,1000),
                        wx.Size(100,100), wx.Size(100,500),
                        wx.Size(200,100), wx.Size(500,500),
                        wx.Size(1000,1000), wx.Size(1,1000),
                        wx.Size(1000,1), wx.Size(314, 217),
                        wx.Size(31415, 27182), wx.Size(0,0)
                    )
        # TODO: delegate to testRect module
        # TODO: find out if 32767 is some Windows limit, or a hard one.
        #       Can we get rid of this magic number?
        self.RECTS = (
                        wx.Rect(1,1,1,1), wx.Rect(1,1,1000,1000),
                        wx.Rect(1000,1000,1,1), wx.Rect(1000,1000,1000,1000),
                        wx.Rect(10,200,3000,1), wx.Rect(1,999,111,9999),
                        wx.Rect(32767,32767,32767,32767), wx.Rect()
                    )
        self.SIZE_HINTS = ( # minW, minH, maxW, maxH
                            (0,0,1000,1000), (0,0,1,1),
                            (0,0,0,0), (10,10,100,100),
                            (1000,1000,9999,9999), (1, 10, 100, 1000)
                        )
        # TODO: delegate to testFont module
        self.FONTS = (
                        wx.Font(1, wx.FONTFAMILY_DECORATIVE, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_NORMAL),
                        wx.Font(8, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                        wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_NORMAL),
                        wx.Font(12, wx.FONTFAMILY_MODERN, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_LIGHT),
                        wx.Font(16, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                        wx.Font(18, wx.FONTFAMILY_SCRIPT, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_LIGHT),
                        wx.Font(24, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_NORMAL),
                        wx.Font(32, wx.FONTFAMILY_DECORATIVE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                        wx.Font(36, wx.FONTFAMILY_UNKNOWN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT),
                        wx.Font(48, wx.FONTFAMILY_MODERN, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_BOLD),
                        wx.Font(72, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_NORMAL),
                        wx.Font(96, wx.FONTFAMILY_MAX, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_BOLD),
                        wx.Font(128, wx.FONTFAMILY_SCRIPT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT),
                        wx.Font(256, wx.FONTFAMILY_MAX, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_BOLD)
                    )
    
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.frame = WindowTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        self.children = self.frame.children
        self.children_ids = self.frame.children_ids
        self.children_names = self.frame.children_names

    def tearDown(self):
        self.frame.Destroy()
        
    ##################
    ## Test Methods ##
    ##################
    def testWindowChildren(self):
        '''GetParent
        Tests to make sure the window's children register as such'''
        for child in self.children:
            self.assertEquals(self.testControl, child.GetParent())
    
    # interesting... enable/disable doesn't do it to the children, like 
    # the documentation says. even though they are indeed children.
    def testEnableDisable(self):
        '''Enable, Disable, IsEnabled'''
        self.testControl.Enable(True)
        self.assert_(self.testControl.IsEnabled())
        for child in self.children:
            self.assert_(child.IsEnabled())
        self.testControl.Enable(False)
        self.assert_(not self.testControl.IsEnabled())
        for child in self.children:
            self.assert_(not child.IsEnabled())
        self.testControl.Enable()
        self.assert_(self.testControl.IsEnabled())
        for child in self.children:
            self.assert_(child.IsEnabled())
        self.testControl.Disable()
        self.assert_(not self.testControl.IsEnabled())
        for child in self.children:
            self.assert_(not child.IsEnabled())
        self.testControl.Enable()
        self.assert_(not self.testControl.Enable())
        self.assert_(self.testControl.Disable())
        self.assert_(not self.testControl.Disable())
    
    def testShowHide(self):
        '''Show, Hide, IsShown'''
        self.testControl.Show(True)
        self.assert_(self.testControl.IsShown())
        self.testControl.Show(False)
        self.assert_(not self.testControl.IsShown())
        self.testControl.Show()
        self.assert_(self.testControl.IsShown())
        self.testControl.Hide()
        self.assert_(not self.testControl.IsShown())
        self.testControl.Show()
        self.assert_(not self.testControl.Show())
        self.assert_(self.testControl.Hide())
        self.assert_(not self.testControl.Hide())
    
    def testBackgroundColor(self):
        '''SetBackgroundColour, GetBackgroundColour'''
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetBackgroundColour(test)
            self.assertEquals(actual, self.testControl.GetBackgroundColour())
    
    # see testBackgroundColor
    def testForegroundColor(self):
        '''SetForegroundColour, GetForegroundColour'''
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetForegroundColour(test)
            self.assertEquals(actual, self.testControl.GetForegroundColour())
    
    def testBackgroundStyle(self):
        '''SetBackgroundStyle, GetBackgroundStyle'''
        possible_styles = ( wx.BG_STYLE_COLOUR, wx.BG_STYLE_CUSTOM, wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_CUSTOM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_CUSTOM | wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_CUSTOM | wx.BG_STYLE_SYSTEM
                          )
        for style in possible_styles:
            self.testControl.SetBackgroundStyle(style)
            self.assertEquals(self.testControl.GetBackgroundStyle(), style)
    
    # not strictly a test, there's no way to verify!
    def testCenter(self):
        '''Center, Centre'''
        self.testControl.Center() # default: BOTH
        self.testControl.Center(wx.HORIZONTAL)
        self.testControl.Center(wx.BOTH)
        self.testControl.Center(wx.VERTICAL)
        self.testControl.Centre() # default: BOTH
        self.testControl.Centre(wx.HORIZONTAL)
        self.testControl.Centre(wx.BOTH)
        self.testControl.Centre(wx.VERTICAL)
        # TODO: find out why the following call fails
        # self.testControl.Center(wx.CENTER_ON_SCREEN)
        # Documentation states:
        # "It may also include wx.CENTER_ON_SCREEN flag if you want to
        # center the window on the entire screen and not on its parent window."
    
    def testFreezeThaw(self):
        '''Freeze, Thaw, IsFrozen'''
        self.testControl.Freeze()
        self.assert_(self.testControl.IsFrozen())
        self.testControl.Thaw()
        self.assert_(not self.testControl.IsFrozen())
    
    # This fails because there does not exist a method for equality 
    # checking of wxAcceleratorTables
    # TODO: rewrite to manually check each AcceleratorEntry
    # TODO: test the AcceleratorTable by firing simulated events
    #               and checking responses
    # TODO: implement C++ equality method in wxAcceleratorTable
    def testAcceleratorTable(self):
        '''SetAcceleratorTable, GetAcceleratorTable'''
        # from wx.AcceleratorTable documentation, ids replaced
        #aTable = wx.AcceleratorTable([(wx.ACCEL_ALT,  ord('X'), wx.ID_ANY),
        #                      (wx.ACCEL_CTRL, ord('H'), wx.ID_ANY),
        #                      (wx.ACCEL_CTRL, ord('F'), wx.ID_ANY),
        #                      (wx.ACCEL_NORMAL, wx.WXK_F3, wx.ID_ANY)
        #                      ])
        #self.testControl.SetAcceleratorTable(aTable)
        #self.assertEquals(aTable, self.testControl.GetAcceleratorTable())
        pass
    
    def testParent(self):
        '''GetParent, Reparent'''
        parent = self.testControl.GetParent()
        self.assertEquals(parent, self.frame)
        self.assert_(not self.testControl.Reparent(parent))
        if self.testControl.Reparent(self.anotherFrame):
            newParent = self.testControl.GetParent()
            self.assertEquals(newParent, self.anotherFrame)
        else:
            self.assert_(False)
    
    def testGrandParent(self):
        '''GetGrandParent, Reparent'''
        self.assertEquals(None, self.testControl.GetGrandParent())
        self.testControl.Reparent(self.yetAnotherFrame)
        self.assertEquals(self.anotherFrame, self.testControl.GetGrandParent())
        
    def testSize(self):
        '''SetSize, GetSize'''
        for size in self.SIZES:
            self.testControl.SetSize(size)
            self.assertEquals(size, self.testControl.GetSize())
    
    def testMinSize(self):
        '''SetMinSize, GetMinSize'''
        for min_size in self.SIZES:
            self.testControl.SetMinSize(min_size)
            self.assertEquals(min_size, self.testControl.GetMinSize())
            
    def testMaxSize(self):
        '''SetMaxSize, GetMaxSize'''
        for max_size in self.SIZES:
            self.testControl.SetMaxSize(max_size)
            self.assertEquals(max_size, self.testControl.GetMaxSize())
    
    def testSizeHints(self):
        '''SetSizeHints, GetMinWidth, GetMinHeight, GetMaxWidth, GetMaxHeight'''
        for minW, minH, maxW, maxH in self.SIZE_HINTS:
            self.testControl.SetSizeHints(minW, minH, maxW, maxH)
            self.assertEquals(minW, self.testControl.GetMinWidth())
            self.assertEquals(minH, self.testControl.GetMinHeight())
            self.assertEquals(maxW, self.testControl.GetMaxWidth())
            self.assertEquals(maxH, self.testControl.GetMaxHeight())
        self.assertRaises(wx.PyAssertionError, self.testControl.SetSizeHints, 100,100,10,10)
    
    def testRect(self):
        '''SetRect, GetRect'''
        for rect in self.RECTS:
            self.testControl.SetRect(rect)
            self.assertEquals(rect, self.testControl.GetRect())
    
    def testName(self):
        '''SetName, GetName'''
        self.testControl.SetName("The Name of the Panel")
        self.assertEquals("The Name of the Panel", self.testControl.GetName())
    
    def testFont(self):
        '''SetFont, GetFont'''
        for font in self.FONTS:
            self.testControl.SetFont(font)
            self.assertEquals(font, self.testControl.GetFont())
    
    def testFindWindow(self):
        '''FindWindowById, FindWindowByName'''
        for child, id, name in zip(self.children, self.children_ids, self.children_names):
            self.assertEquals(child, self.testControl.FindWindowById(id))
            self.assertEquals(child, self.testControl.FindWindowByName(name))
    

def suite():
    suite = unittest.makeSuite(WindowTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
