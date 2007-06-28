import unittest
import wx

import wxtest
import testColour
import testRect
import testFont
import testPoint
import testSize

"""
This file contains classes and methods for unit testing the API of wx.Window.

WindowTest is meant to be the base class of all test cases for classes derived
from wx.Window.  For the tests to run properly, derived classes must make sure
to call the constructor's superclass if they override it.  Additionally, they must create a 
few class properties from within the setUp method, and may create a few more.

Required properties:
    frame - just a generic frame, to use as a parent for Windows that need one
    testControl - an alias of frame.testControl

Optional properties:
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
Create, DestroyChildren, DissociateHandle, DLG_PNT, DLG_SZE, DragAcceptFiles, 
FindFocus, Fit, FitInside, GetAcceleratorTable, GetAutoLayout, GetBestSize, 
GetBestSizeTuple, GetBestVirtualSize, GetBorder, GetCapture, GetCaret, GetCharHeight, 
GetCharWidth, GetClassDefaultAttributes, GetClientAreaOrigin,
GetConstraints, GetContainingSizer, GetCursor, 
GetDefaultAttributes, GetDropTarget, GetEffectiveMinSize, GetEventHandler, GetExtraStyle, 
GetFullTextExtent, GetHandle, GetHelpTextAtPoint, GetLayoutDirection,
GetScreenPosition, GetScreenPositionTuple, GetScreenRect, GetScrollPos, GetScrollRange, 
GetScrollThumb, GetTextExtent, GetThemeEnabled,
GetUpdateClientRect, GetUpdateRegion, GetValidator, GetWindowBorderSize, 
GetWindowStyle, GetWindowStyleFlag, GetWindowVariant,
HasCapture, HasFlag, HasMultiplePages, HasScrollbar, HasTransparentBackground, HitTest, 
HitTestXY, InheritAttributes, InheritsBackgroundColour, InitDialog, InvalidateBestSize, 
IsDoubleBuffered, IsExposed, IsExposedPoint, IsExposedRect, IsRetained, 
Layout, LineDown, LineUp, Lower, MakeModal, 
MoveAfterInTabOrder, MoveBeforeInTabOrder, Navigate, NewControlId, NextControlId, 
PageDown, PageUp, PopEventHandler, PopupMenu, PopupMenuXY, PostCreate, PrepareDC, PrevControlId, 
PushEventHandler, Raise, Refresh, RefreshRect, RegisterHotKey, ReleaseMouse, RemoveChild, 
RemoveEventHandler, ScreenToClient, ScreenToClientXY, ScrollLines, ScrollPages, 
ScrollWindow, SendSizeEvent, SetAcceleratorTable, SetAutoLayout, SetCaret, 
SetConstraints, SetContainingSizer, SetCursor, SetDimensions, SetDoubleBuffered,
SetDropTarget, SetEventHandler, SetExtraStyle, SetFocus, SetFocusFromKbd, 
SetHelpTextForId, SetInitialSize, SetLayoutDirection, 
SetScrollbar, SetScrollPos, SetSizeHintsSz, SetSizerAndFit, SetThemeEnabled, 
SetTransparent, SetValidator, SetVirtualSizeHints, SetVirtualSizeHintsSz,
SetWindowStyle, SetWindowStyleFlag, SetWindowVariant,
ShouldInheritColours, ToggleWindowStyle, TransferDataFromWindow, TransferDataToWindow, 
UnregisterHotKey, Update, UpdateWindowUI, UseBgCol, Validate, WarpPointer

GetAdjustedBestSize -> Use GetEffectiveMinSize instead.
GetBestFittingSize(*args, **kwargs) -> Use GetEffectiveMinSize instead.
SetBestFittingSize -> Use SetInitialSize
"""

class WindowTest(unittest.TestCase):
    def __init__(self, arg):
        # superclass setup
        super(WindowTest,self).__init__(arg)
        # WindowTest setup
        self.app = wx.PySimpleApp()
        # make derived classes less annoying
        self.children = []
        self.children_ids = []
        self.children_names = []

    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Window(parent=self.frame, id=wx.ID_ANY)
        self.children_ids = (42, 43, 44)
        self.children_names = ('Child One', 'Child Two', 'Child Three')
        self.children = ( wx.Frame(self.testControl, id=id, name=name)
                            for id, name in zip(self.children_ids, self.children_names) )

    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
    
    def testBackgroundColor(self):
        """SetBackgroundColour, GetBackgroundColour"""
        for test, actual in testColour.getColourEquivalents():
            self.assert_(self.testControl.SetBackgroundColour(test))
                        # return True when background colour changed
            self.assertEquals(actual, self.testControl.GetBackgroundColour())
            self.assert_(not self.testControl.SetBackgroundColour(test))
            
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
        # This test is finicky.
        if wxtest.PlatformIsWindows():
            self.assertRaises(wx.PyAssertionError, self.testControl.Center, wx.CENTER_ON_SCREEN)
        # This, however, functions properly (it has a parent)
        for child in self.children:
            child.Center(wx.CENTER_ON_SCREEN)

    # Looks like this one fails on Windows
    # (first two attributes of the wx.Rect are 0 in every instance)
    def testClientRect(self):
        """SetClientRect, GetClientRect"""
        for rect in testRect.getValidRectData():
            self.testControl.SetClientRect(rect)
            self.assertEquals(rect, self.testControl.GetClientRect())
    
    def testClientSize(self):
        """SetClientSize, GetClientSize"""
        for size in testSize.getValidSizeData():
            self.testControl.SetClientSize(size)
            self.assertEquals(size, self.testControl.GetClientSize())
    
    def testClientSizeWH(self):
        """SetClientSizeWH, GetClientSizeTuple"""
        for w,h in testSize.getValidSizeData():
            self.testControl.SetClientSizeWH(w,h)
            self.assertEquals((w,h), self.testControl.GetClientSizeTuple())
    
    def testDestroy(self):
        """Destroy"""
        self.assert_(self.testControl.Destroy())
        flag = False
        try:
            # for some reason, an assertRaises statement here will also 
            # raise an wx.PyDeadObjectError
            self.testControl.Destroy()
        except wx.PyDeadObjectError:
            flag = True
        finally:
            self.assert_(flag)
        # put back a dummy object so cleanup can happen
        self.testControl = wx.Window(self.frame)
    
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
        
    def testFindWindow(self):
        """FindWindowById, FindWindowByName"""
        for child, id, name in zip(self.children, self.children_ids, self.children_names):
            self.assertEquals(child, self.testControl.FindWindowById(id))
            self.assertEquals(child, self.testControl.FindWindowByName(name))
            
    def testFont(self):
        """SetFont, GetFont"""
        for font in testFont.getFontData():
            self.testControl.SetFont(font)
            self.assertEquals(font, self.testControl.GetFont())
            
    def testForegroundColor(self):
        """SetForegroundColour, GetForegroundColour"""
        for test, actual in testColour.getColourEquivalents():
            self.assert_(self.testControl.SetForegroundColour(test))
                            # return True when background colour changed
            self.assertEquals(actual, self.testControl.GetForegroundColour())
            self.assert_(not self.testControl.SetForegroundColour(test))
            
    def testFreezeThaw(self):
        """Freeze, Thaw, IsFrozen"""
        self.testControl.Freeze()
        self.assert_(self.testControl.IsFrozen())
        self.testControl.Thaw()
        self.assert_(not self.testControl.IsFrozen())
    
    def testGetChildren(self):
        """GetChildren"""
        # segfaults on Ubuntu for unknown reason
        if wxtest.PlatformIsNotLinux():
            a = wx.Window(self.testControl)
            b = wx.Window(self.testControl)
            c = wx.Window(self.testControl)
            for child in (a,b,c):
                self.assert_(child in self.testControl.GetChildren())
    
    def testGrandParent(self):
        """GetGrandParent, Reparent"""
        self.assertEquals(None, self.testControl.GetGrandParent())
        grandparent = wx.Frame(self.frame)
        parent = wx.Frame(grandparent)
        self.testControl.Reparent(parent)
        self.assertEquals(grandparent, self.testControl.GetGrandParent())
    
    # NOTE: this is peculiar
    def testHelpText(self):
        """SetHelpText, GetHelpText"""
        txt = "Here is some help text!"
        self.testControl.SetHelpText(txt)
        #self.assertEquals(txt, self.testControl.GetHelpText())
    
    def testId(self):
        """SetId, GetId"""
        for id in (42, 314, 2718):
            self.testControl.SetId(id)
            self.assertEquals(id, self.testControl.GetId())
    
    # At least one of these came out valid on Ubuntu.
    # TODO: isolate the culprit (and make the whole thing more robust)
    def testInvalidSizeHints(self):
        for invalid_hint in testSize.getInvalidSizeHints():
            self.assertRaises(wx.PyAssertionError, self.testControl.SetSizeHints, *invalid_hint)
    
    def testIsBeingDeleted(self):
        """IsBeingDeleted
        TODO: find a way to test this when it will return True
        """
        self.assert_(not self.testControl.IsBeingDeleted())
    
    def testLabel(self):
        """SetLabel, GetLabel"""
        one = "here is one label"
        two = "and here there is another"
        self.testControl.SetLabel(one)
        self.assertEquals(one, self.testControl.GetLabel())
        self.testControl.SetLabel(two)
        self.assertEquals(two, self.testControl.GetLabel())
        self.assertNotEquals(one, self.testControl.GetLabel())

    def testMaxSize(self):
        """SetMaxSize, GetMaxSize"""
        for max_size in testSize.getValidSizeData():
            self.testControl.SetMaxSize(max_size)
            self.assertEquals(max_size, self.testControl.GetMaxSize())
            
    def testMinSize(self):
        """SetMinSize, GetMinSize"""
        for min_size in testSize.getValidSizeData():
            self.testControl.SetMinSize(min_size)
            self.assertEquals(min_size, self.testControl.GetMinSize())
            
    def testMove(self):
        """Move, MoveXY, GetPositionTuple"""
        for point in testPoint.getValidPointData():
            self.testControl.Move(point)
            self.assertEquals(point.Get(), self.testControl.GetPositionTuple())
        # TODO: what is expected behavior? see 'testPosition' above.
        unchanged = self.testControl.GetPositionTuple()
        self.testControl.Move(wx.Point(-1,-1))
        self.assertEquals(unchanged,self.testControl.GetPositionTuple())
        for point in testPoint.getValidPointData():
            x,y = point.Get()
            self.testControl.MoveXY(x,y)
            self.assertEquals((x,y), self.testControl.GetPositionTuple())
    
    def testName(self):
        """SetName, GetName"""
        self.testControl.SetName("The Name of the Panel")
        self.assertEquals("The Name of the Panel", self.testControl.GetName())
        
    # what is the difference between SetOwnBackgroundColour and SetBackgroundColour?
    # the docs don't say anything about SetOwnBackgroundColour
    def testOwnBackgroundColor(self):
        """SetOwnBackgroundColour"""
        for test, actual in testColour.getColourEquivalents():
            self.testControl.SetOwnBackgroundColour(test)
            self.assertEquals(actual, self.testControl.GetBackgroundColour())
    
    def testOwnFont(self):
        """SetOwnFont"""
        for font in testFont.getFontData():
            self.testControl.SetOwnFont(font)
            self.assertEquals(font, self.testControl.GetFont())
            
    def testOwnForegroundColor(self):
        """SetOwnForegroundColour"""
        for test, actual in testColour.getColourEquivalents():
            self.testControl.SetOwnForegroundColour(test)
            self.assertEquals(actual, self.testControl.GetForegroundColour())
    
    def testParent(self):
        """GetParent, Reparent"""
        parent = self.testControl.GetParent()
        self.assertEquals(parent, self.frame)
        self.assert_(not self.testControl.Reparent(parent))
        anotherFrame = wx.Frame(self.frame)
        if self.testControl.Reparent(anotherFrame):
            newParent = self.testControl.GetParent()
            self.assertEquals(newParent, anotherFrame)
        else:
            self.assert_(False)
    
    def testPosition(self):
        """SetPosition, GetPosition"""
        for point in testPoint.getValidPointData():
            self.testControl.SetPosition(point)
            self.assertEquals(point, self.testControl.GetPosition())
        # TODO:
        # setting point of (-1,-1) does not affect position
        # is this expected behavior??
        unchanged = self.testControl.GetPosition()
        self.testControl.SetPosition(wx.Point(-1,-1))
        self.assertEquals(unchanged, self.testControl.GetPosition())
    
    def testRect(self):
        """SetRect, GetRect"""
        for rect in testRect.getValidRectData():
            self.testControl.SetRect(rect)
            self.assertEquals(rect, self.testControl.GetRect())
        
    def testShow(self):
        """Show, IsShown"""
        self.testControl.Show(True)
        self.assert_(self.testControl.IsShown())
        self.assert_(self.testControl.Show(False))
        self.assert_(not self.testControl.IsShown())
        self.assert_(self.testControl.Show())
        self.assert_(self.testControl.IsShown())
        self.assert_(not self.testControl.Show())
    
    def testHide(self):
        """Hide"""
        self.testControl.Show()
        self.assert_(self.testControl.Hide())
        self.assert_(not self.testControl.IsShown())
        self.assert_(not self.testControl.Hide())
        
    def testShownOnScreen(self):
        """IsShownOnScreen"""
        self.testControl.Show()
        self.assert_(not self.testControl.IsShownOnScreen())
        self.frame.Show()
        self.assert_(self.testControl.IsShownOnScreen())
        self.frame.Hide()
        self.assert_(not self.testControl.IsShownOnScreen())
    
    def testSize(self):
        """SetSize, GetSize"""
        for size in testSize.getValidSizeData():
            self.testControl.SetSize(size)
            self.assertEquals(size, self.testControl.GetSize())
    
    def testSizeWH(self):
        """SetSizeWH, GetSizeTuple"""
        for w,h in testSize.getValidSizeData():
            self.testControl.SetSizeWH(w,h)
            self.assertEquals((w,h), self.testControl.GetSizeTuple())
    
    def testSizeHints(self):
        """SetSizeHints, GetMinWidth, GetMinHeight, GetMaxWidth, GetMaxHeight"""
        for minW, minH, maxW, maxH in testSize.getValidSizeHints():
            self.testControl.SetSizeHints(minW, minH, maxW, maxH)
            self.assertEquals(minW, self.testControl.GetMinWidth())
            self.assertEquals(minH, self.testControl.GetMinHeight())
            self.assertEquals(maxW, self.testControl.GetMaxWidth())
            self.assertEquals(maxH, self.testControl.GetMaxHeight())
    
    def testSizer(self):
        """SetSizer, GetSizer"""
        # TODO: test for other functionality provided in SetSizer
        sz = wx.BoxSizer()
        self.testControl.SetSizer(sz)
        self.assertEquals(sz, self.testControl.GetSizer())
    
    def testToolTip(self):
        """SetToolTip, GetToolTip"""
        # wx.ToolTips don't have an equality test, so do it manually
        tip = wx.ToolTip('Here is a tip!')
        self.testControl.SetToolTip(tip)
        tip2 = self.testControl.GetToolTip()
        self.assertEquals(self.testControl, tip2.GetWindow())
        self.assertEquals(tip.GetTip(), tip2.GetTip())
    
    def testToolTipString(self):
        """SetToolTipString"""
        for txt in ('one','two','three'):
            self.testControl.SetToolTipString(txt)
            self.assertEquals(txt, self.testControl.GetToolTip().GetTip())
    
    def testTopLevel(self):
        """IsTopLevel"""
        self.assert_(not self.testControl.IsTopLevel())
    
    def testTopLevelParent(self):
        """GetTopLevelParent"""
        parent = wx.Frame(None)
        one = wx.Window(parent)
        two = wx.Window(one)
        three = wx.Window(two)
        four = wx.Window(three)
        self.assertEquals(parent, four.GetTopLevelParent())
    
    def testVirtualSize(self):
        """SetVirtualSize, GetVirtualSize"""
        for size in testSize.getValidSizeData():
            self.testControl.SetVirtualSize(size)
            self.assertEquals(size, self.testControl.GetVirtualSize())
    
    def testVirtualSizeWH(self):
        """SetVirtualSizeWH, GetVirtualSizeTuple"""
        for w,h in testSize.getValidSizeData():
            self.testControl.SetVirtualSizeWH(w,h)
            self.assertEquals((w,h),self.testControl.GetVirtualSizeTuple())
        
    def testWindowChildren(self):
        """GetParent
        Tests to make sure the window's children register as such"""
        for child in self.children:
            self.assertEquals(self.testControl, child.GetParent())


def suite():
    suite = unittest.makeSuite(WindowTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
