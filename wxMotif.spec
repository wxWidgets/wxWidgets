# RPM .spec file for wxMotif. Based on the .spec file for wxGTK
# (c) by Robert Roebling. Modifications (c) 2000/03 by Robert Fendt.

# Note that this is NOT a relocatable package
%define pref /usr
%define ver 2.4.2
%define ver2 2.4
%define rel 1

Summary: The Motif/Lesstif port of the wxWindows library
Name: wxMotif
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: wxMOTIF-%{ver}.tar.bz2
URL: http://www.wxwindows.org
Packager: Robert Fendt <rfendt@myokay.net>
BuildRoot: /tmp/wxmotif_root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxuniv..."
Provides: wxwin

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%package devel
Summary: The Motif/Lesstif port of the wxWindows library
Group: X11/Libraries
Requires: wxMotif = %{ver}

%description devel
Header files for wxMotif, the Motif/Lesstif port of the wxWindows library.

%package gl
Summary: The Motif/Lesstif port of the wxWindows library, OpenGL add-on.
Group: X11/Libraries
Requires: wxMotif = %{ver}

%description gl
OpenGL add-on library for wxMotif, the Motif/Lesstif port of the wxWindows library.

%package static
Summary: wxMotif static libraries
Group: Development/Libraries

%description static
Static libraries for wxMotif. You need them if you want to link statically against wxMotif.


%prep
%setup -n wxMOTIF-%{ver}

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi

mkdir obj-shared
cd obj-shared
../configure --prefix=%{pref} --enable-soname --with-odbc --with-opengl --with-motif
$MAKE
cd ..

mkdir obj-static
cd obj-static
../configure --prefix=%{pref} --disable-shared --with-odbc --with-opengl --with-motif
$MAKE
cd ..

%install
rm -rf $RPM_BUILD_ROOT
(cd obj-static; make prefix=$RPM_BUILD_ROOT%{pref} install)
(cd obj-shared; make prefix=$RPM_BUILD_ROOT%{pref} install)

%find_lang wxstd

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%post gl
/sbin/ldconfig

%postun gl
/sbin/ldconfig

%post devel
# Install wx-config if there isn't any
if test ! -f %{_bindir}/wx-config ; then
    ln -sf wxmotif-%{ver2}-config %{_bindir}/wx-config
fi

%preun devel
# Remove wx-config if it points to this package
if test -f %{_bindir}/wx-config -a -f /usr/bin/md5sum ; then
  SUM1=`md5sum %{_bindir}/wxmotif-%{ver2}-config | cut -c 0-32`
  SUM2=`md5sum %{_bindir}/wx-config | cut -c 0-32`
  if test "x$SUM1" = "x$SUM2" ; then
    rm -f %{_bindir}/wx-config
  fi
fi


%files -f wxstd.lang
%defattr (-,root,root)
%doc COPYING.LIB *.txt
%dir %{_datadir}/wx
%{_datadir}/wx/*
%{_libdir}/libwx_motif-%{ver2}*.so.*

%files devel
%defattr (-,root,root)
%{_libdir}/libwx_motif-%{ver2}*.so
%dir %{_includedir}/wx
%{_includedir}/wx/*
%dir %{_libdir}/wx
%{_libdir}/wx/*
%{_bindir}/wxmotif-%{ver2}-config
%{_datadir}/aclocal/*.m4

%files gl
%defattr(-,root,root)
%{_libdir}/libwx_motif_gl*

%files static
%defattr (-,root,root)
%{_libdir}/lib*.a
