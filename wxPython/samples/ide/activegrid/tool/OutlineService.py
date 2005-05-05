#----------------------------------------------------------------------------
# Name:         OutlineService.py
# Purpose:      Outline View Service for pydocview
#
# Author:       Morgan Hua
#
# Created:      8/3/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.docview
import wx.lib.pydocview
import Service
_ = wx.GetTranslation


#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------
SORT_NONE = 0
SORT_ASC = 1
SORT_DESC = 2

class OutlineView(Service.ServiceView):
    """ Reusable Outline View for any document.
        As a default, it uses a modified tree control (OutlineTreeCtrl) that allows sorting.
        Subclass OutlineTreeCtrl to customize the tree control and call SetTreeCtrl to install a customized tree control.
        When an item is selected, the document view is called back (with DoSelectCallback) to highlight and display the corresponding item in the document view.
    """

    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, service):
        Service.ServiceView.__init__(self, service)
        self._actionOnSelect = True


    def _CreateControl(self, parent, id):
        treeCtrl = OutlineTreeCtrl(parent, id)
        wx.EVT_TREE_SEL_CHANGED(treeCtrl, treeCtrl.GetId(), self.DoSelection)
        wx.EVT_SET_FOCUS(treeCtrl, self.DoSelection)
        wx.EVT_ENTER_WINDOW(treeCtrl, treeCtrl.CallDoLoadOutlineCallback)
        wx.EVT_RIGHT_DOWN(treeCtrl, self.OnRightClick)

        return treeCtrl


    #----------------------------------------------------------------------------
    # Service specific methods
    #----------------------------------------------------------------------------

    def OnRightClick(self, event):
        menu = wx.Menu()

        menu.AppendRadioItem(OutlineService.SORT_NONE, _("Unsorted"), _("Display items in original order"))
        menu.AppendRadioItem(OutlineService.SORT_ASC, _("Sort A-Z"), _("Display items in ascending order"))
        menu.AppendRadioItem(OutlineService.SORT_DESC, _("Sort Z-A"), _("Display items in descending order"))

        config = wx.ConfigBase_Get()
        sort = config.ReadInt("OutlineSort", SORT_NONE)
        if sort == SORT_NONE:
            menu.Check(OutlineService.SORT_NONE, True)
        elif sort == SORT_ASC:
            menu.Check(OutlineService.SORT_ASC, True)
        elif sort == SORT_DESC:
            menu.Check(OutlineService.SORT_DESC, True)

        self.GetControl().PopupMenu(menu, event.GetPosition())
        menu.Destroy()


    #----------------------------------------------------------------------------
    # Tree Methods
    #----------------------------------------------------------------------------

    def DoSelection(self, event):
        if not self._actionOnSelect:
            return
        item = self.GetControl().GetSelection()
        if item:
            self.GetControl().CallDoSelectCallback(item)


    def ResumeActionOnSelect(self):
        self._actionOnSelect = True


    def StopActionOnSelect(self):
        self._actionOnSelect = False


    def SetTreeCtrl(self, tree):
        self.SetControl(tree)
        wx.EVT_TREE_SEL_CHANGED(self.GetControl(), self.GetControl().GetId(), self.DoSelection)
        wx.EVT_ENTER_WINDOW(self.GetControl(), treeCtrl.CallDoLoadOutlineCallback)
        wx.EVT_RIGHT_DOWN(self.GetControl(), self.OnRightClick)


    def GetTreeCtrl(self):
        return self.GetControl()


    def OnSort(self, sortOrder):
        treeCtrl = self.GetControl()
        treeCtrl.SetSortOrder(sortOrder)
        treeCtrl.SortAllChildren(treeCtrl.GetRootItem())


    def ClearTreeCtrl(self):
        if self.GetControl():
            self.GetControl().DeleteAllItems()


    def GetExpansionState(self):
        expanded = []

        treeCtrl = self.GetControl()
        if not treeCtrl:
            return expanded

        parentItem = treeCtrl.GetRootItem()

        if not parentItem:
            return expanded

        if not treeCtrl.IsExpanded(parentItem):
            return expanded

        expanded.append(treeCtrl.GetItemText(parentItem))

        (child, cookie) = treeCtrl.GetFirstChild(parentItem)
        while child.IsOk():
            if treeCtrl.IsExpanded(child):
                expanded.append(treeCtrl.GetItemText(child))
            (child, cookie) = treeCtrl.GetNextChild(parentItem, cookie)
        return expanded


    def SetExpansionState(self, expanded):
        if not expanded or len(expanded) == 0:
            return

        treeCtrl = self.GetControl()
        parentItem = treeCtrl.GetRootItem()
        if expanded[0] != treeCtrl.GetItemText(parentItem):
            return

        (child, cookie) = treeCtrl.GetFirstChild(parentItem)
        while child.IsOk():
            if treeCtrl.GetItemText(child) in expanded:
                treeCtrl.Expand(child)
            (child, cookie) = treeCtrl.GetNextChild(parentItem, cookie)

        if parentItem:
            treeCtrl.EnsureVisible(parentItem)


class OutlineTreeCtrl(wx.TreeCtrl):
    """ Default Tree Control Class for OutlineView.
        This class has the added functionality of sorting by the labels
    """


    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    ORIG_ORDER = 0
    VIEW = 1
    CALLBACKDATA = 2


    #----------------------------------------------------------------------------
    # Overridden Methods
    #----------------------------------------------------------------------------

    def __init__(self, parent, id, style=wx.TR_HAS_BUTTONS|wx.TR_DEFAULT_STYLE):
        wx.TreeCtrl.__init__(self, parent, id, style = style)
        self._origOrderIndex = 0
        self._sortOrder = SORT_NONE


    def DeleteAllItems(self):
        self._origOrderIndex = 0
        wx.TreeCtrl.DeleteAllItems(self)


    #----------------------------------------------------------------------------
    # Sort Methods
    #----------------------------------------------------------------------------

    def SetSortOrder(self, sortOrder = SORT_NONE):
        """ Sort Order constants are defined at top of file """
        self._sortOrder = sortOrder


    def OnCompareItems(self, item1, item2):
        if self._sortOrder == SORT_ASC:
            return cmp(self.GetItemText(item1).lower(), self.GetItemText(item2).lower())  # sort A-Z
        elif self._sortOrder == SORT_DESC:
            return cmp(self.GetItemText(item2).lower(), self.GetItemText(item1).lower())  # sort Z-A
        else:
            return (self.GetPyData(item1)[self.ORIG_ORDER] > self.GetPyData(item2)[self.ORIG_ORDER]) # unsorted


    def SortAllChildren(self, parentItem):
        if parentItem and self.GetChildrenCount(parentItem, False):
            self.SortChildren(parentItem)
            (child, cookie) = self.GetFirstChild(parentItem)
            while child.IsOk():
                self.SortAllChildren(child)
                (child, cookie) = self.GetNextChild(parentItem, cookie)


    #----------------------------------------------------------------------------
    # Select Callback Methods
    #----------------------------------------------------------------------------

    def CallDoSelectCallback(self, item):
        """ Invoke the DoSelectCallback of the given view to highlight text in the document view
        """
        data = self.GetPyData(item)
        if not data:
            return

        view = data[self.VIEW]
        cbdata = data[self.CALLBACKDATA]
        if view:
            view.DoSelectCallback(cbdata)


    def SelectClosestItem(self, position):
        tree = self
        distances = []
        items = []
        self.FindDistanceToTreeItems(tree.GetRootItem(), position, distances, items)
        mindist = 1000000
        mindex = -1
        for index in range(0, len(distances)):
            if distances[index] <= mindist:
                mindist = distances[index]
                mindex = index
        if mindex != -1:
            item = items[mindex]
            self.EnsureVisible(item)
            os_view = wx.GetApp().GetService(OutlineService).GetView()
            if os_view:
               os_view.StopActionOnSelect()
            self.SelectItem(item)
            if os_view:
               os_view.ResumeActionOnSelect()


    def FindDistanceToTreeItems(self, item, position, distances, items):
        data = self.GetPyData(item)
        this_dist = 1000000
        if data and data[2]:
            positionTuple = data[2]
            if position >= positionTuple[1]:
                items.append(item)
                distances.append(position - positionTuple[1])

        if self.ItemHasChildren(item):
            child, cookie = self.GetFirstChild(item)
            while child and child.IsOk():
                self.FindDistanceToTreeItems(child, position, distances, items)
                child, cookie = self.GetNextChild(item, cookie)
        return False


    def SetDoSelectCallback(self, item, view, callbackdata):
        """ When an item in the outline view is selected,
        a method is called to select the respective text in the document view.
        The view must define the method DoSelectCallback(self, data) in order for this to work
        """
        self.SetPyData(item, (self._origOrderIndex, view, callbackdata))
        self._origOrderIndex = self._origOrderIndex + 1


    def CallDoLoadOutlineCallback(self, event):
        """ Invoke the DoLoadOutlineCallback
        """
        rootItem = self.GetRootItem()
        if rootItem:
            data = self.GetPyData(rootItem)
            if data:
                view = data[self.VIEW]
                if view and view.DoLoadOutlineCallback():
                    self.SortAllChildren(self.GetRootItem())


    def GetCallbackView(self):
        rootItem = self.GetRootItem()
        if rootItem:
            return self.GetPyData(rootItem)[self.VIEW]
        else:
            return None


class OutlineService(Service.Service):


    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    SHOW_WINDOW = wx.NewId()  # keep this line for each subclass, need unique ID for each Service
    SORT = wx.NewId()
    SORT_ASC = wx.NewId()
    SORT_DESC = wx.NewId()
    SORT_NONE = wx.NewId()


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, serviceName, embeddedWindowLocation = wx.lib.pydocview.EMBEDDED_WINDOW_BOTTOM):
        Service.Service.__init__(self, serviceName, embeddedWindowLocation)
        self._validTemplates = []


    def _CreateView(self):
        return OutlineView(self)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        Service.Service.InstallControls(self, frame, menuBar, toolBar, statusBar, document)

        wx.EVT_MENU(frame, OutlineService.SORT_ASC, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, OutlineService.SORT_ASC, frame.ProcessUpdateUIEvent)
        wx.EVT_MENU(frame, OutlineService.SORT_DESC, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, OutlineService.SORT_DESC, frame.ProcessUpdateUIEvent)
        wx.EVT_MENU(frame, OutlineService.SORT_NONE, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, OutlineService.SORT_NONE, frame.ProcessUpdateUIEvent)


        if wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            return True

        viewMenu = menuBar.GetMenu(menuBar.FindMenu(_("&View")))
        self._outlineSortMenu = wx.Menu()
        self._outlineSortMenu.AppendRadioItem(OutlineService.SORT_NONE, _("Unsorted"), _("Display items in original order"))
        self._outlineSortMenu.AppendRadioItem(OutlineService.SORT_ASC, _("Sort A-Z"), _("Display items in ascending order"))
        self._outlineSortMenu.AppendRadioItem(OutlineService.SORT_DESC, _("Sort Z-A"), _("Display items in descending order"))
        viewMenu.AppendMenu(wx.NewId(), _("Outline Sort"), self._outlineSortMenu)

        return True


    #----------------------------------------------------------------------------
    # Event Processing Methods
    #----------------------------------------------------------------------------

    def ProcessEvent(self, event):
        if Service.Service.ProcessEvent(self, event):
            return True

        id = event.GetId()
        if id == OutlineService.SORT_ASC:
            self.OnSort(event)
            return True
        elif id == OutlineService.SORT_DESC:
            self.OnSort(event)
            return True
        elif id == OutlineService.SORT_NONE:
            self.OnSort(event)
            return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        if Service.Service.ProcessUpdateUIEvent(self, event):
            return True

        id = event.GetId()
        if id == OutlineService.SORT_ASC:
            event.Enable(True)

            config = wx.ConfigBase_Get()
            sort = config.ReadInt("OutlineSort", SORT_NONE)
            if sort == SORT_ASC:
                self._outlineSortMenu.Check(OutlineService.SORT_ASC, True)
            else:
                self._outlineSortMenu.Check(OutlineService.SORT_ASC, False)

            return True
        elif id == OutlineService.SORT_DESC:
            event.Enable(True)

            config = wx.ConfigBase_Get()
            sort = config.ReadInt("OutlineSort", SORT_NONE)
            if sort == SORT_DESC:
                self._outlineSortMenu.Check(OutlineService.SORT_DESC, True)
            else:
                self._outlineSortMenu.Check(OutlineService.SORT_DESC, False)

            return True
        elif id == OutlineService.SORT_NONE:
            event.Enable(True)

            config = wx.ConfigBase_Get()
            sort = config.ReadInt("OutlineSort", SORT_NONE)
            if sort == SORT_NONE:
                self._outlineSortMenu.Check(OutlineService.SORT_NONE, True)
            else:
                self._outlineSortMenu.Check(OutlineService.SORT_NONE, False)

            return True
        else:
            return False


    def OnSort(self, event):
        id = event.GetId()
        if id == OutlineService.SORT_ASC:
            wx.ConfigBase_Get().WriteInt("OutlineSort", SORT_ASC)
            self.GetView().OnSort(SORT_ASC)
            return True
        elif id == OutlineService.SORT_DESC:
            wx.ConfigBase_Get().WriteInt("OutlineSort", SORT_DESC)
            self.GetView().OnSort(SORT_DESC)
            return True
        elif id == OutlineService.SORT_NONE:
            wx.ConfigBase_Get().WriteInt("OutlineSort", SORT_NONE)
            self.GetView().OnSort(SORT_NONE)
            return True


    #----------------------------------------------------------------------------
    # Service specific methods
    #----------------------------------------------------------------------------

    def LoadOutline(self, view, position=-1, force=False):
        if not self.GetView():
            return

        if hasattr(view, "DoLoadOutlineCallback"):
            self.SaveExpansionState()
            if view.DoLoadOutlineCallback(force=force):
                self.GetView().OnSort(wx.ConfigBase_Get().ReadInt("OutlineSort", SORT_NONE))
                self.LoadExpansionState()
            if position >= 0:
                self.SyncToPosition(position)


    def SyncToPosition(self, position):
        if not self.GetView():
            return
        self.GetView().GetTreeCtrl().SelectClosestItem(position)


    def OnCloseFrame(self, event):
        Service.Service.OnCloseFrame(self, event)
        self.SaveExpansionState(clear = True)

        return True


    def SaveExpansionState(self, clear = False):
        if clear:
            expanded = []
        elif self.GetView():
            expanded = self.GetView().GetExpansionState()
        wx.ConfigBase_Get().Write("OutlineLastExpanded", expanded.__repr__())


    def LoadExpansionState(self):
        expanded = wx.ConfigBase_Get().Read("OutlineLastExpanded")
        if expanded:
            self.GetView().SetExpansionState(eval(expanded))


    #----------------------------------------------------------------------------
    # Timer Methods
    #----------------------------------------------------------------------------

    def StartBackgroundTimer(self):
        self._timer = wx.PyTimer(self.DoBackgroundRefresh)
        self._timer.Start(250)


    def DoBackgroundRefresh(self):
        """ Refresh the outline view periodically """
        self._timer.Stop()
        
        foundRegisteredView = False
        if self.GetView():
            currView = wx.GetApp().GetDocumentManager().GetCurrentView()
            if currView:
                for template in self._validTemplates:
                    type = template.GetViewType()
                    if isinstance(currView, type):
                        self.LoadOutline(currView)
                        foundRegisteredView = True
                        break

            if not foundRegisteredView:
                self.GetView().ClearTreeCtrl()
                    
        self._timer.Start(1000) # 1 second interval


    def AddTemplateForBackgroundHandler(self, template):
        self._validTemplates.append(template)


    def GetTemplatesForBackgroundHandler(self):
        return self._validTemplates


    def RemoveTemplateForBackgroundHandler(self, template):
        self._validTemplates.remove(template)

