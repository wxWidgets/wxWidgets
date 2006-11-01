import wx

class ListBoxFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'List Box Example', 
                size=(250, 200))
        panel = wx.Panel(self, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight', 'nine', 'ten', 'eleven',
                      'twelve', 'thirteen', 'fourteen']

        listBox = wx.ListBox(panel, -1, (20, 20), (80, 120), sampleList, 
                wx.LB_SINGLE)
        listBox.SetSelection(3)
                
if __name__ == '__main__':
    app = wx.PySimpleApp()
    ListBoxFrame().Show()
    app.MainLoop()  

