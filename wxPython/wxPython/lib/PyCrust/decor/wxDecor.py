"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


_topics = {
    'wxAccelerators': None,
    'wxApp': None,
    'wxBase': None,
    'wxClipDragDrop': None,
    'wxConfig': None,
    'wxControls': None,
    'wxDataStructures': None,
    'wxDateTime': None,
    'wxDialogs': None,
    'wxDrawing': None,
    'wxErrors': None,
    'wxEventFunctions': None,
    'wxEvents': None,
    'wxFileSystem': None,
    'wxFrames': None,
    'wxFunctions': None,
    'wxHelp': None,
    'wxImageHandlers': None,
    'wxJoystick': None,
    'wxLayoutConstraints': None,
    'wxLogging': None,
    'wxMenus': None,
    'wxMimeTypes': None,
    'wxMisc': None,
    'wxPanel': None,
    'wxPrinting': None,
    'wxProcess': None,
    'wxSashSplitter': None,
    'wxSizers': None,
    'wxStreams': None,
    'wxThreading': None,
    'wxToolBar': None,
    'wxTree': None,
    'wxValidators': None,
    'wxWindow': None,
    }

for topic in _topics:
    _topics[topic] = __import__(topic, globals())
    exec 'from %s import *' % topic

del topic  # Cleanup the namespace.
