"""Decorator classes for documentation and shell scripting.

Sizer is the abstract base class used for laying out subwindows in a
window.  You cannot use Sizer directly; instead, you will have to use
one of the sizer classes derived from it.  Currently there are
BoxSizer, StaticBoxSizer, NotebookSizer, GridSizer, and FlexGridSizer.

The layout algorithm used by sizers in wxPython is closely related to
layout in other GUI toolkits, such as Java's AWT, the GTK toolkit or
the Qt toolkit.  It is based upon the idea of the individual
subwindows reporting their minimal required size and their ability to
get stretched if the size of the parent window has changed.  This will
most often mean, that the programmer does not set the original size of
a dialog in the beginning, rather the dialog will assigned a sizer and
this sizer will be queried about the recommended size.  The sizer in
turn will query its children, which can be normal windows, empty space
or other sizers, so that a hierarchy of sizers can be constructed.
Note that wxSizer does not derive from wxWindow and thus do not
interfere with tab ordering and requires very little resources
compared to a real window on screen.

What makes sizers so well fitted for use in wxPython is the fact that
every control reports its own minimal size and the algorithm can
handle differences in font sizes or different window (dialog item)
sizes on different platforms without problems.  If e.g. the standard
font as well as the overall design of Motif widgets requires more
space than on Windows, the initial dialog size will automatically be
bigger on Motif than on Windows.

If you wish to create a sizer class in wxPython you should derive the
class from PySizer in order to get Python-aware capabilities for the
various virtual methods.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Sizer(Object):
    """Sizer is the abstract base class used for laying out subwindows
    in a window. You shouldn't use Sizer directly; instead, you should
    use one of the sizer classes derived from it.

    If you wish to create a sizer class in wxPython you should derive
    the class from PySizer in order to get Python-aware capabilities
    for the various virtual methods.

    Placing a child sizer in a sizer allows you to create hierarchies
    of sizers (typically a vertical box as the top sizer and several
    horizontal boxes on the level beneath).

    When you place a window in a sizer the window's initial size
    (either set explicitly by the user or calculated internally when
    using wxDefaultSize) is interpreted as the minimal and in many
    cases also the initial size.  This is particularly useful in
    connection with SetSizeHints.

    Adding spacers to sizers gives more flexibility in the design of
    dialogs.  Imagine for example a horizontal box with two buttons at
    the bottom of a dialog: you might want to insert a space between
    the two buttons and make that space stretchable using the
    proportion flag and the result will be that the left button will
    be aligned with the left side of the dialog and the right button
    with the right side - the space in between will shrink and grow
    with the dialog.

    Several methods (Add, Insert, Prepend) take the following
    parameters:

    proportion - Used only by BoxSizer to indicate if a child of a
    sizer can change its size in the main orientation of the BoxSizer,
    where 0 stands for not changeable and a value of more than zero is
    interpreted relative to the value of other children of the same
    BoxSizer.  For example, you might have a horizontal BoxSizer with
    three children, two of which are supposed to change their size
    with the sizer.  Then the two stretchable windows would each get a
    value of 1 to make them grow and shrink equally with the sizer's
    horizontal dimension.

    flag - This parameter can be used to set a number of flags which
    can be combined using the binary OR operator |.  Two main
    behaviours are defined using these flags.  One is the border
    around a window: the border parameter determines the border width
    whereas the flags given here determine where the border may be
    (wx.TOP, wx.BOTTOM, wx.LEFT, wx.RIGHT or wx.ALL).  The other flags
    determine the child window's behaviour if the size of the sizer
    changes.  However this is not - in contrast to the proportion flag
    - in the main orientation, but in the respectively other
    orientation.  So if you created a BoxSizer with the wx.VERTICAL
    option, these flags will be relevant if the sizer changes its
    horizontal size.  A child may get resized to completely fill out
    the new size (using either wx.GROW or wx.EXPAND), it may get
    proportionally resized (wx.SHAPED), it may get centered
    (wx.ALIGN_CENTER or wx.ALIGN_CENTRE) or it may get aligned to
    either side (wx.ALIGN_LEFT and wx.ALIGN_TOP are set to 0 and thus
    represent the default, wx.ALIGN_RIGHT and wx.ALIGN_BOTTOM have
    their obvious meaning).  With proportional resize, a child may
    also be centered in the main orientation using
    wx.ALIGN_CENTER_VERTICAL (same as wx.ALIGN_CENTRE_VERTICAL) and
    wx.ALIGN_CENTER_HORIZONTAL (same as wx.ALIGN_CENTRE_HORIZONTAL)
    flags.  Finally, you can also specify wx.ADJUST_MINSIZE flag to
    make the minimal size of the control dynamically adjust to the
    value returned by its GetAdjustedBestSize() method - this allows,
    for example, for correct relayouting of a static text control even
    if its text is changed during run-time.

    border - Determines the border width, if the flag parameter is set
    to any border.  A border is not a visible element, but rather a
    margin of empty space surrounding the item.

    userData - Allows an extra object to be attached to the sizer
    item, for use in derived classes when sizing information is more
    complex than the option and flag parameters will allow."""

    def __init__(self):
        """Must be defined by subclasses."""
        pass

    def Add(self, item, proportion=0, flag=0, border=0,
            userData=wx.NULL):
        """Add item to sizer.

        item - window, sizer, or spacer.  Spacer is specified with a
        (width, height) tuple or wx.Size representing the spacer size.

        Call Layout() to update the layout on-screen after adding."""
        pass

    def Clear(self, delete_windows=False):
        """Remove all items from this sizer.

        If delete_windows is True, destroy any window items."""
        pass

    def DeleteWindows(self):
        """Destroy windows associated with this sizer."""
        pass

    def Destroy(self):
        """Destroy the sizer."""
        pass

    def Fit(self, window):
        """Resize window to match sizer's minimal size; return size.

        This is commonly done in the constructor of the window itself."""
        pass

    def FitInside(self, window):
        """Resize window virtual size to match sizer's minimal size.

        This will not alter the on screen size of the window, but may
        cause the addition/removal/alteration of scrollbars required
        to view the virtual area in windows which manage it."""
        pass

    def GetChildren(self):
        """Return list of SizerItem instances."""
        pass

    def GetMinSize(self):
        """Return the minimal size of the sizer.

        This is either the combined minimal size of all the children
        and their borders or the minimal size set by SetMinSize,
        whichever is larger."""
        pass

    def GetMinSizeTuple(self):
        """Return the minimal size of the sizer as a tuple.

        This is either the combined minimal size of all the children
        and their borders or the minimal size set by SetMinSize,
        whichever is larger."""
        pass

    def GetPosition(self):
        """Return the current position of the sizer."""
        pass

    def GetPositionTuple(self):
        """Return the current position of the sizer as a tuple."""
        pass

    def GetSize(self):
        """Return the current size of the sizer."""
        pass

    def GetSizeTuple(self):
        """Return the current size of the sizer as a tuple."""
        pass

    def Hide(self, item):
        """Hide item (sizer or window).  To make a sizer item
        disappear on-screen, use Hide() followed by Layout()."""
        pass

    def Insert(self, before, item, proportion=0, flag=0, border=0,
               userData=wx.NULL):
        """Same as Add, but inserts item into list of items (windows,
        subsizers or spacers) owned by this sizer.

        Call Layout() to update the layout on-screen after inserting."""
        pass

    def IsShown(self, item):
        """Return True if item (sizer or window) is shown."""
        pass

    def Layout(self):
        """Force layout of children anew.

        Use after adding or removing a child (window, other sizer, or
        spacer) from the sizer while keeping the current dimension."""
        pass

    def Prepend(self, item, proportion=0, flag=0, border=0,
                userData=wx.NULL):
        """Same as Add, but prepends item to beginning of list of
        items (windows, subsizers or spacers) owned by this sizer.

        Call Layout() to update the layout on-screen after prepending."""
        pass

    def Remove(self, item):
        """Remove item from the sizer.

        item - sizer, window, or index of item in the sizer, typically
        0 for the first item.

        Does not cause any layout or resizing to take place, and does
        not delete the child itself.  Call Layout() to update the
        layout on-screen after removing child.

        Return True if child found and removed, False otherwise."""
        pass

    def SetDimension(self, x, y, width, height):
        """Force sizer to take the given dimension and thus force
        items owned by sizer to resize themselves according to the
        rules defined by the parameter in the Add and Prepend methods."""
        pass

    def SetItemMinSize(self, item, width, height):
        """Set minimal size of item.

        item - sizer, window, or index of item in the sizer, typically
        0 for the first item.

        The item will be found recursively in the sizer's descendants.
        Enables application to set size of item after initialization."""
        pass

    def SetMinSize(self, size):
        """Set minimal size.

        Normally, sizer will calculate minimal size based on how much
        space its children need.  After calling this method,
        GetMinSize will return the minimal size as requested by its
        children or the minimal size set here, whichever is larger."""
        pass

    def SetSizeHints(self, window):
        """Set (and Fit) minimal size of window to match sizer's
        minimal size.  Commonly called in the window's init."""
        pass

    def SetVirtualSizeHints(self, window):
        """Set minimal size of window virtual area to match sizer's
        minimal size.  For windows with managed scrollbars this will
        set them appropriately."""
        pass

    def Show(self, item, show=True):
        """Show or hide item (sizer or window).  To make item
        disappear or reappear on-screen, use Show() followed by
        Layout()."""
        pass

    def ShowItems(self, show):
        """Recursively call Show() on all sizer items."""
        pass


class PySizer(Sizer):
    """If you wish to create a custom sizer class you should derive
    the class from PySizer in order to get Python-aware capabilities
    for the various virtual methods."""

    def __init__(self):
        """Create a PySizer instance.  Override in subclass."""
        pass


class BoxSizer(Sizer):
    """A box sizer is used to lay out a rather simple geometry,
    typically a row or column or several hierarchies of either."""

    def __init__(self, orient=wx.HORIZONTAL):
        """Create BoxSizer instance.

        orient is either wx.VERTICAL or wx.HORIZONTAL"""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def GetOrientation(self):
        """Return orientation: wx.VERTICAL or wx.HORIZONTAL."""
        pass

    def RecalcSizes(self):
        """Recalculate sizes, then set the size of its children
        (calling SetSize if child is a window).  Do not call directly."""
        pass

    def SetOrientation(self, orient):
        """Set orientation to either wx.VERTICAL or wx.HORIZONTAL."""
        pass


class StaticBoxSizer(BoxSizer):
    """Like BoxSizer, but adds a static box around the sizer.  Note
    that the static box has to be created separately."""

    def __init__(self, box, orient=wx.HORIZONTAL):
        """Create StaticBoxSizer instance.

        box - instance of wx.StaticBox

        orient - either wx.VERTICAL or wx.HORIZONTAL"""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def GetStaticBox(self):
        """Return the static box associated with the sizer."""
        pass

    def RecalcSizes(self):
        """Recalculate sizes, then set the size of its children
        (calling SetSize if child is a window).  Do not call directly."""
        pass


class GridSizer(Sizer):
    """A grid sizer lays out its children in a two-dimensional table
    where all cells have the same size: the width of each cell is the
    width of the widest child, the height of each cell is the height
    of the tallest child.  See also the FlexGridSizer."""

    def __init__(self, rows=1, cols=0, vgap=0, hgap=0):
        """Create a GridSizer instance.

        rows and cols - the number of rows and columns in the grid; if
        either is zero, it will be calculated as the number of
        children in the sizer, allowing the sizer grow dynamically.

        vgap and hgap - extra space between all cells, in pixels."""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def GetCols(self):
        """Return the number of columns in the grid."""
        pass

    def GetHGap(self):
        """Return the horizontal gap (in pixels) between cells."""
        pass

    def GetRows(self):
        """Return the number of rows in the grid."""
        pass

    def GetVGap(self):
        """Return the vertical gap (in pixels) between cells."""
        pass

    def RecalcSizes(self):
        """Recalculate sizes, then set the size of its children
        (calling SetSize if child is a window).  Do not call directly."""
        pass

    def SetCols(self, cols):
        """Set the number of columns in the grid."""
        pass

    def SetHGap(self, gap):
        """Set the horizontal gap (in pixels) between cells."""
        pass

    def SetRows(self, rows):
        """Sets the number of rows in the grid."""
        pass

    def SetVGap(self, gap):
        """Set the vertical gap (in pixels) between cells."""
        pass


class FlexGridSizer(GridSizer):
    """A flex grid sizer lays out its children in a two-dimensional
    table where all cells in one row have the same height and all
    cells in one column have the same width, but all cells are not
    necessarily the same height and width, as in the GridSizer."""

    def __init__(self, rows=1, cols=0, vgap=0, hgap=0):
        """Create a GridSizer instance.

        rows and cols - the number of rows and columns in the grid; if
        either is zero, it will be calculated as the number of
        children in the sizer, allowing the sizer grow dynamically.

        vgap and hgap - extra space between all cells, in pixels."""
        pass

    def AddGrowableCol(self, idx):
        """Specify that column idx (starting from zero) should expand
        if there is extra space available to the sizer."""
        pass

    def AddGrowableRow(self, idx):
        """Specify that row idx (starting from zero) should expand if
        there is extra space available to the sizer."""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def RecalcSizes(self):
        """Recalculate sizes, then set the size of its children
        (calling SetSize if child is a window).  Do not call directly."""
        pass

    def RemoveGrowableCol(self, idx):
        """Specify that column idx is no longer growable."""
        pass

    def RemoveGrowableRow(self, idx):
        """Specify that row idx is no longer growable."""
        pass


class NotebookSizer(Sizer):
    """NotebookSizer works with a notebook to determine the size of
    the biggest page and report an adjusted minimal size to a more
    toplevel sizer.  Do not add children to a NotebookSizer."""

    def __init__(self, nb):
        """Create a NotebookSizer instance for notebook."""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def GetNotebook(self):
        """Return the notebook associated with the sizer."""
        pass

    def RecalcSizes(self):
        """Recalculate size.  Do not call directly."""
        pass


class SizerItem(Object):
    """SizerItem class.  Wrapper for items managed by a sizer."""

    def __init__(self, this):
        """Create a SizerItem instance.  You don't normally create one
        directly."""
        pass

    def CalcMin(self):
        """Calculate minimum size.  Do not call directly."""
        pass

    def DeleteWindows(self):
        """Recursively destroy windows associated with this SizerItem."""
        pass

    def GetBorder(self):
        """Return border width."""
        pass

    def GetFlag(self):
        """Return flag value."""
        pass

    def GetOption(self):
        """Return option value."""
        pass

    def GetPosition(self):
        """Return wx.Point instance representing position relative to
        the client area."""
        pass

    def GetRatio(self):
        """Return a floating point aspect ratio (width/height).  If
        wx.SHAPED flag is used item will maintain ratio when resized."""
        pass

    def GetSize(self):
        """Return wx.Size instance with size."""
        pass

    def GetSizer(self):
        """If IsSizer() return the sizer; otherwise return None."""
        pass

    def GetUserData(self):
        """Return a wx.PyUserData object."""
        pass

    def GetWindow(self):
        """If IsWindow() return the window; otherwise return None."""
        pass

    def IsShown(self):
        """Return True if item is shown."""
        pass

    def IsSizer(self):
        """Return True if SizerItem represents a sizer."""
        pass

    def IsSpacer(self):
        """Return True if SizerItem represents a spacer."""
        pass

    def IsWindow(self):
        """Return True if SizerItem represents a window."""
        pass

    def SetBorder(self, border):
        """Set border width for item."""
        pass

    def SetDimension(self, pos, size):
        """Set position and size for item."""
        pass

    def SetFlag(self, flag):
        """Set flag for item."""
        pass

    def SetInitSize(self, x, y):
        """Set initial size of item."""
        pass

    def SetOption(self, option):
        """Set option for item."""
        pass

    def SetRatio(self, ratio):
        """Set a floating point aspect ratio (width/height).  If
        wx.SHAPED flag is used item will maintain ratio when resized."""
        pass

    def SetRatioSize(self, size):
        """Set a floating point aspect ratio (width/height).  If
        wx.SHAPED flag is used item will maintain ratio when resized."""
        pass

    def SetRatioWH(self, width, height):
        """Set a floating point aspect ratio (width/height).  If
        wx.SHAPED flag is used item will maintain ratio when resized."""
        pass

    def SetSizer(self, sizer):
        """Set sizer associated with SizerItem."""
        pass

    def SetWindow(self, window):
        """Set window associated with SizerItem."""
        pass

    def Show(self, show):
        """Is show is True, show item, otherwise hide item."""
        pass
