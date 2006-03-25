__author__  = "E. A. Tacao <e.a.tacao |at| estadao.com.br>"
__date__    = "15 Fev 2006, 22:00 GMT-03:00"
__version__ = "0.02"
__doc__     = """
AnalogClock - an analog clock.

This control creates an analog clock window. Its features include shadowing,
the ability to render numbers as well as any arbitrary polygon as tick marks,
resize marks and hands proportionally as the widget itself is resized, rotate
marks in a way the get aligned to the watch. It also has a dialog, accessed 
via a context menu item, allowing one to change on the fly all of its settings.


Usage:

       AnalogClock(parent, id=-1, pos=wx.DefaultPosition,
                   size=wx.DefaultSize, style=wx.NO_BORDER, name="AnalogClock",
                   clockStyle=DEFAULT_CLOCK_STYLE,
                   minutesStyle=TICKS_CIRCLE, hoursStyle=TICKS_POLY)

- parent, id, pos, size, style and name are used as in a wx.Window. Please
  refer to the wx.Window docs for more details.

- clockStyle defines the clock style, according to the options below:

    ====================  ================================
    SHOW_QUARTERS_TICKS   Show marks for hours 3, 6, 9, 12
    SHOW_HOURS_TICKS      Show marks for all hours
    SHOW_MINUTES_TICKS    Show marks for minutes

    SHOW_HOURS_HAND       Show hours hand
    SHOW_MINUTES_HAND     Show minutes hand
    SHOW_SECONDS_HAND     Show seconds hand

    SHOW_SHADOWS          Show hands and marks shadows

    ROTATE_TICKS          Align tick marks to watch
    OVERLAP_TICKS         Draw tick marks for minutes even
                          when they match the hours marks.

    DEFAULT_CLOCK_STYLE   The same as SHOW_HOURS_TICKS|
                          SHOW_MINUTES_TICKS|
                          SHOW_HOURS_HAND|
                          SHOW_MINUTES_HAND|
                          SHOW_SECONDS_HAND|
                          SHOW_SHADOWS|ROTATE_TICKS
    ====================  ================================

- minutesStyle and hoursStyle define the the tick styles, according to the
  options below:

    =================   ======================================
    TICKS_NONE          Don't show tick marks.
    TICKS_SQUARE        Use squares as tick marks.
    TICKS_CIRCLE        Use circles as tick marks.
    TICKS_POLY          Use a polygon as tick marks. A
                        polygon can be passed using
                        SetTickPolygon, otherwise the default
                        polygon will be used.
    TICKS_DECIMAL       Use decimal numbers as tick marks.
    TICKS_ROMAN         Use Roman numbers as tick marks.
    TICKS_BINARY        Use binary numbers as tick marks.
    TICKS_HEX           Use hexadecimal numbers as tick marks.
    =================   ======================================


Notes:

The 'target' keyword that's present in various of the AnalogClock methods may
accept one (or more, combined using '|') of the following values:

    =========  ===========================================
    HOUR       The values passed/retrieved are related to
               the hours hand/ticks

    MINUTE     The values passed/retrieved are related to
               the minutes hand/ticks

    SECOND     The values passed/retrieved are related to
               the seconds hand/ticks

    ALL        The same as HOUR|MINUTE|SECOND, i. e., the
               values passed/retrieved are related to all
               of the hours hands/ticks. This is the
               default value in all methods.
    =========  ===========================================

It is legal to pass target=ALL to methods that don't handle seconds (tick
mark related methods). In such cases, ALL will be equivalent to HOUR|MINUTE.

All of the 'Get' AnalogClock methods that allow the 'target' keyword
will always return a tuple, e. g.:

    =================================  ========================================
    GetHandSize(target=HOUR)           Returns a 1 element tuple, containing
                                       the size of the hours hand.

    GetHandSize(target=HOUR|MINUTE)    Returns a 2 element tuple, containing
                                       the sizes of the hours and the minutes
                                       hands, respectively.

    GetHandSize(target=ALL)            Returns a 3 element tuple, containing
         or                            the sizes of the hours, minutes and
    GetHandSize()                      seconds hands, respectively.
    =================================  ========================================


About:

Most of the ideas and part of the code of AnalogClock were based on the
original wxPython's AnalogClock module, which was created by several folks on
the wxPython-users list.

AnalogClock is distributed under the wxWidgets license.

This code should meet the wxPython Coding Guidelines 
<http://www.wxpython.org/codeguidelines.php> and the wxPython Style Guide
<http://wiki.wxpython.org/index.cgi/wxPython_20Style_20Guide>.

For all kind of problems, requests, enhancements, bug reports, etc,
please drop me an e-mail.

For updates please visit <http://j.domaindlx.com/elements28/wxpython/>.
"""

# History:
#
# Version 0.02:
#   - Module/namespace rearranges;
#   - All '-1' occurrences meaning "use any id" were eliminated or replaced 
#     to 'wx.ID_ANY'.
#   - Better names to the methods triggered by the context menu events.
#   - Included small demo class code in analogclock.py.
# Version 0.01:
#   - Initial release.

#----------------------------------------------------------------------

from analogclock import AnalogClock, AnalogClockWindow
from styles import *

#
##
### eof
