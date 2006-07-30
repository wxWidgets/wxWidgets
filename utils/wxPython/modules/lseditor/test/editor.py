from wxPython.wx import *
from wxPython.lseditor import *

class LSEditorFrame(wxFrame):
	def __init__(self, parent, id, title, pos = wxDefaultPosition, size = wxSize(400,400)):
		wxFrame.__init__(self, parent, id, title, pos, size)
		self.editor = wxsLSEditorPlugin()
		self.editor.Create(self, -1)
		self.SetMenuBar(self.GetDefaultMenuBar())		
		self.CreateStatusBar()
		self.SetDefaultEvents()
		self.filename = ""
	def GetDefaultMenuBar(self):
		mbar = wxMenuBar()
		menu = wxMenu()
		menu.Append(1500, "Open")
		menu.Append(1501, "Save")
		menu.Append(1502, "SaveAs")
		menu.AppendSeparator()
		menu.Append(1503, "Exit")
		mbar.Append(menu, "File")
		menu = wxMenu()
		menu.Append(1510, "Copy")
		menu.Append(1511, "Cut")
		menu.Append(1512, "Paste")
		menu.AppendSeparator()
		menu.Append(1513, "Delete")
		menu.AppendSeparator()
		menu.Append(1514, "Undo")
		menu.Append(1515, "Redo")
		menu.AppendSeparator()
		menu.Append(1516, "Find...")
		menu.Append(1517, "Find Next")
		menu.Append(1518, "Find Previous")
		menu.Append(1519, "Replace...")
		mbar.Append(menu, "Edit")	
		menu = wxMenu()
		menu.Append(1520, "Toggle")
		menu.Append(1521, "Next")
		menu.Append(1522, "Prev")
		mbar.Append(menu, "Bookmarks")
		return mbar
	def SetDefaultEvents(self):	
		EVT_MENU(self, 1500, self.evt_OnOpen)
		EVT_MENU(self, 1501, self.evt_OnSave)
		EVT_MENU(self, 1502, self.evt_OnSaveAs)
		EVT_MENU(self, 1503, self.OnClose)
		EVT_MENU(self, 1510, self.evt_OnCopy)
		EVT_MENU(self, 1511, self.evt_OnCut)
		EVT_MENU(self, 1512, self.evt_OnPaste)
		EVT_MENU(self, 1513, self.evt_OnDelete)
		EVT_MENU(self, 1514, self.evt_OnUndo)
		EVT_MENU(self, 1515, self.evt_OnRedo)
		EVT_MENU(self, 1516, self.evt_OnFind)
		EVT_MENU(self, 1517, self.evt_OnFindNext)
		EVT_MENU(self, 1518, self.evt_OnFindPrevious)
		EVT_MENU(self, 1519, self.evt_OnReplace)
		EVT_MENU(self, 1520, self.evt_OnToggle)
		EVT_MENU(self, 1521, self.evt_OnNext)
		EVT_MENU(self, 1522, self.evt_OnPrev)
		#EVT_MENU(self, 15, self.evt_)
		#EVT_MENU(self, 15, self.evt_)
	def evt_OnOpen(self, event):
		dlg = wxFileDialog(NULL, "Open file")
		if dlg.ShowModal() == wxID_OK:
			self.filename = dlg.GetPath()
			self.editor.OnOpen(self.filename)
	def evt_OnSaveAs(self, event):
		dlg = wxFileDialog(NULL, "Save As", self.filename)
		if dlg.ShowModal() == wxID_OK:
			self.filename = dlg.GetPath()
			self.editor.OnSave(self.filename)
	def evt_OnSave(self, event):
		if self.filename:
			self.editor.OnSave(self.filename)
		else:
			self.evt_OnSaveAs(None)
	def OnClose(self,event):
		self.Destroy()
	def evt_OnCopy(self,event):
		self.editor.OnCopy()
	def evt_OnCut(self,event):
		self.editor.OnCut()
	def evt_OnPaste(self,event):
		self.editor.OnPaste()
	def evt_OnDelete(self,event):
		self.editor.OnDelete()
	def evt_OnUndo(self,event):
		self.editor.OnUndo()
	def evt_OnRedo(self,event):
		self.editor.OnRedo()
	def evt_OnToggle(self,event):
		self.editor.OnToggleBookmark()
	def evt_OnNext(self,event):
		self.editor.OnNextBookmark()
	def evt_OnPrev(self,event):
		self.editor.OnPreviousBookmark()
	def evt_OnFind(self,event):
		self.editor.OnFind()
	def evt_OnFind(self,event):
		self.editor.OnFind()
	def evt_OnFindNext(self,event):
		self.editor.OnFindNext()
	def evt_OnFindPrevious(self,event):
		self.editor.OnFindPrevious()
		self.SetStatusText("OnFindPrevious: Not implemented")
	def evt_OnReplace(self,event):
		self.editor.OnReplace()
		self.SetStatusText("OnReplace: Not implemented")

class MyApp(wxApp):
	def OnInit(self):
		frame = LSEditorFrame(NULL, -1, "Editor")
		frame.Show(TRUE)
		return TRUE

App = MyApp(0)
App.MainLoop()
