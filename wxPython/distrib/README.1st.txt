README for wxPythonSrc-*.tar.gz
-------------------------------

Prior to version 2.3.3 of wxPython I had always made my Linux/Unix
binaries based on the released binary of wxGTK and wxGTK-gl.  This
imposed a few restrictions and so starting with 2.3.3 I have decided
to do a combined binary that inlcudes wxGTK as well as wxPython.  This
allows me a bit more flexibility and is consistent with how the
Windows and Mac OS X binaries are built.

If you are reading this file then you are probably interested in
building your own copy of wxPython from the sources contained in this
archive.  If you wish to use the released wxGTK binary as has been
done in the past then you can still follow the old build directions in
wxPython/BUILD.unix.txt.  If you are building for Windows or Mac OS X
then you should look at wxPython/BUILD.win32.txt or
wxPython/BUILD.osx.txt respectivly.  In all these cases you should use
the IN_CVS_TREE=1 flag since this archive is really just a modified
CVS snapshot.

If, on the other hand, you would like to build Linux/Unix binaries
with a private copy of wxGTK like what I am now distributing then
you'll want to follow the instructions in this file.

Clear as mud?  Good.  Let's get started.


1. We'll be making a private copy of wxGTK so it doesn't conflict with
   one used by wxGTK C++ apps that expect to have the default binary
   installed from RPM or whatever.  I put it in /usr/lib/wxPython, but
   you can use whatever you like.  I'll just set a variable to our wx
   prefix to reference later:

        export WXPREF=/usr/lib/wxPython


2. Make a build directory and configure wxGTK.

        cd wxPythonGTK-2.3.3   # or whatever the top-level dir is
        mkdir build
        cd build
        ../configure --with-gtk \
                     --prefix=$WXPREF \
                     --enable-rpath=$WXPREF/lib \
                     --with-opengl \
		     --enable-geometry \
                     --enable-optimise \
                     --enable-debug_flag \

   You may want to use --enable-debug instead of --enable-optimise if
   you need to run though a debugger and want full debugging symbols.

   if you want to use the image and zlib libraries included with
   wxWindows instead of those already installed on your system, (for
   example, to reduce dependencies on 3rd party libraries) then you
   can add these flags to the configure command:

                     --with-libjpeg=builtin \
                     --with-libpng=builtin \
                     --with-libtiff=builtin \
                     --with-zlib=builtin


3. Build and install wxGTK.  (You may need to be root for the last
   step, depending on where your WXPREF is.)

        make
        cd ../locale
        make allmo
        cd ../build
        make install

4. Build and install wxPython.  If you want to use a different version
   of Python than is found by default on the PATH then specify the
   whole pathname in these steps.  The version of Python that runs
   setup.py is the version wxPython will be built and installed for.
   (You will need to be root for the install step unless your Python
   is not in a system location.)

        cd ../wxPython
        python setup.py \
               IN_CVS_TREE=1 WX_CONFIG=$WXPREF/bin/wx-config \
               build install

5. That's all!

--
Robin Dunn
Software Craftsman
http://wxPython.org  Java give you jitters?  Relax with wxPython!

