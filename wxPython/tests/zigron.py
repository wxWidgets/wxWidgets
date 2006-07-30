#!/bin/env python
#----------------------------------------------------------------------------
## import all of the wxPython GUI package
from wxPython.wx import *


#---------------------------------------------------------------------------
class GeneralTab(wxWindow):
    def __init__(self,parent,id):
        wxWindow.__init__(self,parent,id,wxPoint(5,25))
        self.Opts = {}
        hdr = wxStaticText(self,-1,"This space left intentionally blank.",wxPoint(5,10))
    def GetOpts(self):
        return self.Opts

class ServersTab(wxWindow):
    def __init__(self,parent,id):
        wxWindow.__init__(self,parent,id,wxPoint(5,25))
        hdr = wxStaticText(self,-1,"This is also blank on purpose.",wxPoint(5,10))
        self.Opts = {}
    def GetOpts(self):
        return self.Opts

class OptionsTab(wxWindow):
    def __init__(self,parent,id):
        wxWindow.__init__(self,parent,id,wxPoint(5,25))
        hdr = wxStaticText(self,-1,"Quit bugging me!.",wxPoint(5,10))
        self.Opts = {}
    def GetOpts(self):
        return self.Opts

class SettingsWindow(wxFrame):
    NOTEBOOK   = 3201
    GENERAL_TAB  = 3210
    OPTIONS_TAB  = 3211
    SERVERS_TAB  = 3212

    def __init__(self,parent,id):
        self.id = id
        self.parent = parent
        wxFrame.__init__(self,parent,id,'Pyces  Settings',
                          wxPoint(50,50), wxSize(350,475),
                          wxDIALOG_MODAL|wxSTATIC_BORDER|wxCAPTION|wxSYSTEM_MENU)
        nb = wxNotebook(self, self.NOTEBOOK)
        self.GeneralTab = GeneralTab(self,-1)
        self.OptionsTab = OptionsTab(self,-1)
        self.ServersTab = ServersTab(self,-1)
        nb.AddPage(self.GeneralTab,'General')
        nb.AddPage(self.OptionsTab,'Options')
        nb.AddPage(self.ServersTab,'Servers')
        nb.SetSelection(0)
        nb.SetSize(wxSize(350,420))



#---------------------------------------------------------------------------

class MyApp(wxApp):
    def OnInit(self):
        frame = SettingsWindow(NULL, -1)
        #frame.ShowModal()
        #return false
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

#---------------------------------------------------------------------------


app = MyApp(0)     # Create an instance of the application class
app.MainLoop()     # Tell it to start processing events

#----------------------------------------------------------------------------
#

