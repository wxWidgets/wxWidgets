#----------------------------------------------------------------------------
# Name:        wx.lib.inspection
# Purpose:     A widget inspection tool that allows easy introspection of
#              all the live widgets and sizers in an application.
#
# Author:      Robin Dunn
#
# Created:     26-Jan-2007
# RCS-ID:      $Id$
# Copyright:   (c) 2007 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

# NOTE: This class was originally based on ideas sent to the
# wxPython-users mail list by Dan Eloff.  See also
# wx.lib.mixins.inspect for a class that can be mixed-in with wx.App
# to provide Hot-Key access to the inspection tool.

import wx
import wx.py
import wx.stc
import wx.aui
import sys

#----------------------------------------------------------------------------

class InspectionTool:
    """
    The InspectionTool is a singleton that manages creating and
    showing an InspectionFrame.
    """

    # Note: This is the Borg design pattern which ensures that all
    # instances of this class are actually using the same set of
    # instance data.  See
    # http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/66531
    __shared_state = {}
    def __init__(self):
        self.__dict__ = self.__shared_state
        if not hasattr(self, 'initialized'):
            self.initialized = False

    def Init(self, pos=wx.DefaultPosition, size=wx.Size(850,700),
             config=None, locals=None, app=None):
        """
        Init is used to set some parameters that will be used later
        when the inspection tool is shown.  Suitable defaults will be
        used for all of these parameters if they are not provided.

        :param pos:   The default position to show the frame at
        :param size:  The default size of the frame
        :param config: A wx.Config object to be used to store layout
            and other info to when the inspection frame is closed.
            This info will be restored the next time the inspection
            frame is used.
        :param locals: A dictionary of names to be added to the PyCrust
            namespace.
        :param app:  A reference to the wx.App object.
        """
        self._frame = None
        self._pos = pos
        self._size = size
        self._config = config
        self._locals = locals
        self._app = app
        if not self._app:
            self._app = wx.GetApp()
        self.initialized = True
            
        
    def Show(self, selectObj=None, refreshTree=False):
        """
        Creates the inspection frame if it hasn't been already, and
        raises it if neccessary.  Pass a widget or sizer in selectObj
        to have that object be preselected in widget tree.  If
        refreshTree is True then the widget tree will be rebuilt,
        otherwise if the tree has already been built it will be left
        alone.
        """
        if not self.initialized:
            self.Init()

        parent = self._app.GetTopWindow()
        if not selectObj:
            selectObj = parent
        if not self._frame:
            self._frame = InspectionFrame( parent=parent,
                                           pos=self._pos,
                                           size=self._size,
                                           config=self._config,
                                           locals=self._locals,
                                           app=self._app)
        if selectObj:
            self._frame.SetObj(selectObj)
        if refreshTree:
            self._frame.RefreshTree()
        self._frame.Show()
        if self._frame.IsIconized():
            self._frame.Iconize(False)
        self._frame.Raise()

        
#----------------------------------------------------------------------------


class InspectionFrame(wx.Frame):
    """
    This class is the frame that holds the wxPython inspection tools.
    The toolbar and AUI splitters/floating panes are also managed
    here.  The contents of the tool windows are handled by other
    classes.
    """
    def __init__(self, wnd=None, locals=None, config=None,
                 app=None, title="wxPython Widget Inspection Tool",
                 *args, **kw):
        kw['title'] = title
        wx.Frame.__init__(self, *args, **kw)

        self.includeSizers = False
        self.started = False

        self.SetIcon(getIconIcon())
        self.MacSetMetalAppearance(True)
        self.MakeToolBar()
        panel = wx.Panel(self, size=self.GetClientSize())
        
        # tell FrameManager to manage this frame        
        self.mgr = wx.aui.AuiManager(panel,
                                     wx.aui.AUI_MGR_DEFAULT
                                     | wx.aui.AUI_MGR_TRANSPARENT_DRAG
                                     | wx.aui.AUI_MGR_ALLOW_ACTIVE_PANE)

        # make the child tools        
        self.tree = InspectionTree(panel)
        self.info = InspectionInfoPanel(panel,
                                        style=wx.NO_BORDER,
                                        )

        if not locals:
            locals = {}
        myIntroText = (
            "Python %s on %s\nNOTE: The 'obj' variable refers to the object selected in the tree."
            % (sys.version.split()[0], sys.platform))
        self.crust = wx.py.crust.Crust(panel, locals=locals,
                                       intro=myIntroText,
                                       showInterpIntro=False,
                                       style=wx.NO_BORDER,
                                       )
        self.locals = self.crust.shell.interp.locals
        self.crust.shell.interp.introText = ''
        self.locals['obj'] = self.obj = wnd
        self.locals['app'] = app
        self.locals['wx'] = wx
        wx.CallAfter(self._postStartup)

        # put the chlid tools in AUI panes
        self.mgr.AddPane(self.info,
                         wx.aui.AuiPaneInfo().Name("info").Caption("Object Info").
                         CenterPane().CaptionVisible(True).
                         CloseButton(False).MaximizeButton(True)
                         )
        self.mgr.AddPane(self.tree,
                         wx.aui.AuiPaneInfo().Name("tree").Caption("Widget Tree").
                         CaptionVisible(True).Left().Dockable(True).Floatable(True).
                         BestSize((280,200)).CloseButton(False).MaximizeButton(True)
                         )
        self.mgr.AddPane(self.crust,
                         wx.aui.AuiPaneInfo().Name("crust").Caption("PyCrust").
                         CaptionVisible(True).Bottom().Dockable(True).Floatable(True).
                         BestSize((400,200)).CloseButton(False).MaximizeButton(True)
                         )

        self.mgr.Update()

        if config is None:
            config = wx.Config('wxpyinspector')
        self.config = config
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.LoadSettings(self.config)
        self.crust.shell.lineNumbers = False
        self.crust.shell.setDisplayLineNumbers(False)
        self.crust.shell.SetMarginWidth(1, 0)
        

    def MakeToolBar(self):
        tbar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.TB_FLAT | wx.TB_TEXT | wx.NO_BORDER )
        tbar.SetToolBitmapSize((24,24))

        refreshBmp = getRefreshBitmap() 
        findWidgetBmp = getFindBitmap() 
        showSizersBmp = getShowSizersBitmap() 
        toggleFillingBmp = getShowFillingBitmap() 

        refreshTool = tbar.AddLabelTool(-1, 'Refresh', refreshBmp,
                                        shortHelp = 'Refresh widget tree')
        findWidgetTool = tbar.AddLabelTool(-1, 'Find', findWidgetBmp,
                                           shortHelp='Find new target widget.  Click here and\nthen on another widget in the app.')
        showSizersTool = tbar.AddLabelTool(-1, 'Sizers', showSizersBmp,
                                           shortHelp='Include sizers in widget tree',
                                           kind=wx.ITEM_CHECK)
        toggleFillingTool = tbar.AddLabelTool(-1, 'Filling', toggleFillingBmp,
                                              shortHelp='Show PyCrust \'filling\'',
                                              kind=wx.ITEM_CHECK)
        tbar.Realize()

        self.Bind(wx.EVT_TOOL,      self.OnRefreshTree,     refreshTool)
        self.Bind(wx.EVT_TOOL,      self.OnFindWidget,      findWidgetTool)
        self.Bind(wx.EVT_TOOL,      self.OnShowSizers,      showSizersTool)
        self.Bind(wx.EVT_TOOL,      self.OnToggleFilling,   toggleFillingTool)
        self.Bind(wx.EVT_UPDATE_UI, self.OnShowSizersUI,    showSizersTool)
        self.Bind(wx.EVT_UPDATE_UI, self.OnToggleFillingUI, toggleFillingTool)



    def _postStartup(self):
        if self.crust.ToolsShown():
            self.crust.ToggleTools()
        self.UpdateInfo()
        self.started = True


    def OnClose(self, evt):
        self.SaveSettings(self.config)
        evt.Skip()
        

    def UpdateInfo(self):
        self.info.Update(self.obj)


    def SetObj(self, obj):
        if self.obj is obj:
            return
        self.locals['obj'] = self.obj = obj
        self.UpdateInfo()
        if not self.tree.built:
            self.tree.BuildTree(obj, includeSizers=self.includeSizers)
        else:
            self.tree.SelectObj(obj)


    def RefreshTree(self):
        self.tree.BuildTree(self.obj, includeSizers=self.includeSizers)


    def OnRefreshTree(self, evt):
        self.RefreshTree()
        self.UpdateInfo()
        

    def OnFindWidget(self, evt):
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_MOUSE_CAPTURE_LOST, self.OnCaptureLost)
        self.CaptureMouse()
        self.finding = wx.BusyInfo("Click on any widget in the app...")


    def OnCaptureLost(self, evt):
        self.Unbind(wx.EVT_LEFT_DOWN)
        self.Unbind(wx.EVT_MOUSE_CAPTURE_LOST)
        del self.finding

    def OnLeftDown(self, evt):
        self.ReleaseMouse()
        wnd = wx.FindWindowAtPointer()
        if wnd is not None:
            self.SetObj(wnd)
        else:
            wx.Bell()
        self.OnCaptureLost(evt)


    def OnShowSizers(self, evt):
        self.includeSizers = not self.includeSizers
        self.RefreshTree()


    def OnToggleFilling(self, evt):
        self.crust.ToggleTools()


    def OnShowSizersUI(self, evt):
        evt.Check(self.includeSizers)


    def OnToggleFillingUI(self, evt):
        if self.started:
            evt.Check(self.crust.ToolsShown())


    def LoadSettings(self, config):
        self.crust.LoadSettings(config)

        pos  = wx.Point(config.ReadInt('Window/PosX', -1),
                        config.ReadInt('Window/PosY', -1))
                        
        size = wx.Size(config.ReadInt('Window/Width', -1),
                       config.ReadInt('Window/Height', -1))
        self.SetSize(size)
        self.Move(pos)

        perspective = config.Read('perspective', '')
        if perspective:
            self.mgr.LoadPerspective(perspective)
        self.includeSizers = config.ReadBool('includeSizers', False)
            

    def SaveSettings(self, config):
        self.crust.SaveSettings(config)

        if not self.IsIconized() and not self.IsMaximized():
            w, h = self.GetSize()
            config.WriteInt('Window/Width', w)
            config.WriteInt('Window/Height', h)
            
            px, py = self.GetPosition()
            config.WriteInt('Window/PosX', px)
            config.WriteInt('Window/PosY', py)

        perspective = self.mgr.SavePerspective()
        config.Write('perspective', perspective)
        config.WriteBool('includeSizers', self.includeSizers)
        
#---------------------------------------------------------------------------

# should inspection frame (and children) be includeed in the tree?
INCLUDE_INSPECTOR = True

class InspectionTree(wx.TreeCtrl):
    """
    All of the widgets in the app, and optionally their sizers, are
    loaded into this tree.
    """
    def __init__(self, *args, **kw):
        #s = kw.get('style', 0)
        #kw['style'] = s | wx.TR_DEFAULT_STYLE | wx.TR_HIDE_ROOT
        wx.TreeCtrl.__init__(self, *args, **kw)
        self.roots = []
        self.built = False
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelectionChanged)
        self.toolFrame = wx.GetTopLevelParent(self)
        if 'wxMac' in wx.PlatformInfo:
            self.SetWindowVariant(wx.WINDOW_VARIANT_SMALL)
        

    def BuildTree(self, startWidget, includeSizers=False):
        if self.GetCount():
            self.DeleteAllItems()
            self.roots = []
            self.built = False

        realRoot = self.AddRoot('Top-level Windows')

        for w in wx.GetTopLevelWindows():
            if w is wx.GetTopLevelParent(self) and not INCLUDE_INSPECTOR:
                continue
            root  = self._AddWidget(realRoot, w, includeSizers)
            self.roots.append(root)

        # Expand the subtree containing the startWidget, and select it.
        if not startWidget or not isinstance(startWidget, wx.Window):
            startWidget = wx.GetApp().GetTopWindow()
        top = wx.GetTopLevelParent(startWidget)
        topItem = self.FindWidgetItem(top)
        if topItem:
            self.ExpandAllChildren(topItem)
        self.SelectObj(startWidget)
        self.built = True


    def _AddWidget(self, parentItem, widget, includeSizers):
        text = self.GetTextForWidget(widget)
        item = self.AppendItem(parentItem, text)
        self.SetItemPyData(item, widget)

        # Add the sizer and widgets in the sizer, if we're showing them
        widgetsInSizer = []
        if includeSizers and widget.GetSizer() is not None:
            widgetsInSizer = self._AddSizer(item, widget.GetSizer())

        # Add any children not in the sizer, or all children if we're
        # not showing the sizers
        for child in widget.GetChildren():
            if not child in widgetsInSizer and not child.IsTopLevel():
                self._AddWidget(item, child, includeSizers)

        return item


    def _AddSizer(self, parentItem, sizer):
        widgets = []
        text = self.GetTextForSizer(sizer)
        item = self.AppendItem(parentItem, text)
        self.SetItemPyData(item, sizer)
        self.SetItemTextColour(item, "blue")

        for si in sizer.GetChildren():
            if si.IsWindow():
                w = si.GetWindow()
                self._AddWidget(item, w, True)
                widgets.append(w)
            elif si.IsSizer():
                widgets += self._AddSizer(item, si.GetSizer())
            else:
                i = self.AppendItem(item, "Spacer")
                self.SetItemPyData(i, si)
                self.SetItemTextColour(i, "blue")
        return widgets
    

    def FindWidgetItem(self, widget):
        """
        Find the tree item for a widget.
        """
        for item in self.roots:
            found = self._FindWidgetItem(widget, item)
            if found:
                return found
        return None

    def _FindWidgetItem(self, widget, item):
        if self.GetItemPyData(item) is widget:
            return item
        child, cookie = self.GetFirstChild(item)
        while child:
            found = self._FindWidgetItem(widget, child)
            if found:
                return found
            child, cookie = self.GetNextChild(item, cookie)
        return None


    def GetTextForWidget(self, widget):
        """
        Returns the string to be used in the tree for a widget
        """
        return "%s (\"%s\")" % (widget.__class__.__name__, widget.GetName())

    def GetTextForSizer(self, sizer):
        """
        Returns the string to be used in the tree for a sizer
        """
        return "%s" % sizer.__class__.__name__


    def SelectObj(self, obj):
        item = self.FindWidgetItem(obj)
        if item:
            self.EnsureVisible(item)
            self.SelectItem(item)


    def OnSelectionChanged(self, evt):
        obj = self.GetItemPyData(evt.GetItem())
        self.toolFrame.SetObj(obj)


#---------------------------------------------------------------------------

class InspectionInfoPanel(wx.stc.StyledTextCtrl):
    """
    Used to display information about the currently selected items.
    Currently just a read-only wx.stc.StyledTextCtrl with some plain
    text.  Should probably add some styles to make things easier to
    read.
    """
    def __init__(self, *args, **kw):
        wx.stc.StyledTextCtrl.__init__(self, *args, **kw)

        from wx.py.editwindow import FACES
        self.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,
                          "face:%(mono)s,size:%(size)d,back:%(backcol)s" % FACES)
        self.StyleClearAll()
        self.SetReadOnly(True)
        self.SetMarginType(1, 0)
        self.SetMarginWidth(1, 0)
        self.SetSelForeground(True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT))
        self.SetSelBackground(True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT))
        

    def Update(self, obj):
        st = []
        if not obj:
            st.append("Item is None or has been destroyed.")

        elif isinstance(obj, wx.Window):
            st += self.FmtWidget(obj)
            
        elif isinstance(obj, wx.Sizer):
            st += self.FmtSizer(obj)

        elif isinstance(obj, wx.SizerItem):
            st += self.FmtSizerItem(obj)

        self.SetReadOnly(False)
        self.SetText('\n'.join(st))
        self.SetReadOnly(True)


    def Fmt(self, name, value):
        if isinstance(value, (str, unicode)):
            return "    %s = '%s'" % (name, value)
        else:
            return "    %s = %s" % (name, value)


    def FmtWidget(self, obj):
        st = ["Widget:"]
        st.append(self.Fmt('name',       obj.GetName()))
        st.append(self.Fmt('class',      obj.__class__))
        st.append(self.Fmt('bases',      obj.__class__.__bases__))
        st.append(self.Fmt('id',         obj.GetId()))
        st.append(self.Fmt('style',      obj.GetWindowStyle()))
        st.append(self.Fmt('pos',        obj.GetPosition()))
        st.append(self.Fmt('size',       obj.GetSize()))
        st.append(self.Fmt('minsize',    obj.GetMinSize()))
        st.append(self.Fmt('bestsize',   obj.GetBestSize()))
        st.append(self.Fmt('client size',obj.GetClientSize()))
        st.append(self.Fmt('IsEnabled',  obj.IsEnabled()))
        st.append(self.Fmt('IsShown',    obj.IsShown()))
        st.append(self.Fmt('fg color',   obj.GetForegroundColour()))
        st.append(self.Fmt('bg color',   obj.GetBackgroundColour()))
        st.append(self.Fmt('label',      obj.GetLabel()))
        if hasattr(obj, 'GetTitle'):
            st.append(self.Fmt('title',      obj.GetTitle()))
        if hasattr(obj, 'GetValue'):
            st.append(self.Fmt('value',      obj.GetValue()))
        if obj.GetContainingSizer() is not None:
            st.append('')
            sizer = obj.GetContainingSizer()
            st += self.FmtSizerItem(sizer.GetItem(obj))
        return st


    def FmtSizerItem(self, obj):
        st = ['SizerItem:']
        st.append(self.Fmt('proportion', obj.GetProportion()))
        st.append(self.Fmt('flag',
                           FlagsFormatter(itemFlags, obj.GetFlag())))
        st.append(self.Fmt('border',     obj.GetBorder()))
        st.append(self.Fmt('pos',        obj.GetPosition()))
        st.append(self.Fmt('size',       obj.GetSize()))
        st.append(self.Fmt('minsize',    obj.GetMinSize()))
        st.append(self.Fmt('ratio',      obj.GetRatio()))
        st.append(self.Fmt('IsWindow',   obj.IsWindow()))
        st.append(self.Fmt('IsSizer',    obj.IsSizer()))
        st.append(self.Fmt('IsSpacer',   obj.IsSpacer()))
        st.append(self.Fmt('IsShown',    obj.IsShown()))
        if isinstance(obj, wx.GBSizerItem):
            st.append(self.Fmt('cellpos',    obj.GetPos()))
            st.append(self.Fmt('cellspan',   obj.GetSpan()))
            st.append(self.Fmt('endpos',     obj.GetEndPos()))
        return st


    def FmtSizer(self, obj):
        st = ['Sizer:']
        st.append(self.Fmt('class',      obj.__class__))
        st.append(self.Fmt('pos',        obj.GetPosition()))
        st.append(self.Fmt('size',       obj.GetSize()))
        st.append(self.Fmt('minsize',    obj.GetMinSize()))
        if isinstance(obj, wx.BoxSizer):
            st.append(self.Fmt('orientation',
                               FlagsFormatter(orientFlags, obj.GetOrientation())))
        if isinstance(obj, wx.GridSizer):
            st.append(self.Fmt('cols', obj.GetCols()))
            st.append(self.Fmt('rows', obj.GetRows()))
            st.append(self.Fmt('vgap', obj.GetVGap()))
            st.append(self.Fmt('hgap', obj.GetHGap()))
        if isinstance(obj, wx.FlexGridSizer):
            st.append(self.Fmt('rowheights', obj.GetRowHeights()))
            st.append(self.Fmt('colwidths', obj.GetColWidths()))
            st.append(self.Fmt('flexdir',
                               FlagsFormatter(orientFlags, obj.GetFlexibleDirection())))
            st.append(self.Fmt('nonflexmode',
                               FlagsFormatter(flexmodeFlags, obj.GetNonFlexibleGrowMode())))
        if isinstance(obj, wx.GridBagSizer):
            st.append(self.Fmt('emptycell', obj.GetEmptyCellSize()))
            
        if obj.GetContainingWindow():
            si = obj.GetContainingWindow().GetSizer().GetItem(obj)
            if si:
                st.append('')
                st += self.FmtSizerItem(si)
        return st


class FlagsFormatter(object):
    def __init__(self, d, val):
        self.d = d
        self.val = val
        
    def __str__(self):
        st = []
        for k in self.d.keys():
            if self.val & k:
                st.append(self.d[k])
        if st:
            return '|'.join(st)
        else:
            return '0'

orientFlags = {
    wx.HORIZONTAL : 'wx.HORIZONTAL',
    wx.VERTICAL : 'wx.VERTICAL',
    }

itemFlags = {
    wx.TOP : 'wx.TOP',
    wx.BOTTOM : 'wx.BOTTOM',
    wx.LEFT : 'wx.LEFT',
    wx.RIGHT : 'wx.RIGHT',
#    wx.ALL : 'wx.ALL',
    wx.EXPAND : 'wx.EXPAND',
#    wx.GROW : 'wx.GROW',
    wx.SHAPED : 'wx.SHAPED',
    wx.STRETCH_NOT : 'wx.STRETCH_NOT',
    wx.ALIGN_CENTER : 'wx.ALIGN_CENTER',
    wx.ALIGN_LEFT : 'wx.ALIGN_LEFT',
    wx.ALIGN_RIGHT : 'wx.ALIGN_RIGHT',
    wx.ALIGN_TOP : 'wx.ALIGN_TOP',
    wx.ALIGN_BOTTOM : 'wx.ALIGN_BOTTOM',
    wx.ALIGN_CENTER_VERTICAL : 'wx.ALIGN_CENTER_VERTICAL',
    wx.ALIGN_CENTER_HORIZONTAL : 'wx.ALIGN_CENTER_HORIZONTAL',
    wx.ADJUST_MINSIZE : 'wx.ADJUST_MINSIZE',
    wx.FIXED_MINSIZE : 'wx.FIXED_MINSIZE',
    }

flexmodeFlags = {
    wx.FLEX_GROWMODE_NONE : 'wx.FLEX_GROWMODE_NONE',
    wx.FLEX_GROWMODE_SPECIFIED : 'wx.FLEX_GROWMODE_SPECIFIED',
    wx.FLEX_GROWMODE_ALL : 'wx.FLEX_GROWMODE_ALL',
    }

#---------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO


def getRefreshData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\x00\x00\x00\x18\x08\x06\
\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x05\xe8IDATH\x89\x9d\x96]h\x94\xd9\x19\xc7\x7f\xefG21\x93\x99Il\xcc&\x18w\
\x1c?\xe2GK\xa5kb\x8d\xd3(+\xcb\xd2-\x0b-4A\xf1\xc6\xcb\x8a\xdaz\xa7\xd8\x9b\
zQ\xe8\x85R\xd3\xd0\x1a\x97\x06w\xad\xa8\x14/z%\x85\xd2\x85\xe2\x17\xedEK\
\x83\xd4\xc4X\x1b\x9bL\x98L\xdf\xc9d\xbe\'\x99\xc9{\xfe\xbd\xc8$\xc4e\xa1\
\xd0\x07\x0e\x1c\x0e\xcf\xc7y\xfe\xe7\xff<\xcf\xb1\xf8\xdfr\x008\x03|\x0bp\
\x00\x0b\xf0\x81\xa7\xc0\'\xc0\xdf\xbe\xccH\xabz\xb8\x1b\xce\x9c\xba!\xc0\
\x07\xc0\xbf\x81\x92\xe38\xef\xf8\xbe\xff.\xf0\xb5\xae\xae\xae%\x80d2\xd9\
\xd4\xd0\xd0\x90\xd9\xb9s\xe7\xf6\xc9\xc9\xc9y\xa0\x19\x88\x01\x9f\x03\x06p,\
\xf0\x05\x96\xfb\x05\xe7[\x80\x9f\xb9\xae{d\xeb\xd6\xad%\xd7u\x7f\xf0\xfa\
\xf5\xeb\xd7\xb6m[\x03\x03\x03\x0c\x0e\x0e6Y\x96\xc5\x83\x07\x0f\x98\x98\x98\
h\xf4}?\x01\x84c\xb1\xd8\'\xb3\xb3\xb3\x1d+++\x7f\x06~\x0c$\xd7\x82l\xccj\
\x8b\xe38\xf7W\xb3[_\x9f\x02W\x81\x7f\x0e\r\r)\x95J\xa9\\.\xeb\xf4\xe9\xd3\
\x02fC\xa1\xd0\xcd\xb6\xb6\xb6\xcf6\xda\xd8\xb6\xfd\x00\xe8\xfc2\xd8\xc6\x00\
\x85B!\xf5\xf6\xf6\x9aC\x87\x0e\x99\xd6\xd6V\x01joo\xd7\xcd\x9b7\xfdt:\xadR\
\xa9\xa4\xb1\xb11?\x14\n\t\xd0\xbe}\xfb\xd4\xdb\xdb\xab\xde\xde^\x13\x0e\x87\
\xd7\x02\xfd\xa6\x8e\xca\xba|\xe08\xce?\x00\x1d<x\xd0LMM\xc9\xf3<]\xbatI\xb6\
m\xeb\xe2\xc5\x8b\x92\xa4B\xa1\xa0d2)I\xba|\xf9\xb2l\xdb\xd6\x85\x0b\x174??\
\xaf\xe9\xe9i\xf5\xf7\xf7\x1b@\xae\xeb\xbe\x04>\xda\x18\xa0g\xfb\xf6\xed\xcf\
\x00\xf5\xf5\xf5\xc9\xf3<%\x12\t%\x93I\xcd\xcc\xcc(\x95J\xa9T*)\x9f\xcf+\x97\
\xcb\xa9X,*\x95JiffF\xc9dR\x89DB\x8b\x8b\x8b\x1a\x18\x180\x80\xa2\xd1\xe8\
\xdf\x81\xfdlH\xa31\x9b\xcd~\xde\xd6\xd6\x16\t\x04\x02_\xf7<\x8fx<NGG\x07\
\xa1P\x88\xc6\xc6Fj\xb5\x1a\x96eaY\x16\xc6\x18\x82\xc1 \x91H\x84`0H\xadVcxx\
\x98\x87\x0f\x1fZ\x95J\xe5n{{\xfb\xe5\xc5\xc5\xc5\x0c\x90\x078\xd0\xd3\xd3\
\xf3\xdd]\xbbv}#\x12\x89\xfc\xaa\xfeP\x9a\x99\x99\xd1\xca\xca\x8a\xb2\xd9\
\xac\xca\xe5\xb2\xbe(\x95JE\xd9lV\xd5jU\x9e\xe7)\x18\x0c\xae\xe1\xff\x0b`\
\xa7\xe38\x1f\x02\x07\x1c\xe0J\xa1P\xf8\xd8q\x9c\x8f3\x99L_0\x18\x8c\x9c?\
\x7f\x9ec\xc7\x8e\xd1\xd8\xd8\x88m\xdb\x14\x8bE\xc6\xc7\xc7y\xf2\xe4\t\xcf\
\x9f?\'\x9f\xcf\x13\n\x85\xd8\xb4i\x13\xc6\x18|\x7f\x95\x8d\x13\x13\x13\x94\
\xcb\xe5\x0e\xdb\xb6\xfb\x8c1\xfd\xc0n\x80\xe7\x80\xe2\xf1\xb8N\x9e<\xa9\xd1\
\xd1Q_\x92\x8a\xc5\xa2r\xb9\x9c$\xe9\xe9\xd3\xa7z\xff\xfd\xf7\xd5\xd0\xd0 \
\xdb\xb6u\xf4\xe8Q=z\xf4H\x92\x94\xcb\xe5T(\x14$I\xb7n\xdd\xf2\x07\x07\x07u\
\xfc\xf8q\xd9\xb6\xad\xbao^tuuUFFF\x94\xcb\xe5\x94\xcf\xe7\x95L&\xd7\xf7\x92\
t\xef\xde=544l\xe4\xban\xdf\xbe-I\xca\xe7\xf3\xca\xe7\xf3*\x14\nJ\xa7\xd3J\
\xa5R\x1a\x1d\x1dUww\xf72\xf0\xc2e\xadg\xb8.\xbe\xefcY\x16\xc1`\xf0\xad\x02\
\x91\xb4\x0e\x03\x801\x06c\xcc[:\xc5b\x91p8\x8ceY\xd8\xb6\x8deY\x00\x96\x03\
\xfc\xa8T*\xbd\xb3\xb4\xb4\xc4\xe3\xc7\x8fYZZ2\x87\x0f\x1f\xb6|\xdf\xc7\x18C\
 \x10 \x9f\xcf\xf3\xea\xd5+fgg\x91\xc4\x91#G\x18\x1a\x1a"\x1a\x8d\xb2\xbc\
\xbc\x8cm\xdbl\xde\xbc\x99\xfb\xf7\xef\x9b\x91\x91\x11\xeb\xd9\xb3g\x8c\x8f\
\x8f;@\xca\x02\xae\x00\xbd\xb6m7\x19cvttt\xc4\xce\x9c9\xc3\xb9s\xe7\x08\x87\
\xc3\x00\x14\n\x05&\'\'\x99\x9e\x9e\xc6\x18\xc3\x8e\x1d;\xd8\xbbw/\xe1p\x18I\
T*\x15FFF\x18\x1e\x1e&\x97\xcb\xcduvv\xcef2\x99\x95j\xb5\xfa\x02\xa0\xc3q\
\x9c\xe3\xf5\xc2\xf8\xf9\xffK\xd3\xe6\xe6f\x01\n\x87\xc3\xb7zzz\xfa\xf6\xec\
\xd9\xf3}\xe0=\x07(I\xaa\xc5b\xb1`6\x9b=\x1a\x89D\xde;{\xf6,\x03\x03\x03\x04\
\x83A\\\xd7\xc5\x18C\xa5R\xa1Z\xadR\xadV\xd7a\t\x04\x02\x00,//S\xab\xd5X\\\\\
\xa4T*M\xce\xcd\xcd\xfdqaa\xe1_\xc0\xcb\xb57\xda\xb5\xb1U\xcc\xcf\xcf+\x91H\
\xc8\xf3<y\x9e\xa7\x85\x85\x05\x15\n\x85\xb7\x18\x93\xc9d\xe4y\x9e\xd2\xe9\
\xb4\x12\x89\x84\xd2\xe9\xb4\xe2\xf1\xb8\x00\xc5b\xb1g\xc0>\x81e\xd7\x03lO\
\xcc\xcdm^#M\xa5R!\x18\x0cr\xfd\xfau\xa2\xd1(W\xaf^\xa5\xa5\xa5\x85r\xb9\x0c\
@KK\x0b7n\xdc \x1a\x8dr\xed\xda5\x9a\x9a\x9aXZZB\x92\x00\x12\x89\xc4\x16 f\
\x81\x10XC\xab=\xe9\xd3:\x86\xea\xef\xef7\xf1x\xdc\xec\xdf\xbf_\x80ZZZ466\
\xe6\xe7\xf3ye\xb3Y\xdd\xb9s\xc7\xef\xec\xec\x14\xa0\xd6\xd6V\xc5\xe3q\xc5\
\xe3\xf1\xf5\xf6\x0e\xdc\x05\x1aW\xaf[\xaf\x03\xa0\xab>,\xd6\x0b\xaa\xb5\xb5\
\xf5\xb7\xe1p\xf8\xd7\xc0\xec\x89\x13\'\x94\xcb\xe5\x94\xc9dt\xea\xd4)\x01o\
\x80_\x02\x9fm\xb4q]\xf7w@\xf7\x9ao\x17\xe0 4\xfc\x15\x92\xc6\x98\x1f\x02\
\x15\xd7u\xbf\xd9\xdd\xdd\x9d\x7f\xf3\xe6\xcdOw\xef\xde\xed477\xefK&\x93\xdd\
w\xef\xde\xc5u]<\xcf\xc3\xb6\xed\x97\xc6\x98\x91X,\xd6$\xe9\xabsssm\xb5Z\xed\
/+++\x97\x80\x04\xab\xa8\x18~\x02v=\x8b\x8d\xf3\xf9\xa3:m\xb7\xf6\xf4\xf4|/\
\x10\x08\xfc\tPww\xf7\xf2\xb6m\xdb\x96\x1d\xc7\x11\xf0\x07\xc7q\xbe\r\xbc\
\x0b\xec\x05\xbe\xb3\x0eK}\x0c\xacgp\x05,\xad\x0eh\x0b0\x16\xfc~\x8d^SSS\xed\
\xc0\x04\xf0\x95D"\xb1\xf1\xdb2\xed\xfb\xfe\x7f\x80\x99\xba\xead\xfd\xa2\x16\
\xab?\x0b\x0b\xe0\xbf\xf5\x19yJo\xfcW\xe3\x00\x00\x00\x00IEND\xaeB`\x82' 

def getRefreshBitmap():
    return BitmapFromImage(getRefreshImage())

def getRefreshImage():
    stream = cStringIO.StringIO(getRefreshData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getFindData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\x00\x00\x00\x18\x08\x06\
\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x06\x04IDATH\x89\xb5\x95Mh\x1b\xdb\x15\xc7\xffs\xe7\x8e4\xfa@V%K\xc6\r\x91\
\x12^\xeb$d\xf3\x08\xc1V\xa8\xc9\xb3i\xe2x\x97\x10(]$\xf0\xc0!i> i\xc0\x8bR(\
\xcf\xdd\xb4\xf1\xaad\xd7E\x9cdS\x08\xdd\x98.\x82\xbc0\xe9\xc3q)\xc6\xa4v\
\x82\xbf\x82\x15;\x93\x91,[c\xcb\x965\xdfsgn\x17\x0f\xf9\xb9\xa4o\xd7\x1e\
\x18\xb8w8s\x7f\xf7\xf0\xff\x9f3\xc0\xff9\x84\xc3\x9b\xb1\xb1\xb1\xdf\xf8\
\xbe\xff\xc7\xd7\xaf_C\x92$\x00\x80\xe388u\xea\x14\xb2\xd9\xec\xb7\xb7n\xdd\
\xea\x07\x80\'O\x9e\x145M\x1b\\XX@8\x1c\x06\x00x\x9e\x87\xde\xde^PJ\x7f\x7f\
\xe3\xc6\x8d\x91\xcfH\xe3\xe3\xe3\xc3\xf7\xee\xddc\xf9|\x9e\x038xB\xa1\x10/\
\x14\n\xfc\xfa\xf5\xeb\x7fo\xe5^\xbbv\xad\xd8\xdb\xdb{\x90C\x08\xe1\xa2(\xf2\
\\.\xc7o\xdf\xbe\xcd\xc6\xc7\xc7\x7f\xdb\xca\xa5\xad\x85\xef\xfb_\x11B\xc4\
\xcd\xcdMv\xf5\xeaUZ(\x14P.\x97\xf1\xfc\xf9s\xcc\xcd\xcdA\x10\x04\xab\x95\
\xbb\xb6\xb6f\xce\xcc\xcc\xe0\xf4\xe9\xd3\xb8s\xe7\x0e&\'\'Q\xab\xd50;;\xcb\
\x04A\xa0\x8c\xb1~\x00\x7f\xf8\x0f\x00\xa5t\xbb^\xaf#\x91H\xd0\x0b\x17.\xe0\
\xca\x95+X[[\xc3\xabW\xaf\xf0\xe9\xd3\'tuu}q\xe6\xcc\x99\xaf\t!\xac\xd9l\x9e\
x\xf3\xe6\rN\x9e<\x89\x9b7o\x82R\nUUQ*\x95\xe8\xee\xee.(\xa5\xdb\x9fU \xcb2\
\x18c\xd04\r333\xd8\xda\xda\x82\xef\xfb\xd8\xde\xde\x86,\xcb,\x9dNwy\x9e\xf7\
\\\x10\x04\xa4R)$\x93Io~~^z\xfc\xf81\x16\x17\x17!\x08\x024M\x83m\xdb\x08\x85\
B\xf8\x0c\xe0\xba.8\xe7 \x84@\x14E\xd4\xebuX\x96\x85 \x08\x10\x04\x01U\x14%0\
M\xd3\x01\x80X,\x16\xe6\x9cK\xb6mcuu\x15\x9a\xa6!\x9dN\x1f\x18\x831v\x00 \
\xad\xc5\x87\x0f\x1f\x9a\x8a\xa2@\x14Ed\xb3Yx\x9e\x87b\xb1\x08\xc7q\xd0\xd3\
\xd3\x83l6\xfb\x8fb\xb1\x18-\x16\x8b\xd1l6\xfb\xea\xdc\xb9s0\x0c\x03\x13\x13\
\x138v\xec\x18\x18cp]\x17\xd5j\x15+++\x07\x00\x11\x00\xee\xde\xbd\xfb\xbb\
\xb7o\xdf\xde\x92e9\xd2\xdd\xdd\r]\xd7111\x81J\xa5\x02Y\x96q\xf6\xecY\xd8\
\xb6M\xdf\xbd{w\x04\xc0\x85\xae\xae\xae\xaf\x92\xc9d\xdb\xd2\xd2\x12j\xb5\
\x1a4MC>\x9fGGG\x07$I\x82\xaa\xaa\xd3\xa5R\xe9\xe5\x01\xa0\xbb\xbb\xfbo\xab\
\xab\xab?\xda\xda\xdab\xc9d\x92\xcc\xce\xceBQ\x14\xc8\xb2\x8c\\.\xc7\xc2\xe1\
0\xd1u=a\x9a\xe6\xb9x<\xfe\xb3x<\xde\xe6\xba.\x0c\xc3`\xf5z\x9d\xec\xed\xedA\
\x14Ed2\x19\xa8\xaa\x8ar\xb9\xdcv\xff\xfe\xfd\xad\xa9\xa9\xa9E\n\x00\xe1p\
\xf8c\xa5R\xf9\xd24MZ(\x14\xd0\xdf\xdf\x8fK\x97.AUU\xac\xaf\xaf\xd3\x85\x85\
\x85R4\x1a-\xa7R\xa9>\x00(\x97\xcbS\x8dF\xa3#\x9f\xcf\x9f\x18\x18\x18\x80(\
\x8a\xd0u\x1d\xd5j\x15\xef\xdf\xbf\xf7r\xb9\xdcO\x1b\x8d\xc6_\x00\xfc\x15\
\x00\xf0\xf0\xe1\xc3\xb9T*\xc5\x07\x06\x06x\xadV\xe3\x86a\xf0\xf5\xf5u><<\
\xcc\xdb\xda\xda8\x80a\x00\x91C\r\x98\x04p\xbb\xaf\xaf\x8f+\x8a\xc2\x9b\xcd&\
\xd74\x8d\x8f\x8c\x8cpY\x96y.\x97\xe3\x0f\x1e<X>p\x11!\x04\x94R\xe8\xba\x0eU\
U\x11\x89DP*\x95\xa0\xebz\xcb\x19\x0c\x80{\xa8\xf1=\x00\xbe\xe38PU\x15\x86a`\
cc\x03{{{\x90$\t\x82 \x80\x10\xf2\xbdM\x1d\xc7qL\xd3\x84\xa6i\xd8\xdc\xdc\
\x84,\xcbX^^\x86i\x9a\xadY\xf3+\x00\x83\x87\x00\xe3\x00~\xec8\x0e*\x95\n<\
\xcf\xc3\xca\xca\nvvv`\x9a&$I\x82\xe38.\x00\x88ccc#/^\xbc\xf89\x80x\xa1P@gg\
\'l\xdbF\xb5ZE\xbd^\xc7\xf2\xf2\xb2\xc79\xef\xe8\xe9\xe9\xf9\xc9\xf1\xe3\xc7\
\x91\xcf\xe7q\xf4\xe8\xd1/\xca\xe5r6\x91Hx\xdd\xdd\xdd\xa2\xeb\xba(\x97\xcb\
\x90$\t\xed\xed\xedh4\x1a\xa8T*\xe2\xa3G\x8fb\x941\xf6\xcd\xe2\xe2".^\xbc\
\xe8\x9f?\x7f^\xacV\xabH&\x93p\x1c\x07\x9cs\xd8\xb6-\xe5r9\x0c\x0e\x0en\xeb\
\xba\xfe-\x80 \x1e\x8f\xf7U\xab\xd5\xaca\x18\x92i\x9ap\x1c\x07\xba\xae#\x1a\
\x8d\xe2\xf2\xe5\xcb`\x8c\xf9/_\xbelg\x8c}C\xa6\xa7\xa7\xc19G&\x93\x11=\xcf\
\xc3\xce\xce\x0el\xdb\xc6\xce\xce\x0e\x18c\xa0\x94\x82\x10\x82\xfd\xfd\xfd\
\x7f\x8d\x8e\x8e\xfebtt\xf4\x97\xcdf\xf3\x9f\x94R\xb8\xae\x8bz\xbd\x0e\xdb\
\xb6\xd1h4\xa0(\n,\xcbB&\x93\x11\x01`zz\x1a\xa4%\x8aeY`\x8c\x81\x10\x02\xc7q\
@\x08\x81 |\xf7\xbb\xe0\x9c\x83s\x1e:\xa4A\xb85V\x04A\x80\xe38\x10\x04\x01\
\xa1P\x08\x9e\xe7\xc1\xb2\xbe\x1b\xbc\x92$\xa1\xd5\x07(\x95J\x90e\x19\xae\
\xebbww\x17\x86a\xc0\xf7}\xfcPPJa\x18\x06VWW\x11\n\x85`\x9a&,\xcb\xc2\xdc\
\xdc\x1cJ\xa5\xd2\xc1\\\xa2\xad\x1b\x8a\xa2\x88T*\x85X,\x86L&\x83\x8f\x1f?"\
\x08\x02\xc8\xb2\xfc_AA\x10\x80R\x8a\xce\xceNttt@UUloo\x831\x06\xcb\xb2\xbe\
\xb7i$\x12A\x10\x04PU5XZZ"\xd1h\x14\xf1x\x1c\x8a\xa2 \x08\x02\xd4j5?\x9dN\
\x8b\x00\xda\x0f\x1d\xdeN)E\xbd^\xf7\xe7\xe7\xe7\xc5t:\rUU\xd1l6\xc19\x87\
\xa2(\x81\xef\xfb$\x12\x89\x80Z\x965\x7f\xe4\xc8\x91\x13\x1b\x1b\x1b\x91\xa9\
\xa9)p\xce\x11\x04\xc1A\x89\x92$\x89\xf1x\x1c\x00\xa6Z\x00B\xc8\xebX,vV\x92$\
qrr\x12\xbe\xefC\x92\xa4\xc3\xba\x91\xce\xceN\xc7\xb2\xace\x01\x00\x86\x86\
\x86\xe6"\x91\xc8\x97-q\x0eG"\x91@\xa3\xd1\xf8\xf3\xb3g\xcf\xee\x1c~?44\xf4\
\xa7D"\xf1\xeb\xfd\xfd\xfd\xcf\xbe\x91e\x19\xb6m\xaf<}\xfa\xf4\xd4\x0f\x8a\
\xf8\xbf\x8a\x7f\x03\x7f_\x17\xc3%\x00\xe7\xe4\x00\x00\x00\x00IEND\xaeB`\x82\
' 

def getFindBitmap():
    return BitmapFromImage(getFindImage())

def getFindImage():
    stream = cStringIO.StringIO(getFindData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getShowSizersData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\x00\x00\x00\x18\x08\x06\
\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x04(IDATH\x89\xb5\x95Mh\\U\x14\xc7\x7f\xf7\xdey\xf7\xcd\xbcy\x99\xc9\xb43\
\x93\x99\xc4iM\x02~\x97R\xa9Q(\x82\x16*\xb5\xeaFP\x8b;\x0b\xe2\xb6\xba(\xa8\
\xa0\xe0F\x17"\xaeD\xa9\x8a\x04\x84\n]\x16\x05\xadt\xe1B\xa8H\x8a\x0bK\xc4\
\xd6\x966\xb553\x93\xc9L\xf3\xe6\xcd\xfb\xb8\xd7EZ\x9b\xa6%)-\x1e\xb8\xab\
\xcb\xff\xfc\xce=\xfc\xcf\xb9\xf0?\x87X\xe7\xfe\xc1R\xa9\xf4\xaa\xeb\xba\xdb\
\xad\xb5\x8b\xd7\t\x85(\xc6q<\xd3j\xb5\x0e\x02\xbf\xdd\x16}lll\xbf7\xe4\x9d\
\x92\xc8(Cf\xe1\xdaaAB\x94\xf3\xbd\xd3\xf5z\xfd\xc0Z92k]*\xa5\xee2=\xb3\xd9\
\xdcg\x94y\xc1\x0c\xb3\x04(`\xae\x08\xdf\x0f\xb0\xcdh\xb3\xde\xa8\x1b\xb7\r\
\x00B\x81\xe8\xb3\x05\x9f7\x1c\x98-\x816\xf0g\x15~\xbf\x08\xcdN`\x8c\xe9\xdf\
\t`9b\xe0\xaf\x1c\xfc4\t\x19\x0b\x84`\xe3[\x92\xde\x1a \x05Z1\x9ci\x82\xb2Pr\
\xc0\xc2\xfa\x1eY\x07 \xa5\xcc\xd9\x8c\xcd\xf3\x03p\xa2\x0f\x97\xcf\x02X\xa4\
\x15tbp\xdc\xbc\x94\xd2[+\x87Z\xb3\xf04U\xd2\x95%\xdf\xf8\xa1j\x89f\x14\x86U\
\xc2Tx}\xe7\xe4\x90?4g\xb5:\x11\x06\xfdo\xa2(\x9a\xbd-@\x14E\xedJ\xb9RP\xae\
\xba\xa7\x9f\x86\x15\xadu\xe48N`\x14*_\xf0\xdb^.\xf7m\xb3\xd9<\x02\x04W$7\
\xf4l-@\xaeX,N%I\xb2\xa7\xd7\xeb\xedJ\xd3tP.\x97\xa7}\xdf\xff\xb5\xdb\xedn\
\x1d\x0c\x06\xf7Zk\x17\xb4\xd6\x97\x06\x83\xc1E\x96\xadpS\xc8\xcd\xa2\xa8\
\x94z\xaeV\xab\x1d\xd2Z[\xaduk\xd3\xa6M\xef\x00\xdb\x81m\x8dF\xe3m\xd7u\xe7\
\x1d\xc7\xb1###\x87\x95R\xcf\x03\xa5\xf5\x92^%+\xa5\xd4\xee\xb1\xb1\xb1O\x94\
RK\x9e\xe7\x85\xe5r\xf9K\xe0\xb1\x15/~\xa4R\xa9|\x9e\xcf\xe7\x97\xa4\x94\xc1\
\xe8\xe8\xe8A\xa5\xd43\\3\xcd\x7f\xaf\xb8\xa1E\xf9|~g\xa1Px\xa5\xd9l\xee\xd5\
Z\xf7]\xd7=\x94$\xc9g\x83\xc1\xe08W\xcc\t\\\x00\xe6\xb4\xd6J\x081\xd9\xedv\
\xa7J\xa5\x92\'\x84h\xc7q|ze>\x05\x8c4\x1a\x8d\x1d\xb5Zm\x8b\xef\xfb\x0f\'I\
\xf2b\xbf\xdf\x7fVJ9_.\x97g\x93$9\xd2\xe9t\x8e\xb2<\r\xe2juq\x1c7\xb3\xd9l\
\xb1T*\xd5\xc20TI\x92l\xd5Z\xfb\xd5j5;222\xe1\xfb\xfe\xf8\xe2\xe2bOx\x9ew\
\xc0q\x9c\x971\xe9\x03\x12\xd3\t\x07Q\xa1\x1f%\xee\xb0\xe2g<\xbf\x90\x085c\
\x92\xf8\xa3 \x08fVV\xe6y\xde\xd6\x8c\x94\xfbQj\xaa/M\x10/\xf4\xb6\x93u\x02\
\'\xe7-(c6d\x91\xa7\xa38\x9e\xa6^\xaf\x1f\x93B$\x08eqr-\x94\x9e\xc7\xcd\xcfS\
\x1a\xb5 \xedP\xde\xebOLL\xbc\xb6\xba\x95\xe3\xe3\xe3\xfb\xfc\xc2P\x1f\xb0%T\
\xb8\x91Lg\x18uy\x03*\xc8\x81E\x8a\xb4^\xaf\x1f\xcb\x18c\x02a\xad\xe2\xa1]_\
\xf1\xe9w\xd3\xfc8\x93\xc5u\xef\xc6\xcd|\xc8\xc7/y\x9c=\x11\x00\xd1ML\x11-\
\xd94\xad\x91\xe9\x9fdj\xda`\x92\x90T\x8cR\xe8\xbd\xc9\xc9G?\x10\x17\x9e0\
\xc6\x04\xd7V\x85\x10\x97\xd8\x81\xcf?\xdb\x9e$%O\x7fI!\xd7\x9cC,\x08\t\xf10\
C\x01dw\x82\xaat\t\x83K\xc8\n\x10\xc2\xca]\x94\xda9R&\xb1\xbc\x8e\xa4\xcdP^#\
\xe4\x9a\x00@d\x10)x\xe1,K\x93\x92\x8c? %\x8b\x06\xe8\x01d\x84\x109\x0bp\xfa\
\xf8n\xf6\xbe\xe7S\x19\x85\xe0\xf2\x06\x10\xd0\x9e\x83\x0c\x1e,+\xae\xcb,\
\x84\xa3\x85t:\xc4\xb9\xb7\xf8\xe5\xf1S\xa4n\x04lD3K\x0f,Y!D.c\x8c9\xa7\xb4>\
C\xd8\xde#\x0e\xbf\x0b\xb0\xc0\x95\xf9\xb0\x90\xb7\xc5\xe1\xb3I\x92\xb4V\x03\
\xe28n\xe7\x90\x7f,Jq\xff\xfb\xe2\xfc\x0e`\x89\xe5\x9d$\xb0hW\xe9\xb61\xe6\
\x9c\x00\x9e\xaeV\xab{\xa5\x94\rk\xedu\xbf\x93\x10\xa2\x10\x04\xc1\xd1n\xb7\
\xfb\x05p~\x15c\xd4\xf7\xfd}\xbe\xef?e\xad\xed\xad\xd2e\xd34\xfd{~~\xfe\xeb\
\xf5z|\xc7\xf1/Y%\x9eF\x90EP\xda\x00\x00\x00\x00IEND\xaeB`\x82' 

def getShowSizersBitmap():
    return BitmapFromImage(getShowSizersImage())

def getShowSizersImage():
    stream = cStringIO.StringIO(getShowSizersData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getShowFillingData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\x00\x00\x00\x18\x08\x06\
\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x01\x9eIDATH\x89\xed\x95\xcfJ\x02A\x1c\xc7?\xad\xbb\xb3\xb1-\x14\xae\x19E\
\xed!\xf5)\xbaE\xbd@\x97\x0e\xdez\x84.\x82\xd7.\xbd\x81\xb7\x8e\xddD\x10*\
\x83\x12z\x06\xa30<\x97\xb4\x14Ibh\xb3k\x17\x15u\xcdv\xadc\xdf\xd303\xdf\xef\
g\x86\xdf\xfc\x81\x7f\xfd\xa0\xb9\xa1\xf6\x0e\xb0\x0b\xac\x00\xed\x19\xf3\
\x04\xf0\x02\x94\x80+\x00uhp_U\xd5m)\xe52\xd0\x99\x11\xa0)\x8a\xfa\xeayrq\
\x12`CJ\xb9~tt(67\xf78?\x7f\xc64\x83'\x9b\xe62\xb7\xb7g\\^\x1e/\x00\x1b\xfd\
\xfea\xc0\x07\xf0a\x18I\xb1\xba\xba\x85m?b\x18\xc1\x01KKk<==\x0cr&\x01\x00\
\xb8\xb9\xb9G\xd3\xae\xb1,\x07\xcf\x0b\x0e\xd0u\x0bM\xbb\x03\xe8~7\xa7\x00\
\xbc\x15\n\xf9\xee\xac\xba\xb88\xeb\x02\xef\xbd,\x00\x94qJ\xbb\xfd\x19|\xd9\
\x01\xbc>\x80\x10bf\xc0$\xaf\xaf\x06\x8dF\x03\x80f\xb3\x19*\xdc4\xcd\x81w* \
\x97\xcbQ.\x97i\xb5Z\xa1\x00\x86aP\xab\xd5`\xac\xc8>@\xa5R\xa1Z\xad\xe2\x859\
B\x80\xa2(t:\xfe\xfb\xe9\x03d2\x19\xd2\xe94\xf5z=\x14 \x1e\x8f\x93\xcf\xe7\
\xc9f\xb3\xd3\x01\x89D\x82d2\x89m\xdb\xa1\x00B\x08R\xa9\x14\x8c\xbeo~\x80\
\xae\xeb\x03CX\xf5\xbc#5\xf0\x1d\xd3h4\x1a:\xb8/\xcb\xb2`\xca\x0e\xe6\x81\
\xf9b\xb1\x08\x80\xe38\xa1\xc2c\xb1\x18\xa5R\t@\xefe1N;\xf9\x8b\xe7:\x12\x89\
\xbc\xba\xae{\x05\x1c\xc0\xe8\x0eN\xa5\x94\x0e\xbf\xfcp\\\xd7\xed\x7f8\xff\n\
\xa6/8\xf7\xb7\xf5\xb4m\x07\xcd\x00\x00\x00\x00IEND\xaeB`\x82" 

def getShowFillingBitmap():
    return BitmapFromImage(getShowFillingImage())

def getShowFillingImage():
    stream = cStringIO.StringIO(getShowFillingData())
    return ImageFromStream(stream)

#---------------------------------------------------------------------------

def getIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00\
\xb8IDATX\x85\xed\x97\xcd\x12\x83 \x0c\x84w\x91\xf7\xd6>x\x9b\x1e,\x07Gl\x93\
4N8\xb07gH\xf8\xc8\x1fB\x96\x05\x99*\xa9\xbbO\x80\x11\x00\xea\xaf\x05\xf2z\
\x8a\xc71\xcb\xc2\x10\x80]\x9bq{\xfd\xfa\xf4\x14\xa8\x01DV\x88\xacy\x00w)\
\x1d@Y\x84\x1am\x87/m\xf7\x04\x02\x00blX\x12\xa8\xde>\x8fR\x05\x80o\x04\xaai\
rP\xf3\xa6\xb3\x1c\xa8\x08[\x02\xd9\'\'\x1f\xb7\x00\xa4G\x80hg\xbf\x88\x80\
\xa5\x06>\x8e\xd4\x96\xa4\xe66\xec\x19\xe2|\xdby\xbb)=\x05\xe9\x00\xa1\x93p\
\x97mr\x0c\x14\x81\x8b\xfe\xb7\xc8\xe3",\x05\xda\x7f\xc0.\xc0\xffg\xf7\x8b\
\xf3i6\x01\xb2\x01\xde\x86\xde%]y\x9b\xef$\x00\x00\x00\x00IEND\xaeB`\x82'

def getIconBitmap():
    return BitmapFromImage(getIconImage())

def getIconImage():
    stream = cStringIO.StringIO(getIconData())
    return ImageFromStream(stream)

def getIconIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getIconBitmap())
    return icon

#---------------------------------------------------------------------------


