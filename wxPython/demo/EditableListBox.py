
import  wx
import  wx.gizmos   as  gizmos

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        self.elb = gizmos.EditableListBox(
                    self, -1, "List of Stuff", (50,50), (250, 250)
                    )
                    #style=wx.EL_ALLOW_NEW | wx.EL_ALLOW_EDIT | wx.EL_ALLOW_DELETE)

        self.elb.SetStrings(["This is a nifty ListBox widget",
                             "that is editable by the user.",
                             "",
                             "Use the buttons above to",
                             "manipulate items in the list",
                             "Or to add new ones.",
                             ])



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """\
<html>
<body>
This class provides a composite control that lets the user easily enter and edit 
a list of strings.

<p><b>Styles supported:</b><p>

<ul>
<li><b>EL_ALLOW_NEW</b> - Allow user to create new items.
<li><b>EL_ALLOW_EDIT</b> - Allow user to edit text in the control.
<li><b>EL_ALLOW_DELETE</b> - Allow user to delete text from the control.
</ul>

<p><b>Init:</b>
<pre>
 EditableListBox(wxWindow *parent, wxWindowID id=-1,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = EL_ALLOW_NEW | EL_ALLOW_EDIT | EL_ALLOW_DELETE,
                 const wxString& name = "editableListBox")
</pre>

<p><b>Methods:</b>
<ul>
    <li><b>SetStrings(const wxArrayString& strings)</b> - Set an array of strings 
    into the control. <b>Note</b>: The wxPython method accepts a Python list instead 
    of an array of strings.
            
    <li><b>void GetStrings(wxArrayString& strings)</b> - Retrieves an array
    of strings from the control. The wxPython version returns a list of strings.

    <li><b>GetListCtrl()</b> - Retrieves a reference to the actual list control
    portion of the custom control.
    
    <li><b>GetDelButton()</b> - Retrieves a reference to the BitmapButton that is used
    as the 'delete' button in the control.
    
    <li><b>GetNewButton()</b> - Retrieves a reference to the BitmapButton that is used
    as the 'new' button in the control.

    <li><b>GetUpButton()</b> - Retrieves a reference to the BitmapButton that is used
    as the 'up' button in the control.

    <li><b>GetDownButton()</b> - Retrieves a reference to the BitmapButton that is used
    as the 'down' button in the control.
    
    <li><b>GetEditButton()</b> - Retrieves a reference to the BitmapButton that is used
    as the 'edit' button in the control.
</ul>
</body>
</html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

