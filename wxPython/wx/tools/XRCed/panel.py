# Name:         panel.py
# Purpose:      XRC editor, Panel class and related
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      02.12.2002
# RCS-ID:       $Id$

from xxx import *                       # xxx imports globals and params
from undo import *
from wxPython.html import wxHtmlWindow

# Properties panel containing notebook
class Panel(wxNotebook):
    def __init__(self, parent, id = -1):
        if wxPlatform != '__WXMAC__':   # some problems with this style on macs
            wxNotebook.__init__(self, parent, id, style=wxNB_BOTTOM)
        else:
            wxNotebook.__init__(self, parent, id)
        global panel
        g.panel = panel = self
        self.modified = False

        # Set common button size for parameter buttons
        bTmp = wxButton(self, -1, '')
        import params
        params.buttonSize = (self.DLG_SZE(buttonSize)[0], bTmp.GetSize()[1])
        bTmp.Destroy()
        del bTmp

        # List of child windows
        self.pages = []
        # Create scrolled windows for pages
        self.page1 = wxScrolledWindow(self, -1)
        sizer = wxBoxSizer()
        sizer.Add(wxBoxSizer())         # dummy sizer
        self.page1.SetAutoLayout(True)
        self.page1.SetSizer(sizer)
        self.AddPage(self.page1, 'Properties')
        # Second page
        self.page2 = wxScrolledWindow(self, -1)
        self.page2.Hide()
        sizer = wxBoxSizer()
        sizer.Add(wxBoxSizer())         # dummy sizer
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
            if isinstance(w, ParamPage):
                if w.IsShown():
                    w.Hide()
            else:
                w.Destroy()
        topSizer.Remove(sizer)
        # Create new windows
        sizer = wxBoxSizer(wxVERTICAL)
        # Special case - resize html window
        if g.conf.panic:
            topSizer.Add(sizer, 1, wxEXPAND)
        else:
            topSizer.Add(sizer, 0, wxALL, 5)
        return sizer

    def SetData(self, xxx):
        self.pages = []
        # First page
        # Remove current objects and sizer
        sizer = self.ResetPage(self.page1)
        if not xxx or (not xxx.allParams and not xxx.hasName):
            if g.tree.selection:
                sizer.Add(wxStaticText(self.page1, -1, 'This item has no properties.'))
            else:                       # nothing selected
                # If first time, show some help
                if g.conf.panic:
                    html = wxHtmlWindow(self.page1, -1, wxDefaultPosition,
                                        wxDefaultSize, wxSUNKEN_BORDER)
                    html.SetPage(g.helpText)
                    sizer.Add(html, 1, wxEXPAND)
                    g.conf.panic = False
                else:
                    sizer.Add(wxStaticText(self.page1, -1, 'Select a tree item.'))
        else:
            g.currentXXX = xxx.treeObject()
            # Normal or SizerItem page
            isGBSizerItem = isinstance(xxx.parent, xxxGridBagSizer)
            cacheID = (xxx.__class__, isGBSizerItem)            
            try:
                page = self.pageCache[cacheID]
                page.box.SetLabel(xxx.panelName())
                page.Show()
            except KeyError:
                page = PropPage(self.page1, xxx.panelName(), xxx)
                self.pageCache[cacheID] = page
            page.SetValues(xxx)
            self.pages.append(page)
            sizer.Add(page, 1, wxEXPAND)
            if xxx.hasChild:
                # Special label for child objects - they may have different GUI
                cacheID = (xxx.child.__class__, xxx.__class__)
                try:
                    page = self.pageCache[cacheID]
                    page.box.SetLabel(xxx.child.panelName())
                    page.Show()
                except KeyError:
                    page = PropPage(self.page1, xxx.child.panelName(), xxx.child)
                    self.pageCache[cacheID] = page
                page.SetValues(xxx.child)
                self.pages.append(page)
                sizer.Add(page, 0, wxEXPAND | wxTOP, 5)
        self.page1.Layout()
        size = self.page1.GetSizer().GetMinSize()
        self.page1.SetScrollbars(1, 1, size.width, size.height, 0, 0, True)

        # Second page
        # Create if does not exist
        if xxx and xxx.treeObject().hasStyle:
            xxx = xxx.treeObject()
            # Simplest case: set data if class is the same
            sizer = self.ResetPage(self.page2)
            try:
                page = self.stylePageCache[xxx.__class__]
                page.Show()
            except KeyError:
                page = StylePage(self.page2, xxx.className + ' style', xxx)
                self.stylePageCache[xxx.__class__] = page
            page.SetValues(xxx)
            self.pages.append(page)
            sizer.Add(page, 0, wxEXPAND)
            # Add page if not exists
            if not self.GetPageCount() == 2:
                self.AddPage(self.page2, 'Style')
            self.page2.Layout()
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
        if not self.modified and value:
           g.undoMan.RegisterUndo(UndoEdit())
        self.modified = value
        
    def Apply(self):
        for p in self.pages: p.Apply()

################################################################################

# General class for notebook pages
class ParamPage(wxPanel):
    def __init__(self, parent, xxx):
        wxPanel.__init__(self, parent, -1)
        self.SetBackgroundColour(parent.GetBackgroundColour())
        self.SetForegroundColour(parent.GetForegroundColour())
        self.xxx = xxx
        # Register event handlers
        for id in paramIDs.values():
            EVT_CHECKBOX(self, id, self.OnCheckParams)
        self.checks = {}
        self.controls = {}              # save python objects
        self.controlName = None
        
    def OnCheckParams(self, evt):
        xxx = self.xxx
        param = evt.GetEventObject().GetName()
        w = self.controls[param]
        w.Enable(True)
        objElem = xxx.element
        if evt.IsChecked():
            # Ad  new text node in order of allParams
            w.SetValue('')              # set empty (default) value
            w.SetModified()             # mark as changed
            elem = g.tree.dom.createElement(param)
            # Some classes are special
            if param == 'font':
                xxx.params[param] = xxxParamFont(xxx.element, elem)
            elif param in xxxObject.bitmapTags:
                xxx.params[param] = xxxParamBitmap(elem)
            else:
                xxx.params[param] = xxxParam(elem)
            # Find place to put new element: first present element after param
            found = False
            paramStyles = xxx.allParams + xxx.styles
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
            w.modified = False          # mark as not changed
            w.Enable(False)
        # Set modified flag (provokes undo storing is necessary)
        panel.SetModified(True)
    def Apply(self):
        xxx = self.xxx
        if self.controlName:
            name = self.controlName.GetValue()
            if xxx.name != name:
                xxx.name = name
                xxx.element.setAttribute('name', name)
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
        self.origControls = map(lambda i: (i[0], i[1].GetValue(), i[1].IsEnabled()),
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
            if e: self.controls[k].modified = True
        if self.controlName:
            self.controlName.SetValue(state[2])

################################################################################

LABEL_WIDTH = 125

# Panel for displaying properties
class PropPage(ParamPage):
    def __init__(self, parent, label, xxx):
        ParamPage.__init__(self, parent, xxx)
        self.box = wxStaticBox(self, -1, label)
        self.box.SetFont(g.labelFont())
        topSizer = wxStaticBoxSizer(self.box, wxVERTICAL)
        sizer = wxFlexGridSizer(len(xxx.allParams), 2, 1, 1)
        sizer.AddGrowableCol(1)
        if xxx.hasName:
            label = wxStaticText(self, -1, 'XML ID:', size=(LABEL_WIDTH,-1))
            control = ParamText(self, 'XML_name', 200)
            sizer.AddMany([ (label, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL | wxBOTTOM | wxGROW, 5) ])
            self.controlName = control
        for param in xxx.allParams:
            present = xxx.params.has_key(param)
            if param in xxx.required:
                label = wxStaticText(self, paramIDs[param], param + ':',
                                     size = (LABEL_WIDTH,-1), name = param)
            else:
                # Notebook has one very loooooong parameter
                if param == 'usenotebooksizer': sParam = 'usesizer:'
                else: sParam = param + ':'
                label = wxCheckBox(self, paramIDs[param], sParam,
                                   size = (LABEL_WIDTH,-1), name = param)
                self.checks[param] = label
            try:
                typeClass = xxx.paramDict[param]
            except KeyError:
                try:
                    # Standart type
                    typeClass = paramDict[param]
                except KeyError:
                    # Default
                    typeClass = ParamText
            control = typeClass(self, param)
            control.Enable(present)
            sizer.AddMany([ (label, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL | wxGROW) ])
            self.controls[param] = control
        topSizer.Add(sizer, 1, wxALL | wxEXPAND, 3)
        self.SetAutoLayout(True)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
    def SetValues(self, xxx):
        self.xxx = xxx
        self.origChecks = []
        self.origControls = []
        # Set values, checkboxes to False, disable defaults
        if xxx.hasName:
            self.controlName.SetValue(xxx.name)
            self.origName = xxx.name
        for param in xxx.allParams:
            w = self.controls[param]
            w.modified = False
            try:
                value = xxx.params[param].value()
                w.Enable(True)
                w.SetValue(value)
                if not param in xxx.required:
                    self.checks[param].SetValue(True)
                    self.origChecks.append((param, True))
                self.origControls.append((param, value, True))
            except KeyError:
                self.checks[param].SetValue(False)
                w.SetValue('')
                w.Enable(False)
                self.origChecks.append((param, False))
                self.origControls.append((param, '', False))

################################################################################

# Style notebook page
class StylePage(ParamPage):
    def __init__(self, parent, label, xxx):
        ParamPage.__init__(self, parent, xxx)
        box = wxStaticBox(self, -1, label)
        box.SetFont(g.labelFont())
        topSizer = wxStaticBoxSizer(box, wxVERTICAL)
        sizer = wxFlexGridSizer(len(xxx.styles), 2, 1, 1)
        sizer.AddGrowableCol(1)
        for param in xxx.styles:
            present = xxx.params.has_key(param)
            check = wxCheckBox(self, paramIDs[param],
                               param + ':', size = (LABEL_WIDTH,-1), name = param)
            check.SetValue(present)
            control = paramDict[param](self, name = param)
            control.Enable(present)
            sizer.AddMany([ (check, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL | wxGROW) ])
            self.checks[param] = check
            self.controls[param] = control
        topSizer.Add(sizer, 1, wxALL | wxEXPAND, 3)
        self.SetAutoLayout(True)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
    # Set data for a cahced page
    def SetValues(self, xxx):
        self.xxx = xxx
        self.origChecks = []
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
            self.origChecks.append((param, present))
            self.origControls.append((param, value, present))

