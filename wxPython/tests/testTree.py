#!/usr/bin/env python

from wxPython import wx
import sys, os
from stat import *

GlobalObjList = []

class Obj:
	def __init__(self, obj):
		self.obj = obj
		# Uncomment next line to eliminate crash.
		# GlobalObjList.append(self)

	def Name(self):
		head, tail = os.path.split(self.obj)
		if tail:
			return tail
		else:
			return head
		
	def HasChildren(self):
		return os.path.isdir(self.obj)
	
	def Children(self):
		objList = os.listdir(self.obj)
		objList.sort()
		objList = map(lambda obj,parent=self.obj: os.path.join(parent,obj),
					  objList)
		objectList = map(Obj, objList)
		return objectList
	
	def __str__(self):
		return self.obj
	
	def __repr__(self):
		return self.obj

	def __del__(self):
		print 'del', self.obj
		

#----------------------------------------------------------------------

class pyTree(wx.wxTreeCtrl):

	def __init__(self, parent, id, obj):
		wx.wxTreeCtrl.__init__(self, parent, id)
		self.root = self.AddRoot(obj.Name(), -1, -1, wx.wxTreeItemData(''))
		self.SetPyData(self.root, obj)
		if obj.HasChildren():
			self.SetItemHasChildren(self.root, wx.TRUE)
		wx.EVT_TREE_ITEM_EXPANDING(self, self.GetId(), self.OnItemExpanding)
		wx.EVT_TREE_ITEM_COLLAPSED(self, self.GetId(), self.OnItemCollapsed)
		wx.EVT_TREE_SEL_CHANGED(self, self.GetId(), self.OnSelChanged)
		self.output = None

	def SetOutput(self, output):
		self.output = output

	def OnItemExpanding(self,event):
		item = event.GetItem()
		obj = self.GetPyData(item)
		children = obj.Children()
		for child in children:
			new_item = self.AppendItem(item, child.Name(), -1, -1,
									   wx.wxTreeItemData(''))
			self.SetPyData(new_item, child)
			if child.HasChildren():
				self.SetItemHasChildren(new_item, wx.TRUE)

	def OnItemCollapsed(self, event):
		item = event.GetItem()
		self.DeleteChildren(item)

	def OnSelChanged(self, event):
		if not self.output:
			return
		obj = self.GetPyData( event.GetItem() )
		apply(self.output, (`obj`,))



#----------------------------------------------------------------------
if __name__ == '__main__':

	class MyFrame(wx.wxFrame):

		def __init__(self):
			wx.wxFrame.__init__(self, wx.NULL, -1, 'PyTreeItemData Test',
			 					wx.wxDefaultPosition, wx.wxSize(600,500))
			split = wx.wxSplitterWindow(self, -1)
			
			if sys.platform == 'win32':
				tree = pyTree(split, -1, Obj('C:\\'))
			else:
				tree = pyTree(split, -1, Obj('/'))
				
			text = wx.wxTextCtrl(split, -1, '', wx.wxDefaultPosition,
								 wx.wxDefaultSize, wx.wxTE_MULTILINE)
			split.SplitVertically(tree, text, 200)
			tree.SetOutput(text.SetValue)
			tree.SelectItem(tree.root)

	class MyApp(wx.wxApp):
		 
		def OnInit(self):
			frame = MyFrame()
			frame.Show(wx.TRUE)
			self.SetTopWindow(frame)
			return wx.TRUE

	app = MyApp(0)
	app.MainLoop()


