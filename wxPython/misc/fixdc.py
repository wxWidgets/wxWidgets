"""
This module will do surgery on the wx.DC class in wxPython 2.5.1.5 to
make it act like the wx.DC class in later versions will.  To use this
module simply import it in one of your program's modules before you
use any DC's.  It does its work upon import and then is done.

This module will *only* do something if it is imported in an app
running on wxPython 2.5.1.5, for all other versions it will do
nothing.  This means that you can include it with your application and
then if your user is on 2.5.1.5 the DC methods will be updated, but if
they are on a newer version (or an older one for that matter) then
nothing will be done and your code using DCs will still be compatible.

So what does it do? In a nutshell, the old 2.4.x style of method
names, where the 'normal' name takes separate parameters for x, y,
width and height will be restored, and the new methods that take
wx.Point and/or wx.Size (which can also be converted from 2-element
sequences) will be given new non-default method names.  The 2.5.1.5
'XY' style names will be removed.  The new names for the 'Point/Size'
methods are:

    * BlitPointSize
    * CrossHairPoint
    * DrawArcPoint
    * DrawBitmapPoint
    * DrawCheckMarkRect
    * DrawCirclePoint
    * DrawEllipsePointSize
    * DrawEllipticArcPointSize
    * DrawIconPoint
    * DrawLinePoint
    * DrawPointPoint
    * DrawRectanglePointSize
    * DrawRotatedTextPoint
    * DrawRoundedRectanglePointSize
    * DrawTextPoint
    * FloodFillPoint
    * GetPixelPoint
    * SetClippingRegionPointSize

Please note that only the names that you access the methods by will be
changed.  The names used in docstrings as well as the names used to
call the extenaion functions and the names used when raising
exceptions will still use the old names.  (Of course once a new
version of wxPython has been built with this new style then this will
no longer apply.  The new names will be the real names.)  For
example::

  Traceback (most recent call last):
    File "/usr/lib/python2.3/site-packages/wx/lib/buttons.py", line 272, in OnPaint
      self.DrawBezel(dc, x1, y1, x2, y2)
    File "/usr/lib/python2.3/site-packages/wx/lib/buttons.py", line 220, in DrawBezel
      dc.DrawLine((x1+i, y1), (x1+i, y2-i))
    File "/usr/lib/python2.3/site-packages/wx/gdi.py", line 2293, in DrawLine
      return _gdi.DC_DrawLineXY(*args, **kwargs)
  TypeError: DC_DrawLineXY() takes exactly 5 arguments (3 given)

    
WARNING: If you import this module then the wx.DC class will be
         changed for the entire application, so if you use code from
         the wx.lib package (or 3rd party modules that have already
         been converted to the doomed 2.5.1.5 implementaion of the DC
         Draw methods) then that code will break as shown above.  This
         is an all-or-nothing fix, (just like the next version of
         wxPython will be,) so you *will* need to do something to
         resolve this situation if you run into it.  The best thing to
         do of course is to correct the library module to work with
         the corrected DC semantics and then send me a patch, although
         it probably won't be too long before the library modules are
         updated in CVS so you could get a copy of them there.

--Robin
"""

import wx

_names = [
    ("BlitXY",                  "Blit",                 "BlitPointSize"),
    ("CrossHairXY",             "CrossHair",            "CrossHairPoint"),
    ("DrawArcXY",               "DrawArc",              "DrawArcPoint"),
    ("DrawBitmapXY",            "DrawBitmap",           "DrawBitmapPoint"),
    ("DrawCheckMarkXY",         "DrawCheckMark",        "DrawCheckMarkRect"),
    ("DrawCircleXY",            "DrawCircle",           "DrawCirclePoint"),
    ("DrawEllipseXY",           "DrawEllipse",          "DrawEllipsePointSize"),
    ("DrawEllipticArcXY",       "DrawEllipticArc",      "DrawEllipticArcPointSize"),
    ("DrawIconXY",              "DrawIcon",             "DrawIconPoint"),
    ("DrawLineXY",              "DrawLine",             "DrawLinePoint"),
    ("DrawPointXY",             "DrawPoint",            "DrawPointPoint"),
    ("DrawRectangleXY",         "DrawRectangle",        "DrawRectanglePointSize"),
    ("DrawRotatedTextXY",       "DrawRotatedText",      "DrawRotatedTextPoint"),
    ("DrawRoundedRectangleXY",  "DrawRoundedRectangle", "DrawRoundedRectanglePointSize"),
    ("DrawTextXY",              "DrawText",             "DrawTextPoint"),
    ("FloodFillXY",             "FloodFill",            "FloodFillPoint"),
    ("GetPixelXY",              "GetPixel",             "GetPixelPoint"),
    ("SetClippingRegionXY",     "SetClippingRegion",    "SetClippingRegionPointSize"),
]


# this is a bit of handy code from the Python Cookbook
def _renamefunction(function, name):
    """
    This function returns a function identical to the given one, but
    with the given name.
    """
    from types import FunctionType, CodeType
    
    c = function.func_code
    if c.co_name != name:
        # rename the code object.
        c = CodeType(c.co_argcount, c.co_nlocals, c.co_stacksize,
                     c.co_flags, c.co_code, c.co_consts,
                     c.co_names, c.co_varnames, c.co_filename,
                     name, c.co_firstlineno, c.co_lnotab)
    if function.func_defaults != None:
        return FunctionType(c, function.func_globals, name,
                            function.func_defaults)
    return FunctionType(c, function.func_globals, name)


if wx.VERSION[:4] == (2,5,1,5):
    for old, norm, new in _names:
        m_old  = getattr(wx.DC, old)
        m_norm = getattr(wx.DC, norm)
        setattr(wx.DC, new,  _renamefunction(m_norm, new))
        setattr(wx.DC, norm, _renamefunction(m_old, norm))
        delattr(wx.DC, old)
        
    del old, norm, new, m_old, m_norm


