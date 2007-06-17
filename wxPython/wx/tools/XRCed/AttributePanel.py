# Name:         AttributePanel.py
# Purpose:      View components for editing attributes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      17.06.2007
# RCS-ID:       $Id$

from params import *

# Properties panel containing notebook
class Panel(wx.Notebook):
    def __init__(self, parent, id = -1):
        if wx.Platform != '__WXMAC__':   # some problems with this style on macs
            wx.Notebook.__init__(self, parent, id, style=wx.NB_BOTTOM)
        else:
            wx.Notebook.__init__(self, parent, id)
        global panel
        g.panel = panel = self
        self.modified = False

        # Set common sizes
        import params
        cTmp = wx.Button(self, -1, '')
        params.buttonSize = (self.DLG_SZE(buttonSizeD)[0], cTmp.GetSize()[1])        
        cTmp.Destroy()
        cTmp = wx.TextCtrl(self, -1, '')
        params.textSize = cTmp.GetSize()
        cTmp.Destroy()
        cTmp = wx.CheckBox(self, -1, 'growablerows ') # this is the longest
        global _labelSize
        _labelSize = cTmp.GetSize()
        cTmp.Destroy()
        del cTmp

        # List of child windows
        self.pages = []
        # Create scrolled windows for pages
        self.page1 = wx.ScrolledWindow(self, -1)
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page1.SetAutoLayout(True)
        self.page1.SetSizer(sizer)
        self.AddPage(self.page1, 'Properties')
        # Second page
        self.page2 = wx.ScrolledWindow(self, -1)
        self.page2.Hide()
        sizer = wx.BoxSizer()
        sizer.Add(wx.BoxSizer())         # dummy sizer
        self.page2.SetAutoLayout(True)
        self.page2.SetSizer(sizer)
        # Cache for already used panels
        self.pageCache = {}             # cached property panels
        self.stylePageCache = {}        # cached style panels

    # Delete child windows and recreate page sizer
    def ResetPage(self, page):
        topSizer = page.GetSizer()
        sizer = topSizer.GetChildren()[0].GetSizer()
        for w in page.GetChildren():
            sizer.Detach(w)
            if isinstance(w, AttributePanel):
                if w.IsShown():
                    w.Hide()
            else:
                w.Destroy()
        topSizer.Remove(sizer)
        # Create new windows
        sizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(sizer, 1, wx.ALL, 5)
        return sizer

    def SetData(self, comp, node):
        self.pages = []
        # First page
        # Remove current objects and sizer
        sizer = self.ResetPage(self.page1)
        if not node:
            sizer.Add(wx.StaticText(self.page1, -1, 'This item has no properties.'))
            return
            
        page = AttributePanel(self.page1, comp)
#            page.SetValues(xxx)
        self.pages.append(page)
        sizer.Add(page, 1, wx.EXPAND)
        self.page1.Layout()
        size = self.page1.GetSizer().GetMinSize()
        self.page1.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)

        if comp.windowAttributes:
            sizer = self.ResetPage(self.page2)
            page = WindowAttributePanel(self.page2, comp)
#            page.SetValues(xxx)
            self.pages.append(page)
            sizer.Add(page, 0, wx.EXPAND)
            # Add page if not exists
            if not self.GetPageCount() == 2:
                self.AddPage(self.page2, 'Style')
            self.page2.Layout()
            if 'wxGTK' in wx.PlatformInfo:
                self.page2.Show(True)
            size = self.page2.GetSizer().GetMinSize()
            self.page2.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)
        else:
            # Remove page if exists
            if self.GetPageCount() == 2:
                self.SetSelection(0)
                self.page1.Refresh()
                self.RemovePage(1)            

        self.modified = False
        
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
        for p in self.pages: p.Apply()

################################################################################

class AttributePanel(wx.Panel):
    renameDict = {'orient':'orientation', 'option':'proportion',
                  'usenotebooksizer':'usesizer', 'dontattachtoframe':'dontattach',
                  }
    def __init__(self, parent, comp):
        wx.Panel.__init__(self, parent, -1)
        self.box = wx.StaticBox(self, -1, comp.name)
        #self.box.SetFont(g.labelFont())
        topSizer = wx.StaticBoxSizer(self.box, wx.VERTICAL)
        sizer = wx.FlexGridSizer(len(comp.attributes), 2, 1, 5)
        sizer.AddGrowableCol(1)
        label = wx.StaticText(self, -1, 'XML ID:', size=_labelSize)
        control = ParamText(self, 'XML_name', 200)
        sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                        (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.BOTTOM | wx.GROW, 10) ])
        self.controlName = control
        self.controls = {}
        for a in comp.attributes:
            sParam = self.renameDict.get(a, a)
            label = wx.StaticText(self, -1, sParam, size=_labelSize)
            typeClass = ParamText
            control = typeClass(self, a)
            sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                            (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            self.controls[a] = control
        topSizer.Add(sizer, 1, wx.ALL | wx.EXPAND, 3)
        self.SetSizer(topSizer)
        topSizer.Fit(self)        
        
    # Set data for a cahced page
    def SetValues(self, xxx):
        self.xxx = xxx
        self.origControls = []
        for param in xxx.styles:
            present = xxx.params.has_key(param)
            check = self.checks[param]
            check.SetValue(present)
            w = self.controls[param]
            w.modified = False
            if present:
                value = xxx.params[param].value()
            else:
                value = ''
            w.SetValue(value)
            w.Enable(present)
            self.origControls.append((param, value, present))
        self.xxx = xxx
        self.controls = {}              # save python objects
        self.controlName = None
        
    def OnCheckParams(self, evt):
        xxx = self.xxx
        param = evt.GetEventObject().GetName()
        w = self.controls[param]
        w.Enable(True)
        objElem = xxx.node
        if evt.IsChecked():
            # Ad  new text node in order of allParams
            w.SetValue('')              # set empty (default) value
            w.SetModified()             # mark as changed
            elem = g.tree.dom.createElement(param)
            # Some classes are special
            if param == 'font':
                xxx.params[param] = xxxParamFont(xxx.node, elem)
            elif param in xxxObject.bitmapTags:
                xxx.params[param] = xxxParamBitmap(elem)
            else:
                xxx.params[param] = xxxParam(elem)
            # Find place to put new element: first present element after param
            found = False
            if xxx.hasStyle: 
                paramStyles = xxx.allParams + xxx.styles
            else:
                paramStyles = xxx.allParams 
            for p in paramStyles[paramStyles.index(param) + 1:]:
                # Content params don't have same type
                if xxx.params.has_key(p) and p != 'content':
                    found = True
                    break
            if found:
                nextTextElem = xxx.params[p].node
                objElem.insertBefore(elem, nextTextElem)
            else:
                objElem.appendChild(elem)
        else:
            # Remove parameter
            xxx.params[param].remove()
            del xxx.params[param]
            w.SetValue('')
            w.SetModified(False)        # mark as not changed
            w.Enable(False)
        # Set modified flag (provokes undo storing is necessary)
        panel.SetModified(True)
    def Apply(self):
        xxx = self.xxx
        if self.controlName:
            name = self.controlName.GetValue()
            if xxx.name != name:
                xxx.name = name
                xxx.node.setAttribute('name', name)
        for param, w in self.controls.items():
            if w.modified:
                paramObj = xxx.params[param]
                value = w.GetValue()
                if param in xxx.specials:
                    xxx.setSpecial(param, value)
                else:
                    paramObj.update(value)
                
    # Save current state
    def SaveState(self):
        self.origChecks = map(lambda i: (i[0], i[1].GetValue()), self.checks.items())
        self.origControls = map(lambda i: (i[0], i[1].GetValue(), i[1].enabled),
                            self.controls.items())
        if self.controlName:
            self.origName = self.controlName.GetValue()
    # Return original values
    def GetState(self):
        if self.controlName:
            return (self.origChecks, self.origControls, self.origName)
        else:
            return (self.origChecks, self.origControls)
    # Set values from undo data
    def SetState(self, state):
        for k,v in state[0]:
            self.checks[k].SetValue(v)
        for k,v,e in state[1]:
            self.controls[k].SetValue(v)
            self.controls[k].Enable(e)
            # Set all states to modified
            if e and k in self.xxx.params: self.controls[k].modified = True            
        if self.controlName:
            self.controlName.SetValue(state[2])


class WindowAttributePanel(wx.Panel):
    def __init__(self, parent, comp):
        wx.Panel.__init__(self, parent, -1)
        topSizer = sizer = wx.FlexGridSizer(len(comp.attributes), 2, 1, 5)
        self.controls = {}
        for a in comp.windowAttributes:
            sParam = a
            label = wx.StaticText(self, -1, sParam, size=_labelSize)
            typeClass = ParamText
            control = typeClass(self, a)
            sizer.AddMany([ (label, 0, wx.ALIGN_CENTER_VERTICAL),
                            (control, 0, wx.ALIGN_CENTER_VERTICAL | wx.GROW) ])
            self.controls[a] = control
        self.SetSizer(topSizer)
        topSizer.Fit(self)        
        
