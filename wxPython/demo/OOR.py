# 11/13/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
#

import  wx
import  wx.html as wxhtml

#----------------------------------------------------------------------

BTN1 = wx.NewId()
BTN2 = wx.NewId()

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        sizer = wx.BoxSizer(wx.VERTICAL)
        html = wxhtml.HtmlWindow(self, -1)
        html.SetPage(overview)
        sizer.Add(html, 1, wx.EXPAND|wx.ALL, 5)

        btns = wx.BoxSizer(wx.HORIZONTAL)
        btns.Add((50, -1), 1, wx.EXPAND)
        btn1 = wx.Button(self, BTN1, "Find My Alter-ego") # don't save a ref to this one 
        btns.Add(btn1)
        btns.Add((50, -1), 1, wx.EXPAND)
        self.btn2 = wx.Button(self, BTN2, "Find Myself")
        btns.Add(self.btn2)
        btns.Add((50, -1), 1, wx.EXPAND)

        sizer.Add(btns, 0, wx.EXPAND|wx.ALL, 15)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)

        self.sizer = sizer  # save it for testing later

        self.Bind(wx.EVT_BUTTON, self.OnFindButton1, id=BTN1)
        self.Bind(wx.EVT_BUTTON, self.OnFindButton2, id=BTN2)


    def OnFindButton1(self, evt):
        win = self.FindWindowById(BTN1)

        if win is None:
            self.log.write("***** OOPS! None returned...\n")
            return

        className = win.__class__.__name__

        if className in ["Button", "ButtonPtr"]:
            self.log.write("The types are the same! <grin>\n")
        else:
            self.log.write("Got %s, expected wxButton or wxButtonPtr\n" % className)



    def OnFindButton2(self, evt):
        win = self.FindWindowById(BTN2)

        if win is None:
            self.log.write("***** OOPS! None returned...\n")
            return

        if win is self.btn2:
            self.log.write("The objects are the same! <grin>\n")
        else:
            self.log.write("The objects are NOT the same! <frown>\n")

        win = evt.GetEventObject()

        if win is None:
            self.log.write("***** OOPS! None returned...\n")
            return

        if win is self.btn2:
            self.log.write("The objects are the same! <grin>\n")
        else:
            self.log.write("The objects are NOT the same! <frown>\n")

        sizer = self.GetSizer()

        if sizer is None:
            self.log.write("***** OOPS! None returned...\n")
            return

        if sizer is self.sizer:
            self.log.write("The objects are the same! <grin>\n")
        else:
            self.log.write("The objects are NOT the same! <frown>\n")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
<html><body>
<h2>Original Object Return</h2>

<p>Several methods in wxWindows return pointers to base class objects,
when in fact the actual object pointed to is of a derived type.  Since
SWIG isn't able to tell the actual type it just creates a new Python
shadow object of the base type to wrap around the base type pointer
and returns it.

<p>In wxPython prior to 2.3.0 this could cause annoying issues.  For
example if you called:

<pre>

        myText = someWindow.FindWindowById(txtID)
</pre>

expecting to get a wxTextCtrl you would actually get a wxWindow object
instead.  If you then try to call SetValue on that object you'll get
an exception since there is no such method.  This is the reason for
the wxPyTypeCast hack that has been in wxPython for so long.

<p>Even with wxPyTypeCast there was the issue that the object returned
was not the same one that was created in Python originally, but a new
object of the same type that wraps the same C++ pointer.  If the
programmer has set additional attributes of that original object they
will not exist in the new object.

<p>For a long time now I have wanted to do away with wxPyTypeCast and
also find a way to return the original Python object from methods like
FindWindowById.  This project naturally divides into two phases:

<p><ol>

<li>Teach the wrapper methods how to return objects of the right type,
and be able to then turn wxPyTypeCast in to a no-op.

<li>Be able to return the original Python shadow object if it still exists.

</ol>

<p>The first button below shows the first of these phases (<i>working</i>)
and the second will show #2 (<i>working as of Python 2.3.2</i>)

</body></html>
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

