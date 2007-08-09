
import wx 

#----------------------------------------------------------------------


stockIDs = [
    wx.ID_ABOUT,
    wx.ID_ADD,
    wx.ID_APPLY,
    wx.ID_BOLD,
    wx.ID_CANCEL,
    wx.ID_CLEAR,
    wx.ID_CLOSE,
    wx.ID_COPY,
    wx.ID_CUT,
    wx.ID_DELETE,
    wx.ID_EDIT,
    wx.ID_FIND,
    wx.ID_FILE,
    wx.ID_REPLACE,
    wx.ID_BACKWARD,
    wx.ID_DOWN,
    wx.ID_FORWARD,
    wx.ID_UP,
    wx.ID_HELP,
    wx.ID_HOME,
    wx.ID_INDENT,
    wx.ID_INDEX,
    wx.ID_ITALIC,
    wx.ID_JUSTIFY_CENTER,
    wx.ID_JUSTIFY_FILL,
    wx.ID_JUSTIFY_LEFT,
    wx.ID_JUSTIFY_RIGHT,
    wx.ID_NEW,
    wx.ID_NO,
    wx.ID_OK,
    wx.ID_OPEN,
    wx.ID_PASTE,
    wx.ID_PREFERENCES,
    wx.ID_PRINT,
    wx.ID_PREVIEW,
    wx.ID_PROPERTIES,
    wx.ID_EXIT,
    wx.ID_REDO,
    wx.ID_REFRESH,
    wx.ID_REMOVE,
    wx.ID_REVERT_TO_SAVED,
    wx.ID_SAVE,
    wx.ID_SAVEAS,
    wx.ID_SELECTALL,
    wx.ID_STOP,
    wx.ID_UNDELETE,
    wx.ID_UNDERLINE,
    wx.ID_UNDO,
    wx.ID_UNINDENT,
    wx.ID_YES,
    wx.ID_ZOOM_100,
    wx.ID_ZOOM_FIT,
    wx.ID_ZOOM_IN,
    wx.ID_ZOOM_OUT,

    ]

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sizer = wx.FlexGridSizer(cols=5, hgap=4, vgap=4)
        for ID in stockIDs:
            b = wx.Button(self, ID)
            sizer.Add(b)

        self.SetSizer(sizer)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Stock Buttons</center></h2>

It is now possible to create \"stock\" buttons.  Basically this means
that you only have to provide one of the stock IDs (and an empty
label) when creating the button and wxWidgets will choose the stock
label to go with it automatically.  Additionally on the platforms that
have a native concept of a stock button (currently only GTK2) then the
native stock button will be used.

<p>This sample shows buttons for all of the currenlty available stock
IDs.  Notice that when the button is created that no label is given,
and compare that with the button that is created.


</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

