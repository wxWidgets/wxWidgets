%define pref /usr
%define ver 2.2.6
%define rel 0

Summary: wxBase library - non-GUI support classes of wxWindows toolkit
Name: wxBase
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: Development/Libraries
Source: wxBase-%{ver}.tar.gz
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
./configure --prefix=%{pref} --disable-gui --disable-std_iostreams

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
%defattr (644, root, root, 755)
%doc COPYING.LIB LICENCE.txt README.txt SYMBOLS.txt
%attr(755, -, -) %{pref}/lib/libwx_base*
%dir %{pref}/lib/wx
%dir %{pref}/lib/wx/include
%dir %{pref}/lib/wx/include/wx
%{pref}/lib/wx/include/wx/base/setup.h

%files devel -f src/rpmfiles.lst
%defattr (644, root, root, 755)
%attr(755, -, -) %{pref}/bin/wx-config

