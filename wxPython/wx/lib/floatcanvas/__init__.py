"""
This is the floatcanvas package. It provides two primary modules, and a
support module.

FloatCanvas.py contains the main FloatCanvas class, and its supporting
classes.  NavCanvas.py contains a wrapper for the FloatCanvas that
provides the canvas and a toolbar with tools that allow you to navigate
the canvas (zooming, panning, etc.)  Resources.py is a module that
contains a few resources required by the FloatCanvas (icons, etc)

The FloatCanvas is a high level window for drawing maps and anything
else in an arbitrary coordinate system.

The goal is to provide a convenient way to draw stuff on the screen
without having to deal with handling OnPaint events, converting to pixel
coordinates, knowing about wxWindows brushes, pens, and colors, etc. It
also provides virtually unlimited zooming and scrolling

I am using it for two things:
1) general purpose drawing in floating point coordinates
2) displaying map data in Lat-long coordinates

If the projection is set to None, it will draw in general purpose
floating point coordinates. If the projection is set to 'FlatEarth', it
will draw a FlatEarth projection, centered on the part of the map that
you are viewing. You can also pass in your own projection function.

It is double buffered, so re-draws after the window is uncovered by
something else are very quick.

It relies on NumPy, which is needed for speed (maybe, I haven't profiled
it). It will also use numarray, if you don't have Numeric, but it is
slower.

Bugs and Limitations: Lots: patches, fixes welcome

For Map drawing: It ignores the fact that the world is, in fact, a
sphere, so it will do strange things if you are looking at stuff near
the poles or the date line. so far I don't have a need to do that, so I
havn't bothered to add any checks for that yet.

Zooming: I have set no zoom limits. What this means is that if you zoom
in really far, you can get integer overflows, and get weird results. It
doesn't seem to actually cause any problems other than weird output, at
least when I have run it.

Speed: I have done a couple of things to improve speed in this app. The
one thing I have done is used NumPy Arrays to store the coordinates of
the points of the objects. This allowed me to use array oriented
functions when doing transformations, and should provide some speed
improvement for objects with a lot of points (big polygons, polylines,
pointsets).

The real slowdown comes when you have to draw a lot of objects, because
you have to call the wx.DC.DrawSomething call each time. This is plenty
fast for tens of objects, OK for hundreds of objects, but pretty darn
slow for thousands of objects.

If you are zoomed in, it checks the Bounding box of an object before
drawing it. This makes it a great deal faster when there are a lot of
objects and you are zoomed in so that only a few are shown.

One solution is to be able to pass some sort of object set to the DC
directly. I've used DC.DrawPointList(Points), and it helped a lot with
drawing lots of points. However, when zoomed in, the Bounding boxes need
to be checked, so I may some day write C++ code that does the loop and
checks the BBs.

Mouse Events:

At this point, there are a full set of custom mouse events. They are
just like the regular mouse events, but include an extra attribute:
Event.GetCoords(), that returns the (x,y) position in world coordinates,
as a length-2 NumPy vector of Floats.

There are also a full set of bindings to mouse events on objects, so
that you can specify a given function be called when an objects is
clicked, mouse-over'd, etc.

See the Demo for what it can do, and how to use it.

Copyright: Christopher Barker

License: Same as the version of wxPython you are using it with.

Check for updates at:
http://home.comcast.net/~chrishbarker/FloatCanvas/

Please let me know if you're using this!!!

Contact me at:

Chris.Barker@noaa.gov

"""

__version__ = "0.8.7"


