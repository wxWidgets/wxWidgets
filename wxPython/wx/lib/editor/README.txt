wxEditor component
------------------

The wxEditor class implements a simple text editor using wxPython.  You
can create a custom editor by subclassing wxEditor.  Even though much of
the editor is implemented in Python, it runs surprisingly smoothly on  
normal hardware with small files.


Keys
----
Keys are similar to Windows-based editors:

Tab:                 1 to 4 spaces (to next tab stop)
Cursor movement:     Arrow keys
Beginning of line:   Home
End of line:         End
Beginning of buffer: Control-Home
End of the buffer:   Control-End 
Select text:         Hold down Shift while moving the cursor
Copy:                Shift-Insert,   Control-C
Cut:                 Shift-Delete,   Control-X
Paste:               Control-Insert, Control-V

How to use it
-------------
The demo code (demo/wxEditor.py) shows how to use it as a simple text
box. Use the SetText() and GetText() methods to set or get text from
the component; these both return a list of strings.

The samples/FrogEdit directory has an example of a simple text editor
application that uses the wxEditor component.

Subclassing
-----------
To add or change functionality, you can subclass this
component. One example of this might be to change the key
Alt key commands. In that case you would (for example) override the
SetAltFuncs() method.

History
-------
The original author of this component was Dirk Holtwic. It originally
had limited support for syntax highlighting, but was not a usable text
editor, as it didn't implement select (with keys or mouse), or any of
the usual key sequences you'd expect in an editor. Robin Dunn did some
refactoring work to make it more usable. Steve Howell and Adam Feuer
did a lot of refactoring, and added some functionality, including
keyboard and mouse select, properly working scrollbars, and
overridable keys. Adam and Steve also removed support for
syntax-highlighting while refactoring the code.

To do
-----
Alt/Ctrl Arrow keys move by word
Descriptive help text for keys
Speed improvements
Different fonts/colors


Authors
-------
Steve Howell, Adam Feuer, Dirk Holtwic, Robin Dunn


Contact
-------
You can find the latest code for wxEditor here:
http://www.pobox.com/~adamf/software/

We're not actively maintaining this code, but we can answer
questions about it. You can email us at:

Adam Feuer <adamf at pobox dot com>
Steve Howell <showell at zipcon dot net>

29 November 2001
