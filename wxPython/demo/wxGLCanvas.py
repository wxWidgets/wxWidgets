# 11/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Note: unable to install PyOpenGL on my system as I am running Python 2.3
#   and PyOpenGL does not support anything above Python 2.2. Did what I could,
#   but odds are good there are problems.
#

import  wx

try:
    import  wx.glcanvas as  glcanvas
    haveGLCanvas = True
except ImportError:
    haveGLCanvas = False

try:
    # The Python OpenGL package can be found at
    # http://PyOpenGL.sourceforge.net/

    import  OpenGL.GL   as  gl
    import  OpenGL.GLUT as  glut
    haveOpenGL = True
except ImportError:
    haveOpenGL = False

#----------------------------------------------------------------------

if not haveGLCanvas:
    def runTest(frame, nb, log):
        dlg = wx.MessageDialog(
            frame, 'The wxGLCanvas has not been included with this build of wxPython!',
            'Sorry', wx.OK | wx.ICON_INFORMATION
            )

        dlg.ShowModal()
        dlg.Destroy()

elif not haveOpenGL:
    def runTest(frame, nb, log):
        dlg = wxMessageDialog(
            frame, 'The OpenGL package was not found.  You can get it at\n'
            'http://PyOpenGL.sourceforge.net/', 'Sorry', wx.OK | wx.ICON_INFORMATION
            )

        dlg.ShowModal()
        dlg.Destroy()


else:
    buttonDefs = {
        wx.NewId() : ('CubeCanvas',      'Cube'),
        wx.NewId() : ('ConeCanvas',      'Cone'),
        }

    class ButtonPanel(wx.Panel):
        def __init__(self, parent, log):
            wx.Panel.__init__(self, parent, -1)
            self.log = log

            box = wx.BoxSizer(wx.VERTICAL)
            box.Add((20, 30))
            keys = buttonDefs.keys()
            keys.sort()

            for k in keys:
                text = buttonDefs[k][1]
                btn = wx.Button(self, k, text)
                box.Add(btn, 0, wx.ALIGN_CENTER|wx.ALL, 15)
                self.Bind(wx.EVT_BUTTON, self.OnButton, id=k)

            #** Enable this to show putting a wxGLCanvas on the wxPanel
            if 0:
                c = CubeCanvas(self)
                c.SetSize((200, 200))
                box.Add(c, 0, wx.ALIGN_CENTER|wx.ALL, 15)

            self.SetAutoLayout(True)
            self.SetSizer(box)


        def OnButton(self, evt):
            canvasClassName = buttonDefs[evt.GetId()][0]
            canvasClass = eval(canvasClassName)
            frame = wx.Frame(None, -1, canvasClassName, size=(400,400))
            canvas = canvasClass(frame)
            frame.Show(True)



    def runTest(frame, nb, log):
        win = ButtonPanel(nb, log)
        return win




    class MyCanvasBase(glcanvas.GLCanvas):
        def __init__(self, parent):
            glcanvas.GLCanvas.__init__(self, parent, -1)
            self.init = False
            # initial mouse position
            self.lastx = self.x = 30
            self.lasty = self.y = 30
            self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
            self.Bind(wx.EVT_SIZE, self.OnSize)
            self.Bind(wx.EVT_PAINT, self.OnPaint)
            self.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)  # needs fixing...
            self.Bind(wx.EVT_LEFT_UP, self.OnMouseUp)
            self.Bind(wx.EVT_MOTION, self.OnMouseMotion)

        def OnEraseBackground(self, event):
            pass # Do nothing, to avoid flashing on MSW.

        def OnSize(self, event):
            size = self.GetClientSize()

            if self.GetContext():
                self.SetCurrent()
                glcanvas.glViewport(0, 0, size.width, size.height)

        def OnPaint(self, event):
            dc = wx.PaintDC(self)
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
            glcanvas.glMatrixMode(glcanvas.GL_PROJECTION);
            glcanvas.glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 3.0);

            # position viewer
            glcanvas.glMatrixMode(glcanvas.GL_MODELVIEW);
            glcanvas.glTranslatef(0.0, 0.0, -2.0);

            # position object
            glcanvas.glRotatef(self.y, 1.0, 0.0, 0.0);
            glcanvas.glRotatef(self.x, 0.0, 1.0, 0.0);

            glcanvas.glEnable(glcanvas.GL_DEPTH_TEST);
            glcanvas.glEnable(glcanvas.GL_LIGHTING);
            glcanvas.glEnable(glcanvas.GL_LIGHT0);


        def OnDraw(self):
            # clear color and depth buffers
            glcanvas.glClear(glcanvas.GL_COLOR_BUFFER_BIT | glcanvas.GL_DEPTH_BUFFER_BIT)

            # draw six faces of a cube
            glcanvas.glBegin(glcanvas.GL_QUADS)
            glcanvas.glNormal3f( 0.0, 0.0, 1.0)
            glcanvas.glVertex3f( 0.5, 0.5, 0.5)
            glcanvas.glVertex3f(-0.5, 0.5, 0.5)
            glcanvas.glVertex3f(-0.5,-0.5, 0.5)
            glcanvas.glVertex3f( 0.5,-0.5, 0.5)

            glcanvas.glNormal3f( 0.0, 0.0,-1.0)
            glcanvas.glVertex3f(-0.5,-0.5,-0.5)
            glcanvas.glVertex3f(-0.5, 0.5,-0.5)
            glcanvas.glVertex3f( 0.5, 0.5,-0.5)
            glcanvas.glVertex3f( 0.5,-0.5,-0.5)

            glcanvas.glNormal3f( 0.0, 1.0, 0.0)
            glcanvas.glVertex3f( 0.5, 0.5, 0.5)
            glcanvas.glVertex3f( 0.5, 0.5,-0.5)
            glcanvas.glVertex3f(-0.5, 0.5,-0.5)
            glcanvas.glVertex3f(-0.5, 0.5, 0.5)

            glcanvas.glNormal3f( 0.0,-1.0, 0.0)
            glcanvas.glVertex3f(-0.5,-0.5,-0.5)
            glcanvas.glVertex3f( 0.5,-0.5,-0.5)
            glcanvas.glVertex3f( 0.5,-0.5, 0.5)
            glcanvas.glVertex3f(-0.5,-0.5, 0.5)

            glcanvas.glNormal3f( 1.0, 0.0, 0.0)
            glcanvas.glVertex3f( 0.5, 0.5, 0.5)
            glcanvas.glVertex3f( 0.5,-0.5, 0.5)
            glcanvas.glVertex3f( 0.5,-0.5,-0.5)
            glcanvas.glVertex3f( 0.5, 0.5,-0.5)

            glcanvas.glNormal3f(-1.0, 0.0, 0.0)
            glcanvas.glVertex3f(-0.5,-0.5,-0.5)
            glcanvas.glVertex3f(-0.5,-0.5, 0.5)
            glcanvas.glVertex3f(-0.5, 0.5, 0.5)
            glcanvas.glVertex3f(-0.5, 0.5,-0.5)
            glcanvas.glEnd()

            glcanvas.glRotatef((self.lasty - self.y)/100., 1.0, 0.0, 0.0);
            glcanvas.glRotatef((self.lastx - self.x)/100., 0.0, 1.0, 0.0);

            self.SwapBuffers()


    class ConeCanvas(MyCanvasBase):
        def InitGL( self ):
            glcanvas.glMatrixMode(glcanvas.GL_PROJECTION);
            # camera frustrum setup
            glcanvas.glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 3.0);
            glcanvas.glMaterial(glcanvas.GL_FRONT, glcanvas.GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
            glcanvas.glMaterial(glcanvas.GL_FRONT, glcanvas.GL_DIFFUSE, [0.8, 0.8, 0.8, 1.0])
            glcanvas.glMaterial(glcanvas.GL_FRONT, glcanvas.GL_SPECULAR, [1.0, 0.0, 1.0, 1.0])
            glcanvas.glMaterial(glcanvas.GL_FRONT, glcanvas.GL_SHININESS, 50.0)
            glcanvas.glLight(glcanvas.GL_LIGHT0, glcanvas.GL_AMBIENT, [0.0, 1.0, 0.0, 1.0])
            glcanvas.glLight(glcanvas.GL_LIGHT0, glcanvas.GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
            glcanvas.glLight(glcanvas.GL_LIGHT0, glcanvas.GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
            glcanvas.glLight(glcanvas.GL_LIGHT0, glcanvas.GL_POSITION, [1.0, 1.0, 1.0, 0.0]);
            glcanvas.glLightModel(glcanvas.GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
            glcanvas.glEnable(glcanvas.GL_LIGHTING)
            glcanvas.glEnable(glcanvas.GL_LIGHT0)
            glcanvas.glDepthFunc(glcanvas.GL_LESS)
            glcanvas.glEnable(glcanvas.GL_DEPTH_TEST)
            glcanvas.glClear(glcanvas.GL_COLOR_BUFFER_BIT | glcanvas.GL_DEPTH_BUFFER_BIT)
            # position viewer
            glcanvas.glMatrixMode(glcanvas.GL_MODELVIEW);


        def OnDraw(self):
            # clear color and depth buffers
            glcanvas.glClear(glcanvas.GL_COLOR_BUFFER_BIT | glcanvas.GL_DEPTH_BUFFER_BIT);
            # use a fresh transformation matrix
            glcanvas.glPushMatrix()
            # position object
            glcanvas.glTranslate(0.0, 0.0, -2.0);
            glcanvas.glRotate(30.0, 1.0, 0.0, 0.0);
            glcanvas.glRotate(30.0, 0.0, 1.0, 0.0);

            glcanvas.glTranslate(0, -1, 0)
            glcanvas.glRotate(250, 1, 0, 0)
            glcanvas.glutSolidCone(0.5, 1, 30, 5)
            glcanvas.glPopMatrix()
            glcanvas.glRotatef((self.lasty - self.y)/100., 0.0, 0.0, 1.0);
            glcanvas.glRotatef(0.0, (self.lastx - self.x)/100., 1.0, 0.0);
            # push into visible buffer
            self.SwapBuffers()




#----------------------------------------------------------------------



overview = """\
"""


#----------------------------------------------------------------------

def _test():
    class MyApp(wxApp):
        def OnInit(self):
            frame = wx.Frame(None, -1, "GL Demos", wx.DefaultPosition, (600,300))
            #win = ConeCanvas(frame)
            MySplitter(frame)
            frame.Show(True)
            self.SetTopWindow(frame)
            return True

    app = MyApp(0)
    app.MainLoop()

if __name__ == '__main__':
    _test()

