#!/usr/bin/env python

"""
    Run wxPython in a second thread.

    Overview:
        Importing this module creates a second thread and starts
        wxPython in that thread.  Its single method,
        add_cone(), sends an event to the second thread
        telling it to create a VTK viewer window with a cone in
        it.

        This module is meant to be imported into the standard
        Python interpreter.  It also works with Pythonwin.
        It doesn't seem to work with IDLE (on NT anyways).
        It should also work in a wxPython application.

        Applications already running a wxPython app do not
        need to start a second thread.  In these cases,
        viewer creates the cone windows in the current
        thread.  You can test this by running shell.py
        that comes with wxPython, importing viewer and
        calling add_cone.

    Usage:
        [user]$ python
        Python 1.5.2 (#1, Sep 17 1999, 20:15:36)  ...
        Copyright 1991-1995 Stichting Mathematisch Centrum, Amsterdam
        >>> import viewer
        >>> viewer.add_cone() # pop up a cone window
        >>> a = 1
        1
        >>> viewer.add_cone() # create another cone window

    Why would anyone do this?:
        When using wxPython, the call to app.Mainloop() takes over
        the thread from which it is called.  This presents a
        problem for applications that want to use the standard
        Python command line user interface, while occasionally
        creating a GUI window for viewing an image, plot, etc.
        One solution is to manage the GUI in a second thread.

        wxPython does not behave well if windows are created in
        a thread other than the one where wxPython was originally
        imported. ( I assume importing wxPython initializes some
        info in the thread).  The current solution is to make the
        original import of wxPython in the second thread and then
        create all windows in that second thread.

        Methods in the main thread can create a new window by issuing
        events to a "catcher" window in the second thread.  This
        catcher window has event handlers that actually create the
        new window.
"""

class viewer_thread:
    def start(self):
        """ start the GUI thread
        """
        import  time
        import  thread
        thread.start_new_thread(self.run, ())

    def run(self):
        """
            Note that viewer_basices is first imported ***here***.
            This is the second thread.  viewer_basics imports
            wxPython.  if we imported it at
            the module level instead of in this function,
            the import would occur in the main thread and
            wxPython wouldn't run correctly in the second thread.
        """
        import  viewer_basics

        try:
            self.app = viewer_basics.SecondThreadApp(0)
            self.app.MainLoop()
        except TypeError:
            self.app = None

    def add_cone(self):
        """
            send an event to the catcher window in the
            other thread and tell it to create a cone window.
        """
        import  viewer_basics

        if self.app:
            evt = viewer_basics.AddCone()
            viewer_basics.wxPostEvent(self.app.catcher, evt)
        else:
            viewer_basics.add_cone()

viewer = viewer_thread()
viewer.start()

def add_cone():
    viewer.add_cone()


