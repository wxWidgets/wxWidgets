
from wxPython.wx       import *
try:
    from wxPython.glcanvas import *
    from OpenGL.GL import *
    from OpenGL.GLUT import *
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
        win = wxFrame(frame, -1, "GL Demos", wxDefaultPosition, wxSize(300,300))
        CubeCanvas(win)
        #MySplitter(win)
        frame.otherWin = win
        win.Show(true)
        return None


    class MySplitter(wxSplitterWindow):
        def __init__(self, parent):
            wxSplitterWindow.__init__(self, parent, -1)
            cube = CubeCanvas(self)
            cone = ConeCanvas(self)

            self.SplitVertically(cube, cone)
            self.SetSashPosition(300)



    class CubeCanvas(wxGLCanvas):
        def __init__(self, parent):
            wxGLCanvas.__init__(self, parent, -1)
            EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
            self.init = false

        def OnEraseBackground(self, event):
            pass # Do nothing, to avoid flashing.

        def OnSize(self, event):
            size = self.GetClientSize()
            if self.GetContext():
                self.SetCurrent()
                glViewport(0, 0, size.width, size.height)


        def OnPaint(self, event):
            dc = wxPaintDC(self)

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



    class ConeCanvas(wxGLCanvas):
        def __init__(self, parent):
            wxGLCanvas.__init__(self, parent, -1)
            EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
            self.init = false

        def OnEraseBackground(self, event):
            pass # Do nothing, to avoid flashing.

        def OnSize(self, event):
            size = self.GetClientSize()
            if self.GetContext():
                self.SetCurrent()
                glViewport(0, 0, size.width, size.height)

        def GLInit( self ):
            glMatrixMode(GL_PROJECTION);
            # camera frustrum setup
            glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 3.0);
            glMaterial(GL_FRONT, GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
            glMaterial(GL_FRONT, GL_DIFFUSE, [0.8, 0.8, 0.8, 1.0])
            glMaterial(GL_FRONT, GL_SPECULAR, [1.0, 0.0, 1.0, 1.0])
            glMaterial(GL_FRONT, GL_SHININESS, 50.0)
            glLight(GL_LIGHT0, GL_AMBIENT, [0.0, 1.0, 0.0, 1.0])
            glLight(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
            glLight(GL_LIGHT0, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
            glLight(GL_LIGHT0, GL_POSITION, [1.0, 1.0, 1.0, 0.0]);
            glLightModel(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
            glEnable(GL_LIGHTING)
            glEnable(GL_LIGHT0)
            glDepthFunc(GL_LESS)
            glEnable(GL_DEPTH_TEST)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)


        def OnPaint( self, event ):
            dc = wxPaintDC(self)
            if not self.init:
                self.GLInit()
                self.init = true

            ### Tell system to use _this_ glcanvas for all commands
            self.SetCurrent()
            # clear color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            # position viewer
            glMatrixMode(GL_MODELVIEW);
            # use a fresh transformation matrix
            glPushMatrix()
            # position object
            glTranslate(0.0, 0.0, -2.0);
            glRotate(30.0, 1.0, 0.0, 0.0);
            glRotate(30.0, 0.0, 1.0, 0.0);

            ### From cone.py
            glTranslate(0, -1, 0)
            glRotate(250, 1, 0, 0)
            glutSolidCone(1, 2, 50, 10)
            glPopMatrix()
            # push into visible buffer
            self.SwapBuffers()


#----------------------------------------------------------------------








overview = """\
"""





#----------------------------------------------------------------------

def _test():
    class MyApp(wxApp):
        def OnInit(self):
            frame = wxFrame(NULL, -1, "GL Demos", wxDefaultPosition, wxSize(600,300))
            #win = ConeCanvas(frame)
            MySplitter(frame)
            frame.Show(TRUE)
            self.SetTopWindow(frame)
            return TRUE

    app = MyApp(0)
    app.MainLoop()

if __name__ == '__main__':
    _test()
