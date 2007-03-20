import wx
from wx.lib.combotreebox import ComboTreeBox


#---------------------------------------------------------------------------


class TestComboTreeBox(wx.Panel):
    def __init__(self, parent, log):
        super(TestComboTreeBox, self).__init__(parent)
        self.log = log
        panelSizer = wx.FlexGridSizer(2, 2)
        panelSizer.AddGrowableCol(1)
        for style, labelText in [(0, 'Default style:'), 
                                 (wx.CB_READONLY, 'Read-only style:')]:
            label = wx.StaticText(self, label=labelText)
            panelSizer.Add(label, flag=wx.ALL|wx.ALIGN_CENTER_VERTICAL, 
                           border=5)
            comboBox = self._createComboTreeBox(style)
            panelSizer.Add(comboBox, flag=wx.EXPAND|wx.ALL, border=5)
        self.SetSizerAndFit(panelSizer)

    def _createComboTreeBox(self, style):
        comboBox = ComboTreeBox(self, style=style)
        self._bindEventHandlers(comboBox)
        for i in range(5):
            child = comboBox.Append('Item %d'%i)
            for j in range(5):
                grandChild = comboBox.Append('Item %d.%d'%(i,j), child)
                for k in range(5):
                    comboBox.Append('Item %d.%d.%d'%(i,j, k), grandChild)
        return comboBox
        
    def _bindEventHandlers(self, comboBox):
        for eventType, handler in [(wx.EVT_COMBOBOX, self.OnItemSelected), 
                                   (wx.EVT_TEXT, self.OnItemEntered)]:
            comboBox.Bind(eventType, handler)

    def OnItemSelected(self, event):
        self.log.WriteText('You selected: %s\n'%event.GetString())
        event.Skip()

    def OnItemEntered(self, event):
        self.log.WriteText('You entered: %s\n'%event.GetString())
        event.Skip()


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestComboTreeBox(nb, log)
    return win


#---------------------------------------------------------------------------


overview = wx.lib.combotreebox.__doc__


#---------------------------------------------------------------------------


if __name__ == '__main__':
    import sys, os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

