%define pref /usr
%define ver 2.2.9
%define ver2 2.2
%define rel 0

Summary: wxBase library - non-GUI support classes of wxWindows toolkit
Name: wxBase
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: Development/Libraries
Source: wxBase-%{ver}.tar.bz2
URL: http://www.wxwindows.org/
Packager: Vadim Zeitlin <vadim@wxwindows.org>
Prefix: %{pref}
BuildRoot: /var/tmp/%{name}-root

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
Requires: wxBase

%description devel
Header files for wxBase. You need them to develop programs using wxBase.

%prep
%setup -n wxBase-%{ver}
./configure --prefix=%{pref} --no-recursion --disable-gui --disable-std_iostreams

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
make prefix=$RPM_BUILD_ROOT%{pref} install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr (-, root, root)
%doc COPYING.LIB LICENCE.txt README.txt SYMBOLS.txt
%{pref}/lib/libwx_base*

%files devel
%defattr (-, root, root)
%dir %{pref}/include/wx
%dir %{pref}/lib/wx
%{pref}/lib/wx/*
%{pref}/include/wx/*
%{pref}/bin/wx*-config

