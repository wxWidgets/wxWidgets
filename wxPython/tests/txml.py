"""
Build a GUI Tree (wxWindows) from an XML file
using pyExpat
"""

import sys,string
from xml.parsers import pyexpat

from wxPython.wx import *

class MyFrame(wxFrame):
        def __init__(self, parent, id, title):
                wxFrame.__init__(self, parent, id, title, wxPoint(100, 100), wxSize(160,100))
                menu = wxMenu()
                menu.Append (1001,"Open")
                menu.Append (1002,"Close")
                menu.Append (1003,"Exit")
                menubar = wxMenuBar()
                menubar.Append (menu,"File")
                self.SetMenuBar(menubar)

class MyApp(wxApp):
        def OnInit(self):
                self.frame = MyFrame(NULL, -1, "Tree View of XML")
                self.tree = wx.wxTreeCtrl(self.frame, -1)
                EVT_MENU(self, 1001, self.OnOpen)
                EVT_MENU(self, 1002, self.OnClose)
                EVT_MENU(self, 1003, self.OnExit)
                self.frame.Show(true)
                self.SetTopWindow(self.frame)
                return true

        def OnOpen(self,event):
                f = wxFileDialog(self.frame,"Select a file",".","","*.xml",wxOPEN)
                if f.ShowModal() == wxID_OK:
                        LoadTree(f.GetPath())

        def OnClose(self,event):
                self.tree = wx.wxTreeCtrl(self.frame, -1)
                pass

        def OnExit(self,event):
                self.OnCloseWindow(event)

        def OnCloseWindow(self, event):
                self.frame.Destroy()


NodeStack = []

# Define a handler for start element events
def StartElement( name, attrs ):
        global NodeStack
        NodeStack.append(app.tree.AppendItem(NodeStack[-1],name))

def EndElement( name ):
        global NodeStack
        NodeStack = NodeStack[:-1]

def CharacterData ( data ):
        global NodeStack
        if string.strip(data):
                app.tree.AppendItem(NodeStack[-1],data)


def LoadTree (f):
        print f
        # Create a parser
        Parser = pyexpat.ParserCreate()

        # Tell the parser what the start element handler is
        Parser.StartElementHandler = StartElement
        Parser.EndElementHandler = EndElement
        Parser.CharacterDataHandler = CharacterData

        # Parse the XML File
        ParserStatus = Parser.Parse(open(f,'r').read(), 1)
        if ParserStatus == 0:
                print "oops!"
                raise SystemExit

app = MyApp(0)
NodeStack = [app.tree.AddRoot("Root")]


app.MainLoop()
raise SystemExit
