SWIG 1.3 Patches
================

This directory holds a set of patches for the CVS version of SWIG that
are required if you wish to use SWIG for wxPython development, or for
building your own extension modules that need to interface with
wxPython.  These have been submitted to SWIG's SourceForge patch
tracker, so hopefully they will get incorporated into the main SWIG
source tree soon.

These patches are known to apply cleanly to SWIG 1.3.20 (when
released) and also to the SWIG CVS as of 29-Oct-2003.



swig.SplitLines.patch	    Adds a new SplitLines function to the DOH
			    library.  See SF Patch #829317.

swig.xml.patch		    Adds an option that drastically reduces
			    the size of the XML output of SWIG, which
			    increases the performance of the
			    build_renamers script used in the wxPython
			    build.  See SF Patch #829319.

swig.python.patch	    Lots of changes for SWIG's Python module,
			    especially in how the proxy code is
			    generated.  See swig.python.patch.txt for
			    more details, also SF Patch #829325.

