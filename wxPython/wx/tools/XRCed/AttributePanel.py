# Name:         AttributePanel.py
# Purpose:      View components for editing attributes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      17.06.2007
# RCS-ID:       $Id$

import wx
from globals import *
import params
import component

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
        label = wx.StaticText(self, -1, 'class:')
        self.controlClass = params.ParamText(self, 'class', 200)
        sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                        (self.controlClass, 0, wx.LEFT, 5) ])
        self.labelName = wx.StaticText(self, -1, 'XRC ID:')
        self.controlName = params.ParamText(self, 'XML_name', 200)
        sizer.AddMany([ (self.labelName, 0, wx.ALIGN_CENTER_VERTICAL),
                        (self.controlName, 0, wx.LEFT, 5) ])
        topSizer.Add(sizer, 0, wx.ALL, 10)

        self.nb = wx.Notebook(self, -1)

        self.modified = False

        # Set common sizes
        params.InitSizes(self)

        # Create scrolled windows for panels
        self.page1 = wx.ScrolledWindow(self.nb, -1)
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page1.SetSizer(sizer)
        self.nb.AddPage(self.page1, 'Attributes')
        # Second page
        self.page2 = wx.ScrolledWindow(self.nb, -1)
        self.page2.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page2.SetSizer(sizer)
        # Third page
        self.page3 = wx.ScrolledWindow(self.nb, -1)
        self.page3.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page3.SetSizer(sizer)

        topSizer.Add(self.nb, 1, wx.EXPAND | wx.ALL, 5)
        self.SetSizer(topSizer)

    # Delete child windows and recreate panel sizer
    def ResetPage(self, page):
        topSizer = page.GetSizer()
        try:
            sizer = topSizer.GetChildren()[0].GetSizer()
        except IndexError:
            return
        for w in page.GetChildren():
            w.Destroy()
        topSizer.Remove(sizer)

    # Delete child windows and recreate panel sizer
    def SetPagePanel(self, page, panel):
        topSizer = page.GetSizer()
        # Create new sizer
        sizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(sizer, 1, wx.ALL, 5)
        sizer.Add(panel, 0, wx.EXPAND)
        size = page.GetSizer().GetMinSize()
        page.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)

    def SetData(self, container, comp, node):
        self.comp = comp
        self.node = node
        panels = []
        # First panel
        # Remove current objects and sizer
        self.ResetPage(self.page1)
        self.controlClass.SetValue(node.getAttribute('class'))
        self.labelName.Show(comp.hasName)
        self.controlName.Show(comp.hasName)
        if comp.hasName:
            self.controlName.SetValue(node.getAttribute('name'))

        attributes = comp.attributes[:]
        if comp.styles:
            attributes.append('style')
        if comp.exStyles:
            attributes.append('exstyle')
        panel = AttributePanel(self.page1, attributes)
        panel.SetValues(node)
        panels.append(panel)
        self.SetPagePanel(self.page1, panel)

        if comp.windowAttributes:
            self.ResetPage(self.page2)
            panel = AttributePanel(self.page2, comp.windowAttributes)
            panel.SetValues(node)
            panels.append(panel)
            self.SetPagePanel(self.page2, panel)
            # Add page if not exists
            if self.nb.GetPageCount() < 2:
                self.nb.AddPage(self.page2, 'Window Attributes')
                if 'wxGTK' in wx.PlatformInfo:
                    self.page2.Show(True)

            # Additional panel for hidden node
            if container and container.requireImplicit(node):
                self.ResetPage(self.page3)
                panel = AttributePanel(self.page3, container.implicitAttributes)
                panel.SetValues(node.parentNode)
                panels.append(panel)
                self.SetPagePanel(self.page3, panel)
                # Add page if not exists
                if self.nb.GetPageCount() < 3:
                    self.nb.AddPage(self.page3, container.implicitPageName)
                    if 'wxGTK' in wx.PlatformInfo:
                        self.page3.Show(True)

            # Remove page if exists
            else:
                if self.nb.GetPageCount() == 3:
                    self.nb.SetSelection(1)
#???                    self.page2.Refresh()
                    self.nb.RemovePage(2)
        else:
            self.nb.SetSelection(0)
            map(self.nb.RemovePage, range(self.nb.GetPageCount()-1, 0, -1))

        return panels
        
    def Clear(self):
        self.comp = None
        self.nb.SetSelection(0)
        map(self.nb.RemovePage, range(1, self.nb.GetPageCount()))
        self.ResetPage(self.page1)
        self.modified = False
        
    # If some parameter has changed
    def IsModified(self):
        return self.modified
    
    def SetModified(self, value):
        # Register undo object when modifying first time
        if not self.modified and value:
           g.undoMan.RegisterUndo(UndoEdit())
#           view.frame.SetModified()
        self.modified = value
        
    def Apply(self):
        for p in self.panels: p.Apply()

################################################################################

class AttributePanel(wx.Panel):
    renameDict = {'orient':'orientation', 'option':'proportion',
                  'usenotebooksizer':'usesizer', 'dontattachtoframe':'dontattach',
                  }
    def __init__(self, parent, attributes):
        wx.Panel.__init__(self, parent, -1)
        self.controls = []
        sizer = wx.FlexGridSizer(len(attributes), 2, 1, 5)
        for a in attributes:
            paramClass = params.paramDict.get(a, params.ParamText)
            control = paramClass(self, a)
            sParam = a
            if control.isCheck: # checkbox-like control
                label = wx.StaticText(self, -1, control.defaultString)
                sizer.AddMany([ (control, 0, wx.ALIGN_CENTER_VERTICAL),
                                (label, 0, wx.ALIGN_CENTER_VERTICAL) ])
            else:
                label = wx.StaticText(self, -1, sParam, size=_labelSize)
                sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL | wx.LEFT, 20),
                                (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            self.controls.append((a, control))
        self.SetSizerAndFit(sizer)
        
    # Set data for a panel
    def SetValues(self, node):
        self.node = node
        for a,w in self.controls:
            w.modified = False
            value = panel.comp.getValue(node, a)
            w.SetValue(value)

