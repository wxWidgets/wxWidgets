import sys, string

from   wxPython.wx       import *
from   wxPython.html     import *
import wxPython.lib.wxpTag

#---------------------------------------------------------------------------

class MyAboutBox(wxDialog):
    text = '''
<html>
<body bgcolor="#AC76DE">
<center><table bgcolor="#458154" width="100%%" cellspacing="0"
cellpadding="0" border="1">
<tr>
    <td align="center"><h1>wxPython %s</h1>
    Running on Python %s
    </td>
</tr>
</table>

<p><b>wxPython</b> is a Python extension module that
encapsulates the wxWindows GUI classes.</p>

<p>This demo shows off some of the capabilities
of <b>wxPython</b>.  Select items from the menu or tree control,
sit back and enjoy.  Be sure to take a peek at the source code for each
demo item so you can learn how to use the classes yourself.</p>

<p><b>wxPython</b> is brought to you by <b>Robin Dunn</b> and<br>
<b>Total Control Software</b>, Copyright (c) 1997-2000.</p>

<p>
<font size="-1">Please see <i>license.txt</i> for licensing information.</font>
</p>

<p><wxp class="wxButton">
    <param name="label" value="Okay">
    <param name="id"    value="wxID_OK">
</wxp></p>
</center>
</body>
</html>
'''
    def __init__(self, parent):
        wxDialog.__init__(self, parent, -1, 'About the wxPython demo',)
        html = wxHtmlWindow(self, -1, size=(420, -1))
        py_version = string.split(sys.version)[0]
        html.SetPage(self.text % (wx.__version__, py_version))
        ir = html.GetInternalRepresentation()
        html.SetSize( (ir.GetWidth()+5, ir.GetHeight()+5) )
        self.SetClientSize(html.GetSize())
        self.CentreOnParent(wxBOTH)

#---------------------------------------------------------------------------






