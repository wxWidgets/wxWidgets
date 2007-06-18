# Name:         AttributePanel.py
# Purpose:      View components for editing attributes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      17.06.2007
# RCS-ID:       $Id$

from params import *

# Attribute panel containing notebook
class Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        global panel
        g.panel = panel = self

        global _labelSize
        _labelSize = (100,-1)

        topSizer = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.FlexGridSizer(2, 2, 1, 5)
        label = wx.StaticText(self, -1, 'Class:')
        self.controlClass = ParamText(self, 'class', 200)
        sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                        (self.controlClass, 0, wx.LEFT, 5) ])
        label = wx.StaticText(self, -1, 'XRC ID:')
        self.controlName = ParamText(self, 'XML_name', 200)
        sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                        (self.controlName, 0, wx.LEFT, 5) ])
        topSizer.Add(sizer, 0, wx.ALL, 5)

        self.nb = wx.Notebook(self, -1)

        self.modified = False

        # Set common sizes
        import params
        cTmp = wx.Button(self, -1, '')
        params.buttonSize = (self.DLG_SZE(buttonSizeD)[0], cTmp.GetSize()[1])
        cTmp.Destroy()

        # Create scrolled windows for panels
        self.page1 = wx.ScrolledWindow(self.nb, -1)
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page1.SetAutoLayout(True)
        self.page1.SetSizer(sizer)
        self.nb.AddPage(self.page1, 'Attributes')
        # Second page
        self.page2 = wx.ScrolledWindow(self.nb, -1)
        self.page2.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page2.SetAutoLayout(True)
        self.page2.SetSizer(sizer)

        topSizer.Add(self.nb, 1, wx.EXPAND)
        self.SetSizer(topSizer)

    # Delete child windows and recreate panel sizer
    def ResetPage(self, page):
        topSizer = page.GetSizer()
        sizer = topSizer.GetChildren()[0].GetSizer()
        for w in page.GetChildren():
            w.Destroy()
        topSizer.Remove(sizer)
        # Create new windows
        sizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(sizer, 1, wx.ALL, 5)
        return sizer

    def SetData(self, comp, node):
        self.comp = comp
        self.node = node
        self.controls = {}
        panels = []
        # First panel
        # Remove current objects and sizer
        sizer = self.ResetPage(self.page1)
        if not node:
            sizer.Add(wx.StaticText(self.page1, -1, 'This item has no properties.'))
            return

        self.controlClass.SetValue(node.getAttribute('class'))
        self.controlName.SetValue(node.getAttribute('name'))
            
        panel = AttributePanel(self.page1, comp)
        panel.SetValues(node)
        panels.append(panel)
        sizer.Add(panel, 1, wx.EXPAND)
        self.page1.Layout()
        size = self.page1.GetSizer().GetMinSize()
        self.page1.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)

        if comp.windowAttributes:
            sizer = self.ResetPage(self.page2)
            panel = WindowAttributePanel(self.page2, comp)
            panel.SetValues(node)
            panels.append(panel)
            sizer.Add(panel, 0, wx.EXPAND)
            # Add page if not exists
            if not self.nb.GetPageCount() == 2:
                self.nb.AddPage(self.page2, 'Window Attributes')
            self.page2.Layout()
            if 'wxGTK' in wx.PlatformInfo:
                self.page2.Show(True)
            size = self.page2.GetSizer().GetMinSize()
            self.page2.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)
        else:
            # Remove page if exists
            if self.nb.GetPageCount() == 2:
                self.nb.SetSelection(0)
                self.page1.Refresh()
                self.nb.RemovePage(1)            

        return panels
        
    def Clear(self):
        self.SetData(None)
        self.modified = False
        
    # If some parameter has changed
    def IsModified(self):
        return self.modified
    
    def SetModified(self, value):
        # Register undo object when modifying first time
#        if not self.modified and value:
#           g.undoMan.RegisterUndo(UndoEdit())
#           g.frame.SetModified()
        self.modified = value
        
    def Apply(self):
        for p in self.panels: p.Apply()

################################################################################

class AttributePanel(wx.Panel):
    renameDict = {'orient':'orientation', 'option':'proportion',
                  'usenotebooksizer':'usesizer', 'dontattachtoframe':'dontattach',
                  }
    def __init__(self, parent, comp):
        wx.Panel.__init__(self, parent, -1)
        self.comp = comp
        sizer = wx.FlexGridSizer(len(comp.attributes), 2, 1, 5)
        for a in comp.attributes:
            sParam = self.renameDict.get(a, a)
            label = wx.StaticText(self, -1, sParam, size=_labelSize)
            paramClass = paramDict.get(a, ParamText)
            control = paramClass(self, a)
            sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                            (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            panel.controls[a] = control
        self.SetSizerAndFit(sizer)
        
    # Set data for a panel
    def SetValues(self, node):
        self.node = node
        for a in self.comp.attributes:
            w = panel.controls[a]
            w.modified = False
            value = self.comp.getValue(node, a)
            w.SetValue(value)

    def GetValues(self):
        values = {}
        for a,w in panel.controls.items():
            values[a] = w.GetValue()
        return self.controlName.GetValue(), values
        
class WindowAttributePanel(wx.Panel):
    def __init__(self, parent, comp):
        wx.Panel.__init__(self, parent, -1)
        self.comp = comp
        sizer = wx.FlexGridSizer(len(comp.attributes), 2, 1, 5)
        for a in comp.windowAttributes:
            sParam = a
            label = wx.StaticText(self, -1, sParam, size=_labelSize)
            paramClass = paramDict.get(a, ParamText)
            control = paramClass(self, a)
            sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                            (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            panel.controls[a] = control
        self.SetSizerAndFit(sizer)
        
    # Set data for a panel
    def SetValues(self, node):
        self.node = node
        for a in self.comp.windowAttributes:
            w = panel.controls[a]
            w.modified = False
            value = self.comp.getValue(node, a)
            w.SetValue(value)
        
    def GetValues(self):
        values = {}
        for a,w in panel.controls.items():
            values[a] = w.GetValue()
        return values
        
