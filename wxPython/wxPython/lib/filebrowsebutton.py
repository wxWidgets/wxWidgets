#----------------------------------------------------------------------
# Name:        wxPython.lib.filebrowsebutton
# Purpose:     Composite controls that provide a Browse button next to
#              either a wxTextCtrl or a wxComboBox.  The Browse button
#              launches a wxFileDialog and loads the result into the
#              other control.
#
# Author:      Mike Fletcher
#
# RCS-ID:      $Id$
# Copyright:   (c) 2000 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from wxPython.wx import *
import os

#----------------------------------------------------------------------

class FileBrowseButton(wxPanel):
    """ A control to allow the user to type in a filename
    or browse with the standard file dialog to select file

    __init__ (
        parent, id, pos, size -- passed directly to wxPanel initialisation
        style = wxTAB_TRAVERSAL -- passed directly to wxPanel initialisation
        labelText -- Text for label to left of text field
        buttonText -- Text for button which launches the file dialog
        toolTip -- Help text
        dialogTitle -- Title used in file dialog
        startDirectory -- Default directory for file dialog startup
        fileMask -- File mask (glob pattern, such as *.*) to use in file dialog
        fileMode -- wxOPEN or wxSAVE, indicates type of file dialog to use
        changeCallback -- callback receives all changes in value of control
    )
    GetValue() -- retrieve current value of text control
    SetValue(string) -- set current value of text control
    label -- pointer to internal label widget
    textControl -- pointer to internal text control
    browseButton -- pointer to button
    """
    def __init__ (self, parent, id= -1,
              pos = wxDefaultPosition, size = wxDefaultSize,
              style = wxTAB_TRAVERSAL,
              labelText= "File Entry:",
              buttonText= "Browse",
              toolTip= "Type filename or click browse to choose file",
              # following are the values for a file dialog box
              dialogTitle = "Choose a file",
              startDirectory = ".",
              initialValue = "",
              fileMask = "*.*",
              fileMode = wxOPEN,
              # callback for when value changes (optional)
              changeCallback= lambda x:x
        ):
        # store variables
        self.labelText = labelText
        self.buttonText = buttonText
        self.toolTip = toolTip
        self.dialogTitle = dialogTitle
        self.startDirectory = startDirectory
        self.initialValue = initialValue
        self.fileMask = fileMask
        self.fileMode = fileMode
        self.changeCallback = changeCallback

        # create the dialog
        self.createDialog(parent, id, pos, size, style )
        # Setting a value causes the changeCallback to be called.
        # In this case that would be before the return of the
        # constructor. Not good. So a default value on
        # SetValue is used to disable the callback
        self.SetValue( initialValue, 0)

    def createDialog( self, parent, id, pos, size, style ):
        """Setup the graphic representation of the dialog"""
        wxPanel.__init__ (self, parent, id, pos, size, style)

        box = wxBoxSizer(wxHORIZONTAL)

        self.label = self.createLabel( )
        box.Add( self.label, 0, wxCENTER )

        self.textControl = self.createTextControl()
        box.Add( self.textControl, 1, wxLEFT|wxCENTER, 5)

        self.browseButton = self.createBrowseButton()
        box.Add( self.browseButton, 0, wxCENTER)

        # add a border around the whole thing and resize the panel to fit
        outsidebox = wxBoxSizer(wxVERTICAL)
        outsidebox.Add(box, 1, wxEXPAND|wxALL, 3)
        outsidebox.Fit(self)

        self.SetAutoLayout(true)
        self.SetSizer( outsidebox )
        self.Layout()
        if size.width != -1 or size.height != -1:
            self.SetSize(size)

    def createLabel( self ):
        """Create the label/caption"""
        label = wxStaticText(self, -1, self.labelText, style =wxALIGN_RIGHT )
        font = label.GetFont()
        w, h, d, e = self.GetFullTextExtent(self.labelText, font)
        label.SetSize(wxSize(w+5, h))
        return label

    def createTextControl( self):
        """Create the text control"""
        ID = wxNewId()
        textControl = wxTextCtrl(self, ID)
        textControl.SetToolTipString( self.toolTip )
        if self.changeCallback:
            EVT_TEXT(textControl, ID, self.changeCallback)
            EVT_COMBOBOX(textControl, ID, self.changeCallback)
        return textControl

    def createBrowseButton( self):
        """Create the browse-button control"""
        ID = wxNewId()
        button =wxButton(self, ID, self.buttonText)
        button.SetToolTipString( self.toolTip )
        EVT_BUTTON(button, ID, self.OnBrowse)
        return button

    def OnBrowse (self, event = None):
        """ Going to browse for file... """
        current = self.GetValue ()
        directory = os.path.split(current)
        if os.path.isdir( current):
            directory = current
        elif directory and os.path.isdir( directory[0] ):
            directory = directory [0]
        else:
            directory = self.startDirectory
        dlg = wxFileDialog(self, self.dialogTitle, directory, current, self.fileMask, self.fileMode)
        if dlg.ShowModal() == wxID_OK:
            self.SetValue (dlg.GetPath())
        dlg.Destroy()

    def GetValue (self):
        """ Convenient access to text control value """
        return self.textControl.GetValue ()

    def SetValue (self, value, callBack=1):
        """ Convenient setting of text control value """
        # Removed the return from here because SetValue doesn't return anything.
        self.textControl.SetValue (value)

    def Enable (self, value):
        """ Convenient enabling/disabling of entire control """
        self.label.Enable (value)
        self.textControl.Enable (value)
        return self.browseButton.Enable (value)

    def GetLabel( self ):
        """ Retrieve the label's current text """
        return self.label.GetLabel()

    def SetLabel( self, value ):
        """ Set the label's current text """
        rvalue = self.label.SetLabel( value )
        self.Refresh( true )
        return rvalue



class FileBrowseButtonWithHistory( FileBrowseButton ):
    """ with following additions:
        __init__(..., history=None)

            history -- optional list of paths for initial history drop-down
                (must be passed by name, not a positional argument)
                If history is callable it will must return a list used
                for the history drop-down
            changeCallback -- as for FileBrowseButton, but with a work-around
                for win32 systems which don't appear to create EVT_COMBOBOX
                events properly.  There is a (slight) chance that this work-around
                will cause some systems to create two events for each Combobox
                selection. If you discover this condition, please report it!
            As for a FileBrowseButton.__init__ otherwise.
        GetHistoryControl()
            Return reference to the control which implements interfaces
            required for manipulating the history list.  See GetHistoryControl
            documentation for description of what that interface is.
        GetHistory()
            Return current history list
        SetHistory( value=(), selectionIndex = None )
            Set current history list, if selectionIndex is not None, select that index
        """
    def __init__( self, *arguments, **namedarguments):
        self.history = namedarguments.get( "history" )
        if self.history:
            del namedarguments["history"]

        self.historyCallBack=None
        if callable(self.history):
            self.historyCallBack=self.history
            self.history=None
        apply( FileBrowseButton.__init__, ( self,)+arguments, namedarguments)

    def createTextControl( self):
        """Create the text control"""
        ID = wxNewId()
        textControl = wxComboBox(self, ID, style = wxCB_DROPDOWN )
        textControl.SetToolTipString( self.toolTip )
        EVT_SET_FOCUS(textControl, self.OnSetFocus)
        if self.changeCallback:
            EVT_TEXT(textControl, ID, self.changeCallback)
            EVT_COMBOBOX(textControl, ID, self.changeCallback)
        if self.history:
            history=self.history
            self.history=None
            self.SetHistory( history, control=textControl)
        return textControl

    def GetHistoryControl( self ):
        """Return a pointer to the control which provides (at least)
        the following methods for manipulating the history list.:
            Append( item ) -- add item
            Clear() -- clear all items
            Delete( index ) -- 0-based index to delete from list
            SetSelection( index ) -- 0-based index to select in list
        Semantics of the methods follow those for the wxComboBox control
        """
        return self.textControl

    def SetHistory( self, value=(), selectionIndex = None, control=None ):
        """Set the current history list"""
        if control is None:
            control = self.GetHistoryControl()
        if self.history == value:
            return
        self.history = value
        # Clear history values not the selected one.
        tempValue=control.GetValue()
        # clear previous values
        control.Clear()
        control.SetValue(tempValue)
        # walk through, appending new values
        for path in value:
            control.Append( path )
        if selectionIndex is not None:
            control.SetSelection( selectionIndex )

    def GetHistory( self ):
        """Return the current history list"""
        if self.historyCallBack != None:
            return self.historyCallBack()
        else:
            return list( self.history )

    def OnSetFocus(self, event):
        """When the history scroll is selected, update the history"""
        if self.historyCallBack != None:
            self.SetHistory( self.historyCallBack(), control=self.textControl)
        event.Skip()

    if wxPlatform == "__WXMSW__":
        def SetValue (self, value, callBack=1):
            """ Convenient setting of text control value, works
                around limitation of wxComboBox """
            # Removed the return from here because SetValue doesn't return anything.
            self.textControl.SetValue (value)
            # Hack to call an event handler
            class LocalEvent:
                def __init__(self, string):
                    self._string=string
                def GetString(self):
                    return self._string
            if callBack==1:
                # The callback wasn't being called when SetValue was used ??
                # So added this explicit call to it
                self.changeCallback(LocalEvent(value))


class DirBrowseButton(FileBrowseButton):
    def __init__(self, parent, id = -1,
                 pos = wxDefaultPosition, size = wxDefaultSize,
                 style = wxTAB_TRAVERSAL,
                 labelText = 'Select a directory:',
                 buttonText = 'Browse',
                 toolTip = 'Type directory name or browse to select',
                 dialogTitle = '',
                 startDirectory = '.',
                 changeCallback = None,
                 dialogClass = wxDirDialog):
        FileBrowseButton.__init__(self, parent, id, pos, size, style,
                                  labelText, buttonText, toolTip,
                                  dialogTitle, startDirectory,
                                  changeCallback = changeCallback)
        #
        self._dirDialog = dialogClass(self,
                                      message = dialogTitle,
                                      defaultPath = startDirectory)
    #
    def OnBrowse(self, ev = None):
        dialog = self._dirDialog
        if dialog.ShowModal() == wxID_OK:
            self.SetValue(dialog.GetPath())
    #
    def __del__(self):
        if self.__dict__.has_key('_dirDialog'):
            self._dirDialog.Destroy()


#----------------------------------------------------------------------


if __name__ == "__main__":
    #from skeletonbuilder import rulesfile
    class SimpleCallback:
        def __init__( self, tag ):
            self.tag = tag
        def __call__( self, event ):
            print self.tag, event.GetString()
    class DemoFrame( wxFrame ):
        def __init__(self, parent):
            wxFrame.__init__(self, parent, 2400, "File entry with browse", size=(500,260) )
            EVT_CLOSE(self, self.OnCloseWindow)
            panel = wxPanel (self,-1)
            innerbox = wxBoxSizer(wxVERTICAL)
            control = FileBrowseButton(
                panel,
                initialValue = "z:\\temp",
            )
            innerbox.Add(  control, 0, wxEXPAND )
            middlecontrol = FileBrowseButtonWithHistory(
                panel,
                labelText = "With History",
                initialValue = "d:\\temp",
                history = ["c:\\temp", "c:\\tmp", "r:\\temp","z:\\temp"],
                changeCallback= SimpleCallback( "With History" ),
            )
            innerbox.Add( middlecontrol, 0, wxEXPAND )
            middlecontrol = FileBrowseButtonWithHistory(
                panel,
                labelText = "History callback",
                initialValue = "d:\\temp",
                history = self.historyCallBack,
                changeCallback= SimpleCallback( "History callback" ),
            )
            innerbox.Add( middlecontrol, 0, wxEXPAND )
            self.bottomcontrol = control = FileBrowseButton(
                panel,
                labelText = "With Callback",
                style = wxSUNKEN_BORDER|wxCLIP_CHILDREN ,
                changeCallback= SimpleCallback( "With Callback" ),
            )
            innerbox.Add(  control, 0, wxEXPAND)
            self.bottommostcontrol = control = DirBrowseButton(
                panel,
                labelText = "Simple dir browse button",
                style = wxSUNKEN_BORDER|wxCLIP_CHILDREN)
            innerbox.Add(  control, 0, wxEXPAND)
            ID = wxNewId()
            innerbox.Add( wxButton( panel, ID,"Change Label",  ), 1, wxEXPAND)
            EVT_BUTTON( self, ID, self.OnChangeLabel )
            ID = wxNewId()
            innerbox.Add( wxButton( panel, ID,"Change Value",  ), 1, wxEXPAND)
            EVT_BUTTON( self, ID, self.OnChangeValue )
            panel.SetAutoLayout(true)
            panel.SetSizer( innerbox )
            self.history={"c:\\temp":1, "c:\\tmp":1, "r:\\temp":1,"z:\\temp":1}

        def historyCallBack(self):
            keys=self.history.keys()
            keys.sort()
            return keys

        def OnFileNameChangedHistory (self, event):
            self.history[event.GetString ()]=1

        def OnCloseMe(self, event):
            self.Close(true)
        def OnChangeLabel( self, event ):
            self.bottomcontrol.SetLabel( "Label Updated" )
        def OnChangeValue( self, event ):
            self.bottomcontrol.SetValue( "r:\\somewhere\\over\\the\\rainbow.htm" )

        def OnCloseWindow(self, event):
            self.Destroy()

    class DemoApp(wxApp):
        def OnInit(self):
            wxImage_AddHandler(wxJPEGHandler())
            wxImage_AddHandler(wxPNGHandler())
            wxImage_AddHandler(wxGIFHandler())
            frame = DemoFrame(NULL)
            #frame = RulesPanel(NULL )
            frame.Show(true)
            self.SetTopWindow(frame)
            return true

    def test( ):
        app = DemoApp(0)
        app.MainLoop()
    print 'Creating dialog'
    test( )



