from wxPython.wx import *

LIST_ID = 101

class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title,
                         wxDefaultPosition, wxSize(400, 400))
        self.panel = wxPanel(self, -1)
        self.list = wxListCtrl(self.panel, LIST_ID,
                               wxPoint(10, 10), wxSize(370, 330),
                               wxLC_REPORT|wxSUNKEN_BORDER)
        self.list.InsertColumn(0, "Id")
        self.list.InsertColumn(1, "Type")
        self.list.InsertColumn(2, "Description")

        self.insertRow(self.list, 0, 'CD', 'Dark Side of the Moon')
        self.insertRow(self.list, 1, 'DVD', 'The Matrix')
        self.insertRow(self.list, 2, 'Book', 'Crime and Punishment')

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER)

        EVT_LIST_ITEM_SELECTED(self.panel, LIST_ID, self.OnListSelect)

        self.panel.Layout()
        return

    def OnListSelect(self, event):
        item = self.list.GetItem(event.m_itemIndex, 1)
        print item.m_itemId, item.m_col, item.m_state
        type = self.list.GetItem(event.m_itemIndex, col=1).m_text
        desc = self.list.GetItem(event.m_itemIndex, col=2).m_text
        print ('Row Selected: Id: %d, Type: %s, Desc: %s' %
               (event.m_itemIndex, `type`, `desc`))
        return

    def insertRow(self, list, row, type, desc):
        list.InsertStringItem(row, 'label' + `row`)
        list.SetStringItem(row, 0, `row`)
        list.SetStringItem(row, 1, type)
        list.SetStringItem(row, 2, desc)
        return

    def OnCloseWindow(self, event):
        self.Destroy()
        return



class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame(None, -1, 'ListCtrl Test')
        frame.Show(1)
        self.SetTopWindow(frame)
        return 1


app = MyApp(0)
app.MainLoop()
