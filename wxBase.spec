%define pref /usr
%define ver 2.5.4
%define ver2 2.5
%define rel 1

# Configurable settings (use --with(out) unicode on rpmbuild command line):
%define unicode 0
%{?_with_unicode: %{expand: %%define unicode 1}}
%{?_without_unicode: %{expand: %%define unicode 0}}

%if %{unicode}
%define wxconfigname wxbaseu-%{ver2}-config
%else
%define wxconfigname wxbase-%{ver2}-config
%endif

%if %{unicode}
%define name wx-base-unicode
%else 
%define name wx-base
%endif

Summary: wxBase library - non-GUI support classes of wxWindows toolkit
Name:   %{name}
Version: %{ver}
Release: %{rel}
License: wxWindows Licence
Group: Development/Libraries
Source: wxBase-%{ver}.tar.bz2
URL: http://www.wxwindows.org
Packager: Vadim Zeitlin <vadim@wxwindows.org>
Prefix: %{pref}
BuildRoot: /var/tmp/%{name}-root
Provides: wxBase

%description
wxBase is a collection of C++ classes providing basic data structures (strings,
lists, arrays), powerful wxDateTime class for date manipulations, portable
wrappers around many OS-specific functions allowing to build the same program
under all supported folders, wxThread class for writing multithreaded programs
using either Win32 or POSIX threads and much more. wxBase currently supports
the following platforms: Win32, generic Unix (Linux, FreeBSD, Solaris, HP-UX,
...) and BeOS.

%package devel
Summary: wxBase headers needed for developping with wxBase
Group: Development/Libraries
Requires: %{name} = %{ver}
Provides: wxBase-devel

%description devel
Header files for wxBase. You need them to develop programs using wxBase.

%package static
Summary: wxBase static libraries
Group: Development/Libraries

%description static
Static libraries for wxBase. You need them if you want to link statically against wxBase.

%prep
%setup -q -n wxBase-%{ver}

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi

mkdir obj-shared
cd obj-shared
../configure --prefix=%{pref} --disable-gui \
%if %{unicode}
             --enable-unicode
%else
             --with-odbc
%endif
$MAKE
cd ..

mkdir obj-static
cd obj-static
../configure --prefix=%{pref} --disable-shared --disable-gui \
%if %{unicode}
             --enable-unicode
%else
             --with-odbc
%endif
$MAKE
cd ..

%install
rm -rf $RPM_BUILD_ROOT
(cd obj-static ; make prefix=$RPM_BUILD_ROOT%{pref} install)
(cd obj-shared ; make prefix=$RPM_BUILD_ROOT%{pref} install)

%find_lang wxstd

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files -f wxstd.lang
%defattr (-,root,root)
%doc COPYING.LIB *.txt
%{_libdir}/libwx_base*so.*

%files devel
%defattr (-,root,root)
%dir %{_includedir}/wx-*
%{_includedir}/wx-*/*
%{_libdir}/libwx_base*.so
%dir %{_libdir}/wx
%{_libdir}/wx/*
%{_bindir}/wxbase*-%{ver2}-config
%{_bindir}/wx-config
%{_datadir}/aclocal/*.m4

%files static
%defattr (-,root,root)
%{_libdir}/lib*.a
