Animation sample
================

Every now and then someone asks whether there are animation
classes in wxWindows. I started these animation player classes
nearly two years ago and never got round to finishing them.
Now I've done some hacking on them and (after very limited testing)
it seems to work on Windows for animated GIFs, both transparent
and non-transparent.

Basically the classes makes use of the existing GIF decoder in
wxWindows to read an animated GIF into wxGIFAnimation, and then
play that animation using wxAnimationPlayer. It's quite tied
to the animated GIF way of doing animation, so don't expect anything too generic.
However, it would be quite possible to write code to convert an animated
GIF into a PNG-based invented format, and then write a wxPNGAnimation
handler.

The next steps are:

1. Test on other platforms.
2. Write control classes to make it easy to embed animations in dialogs, etc.
   See my thoughts in animate.h.
3. Write documentation.

*** IMPORTANT NOTE: to compile this, you must first edit the
file:

include/wx/gifdecod.h

and change the keyword 'protected' to 'public', then recompile
wxWindows. If you have downloaded the latest code from the CVS trunk,
the problem has been corrected already.

As an exercise, you might like to write a handler for the
PNG-based animation format:

http://www.libpng.org/mng/
http://www.libmng.com/

Julian Smart, 5th July 2001
