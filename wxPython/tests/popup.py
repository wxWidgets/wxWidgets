# popup.py:
# Illustrates how to create a wxListCtrl with an associated pop-up menu, which is
# activated when the right mouse button is clicked.

from wxPython.wx import *


class cPopupHandler(wxEvtHandler):

    def __init__(self, this):
        wxEvtHandler.__init__(self, this)


    def ProcessEvent(self, event):
        print "G"
        #wxEvtHandler.ProcessEvent(self, event)

        if event.GetEventClass() != wxTYPE_MOUSE_EVENT:
            return

        if not event.ButtonUp(3):
            return

        if event.ButtonDown(1):
            print "left down"
        elif event.ButtonUp(1):
            print "left up"
        elif event.ButtonDown(3):
            print "right down"
        elif event.ButtonUp(3):
            print "right up"


    def xProcessEvent(self, event):
        # I tried to pass this one in as the Connect() handler,
        # but all I got from that was that the icons disappeared
        # from the wxListCtrl.
        print "H"
        pass



class cMyFrame(wxFrame):

    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, -1, title, wxDefaultPosition, wxSize(800, 600))

        self.Centre(wxBOTH)

        # create a dummy icon; can't seem to get the wxListCtrl to work without an icon
        #self.imagelist = wxImageList(16, 16)
        #self.image = self.imagelist.Add(wxNoRefBitmap('smile.bmp', wxBITMAP_TYPE_BMP))

        # create a ListCtrl
        id = NewId()
        self.listctrl = wxListCtrl(self, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT)
        #self.listctrl.SetImageList(self.imagelist, wxIMAGE_LIST_SMALL)

        if 1:
            # install a handler for mouse right button up events
            #EVT_RIGHT_DOWN(self.listctrl, self.OnListMouseEvent)
            #EVT_RIGHT_UP(self.listctrl, self.OnListMouseEvent)

            #EVT_RIGHT_DOWN(self.listctrl, self.OnSaveMousePos)

            EVT_LIST_ITEM_SELECTED(self, id, self.OnSaveSelection)
            EVT_COMMAND_RIGHT_CLICK(self, id, self.OnListRightClick)
        else:
            # create an wxEvtHandler and connect it to the wxListCtrl
            print "A"
            self.listctrl.handler = cPopupHandler(self.listctrl)
            print "B"
            id = NewId()
            self.listctrl.Connect(id, id, wxEVT_RIGHT_DOWN, self.OnListMouseEvent)
            print "C"

        # define the ListCtrl column
        self.listctrl.InsertColumn(0, "Name")

        # create a set of dummy ListCtrl entries
        for Index in range(20):
            self.listctrl.InsertStringItem(Index, "Item number %d" % Index)

        # re-adjust the width of the column
        self.listctrl.SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER)


    def OnSaveSelection(self, event):
        self.lastSelection = event.m_itemIndex
        print self.lastSelection


    def OnListRightClick(self, event):
        menu = wxPyMenu()
        menu.Append(0, "One")
        menu.Append(1, "Two")
        menu.Append(2, "Three")

        pos = self.listctrl.GetItemPosition(self.lastSelection)
        self.listctrl.PopupMenu(menu, pos.x, pos.y)


class cMyApp(wxApp):

    def OnInit(self):
        frame = cMyFrame(NULL, -1, "Popup Sample")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true


def main():
    App = cMyApp(0)
    App.MainLoop()


if __name__ == "__main__":
    main()





