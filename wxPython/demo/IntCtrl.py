
import  wx
import  wx.lib.intctrl

#----------------------------------------------------------------------

class TestPanel( wx.Panel ):
    def __init__( self, parent, log ):

        wx.Panel.__init__( self, parent, -1 )
        self.log = log
        panel = wx.Panel( self, -1 )

        self.set_min = wx.CheckBox( panel, -1, "Set minimum value:" )
        self.min = wx.lib.intctrl.IntCtrl( panel, size=( 50, -1 ) )
        self.min.Enable( False )

        self.set_max = wx.CheckBox( panel, -1, "Set maximum value:" )
        self.max = wx.lib.intctrl.IntCtrl( panel, size=( 50, -1 ) )
        self.max.Enable( False )

        self.limit_target = wx.CheckBox( panel, -1, "Limit control" )
        self.allow_none = wx.CheckBox( panel, -1, "Allow empty control" )
        self.allow_long = wx.CheckBox( panel, -1, "Allow long integers" )

        label = wx.StaticText( panel, -1, "Resulting integer control:" )
        self.target_ctl = wx.lib.intctrl.IntCtrl( panel )

        grid = wx.FlexGridSizer( 0, 2, 0, 0 )
        grid.Add( self.set_min, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid.Add( self.min, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid.Add(self.set_max, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid.Add( self.max, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid.Add( self.limit_target, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( self.allow_none, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( self.allow_long, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid.Add( label, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid.Add( self.target_ctl, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        outer_box = wx.BoxSizer( wx.VERTICAL )
        outer_box.Add( grid, 0, wx.ALIGN_CENTRE|wx.ALL, 20 )

        panel.SetAutoLayout( True )
        panel.SetSizer( outer_box )
        outer_box.Fit( panel )
        panel.Move( (50,50) )
        self.panel = panel

        self.Bind(wx.EVT_CHECKBOX, self.OnSetMin, self.set_min)
        self.Bind(wx.EVT_CHECKBOX, self.OnSetMax, self.set_max)
        self.Bind(wx.EVT_CHECKBOX, self.SetTargetMinMax, self.limit_target)
        self.Bind(wx.EVT_CHECKBOX, self.OnSetAllowNone, self.allow_none)
        self.Bind(wx.EVT_CHECKBOX, self.OnSetAllowLong, self.allow_long)

        self.Bind(wx.lib.intctrl.EVT_INT, self.SetTargetMinMax, self.min)
        self.Bind(wx.lib.intctrl.EVT_INT, self.SetTargetMinMax, self.max)
        self.Bind(wx.lib.intctrl.EVT_INT, self.OnTargetChange, self.target_ctl)


    def OnSetMin( self, event ):
        self.min.Enable( self.set_min.GetValue() )
        self.SetTargetMinMax()

    def OnSetMax( self, event ):
        self.max.Enable( self.set_max.GetValue() )
        self.SetTargetMinMax()


    def SetTargetMinMax( self, event=None ):
        min = max = None
        self.target_ctl.SetLimited( self.limit_target.GetValue() )

        if self.set_min.GetValue():
            min = self.min.GetValue()

        if self.set_max.GetValue():
            max = self.max.GetValue()

        cur_min, cur_max = self.target_ctl.GetBounds()

        if min != cur_min and not self.target_ctl.SetMin( min ):
            self.log.write( "min (%d) > current max (%d) -- bound not set\n" % ( min, self.target_ctl.GetMax() ) )
            self.min.SetForegroundColour( wx.RED )
        else:
            self.min.SetForegroundColour( wx.BLACK )

        self.min.Refresh()

        if max != cur_max and not self.target_ctl.SetMax( max ):
            self.log.write( "max (%d) < current min (%d) -- bound not set\n" % ( max, self.target_ctl.GetMin() ) )
            self.max.SetForegroundColour( wx.RED )
        else:
            self.max.SetForegroundColour( wx.BLACK )

        self.max.Refresh()

        if min != cur_min or max != cur_max:
            new_min, new_max = self.target_ctl.GetBounds()
            self.log.write( "current min, max:  (%s, %s)\n" % ( str(new_min), str(new_max) ) )


    def OnSetAllowNone( self, event ):
        self.target_ctl.SetNoneAllowed( self.allow_none.GetValue() )


    def OnSetAllowLong( self, event ):
        self.target_ctl.SetLongAllowed( self.allow_long.GetValue() )


    def OnTargetChange( self, event ):
        ctl = event.GetEventObject()
        value = ctl.GetValue()
        ib_str = [ "  (out of bounds)", "" ]
        self.log.write( "integer value = %s%s\n" % ( str(value), ib_str[ ctl.IsInBounds(value) ] ) )


#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------

overview = """<html><body>
<P>
<B>IntCtrl</B> provides a control that takes and returns integers as
value, and provides bounds support and optional value limiting.
<P>
<P>
Here's the API for IntCtrl:
<DL><PRE>
    <B>IntCtrl</B>(
         parent, id = -1,
         <B>value</B> = 0,
         <B>min</B> = None,
         <B>max</B> = None,
         <B>limited</B> = False,
         <B>allow_none</B> = False,
         <B>allow_long</B> = False,
         <B>default_color</B> = wxBLACK,
         <B>oob_color</B> = wxRED,
         pos = wxDefaultPosition,
         size = wxDefaultSize,
         style = 0,
         name = "integer")
</PRE>
<UL>
    <DT><B>value</B>
    <DD>If no initial value is set, the default will be zero, or
    the minimum value, if specified.  If an illegal string is specified,
    a ValueError will result. (You can always later set the initial
    value with SetValue() after instantiation of the control.)
    <BR>
    <DL><B>min</B>
    <DD>The minimum value that the control should allow.  This can be
    adjusted with SetMin().  If the control is not limited, any value
    below this bound will be colored with the current out-of-bounds color.
    <BR>
    <DT><B>max</B>
    <DD>The maximum value that the control should allow.  This can be
    adjusted with SetMax().  If the control is not limited, any value
    above this bound will be colored with the current out-of-bounds color.
    <BR>
    <DT><B>limited</B>
    <DD>Boolean indicating whether the control prevents values from
    exceeding the currently set minimum and maximum values (bounds).
    If <I>False</I> and bounds are set, out-of-bounds values will
    be colored with the current out-of-bounds color.
    <BR>
    <DT><B>allow_none</B>
    <DD>Boolean indicating whether or not the control is allowed to be
    empty, representing a value of <I>None</I> for the control.
    <BR>
    <DT><B>allow_long</B>
    <DD>Boolean indicating whether or not the control is allowed to hold
    and return a value of type long as well as int.  If False, the
    control will be implicitly limited to have a value such that
    -sys.maxint-1 &lt;= n &lt;= sys.maxint.
    <BR>
    <DT><B>default_color</B>
    <DD>Color value used for in-bounds values of the control.
    <BR>
    <DT><B>oob_color</B>
    <DD>Color value used for out-of-bounds values of the control
    when the bounds are set but the control is not limited.
</UL>
<BR>
<BR>
<DT><B>EVT_INT(win, id, func)</B>
<DD>Respond to a wxEVT_COMMAND_INT_UPDATED event, generated when the
value changes. Notice that this event will always be sent when the
control's contents changes - whether this is due to user input or
comes from the program itself (for example, if SetValue() is called.)
<BR>
<BR>
<DT><B>SetValue(int)</B>
<DD>Sets the value of the control to the integer value specified.
The resulting actual value of the control may be altered to
conform with the bounds set on the control if limited,
or colored if not limited but the value is out-of-bounds.
A ValueError exception will be raised if an invalid value
is specified.
<BR>
<DT><B>GetValue()</B>
<DD>Retrieves the integer value from the control.  The value
retrieved will be sized as an int if possible or a long,
if necessary.
<BR>
<BR>
<DT><B>SetMin(min=None)</B>
<DD>Sets the expected minimum value, or lower bound, of the control.
(The lower bound will only be enforced if the control is
configured to limit its values to the set bounds.)
If a value of <I>None</I> is provided, then the control will have
no explicit lower bound.  If the value specified is greater than
the current lower bound, then the function returns 0 and the
lower bound will not change from its current setting.  On success,
the function returns 1.
<DT><DD>If successful and the current value is
lower than the new lower bound, if the control is limited, the
value will be automatically adjusted to the new minimum value;
if not limited, the value in the control will be colored with
the current out-of-bounds color.
<BR>
<DT><B>GetMin()</B>
<DD>Gets the current lower bound value for the control.
It will return None if no lower bound is currently specified.
<BR>
<BR>
<DT><B>SetMax(max=None)</B>
<DD>Sets the expected maximum value, or upper bound, of the control.
(The upper bound will only be enforced if the control is
configured to limit its values to the set bounds.)
If a value of <I>None</I> is provided, then the control will
have no explicit upper bound.  If the value specified is less
than the current lower bound, then the function returns 0 and
the maximum will not change from its current setting. On success,
the function returns 1.
<DT><DD>If successful and the current value
is greater than the new upper bound, if the control is limited
the value will be automatically adjusted to the new maximum value;
if not limited, the value in the control will be colored with the
current out-of-bounds color.
<BR>
<DT><B>GetMax()</B>
<DD>Gets the current upper bound value for the control.
It will return None if no upper bound is currently specified.
<BR>
<BR>
<DT><B>SetBounds(min=None,max=None)</B>
<DD>This function is a convenience function for setting the min and max
values at the same time.  The function only applies the maximum bound
if setting the minimum bound is successful, and returns True
only if both operations succeed.  <B><I>Note:</I></B> leaving out an argument
will remove the corresponding bound.
<DT><B>GetBounds()</B>
<DD>This function returns a two-tuple (min,max), indicating the
current bounds of the control.  Each value can be None if
that bound is not set.
<BR>
<BR>
<DT><B>IsInBounds(value=None)</B>
<DD>Returns <I>True</I> if no value is specified and the current value
of the control falls within the current bounds.  This function can also
be called with a value to see if that value would fall within the current
bounds of the given control.
<BR>
<BR>
<DT><B>SetLimited(bool)</B>
<DD>If called with a value of True, this function will cause the control
to limit the value to fall within the bounds currently specified.
If the control's value currently exceeds the bounds, it will then
be limited accordingly.
If called with a value of 0, this function will disable value
limiting, but coloring of out-of-bounds values will still take
place if bounds have been set for the control.
<DT><B>IsLimited()</B>
<DD>Returns <I>True</I> if the control is currently limiting the
value to fall within the current bounds.
<BR>
<BR>
<DT><B>SetNoneAllowed(bool)</B>
<DD>If called with a value of True, this function will cause the control
to allow the value to be empty, representing a value of None.
If called with a value of false, this function will prevent the value
from being None.  If the value of the control is currently None,
ie. the control is empty, then the value will be changed to that
of the lower bound of the control, or 0 if no lower bound is set.
<DT><B>IsNoneAllowed()</B>
<DD>Returns <I>True</I> if the control currently allows its
value to be None.
<BR>
<BR>
<DT><B>SetLongAllowed(bool)</B>
<DD>If called with a value of True, this function will cause the
control to allow the value to be a long. If called with a value
of False, and the value of the control is currently a long value,
the value of the control will be adjusted to fall within the
size of an integer type, at either the sys.maxint or -sys.maxint-1,
for positive and negative values, respectively.
<DT><B>IsLongAllowed()</B>
<DD>Returns <I>True</I> if the control currently allows its
value to be of type long.
<BR>
<BR>
<DT><B>SetColors(default_color=wxBLACK, oob_color=wxRED)</B>
<DD>Tells the control what colors to use for normal and out-of-bounds
values.  If the value currently exceeds the bounds, it will be
recolored accordingly.
<DT><B>GetColors()</B>
<DD>Returns a tuple of <I>(default_color, oob_color)</I> indicating
the current color settings for the control.
<BR>
<BR>
<DT><B>Cut()</B>
<DD>Will allow cuts to the clipboard of the text portion of the value,
leaving the value of zero if the entire contents are "cut."
<DT><B>Paste()</B>
<DD>Will paste the contents of the clipboard to the selected portion
of the value; if the resulting string does not represent a legal
value, a ValueError will result.  If the result is out-of bounds,
it will either be adjusted or colored as appropriate.
</DL>
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
