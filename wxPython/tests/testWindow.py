import unittest
import wx

import testColor
import testRect
import testFont
import testPoint
import testSize

"""
This file contains classes and methods for unit testing the API of wx.Window.

WindowTest is meant to be the base class of all test cases for classes derived
from wx.Window.  For the tests to run properly, derived classes must make sure
to call the constructor's superclass.  Additionally, they must create a few class
properties from within the setUp method.  Generally these are just symbols pointing
to their respective data within the testframe.
These properties include:
    testControl - an alias of frame.testControl
    children - a sequence of frames whose parent is the testControl
    children_ids - IDs corresponding to each child
    children_names - names corresponding to each child

To find out where a particular method is tested, search for the name of that
method.  Each test contains, as a docstring, the names of the methods tested
within them.  Additionally, below is a list of methods needing tests.

Methods yet to test:
__init__, AcceptsFocus, AcceptsFocusFromKeyboard, AddChild, AdjustForLayoutDirection,
AssociateHandle, CacheBestSize, CanSetTransparent, CaptureMouse, CenterOnParent, CentreOnParent, 
ClearBackground, ClientToScreen, ClientToScreenXY, Close, ConvertDialogPointToPixels, 
ConvertDialogSizeToPixels, ConvertPixelPointToDialog, ConvertPixelSizeToDialog, 
Create, Destroy, DestroyChildren, DissociateHandle, DLG_PNT, DLG_SZE, DragAcceptFiles, 
FindFocus, Fit, FitInside, GetAutoLayout, GetBestSize, 
GetBestSizeTuple, GetBestVirtualSize, GetBorder, GetCapture, GetCaret, GetCharHeight, 
GetCharWidth, GetChildren, GetClassDefaultAttributes, GetClientAreaOrigin, GetClientRect, 
GetClientSize, GetClientSizeTuple, GetConstraints, GetContainingSizer, GetCursor, 
GetDefaultAttributes, GetDropTarget, GetEffectiveMinSize, GetEventHandler, GetExtraStyle, 
GetFullTextExtent, GetHandle, GetHelpText, GetHelpTextAtPoint, GetLabel, GetLayoutDirection,
GetScreenPosition, GetScreenPositionTuple, GetScreenRect, GetScrollPos, GetScrollRange, 
GetScrollThumb, GetSizer, GetSizeTuple, GetTextExtent, GetThemeEnabled, GetToolTip, 
GetTopLevelParent, GetUpdateClientRect, GetUpdateRegion, GetValidator, GetVirtualSize, 
GetVirtualSizeTuple, GetWindowBorderSize, GetWindowStyle, GetWindowStyleFlag, GetWindowVariant, 
HasCapture, HasFlag, HasMultiplePages, HasScrollbar, HasTransparentBackground, HitTest, 
HitTestXY, InheritAttributes, InheritsBackgroundColour, InitDialog, InvalidateBestSize, 
IsBeingDeleted, IsDoubleBuffered, IsExposed, IsExposedPoint, IsExposedRect, IsRetained, 
IsShownOnScreen, IsTopLevel, Layout, LineDown, LineUp, Lower, MakeModal, 
MoveAfterInTabOrder, MoveBeforeInTabOrder, Navigate, NewControlId, NextControlId, 
PageDown, PageUp, PopEventHandler, PopupMenu, PopupMenuXY, PostCreate, PrepareDC, PrevControlId, 
PushEventHandler, Raise, Refresh, RefreshRect, RegisterHotKey, ReleaseMouse, RemoveChild, 
RemoveEventHandler, ScreenToClient, ScreenToClientXY, ScrollLines, ScrollPages, 
ScrollWindow, SendSizeEvent, SetAutoLayout, SetCaret, SetClientRect, SetClientSize, 
SetClientSizeWH, SetConstraints, SetContainingSizer, SetCursor, SetDimensions, SetDoubleBuffered, 
SetDropTarget, SetEventHandler, SetExtraStyle, SetFocus, SetFocusFromKbd, 
SetHelpText, SetHelpTextForId, SetInitialSize, SetLabel, SetLayoutDirection, 
SetScrollbar, SetScrollPos, SetSizeHintsSz, SetSizer, SetSizerAndFit, SetSizeWH, SetThemeEnabled, 
SetToolTip, SetToolTipString, SetTransparent, SetValidator, SetVirtualSize, SetVirtualSizeHints, 
SetVirtualSizeHintsSz, SetVirtualSizeWH, SetWindowStyle, SetWindowStyleFlag, SetWindowVariant, 
ShouldInheritColours, ToggleWindowStyle, TransferDataFromWindow, TransferDataToWindow, 
UnregisterHotKey, Update, UpdateWindowUI, UseBgCol, Validate, WarpPointer

GetAdjustedBestSize -> Use GetEffectiveMinSize instead.
GetBestFittingSize(*args, **kwargs) -> Use GetEffectiveMinSize instead.
SetBestFittingSize -> Use SetInitialSize
"""

class WindowTestFrame(wx.Frame):
    """A simple frame class to test wx.Window"""
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Window(self, wx.ID_ANY)
        self.children_ids = (42, 43, 44)
        self.children_names = ('Child One', 'Child Two', 'Child Three' )
        self.children = ( wx.Frame(self.testControl, id=id, name=name)
                            for id, name in zip(self.children_ids, self.children_names) )


class WindowTest(unittest.TestCase):
    def __init__(self, arg):
        # superclass setup
        super(WindowTest,self).__init__(arg)
        # WindowTest setup
        self.app = wx.PySimpleApp()
        self.anotherFrame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.yetAnotherFrame = wx.Frame(parent=self.anotherFrame, id=wx.ID_ANY)
        self.COLOUR_TESTS = testColor.getColourData()
        self.SIZES = testSize.getValidSizeData()
        self.RECTS = testRect.getValidRectData()
        self.SIZE_HINTS = testSize.getValidSizeHints()
        self.INVALID_SIZE_HINTS = testSize.getInvalidSizeHints()
        self.FONTS = testFont.getFontData()
        self.POINTS = testPoint.getValidPointData()
    
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = WindowTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        self.children = self.frame.children
        self.children_ids = self.frame.children_ids
        self.children_names = self.frame.children_names

    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
        
    ##################
    ## Test Methods ##
    ##################
    def testWindowChildren(self):
        """GetParent
        Tests to make sure the window's children register as such"""
        for child in self.children:
            self.assertEquals(self.testControl, child.GetParent())
    
    def testEnableDisable(self):
        """Enable, Disable, IsEnabled"""
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
        """Show, Hide, IsShown"""
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
        """SetBackgroundColour, GetBackgroundColour"""
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetBackgroundColour(test)
            self.assertEquals(actual, self.testControl.GetBackgroundColour())
    
    # what is the difference between SetOwnBackgroundColour and SetBackgroundColour?
    # the docs don't say anything about SetOwnBackgroundColour
    def testOwnBackgroundColor(self):
        """SetOwnBackgroundColour"""
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetOwnBackgroundColour(test)
            self.assertEquals(actual, self.testControl.GetBackgroundColour())
    
    def testForegroundColor(self):
        """SetForegroundColour, GetForegroundColour"""
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetForegroundColour(test)
            self.assertEquals(actual, self.testControl.GetForegroundColour())
    
    def testOwnForegroundColor(self):
        """SetOwnForegroundColour"""
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetOwnForegroundColour(test)
            self.assertEquals(actual, self.testControl.GetForegroundColour())
    
    def testBackgroundStyle(self):
        """SetBackgroundStyle, GetBackgroundStyle"""
        possible_styles = ( wx.BG_STYLE_COLOUR, wx.BG_STYLE_CUSTOM, wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_CUSTOM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_CUSTOM | wx.BG_STYLE_SYSTEM,
                            wx.BG_STYLE_COLOUR | wx.BG_STYLE_CUSTOM | wx.BG_STYLE_SYSTEM
                          )
        for style in possible_styles:
            self.testControl.SetBackgroundStyle(style)
            self.assertEquals(self.testControl.GetBackgroundStyle(), style)
    
    # TODO: refactor this method
    # not strictly a test, there's no way to verify!
    def testCenter(self):
        """Center, Centre"""
        self.testControl.Center() # default: BOTH
        self.testControl.Center(wx.HORIZONTAL)
        self.testControl.Center(wx.BOTH)
        self.testControl.Center(wx.VERTICAL)
        self.testControl.Centre() # default: BOTH
        self.testControl.Centre(wx.HORIZONTAL)
        self.testControl.Centre(wx.BOTH)
        self.testControl.Centre(wx.VERTICAL)
        # must fail if window has no parent
        self.assertRaises(wx.PyAssertionError, self.testControl.Center, wx.CENTER_ON_SCREEN)
        # This, however, functions properly (it has a parent)
        for child in self.children:
            child.Center(wx.CENTER_ON_SCREEN)
    
    def testFreezeThaw(self):
        """Freeze, Thaw, IsFrozen"""
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
        """SetAcceleratorTable, GetAcceleratorTable"""
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
        """GetParent, Reparent"""
        parent = self.testControl.GetParent()
        self.assertEquals(parent, self.frame)
        self.assert_(not self.testControl.Reparent(parent))
        if self.testControl.Reparent(self.anotherFrame):
            newParent = self.testControl.GetParent()
            self.assertEquals(newParent, self.anotherFrame)
        else:
            self.assert_(False)
    
    def testGrandParent(self):
        """GetGrandParent, Reparent"""
        self.assertEquals(None, self.testControl.GetGrandParent())
        self.testControl.Reparent(self.yetAnotherFrame)
        self.assertEquals(self.anotherFrame, self.testControl.GetGrandParent())
        
    def testSize(self):
        """SetSize, GetSize"""
        for size in self.SIZES:
            self.testControl.SetSize(size)
            self.assertEquals(size, self.testControl.GetSize())
    
    def testMinSize(self):
        """SetMinSize, GetMinSize"""
        for min_size in self.SIZES:
            self.testControl.SetMinSize(min_size)
            self.assertEquals(min_size, self.testControl.GetMinSize())
            
    def testMaxSize(self):
        """SetMaxSize, GetMaxSize"""
        for max_size in self.SIZES:
            self.testControl.SetMaxSize(max_size)
            self.assertEquals(max_size, self.testControl.GetMaxSize())
    
    def testSizeHints(self):
        """SetSizeHints, GetMinWidth, GetMinHeight, GetMaxWidth, GetMaxHeight"""
        for minW, minH, maxW, maxH in self.SIZE_HINTS:
            self.testControl.SetSizeHints(minW, minH, maxW, maxH)
            self.assertEquals(minW, self.testControl.GetMinWidth())
            self.assertEquals(minH, self.testControl.GetMinHeight())
            self.assertEquals(maxW, self.testControl.GetMaxWidth())
            self.assertEquals(maxH, self.testControl.GetMaxHeight())
        for invalid_hint in self.INVALID_SIZE_HINTS:
            self.assertRaises(wx.PyAssertionError, self.testControl.SetSizeHints, *invalid_hint)
    
    def testRect(self):
        """SetRect, GetRect"""
        for rect in self.RECTS:
            self.testControl.SetRect(rect)
            self.assertEquals(rect, self.testControl.GetRect())
    
    def testName(self):
        """SetName, GetName"""
        self.testControl.SetName("The Name of the Panel")
        self.assertEquals("The Name of the Panel", self.testControl.GetName())
    
    def testFont(self):
        """SetFont, GetFont"""
        for font in self.FONTS:
            self.testControl.SetFont(font)
            self.assertEquals(font, self.testControl.GetFont())
    
    def testOwnFont(self):
        """SetOwnFont"""
        for font in self.FONTS:
            self.testControl.SetOwnFont(font)
            self.assertEquals(font, self.testControl.GetFont())
    
    def testFindWindow(self):
        """FindWindowById, FindWindowByName"""
        for child, id, name in zip(self.children, self.children_ids, self.children_names):
            self.assertEquals(child, self.testControl.FindWindowById(id))
            self.assertEquals(child, self.testControl.FindWindowByName(name))
    
    def testId(self):
        """SetId, GetId"""
        for id in (42, 314, 2718):
            self.testControl.SetId(id)
            self.assertEquals(id, self.testControl.GetId())
    
    def testPosition(self):
        """SetPosition, GetPosition"""
        for point in self.POINTS:
            self.testControl.SetPosition(point)
            self.assertEquals(point, self.testControl.GetPosition())
        # TODO:
        # setting point of (-1,-1) does not affect position
        # is this expected behavior??
        unchanged = self.testControl.GetPosition()
        self.testControl.SetPosition(wx.Point(-1,-1))
        self.assertEquals(unchanged, self.testControl.GetPosition())
    
    def testMove(self):
        """Move, MoveXY, GetPositionTuple"""
        for point in self.POINTS:
            self.testControl.Move(point)
            self.assertEquals(point.Get(), self.testControl.GetPositionTuple())
        # TODO: what is expected behavior? see 'testPosition' above.
        unchanged = self.testControl.GetPositionTuple()
        self.testControl.Move(wx.Point(-1,-1))
        self.assertEquals(unchanged,self.testControl.GetPositionTuple())
        for point in self.POINTS:
            x,y = point.Get()
            self.testControl.MoveXY(x,y)
            self.assertEquals((x,y), self.testControl.GetPositionTuple())

def suite():
    suite = unittest.makeSuite(WindowTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
