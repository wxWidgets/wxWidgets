
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sizer = wx.FlexGridSizer(0, 3, 5, 5)
        box = wx.BoxSizer(wx.VERTICAL)
        fs = self.GetFont().GetPointSize()
        bf = wx.Font(fs+4, wx.SWISS, wx.NORMAL, wx.BOLD)

        t = wx.StaticText(self, -1, "StandardPaths")
        t.SetFont(bf)
        box.Add(t, 0, wx.CENTER|wx.ALL, 4)
        box.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)

        # get the global (singleton) instance of wx.StandardPaths
        sp = wx.StandardPaths.Get()

        # StandardPaths will use the value of wx.App().GetAppName()
        # for some of the stnadard path components.  Let's set it to
        # something that makes that obvious for the demo.  In your own
        # apps you'll set it in to something more meaningfull for your
        # app in your OnInit, (or just let it default.)
        wx.GetApp().SetAppName("AppName")

        self.help = {}

        # Loop through all of the getters in wx.StandardPaths and make
        # a set of items in the sizer for each.
        def makeitem(name, *args):
            func = getattr(sp, name)
            sizer.Add(wx.StaticText(self, -1, "%s%s:" %(name, repr(args))),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
            sizer.Add(wx.TextCtrl(self, -1, func(*args),
                                  size=(275,-1), style=wx.TE_READONLY),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)

            btn = wx.Button(self, wx.ID_HELP)
            sizer.Add(btn)
            self.help[btn] = func.__doc__
            
        for x in ['GetConfigDir',
                  'GetUserConfigDir',
                  'GetDataDir',
                  'GetLocalDataDir',
                  'GetUserDataDir',
                  'GetUserLocalDataDir',
                  'GetDocumentsDir',
                  'GetPluginsDir',
                  'GetInstallPrefix',
                  'GetResourcesDir',
                  'GetTempDir',
                  ]:
            makeitem(x)

        # this one needs parameters
        makeitem('GetLocalizedResourcesDir', 'en',
                 wx.StandardPaths.ResourceCat_Messages )

        self.Bind(wx.EVT_BUTTON, self.OnShowDoc, id=wx.ID_HELP)
        box.Add(sizer, 0, wx.CENTER|wx.ALL, 10)
        self.SetSizer(box)


    def OnShowDoc(self, evt):
        doc = self.help[evt.GetEventObject()]

        # trim the whitespace from each line
        lines = []
        for line in doc.split('\n'):
            lines.append(line.strip())
        doc = '\n'.join(lines)
        wx.TipWindow(self, doc, 500)
        
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>StandardPaths</center></h2>

wxWidgets provides this class to simply determine where to locate
certain types of files in a platform specific manner.  This includes
things like configuration files, general data files writeable by the
user, and application files that are shared by all user.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

