%define pref /usr
%define ver 2.2.9
%define ver2 2.2
%define rel 0

Summary: The GTK+ 1.2 port of the wxWindows library
Name: wxGTK
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: wxGTK-%{ver}.tar.bz2
URL: http://www.wxwindows.org/
Prefix: %{pref}
BuildRoot: /var/tmp/%{name}-root
Packager: Vadim Zeitlin <vadim@wxwindows.org>

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

# in addition, we should provide libwx_gtk as automatic generator only notices
# libwx_gtk-%{ver}-%{rel}
Provides: libwx_gtk.so
Provides: libwx_gtk-%{ver2}.so

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%package devel
Summary: The GTK+ 1.2 port of the wxWindows library
Group: X11/Libraries
Requires: wxGTK

%description devel
Header files for wxGTK, the GTK+ 1.2 port of the wxWindows library.

%package gl
Summary: The GTK+ 1.2 port of the wxWindows library, OpenGl add-on.
Group: X11/Libraries
Requires: wxGTK

%description gl
OpenGl add-on library for wxGTK, the GTK+ 1.2 port of the wxWindows library.

%prep
%setup -n wxGTK-%{ver}
export CPPFLAGS=-I/usr/X11R6/include
export LDFLAGS=-L/usr/X11R6/lib
./configure --prefix=%{pref} --no-recursion --enable-burnt_name --with-odbc --with-opengl

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
make prefix=$RPM_BUILD_ROOT%{pref} install

%find_lang wxstd

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files -f wxstd.lang
%defattr(-, root, root)
%doc COPYING.LIB INSTALL.txt LICENCE.txt README.txt SYMBOLS.txt TODO.txt
%dir %{pref}/share/wx
%{pref}/share/wx/*
%{pref}/lib/libwx_gtk*

%files devel
%defattr(-, root, root)
%dir %{pref}/include/wx
%{pref}/include/wx/*
%dir %{pref}/lib/wx
%{pref}/lib/wx/*
%{pref}/bin/wx*-config

%files gl
%defattr(-, root, root)
%{pref}/lib/libwx_gtk_gl*
