
import string, sys

py2 = sys.version[0] == '2'

from wxPython.wx import *
try:
    if py2:
        from xml.parsers import expat
        parsermodule = expat
    else:
        from xml.parsers import pyexpat
        parsermodule = pyexpat
    haveXML = true
except ImportError:
    haveXML = false

#----------------------------------------------------------------------

if not haveXML:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(frame, 'This demo requires the XML package.  '
                              'See http://www.python.org/sigs/xml-sig/',
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
            if py2:
                name = name.encode()
            id = self.AppendItem(self.nodeStack[-1], name)
            self.nodeStack.append(id)

        def EndElement(self,  name ):
            self.nodeStack = self.nodeStack[:-1]

        def CharacterData(self, data ):
            if string.strip(data):
                if py2:
                    data = data.encode()
                self.AppendItem(self.nodeStack[-1], data)


        def LoadTree(self, filename):
            # Create a parser
            Parser = parsermodule.ParserCreate()

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

