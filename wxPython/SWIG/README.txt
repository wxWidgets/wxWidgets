SWIG 1.3.x Patches
==================

This directory holds a set of patches for the CVS version of SWIG that
are required if you wish to use SWIG for wxPython development, or for
building your own extension modules that need to interface with
wxPython.  These have been submitted to SWIG's SourceForge patch
tracker, so hopefully they will get incorporated into the main SWIG
source tree soon.

wxPython currently uses the 1.3.29 version of SWIG, which you can get
from https://sourceforge.net/projects/swig/, plus the patch(es) in
this directory.  Download the SWIG sources, apply the patch(es) here
and then build as normal.  I have also have made available a tarball
containing the patched SWIG sources, plus a win32 binary, located
here:

	http://wxpython.wxcommunity.com/tools/

If you want to use the patched version of SWIG and still have the
stock version available for other projects, then you can configure the
patched version to use a different --prefix and then specify that
executable when running setup.py, like this:

	python setup.py SWIG=/path/to/my/swig [other params]



------------------------------------------------------------------------

swig-1.3.29.patch

    SWIG changed how the import statements are output to the proxy
    file, but this also caused the order to change (they all moved to
    the very top of the file) so this broke the module docstring, as
    well as some behavior that Chandler development is depending upon,
    so this patch changes back to how it was done in prior releases.

    Changed the exception message used for TypeErrors to make a little
    more sense.

    Added support for dropping the leading wx from wxNames using
    %rename("%(wxpy)s") ""; 

    Don't set a module attribute for the *_swigregister functions,
    just call the one in the extension module directly.

    Fixes to allow compiling with Python 2.5 headers.


------------------------------------------------------------------------

swig-1.3.27.patch

    SWIG changed slightly how the runtime type_info data is structured
    in order to optimize load time and runtime access.  wxPython
    uncovered a bug in the implementation, so this patch includes the
    fix that was checked in to CVS for 1.3.28.

    SWIG changed how the import statements are output to the proxy
    file, but this also caused the order to change (they all moved to
    the very top of the file) so this broke the module docstring, as
    well as some behavior that Chandler development is depending upon,
    so this patch changes back to how it was done in prior releases.

    Bug fix for SWIG's definition of the %makedefault macro.

    Fixes to allow compiling with Python 2.5 headers.
    

------------------------------------------------------------------------

swig-1.3.24.patch (now obsolete)

    A bug was introduced in SWIG 1.3.23 and remains in 1.3.24 that
    causes compilation problems with wxPython (copies are being made
    of objects that don't have a copy constructor.)  This patch fixes
    the code generator to use a reference to the object instead of
    making a copy.

    Part of my autodoc patch was disabled because a unit-test failed.
    It turns out that the failure was due to a name clash in the unit
    test itself, so I re-enabled that section of code in this patch.

    Don't generate the autodocs string for a class if it has a
    docstring attribute.

    Some typos fixed, etc.




------------------------------------------------------------------------
This patch was added to SWIG's CVS on 10/2/2004 and a modified version
of it is in 1.3.23 and 1.3.24.
------------------------------------------------------------------------


swig.python-2.patch  

    Adds the following features to the Python Module in SWIG.  See the
    updated docs in the patch for more details.

            %feature("autodoc")
            %feature("docstring")
            %feature("pythonprepend")
            %feature("pythonappend")

            %module(docstring="string")
            %module(package="string")

    https://sourceforge.net/tracker/index.php?func=detail&aid=1023309&group_id=1645&atid=301645


------------------------------------------------------------------------
This patch was applied to SWIG's CVS on 07/12/2004 and is in the
1.3.22 release.
------------------------------------------------------------------------

swig.xmlout.patch            Fixes a couple problems in the XML output
                             of SWIG: an extra "/>" was removed and
                             newlines in attribute values were changed
                             to the #10; entity reference so they will
                             be preserved by parsers.

                             Also, added options for dumping or
                             writing to a file the XML of the parse
                             tree *after* other language modules have
                             been run (previously you could only do
                             the XML output *instead of* a regular
                             language module.)
                             See SF Patch #864689



------------------------------------------------------------------------
These patches have already been checked in to SWIG's CVS and are in
the 1.3.20 release.
------------------------------------------------------------------------

swig.SplitLines.patch       Adds a new SplitLines function to the DOH
                            library.  See SF Patch #829317.
                            *Checked in 10/31/2003*

swig.xml.patch              Adds an option that drastically reduces
                            the size of the XML output of SWIG, which
                            increases the performance of the
                            build_renamers script used in the wxPython
                            build.  See SF Patch #829319.
                            *Checked in 10/31/2003*

swig.python.patch           Lots of changes for SWIG's Python module,
                            especially in how the proxy code is
                            generated.  See swig.python.patch.txt for
                            more details, also SF Patch #829325.
                            *Checked in 10/31/2003*

------------------------------------------------------------------------
