Summary: The GTK+ 1.0 port of wxWindows library
Name: wxGTK
Version: 2.0.1
Release: 1
Copyright: wxWindows Licence
Group: X11/Libraries
Source: ftp://wesley.informatik.uni-freiburg.de/pub/linux/wxxt/source/wxGTK201.tgz
URL: http://www.freiburg.linux.de/~wxxt/docs.html
Packager: Robert Roebling <roebling@ruf.uni-freiburg.de>
Requires: gtk+ >= 1.0.4

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.
This package also contains a dialog editor.

%prep
%setup -n wxGTK
./configure --prefix=/usr --with-threads

%build
make

%install
make install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%doc COPYING.LIB INSTALL.txt LICENCE.txt README.txt SYMBOLS.txt TODO.txt
/usr/include/wx
/usr/lib/wx
/usr/lib/libwx_gtk2.so
/usr/lib/libwx_gtk2.so.0
/usr/lib/libwx_gtk2.so.0.1
/usr/bin/wx-config

