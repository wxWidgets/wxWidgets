#----------------------------------------------------------------------------
# Name:        wx.lib.mixins.inspect
# Purpose:     A mix-in class that can add PyCrust-based inspection of the
#              app's widgets and sizers.
#
# Author:      Robin Dunn
#
# Created:     21-Nov-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

# NOTE: This class was originally based on ideas sent to the
# wxPython-users mail list by Dan Eloff.

import wx
import wx.py
import wx.stc
import sys


class InspectionMixin(object):
    """
    This class is intended to be used as a mix-in with the wx.App
    class.  When used it will add the ability to popup a
    InspectionFrame window where the widget under the mouse cursor
    will be selected in the tree and loaded into the shell's namespace
    as 'obj'.  The default key sequence to activate the inspector is
    Ctrl-Alt-I (or Cmd-Alt-I on Mac) but this can be changed via
    parameters to the `Init` method, or the application can call
    `ShowInspectionTool` from other event handlers if desired.

    To use this class simply derive a class from wx.App and
    InspectionMixin and then call the `Init` method from the app's
    OnInit.
    """
    def Init(self, pos=(-1, -1), size=(850,700), config=None, locals=None,
             alt=True, cmd=True, shift=False, keyCode=ord('I')):
        """
        Make the event binding that will activate the InspectionFrame window.
        """
        self.Bind(wx.EVT_KEY_DOWN, self._OnKeyPress)
        self._tool = None
        self._pos = pos
        self._size = size
        self._config = config
        self._locals = locals
        self._alt = alt
        self._cmd = cmd
        self._shift = shift
        self._keyCode = keyCode


    def _OnKeyPress(self, evt):
        """
        Event handler, check for our hot-key.  Normally it is
        Ctrl-Alt-I but that can be changed by what is passed to the
        Init method.
        """
        if evt.AltDown() == self._alt  and \
               evt.CmdDown() == self._cmd and \
               evt.ShiftDown() == self._shift and \
               evt.GetKeyCode() == self._keyCode:
            self.ShowInspectionTool()
        else:
            evt.Skip()


    def ShowInspectionTool(self):
        """
        Show the Inspection tool, creating it if neccesary.
        """
        if not self._tool:
            self._tool = InspectionFrame(parent=self.GetTopWindow(),
                                         pos=self._pos,
                                         size=self._size,
                                         config=self._config,
                                         locals=self._locals,
                                         app=self)
        # get the current widget under the mouse
        wnd = wx.FindWindowAtPointer()
        self._tool.SetObj(wnd)

        self._tool.Show()
        self._tool.Raise()


#---------------------------------------------------------------------------

class InspectionFrame(wx.Frame):
    """
    This class is the frame that holds the wxPython inspection tools.
    The toolbar and splitter windows are also managed here.  The
    contents of the splitter windows are handled by other classes.
    """
    def __init__(self, wnd=None, locals=None, config=None,
                 app=None, title="wxPython Widget Inspection Tool",
                 *args, **kw):
        kw['title'] = title
        wx.Frame.__init__(self, *args, **kw)

        self.includeSizers = False
        self.started = False

        self.MakeToolBar()

        self.outerSplitter = wx.SplitterWindow(self,style=wx.SP_3D|wx.SP_LIVE_UPDATE)
        self.innerSplitter = wx.SplitterWindow(self.outerSplitter,style=wx.SP_3D|wx.SP_LIVE_UPDATE)
        self.tree = InspectionTree(self.outerSplitter)
        self.info = InspectionInfoPanel(self.innerSplitter)

        if not locals:
            locals = {}
        myIntroText = (
            "Python %s on %s\nNOTE: The 'obj' variable refers to the selected object."
            % (sys.version.split()[0], sys.platform))
        self.crust = wx.py.crust.Crust(self.innerSplitter, locals=locals,
                                       intro=myIntroText,
                                       showInterpIntro=False,
                                       )
        self.locals = self.crust.shell.interp.locals
        self.crust.shell.interp.introText = ''
        self.locals['obj'] = self.obj = wnd
        self.locals['app'] = app
        self.locals['wx'] = wx
        wx.CallAfter(self._postStartup)

        self.innerSplitter.SplitHorizontally(self.info, self.crust, -225)
        self.outerSplitter.SplitVertically(self.tree, self.innerSplitter, 280)
        self.outerSplitter.SetMinimumPaneSize(20)
        self.innerSplitter.SetMinimumPaneSize(20)


    def MakeToolBar(self):
        tbar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.TB_FLAT | wx.TB_TEXT | wx.NO_BORDER )
        tbar.SetToolBitmapSize((20,20))

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
        toolFrm = wx.GetTopLevelParent(self)
        toolFrm.SetObj(obj)


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

        #self.SetValue('\n'.join(st))
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
import cStringIO


def getRefreshData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x14\x00\x00\x00\x14\x08\x06\
\x00\x00\x00\x8d\x89\x1d\r\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x02TIDAT8\x8d\x9d\x95\xbfk\xdbP\x10\xc7?\xaaC\'\xa3%\x85d\n\x1c\x04\xba\
\x96[:\x14\x8aK;\xe6O\xc8\x90\xc1\xde\r\x9d\x0b\xfa\x13\x84\x97\x0c\xf5\x98\
\xc9c\xe9\xd0%i\xe2\xad\x14D\x97\x0c\x15\x86\x83\x0c\xc2\x01\x17\xc3CK\x83\
\xcd\xeb\xa0\x1f\x95\x9c\x1f\r=\x10z\xe8\xee\xbe\xef\xbd\xef\xdd}\x15D\xdco\
\x11\x0c\x80C\xe0Y\xf9i\x01\x9cD\xf0\xf1\x8e\xf0\x00`\xeb\x1e\x90+\xe0\x1a\
\xd8\x01^\xab*\x00I\x92 "\xdf#\xb3\x17\xa5o\xaf\x01\xee\x81\xe0\xc9\x06\xd8)\
p,"_\xaao\xaaJ\x1c\xc7\xc4qL\x05\x0c "\x9f\x80\xe32\xa72OT\x00A\xe1\xf0\x8dg\
\x01\xacT\xd5\xa7i\xea\xb3,\xf3\xaa\xea\x81\x95\x88\xfc\xde\x88=\xadqJ\xb0\
\x01\xb0\x02\xfcd2\xf1I\x92T\xc9\xde9\xe7\x9ds>\xcb2\x9fe\x99\x07\xbc\xaa\
\xfa$I\xfcd2\xa9\x00W\xc0 jpx%"k3\xeb\xec\xef\xef\xb3\xbb\xbb\xcb\xf9\xf9y\
\x8b\xdbn\xb7\x0b\x80s\x0e\x80<\xcf\x9b\xd7\xef\x98\xd9\x15@\xc5\xe1\xb5\x99\
\xbd\x14\x91\x9b~\xbf_\'?d\xddn\x97~\xbf\x0f\xf0\xcb\xcc\x94\xa2\x88\x04\xc0\
@D\x9e\x03\x98\xd9\x10\xe8T\xa7\xf8\x97\x85a\x08\xb0\xa6f\xae\x00\xbd\xa0\
\xe4EU}\x96e5o\xce9?\x1a\x8dj\xdfh4j\xf9\x9cs\xb5\x8f\x82\xcb\x0b\x80KU\xf5\
\xd3\xe9\xb4&~3\xa1\x0c\xf6\xaaz\x0b0MS?\x9dN\xab\xb8\xcbV\x1f\xe6y\xfe(\xfe\
\x1e\x8a\xdf\x02\x16I\x920\x1c\x0e\x01nU\xf7\xe8\xe8\xe8\xceue\x07\x07\x07@1\
E\xc0"\x00\xde\x00\xafJ\x7f\xc4\x7f\x14EDb\x003\xfb\xb9\x15\xc1\xd7\x08\x96\
\x14\x15\x1f\xaa\xea\xf6\xa3\xd0(\xc6r\xb9\\\xae\xcd\xec\xa4<\xcd\x8f\x8a\
\xc3\x1d\x11I\x80\xed\xf1x\xdcj\xda\xfb,\xcfs\xc6\xe31f\xf6TD\xbeQ\x88E-\x0e\
{f\xb6\x06\x98\xcdf\xcc\xe7sz\xbd\x1ea\x18\x92\xe7yk\x830\x0c\xe9\xf5z\xcc\
\xe7sf\xb3\x19\xe5U;\xc0\x1e\xa58\x04Qq\xdd\x968\x94\x02\xb0\xaaz3M\xd3Z\x1c\
(\x84\xe3Nq\xa8\xdb&\x82\xb7\xc0\x19\x05\xc97\xe5(\xc6\xcdS\xff\r\xe5\x9d\
\x88@1%ge.@\x10D\x05h\xb5SK`E\xe4\xd0\xcc\xde7\x05\x16\xf8\x00|\xe6\xb6\xc0\
\xb6\x00\xa9\x00\x1b\xef\xc7\xfe\x02\x82\xc6\x9a?\xea\xc8x\xd8\xb0\t\xb9\xf6\
\x00\x00\x00\x00IEND\xaeB`\x82'

def getRefreshBitmap():
    return BitmapFromImage(getRefreshImage())

def getRefreshImage():
    stream = cStringIO.StringIO(getRefreshData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getFindData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x14\x00\x00\x00\x14\x08\x06\
\x00\x00\x00\x8d\x89\x1d\r\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\xe6IDAT8\x8d\xa5\xd3\xbdk\x13q\x1c\xc7\xf1W\x9a\x12t\xe8\x18\xcd\
\xd6\xa1\x17\x10\x8a\xa8\x8b\x83C\x0c\x9d\xae\x07R\x1d|@\x9d\x02\xa2 \xde\
\x1f\xe0t\x93\x7f@\xbb(\x82\xb8\xa4\xb8i;\xa47\xd6\x08\x0e\x1d\xdaB\xc7\\)\
\x05\x1fJ\x07\xff\x80\n\xe2pi\x9a\xda\xa6O~\xe0\x0b?\x8e\xef\xef}\xef\xdfS!\
\xb1\x97\x84\xeb\xb8\x81\xcb\xf6g\r_\x13\x96\x1c\x93b}\x0f\xf6\x08wq\x05\xf7\
p\xad\xaf\xfe\xe0\xd2"#\xf5\x1c>0\xc3}\xb0\x87\x88\xe28\x16\x86\xa1 \x08@\
\x96e\xd24\x8d\xa6\xa7\xa7\xa1\x92\xe4\xfd\xcdA\xc0\x82|\x99O\xd1h\xb5Z\x82 \
\xb0\xb3S\xdd\xd7T*udY&\x8a"x\x877\x83\x96?$\xdf\xb3J\x1c\xc7=\xd8\xf88\xeb\
\xeby\xc1\xceNU\x10\x04\xe28\x86Jw\xce\xa1\x19\x92\x1f@\x14\x86a\x0f\x06\x17\
\xba\xd5\x0f\r\xc3\x10"\x07\x0f\xad\x97\xe1\xddAn\xc7\xfc|\x0e\xda\xcd.tlLo_\
\x8f\xca\xd0\xb1\x1d\xa7L\x0f\x98e\x19r\x93\xed\xbe\x86\xed\xee\xb7\xfe\x1e\
\xfc<\n\xb8\x86V\x9a\xa6J\xa5\xce>h?\xacT\xeaH\xd3T\xadV\x83\xd5A\xc0\xb3\\\
\x1bh\'\xdc<\x0cXL\xf8\xbe\xc8\x08\xce5\x9b\xcdj\xa1PP.S.S,\xfe\xb2\xb1\xb1d\
vvV\x1c\xc7\xa2(R\xadvt:F\x17\x99\xa8\xf3\xfe\x80a\xd2\x1d$\xf9k\x99\x90\xdf\
\xb3\xa8\xbf\xa9V\xabi\xb7\xdb\x1a\rn]\xe5\xdb\x16/^\x1dn\xda{\xcbu\xd6\x16\
\xd9\xc2o\xfc\xc0J\xb7Z\x9b\x9b\x9b3\xa8\xac\xac\x18}0\xc9\xfd\xc7s.\x9e\xff\
`\xe1\xcbA\xd3\x9e\xe1I\x92\xf0\x19\xb5\x8f\xd3L\xde\x9e\xf3\xf6\xf5\xd4\x01\
\xd3S\xdd\xc3\xee\xa4\xf6\x9d\x98\x85OS\x9e<\x9b3\xf3\x12\xd4\xba?;\x9d\xe1I\
L\xcf\xf4R\x0e3m4r\xd33\x19\xfek\xdah\xb0\xba\xca\xf2\xb2\xf6\x7f\x01\xfb\
\xa1h\xe3\xf9_3n\xb4\xee\xba\xaf\xf1\xb8\x00\x00\x00\x00IEND\xaeB`\x82'

def getFindBitmap():
    return BitmapFromImage(getFindImage())

def getFindImage():
    stream = cStringIO.StringIO(getFindData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getShowSizersData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x14\x00\x00\x00\x14\x08\x06\
\x00\x00\x00\x8d\x89\x1d\r\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01-IDAT8\x8d\xad\x931N\xc3@\x10E\x9f!\x1dU\xba\x7f\x008\x00\x17@\xe9\
\x007\\\x81\x16\xc5\xe2\x0c{\x01\xba Qr\x85H\x89#Rr\r\xe2~\xbaH\xc8\xa2B2\
\x05^\xb3$v\xd6\x962\xd2Jc\xff\xd9\xb73;;\x89\xe3\xb86\xea\x12\x1c\xdcJ\xca\
\xda43\x9b9X\r\x02J\xca\xcc,\xed\xd0\xc0l\x18\xb0\xb1\xb7\xc0__\xc0\xd3\xc7\
\xc1\xf08\x10\xa0\xb8\xac\x9d2\x1a\xda\x13\x18\x07\xf5\x07^\x03\x1c.\xb3\x17\
\xd0\xccf\x92\xbc\x9f\x02H\xca\xbd\xd6\xb5\xef$v\xa2\x99\xa5\x92rIyW\xd7\xa3\
\x19\xba\xfa\rz\x98\xcfH\x92?\x00g\x86ky\x8b\xa7\x93\x1e0\x07\xab\tl\x16e\
\xb9\x954\xae\xb5\xf1\xa2,\xb7\x13\xd8t\x02\xbb`^\xef\x03\x1d\xb9`\xc4\x14\
\xdcY\xec\xae\xfc\x9dJ\xe2\x052\xa8\x9b%i\tTmK\xd2\xd2\xfdf\xde\xacX\xfc_S^\
\xab\xbb\xc6\xff,\xe6<\x9e\x1f\xcc\xb0\xe2\xea9\xfcNx\x9f\xc2n\x97\xbfy\x88\
\x95\x1aZ\xc1\xd9\x14`\x8d5\xff\xfe\x03\xbf\x8a\x9b!\xc0\x10\xb4\x0f\xbcO\
\xe6C`\xbe\xc4=`8b\xbb\xd66b\xb1\xf8\xc4\rI\xab\x87\xfd\x00>\xff\xa7/3u\xdc\
\xf3\x00\x00\x00\x00IEND\xaeB`\x82'

def getShowSizersBitmap():
    return BitmapFromImage(getShowSizersImage())

def getShowSizersImage():
    stream = cStringIO.StringIO(getShowSizersData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getShowFillingData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x14\x00\x00\x00\x14\x08\x06\
\x00\x00\x00\x8d\x89\x1d\r\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x99IDAT8\x8d\xd5\xd2\xb1\r\xc3 \x10\x85\xe1\xff"\xcf\x90\x89\xa0O\
\xe3\x19\\d\x89["\x8dWp\xe1\x01`"/q)\x12G\xc2r\n\xccEr^\x03\x08\xf4\xe9I\x87\
(\xbe\xb98{t\x00\n\x01\xe8\x1b\x9cI!\x7f@\xe0\x0e\xdc\x1a\xc0+\x1b\x10\x003c\
\x1c\xeb\xa4a\x90\xe2\xdc\x95\x97\x95\xda^\xf4\xb5\xcc\x80\x1d\t`\xc0\xaco\
\xcb}\xca\xbf\xf96kr\xce\xbe`\x8c\xd1\x17L)U\x03\xdb\x12\x05\x18B8R\xaa\xc8\
\x9fMYD\xbe\xbd\xab\x06\x1f\xc0\xd2\xe0L\xebF\xb4\xa5\xceN\xce?\x94\'\x9egM\
\xd2gvb\xb2\x00\x00\x00\x00IEND\xaeB`\x82'

def getShowFillingBitmap():
    return BitmapFromImage(getShowFillingImage())

def getShowFillingImage():
    stream = cStringIO.StringIO(getShowFillingData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
