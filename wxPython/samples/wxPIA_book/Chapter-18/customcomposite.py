"""
This sample shows how to put multiple objects in the clipboard, one of
which uses a custom data format.  In this case we use a Python
dictionary of values for our custom format, and we also put a textual
representation of the dictionary.  To test this, run two instances of
this program, enter data in one and click the copy button.  Then click
the paste button in the other instance.  Also paste into a text editor
to see the data in the standard text format.
"""


import wx
import cPickle
import pprint

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Copy/Paste Test")
        panel = wx.Panel(self)

        # First create the controls
        topLbl = wx.StaticText(panel, -1, "Account Information")
        topLbl.SetFont(wx.Font(18, wx.SWISS, wx.NORMAL, wx.BOLD))

        nameLbl = wx.StaticText(panel, -1, "Name:")
        self.name = wx.TextCtrl(panel, -1, "");

        addrLbl = wx.StaticText(panel, -1, "Address:")
        self.addr1 = wx.TextCtrl(panel, -1, "");
        self.addr2 = wx.TextCtrl(panel, -1, "");

        cstLbl = wx.StaticText(panel, -1, "City, State, Zip:")
        self.city  = wx.TextCtrl(panel, -1, "", size=(150,-1));
        self.state = wx.TextCtrl(panel, -1, "", size=(50,-1));
        self.zip   = wx.TextCtrl(panel, -1, "", size=(70,-1));

        phoneLbl = wx.StaticText(panel, -1, "Phone:")
        self.phone = wx.TextCtrl(panel, -1, "");

        emailLbl = wx.StaticText(panel, -1, "Email:")
        self.email = wx.TextCtrl(panel, -1, "");

        copyBtn = wx.Button(panel, -1, "Copy")
        pasteBtn = wx.Button(panel, -1, "Paste")
        self.Bind(wx.EVT_BUTTON, self.OnCopy, copyBtn)
        self.Bind(wx.EVT_BUTTON, self.OnPaste, pasteBtn)

        # Now do the layout.

        # mainSizer is the top-level one that manages everything
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        mainSizer.Add(topLbl, 0, wx.ALL, 5)
        mainSizer.Add(wx.StaticLine(panel), 0,
                wx.EXPAND|wx.TOP|wx.BOTTOM, 5)

        # addrSizer is a grid that holds all of the address info
        addrSizer = wx.FlexGridSizer(cols=2, hgap=5, vgap=5)
        addrSizer.AddGrowableCol(1)
        addrSizer.Add(nameLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(self.name, 0, wx.EXPAND)
        addrSizer.Add(addrLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(self.addr1, 0, wx.EXPAND)
        addrSizer.Add((10,10)) # some empty space
        addrSizer.Add(self.addr2, 0, wx.EXPAND)

        addrSizer.Add(cstLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)

        # the city, state, zip fields are in a sub-sizer
        cstSizer = wx.BoxSizer(wx.HORIZONTAL)
        cstSizer.Add(self.city, 1)
        cstSizer.Add(self.state, 0, wx.LEFT|wx.RIGHT, 5)
        cstSizer.Add(self.zip)
        addrSizer.Add(cstSizer, 0, wx.EXPAND)

        addrSizer.Add(phoneLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(self.phone, 0, wx.EXPAND)
        addrSizer.Add(emailLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(self.email, 0, wx.EXPAND)

        # now add the addrSizer to the mainSizer
        mainSizer.Add(addrSizer, 0, wx.EXPAND|wx.ALL, 10)

        # The buttons sizer will put them in a row with resizeable
        # gaps between and on either side of the buttons
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)
        btnSizer.Add((20,20), 1)
        btnSizer.Add(copyBtn)
        btnSizer.Add((20,20), 1)
        btnSizer.Add(pasteBtn)
        btnSizer.Add((20,20), 1)

        mainSizer.Add(btnSizer, 0, wx.EXPAND|wx.BOTTOM, 10)

        panel.SetSizer(mainSizer)

        # Fit the frame to the needs of the sizer.  The frame will
        # automatically resize the panel as needed.  Also prevent the
        # frame from getting smaller than this size.
        mainSizer.Fit(self)
        self.SetMinSize(self.GetSize())


    fieldNames = ["name", "addr1", "addr2",
                  "city", "state", "zip", "phone", "email"]
    
    def OnCopy(self, evt):
        # make a dictionary of values
        fieldData = {}
        for name in self.fieldNames:
            tc = getattr(self, name)
            fieldData[name] = tc.GetValue()

        # pickle it and put in a custom data object
        cdo = wx.CustomDataObject("ContactDictFormat")
        cdo.SetData(cPickle.dumps(fieldData))

        # also make a text representaion
        tdo = wx.TextDataObject(pprint.pformat(fieldData))

        # and put them both in the clipboard
        dataobj = wx.DataObjectComposite()
        dataobj.Add(cdo)
        dataobj.Add(tdo)
        if wx.TheClipboard.Open():
            wx.TheClipboard.SetData(dataobj)
            wx.TheClipboard.Close()

        
    def OnPaste(self, evt):
        # Get the custom format object and put it into
        # the entry fields
        cdo = wx.CustomDataObject("ContactDictFormat")
        if wx.TheClipboard.Open():
            success = wx.TheClipboard.GetData(cdo)
            wx.TheClipboard.Close()
        if success:
            data = cdo.GetData()
            fieldData = cPickle.loads(data)
            for name in self.fieldNames:
                tc = getattr(self, name)
                tc.SetValue(fieldData[name])
                
    

app = wx.PySimpleApp()
TestFrame().Show()
app.MainLoop()
