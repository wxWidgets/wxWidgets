
import  wx
import  images

FRAMETB = True
TBFLAGS = ( wx.TB_HORIZONTAL
            | wx.NO_BORDER
            | wx.TB_FLAT
            #| wx.TB_TEXT
            #| wx.TB_HORZ_LAYOUT
            )

#---------------------------------------------------------------------------

class TestSearchCtrl(wx.SearchCtrl):
    maxSearches = 5
    
    def __init__(self, parent, id=-1, value="",
                 pos=wx.DefaultPosition, size=wx.DefaultSize, style=0,
                 doSearch=None):
        style |= wx.TE_PROCESS_ENTER
        wx.SearchCtrl.__init__(self, parent, id, value, pos, size, style)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnTextEntered)
        self.Bind(wx.EVT_MENU_RANGE, self.OnMenuItem, id=1, id2=self.maxSearches)
        self.doSearch = doSearch
        self.searches = []

    def OnTextEntered(self, evt):
        text = self.GetValue()
        if self.doSearch(text):
            self.searches.append(text)
            if len(self.searches) > self.maxSearches:
                del self.searches[0]
            self.SetMenu(self.MakeMenu())            
        self.SetValue("")

    def OnMenuItem(self, evt):
        text = self.searches[evt.GetId()-1]
        self.doSearch(text)
        
    def MakeMenu(self):
        menu = wx.Menu()
        item = menu.Append(-1, "Recent Searches")
        item.Enable(False)
        for idx, txt in enumerate(self.searches):
            menu.Append(1+idx, txt)
        return menu
    


class TestToolBar(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, 'Test ToolBar', size=(600, 400))
        self.log = log
        self.timer = None
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        client = wx.Panel(self)
        client.SetBackgroundColour(wx.NamedColour("WHITE"))

        if FRAMETB:
            # Use the wxFrame internals to create the toolbar and
            # associate it all in one tidy method call.  By using
            # CreateToolBar or SetToolBar the "client area" of the
            # frame will be adjusted to exclude the toolbar.
            tb = self.CreateToolBar( TBFLAGS )

            # Here's a 'simple' toolbar example, and how to bind it using SetToolBar()
            #tb = wx.ToolBarSimple(self, -1, wx.DefaultPosition, wx.DefaultSize,
            #               wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)
            #self.SetToolBar(tb)
            # But we're doing it a different way here.

        else:
            # The toolbar can also be a child of another widget, and
            # be managed by a sizer, although there may be some
            # implications of doing this on some platforms.
            tb = wx.ToolBar(client, style=TBFLAGS)
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(tb, 0, wx.EXPAND)
            client.SetSizer(sizer)
            

        log.write("Default toolbar tool size: %s\n" % tb.GetToolBitmapSize())

        self.CreateStatusBar()

        tsize = (24,24)
        new_bmp =  wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR, tsize)
        open_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR, tsize)
        copy_bmp = wx.ArtProvider.GetBitmap(wx.ART_COPY, wx.ART_TOOLBAR, tsize)
        paste_bmp= wx.ArtProvider.GetBitmap(wx.ART_PASTE, wx.ART_TOOLBAR, tsize)

        tb.SetToolBitmapSize(tsize)
        
        #tb.AddSimpleTool(10, new_bmp, "New", "Long help for 'New'")
        tb.AddLabelTool(10, "New", new_bmp, shortHelp="New", longHelp="Long help for 'New'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=10)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=10)

        #tb.AddSimpleTool(20, open_bmp, "Open", "Long help for 'Open'")
        tb.AddLabelTool(20, "Open", open_bmp, shortHelp="Open", longHelp="Long help for 'Open'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=20)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=20)

        tb.AddSeparator()
        tb.AddSimpleTool(30, copy_bmp, "Copy", "Long help for 'Copy'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=30)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=30)

        tb.AddSimpleTool(40, paste_bmp, "Paste", "Long help for 'Paste'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=40)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=40)

        tb.AddSeparator()

        #tool = tb.AddCheckTool(50, images.getTog1Bitmap(), shortHelp="Toggle this")
        tool = tb.AddCheckLabelTool(50, "Checkable", images.getTog1Bitmap(),
                                    shortHelp="Toggle this")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=50)

        self.Bind(wx.EVT_TOOL_ENTER, self.OnToolEnter)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick) # Match all
        self.Bind(wx.EVT_TIMER, self.OnClearSB)

        tb.AddSeparator()
        cbID = wx.NewId()

        tb.AddControl(
            wx.ComboBox(
                tb, cbID, "", choices=["", "This", "is a", "wx.ComboBox"],
                size=(150,-1), style=wx.CB_DROPDOWN
                ))
        self.Bind(wx.EVT_COMBOBOX, self.OnCombo, id=cbID)

        tb.AddSeparator()
        search = TestSearchCtrl(tb, size=(150,-1), doSearch=self.DoSearch)
        tb.AddControl(search)

        # Final thing to do for a toolbar is call the Realize() method. This
        # causes it to render (more or less, that is).
        tb.Realize()


    def DoSearch(self,  text):
        # called by TestSearchCtrl
        self.log.WriteText("DoSearch: %s\n" % text)
        # return true to tell the search ctrl to remember the text
        return True
    

    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())
        #tb = self.GetToolBar()
        tb = event.GetEventObject()
        tb.EnableTool(10, not tb.GetToolEnabled(10))

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())

    def OnCombo(self, event):
        self.log.WriteText("combobox item selected: %s\n" % event.GetString())

    def OnToolEnter(self, event):
        self.log.WriteText('OnToolEnter: %s, %s\n' % (event.GetId(), event.GetInt()))

        if self.timer is None:
            self.timer = wx.Timer(self)

        if self.timer.IsRunning():
            self.timer.Stop()

        self.timer.Start(2000)
        event.Skip()


    def OnClearSB(self, event):  # called for the timer event handler
        self.SetStatusText("")
        self.timer.Stop()
        self.timer = None


    def OnCloseWindow(self, event):
        if self.timer is not None:
            self.timer.Stop()
            self.timer = None
        self.Destroy()

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the ToolBar sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = TestToolBar(self, self.log)
        win.Show(True)
        self.frame = win


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
wx.ToolBar is a narrow strip of icons on one side of a frame (top, bottom, sides)
that acts much like a menu does, except it is always visible. Additionally, actual
wxWindows controls, such as wx.TextCtrl or wx.ComboBox, can be added to the toolbar
and used from within it.

Toolbar creation is a two-step process. First, the toolbar is defined using the
various Add* methods of wx.ToolBar. Once all is set up, then wx.Toolbar.Realize()
must be called to render it.

wx.Toolbar events are also propogated as Menu events; this is especially handy when
you have a menu bar that contains items that carry out the same function. For example,
it is not uncommon to have a little 'floppy' toolbar icon to 'save' the current file 
(whatever it is) as well as a FILE/SAVE menu item that does the same thing. In this
case, both events can be captured and acted upon using the same event handler
with no ill effects.

If there are cases where a toolbar icon should *not* be associated with a menu item,
use a unique ID to trap it.

There are a number of ways to create a toolbar for a wx.Frame. wx.Frame.CreateToolBar() 
does all the work except it adds no buttons at all unless you override the virtual method
OnCreateToolBar(). On the other hand, you can just subclass wx.ToolBar and then use
wx.Frame.SetToolBar() instead.

Note that wx.TB_DOCKABLE is only supported under GTK. An attempt to alleviate this
is provided in wx.lib.floatbar, but it is not formally supported.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

