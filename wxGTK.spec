Summary: The GTK+ 1.0 port of wxWindows library
Name: wxGTK
Version: 2b1
Release: 1
Copyright: LGPL
Group: X11/Libraries
Source: ftp://wesley.informatik.uni-freiburg.de/pub/linux/wxxt/source/wxGTK2b1.tgz
URL: http://www.freiburg.linux.de/~wxxt/docs.html
Packager: Michael Kiefte <mkiefte@gpu.srv.ualberta.ca>
Requires: gtk+ >= 1.0.4

%description
wxWindows is a free C++ library for cross-platform GUI development. With wxWindows, you can create applications for different GUIs from the same source code.

%prep
%setup -n wxGTK
./configure --with-threads

%build
make src

%install
make install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%doc COPYING.LIB INSTALL.txt LICENCE.txt README.txt SYMBOLS.txt TODO.txt
/usr/local/include
/usr/local/lib
