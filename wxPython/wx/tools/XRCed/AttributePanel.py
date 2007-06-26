# Name:         AttributePanel.py
# Purpose:      View components for editing attributes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      17.06.2007
# RCS-ID:       $Id$

import wx
from globals import *
import params
import component

labelSize = (100,-1)

# Panel class is the attribute panel containing class name, XRC ID and
# a notebook with particular pages.

class Panel(wx.Panel):
    '''Attribute panel main class.'''
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

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

        # Set common sizes
        params.InitSizes(self)

        # Create scrolled windows for panels
        self.pageA = wx.ScrolledWindow(self.nb, -1)
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.pageA.SetSizer(sizer)
        self.nb.AddPage(self.pageA, 'Attributes')
        # Second page
        self.pageWA = wx.ScrolledWindow(self.nb, -1)
        self.pageWA.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.pageWA.SetSizer(sizer)
        # Third page
        self.pageIA = wx.ScrolledWindow(self.nb, -1)
        self.pageIA.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.pageIA.SetSizer(sizer)

        topSizer.Add(self.nb, 1, wx.EXPAND | wx.ALL, 5)
        self.SetSizer(topSizer)

    def ResetPage(self, page):
        '''Destroy child windows and sizer.'''
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
        self.nb.SetSelection(0)
        map(self.nb.RemovePage, range(self.nb.GetPageCount()-1, 0, -1))
        self.ResetPage(self.pageA)
        
        self.comp = comp
        panels = []
        # First panel
        # Remove current objects and sizer
        self.ResetPage(self.pageA)
        self.controlClass.SetValue(node.getAttribute('class'))
        self.labelName.Show(comp.hasName)
        self.controlName.Show(comp.hasName)
        if comp.hasName:
            self.controlName.SetValue(node.getAttribute('name'))

        self.Layout()

        attributes = comp.attributes[:]
        if comp.styles:
            attributes.append('style')
        if comp.exStyles:
            attributes.append('exstyle')
        panel = AttributePanel(self.pageA, attributes, comp.params)
        self.SetValues(panel, node)
        panels.append(panel)
        self.SetPagePanel(self.pageA, panel)

        if comp.windowAttributes:
            # Create attributes page
            self.ResetPage(self.pageWA)
            panel = AttributePanel(self.pageWA, comp.windowAttributes)
            self.SetValues(panel, node)
            panels.append(panel)
            self.SetPagePanel(self.pageWA, panel)
            self.nb.AddPage(self.pageWA, 'Window Attributes')
            if 'wxGTK' in wx.PlatformInfo:
                self.pageWA.Show(True)

        # Additional panel for hidden node
        if container and container.requireImplicit(node):
            self.ResetPage(self.pageIA)
            panel = AttributePanel(self.pageIA, 
                                   container.implicitAttributes, 
                                   container.implicitParams)
            self.SetValues(panel, node.parentNode)
            panels.append(panel)
            self.SetPagePanel(self.pageIA, panel)
            self.nb.AddPage(self.pageIA, container.implicitPageName)
            if 'wxGTK' in wx.PlatformInfo:
                self.pageIA.Show(True)

        return panels
        
    def Clear(self):
        self.comp = None
        self.nb.SetSelection(0)
        map(self.nb.RemovePage, range(self.nb.GetPageCount()-1, 0, -1))
        self.ResetPage(self.pageA)

        self.controlClass.SetValue('')
        self.labelName.Show(False)
        self.controlName.Show(False)
        
    # Set data for a panel
    def SetValues(self, panel, node):
        panel.node = node
        for a,w in panel.controls:
            value = self.comp.getAttribute(node, a)
            w.SetValue(value)

################################################################################

class AttributePanel(wx.Panel):
    '''Particular attribute panel, normally inside a notebook.'''
    def __init__(self, parent, attributes, paramsDict={}):
        wx.Panel.__init__(self, parent, -1)
        self.controls = []
        sizer = wx.FlexGridSizer(len(attributes), 2, 1, 5)
        for a in attributes:
            # Find good control class
            paramClass = paramsDict.get(a, params.paramDict.get(a, params.ParamText))
            control = paramClass(self, a)
            sParam = a
            if control.isCheck: # checkbox-like control
                label = wx.StaticText(self, -1, control.defaultString)
                sizer.AddMany([ (control, 0, wx.ALIGN_CENTER_VERTICAL),
                                (label, 0, wx.ALIGN_CENTER_VERTICAL) ])
            else:
                label = wx.StaticText(self, -1, sParam, size=labelSize)
                sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL | wx.LEFT, 20),
                                (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            self.controls.append((a, control))
        self.SetSizerAndFit(sizer)
        
