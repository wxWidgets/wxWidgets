# This will build an Application Bundle of the superdoodle sample
# application for OSX.  Run like this:
#
#       pythonw buildapp.py build
#

import os, glob
from bundlebuilder import buildapp


# See below.  Set to wherever your wxMac dynlibs are installed.
# (Probably /usr/local/lib/wxPython-2.5.2.?/lib) If you don't use all
# the contribs libs then you can use a more restrictive list.
wxLibs = "/opt/wx/2.5/lib/libwx*2.5.2.[dr]*"


buildapp(
    name = "SuperDoodle",
    mainprogram = "superdoodle.py",

    # This tells bundlebuilder to include Python and everything else
    # that it needs to run the app
    standalone = 1,

    # Bunndlebuilder isn't yet able to find the shared libs that may be
    # needed by extension modules, so we have to help it out.
    libs = glob.glob(wxLibs),

    # Some modules will not be found automatically, so we need to help
    includeModules = ["cStringIO"],
    
    verbosity = 1
    )

