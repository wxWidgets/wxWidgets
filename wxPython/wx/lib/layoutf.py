# 12/09/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 12/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxScrolledMessageDialog -> ScrolledMessageDialog
# 

import  re
import  wx

class Layoutf(wx.LayoutConstraints):
    """
The class Layoutf(wxLayoutConstraints) presents a simplification
of the wxLayoutConstraints syntax. The name Layoutf is choosen
because of the similarity with C's printf function.

Quick Example:

    lc = Layoutf('t=t#1;l=r10#2;r!100;h%h50#1', (self, self.panel))

is equivalent to

    lc = wxLayoutContraints()
    lc.top.SameAs(self, wxTop)
    lc.left.SameAs(self.panel, wxRight, 10)
    lc.right.Absolute(100)
    lc.height.PercentOf(self, wxHeight, 50)

Usage:

    You can give a constraint string to the Layoutf constructor,
or use the 'pack' method. The following are equivalent:

    lc = Layoutf('t=t#1;l=r#2;r!100;h%h50#1', (self, self.panel))

and

    lc = Layoutf()
    lc.pack('t=t#1;l=r#2;r!100;h%h50#1', (self, self.panel))

    Besides 'pack' there's also 'debug_pack' which does not set
constraints, but prints traditional wxLayoutConstraint calls to
stdout.

    The calls to the Layoutf constructor and pack methods have
the following argument list:

    (constraint_string, objects_tuple)

Constraint String syntax:

    Constraint directives are separated by semi-colons. You
generally (always?) need four directives to completely describe a
subwindow's location.

    A single directive has either of the following forms:

    1. <own attribute><compare operation>[numerical argument]
    for example r!100 -> lc.right.Absolute(100) )
    and w* -> lc.width.AsIs()

    2. <own attribute><compare operation>[numerical argument]
       #<compare object nr.>
    for example t_10#2 (lc.top.Below(<second obj>, 10)

    3. <own attribute><compare operation><compare attribute>
       [numerical argument]#<compare object nr.>
    for example w%h50#2 ( lc.width.PercentOf(<second obj>,
    wxHeight, 50) and t=b#1 ( lc.top.SameAs(<first obj>,
    wxBottom) )

    Which one you need is defined by the <compare operation>
type. The following take type 1 (no object to compare with):

    '!': 'Absolute', '?': 'Unconstrained', '*': 'AsIs'

These take type 2 (need to be compared with another object)

    '<': 'LeftOf', '>': 'RightOf', '^': 'Above', '_': 'Below'

These take type 3 (need to be compared to another object
attribute)

    '=': 'SameAs', '%': 'PercentOf'

For all types, the <own attribute> letter can be any of

    't': 'top', 'l': 'left', 'b': 'bottom',
    'r': 'right', 'h': 'height', 'w': 'width',
    'x': 'centreX', 'y': 'centreY'

If the operation takes an (optional) numerical argument, place it
in [numerical argument].  For type 3 directives, the <compare
attribute> letter can be any of

    't': 'wxTop', 'l': 'wxLeft', 'b': 'wxBottom'
    'r': 'wxRight', 'h': 'wxHeight', 'w': 'wxWidth',
    'x': 'wxCentreX', 'y': 'wxCentreY'

Note that these are the same letters as used for <own attribute>,
so you'll only need to remember one set. Finally, the object
whose attribute is refered to, is specified by #<compare object
nr>, where <compare object nr> is the 1-based (stupid, I know,
but I've gotten used to it) index of the object in the
objects_tuple argument.

Bugs:

Not entirely happy about the logic in the order of arguments
after the <compare operation> character.

Not all wxLayoutConstraint methods are included in the
syntax. However, the type 3 directives are generally the most
used. Further excuse: wxWindows layout constraints are at the
time of this writing not documented.

"""

    attr_d = { 't': 'top', 'l': 'left', 'b': 'bottom',
             'r': 'right', 'h': 'height', 'w': 'width',
             'x': 'centreX', 'y': 'centreY' }
    op_d = { '=': 'SameAs', '%': 'PercentOf', '<': 'LeftOf',
              '>': 'RightOf', '^': 'Above', '_': 'Below',
               '!': 'Absolute', '?': 'Unconstrained', '*': 'AsIs' }
    cmp_d = { 't': 'wx.Top', 'l': 'wx.Left', 'b': 'wx.Bottom',
             'r': 'wx.Right', 'h': 'wx.Height', 'w': 'wx.Width',
             'x': 'wx.CentreX', 'y': 'wx.CentreY' }

    rexp1 = re.compile('^\s*([tlrbhwxy])\s*([!\?\*])\s*(\d*)\s*$')
    rexp2 = re.compile('^\s*([tlrbhwxy])\s*([=%<>^_])\s*([tlrbhwxy]?)\s*(\d*)\s*#(\d+)\s*$')

    def __init__(self,pstr=None,winlist=None):
        wx.LayoutConstraints.__init__(self)
        if pstr:
            self.pack(pstr,winlist)

    def pack(self, pstr, winlist):
        pstr = pstr.lower()
        for item in pstr.split(';'):
            m = self.rexp1.match(item)
            if m:
                g = list(m.groups())
                attr = getattr(self, self.attr_d[g[0]])
                func = getattr(attr, self.op_d[g[1]])
                if g[1] == '!':
                    func(int(g[2]))
                else:
                    func()
                continue
            m = self.rexp2.match(item)
            if not m: raise ValueError
            g = list(m.groups())
            attr = getattr(self, self.attr_d[g[0]])
            func = getattr(attr, self.op_d[g[1]])
            if g[3]: g[3] = int(g[3])
            else: g[3] = None;
            g[4] = int(g[4]) - 1
            if g[1] in '<>^_':
                if g[3]: func(winlist[g[4]], g[3])
                else: func(winlist[g[4]])
            else:
                cmp = eval(self.cmp_d[g[2]])
                if g[3]: func(winlist[g[4]], cmp, g[3])
                else: func(winlist[g[4]], cmp)

    def debug_pack(self, pstr, winlist):
        pstr = pstr.lower()
        for item in pstr.split(';'):
            m = self.rexp1.match(item)
            if m:
                g = list(m.groups())
                attr = getattr(self, self.attr_d[g[0]])
                func = getattr(attr, self.op_d[g[1]])
                if g[1] == '!':
                    print "%s.%s.%s(%s)" % \
                     ('self',self.attr_d[g[0]],self.op_d[g[1]],g[2])
                else:
                    print "%s.%s.%s()" % \
                     ('self',self.attr_d[g[0]],self.op_d[g[1]])
                continue
            m = self.rexp2.match(item)
            if not m: raise ValueError
            g = list(m.groups())
            if g[3]: g[3] = int(g[3])
            else: g[3] = 0;
            g[4] = int(g[4]) - 1
            if g[1] in '<>^_':
                if g[3]: print "%s.%s.%s(%s,%d)" % \
                 ('self',self.attr_d[g[0]],self.op_d[g[1]],winlist[g[4]],
                  g[3])
                else: print "%s.%s.%s(%s)" % \
                 ('self',self.attr_d[g[0]],self.op_d[g[1]],winlist[g[4]])
            else:
                if g[3]: print "%s.%s.%s(%s,%s,%d)" % \
                 ('self',self.attr_d[g[0]],self.op_d[g[1]],winlist[g[4]],
                  self.cmp_d[g[2]],g[3])
                else: print "%s.%s.%s(%s,%s)" % \
                 ('self',self.attr_d[g[0]],self.op_d[g[1]],winlist[g[4]],
                  self.cmp_d[g[2]])

if __name__=='__main__':

    class TestLayoutf(wx.Frame):
        def __init__(self, parent):
            wx.Frame.__init__(self, parent, -1, 'Test Layout Constraints',
                              wx.DefaultPosition, (500, 300))
            self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

            self.SetAutoLayout(True)

            self.panelA = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
            self.panelA.SetBackgroundColour(wx.BLUE)
            self.panelA.SetConstraints(Layoutf('t=t10#1;l=l10#1;b=b10#1;r%r50#1',(self,)))

            self.panelB = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
            self.panelB.SetBackgroundColour(wx.RED)
            self.panelB.SetConstraints(Layoutf('t=t10#1;r=r10#1;b%b30#1;l>10#2', (self,self.panelA)))

            self.panelC = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
            self.panelC.SetBackgroundColour(wx.WHITE)
            self.panelC.SetConstraints(Layoutf('t_10#3;r=r10#1;b=b10#1;l>10#2', (self,self.panelA,self.panelB)))

            b = wx.Button(self.panelA, -1, ' About: ')
            b.SetConstraints(Layoutf('X=X#1;Y=Y#1;h*;w%w50#1', (self.panelA,)))
            self.Bind(wx.EVT_BUTTON, self.OnAbout, b)

            b = wx.Button(self.panelB, 100, ' Panel B ')
            b.SetConstraints(Layoutf('t=t2#1;r=r4#1;h*;w*', (self.panelB,)))

            self.panelD = wx.Window(self.panelC, -1, style=wx.SIMPLE_BORDER)
            self.panelD.SetBackgroundColour(wx.GREEN)
            self.panelD.SetConstraints(Layoutf('b%h50#1;r%w50#1;h=h#2;w=w#2', (self.panelC, b)))

            b = wx.Button(self.panelC, -1, ' Panel C ')
            b.SetConstraints(Layoutf('t_#1;l>#1;h*;w*', (self.panelD,)))
            self.Bind(wx.EVT_BUTTON, self.OnButton, b)

            wx.StaticText(self.panelD, -1, "Panel D", (4, 4)).SetBackgroundColour(wx.GREEN)

        def OnButton(self, event):
            self.Close(True)

        def OnAbout(self, event):
            import  wx.lib.dialogs
            msg = wx.lib.dialogs.ScrolledMessageDialog(self, Layoutf.__doc__, "about")
            msg.ShowModal()
            msg.Destroy()

        def OnCloseWindow(self, event):
            self.Destroy()

    class TestApp(wx.App):
        def OnInit(self):
            frame = TestLayoutf(None)
            frame.Show(1)
            self.SetTopWindow(frame)
            return 1

    app = TestApp(0)
    app.MainLoop()





