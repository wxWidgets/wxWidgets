Summary: The GTK+ 1.0 port of wxWindows library
Name: wxGTK
Version: 2.0 beta 2
Release: 1
Copyright: LGPL
Group: X11/Libraries
Source0: ftp://wesley.informatik.uni-freiburg.de/pub/linux/wxxt/source
URL: http://www.freiburg.linux.de/~wxxt/docs.html
Packager: Michael Kiefte <mkiefte@gpu.srv.ualberta.ca>
Requires: gtk+ >= 1.0.4

%description
wxWindows is a free C++ library for cross-platform GUI development. With wxWindows, you can create applications for different GUIs from the same source code. This package also contains a dialog editor.

%prep
%setup -n wxGTK
./configure --prefix=/usr --with-threads

%build
make src

%install
make install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%doc docs/gtk/COPYING.LIB docs/gtk/install.txt docs/gtk/licence.txt docs/gtk/readme.txt docs/symbols.txt docs/gtk/todo.txt
/usr/include/wx
/usr/lib/wx
/usr/lib/libwx_gtk2.a
/usr/lib/libwx_gtk2.so
/usr/lib/libwx_gtk2.so.0
/usr/lib/libwx_gtk2.so.0.1
/usr/bin/wx-config
