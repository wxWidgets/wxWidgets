# Note that this is NOT a relocatable package
%define pref /usr
%define ver 2.2.2
%define rel 0

Summary: The GTK+ 1.2 port of the wxWindows library
Name: wxGTK
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: wxGTK-%{ver}.tar.gz
URL: http://wxwindows.org
Packager: Robert Roebling <robert@roebling.de>

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

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
%setup -n wxGTK
./configure --prefix=%{pref} --enable-burnt_name --with-odbc --with-opengl

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
make install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr (644, root, root, 755)
%doc COPYING.LIB INSTALL.txt LICENCE.txt README.txt SYMBOLS.txt TODO.txt
%dir %{pref}/share/wx
%{pref}/share/wx/*
%attr(755, -, -) %{pref}/lib/libwx_gtk.*
%attr(755, -, -) %{pref}/lib/libwx_gtk-2.2.*

%files devel
%defattr (644, root, root, 755)
%dir %{pref}/include/wx
%{pref}/include/wx/*
%dir %{pref}/lib/wx
%{pref}/lib/wx/*
%attr(755, -, -) %{pref}/bin/wxgtk-config
%attr(755, -, -) %{pref}/bin/wx-config

%files gl
%attr(755, -, -) %{pref}/lib/libwx_gtk_gl*
