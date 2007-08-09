wxPython-docs
-------------

The wxWidgets docs can now be viewed on non-Win32 platforms with a
nice viewer modeled after the MS HTMLHelp viewer.  Simply execute the
viewdocs.py script located in this directory and you're all set.
(You'll need to first have wxPython installed and accessible via the
PYTHONPATH.)

You can also add other HTML books to be displayed by the viewer simply
by dropping their .zip file here, viewdocs.py will find them
automatically and add them to the list.  The zip file just needs to
contain a .hhp file that defines the contents of the book.  See the
docs on wxHtmlHelpController for details.

If you'd still like to view these docs in your regular browser you can
simply unzip each of the books into their own directory.