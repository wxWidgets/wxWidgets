%define pref /usr
%define ver 2.5.0
%define ver2 2.5
%define rel 1

# Configurable settings (use --with(out) {unicode,gtk2} on rpmbuild cmd line):
%define unicode 0
%{?_with_unicode: %{expand: %%define unicode 1}}
%{?_without_unicode: %{expand: %%define unicode 0}}

%define gtk2 0
%{?_with_gtk2: %{expand: %%define gtk2 1}}
%{?_without_gtk2: %{expand: %%define gtk2 0}}

%if %{gtk2}
%define gtkver   2
%define portname gtk2
%else
%define gtkver   1.2
%define portname gtk
%endif

%if %{unicode}
%define name        wx-%{portname}-unicode
%define wxbasename  wx-base-unicode
%else 
%define name        wx-%{portname}
%define wxbasename  wx-base
%endif

%if %{unicode}
%define wxconfigname wx%{portname}u-%{ver2}-config
%else
%define wxconfigname wx%{portname}-%{ver2}-config
%endif

Name:    %{name}
Summary: The GTK+ %{gtkver} port of the wxWindows library
Version: %{ver}
Release: %{rel}
License: wxWindows Licence
Group: X11/Libraries
Source: wxGTK-%{ver}.tar.bz2
URL: http://www.wxwindows.org
Packager: Vadim Zeitlin <vadim@wxwindows.org>
Prefix: %{pref}
BuildRoot: /var/tmp/%{name}-root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxuniv..."
Provides: wxwin
Provides: wxGTK

# in addition, we should provide libwx_gtk as automatic generator only notices
# libwx_gtk-%{ver}-%{rel}
Provides: libwx_gtk.so
Provides: libwx_gtk-%{ver2}.so

Requires: %{wxbasename} = %{ver}
BuildRequires: %{wxbasename}-devel = %{ver}

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%package devel
Summary: The GTK+ %{gtkver} port of the wxWindows library
Group: X11/Libraries
Requires: %{name} = %{ver}
Requires: %{wxbasename}-devel = %{ver}
Provides: wxGTK-devel

%description devel
Header files for wxGTK, the GTK+ %{gtkver} port of the wxWindows library.

%package gl
Summary: The GTK+ %{gtkver} port of the wxWindows library, OpenGL add-on.
Group: X11/Libraries
Requires: %{name} = %{ver}

%description gl
OpenGL add-on library for wxGTK, the GTK+ %{gtkver} port of the wxWindows library.

%package static
Summary: wxGTK static libraries
Group: Development/Libraries
Requires: %{wxbasename}-static

%description static
Static libraries for wxGTK. You need them if you want to link statically against wxGTK.

%package contrib
Summary: The GTK+ %{gtkver} port of the wxWindows library, contributed libraries.
Group: X11/Libraries
Requires: %{name} = %{ver}

%description contrib
Contributed libraries for wxGTK, the GTK+ %{gtkver} port of the wxWindows library.

%package contrib-devel
Summary: The GTK+ %{gtkver} port of the wxWindows library
Group: X11/Libraries
Requires: %{name}-contrib = %{ver}
Requires: %{name}-devel = %{ver}

%description contrib-devel
Header files for contributed libraries for wxGTK, the GTK+ %{gtkver} port of the wxWindows library.

%prep
%setup -q -n wxGTK-%{ver}

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi

mkdir obj-shared
cd obj-shared
../configure --prefix=%{pref} \
%if %{unicode}
             --enable-unicode \
%else
             --with-odbc \
%endif
%if %{gtk2}
             --enable-gtk2 \
%endif
             --with-opengl
$MAKE
cd contrib/src
$MAKE
cd ../../..

mkdir obj-static
cd obj-static
../configure --prefix=%{pref} --disable-shared \
%if %{unicode}
             --enable-unicode \
%else
             --with-odbc \
%endif
%if %{gtk2}
             --enable-gtk2 \
%endif
             --with-opengl
$MAKE
cd ..

%install
rm -rf $RPM_BUILD_ROOT
(cd obj-static; make prefix=$RPM_BUILD_ROOT%{pref} install)
(cd obj-shared; make prefix=$RPM_BUILD_ROOT%{pref} install)

# Remove headers that are part of wx-base-devel:
(
cd $RPM_BUILD_ROOT
for f in  `rpm -ql %{wxbasename}-devel | sed -e 's,\(.*\),.\1,g'` ; do
  if test -f $f ; then rm -f $f ; fi
done
)

# list of all core headers:
find $RPM_BUILD_ROOT/usr/include/wx -type f | sed -e "s,$RPM_BUILD_ROOT,,g" >core-headers.files

# contrib stuff:
(cd obj-shared/contrib/src; make prefix=$RPM_BUILD_ROOT%{pref} install)
(cd obj-shared/contrib/utils/wxrc; make prefix=$RPM_BUILD_ROOT%{pref} install)

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
    ln -sf %{wxconfigname} %{_bindir}/wx-config
fi

%preun devel
# Remove wx-config if it points to this package
if test -f %{_bindir}/wx-config -a -f /usr/bin/md5sum ; then
  SUM1=`md5sum %{_bindir}/%{wxconfigname} | cut -c 0-32`
  SUM2=`md5sum %{_bindir}/wx-config | cut -c 0-32`
  if test "x$SUM1" = "x$SUM2" ; then
    rm -f %{_bindir}/wx-config
  fi
fi


%files
%defattr(-,root,root)
%doc COPYING.LIB *.txt
%dir %{_datadir}/wx
%{_datadir}/wx/*
%{_libdir}/libwx_%{portname}*_core*.so.*
%{_libdir}/libwx_%{portname}*_html*.so.*
%{_libdir}/libwx_%{portname}*_adv*.so.*
%if !%{unicode}
%{_libdir}/libwx_%{portname}*_dbgrid*.so.*
%endif

%files devel -f core-headers.files
%defattr(-,root,root)
%{_libdir}/libwx_%{portname}*_core*.so
%{_libdir}/libwx_%{portname}*_html*.so
%{_libdir}/libwx_%{portname}*_adv*.so
%if !%{unicode}
%{_libdir}/libwx_%{portname}*_dbgrid*.so
%endif
%{_libdir}/libwx_%{portname}*_gl*.so
%dir %{_libdir}/wx
%{_libdir}/wx/*
%{_bindir}/%{wxconfigname}

%files gl
%defattr(-,root,root)
%{_libdir}/libwx_%{portname}*_gl*.so.*

%files static
%defattr (-,root,root)
%{_libdir}/libwx_gtk*.a

%files contrib
%defattr(-,root,root)
%{_libdir}/libwx_%{portname}*_animate*.so.*
%{_libdir}/libwx_%{portname}*_deprecated*.so.*
%{_libdir}/libwx_%{portname}*_fl*.so.*
%{_libdir}/libwx_%{portname}*_gizmos*.so.*
%{_libdir}/libwx_%{portname}*_mmedia*.so.*
%{_libdir}/libwx_%{portname}*_ogl*.so.*
%{_libdir}/libwx_%{portname}*_plot*.so.*
%{_libdir}/libwx_%{portname}*_stc*.so.*
%{_libdir}/libwx_%{portname}*_svg*.so.*
%{_libdir}/libwx_%{portname}*_xrc*.so.*

%files contrib-devel
%defattr(-,root,root)
%{_bindir}/wxrc
%dir %{_includedir}/wx/animate
%{_includedir}/wx/animate/*
%{_libdir}/libwx_%{portname}*_animate*.so

%dir %{_includedir}/wx/deprecated
%{_includedir}/wx/deprecated/*
%{_libdir}/libwx_%{portname}*_deprecated*.so

%dir %{_includedir}/wx/fl
%{_includedir}/wx/fl/*
%{_libdir}/libwx_%{portname}*_fl*.so

%dir %{_includedir}/wx/gizmos
%{_includedir}/wx/gizmos/*
%{_libdir}/libwx_%{portname}*_gizmos*.so

%dir %{_includedir}/wx/mmedia
%{_includedir}/wx/mmedia/*
%{_libdir}/libwx_%{portname}*_mmedia*.so

%dir %{_includedir}/wx/ogl
%{_includedir}/wx/ogl/*
%{_libdir}/libwx_%{portname}*_ogl*.so

%dir %{_includedir}/wx/plot
%{_includedir}/wx/plot/*
%{_libdir}/libwx_%{portname}*_plot*.so

%dir %{_includedir}/wx/stc
%{_includedir}/wx/stc/*
%{_libdir}/libwx_%{portname}*_stc*.so

%dir %{_includedir}/wx/svg
%{_includedir}/wx/svg/*
%{_libdir}/libwx_%{portname}*_svg*.so

%dir %{_includedir}/wx/xrc
%{_includedir}/wx/xrc/*
%{_libdir}/libwx_%{portname}*_xrc*.so
