#!/usr/bin/env python

from wxPython.wx import *
from wxPython.dllwidget import wxDllWidget, wxDllWidget_GetDllExt

#----------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self):
        wxFrame.__init__(self, None, -1, "Test wxDllWidget")

        menu = wxMenu()
        menu.Append(101, "Send command &1")
        menu.Append(102, "Send command &2")
        menu.Append(103, "Send command &3")
        menu.AppendSeparator()
        menu.Append(110, "E&xit")

        mb = wxMenuBar()
        mb.Append(menu, "&Test")
        self.SetMenuBar(mb)

        EVT_MENU_RANGE(self, 101, 109, self.OnSendCommand)
        EVT_MENU(self, 110, self.OnExit)

        panel = wxPanel(self, -1)
        panel.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))

        st = wxStaticText(panel, -1,
                          "The widget below was dynamically imported from\n"
                          "test_dll.dll or test_dll.so with no prior knowledge\n"
                          "of it's contents or structure by wxPython.")

        self.dw = dw = wxDllWidget(panel, -1,
                                   "test_dll" + wxDllWidget_GetDllExt(),
                                   "TestWindow",
                                   size=(250, 150))

        if dw.Ok():
            # The embedded window is the one exported from the DLL
            print dw.GetWidgetWindow().GetClassName()

            # This shows that we can give it a child from this side of things.
            # You can also call any wxWindow methods on it too.
            wxStaticText(dw.GetWidgetWindow(), -1,
                         "Loaded from test_dll...", pos=(10,10))
        else:
            wxStaticText(dw, -1, "ERROR!!!!", pos=(20,20))

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(wxStaticLine(panel, -1), 0, wxGROW)
        sizer.Add(st, 0, wxGROW|wxALL, 5)
        sizer.Add(dw, 1, wxGROW|wxALL, 5)

        panel.SetSizer(sizer)
        panel.SetAutoLayout(true)
        sizer.Fit(self)
        sizer.SetSizeHints(self)


    def OnExit(self, evt):
        self.Close()


    def OnSendCommand(self, evt):
        ID = evt.GetId() - 100  # use the menu ID as the command
        param = ""
        if ID == 2:
            dlg = wxTextEntryDialog(self, "Enter a colour name to pass to the embedded widget:")
            if dlg.ShowModal() == wxID_OK:
                param = dlg.GetValue()
            dlg.Destroy()
        self.dw.SendCommand(ID, param)



#----------------------------------------------------------------------


if __name__ == "__main__":
    app = wxPySimpleApp()
    frame = TestFrame()
    frame.Show(true)
    app.MainLoop()
