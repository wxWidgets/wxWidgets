#----------------------------------------------------------------------
# Name:        wxPython.lib.splashscreen
# Purpose:     A simple frame that can display a bitmap and closes itself
#              after a specified timeout or a mouse click.
#
# Author:      Mike Fletcher, Robin Dunn
#
# Created:     19-Nov-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from wxPython.wx import *

#----------------------------------------------------------------------

def bitmapFromFile(filename):
    '''Non-portable test for bitmap type...'''
    import imghdr
    BITMAPTYPEGUESSDICT = {
        "bmp" :wxBITMAP_TYPE_BMP,
        "png" :wxBITMAP_TYPE_PNG,
        "jpeg":wxBITMAP_TYPE_JPEG,
        "gif" :wxBITMAP_TYPE_GIF,
        "xbm" :wxBITMAP_TYPE_XBM,
    }
    # following assumes bitmap type if we cannot resolve image type
    typ = BITMAPTYPEGUESSDICT.get(imghdr.what(filename), wxBITMAP_TYPE_BMP)
    bitmap = wxImage(filename, typ).ConvertToBitmap()
    return bitmap

#----------------------------------------------------------------------

class SplashScreen(wxFrame):
    def __init__(self, parent, ID=-1, title="SplashScreen",
                 style=wxSIMPLE_BORDER|wxSTAY_ON_TOP,
                 duration=1500, bitmapfile="bitmaps/splashscreen.bmp",
                 callback = None):
        '''
        parent, ID, title, style -- see wxFrame
        duration -- milliseconds to display the splash screen
        bitmapfile -- absolute or relative pathname, extension used for type negotiation
        callback -- if specified, is called when timer completes, callback is responsible for closing the splash screen
        '''
        ### Loading bitmap
        self.bitmap = bmp = bitmapFromFile(bitmapfile)
        ### Determine size of bitmap to size window...
        size = (bmp.GetWidth(), bmp.GetHeight())
        # size of screen
        width = wxSystemSettings_GetSystemMetric(wxSYS_SCREEN_X)
        height = wxSystemSettings_GetSystemMetric(wxSYS_SCREEN_Y)
        pos = ((width-size[0])/2, (height-size[1])/2)

        # check for overflow...
        if pos[0] < 0:
            size = (wxSystemSettings_GetSystemMetric(wxSYS_SCREEN_X), size[1])
        if pos[1] < 0:
            size = (size[0], wxSystemSettings_GetSystemMetric(wxSYS_SCREEN_Y))

        wxFrame.__init__(self, parent, ID, title, pos, size, style)
        self.Show(true)
        dc = wxClientDC(self)
        dc.DrawBitmap(self.bitmap, 0,0, false)

        class SplashTimer(wxTimer):
            def __init__(self, targetFunction):
                self.Notify = targetFunction
                wxTimer.__init__(self)

        if callback is None:
            callback = self.OnSplashExitDefault

        self.timer = SplashTimer(callback)
        self.timer.Start(duration, 1) # one-shot only
        EVT_LEFT_DOWN(self, self.OnMouseClick)

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        dc.DrawBitmap(self.bitmap, 0,0, false)


    def OnSplashExitDefault(self, event=None):
        self.Close(true)


    def OnCloseWindow(self, event=None):
        self.Show(false)
        self.timer.Stop()
        del self.timer
        self.Destroy()


    def OnMouseClick(self, event):
        self.timer.Notify()

#----------------------------------------------------------------------


if __name__ == "__main__":
    class DemoApp(wxApp):
        def OnInit(self):
            wxImage_AddHandler(wxJPEGHandler())
            wxImage_AddHandler(wxPNGHandler())
            wxImage_AddHandler(wxGIFHandler())
            self.splash = SplashScreen(NULL, bitmapfile="splashscreen.jpg", callback=self.OnSplashExit)
            self.splash.Show(true)
            self.SetTopWindow(self.splash)
            return true
        def OnSplashExit(self, event=None):
            print "Yay! Application callback worked!"
            self.splash.Close(true)
            del self.splash
            ### Build working windows here...
    def test(sceneGraph=None):
        app = DemoApp(0)
        app.MainLoop()
    test()
