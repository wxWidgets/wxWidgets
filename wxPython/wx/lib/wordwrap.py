#----------------------------------------------------------------------
# Name:        wx.lib.wrap
# Purpose:     Contains a function to aid in word-wrapping some text
#
# Author:      Robin Dunn
#
# Created:     15-Oct-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------



def wordwrap(text, width, dc, breakLongWords=True):
    """
    Returns a copy of text with newline characters inserted where long
    lines should be broken such that they will fit within the given
    width, on the given `wx.DC` using its current font settings.  By
    default words that are wider than width will be broken at the
    nearest character boundary, but this can be disabled by passing
    ``False`` for the ``breakLongWords`` parameter.
    """

    wrapped_lines = []
    text = text.split('\n')
    for line in text:
        pte = dc.GetPartialTextExtents(line)
        idx = 0
        start = 0
        startIdx = 0
        spcIdx = -1
        while idx < len(pte):
            # remember the last seen space
            if line[idx] == ' ':                
                spcIdx = idx

            # have we reached the max width?
            if pte[idx] - start > width and (spcIdx != -1 or breakLongWords):
                if spcIdx != -1:
                    idx = spcIdx + 1
                wrapped_lines.append( line[startIdx : idx] )
                start = pte[idx]
                startIdx = idx
                spcIdx = -1

            idx += 1

        wrapped_lines.append( line[startIdx : idx] )

    return '\n'.join(wrapped_lines)





if __name__ == '__main__':
    import wx
    class TestPanel(wx.Panel):
        def __init__(self, parent):
            wx.Panel.__init__(self, parent)

            self.tc = wx.TextCtrl(self, -1, "", (20,20), (150,150), wx.TE_MULTILINE)
            self.Bind(wx.EVT_TEXT, self.OnDoUpdate, self.tc)

        def OnDoUpdate(self, evt):
            WIDTH = 200
            bmp = wx.EmptyBitmap(WIDTH, WIDTH)
            mdc = wx.MemoryDC(bmp)
            mdc.SetBackground(wx.Brush("white"))
            mdc.Clear()
            mdc.SetPen(wx.Pen("black"))
            mdc.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.NORMAL))
            mdc.DrawRectangle(0,0, WIDTH, WIDTH)
            
            text = wordwrap(self.tc.GetValue(), WIDTH-2, mdc, False)
            #print repr(text)
            mdc.DrawLabel(text, (1,1, WIDTH-2, WIDTH-2))

            del mdc
            dc = wx.ClientDC(self)
            dc.DrawBitmap(bmp, 200, 20)
            

    app = wx.App(False)
    frm = wx.Frame(None, title="Test wordWrap")
    pnl = TestPanel(frm)
    frm.Show()
    app.MainLoop()

    
