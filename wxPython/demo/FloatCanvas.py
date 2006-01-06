

#print "running:", wx.__version__
##First, make sure Numeric or numarray can be imported.
try:
    import Numeric
    import RandomArray
    haveNumeric = True
except ImportError:
    # Numeric isn't there, let's try numarray
    try:
        import numarray as Numeric
        import numarray.random_array as RandomArray
        haveNumeric = True
    except ImportError:
        # numarray isn't there either
        haveNumeric = False
        errorText = (
        "The FloatCanvas requires either the Numeric or numarray module\n\n"
        "You can get them at:\n"
        "http://sourceforge.net/projects/numpy\n\n"
        "NOTE: The Numeric module is substantially faster than numarray for this\n"
        "purpose, if you have lots of objects\n"
        )
      
#---------------------------------------------------------------------------


def BuildDrawFrame(): # this gets called when needed, rather than on import
    try:
        from floatcanvas import NavCanvas, FloatCanvas, Resources
    except ImportError: # if it's not there locally, try the wxPython lib.
        from wx.lib.floatcanvas import NavCanvas, FloatCanvas, Resources
    import wx.lib.colourdb
    import time, random

    class DrawFrame(wx.Frame):

        """
        A frame used for the FloatCanvas Demo

        """


        def __init__(self,parent, id,title,position,size):
            wx.Frame.__init__(self,parent, id,title,position, size)

            ## Set up the MenuBar
            MenuBar = wx.MenuBar()

            file_menu = wx.Menu()
            item = file_menu.Append(-1, "&Close","Close this frame")
            self.Bind(wx.EVT_MENU, self.OnQuit, item)

            item = file_menu.Append(-1, "&SavePNG","Save the current image as a PNG")
            self.Bind(wx.EVT_MENU, self.OnSavePNG, item)
            MenuBar.Append(file_menu, "&File")

            draw_menu = wx.Menu()

            item = draw_menu.Append(-1, "&Clear","Clear the Canvas")
            self.Bind(wx.EVT_MENU, self.Clear, item)

            item = draw_menu.Append(-1, "&Draw Test","Run a test of drawing random components")
            self.Bind(wx.EVT_MENU, self.DrawTest, item)

            item = draw_menu.Append(-1, "&Line Test","Run a test of drawing random lines")
            self.Bind(wx.EVT_MENU, self.LineTest, item)

            item = draw_menu.Append(-1, "Draw &Map","Run a test of drawing a map")
            self.Bind(wx.EVT_MENU, self.DrawMap, item)

            item = draw_menu.Append(-1, "&Text Test","Run a test of text drawing")
            self.Bind(wx.EVT_MENU, self.TestText, item)

            item = draw_menu.Append(-1, "&ScaledText Test","Run a test of text drawing")
            self.Bind(wx.EVT_MENU, self.TestScaledText, item)

            item = draw_menu.Append(-1, "&ScaledTextBox Test","Run a test of the Scaled Text Box")
            self.Bind(wx.EVT_MENU, self.TestScaledTextBox, item)

            item = draw_menu.Append(-1, "&Bitmap Test","Run a test of the Bitmap Object")
            self.Bind(wx.EVT_MENU, self.TestBitmap, item)

            item = draw_menu.Append(-1, "&Hit Test","Run a test of the hit test code")
            self.Bind(wx.EVT_MENU, self.TestHitTest, item)

            item = draw_menu.Append(-1, "Hit Test &Foreground","Run a test of the hit test code with a foreground Object")
            self.Bind(wx.EVT_MENU, self.TestHitTestForeground, item)

            item = draw_menu.Append(-1, "&Animation","Run a test of Animation")
            self.Bind(wx.EVT_MENU, self.TestAnimation, item)

            #item = draw_menu.Append(-1, "&Speed","Run a test of Drawing Speed")
            #self.Bind(wx.EVT_MENU, self.SpeedTest, item)

            item = draw_menu.Append(-1, "Change &Properties","Run a test of Changing Object Properties")
            self.Bind(wx.EVT_MENU, self.PropertiesChangeTest, item)

            item = draw_menu.Append(-1, "&Arrows","Run a test of Arrows")
            self.Bind(wx.EVT_MENU, self.ArrowTest, item)

            item = draw_menu.Append(-1, "&Hide","Run a test of the Show() Hide() Show() and methods")
            self.Bind(wx.EVT_MENU, self.HideTest, item)

            MenuBar.Append(draw_menu, "&Tests")

            view_menu = wx.Menu()
            item = view_menu.Append(-1, "Zoom to &Fit","Zoom to fit the window")
            self.Bind(wx.EVT_MENU, self.ZoomToFit, item)
            MenuBar.Append(view_menu, "&View")

            help_menu = wx.Menu()
            item = help_menu.Append(-1, "&About",
                                    "More information About this program")
            self.Bind(wx.EVT_MENU, self.OnAbout, item)
            MenuBar.Append(help_menu, "&Help")

            self.SetMenuBar(MenuBar)

            self.CreateStatusBar()

            
            # Add the Canvas
            self.Canvas = NavCanvas.NavCanvas(self,
                                              Debug = 0,
                                              BackgroundColor = "DARK SLATE BLUE")

            self.MsgWindow = wx.TextCtrl(self, wx.ID_ANY,
                                         "Look Here for output from events\n",
                                         style = (wx.TE_MULTILINE |
                                                  wx.TE_READONLY |
                                                  wx.SUNKEN_BORDER)
                                         )
            
            ##Create a sizer to manage the Canvas and message window
            MainSizer = wx.BoxSizer(wx.VERTICAL)
            MainSizer.Add(self.Canvas, 4, wx.EXPAND)
            MainSizer.Add(self.MsgWindow, 1, wx.EXPAND | wx.ALL, 5)

            self.SetSizer(MainSizer)
            wx.EVT_CLOSE(self, self.OnCloseWindow)

            FloatCanvas.EVT_MOTION(self.Canvas, self.OnMove ) 

            self.EventsAreBound = False

            ## getting all the colors for random objects
            wx.lib.colourdb.updateColourDB()
            self.colors = wx.lib.colourdb.getColourList()


            return None

        def Log(self, text):
            self.MsgWindow.AppendText(text)
            if not text[-1] == "\n":
                self.MsgWindow.AppendText("\n")
            

        def BindAllMouseEvents(self):
            if not self.EventsAreBound:
                ## Here is how you catch FloatCanvas mouse events
                FloatCanvas.EVT_LEFT_DOWN(self.Canvas, self.OnLeftDown ) 
                FloatCanvas.EVT_LEFT_UP(self.Canvas, self.OnLeftUp )
                FloatCanvas.EVT_LEFT_DCLICK(self.Canvas, self.OnLeftDouble ) 

                FloatCanvas.EVT_MIDDLE_DOWN(self.Canvas, self.OnMiddleDown ) 
                FloatCanvas.EVT_MIDDLE_UP(self.Canvas, self.OnMiddleUp ) 
                FloatCanvas.EVT_MIDDLE_DCLICK(self.Canvas, self.OnMiddleDouble ) 

                FloatCanvas.EVT_RIGHT_DOWN(self.Canvas, self.OnRightDown ) 
                FloatCanvas.EVT_RIGHT_UP(self.Canvas, self.OnRightUp ) 
                FloatCanvas.EVT_RIGHT_DCLICK(self.Canvas, self.OnRightDouble ) 

                FloatCanvas.EVT_MOUSEWHEEL(self.Canvas, self.OnWheel ) 
            self.EventsAreBound = True

        def UnBindAllMouseEvents(self):
            ## Here is how you unbind FloatCanvas mouse events
            FloatCanvas.EVT_LEFT_DOWN(self.Canvas, None ) 
            FloatCanvas.EVT_LEFT_UP(self.Canvas, None )
            FloatCanvas.EVT_LEFT_DCLICK(self.Canvas, None) 

            FloatCanvas.EVT_MIDDLE_DOWN(self.Canvas, None )
            FloatCanvas.EVT_MIDDLE_UP(self.Canvas, None )
            FloatCanvas.EVT_MIDDLE_DCLICK(self.Canvas, None )

            FloatCanvas.EVT_RIGHT_DOWN(self.Canvas, None )
            FloatCanvas.EVT_RIGHT_UP(self.Canvas, None )
            FloatCanvas.EVT_RIGHT_DCLICK(self.Canvas, None )

            FloatCanvas.EVT_MOUSEWHEEL(self.Canvas, None )
            FloatCanvas.EVT_LEFT_DOWN(self.Canvas, None ) 
            FloatCanvas.EVT_LEFT_UP(self.Canvas, None )
            FloatCanvas.EVT_LEFT_DCLICK(self.Canvas, None) 

            FloatCanvas.EVT_MIDDLE_DOWN(self.Canvas, None )
            FloatCanvas.EVT_MIDDLE_UP(self.Canvas, None )
            FloatCanvas.EVT_MIDDLE_DCLICK(self.Canvas, None )

            FloatCanvas.EVT_RIGHT_DOWN(self.Canvas, None )
            FloatCanvas.EVT_RIGHT_UP(self.Canvas, None )
            FloatCanvas.EVT_RIGHT_DCLICK(self.Canvas, None )

            FloatCanvas.EVT_MOUSEWHEEL(self.Canvas, None )

            self.EventsAreBound = False


        def PrintCoords(self,event):
            #print "coords are: %s"%(event.Coords,)
            #print "pixel coords are: %s\n"%(event.GetPosition(),)
            self.Log("coords are: %s"%(event.Coords,))
            self.Log("pixel coords are: %s\n"%(event.GetPosition(),))

        def OnSavePNG(self, event=None):
            import os
            dlg = wx.FileDialog(
                self, message="Save file as ...", defaultDir=os.getcwd(), 
                defaultFile="", wildcard="*.png", style=wx.SAVE
                )
            if dlg.ShowModal() == wx.ID_OK:
                path = dlg.GetPath()
                if not(path[-4:].lower() == ".png"):
                    path = path+".png"
                self.Canvas.SaveAsImage(path)

                    
        def OnLeftDown(self, event):
            self.Log("LeftDown")
            self.PrintCoords(event)

        def OnLeftUp(self, event):
            self.Log("LeftUp")
            self.PrintCoords(event)

        def OnLeftDouble(self, event):
            self.Log("LeftDouble")
            self.PrintCoords(event)

        def OnMiddleDown(self, event):
            self.Log("MiddleDown")
            self.PrintCoords(event)

        def OnMiddleUp(self, event):
            self.Log("MiddleUp")
            self.PrintCoords(event)

        def OnMiddleDouble(self, event):
            self.Log("MiddleDouble")
            self.PrintCoords(event)

        def OnRightDown(self, event):
            self.Log("RightDown")
            self.PrintCoords(event)

        def OnRightUp(self, event):
            self.Log("RightDown")
            self.PrintCoords(event)

        def OnRightDouble(self, event):
            self.Log("RightDouble")
            self.PrintCoords(event)

        def OnWheel(self, event):
            self.Log("Mouse Wheel")
            self.PrintCoords(event)
            Rot = event.GetWheelRotation()
            Rot = Rot / abs(Rot) * 0.1
            if event.ControlDown(): # move left-right
                self.Canvas.MoveImage( (Rot, 0), "Panel" )
            else: # move up-down
                self.Canvas.MoveImage( (0, Rot), "Panel" )
                
        def OnMove(self, event):
            """
            Updates the status bar with the world coordinates
            """
            self.SetStatusText("%.2f, %.2f"%tuple(event.Coords))

        def OnAbout(self, event):
            dlg = wx.MessageDialog(self,
                                   "This is a small program to demonstrate\n"
                                   "the use of the FloatCanvas\n",
                                   "About Me",
                                   wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()

        def ZoomToFit(self,event):
            self.Canvas.ZoomToBB()

        def Clear(self,event = None):
            self.UnBindAllMouseEvents()
            self.Canvas.ClearAll()
            self.Canvas.SetProjectionFun(None)
            self.Canvas.Draw()

        def OnQuit(self,event):
            self.Close(True)

        def OnCloseWindow(self, event):
            self.Destroy()

        def DrawTest(self,event=None):
            wx.GetApp().Yield()

            Range = (-10,10)
            colors = self.colors

            self.BindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            ############# Random tests of everything ##############

            # Rectangles
            for i in range(3):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                wh = (random.randint(1,5), random.randint(1,5))
                Canvas.AddRectangle(xy, wh, LineWidth = lw, FillColor = colors[cf])

            # Ellipses
            for i in range(3):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                h = random.randint(1,5)
                w = random.randint(1,5)
                Canvas.AddEllipse(xy, (h,w), LineWidth = lw,FillColor = colors[cf])

            # Points
            for i in range(5):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                D = random.randint(1,50)
                cf = random.randint(0,len(colors)-1)
                Canvas.AddPoint(xy, Color = colors[cf], Diameter = D)

            # SquarePoints
            for i in range(500):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                S = random.randint(1, 50)
                cf = random.randint(0,len(colors)-1)
                Canvas.AddSquarePoint(xy, Color = colors[cf], Size = S)

            # Circles
            for i in range(5):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                D = random.randint(1,5)
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddCircle(xy, D, LineWidth = lw, LineColor = colors[cl], FillColor = colors[cf])
                Canvas.AddText("Circle # %i"%(i), xy, Size = 12, BackgroundColor = None, Position = "cc")
            # Lines
            for i in range(5):
                points = []
                for j in range(random.randint(2,10)):
                    point = (random.randint(Range[0],Range[1]),random.randint(Range[0],Range[1]))
                    points.append(point)
                lw = random.randint(1,10)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddLine(points, LineWidth = lw, LineColor = colors[cl])
            # Polygons
            for i in range(3):
                points = []
                for j in range(random.randint(2,6)):
                    point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                    points.append(point)
                lw = random.randint(1,6)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddPolygon(points,
                                       LineWidth = lw,
                                       LineColor = colors[cl],
                                       FillColor = colors[cf],
                                       FillStyle = 'Solid')

            ## Pointset
            for i in range(4):
                points = []
                points = RandomArray.uniform(Range[0],Range[1],(100,2))
                cf = random.randint(0,len(colors)-1)
                D = random.randint(1,4)
                Canvas.AddPointSet(points, Color = colors[cf], Diameter = D)

            # Text
            String = "Unscaled text"
            for i in range(3):
                ts = random.randint(10,40)
                cf = random.randint(0,len(colors)-1)
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                Canvas.AddText(String, xy, Size = ts, Color = colors[cf], Position = "cc")

            # Scaled Text
            String = "Scaled text"
            for i in range(3):
                ts = random.random()*3 + 0.2
                cf = random.randint(0,len(colors)-1)
                Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                Canvas.AddScaledText(String, Point, Size = ts, Color = colors[cf], Position = "cc")

            # Arrows
            N = 5
            Points = RandomArray.uniform(Range[0], Range[1], (N,2) )
            for i in range(N):
                Canvas.AddArrow(Points[i],
                                random.uniform(20,100),
                                Direction = random.uniform(0,360),
                                LineWidth = random.uniform(1,5),
                                LineColor = colors[random.randint(0,len(colors)-1)],
                                ArrowHeadAngle = random.uniform(20,90))

            Canvas.ZoomToBB()

        def TestAnimation(self,event=None):
            """

            In this test, a relatively complex background is drawn, and
            a simple object placed in the foreground is moved over
            it. This demonstrates how to use the InForeground attribute
            to make an object in the foregorund draw fast, without
            having to re-draw the whole background.

            """
            wx.GetApp().Yield()
            Range = (-10,10)
            self.Range = Range

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            ##		Random tests of everything:
            colors = self.colors
            # Rectangles
            for i in range(3):
                xy = (random.uniform(Range[0],Range[1]), random.uniform(Range[0],Range[1]))
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                wh = (random.randint(1,5), random.randint(1,5) )
                Canvas.AddRectangle(xy, wh, LineWidth = lw, FillColor = colors[cf])

            # Ellipses
            for i in range(3):
                xy = (random.uniform(Range[0],Range[1]), random.uniform(Range[0],Range[1]))
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                wh = (random.randint(1,5), random.randint(1,5) )
                Canvas.AddEllipse(xy, wh, LineWidth = lw, FillColor = colors[cf])

            # Circles
            for i in range(5):
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                D = random.randint(1,5)
                lw = random.randint(1,5)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddCircle(xy, D, LineWidth = lw, LineColor = colors[cl], FillColor = colors[cf])
                Canvas.AddText("Circle # %i"%(i), xy, Size = 12, BackgroundColor = None, Position = "cc")

            # Lines
            for i in range(5):
                points = []
                for j in range(random.randint(2,10)):
                    point = (random.randint(Range[0],Range[1]),random.randint(Range[0],Range[1]))
                    points.append(point)
                lw = random.randint(1,10)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddLine(points, LineWidth = lw, LineColor = colors[cl])

            # Polygons
            for i in range(3):
                points = []
                for j in range(random.randint(2,6)):
                    point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                    points.append(point)
                lw = random.randint(1,6)
                cf = random.randint(0,len(colors)-1)
                cl = random.randint(0,len(colors)-1)
                Canvas.AddPolygon(points,
                                       LineWidth = lw,
                                       LineColor = colors[cl],
                                       FillColor = colors[cf],
                                       FillStyle = 'Solid')

            # Scaled Text
            String = "Scaled text"
            for i in range(3):
                ts = random.random()*3 + 0.2
                cf = random.randint(0,len(colors)-1)
                xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                Canvas.AddScaledText(String, xy, Size = ts, Color = colors[cf], Position = "cc")


            # Now the Foreground Object:
            C = Canvas.AddCircle((0,0), 7, LineWidth = 2,LineColor = "Black",FillColor = "Red", InForeground = True)
            T = Canvas.AddScaledText("Click to Move", (0,0), Size = 0.6, Position = 'cc', InForeground = True)
            C.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.MoveMe)
            C.Text = T

            self.Timer = wx.PyTimer(self.ShowFrame)
            self.FrameDelay = 50 # milliseconds

            Canvas.ZoomToBB()

        def ShowFrame(self):
            Object = self.MovingObject
            Range = self.Range
            if  self.TimeStep < self.NumTimeSteps:
                x,y = Object.XY
                if x > Range[1] or x < Range[0]:
                    self.dx = -self.dx
                if y > Range[1] or y < Range[0]:
                    self.dy = -self.dy
                Object.Move( (self.dx,self.dy) )
                Object.Text.Move( (self.dx,self.dy))
                self.Canvas.Draw()
                self.TimeStep += 1
                wx.GetApp().Yield(True)
            else:
                self.Timer.Stop()


        def MoveMe(self, Object):
            self.MovingObject = Object
            Range = self.Range
            self.dx = random.uniform(Range[0]/4,Range[1]/4)
            self.dy = random.uniform(Range[0]/4,Range[1]/4)
            #import time
            #start = time.time()
            self.NumTimeSteps = 200
            self.TimeStep = 1
            self.Timer.Start(self.FrameDelay)
            #print "Did %i frames in %f seconds"%(N, (time.time() - start) )

        def TestHitTest(self,event=None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            #Add a Hit-able rectangle
            w, h = 60, 20

            dx = 80
            dy = 40
            x, y = 20, 20
            FontSize = 8

            #Add one that is not HitAble
            Canvas.AddRectangle((x,y), (w, h), LineWidth = 2)
            Canvas.AddText("Not Hit-able", (x,y), Size = FontSize, Position = "bl")


            x += dx
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2)
            R.Name = "Line Rectangle"
            R.HitFill = False
            R.HitLineWidth = 5 # Makes it a little easier to hit
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHit)
            Canvas.AddText("Left Click Line", (x,y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "Red"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + "Rectangle"
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHit)
            Canvas.AddText("Left Click Fill", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            color = "LightBlue"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_RIGHT_DOWN, self.RectGotHit)
            Canvas.AddText("Right Click Fill", (x, y), Size = FontSize,  Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "Grey"
            R = Canvas.AddEllipse((x, y), (w, h),LineWidth = 2,FillColor = color)
            R.Name = color +" Ellipse"
            R.Bind(FloatCanvas.EVT_FC_RIGHT_DOWN, self.RectGotHit)
            Canvas.AddText("Right Click Fill", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "Brown"
            R = Canvas.AddCircle((x+dx/2, y+dy/2), dx/4, LineWidth = 2, FillColor = color)
            R.Name = color + " Circle"
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DCLICK, self.RectGotHit)
            Canvas.AddText("Left D-Click Fill", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            color = "Pink"
            R = Canvas.AddCircle((x+dx/2, y+dy/2), dx/4, LineWidth = 2,FillColor = color)
            R.Name = color +  " Circle"
            R.Bind(FloatCanvas.EVT_FC_LEFT_UP, self.RectGotHit)
            Canvas.AddText("Left Up Fill", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "White"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_MIDDLE_DOWN, self.RectGotHit)
            Canvas.AddText("Middle Down", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "AQUAMARINE"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_MIDDLE_UP, self.RectGotHit)
            Canvas.AddText("Middle Up", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            color = "CORAL"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_MIDDLE_DCLICK, self.RectGotHit)
            Canvas.AddText("Middle DoubleClick", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "CYAN"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_RIGHT_UP, self.RectGotHit)
            Canvas.AddText("Right Up", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "LIME GREEN"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_RIGHT_DCLICK, self.RectGotHit)
            Canvas.AddText("Right Double Click", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            color = "MEDIUM GOLDENROD"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color
            R.Bind(FloatCanvas.EVT_FC_RIGHT_DOWN, self.RectGotHitRight)
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHitLeft)
            Canvas.AddText("L and R Click", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "SALMON"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color + " Rectangle"
            R.Bind(FloatCanvas.EVT_FC_ENTER_OBJECT, self.RectMouseOver)
            Canvas.AddText("Mouse Enter", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "MEDIUM VIOLET RED"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color
            R.Bind(FloatCanvas.EVT_FC_LEAVE_OBJECT, self.RectMouseLeave)
            Canvas.AddText("Mouse Leave", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            color = "SKY BLUE"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color)
            R.Name = color
            R.Bind(FloatCanvas.EVT_FC_ENTER_OBJECT, self.RectMouseOver)
            R.Bind(FloatCanvas.EVT_FC_LEAVE_OBJECT, self.RectMouseLeave)
            Canvas.AddText("Enter and Leave", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "WHEAT"
            R = Canvas.AddRectangle((x, y), (w+12, h), LineColor = None, FillColor = color)
            R.Name = color
            R.Bind(FloatCanvas.EVT_FC_ENTER_OBJECT, self.RectMouseOver)
            R.Bind(FloatCanvas.EVT_FC_LEAVE_OBJECT, self.RectMouseLeave)
            Canvas.AddText("Mouse Enter&Leave", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "KHAKI"
            R = Canvas.AddRectangle((x-12, y), (w+12, h), LineColor = None, FillColor = color)
            R.Name = color
            R.Bind(FloatCanvas.EVT_FC_ENTER_OBJECT, self.RectMouseOver)
            R.Bind(FloatCanvas.EVT_FC_LEAVE_OBJECT, self.RectMouseLeave)
            Canvas.AddText("Mouse ENter&Leave", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            L = Canvas.AddLine(( (x, y), (x+10, y+10), (x+w, y+h) ), LineWidth = 2, LineColor = "Red")
            L.Name = "A Line"
            L.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHitLeft)
            Canvas.AddText("Left Down", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(L.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "SEA GREEN"
            Points = Numeric.array(( (x, y), (x, y+2.*h/3), (x+w, y+h), (x+w, y+h/2.), (x + 2.*w/3, y+h/2.), (x + 2.*w/3,y) ), Numeric.Float)
            R = Canvas.AddPolygon(Points,  LineWidth = 2, FillColor = color)
            R.Name = color + " Polygon"
            R.Bind(FloatCanvas.EVT_FC_RIGHT_DOWN, self.RectGotHitRight)
            Canvas.AddText("RIGHT_DOWN", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x += dx
            color = "Red"
            Points = Numeric.array(( (x, y), (x, y+2.*h/3), (x+w, y+h), (x+w, y+h/2.), (x + 2.*w/3, y+h/2.), (x + 2.*w/3,y) ), Numeric.Float)
            R = Canvas.AddPointSet(Points,  Diameter = 4, Color = color)
            R.Name = "PointSet"
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.PointSetGotHit)
            Canvas.AddText("LEFT_DOWN", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Size = FontSize, Position = "tl")

            x = 20
            y += dy
            T = Canvas.AddText("Hit-able Text", (x, y), Size = 15, Color = "Red", Position = 'tl')
            T.Name = "Hit-able Text"
            T.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHitLeft)
            Canvas.AddText("Left Down", (x, y), Size = FontSize, Position = "bl")

            x += dx
            T = Canvas.AddScaledText("Scaled Text", (x, y), Size = 1./2*h, Color = "Pink", Position = 'bl')
            Canvas.AddPointSet( (x, y), Diameter = 3)
            T.Name = "Scaled Text"
            T.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHitLeft)
            Canvas.AddText("Left Down", (x, y), Size = FontSize, Position = "tl")

            x += dx
            color = "Cyan"
            Point = (x + w/2, y)
            #Points = Numeric.array(( (x, y), (x, y+2.*h/3), (x+w, y+h), (x+w, y+h/2.), (x + 2.*w/3, y+h/2.), (x + 2.*w/3,y) ), Numeric.Float)
            R = Canvas.AddSquarePoint(Point,  Size = 8, Color = color)
            R.Name = "SquarePoint"
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHit)
            Canvas.AddText("LEFT_DOWN", (x, y), Size = FontSize, Position = "bl")
            Canvas.AddText(R.Name, (x, y), Size = FontSize, Position = "tl")


            self.Canvas.ZoomToBB()

        def TestHitTestForeground(self,event=None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            #Add a Hitable rectangle
            w, h = 60, 20

            dx = 80
            dy = 40
            x,y = 20, 20

            color = "Red"
            R = Canvas.AddRectangle((x, y), (w, h), LineWidth = 2, FillColor = color, InForeground = False)
            R.Name = color + "Rectangle"
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectGotHit)
            Canvas.AddText("Left Click Fill", (x, y), Position = "bl")
            Canvas.AddText(R.Name, (x, y+h), Position = "tl")

            ## A set of Rectangles that move together

            ## NOTE: In a real app, it might be better to create a new
            ## custom FloatCanvas DrawObject

            self.MovingRects = []
            WH = (w/2, h/2)
            x += dx
            color = "LightBlue"
            R = Canvas.AddRectangle((x, y), WH, LineWidth = 2, FillColor = color, InForeground = True)
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectMoveLeft)
            L = Canvas.AddText("Left", (x + w/4, y + h/4), Position = "cc", InForeground = True)
            self.MovingRects.extend( (R,L) )

            x += w/2
            R = Canvas.AddRectangle((x, y), WH, LineWidth = 2, FillColor = color, InForeground = True)
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectMoveRight)
            L = Canvas.AddText("Right", (x + w/4, y + h/4), Position = "cc", InForeground = True)
            self.MovingRects.extend( (R,L) )

            x -= w/2
            y += h/2
            R = Canvas.AddRectangle((x, y), WH, LineWidth = 2, FillColor = color, InForeground = True)
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectMoveUp)
            L = Canvas.AddText("Up", (x + w/4, y + h/4), Position = "cc", InForeground = True)
            self.MovingRects.extend( (R,L) )


            x += w/2
            R = Canvas.AddRectangle((x, y), WH, LineWidth = 2, FillColor = color, InForeground = True)
            R.HitFill = True
            R.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RectMoveDown)
            L = Canvas.AddText("Down", (x + w/4, y + h/4), Position = "cc", InForeground = True)
            self.MovingRects.extend( (R,L) )

            self.Canvas.ZoomToBB()

        def RectMoveLeft(self,Object):
            self.MoveRects("left")

        def RectMoveRight(self,Object):
            self.MoveRects("right")

        def RectMoveUp(self,Object):
            self.MoveRects("up")

        def RectMoveDown(self,Object):
            self.MoveRects("down")

        def MoveRects(self, Dir):
            for Object in self.MovingRects:
                X,Y = Object.XY
                if Dir == "left": X -= 10
                elif Dir == "right": X += 10
                elif Dir == "up": Y += 10
                elif Dir == "down": Y -= 10
                Object.SetPoint((X,Y))
            self.Canvas.Draw()

        def PointSetGotHit(self, Object):
            self.Log(Object.Name + "Got Hit\n")

        def RectGotHit(self, Object):
            self.Log(Object.Name + "Got Hit\n")

        def RectGotHitRight(self, Object):
            self.Log(Object.Name + "Got Hit With Right\n")

        def RectGotHitLeft(self, Object):
            self.Log(Object.Name + "Got Hit with Left\n")

        def RectMouseOver(self, Object):
            self.Log("Mouse entered:" +  Object.Name)

        def RectMouseLeave(self, Object):
            self.Log("Mouse left " +  Object.Name)


        def TestText(self, event= None):
            wx.GetApp().Yield()

            self.BindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Point  = (3, 0)

            ## Add a non-visible rectangle, just to get a Bounding Box
            ## Text objects have a zero-size bounding box, because it changes with zoom
            Canvas.AddRectangle((-10,-10),
                                (20,20),
                                LineWidth = 1,
                                LineColor = None)

            # Text
            String = "Some text"
            self.Canvas.AddText("Top Left",Point,Size = 14,Color = "Yellow",BackgroundColor = "Blue", Position = "tl")
            self.Canvas.AddText("Bottom Left",Point,Size = 14,Color = "Cyan",BackgroundColor = "Black",Position = "bl")
            self.Canvas.AddText("Top Right",Point,Size = 14,Color = "Black",BackgroundColor = "Cyan",Position = "tr")
            self.Canvas.AddText("Bottom Right",Point,Size = 14,Color = "Blue",BackgroundColor = "Yellow",Position = "br")
            Canvas.AddPointSet((Point), Color = "White", Diameter = 2)

            Point  = (3, 2)

            Canvas.AddPointSet((Point), Color = "White", Diameter = 2)
            self.Canvas.AddText("Top Center",Point,Size = 14,Color = "Black",Position = "tc")
            self.Canvas.AddText("Bottom Center",Point,Size = 14,Color = "White",Position = "bc")

            Point  = (3, 4)

            Canvas.AddPointSet((Point), Color = "White", Diameter = 2)
            self.Canvas.AddText("Center Right",Point,Size = 14,Color = "Black",Position = "cr")
            self.Canvas.AddText("Center Left",Point,Size = 14,Color = "Black",Position = "cl")

            Point  = (3, -2)

            Canvas.AddPointSet((Point), Color = "White", Diameter = 2)
            self.Canvas.AddText("Center Center",
                                Point, Size = 14,
                                Color = "Black",
                                Position = "cc")

            self.Canvas.AddText("40 Pixels", (-10,8), Size = 40)
            self.Canvas.AddText("20 Pixels", (-10,5), Size = 20)
            self.Canvas.AddText("10 Pixels", (-10,3), Size = 10)

            self.Canvas.AddText("MODERN Font", (-10, 0), Family = wx.MODERN)
            self.Canvas.AddText("DECORATIVE Font", (-10, -1), Family = wx.DECORATIVE)
            self.Canvas.AddText("ROMAN Font", (-10, -2), Family = wx.ROMAN)
            self.Canvas.AddText("SCRIPT Font", (-10, -3), Family = wx.SCRIPT)
            self.Canvas.AddText("ROMAN BOLD Font", (-10, -4), Family = wx.ROMAN, Weight=wx.BOLD)
            self.Canvas.AddText("ROMAN ITALIC BOLD Font", (-10, -5), Family = wx.ROMAN, Weight=wx.BOLD, Style=wx.ITALIC)

            # NOTE: this font exists on my Linux box..who knows were else you'll find it!
            Font = wx.Font(20, wx.DEFAULT, wx.ITALIC, wx.NORMAL, False, "zapf chancery")
            self.Canvas.AddText("zapf chancery Font", (-10, -6), Font = Font)

            self.Canvas.ZoomToBB()

        def TestScaledText(self, event= None):
            wx.GetApp().Yield()

            self.BindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Point  = (0, 0)

            T = Canvas.AddScaledText("Top Left",
                                     Point,
                                     Size = 5,
                                     Color = "Yellow",
                                     BackgroundColor = "Blue",
                                     Position = "tl")
            T = Canvas.AddScaledText("Bottom Left",Point,Size = 5,Color = "Cyan",BackgroundColor = "Black",Position = "bl")
            T = Canvas.AddScaledText("Top Right",Point,Size = 5,Color = "Black",BackgroundColor = "Cyan",Position = "tr")
            T = Canvas.AddScaledText("Bottom Right",Point,Size = 5,Color = "Blue",BackgroundColor = "Yellow",Position = "br")
            Canvas.AddPointSet((Point), Color = "Red", Diameter = 4)


            Point  = (0, 20)

            Canvas.AddScaledText("Top Center",Point,Size = 7,Color = "Black",Position = "tc")
            Canvas.AddScaledText("Bottom Center",Point,Size = 7,Color = "White",Position = "bc")
            Canvas.AddPointSet((Point), Color = "White", Diameter = 4)

            Point  = (0, -20)

            Canvas.AddScaledText("Center Right",Point,Size = 9,Color = "Black",Position = "cr")
            Canvas.AddScaledText("Center Left",Point,Size = 9,Color = "Black",Position = "cl")
            Canvas.AddPointSet((Point), Color = "White", Diameter = 4)

            x = -200

            self.Canvas.AddScaledText("MODERN Font", (x, 0), Size = 7, Family = wx.MODERN, Color = (0,0,0))
            self.Canvas.AddScaledText("DECORATIVE Font", (x, -10), Size = 7, Family = wx.DECORATIVE, Color = (0,0,1))
            self.Canvas.AddScaledText("ROMAN Font", (x, -20), Size = 7, Family = wx.ROMAN)
            self.Canvas.AddScaledText("SCRIPT Font", (x, -30), Size = 7, Family = wx.SCRIPT)
            self.Canvas.AddScaledText("ROMAN BOLD Font", (x, -40), Size = 7, Family = wx.ROMAN, Weight=wx.BOLD)
            self.Canvas.AddScaledText("ROMAN ITALIC BOLD Font", (x, -50), Size = 7, Family = wx.ROMAN, Weight=wx.BOLD, Style=wx.ITALIC)
            Canvas.AddPointSet((x,0), Color = "White", Diameter = 4)


            # NOTE: this font exists on my Linux box..who knows were else you'll find it!
            Point = (-100, 50)
            Font = wx.Font(12, wx.DEFAULT, wx.ITALIC, wx.NORMAL, False, "zapf chancery")
            T = self.Canvas.AddScaledText("zapf chancery Font", Point, Size = 20, Font = Font, Position = 'bc')

            Point = (-50, -50)
            Font = wx.Font(12, wx.DEFAULT, wx.ITALIC, wx.NORMAL, False, "bookman")
            T = self.Canvas.AddScaledText("Bookman Font", Point, Size = 8, Font = Font)

            self.Canvas.ZoomToBB()

        def TestScaledTextBox(self, event= None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Point = (45,40)
            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          2,
                                          Color = "Black",
                                          BackgroundColor = None,
                                          LineColor = "Red",
                                          LineStyle = "Solid",
                                          LineWidth = 1,
                                          Width = None,
                                          PadSize = 5,
                                          Family = wx.ROMAN,
                                          Style = wx.NORMAL,
                                          Weight = wx.NORMAL,
                                          Underline = False,
                                          Position = 'br',
                                          Alignment = "left",
                                          InForeground = False)

            # All defaults
            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          2)

            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          2,
                                          BackgroundColor = "Yellow",
                                          LineColor = "Red",
                                          LineStyle = "Solid",
                                          PadSize = 5,
                                          Family = wx.TELETYPE,
                                          Position = 'bl')

            Box = Canvas.AddScaledTextBox("A String\nThis box is clickable",
                                          Point,
                                          2,
                                          BackgroundColor = "Yellow",
                                          LineColor = "Red",
                                          LineStyle = "Solid",
                                          PadSize = 5,
                                          Family = wx.TELETYPE,
                                          Position = 'tr')

            Box.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.binding2)

            Canvas.AddPoint(Point, Diameter = 4)

            Point = (45,15)
            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          2,
                                          Color = "Black",
                                          BackgroundColor = 'Red',
                                          LineColor = "Blue",
                                          LineStyle = "LongDash",
                                          LineWidth = 2,
                                          Width = None,
                                          PadSize = 5,
                                          Family = wx.TELETYPE,
                                          Style = wx.NORMAL,
                                          Weight = wx.NORMAL,
                                          Underline = False,
                                          Position = 'cr',
                                          Alignment = "left",
                                          InForeground = False)

            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          1.5,
                                          Color = "Black",
                                          BackgroundColor = 'Red',
                                          LineColor = "Blue",
                                          LineStyle = "LongDash",
                                          LineWidth = 2,
                                          Width = None,
                                          PadSize = 5,
                                          Family = wx.TELETYPE,
                                          Style = wx.NORMAL,
                                          Weight = wx.NORMAL,
                                          Underline = False,
                                          Position = 'cl',
                                          Alignment = "left",
                                          InForeground = False)

            Canvas.AddPoint(Point, Diameter = 4)

            Point = (45,-10)
            Box = Canvas.AddScaledTextBox("A Two Line\nString",
                                          Point,
                                          2,
                                          Color = "Black",
                                          BackgroundColor = 'Red',
                                          LineColor = "Blue",
                                          LineStyle = "LongDash",
                                          LineWidth = 2,
                                          Width = None,
                                          PadSize = 3,
                                          Family = wx.TELETYPE,
                                          Style = wx.NORMAL,
                                          Weight = wx.NORMAL,
                                          Underline = False,
                                          Position = 'tc',
                                          Alignment = "left",
                                          InForeground = False)

            Box = Canvas.AddScaledTextBox("A three\nLine\nString",
                                          Point,
                                          1.5,
                                          Color = "Black",
                                          BackgroundColor = 'Red',
                                          LineColor = "Blue",
                                          LineStyle = "LongDash",
                                          LineWidth = 2,
                                          Width = None,
                                          PadSize = 0.5,
                                          Family = wx.TELETYPE,
                                          Style = wx.NORMAL,
                                          Weight = wx.NORMAL,
                                          Underline = False,
                                          Position = 'bc',
                                          Alignment = "left",
                                          InForeground = False)


            Canvas.AddPoint(Point, Diameter = 4)

            Box = Canvas.AddScaledTextBox("Some Auto Wrapped Text. There is enough to do.",
                                          (80,40),
                                          2,
                                          BackgroundColor = 'White',
                                          LineWidth = 2,
                                          Width = 20,
                                          PadSize = 0.5,
                                          Family = wx.TELETYPE,
                                          )

            Box = Canvas.AddScaledTextBox("Some more auto wrapped text. Wrapped to a different width.\n\nThis is another paragraph.",
                                          (80,20),
                                          2,
                                          BackgroundColor = 'White',
                                          LineWidth = 2,
                                          Width = 40,
                                          PadSize = 0.5,
                                          Family = wx.ROMAN,
                                          Alignment = "right"
                                          )
            Point = Numeric.array((100, -20), Numeric.Float)
            Box = Canvas.AddScaledTextBox("Here is even more auto wrapped text. This time the line spacing is set to 0.8. \n\nThe Padding is set to 0.",
                                          Point,
                                          Size = 3,
                                          BackgroundColor = 'White',
                                          LineWidth = 1,
                                          Width = 40,
                                          PadSize = 0.0,
                                          Family = wx.ROMAN,
                                          Position = "cc",
                                          LineSpacing = 0.8
                                          )
            Canvas.AddPoint(Point, "Red", 2)

            Point = Numeric.array((0, -40), Numeric.Float)
    #        Point = Numeric.array((0, 0), Numeric.Float)
            for Position in ["tl", "bl", "tr", "br"]:
    #        for Position in ["br"]:
                Box = Canvas.AddScaledTextBox("Here is a\nfour liner\nanother line\nPosition=%s"%Position,
                                          Point,
                                          Size = 4,
                                          Color = "Red",
                                          BackgroundColor = None,#'LightBlue',
                                          LineWidth = 1,
                                          LineColor = "White",
                                          Width = None,
                                          PadSize = 2,
                                          Family = wx.ROMAN,
                                          Position = Position,
                                          LineSpacing = 0.8
                                          )
            Canvas.AddPoint(Point, "Red", 4)

            Point = Numeric.array((-20, 60), Numeric.Float)
            Box = Canvas.AddScaledTextBox("Here is some\ncentered\ntext",
                                          Point,
                                          Size = 4,
                                          Color = "Red",
                                          BackgroundColor = 'LightBlue',
                                          LineWidth = 1,
                                          LineColor = "White",
                                          Width = None,
                                          PadSize = 2,
                                          Family = wx.ROMAN,
                                          Position = "tl",
                                          Alignment = "center",
                                          LineSpacing = 0.8
                                          )

            Point = Numeric.array((-20, 20), Numeric.Float)
            Box = Canvas.AddScaledTextBox("Here is some\nright aligned\ntext",
                                          Point,
                                          Size = 4,
                                          Color = "Red",
                                          BackgroundColor = 'LightBlue',
                                          LineColor = None,
                                          Width = None,
                                          PadSize = 2,
                                          Family = wx.ROMAN,
                                          Position = "tl",
                                          Alignment = "right",
                                          LineSpacing = 0.8
                                          )

            Point = Numeric.array((100, -60), Numeric.Float)
            Box = Canvas.AddScaledTextBox("Here is some auto wrapped text. This time it is centered, rather than right aligned.\n\nThe Padding is set to 2.",
                                          Point,
                                          Size = 3,
                                          BackgroundColor = 'White',
                                          LineWidth = 1,
                                          Width = 40,
                                          PadSize = 2.0,
                                          Family = wx.ROMAN,
                                          Position = "cc",
                                          LineSpacing = 0.8,
                                          Alignment = 'center',
                                          )




            self.Canvas.ZoomToBB()

        def binding2(self, event):
            self.Log("I'm the TextBox")

        def TestBitmap(self, event= None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Canvas.AddRectangle((10, 20),
                                (400, 100),
                                LineWidth = 3,
                                LineColor = "Blue",
                                FillColor = "Red")

            bmp = Resources.getMagPlusBitmap()

            Canvas.AddText("These are Unscaled Bitmaps:", (140, 90))
          
            Point = (150, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "cc" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (200, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "br" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (200, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "bl" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (200, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "tr" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (200, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "tl" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (250, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "cr" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (250, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "cl" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (300, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "tc" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (300, 50)
            BitMap = Canvas.AddBitmap(bmp, Point, Position = "bc" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Canvas.AddScaledText("These are Scaled Bitmaps:", (220, -60), Size = 10, Position = "tr")
            Point = (250, -100)
            BitMap = Canvas.AddScaledBitmap(bmp, Point, Height = 50, Position = "bc" )
            BitMap = Canvas.AddScaledBitmap(bmp, Point, Height = 50, Position = "tc" )
            Canvas.AddPoint(Point, Diameter=4, Color="Green")

            Point = (300, -100)
            BitMap = Canvas.AddScaledBitmap(Resources.getMondrianImage(), Point, Height = 50)

            self.Canvas.ZoomToBB()

        def DrawMap(self,event = None):
            wx.GetApp().Yield()
            import os, time
            self.BindAllMouseEvents()

        ## Test of Actual Map Data
            self.Canvas.ClearAll()
            self.Canvas.SetProjectionFun("FlatEarth")
            #start = time.clock()
            self.Log("Loading Map from a File")
            wx.GetApp().Yield() # so log text will get displayed now.
            Shorelines = self.Read_MapGen(os.path.join("data",'world.dat'),stats = 0)
            #print "It took %f seconds to load %i shorelines"%(time.clock() - start,len(Shorelines) )
            #start = time.clock()
            for segment in Shorelines:
                self.Canvas.AddLine(segment)
            #print "It took %f seconds to add %i shorelines"%(time.clock() - start,len(Shorelines) )
            #start = time.clock()
            self.Canvas.ZoomToBB()
            #print "It took %f seconds to draw %i shorelines"%(time.clock() - start,len(Shorelines) )


        def LineTest(self,event = None):
            wx.GetApp().Yield()
            import os, time
#            import random
            colors = self.colors
            Range = (-10,10)
        ## Test of drawing lots of lines
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)
            #start = time.clock()
            linepoints = []
            linecolors = []
            linewidths = []
            for i in range(2000):
                points = (random.randint(Range[0],Range[1]),
                         random.randint(Range[0],Range[1]),
                         random.randint(Range[0],Range[1]),
                         random.randint(Range[0],Range[1]))
                linepoints.append(points)
                linewidths.append(random.randint(1,10) )
                linecolors.append(random.randint(0,len(colors)-1) )
            for (points,color,width) in zip(linepoints,linecolors,linewidths):
                Canvas.AddLine((points[0:2],points[2:4]), LineWidth = width, LineColor = colors[color])
            #print "It took %f seconds to add %i lines"%(time.clock() - start,len(linepoints) )
            #start = time.clock()
            Canvas.ZoomToBB()
            #print "It took %f seconds to draw %i lines"%(time.clock() - start,len(linepoints) )

        def SpeedTest(self,event=None):
            wx.GetApp().Yield()
            BigRange = (-1000,1000)
            colors = self.colors

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            # Pointset
            coords = []
            for i in range(1000):
                Point = (random.uniform(BigRange[0],BigRange[1]),random.uniform(BigRange[0],BigRange[1]))
                coords.append( (Point) )
            print "Drawing the Points"
            start = time.clock()
            for Point in coords:
                Canvas.AddPoint(Point, Diameter = 4)
            print "It took %s seconds to add the points"%(time.clock() - start)
            Canvas.ZoomToBB()

        def PropertiesChangeTest(self,event=None):
            wx.GetApp().Yield()

            Range = (-10,10)
            colors = self.colors

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            self.ColorObjectsAll = []
            self.ColorObjectsLine = []
            self.ColorObjectsColor = []
            self.ColorObjectsText = []
            ##One of each object:
            # Rectangle
            Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            lw = random.randint(1,5)
            cf = random.randint(0,len(colors)-1)
            wh = ( random.randint(1,5), random.randint(1,5) )
            self.Rectangle = Canvas.AddRectangle(Point, wh, LineWidth = lw, FillColor = colors[cf])
            self.ColorObjectsAll.append(self.Rectangle)

            # Ellipse
            Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            lw = random.randint(1,5)
            cf = random.randint(0,len(colors)-1)
            wh = ( random.randint(1,5), random.randint(1,5) )
            self.Ellipse = Canvas.AddEllipse(Point, wh, LineWidth = lw, FillColor = colors[cf])
            self.ColorObjectsAll.append(self.Ellipse)

            # Point 
            xy = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            D = random.randint(1,50)
            lw = random.randint(1,5)
            cf = random.randint(0,len(colors)-1)
            cl = random.randint(0,len(colors)-1)
            self.ColorObjectsColor.append(Canvas.AddPoint(xy, colors[cf], D))

            # Circle
            Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            D = random.randint(1,5)
            lw = random.randint(1,5)
            cf = random.randint(0,len(colors)-1)
            cl = random.randint(0,len(colors)-1)
            self.Circle = Canvas.AddCircle(Point, D, LineWidth = lw, LineColor = colors[cl], FillColor = colors[cf])
            self.ColorObjectsAll.append(self.Circle)

            # Line
            points = []
            for j in range(random.randint(2,10)):
                point = (random.randint(Range[0],Range[1]),random.randint(Range[0],Range[1]))
                points.append(point)
            lw = random.randint(1,10)
            cf = random.randint(0,len(colors)-1)
            cl = random.randint(0,len(colors)-1)
            self.ColorObjectsLine.append(Canvas.AddLine(points, LineWidth = lw, LineColor = colors[cl]))

            # Polygon
##            points = []
##            for j in range(random.randint(2,6)):
##                point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
##                points.append(point)
            points = RandomArray.uniform(Range[0],Range[1],(6,2))
            lw = random.randint(1,6)
            cf = random.randint(0,len(colors)-1)
            cl = random.randint(0,len(colors)-1)
            self.ColorObjectsAll.append(Canvas.AddPolygon(points,
                                                       LineWidth = lw, 
                                                       LineColor = colors[cl],
                                                       FillColor = colors[cf],
                                                       FillStyle = 'Solid'))

            ## Pointset
            points = RandomArray.uniform(Range[0],Range[1],(100,2))
            cf = random.randint(0,len(colors)-1)
            D = random.randint(1,4)
            self.PointSet = Canvas.AddPointSet(points, Color = colors[cf], Diameter = D)
            self.ColorObjectsColor.append(self.PointSet)

            ## Point
            point = RandomArray.uniform(Range[0],Range[1],(2,))
            cf = random.randint(0,len(colors)-1)
            D = random.randint(1,4)
            self.Point = Canvas.AddPoint(point, Color = colors[cf], Diameter = D)
            self.ColorObjectsColor.append(self.Point)

            # Text
            String = "Unscaled text"
            ts = random.randint(10,40)
            cf = random.randint(0,len(colors)-1)
            Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            self.ColorObjectsText.append(Canvas.AddText(String, Point, Size = ts, Color = colors[cf], Position = "cc"))

            # Scaled Text
            String = "Scaled text"
            ts = random.random()*3 + 0.2
            cf = random.randint(0,len(colors)-1)
            Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
            self.ColorObjectsText.append(Canvas.AddScaledText(String, Point, Size = ts, Color = colors[cf], Position = "cc"))

            # A "Button"
            Button = Canvas.AddRectangle((-10, -12), (20, 3), LineStyle = None, FillColor = "Red")
            Canvas.AddScaledText("Click Here To Change Properties",
                                 (0, -10.5),
                                 Size = 0.7,
                                 Color = "Black",
                                 Position = "cc")

            Button.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.ChangeProperties)

            Canvas.ZoomToBB()

        def ChangeProperties(self, Object = None):
            colors = self.colors
            Range = (-10,10)

            for Object in self.ColorObjectsAll:
                pass
                Object.SetFillColor(colors[random.randint(0,len(colors)-1)])
                Object.SetLineColor(colors[random.randint(0,len(colors)-1)])
                Object.SetLineWidth(random.randint(1,7))
                Object.SetLineStyle(FloatCanvas.DrawObject.LineStyleList.keys()[random.randint(0,5)])
            for Object in self.ColorObjectsLine:
                Object.SetLineColor(colors[random.randint(0,len(colors)-1)])
                Object.SetLineWidth(random.randint(1,7))
                Object.SetLineStyle(FloatCanvas.DrawObject.LineStyleList.keys()[random.randint(0,5)])
            for Object in self.ColorObjectsColor:
                Object.SetColor(colors[random.randint(0,len(colors)-1)])
            for Object in self.ColorObjectsText:
                Object.SetColor(colors[random.randint(0,len(colors)-1)])
                Object.SetBackgroundColor(colors[random.randint(0,len(colors)-1)])
            self.Circle.SetDiameter(random.randint(1,10))
            self.PointSet.SetDiameter(random.randint(1,8))
            self.Point.SetDiameter(random.randint(1,8))
            for Object in (self.Rectangle, self.Ellipse):
                Point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                wh = ( random.randint(1,5), random.randint(1,5) )
                Object.SetShape(Point, wh)
            self.Canvas.Draw(Force = True)

        def ArrowTest(self,event=None):
            wx.GetApp().Yield()
            self.UnBindAllMouseEvents()
            Canvas = self.Canvas

            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            # put in a rectangle to get a bounding box
            Canvas.AddRectangle((0,0), (20,20), LineColor = None)

            # Draw some Arrows
            Canvas.AddArrow((10,10),Length = 40, Direction = 0)
            Canvas.AddArrow((10,10),Length = 50, Direction = 45 ,LineWidth = 2, LineColor = "Black", ArrowHeadAngle = 20)
            Canvas.AddArrow((10,10),Length = 60, Direction = 90 ,LineWidth = 3, LineColor = "Red",   ArrowHeadAngle = 30)
            Canvas.AddArrow((10,10),Length = 70, Direction = 135,LineWidth = 4, LineColor = "Red",   ArrowHeadAngle = 40)
            Canvas.AddArrow((10,10),Length = 80, Direction = 180,LineWidth = 5, LineColor = "Blue",  ArrowHeadAngle = 50)
            Canvas.AddArrow((10,10),Length = 90, Direction = 225,LineWidth = 4, LineColor = "Blue",  ArrowHeadAngle = 60)
            Canvas.AddArrow((10,10),Length = 100,Direction = 270,LineWidth = 3, LineColor = "Green", ArrowHeadAngle = 70)
            Canvas.AddArrow((10,10),Length = 110,Direction = 315,LineWidth = 2, LineColor = "Green", ArrowHeadAngle = 90 )

            Canvas.AddText("Clickable Arrow", (4,18), Position = "bc")
            Arrow = Canvas.AddArrow((4,18), 80, Direction = 90 ,LineWidth = 3, LineColor = "Red",   ArrowHeadAngle = 30)
            Arrow.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.ArrowClicked)

            Canvas.AddText("Changable Arrow", (16,4), Position = "cc")
            self.RotArrow = Canvas.AddArrow((16,4), 80, Direction = 0 ,LineWidth = 3, LineColor = "Green",   ArrowHeadAngle = 30)
            self.RotArrow.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.RotateArrow)

            Canvas.ZoomToBB()

        def ArrowClicked(self,event):
            print "The Arrow was Clicked"

        def RotateArrow(self,event):
            ##print "The Changeable Arrow was Clicked"
            ## You can do them either one at a time, or both at once
            ## Doing them both at once prevents the arrow points from being calculated twice
            #self.RotArrow.SetDirection(self.RotArrow.Direction + random.uniform(-90,90))
            #self.RotArrow.SetLength(self.RotArrow.Length + random.randint(-20,20))
            self.RotArrow.SetLengthDirection(self.RotArrow.Length + random.randint(-20,20),
                                             self.RotArrow.Direction + random.uniform(-90,90) )

            self.Canvas.Draw(Force = True)

        def HideTest(self, event=None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Range = (-10,10)

            # Create a couple random Polygons
            points = []
            for j in range(6):
                point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                points.append(point)
            Poly = Canvas.AddPolygon(points,
                                     LineWidth = 2,
                                     LineColor = "Black",
                                     FillColor = "LightBlue",
                                     FillStyle = 'Solid')

            points = []
            for j in range(6):
                point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                points.append(point)
            Poly2 = Canvas.AddPolygon(points,
                                     LineWidth = 2,
                                     LineColor = "Black",
                                     FillColor = "Purple",
                                     FillStyle = 'Solid',
                                     InForeground = True)

            HideButton = Canvas.AddScaledTextBox("Click To Hide\nBackground Polygon",
                                                      (-10, 0),
                                                      .5,
                                                      BackgroundColor="Red",
                                                      PadSize = 0.5,
                                                      Position = 'tr',
                                                      Alignment="center",
                                                      )
            HideButton.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.HidePoly)
            HideButton.HidePoly = Poly
            
            HideButton2 = Canvas.AddScaledTextBox("Click To Hide\nForeground Polygon",
                                                      (-10, 5),
                                                      .5,
                                                      BackgroundColor="Red",
                                                      PadSize = 0.5,
                                                      Position = 'tr',
                                                      Alignment="center",
                                                      )
            # Put a reference to the Polygon in the Button object
            HideButton2.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.HidePoly)
            HideButton2.HidePoly = Poly2
            

            Canvas.ZoomToBB()

        def HidePoly(self, Button):
            Poly = Button.HidePoly
            if Poly.Visible:
                Poly.Hide()
                Button.SetText(Button.String.replace("Hide","Show"))
            else:
                Poly.Show()
                Button.SetText(Button.String.replace("Show", "Hide"))
            self.Canvas.Draw(True)

        def TempTest(self, event= None):
            wx.GetApp().Yield()

            self.UnBindAllMouseEvents()
            Canvas = self.Canvas
            Canvas.ClearAll()
            Canvas.SetProjectionFun(None)

            Range = (-10,10)

            # Create a random Polygon
            points = []
            for j in range(6):
                point = (random.uniform(Range[0],Range[1]),random.uniform(Range[0],Range[1]))
                points.append(point)
            Poly = Canvas.AddPolygon(points,
                                     LineWidth = 2,
                                     LineColor = "Black",
                                     FillColor = "LightBlue",
                                     FillStyle = 'Solid')

            Poly.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.SelectPoly)

            self.SelectedPoly = None
            self.SelectPoints = []
            self.SelectedPoint = None

            Canvas.ZoomToBB()

        def SelectPoly(self, Object):
            print "In SelectPoly"
            Canvas = self.Canvas
            if Object is self.SelectedPoly:
                pass
            else:
                #fixme: Do something to unselect the old one
                self.SelectedPoly = Object
                Canvas.RemoveObjects(self.SelectPoints)
                self.SelectPoints = []
            # Draw points on the Vertices of the Selected Poly:
            for i, point in enumerate(Object.Points):
                P = Canvas.AddPointSet(point, Diameter = 6, Color = "Red")
                P.VerticeNum = i
                P.Bind(FloatCanvas.EVT_FC_LEFT_DOWN, self.SelectPointHit)
                self.SelectPoints.append(P)
            #Canvas.ZoomToBB()
            Canvas.Draw()

        def SelectPointHit(self, Point):
            print "Point Num: %i Hit"%Point.VerticeNum
            self.SelectedPoint = Point

        def Read_MapGen(self, filename, stats = 0,AllLines=0):
            """
            This function reads a MapGen Format file, and
            returns a list of NumPy arrays with the line segments in them.

            Each NumPy array in the list is an NX2 array of Python Floats.

            The demo should have come with a file, "world.dat" that is the
            shorelines of the whole world, in MapGen format.

            """
            import string
            file = open(filename,'rt')
            data = file.readlines()
            data = map(string.strip,data)

            Shorelines = []
            segment = []
            for line in data:
                if line:
                    if line == "# -b": #New segment beginning
                        if segment: Shorelines.append(Numeric.array(segment))
                        segment = []
                    else:
                        segment.append(map(float,string.split(line)))
            if segment: Shorelines.append(Numeric.array(segment))

            if stats:
                NumSegments = len(Shorelines)
                NumPoints = 0
                for segment in Shorelines:
                    NumPoints = NumPoints + len(segment)
                AvgPoints = NumPoints / NumSegments
                print "Number of Segments: ", NumSegments
                print "Average Number of Points per segment: ",AvgPoints
            if AllLines:
                Lines = []
                for segment in Shorelines:
                    Lines.append(segment[0])
                    for point in segment[1:-1]:
                        Lines.append(point)
                        Lines.append(point)
                    Lines.append(segment[-1])
                return Lines
            else:
                return Shorelines
    return DrawFrame 

#---------------------------------------------------------------------------
      
if __name__ == "__main__":

    import wx
   
    
    # check options:
    import sys, getopt
    optlist, args = getopt.getopt(sys.argv[1:],'l',["local",
                                                    "all",
                                                    "text",
                                                    "map",
                                                    "stext",
                                                    "stextbox",
                                                    "bitmap",
                                                    "hit",
                                                    "hitf",
                                                    "animate",
                                                    "speed",
                                                    "temp",
                                                    "props",
                                                    "arrow",
                                                    "hide"])

    if not haveNumeric:
        raise ImportError(errorText)
    StartUpDemo = "all" # the default
    for opt in optlist:
        if opt[0] == "--all":
            StartUpDemo = "all"
        elif opt[0] == "--text":
            StartUpDemo = "text"
        elif opt[0] == "--map":
            StartUpDemo = "map"
        elif opt[0] == "--stext":
            StartUpDemo = "stext"
        elif opt[0] == "--stextbox":
            StartUpDemo = "stextbox"
        elif opt[0] == "--bitmap":
            StartUpDemo = "bitmap"
        elif opt[0] == "--hit":
            StartUpDemo = "hit"
        elif opt[0] == "--hitf":
            StartUpDemo = "hitf"
        elif opt[0] == "--animate":
            StartUpDemo = "animate"
        elif opt[0] == "--speed":
            StartUpDemo = "speed"
        elif opt[0] == "--temp":
            StartUpDemo = "temp"
        elif opt[0] == "--props":
            StartUpDemo = "props"
        elif opt[0] == "--arrow":
            StartUpDemo = "arrow"
        elif opt[0] == "--hide":
            StartUpDemo = "hide"

    class DemoApp(wx.App):
        """
        How the demo works:

        Under the Draw menu, there are three options:

        *Draw Test: will put up a picture of a bunch of randomly generated
        objects, of each kind supported.

        *Draw Map: will draw a map of the world. Be patient, it is a big map,
        with a lot of data, and will take a while to load and draw (about 10 sec 
        on my 450Mhz PIII). Redraws take about 2 sec. This demonstrates how the
        performance is not very good for large drawings.

        *Clear: Clears the Canvas.

        Once you have a picture drawn, you can zoom in and out and move about
        the picture. There is a tool bar with three tools that can be
        selected. 

        The magnifying glass with the plus is the zoom in tool. Once selected,
        if you click the image, it will zoom in, centered on where you
        clicked. If you click and drag the mouse, you will get a rubber band
        box, and the image will zoom to fit that box when you release it.

        The magnifying glass with the minus is the zoom out tool. Once selected,
        if you click the image, it will zoom out, centered on where you
        clicked. (note that this takes a while when you are looking at the map,
        as it has a LOT of lines to be drawn. The image is double buffered, so
        you don't see the drawing in progress)

        The hand is the move tool. Once selected, if you click and drag on the
        image, it will move so that the part you clicked on ends up where you
        release the mouse. Nothing is changed while you are dragging. The
        drawing is too slow for that.

        I'd like the cursor to change as you change tools, but the stock
        wxCursors didn't include anything I liked, so I stuck with the
        pointer. Please let me know if you have any nice cursor images for me to
        use.


        Any bugs, comments, feedback, questions, and especially code are welcome:

        -Chris Barker

        Chris.Barker@noaa.gov

        """

        def __init__(self, *args, **kwargs):
            wx.App.__init__(self, *args, **kwargs)

        def OnInit(self):
            wx.InitAllImageHandlers()
            DrawFrame = BuildDrawFrame()
            frame = DrawFrame(None, -1, "FloatCanvas Demo App",wx.DefaultPosition,(700,700))

            self.SetTopWindow(frame)
            frame.Show()

            ## check to see if the demo is set to start in a particular mode.
            if StartUpDemo == "text":
                frame.TestText()
            elif StartUpDemo == "stext":
                frame.TestScaledText()
            elif StartUpDemo == "stextbox":
                frame.TestScaledTextBox()
            elif StartUpDemo == "bitmap":
                frame.TestBitmap()
            elif StartUpDemo == "all":
                frame.DrawTest()
            elif StartUpDemo == "map":
                frame.DrawMap()
            elif StartUpDemo == "hit":
                frame.TestHitTest()
            elif StartUpDemo == "hitf":
                "starting TestHitTestForeground"
                frame.TestHitTestForeground()
            elif StartUpDemo == "animate":
                "starting TestAnimation"
                frame.TestAnimation()
            elif StartUpDemo == "speed":
                "starting SpeedTest"
                frame.SpeedTest()
            elif StartUpDemo == "temp":
                "starting temp Test"
                frame.TempTest()
            elif StartUpDemo == "props":
                "starting PropertiesChange Test"
                frame.PropertiesChangeTest()
            elif StartUpDemo == "arrow":
                "starting arrow Test"
                frame.ArrowTest()
            elif StartUpDemo == "hide":
                "starting Hide Test"
                frame.HideTest()

            return True

    app = DemoApp(False)# put in True if you want output to go to it's own window.
    app.MainLoop()

else:
    # It's not running stand-alone, set up for wxPython demo.
    # don't  neeed wxversion here.
    import wx
    if not haveNumeric:
        ## TestPanel and runTest used for integration into wxPython Demo
        class TestPanel(wx.Panel):
            def __init__(self, parent, log):
                self.log = log
                wx.Panel.__init__(self, parent, -1)

                import images

                note1 = wx.StaticText(self, -1, errorText)
                note2 = wx.StaticText(self, -1, "This is what the FloatCanvas can look like:")
                S = wx.BoxSizer(wx.VERTICAL)
                S.Add((10, 10), 1)
                S.Add(note1, 0, wx.ALIGN_CENTER)
                S.Add(note2, 0, wx.ALIGN_CENTER | wx.BOTTOM, 4)
                S.Add(wx.StaticBitmap(self,-1,images.getFloatCanvasBitmap()),0,wx.ALIGN_CENTER)
                S.Add((10, 10), 1)
                self.SetSizer(S)
                self.Layout()

    else:
        ## TestPanel and runTest used for integration into wxPython Demo
        class TestPanel(wx.Panel):
            def __init__(self, parent, log):
                self.log = log
                wx.Panel.__init__(self, parent, -1)
                note1 = wx.StaticText(self, -1, "The FloatCanvas Demo needs")
                note2 = wx.StaticText(self, -1, "a separate frame")
                b = wx.Button(self, -1, "Open Demo Frame Now")
                b.Bind(wx.EVT_BUTTON, self.OnButton)

                S = wx.BoxSizer(wx.VERTICAL)
                S.Add((10, 10), 1)
                S.Add(note1, 0, wx.ALIGN_CENTER)
                S.Add(note2, 0, wx.ALIGN_CENTER | wx.BOTTOM, 5)
                S.Add(b, 0, wx.ALIGN_CENTER | wx.ALL, 5)
                S.Add((10, 10), 1)
                self.SetSizer(S)
                self.Layout()

            def OnButton(self, evt):
                DrawFrame = BuildDrawFrame()
                frame = DrawFrame(None, -1, "FloatCanvas Drawing Window",wx.DefaultPosition,(500,500))

                #win = wx.lib.plot.TestFrame(self, -1, "PlotCanvas Demo")
                frame.Show()
                frame.DrawTest()

    def runTest(frame, nb, log):
        win = TestPanel(nb, log)
        return win

    # import to get the doc
    from wx.lib import floatcanvas
    overview = floatcanvas.__doc__








