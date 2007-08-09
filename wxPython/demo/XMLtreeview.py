
import  sys
import  wx

from xml.parsers import expat

#----------------------------------------------------------------------

class XMLTree(wx.TreeCtrl):
    def __init__(self, parent, ID):
        wx.TreeCtrl.__init__(self, parent, ID)
        self._root = self.AddRoot("Root")
        self.nodeStack = [self._root]

        # Trees need an image list to do DnD...
        self.il = wx.ImageList(16,16)
        self.SetImageList(self.il)

        # event handlers for DnD
        self.Bind(wx.EVT_TREE_BEGIN_DRAG, self.OnBeginDrag)
        self.Bind(wx.EVT_TREE_END_DRAG, self.OnEndDrag)


    def OnBeginDrag(self, event):
        item = event.GetItem()

        if item != self.GetRootItem():
            self.draggingItem = item
            event.Allow()  # if DnD of this item is okay Allow it.

    def IsDescendant(self, firstItem, secondItem):
        "Recursive check if firstItem is a descendant of a secondItem."
        if firstItem == self._root:
            return False
        parentItem = self.GetItemParent(firstItem)
        if parentItem == secondItem:
            return True
        else:
            return self.IsDescendant(parentItem, secondItem)

    def OnEndDrag(self, evt):
        itemSrc = self.draggingItem
        itemDst = evt.GetItem()
        self.draggingItem = None

        if not itemDst.IsOk():
            print "Can't drag to here..."
            return

        if self.IsDescendant(itemDst, itemSrc):
            print "Can't move item to its descendant"
            return

        # For this simple example just take the text of the source item
        # and append it to the destination item.  In real life you would
        # possibly want to copy subtrees...
        text = self.GetItemText(itemSrc)
        self.AppendItem(itemDst, text)
        self.Delete(itemSrc)


    # Define a handler for start element events
    def StartElement(self, name, attrs ):
        name = name.encode()

        id = self.AppendItem(self.nodeStack[-1], name)
        self.nodeStack.append(id)

    def EndElement(self,  name ):
        self.nodeStack = self.nodeStack[:-1]

    def CharacterData(self, data ):
        if data.strip():
            data = data.encode()

            self.AppendItem(self.nodeStack[-1], data)


    def LoadTree(self, filename):
        # Create a parser
        Parser = expat.ParserCreate()

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


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
