# RPM .spec file for wxMotif. Based on the .spec file for wxGTK
# (c) by Robert Roebling. Modifications (c) 2000/03 by Robert Fendt.

# Note that this is NOT a relocatable package
%define pref /usr
%define ver 2.2.5
%define rel 0

Summary: The Motif/Lesstif port of the wxWindows library
Name: wxMotif
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: wxMotif-%{ver}.tgz
URL: http://www.wxwindows.org
Packager: Robert Fendt <rfendt@myokay.net>
BuildRoot: /tmp/wxmotif_root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%package devel
Summary: The Motif/Lesstif port of the wxWindows library
Group: X11/Libraries
Requires: wxMotif

%description devel
Header files for wxMotif, the Motif/Lesstif port of the wxWindows library.

%package gl
Summary: The Motif/Lesstif port of the wxWindows library, OpenGl add-on.
Group: X11/Libraries
Requires: wxMotif

%description gl
OpenGl add-on library for wxMotif, the Motif/Lesstif port of the wxWindows library.

%prep
%setup -n wxMotif
./configure --prefix=%{pref} --enable-burnt_name --with-odbc --with-opengl --with-motif

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{pref} install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr (644, root, root, 755)
%doc COPYING.LIB INSTALL.txt LICENCE.txt README.txt SYMBOLS.txt TODO.txt
%dir %{pref}/share/wx
%{pref}/share/wx/*
%attr(755, -, -) %{pref}/lib/libwx_motif.*
%attr(755, -, -) %{pref}/lib/libwx_motif-2.1.*

%files devel
%defattr (644, root, root, 755)
%dir %{pref}/include/wx
%{pref}/include/wx/*
%dir %{pref}/lib/wx
%{pref}/lib/wx/*
%attr(755, -, -) %{pref}/bin/wxmotif-config
%attr(755, -, -) %{pref}/bin/wx-config

%files gl
%attr(755, -, -) %{pref}/lib/libwx_motif_gl*
