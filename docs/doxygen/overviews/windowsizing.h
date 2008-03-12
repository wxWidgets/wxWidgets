/////////////////////////////////////////////////////////////////////////////
// Name:        windowsizing.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_windowsizing Window Sizes

It can sometimes be confusing to keep track of the various size-related
attributes of a wxWindow, how they relate to each other, and how they interact
with sizers. This document will attempt to clear the fog a little, and give
some simple explanations of things.

@b BestSize: The best size of a widget depends on what kind of widget it is,
and usually also on the contents of the widget. For example a wxListBox's best
size will be calculated based on how many items it has, up to a certain limit,
or a wxButton's best size will be calculated based on its label size, but
normally won't be smaller than the platform default button size (unless a style
flag overrides that). Get the picture? There is a special virtual method in the
C++ window classes called @c DoGetBestSize() that a class needs to override if
it wants to calculate its own best size based on its content. The default
@c DoGetBestSize() is designed for use in container windows, such as wxPanel,
and works something like this:

-# If the window has a sizer then it is used to calculate the best size.
-# Otherwise if the window has layout constraints then that is used to
   calculate the best size.
-# Otherwise if the window has children then the best size is set to be large
   enough to show all the children.
-# Otherwise if there are no children then the window's min size will be used
   for the best size.
-# Otherwise if there is no min size set, then the current size is used for the
   best size.

@b MinSize: The min size of a widget is a size that is normally explicitly set
by the programmer either with the @c SetMinSize() method or the
@c SetSizeHints() method. Most controls will also set the min size to the size
given in the control's constructor if a non-default value is passed. Top-level
windows such as wxFrame will not allow the user to resize the frame below the
min size.

@b Size: The size of a widget can be explicitly set or fetched with the
@c SetSize() or @c GetSize() methods. This size value is the size that the
widget is currently using on screen and is the way to change the size of
something that is not being managed by a sizer.

@b ClientSize: The client size represents the widget's area inside of any
borders belonging to the widget and is the area that can be drawn upon in a
@c EVT_PAINT event. If a widget doesn't have a border then its client size is
the same as its size.

@b InitialSize: The initial size of a widget is the size given to the
constructor of the widget, if any.  As mentioned above most controls will also
set this size value as the control's min size. If the size passed to the
constructor is the default @c wxDefaultSize, or if the size is not fully
specified (such as wxSize(150,-1)) then most controls will fill in the missing
size components using the best size and will set the initial size of the
control to the resulting size.

@b GetEffectiveMinSize(): (formerly @c GetBestFittingSize) A blending of the
widget's min size and best size, giving precedence to the min size. For
example, if a widget's min size is set to (150, -1) and the best size is
(80, 22) then the best fitting size is (150, 22). If the min size is (50, 20)
then the best fitting size is (50, 20). This method is what is called by the
sizers when determining what the requirements of each item in the sizer is, and
is used for calculating the overall minimum needs of the sizer.

@b SetInitialSize(size): (formerly @c SetBestFittingSize) This is a little
different than the typical size setters. Rather than just setting an
"initial size" attribute it actually sets the minsize to the value passed in,
blends that value with the best size, and then sets the size of the widget to
be the result. So you can consider this method to be a "Smart SetSize". This
method is what is called by the constructor of most controls to set the minsize
and initial size of the control.

@b window.Fit(): The @c Fit() method sets the size of a window to fit around
its children. If it has no children then nothing is done, if it does have
children then the size of the window is set to the window's best size.

@b sizer.Fit(window): This sets the size of the window to be large enough to
accommodate the minimum size needed by the sizer, (along with a few other
constraints...) If the sizer is the one that is assigned to the window then
this should be equivalent to @c window.Fit().

@b sizer.Layout(): Recalculates the minimum space needed by each item in the
sizer, and then lays out the items within the space currently allotted to the
sizer.

@b window.Layout(): If the window has a sizer then it sets the space given to
the sizer to the current size of the window, which results in a call to
@c sizer.Layout(). If the window has layout constraints instead of a sizer then
the constraints algorithm is run. The @c Layout() method is what is called by
the default @c EVT_SIZE handler for container windows.

*/

