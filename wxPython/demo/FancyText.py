  
import  wx
import  wx.lib.fancytext as fancytext

#----------------------------------------------------------------------

test_str = ('<font style="italic" family="swiss" color="red" weight="bold" >'
            'some  |<sup>23</sup> <angle/>text<sub>with <angle/> subscript</sub>'
            '</font> some other text')

test_str2 = '<font family="swiss" color="dark green" size="40">big green text</font>'


class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)

        w, h = fancytext.GetExtent(test_str, dc)
        fancytext.RenderToDC(test_str, dc, 20, 20)

        fancytext.RenderToDC(test_str2, dc, 20, 20 + h + 10)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win

#----------------------------------------------------------------------



overview = \
"""
<html>
<body>
<h1>FancyText -- <i>methods for rendering XML specified text</i></h1>

<p>This module exports four main methods::
<pre>
    def GetExtent(str, dc=None, enclose=True)
    def GetFullExtent(str, dc=None, enclose=True)
    def RenderToBitmap(str, background=None, enclose=True)
    def RenderToDC(str, dc, x, y, enclose=True)
</pre>

In all cases, 'str' is an XML string. Note that start and end tags
are only required if *enclose* is set to False. In this case the 
text should be wrapped in FancyText tags.

<p>In addition, the module exports one class::
<pre>
    class StaticFancyText(self, window, id, text, background, ...)
</pre>
This class works similar to StaticText except it interprets its text 
as FancyText.

<p>The text can support<sup>superscripts</sup> and <sub>subscripts</sub>, text
in different <font size="+3">sizes</font>, <font color="blue">colors</font>, 
<i>styles</i>, <b>weights</b> and
<font family="script">families</font>. It also supports a limited set of symbols,
currently <times/>, <infinity/>, <angle/> as well as greek letters in both
upper case (<Alpha/><Beta/>...<Omega/>) and lower case (<alpha/><beta/>...<omega/>).

</font></font>
The End
</body>
</html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

