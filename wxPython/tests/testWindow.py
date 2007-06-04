import unittest
import wx

'''
This file contains classes and methods for unit testing the API of wx.Window.

To find out where a particular method is tested, search for the name of that
method.  Each test contains, as a docstring, the names of the methods tested
within them.  Additionally, the following methods have yet to have tests
written for them:

AcceptsFocus
AcceptsFocusFromKeyboard
AddChild
AdjustForLayoutDirection
AssociateHandle
CacheBestSize
CanSetTransparent
CaptureMouse
CenterOnParent
CentreOnParent
ClearBackground
ClientToScreen
ClientToScreenXY
Close
ConvertDialogPointToPixels
ConvertDialogSizeToPixels
ConvertPixelPointToDialog
ConvertPixelSizeToDialog
Create
Destroy
DestroyChildren
DissociateHandle
DLG_PNT
DLG_SZE
DragAcceptFiles
FindFocus
FindWindowById
FindWindowByName
Fit
FitInside
GetAcceleratorTable
GetAdjustedBestSize -> Use GetEffectiveMinSize instead.
GetAutoLayout
GetBestFittingSize(*args, **kwargs) -> Use GetEffectiveMinSize instead.
GetBestSize
GetBestSizeTuple
GetBestVirtualSize
GetBorder
GetCapture
GetCaret
GetCharHeight
GetCharWidth
GetChildren
GetClassDefaultAttributes
GetClientAreaOrigin
GetClientRect
GetClientSize
GetClientSizeTuple
GetConstraints
GetContainingSizer
GetCursor
GetDefaultAttributes
GetDropTarget
GetEffectiveMinSize
GetEventHandler
GetExtraStyle
GetFont
GetForegroundColour
GetFullTextExtent
GetGrandParent
GetHandle
GetHelpText
GetHelpTextAtPoint
GetId
GetLabel
GetLayoutDirection
GetMaxHeight
GetMaxSize
GetMaxWidth
GetMinHeight
GetMinSize
GetMinWidth
GetName
GetParent
GetPosition
GetPositionTuple
GetRect
GetScreenPosition
GetScreenPositionTuple
GetScreenRect
GetScrollPos
GetScrollRange
GetScrollThumb
GetSize
GetSizer
GetSizeTuple
GetTextExtent
GetThemeEnabled
GetToolTip
GetTopLevelParent
GetUpdateClientRect
GetUpdateRegion
GetValidator
GetVirtualSize
GetVirtualSizeTuple
GetWindowBorderSize
GetWindowStyle
GetWindowStyleFlag
GetWindowVariant
HasCapture
HasFlag
HasMultiplePages
HasScrollbar
HasTransparentBackground
Hide
HitTest
HitTestXY
InheritAttributes
InheritsBackgroundColour
InitDialog
InvalidateBestSize
IsBeingDeleted
IsDoubleBuffered
IsExposed
IsExposedPoint
IsExposedRect
IsRetained
IsShown
IsShownOnScreen
IsTopLevel
Layout
LineDown
LineUp
Lower
MakeModal
Move
MoveAfterInTabOrder
MoveBeforeInTabOrder
MoveXY
Navigate
NewControlId
NextControlId
PageDown
PageUp
PopEventHandler
PopupMenu
PopupMenuXY
PostCreate
PrepareDC
PrevControlId
PushEventHandler
Raise
Refresh
RefreshRect
RegisterHotKey
ReleaseMouse
RemoveChild
RemoveEventHandler
Reparent
ScreenToClient
ScreenToClientXY
ScrollLines
ScrollPages
ScrollWindow
SendSizeEvent
SetAcceleratorTable
SetAutoLayout
SetBestFittingSize -> Use SetInitialSize
SetCaret
SetClientRect
SetClientSize
SetClientSizeWH
SetConstraints
SetContainingSizer
SetCursor
SetDimensions
SetDoubleBuffered
SetDropTarget
SetEventHandler
SetExtraStyle
SetFocus
SetFocusFromKbd
SetFont
SetForegroundColour
SetHelpText
SetHelpTextForId
SetId
SetInitialSize
SetLabel
SetLayoutDirection
SetMaxSize
SetMinSize
SetName
SetOwnBackgroundColour
SetOwnFont
SetOwnForegroundColour
SetPosition
SetRect
SetScrollbar
SetScrollPos
SetSize
SetSizeHints
SetSizeHintsSz
SetSizer
SetSizerAndFit
SetSizeWH
SetThemeEnabled
SetToolTip
SetToolTipString
SetTransparent
SetValidator
SetVirtualSize
SetVirtualSizeHints
SetVirtualSizeHintsSz
SetVirtualSizeWH
SetWindowStyle
SetWindowStyleFlag
SetWindowVariant
ShouldInheritColours
Show
ToggleWindowStyle
TransferDataFromWindow
TransferDataToWindow
UnregisterHotKey
Update
UpdateWindowUI
UseBgCol
Validate
WarpPointer
'''

class WindowTestFrame(wx.Frame):
    '''A simple frame class to test wx.Window'''
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Window(self, wx.ID_ANY)


class WindowTest(unittest.TestCase):
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = WindowTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        # set up common code
        self.RED = wx.Color(255,0,0)
        self.GREEN = wx.Color(0,255,0)
        self.BLUE = wx.Color(0,0,255)
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

    def tearDown(self):
        self.frame.Destroy()
        
    ##################
    ## Test Methods ##
    ##################
    def testEnableDisable(self):
        '''Enable, Disable, IsEnabled'''
        self.testControl.Enable(True)
        self.assert_(self.testControl.IsEnabled())
        self.testControl.Enable(False)
        self.assert_(not self.testControl.IsEnabled())
        self.testControl.Enable()
        self.assert_(self.testControl.IsEnabled())
        self.testControl.Disable()
        self.assert_(not self.testControl.IsEnabled())
    
    def testBackgroundColor(self):
        '''SetBackgroundColour, GetBackgroundColour'''
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetBackgroundColour(test)
            gotten = self.testControl.GetBackgroundColour()
            self.assertEquals(gotten, actual)
    
    # see testBackgroundColor
    def testForegroundColor(self):
        '''SetForegroundColour, GetForegroundColour'''
        for test, actual in self.COLOUR_TESTS:
            self.testControl.SetForegroundColour(test)
            gotten = self.testControl.GetForegroundColour()
            self.assertEquals(gotten, actual)
    
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
        
    def testParent(self):
        parent = self.testControl.GetParent()
        self.assertEquals(parent, self.frame)

def suite():
    suite = unittest.makeSuite(WindowTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
