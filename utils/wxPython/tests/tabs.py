from wxPython.wx import *
import string, sys

class Test:
     def __init__(self):
          self.panel = wxPanel(frame, -1)
          self.box = wxListBox(self.panel, 100, wxPoint(10,10),
                               wxSize(300,100), [], wxLB_SINGLE|wxLB_SORT)
          self.text = wxTextCtrl(self.panel, 110,'', wxPoint(310,10),
                                 wxSize(300,100),wxTE_MULTILINE|wxTE_READONLY)
          self.FillList()

     def FillList(self):
          line = 'This   is   a   test'
          self.box.Append(line)
          self.text.AppendText(line)

     def OnCloseWindow(self, event):
          self.panel.Close(true)

class MyApp(wxApp):
     def OnInit(self):
          global frame
          frame = wxFrame(NULL,-1,'Main',wxDefaultPosition,wxSize(630,150))
          test = Test()
          frame.Show(true)
          self.SetTopWindow(frame)
          return true

     def OnCloseWindow(self, event):
          self.Destroy()

if __name__ == '__main__':
     app = MyApp(0)
     app.MainLoop()



