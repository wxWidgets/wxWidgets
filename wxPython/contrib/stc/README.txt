Since STC is not always bundled with distributions of wxWindows, in
order for it to be a standard part of wxPython I need to bundle it
here.  The contents of the contrib directory are copies of the
relevant parts of the main contrib directory in wxWindows.  The
build.py script in this directory will also build the needed files
from there, so you no longer have to worry about aquiring and building
additional libraries beyond wxWindows itself.
