from wxPython.wx       import *
try:
    from wxPython.glcanvas import *
    haveGLCanvas = True
except ImportError:
    haveGLCanvas = False

try:
    # The Python OpenGL package can be found at
    # http://PyOpenGL.sourceforge.net/
    from OpenGL.GL import *
    from OpenGL.GLUT import *
    haveOpenGL = True
except ImportError:
    haveOpenGL = False

#----------------------------------------------------------------------

if not haveGLCanvas:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(frame, 'The wxGLCanvas has not been included with this build of wxPython!',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()

elif not haveOpenGL:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(frame,
                              'The OpenGL package was not found.  You can get it at\n'
                              'http://PyOpenGL.sourceforge.net/',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()




else:
    buttonDefs = {
        wxNewId() : ('CubeCanvas',      'Cube'),
        wxNewId() : ('ConeCanvas',      'Cone'),
        }

    class ButtonPanel(wxPanel):
        def __init__(self, parent, log):
            wxPanel.__init__(self, parent, -1)
            self.log = log

            box = wxBoxSizer(wxVERTICAL)
            box.Add(20, 30)
            keys = buttonDefs.keys()
            keys.sort()
            for k in keys:
                text = buttonDefs[k][1]
                btn = wxButton(self, k, text)
                box.Add(btn, 0, wxALIGN_CENTER|wxALL, 15)
                EVT_BUTTON(self, k, self.OnButton)

            #** Enable this to show putting a wxGLCanvas on the wxPanel
            if 0:
                c = CubeCanvas(self)
                c.SetSize((200, 200))
                box.Add(c, 0, wxALIGN_CENTER|wxALL, 15)

            self.SetAutoLayout(True)
            self.SetSizer(box)


        def OnButton(self, evt):
            canvasClassName = buttonDefs[evt.GetId()][0]
            canvasClass = eval(canvasClassName)
            frame = wxFrame(None, -1, canvasClassName, size=(400,400))
            canvas = canvasClass(frame)
            frame.Show(True)



    def runTest(frame, nb, log):
        win = ButtonPanel(nb, log)
        return win




    class MyCanvasBase(wxGLCanvas):
        def __init__(self, parent):
            wxGLCanvas.__init__(self, parent, -1)
            self.init = False
            # initial mouse position
            self.lastx = self.x = 30
            self.lasty = self.y = 30
            EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
            EVT_SIZE(self, self.OnSize)
            EVT_PAINT(self, self.OnPaint)
            EVT_LEFT_DOWN(self, self.OnMouseDown)  # needs fixing...
            EVT_LEFT_UP(self, self.OnMouseUp)
            EVT_MOTION(self, self.OnMouseMotion)

        def OnEraseBackground(self, event):
            pass # Do nothing, to avoid flashing on MSW.

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
                self.init = True
            self.OnDraw()

        def OnMouseDown(self, evt):
            self.CaptureMouse()

        def OnMouseUp(self, evt):
            self.ReleaseMouse()

        def OnMouseMotion(self, evt):
            if evt.Dragging() and evt.LeftIsDown():
                self.x, self.y = self.lastx, self.lasty
                self.x, self.y = evt.GetPosition()
                self.Refresh(False)




    class CubeCanvas(MyCanvasBase):
        def InitGL(self):
            # set viewing projection
            glMatrixMode(GL_PROJECTION);
            glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 3.0);

            # position viewer
            glMatrixMode(GL_MODELVIEW);
            glTranslatef(0.0, 0.0, -2.0);

            # position object
            glRotatef(self.y, 1.0, 0.0, 0.0);
            glRotatef(self.x, 0.0, 1.0, 0.0);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);


        def OnDraw(self):
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

            glRotatef((self.lasty - self.y)/100., 1.0, 0.0, 0.0);
            glRotatef((self.lastx - self.x)/100., 0.0, 1.0, 0.0);

            self.SwapBuffers()





    class ConeCanvas(MyCanvasBase):
        def InitGL( self ):
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
            # position viewer
            glMatrixMode(GL_MODELVIEW);


        def OnDraw(self):
            # clear color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            # use a fresh transformation matrix
            glPushMatrix()
            # position object
            glTranslate(0.0, 0.0, -2.0);
            glRotate(30.0, 1.0, 0.0, 0.0);
            glRotate(30.0, 0.0, 1.0, 0.0);

            glTranslate(0, -1, 0)
            glRotate(250, 1, 0, 0)
            glutSolidCone(0.5, 1, 30, 5)
            glPopMatrix()
            glRotatef((self.lasty - self.y)/100., 0.0, 0.0, 1.0);
            glRotatef(0.0, (self.lastx - self.x)/100., 1.0, 0.0);
            # push into visible buffer
            self.SwapBuffers()




#----------------------------------------------------------------------








overview = """\
"""





#----------------------------------------------------------------------

def _test():
    class MyApp(wxApp):
        def OnInit(self):
            frame = wxFrame(None, -1, "GL Demos", wxDefaultPosition, wxSize(600,300))
            #win = ConeCanvas(frame)
            MySplitter(frame)
            frame.Show(True)
            self.SetTopWindow(frame)
            return True

    app = MyApp(0)
    app.MainLoop()

if __name__ == '__main__':
    _test()

