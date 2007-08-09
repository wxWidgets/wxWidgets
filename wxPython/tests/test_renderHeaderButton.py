
import wx
import cStringIO

#import os; print os.getpid(); raw_input("Press enter...")


class Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.SetFont(self.GetFont())
        r = wx.RendererNative.Get()
        rect = wx.Rect(40,10, 95,r.GetHeaderButtonHeight(self))
        #print rect
        
        # simple helper to make calling DrawHeaderButton so many times a
        # bit easier and less messy
        def dhb(idx, rect, flags=0, sort=0, params=None):
            dc.DrawText("%02d." % idx, rect.x-25, rect.y)
            r.DrawHeaderButton(self, dc, rect, flags, sort, params)
            rect.y += 30
            
        dhb(1, rect)
        dhb(2, rect, wx.CONTROL_SELECTED)
        dhb(3, rect, wx.CONTROL_CURRENT)
        dhb(4, rect, wx.CONTROL_SELECTED|wx.CONTROL_CURRENT)
        dhb(5, rect, 0, wx.HDR_SORT_ICON_UP)
        dhb(6, rect, 0, wx.HDR_SORT_ICON_DOWN)
        dhb(7, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP)
        dhb(8, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_DOWN)

        rect.x = 180
        rect.y = 10
        hp = wx.HeaderButtonParams()
        hp.m_labelText = "Hello"
        dhb(9, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(10, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_RIGHT
        dhb(11, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(12, rect, wx.CONTROL_SELECTED, 0, hp)
        dhb(13, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)
        hp.m_labelText = "This label is too long"
        dhb(14, rect, params=hp)
        dhb(15, rect, wx.CONTROL_SELECTED, 0, hp)
        dhb(16, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)

        rect.x = 320
        rect.y = 10
        hp = wx.HeaderButtonParams()
        hp.m_labelBitmap = getBitmap()
        dhb(17, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(18, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_RIGHT
        dhb(19, rect, params=hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(20, rect, wx.CONTROL_SELECTED, 0, hp)
        dhb(21, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)

        
        hp.m_labelText = "label"
        hp.m_labelAlignment = wx.ALIGN_LEFT
        dhb(22, rect, 0, 0, hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(23, rect, 0, 0, hp)
        hp.m_labelAlignment = wx.ALIGN_RIGHT
        dhb(24, rect, 0, 0, hp)

        rect.x = 460
        rect.y = 10
        hp.m_labelAlignment = wx.ALIGN_LEFT
        dhb(25, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(26, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)
        hp.m_labelAlignment = wx.ALIGN_RIGHT
        dhb(27, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)

        hp.m_labelText = "longer label"
        hp.m_labelAlignment = wx.ALIGN_LEFT
        dhb(28, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)
        hp.m_labelAlignment = wx.ALIGN_CENTER
        dhb(29, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)
        hp.m_labelAlignment = wx.ALIGN_RIGHT
        dhb(30, rect, wx.CONTROL_SELECTED, wx.HDR_SORT_ICON_UP, hp)

        
def getData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\xceIDAT8\x8d\x95\x93=h\x14A\x14\xc7\x7f\xfbq\xc7m\xce\x0b\xe4\xa2F/\
~\x04\x9b\x03O\x0bE\x10\x04\x91\x10\x88X\xa4J\xe7&\x8a\x10A\xc4Bb\xe9uje\xc0\
\xd2\xd2\xca\x94V\xa9\xa3X\x88i\x14\x0b\x91\x14\xae$dO\x8d\x97#\xf1<n/;\xb3k\
!;\xd9\xbd\xdd\x14\xfea\x9a7\xef\xff{\x8f7\xf34M7\x88+\x90"d\x1f\xe9\x86\xa9\
\xf5\xc7\xcc~\xf3\x8d\x87W\xb9pbR\xc5\xb6\xfc5\x00\xbe}_%\x90"\xec\x87\x98q\
\xf3\xfc\xc2,\xf7\xef=\x00\xa0\xc36\x8d\xf0\x03#\x0c\x00p\x9asL?:\x9b\x82\
\x98q\xf3\x8c=\xcbr\xf3E\xa2E\xcf\xf7\x01\x90\x81`|\xae\x96\x82\xe8\x81\x14\
\xe1\xf4\x9d\te\xf6|_\x9dN\xaf\x8b\x0c\x04\x83\xc6a\xa6F\xef\xe2\xfd\xb0\x18\
\x9f\xabq\xf1\xfa\xa8\x9a\x95\x1eUjt\xbf&*\xcb@\x000\x94\xab`W\xeb\x9c?:I^\
\xb7\x0084r0{\x88Q\xbb\x91\x8a\xc6\x10v\xb5\xcep\xa1\xc2\xe7\xd6;~\xcbM\xacP\
&r\x12\x80\x82V\xe2\xda\xc9\xdb\x1c\x19\x18\xe3\xe5\xeac&\x8e\xcf0\\\xa8\xb0\
\xe4<gy\xfd\x159\xd3\xc0\xc2\xe2\xd7\xcf\xa6\xf2\xe8q@^\xb7h\xf56(\xe5\xca\
\xd8\xd5:\xb5\xf2%e\xdeO\x89\x0e\xdc\xb6\x83\xdbv\xf8\xb8\xf9\x06\xbbZO\x99}\
!\x91\xa1\xc8\x06\xec\xc85\xda\x9d.\xa5\xa2\x85\xdbvx\xf2\xfef"\xd1\xeb\xed\
\x02`h\x07\x92\x00\xdd0\xb5@\x8ap\xea\xd6e\xae\xcc\x1f\xa3\xb1\xd5T\xc9\xfd\
\x1a;3\xc8\xeb\xa7\x0e+\x8b\xae\xfa\xd6&\xfc\xfb\xe3qHP\xfeCgg\xefEtS\xc3*\
\x9a\xbc]Xg\xe9\xd9\'v\xa5\xa7\xee\xb4\xf82E\x90\xfc\xa9\xedT\xf5\x8d/-V\x16\
]\xa2\x82\x99\x80\x08\x92\xd9?\xd9\xdb\x98\x02\xfc\xaf\xfe\x02\xb1S\xd1)\xa5\
\x1a\xc2\x99\x00\x00\x00\x00IEND\xaeB`\x82' 

def getBitmap():
    return wx.BitmapFromImage(getImage())

def getImage():
    stream = cStringIO.StringIO(getData())
    return wx.ImageFromStream(stream)


app = wx.App(False)
frm = wx.Frame(None, title="DrawHeaderButton Test", size=(580,300))
pnl = Panel(frm)
frm.Show()
app.MainLoop()

