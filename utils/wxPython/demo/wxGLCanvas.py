
from wxPython.wx       import *
try:
    from wxPython.glcanvas import *
    haveGLCanvas = true
except ImportError:
    haveGLCanvas = false

#----------------------------------------------------------------------

if not haveGLCanvas:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(frame, 'The wxGLCanvas has not been included with this build of wxPython!',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()

else:


    def runTest(frame, nb, log):
        #win = TestGLCanvas(nb)
        #return win
        win = wxFrame(frame, -1, "GL Cube", wxDefaultPosition, wxSize(400,300))
        canvas = TestGLCanvas(win)
        frame.otherWin = win
        win.Show(true)
        return None



    class TestGLCanvas(wxGLCanvas):
        def __init__(self, parent):
            wxGLCanvas.__init__(self, parent, -1)
            EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
            self.init = false

        def OnEraseBackground(self, event):
            pass # Do nothing, to avoid flashing.


        def OnSize(self, event):
            size = self.GetClientSize()
            if self.GetContext() != 'NULL':
                self.SetCurrent()
                glViewport(0, 0, size.width, size.height)


        def OnPaint(self, event):
            dc = wxPaintDC(self)

            ctx = self.GetContext()
            if ctx == "NULL": return

            self.SetCurrent()


            if not self.init:
                self.InitGL()
                self.init = true

            # clear color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            # draw six faces of a cube
            glBegin(GL_QUADS)
            glNormal3f( 0.0, 0.0, 1.0)
            glVertex3f( 0.5, 0.5, 0.5)
            glVertex3f(-0.5, 0.5, 0.5)
            glVertex3f(-0.5,-0.5, 0.5)
            glVertex3f( 0.5,-0.5, 0.5)

            glNormal3f( 0.0, 0.0,-1.0)
            glVertex3f(-0.5,-0.5,-0.5)
            glVertex3f(-0.5, 0.5,-0.5)
            glVertex3f( 0.5, 0.5,-0.5)
            glVertex3f( 0.5,-0.5,-0.5)

            glNormal3f( 0.0, 1.0, 0.0)
            glVertex3f( 0.5, 0.5, 0.5)
            glVertex3f( 0.5, 0.5,-0.5)
            glVertex3f(-0.5, 0.5,-0.5)
            glVertex3f(-0.5, 0.5, 0.5)

            glNormal3f( 0.0,-1.0, 0.0)
            glVertex3f(-0.5,-0.5,-0.5)
            glVertex3f( 0.5,-0.5,-0.5)
            glVertex3f( 0.5,-0.5, 0.5)
            glVertex3f(-0.5,-0.5, 0.5)

            glNormal3f( 1.0, 0.0, 0.0)
            glVertex3f( 0.5, 0.5, 0.5)
            glVertex3f( 0.5,-0.5, 0.5)
            glVertex3f( 0.5,-0.5,-0.5)
            glVertex3f( 0.5, 0.5,-0.5)

            glNormal3f(-1.0, 0.0, 0.0)
            glVertex3f(-0.5,-0.5,-0.5)
            glVertex3f(-0.5,-0.5, 0.5)
            glVertex3f(-0.5, 0.5, 0.5)
            glVertex3f(-0.5, 0.5,-0.5)
            glEnd()

            self.SwapBuffers()


        def InitGL(self):
            # set viewing projection
            glMatrixMode(GL_PROJECTION);
            glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 3.0);

            # position viewer
            glMatrixMode(GL_MODELVIEW);
            glTranslatef(0.0, 0.0, -2.0);

            # position object
            glRotatef(30.0, 1.0, 0.0, 0.0);
            glRotatef(30.0, 0.0, 1.0, 0.0);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);













overview = """\
"""





#----------------------------------------------------------------------

def _test():
    class MyApp(wxApp):
        def OnInit(self):
            frame = wxFrame(NULL, -1, "GL Cube", wxDefaultPosition, wxSize(400,300))
            win = TestGLCanvas(frame)
            frame.Show(TRUE)
            self.SetTopWindow(frame)
            return TRUE

    app = MyApp(0)
    app.MainLoop()

if __name__ == '__main__':
    _test()
