
import string

from wxPython.wx import *
try:
    from xml.parsers import pyexpat
    haveXML = true
except ImportError:
    haveXML = false

#----------------------------------------------------------------------

if not haveXML:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(frame, 'This demo requires the XML package.  See http://www.python.org/sigs/xml-sig/',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()

else:

    class XMLTree(wxTreeCtrl):
        def __init__(self, parent, ID):
            wxTreeCtrl.__init__(self, parent, ID)
            self.nodeStack = [self.AddRoot("Root")]

        # Define a handler for start element events
        def StartElement(self, name, attrs ):
            self.nodeStack.append(self.AppendItem(self.nodeStack[-1], name))

        def EndElement(self,  name ):
            self.nodeStack = self.nodeStack[:-1]

        def CharacterData(self, data ):
            if string.strip(data):
                self.AppendItem(self.nodeStack[-1], data)


        def LoadTree(self, filename):
            # Create a parser
            Parser = pyexpat.ParserCreate()

            # Tell the parser what the start element handler is
            Parser.StartElementHandler = self.StartElement
            Parser.EndElementHandler = self.EndElement
            Parser.CharacterDataHandler = self.CharacterData

            # Parse the XML File
            ParserStatus = Parser.Parse(open(filename,'r').read(), 1)


    def runTest(frame, nb, log):
        win = XMLTree(nb, -1)
        win.LoadTree("paper.xml")
        return win

#----------------------------------------------------------------------






overview = """\
"""

