# Note that this is NOT a relocatable package
%define pref /usr
%define ver 2.1.0
%define rel 8

Summary: The GTK+ 1.2 port of the wxWindows library
Name: wxGTK
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: ftp://wesley.informatik.uni-freiburg.de/pub/linux/wxxt/source/wxGTK-2.1.0-b7.tgz
URL: http://wesley.informatik.uni-freiburg.de/~wxxt/docs.html
Packager: Robert Roebling <roebling@ruf.uni-freiburg.de>
Requires: gtk+ >= 1.2.1
BuildRoot: /tmp/wxgtk_root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%prep
%setup -n wxGTK
./configure --prefix=%{pref} --enable-threads

%build
cd src && make

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
%dir %{pref}/include/wx
%{pref}/include/wx/*
%dir %{pref}/lib/wx
%{pref}/lib/wx/*
%dir %{pref}/share/wx
%{pref}/share/wx/*
%attr(755, -, -) %{pref}/lib/libwx_gtk*
%attr(755, -, -) %{pref}/bin/wx-config

