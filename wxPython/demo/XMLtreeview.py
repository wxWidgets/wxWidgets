
import sys

py2 = sys.version[0] == '2'

from wxPython.wx import *
try:
    if py2:
        from xml.parsers import expat
        parsermodule = expat
    else:
        from xml.parsers import pyexpat
        parsermodule = pyexpat
    haveXML = True
except ImportError:
    haveXML = False

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

            # Trees need an image list to do DnD...
            self.il = wxImageList(16,16)
            self.SetImageList(self.il)

            # event handlers for DnD
            EVT_TREE_BEGIN_DRAG(self, ID, self.OnBeginDrag)
            EVT_TREE_END_DRAG(self, ID, self.OnEndDrag)


        def OnBeginDrag(self, event):
            item = event.GetItem()
            if item != self.GetRootItem():
                self.draggingItem = item
                event.Allow()  # if DnD of this item is okay Allow it.


        def OnEndDrag(self, evt):
            itemSrc = self.draggingItem
            itemDst = evt.GetItem()
            self.draggingItem = None

            if not itemDst.IsOk():
                print "Can't drag to here..."
                return

            # For this simple example just take the text of the source item
            # and append it to the destination item.  In real life you would
            # possibly want to copy subtrees...
            text = self.GetItemText(itemSrc)
            self.AppendItem(itemDst, text)
            self.Delete(itemSrc)


        # Define a handler for start element events
        def StartElement(self, name, attrs ):
            if py2:
                name = name.encode()
            id = self.AppendItem(self.nodeStack[-1], name)
            self.nodeStack.append(id)

        def EndElement(self,  name ):
            self.nodeStack = self.nodeStack[:-1]

        def CharacterData(self, data ):
            if data.strip():
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

