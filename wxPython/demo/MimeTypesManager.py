#----------------------------------------------------------------------
# Name:        wxMimeTypesManager
# Purpose:     Demonstrate use of wx.MimeTypesManager, wx.FileType
#
# Author:      Jeff Grimmett (grimmtoo@softhome.net), adapted from original
#              .wdr-derived demo
#
# Created:     12/31/03
# RCS-ID:      $Id$
# Copyright:   
# Licence:     wxWindows license
#----------------------------------------------------------------------
#


import  pprint
import  wx
import  images

#----------------------------------------------------------------------------

class MimeTypesDemoPanel(wx.Panel):
    def __init__(self, parent, log):
        
        self.log = log
        
        wx.Panel.__init__(self, parent, -1)

        # This will be used for all of the labels that follow (bold label)
        bfont = wx.Font(
                    self.GetFont().GetPointSize(), 
                    self.GetFont().GetFamily(),
                    self.GetFont().GetStyle(),
                    wx.BOLD
                    )
        
        # Contains everything
        tsizer = wx.BoxSizer(wx.VERTICAL)
        
        # Contains upper controls
        usizer = wx.BoxSizer(wx.HORIZONTAL)

        # Text control for ext / type entry plus label.
        t = wx.StaticText(self, -1, 'Extension / MIME type: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        usizer.Add(t, 0, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 2)

        self.ext = wx.TextCtrl(self, -1, value="wav", style = wx.TE_PROCESS_ENTER )
        usizer.Add(self.ext, 0, wx.ALL | wx.ALIGN_TOP, 4)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnLookup, self.ext)

        # Select how to look it up
        self.useExt = wx.RadioButton(self, -1, "By extension", style = wx.RB_GROUP)
        self.useMime = wx.RadioButton(self, -1, "By MIME type")

        usizer.Add(self.useExt, 0, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 4)
        usizer.Add(self.useMime, 0, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 4)
        self.useExt.SetValue(1)

        # Trigger a lookup (hitting ENTER in the text ctrl will do the same thing)
        self.go = wx.Button(self, -1, "Go get it!")
        usizer.Add(self.go, 0, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 4)
        self.Bind(wx.EVT_BUTTON, self.OnLookup, self.go)

        # StaticBox with larger label than usual
        lbox = wx.StaticBox(self, -1, 'wx.FileType')
        lbox.SetFont(
            wx.Font(
                self.GetFont().GetPointSize() * 2, 
                self.GetFont().GetFamily(),
                self.GetFont().GetStyle(),
                wx.BOLD
                ))

        lsizer = wx.StaticBoxSizer(lbox, wx.HORIZONTAL)

        # Contains the wx.FileType info
        llsizer = wx.GridBagSizer(2, 2)
        llsizer.AddGrowableCol(2)

        #------- Icon info

        t = wx.StaticText(self, -1, 'GetIconInfo: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (0, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.icon = wx.StaticBitmap(self, -1, images.getNoIconBitmap())
        llsizer.Add(self.icon, (0, 1), (1, 1), wx.ALL | wx.ALIGN_CENTER, 2)

        self.iconsource = wx.TextCtrl(self, -1, value="", size=(125, -1), style = wx.TE_READONLY )
        llsizer.Add(self.iconsource, (0, 2), (1, 1), wx.ALL | wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL, 2)

        self.iconoffset = wx.TextCtrl(self, -1, value="", size=(25,-1), style = wx.TE_READONLY )
        llsizer.Add(self.iconoffset, (0, 3), (1, 1), wx.ALL | wx.ALIGN_CENTER_VERTICAL, 2)

        #------- MIME Type

        t = wx.StaticText(self, -1, 'GetMimeType: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (1, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.mimetype = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY )
        llsizer.Add(self.mimetype, (1, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- MIME Types

        t = wx.StaticText(self, -1, 'GetMimeTypes: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (2, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.mimetypes = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY )
        llsizer.Add(self.mimetypes, (2, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- Extensions

        t = wx.StaticText(self, -1, 'GetExtensions: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (3, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.extensions = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY )
        llsizer.Add(self.extensions, (3, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- Description

        t = wx.StaticText(self, -1, 'GetDescription: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (4, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.description = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY)
        llsizer.Add(self.description, (4, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- Open command

        t = wx.StaticText(self, -1, 'GetOpenCommand: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (5, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.opencommand = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY )
        llsizer.Add(self.opencommand, (5, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- Print command

        t = wx.StaticText(self, -1, 'GetPrintCommand: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (6, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.printcommand = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY )
        llsizer.Add(self.printcommand, (6, 1), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        #------- All commands

        t = wx.StaticText(self, -1, 'GetAllCommands: ', style = wx.ALIGN_RIGHT )
        t.SetFont(bfont)
        llsizer.Add(t, (7, 0), (1, 1), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 2)

        self.allcommands = wx.TextCtrl(self, -1, value="", style = wx.TE_READONLY | wx.TE_DONTWRAP | wx.TE_MULTILINE )

        # Set the default height to be smaller than normal (for
        # multi-line) so the sizer can then expand it to whatever
        # space is available
        self.allcommands.SetSize((-1, 20))
        
        llsizer.Add(self.allcommands, (7, 1), (1, 3), wx.ALL | wx.GROW | wx.ALIGN_CENTER, 2)

        # Tell the sizer to expand this row as needed
        llsizer.AddGrowableRow(7)
        
        #----------------------------------------------------------------------------

        lrsizer = wx.BoxSizer(wx.VERTICAL)
        
        #------- List box with known MIME types

        t = wx.StaticText(self, -1, 'Known MIME types')
        t.SetFont(bfont)
        lrsizer.Add(t, 0, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 4)

        self.mimelist = wx.ListBox(self, -1, choices=[], style = wx.LB_SINGLE | wx.LB_SORT)
        lrsizer.Add(self.mimelist, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER | wx.FIXED_MINSIZE, 4)
        self.Bind(wx.EVT_LISTBOX, self.OnListbox, self.mimelist)

        #----------------------------------------------------------------------------

        lsizer.Add(llsizer, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 4)
        lsizer.Add(lrsizer, 0, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 4)

        #----------------------------------------------------------------------------

        tsizer.Add(usizer, 0, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 4)
        tsizer.Add(lsizer, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 4)

        #----------------------------------------------------------------------------

        self.SetSizer(tsizer)
        tsizer.Fit(self)
        
        # Populate the Known MIME types list with what is in the database
        try:
            mtypes = wx.TheMimeTypesManager.EnumAllFileTypes()
        except wx.PyAssertionError:
            mtypes = []
        for mt in mtypes:
            self.mimelist.Append(mt)

        # Do a lookup of *.wav for a starting position
        self.OnLookup()

    # Grab the selection from the listbox, push that into
    # the text box at top, select 'MIME', and then look it up.
    def OnListbox(self, event):
        mimetype = event.GetString()
        self.ext.SetValue(mimetype)
        self.useMime.SetValue(1)
        self.OnLookup()

    # Look up a given file extension or MIME type.
    def OnLookup(self, event=None):
        txt = self.ext.GetValue()

        # For MIME lookups
        if self.useMime.GetValue() == 1:
            fileType = wx.TheMimeTypesManager.GetFileTypeFromMimeType(txt)
            msg = "Mime type"

            # Select the entered value in the list
            if fileType:
                if self.mimelist.FindString(txt) != -1:
                    self.mimelist.SetSelection(self.mimelist.FindString(txt))
        
        # Must be an extension lookup
        else:
            fileType = wx.TheMimeTypesManager.GetFileTypeFromExtension(txt)
            msg = "File extension"

            # Select the entered value in the list
            if fileType:
                if self.mimelist.FindString(str(fileType.GetMimeType())) != -1:
                    # Using CallAfter to ensure that GUI is ready before trying to
                    # select it (otherwise, it's selected but not visible)
                    wx.CallAfter(self.mimelist.SetSelection, self.mimelist.FindString(str(fileType.GetMimeType())))


        if fileType is None:
            wx.MessageBox(msg + " not found.", "Oops!")
        else:
            self.Update(fileType)

    # Populate the wx.FileType fields with actual values.
    def Update(self, ft):

        #------- Icon info
        info = ft.GetIconInfo()

        if info is None:
            bmp = images.getNoIconBitmap()
            self.icon.SetBitmap(bmp)
            self.iconsource.SetValue("")
            self.iconoffset.SetValue("")
        else:
            icon, file, idx = info
            if icon.Ok():
                self.icon.SetIcon(icon)
            else:
                bmp = images.getNoIconBitmap()
                self.icon.SetBitmap(bmp)                
            self.iconsource.SetValue(file)
            self.iconoffset.SetValue(str(idx))

        #------- MIME type
        self.mimetype.SetValue(str(ft.GetMimeType()))
        #------- MIME types
        self.mimetypes.SetValue(str(ft.GetMimeTypes()))
        #------- Associated extensions
        self.extensions.SetValue(str(ft.GetExtensions()))
        #------- Description of file type
        self.description.SetValue(str(ft.GetDescription()))

        #------- Prep a fake command line command
        extList = ft.GetExtensions()

        if extList:
            ext = extList[0]
            if ext[0] == ".": ext = ext[1:]
        else:
            ext = ""

        filename = "SPAM" + "." + ext
        mime = ft.GetMimeType() or ""

        #------- OPEN command
        cmd = ft.GetOpenCommand(filename, mime)
        self.opencommand.SetValue(str(cmd))

        #------- PRINT command
        cmd = ft.GetPrintCommand(filename, mime)
        self.printcommand.SetValue(str(cmd))

        #------- All commands
        all = ft.GetAllCommands(filename, mime)
        
        if all is None:
            self.allcommands.SetValue("")
        else:
            verbs, commands = all
            text = pprint.pformat(map(None, verbs, commands))
            self.allcommands.SetValue(text)
            

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MimeTypesDemoPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """\

The <b>wx.MimeTypesManager</b> class allows the application to retrieve the 
information about all known MIME types from a system-specific location and the 
filename extensions to the MIME types and vice versa. After initialization the 
methods <b>GetFileTypeFromMimeType()</b> and <b>GetFileTypeFromExtension()</b> 
may be called: they will return a <b>wx.FileType</b> object which may be further 
queried for file description, icon and other attributes.

A global instance of <b>wx.MimeTypesManager</b> is always available as
<b>wx.TheMimeTypesManager</b>. It is recommended to use this instance instead 
of creating your own because gathering MIME information may take quite a long 
on Unix systems.

This demo shows how to use wx.TheMimeTypesManager to list all known MIME types
and retrieve that information as a wx.FileType from either an extension or
MIME type.

For further information please consult the wxWindows documentation for
<b>wx.MimeTypesManager</b> and <b>wx.FileType</b>.

"""

#----------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
