SWIG 1.3 Patches
================

This directory holds a set of patches for the CVS version of SWIG that
are required if you wish to use SWIG for wxPython development, or for
building your own extension modules that need to interface with
wxPython.  These have been submitted to SWIG's SourceForge patch
tracker, so hopefully they will get incorporated into the main SWIG
source tree soon.


------------------------------------------------------------------------


swig.python-docstring.patch  Adds "autodoc" and "docstring" features.
			     See SF Patch #883402

			     Also changes the "addtofunc" feature to
			     "pythonappend" and also adds a
			     "pythonprepend" feature that prepends
			     pythoncode to the begining of a
			     SWIG-generated proxy function or method.

swig.xmlout.patch	     Fixes a couple problems in the XML output
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

swig.SplitLines.patch	    Adds a new SplitLines function to the DOH
			    library.  See SF Patch #829317.
			    *Checked in 10/31/2003*

swig.xml.patch		    Adds an option that drastically reduces
			    the size of the XML output of SWIG, which
			    increases the performance of the
			    build_renamers script used in the wxPython
			    build.  See SF Patch #829319.
			    *Checked in 10/31/2003*

swig.python.patch	    Lots of changes for SWIG's Python module,
			    especially in how the proxy code is
			    generated.  See swig.python.patch.txt for
			    more details, also SF Patch #829325.
			    *Checked in 10/31/2003*

------------------------------------------------------------------------
