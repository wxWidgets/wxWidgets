# Note that this is NOT a relocatable package
%define pref /usr
%define ver 2.1.14
%define rel 0

Summary: The Motif port of the wxWindows library
Name: wxMotif
Version: %{ver}
Release: %{rel}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: ftp://wesley.informatik.uni-freiburg.de/pub/linux/wxxt/source/wxMotif-2.1.0-b8.tgz
URL: http://wesley.informatik.uni-freiburg.de/~wxxt/docs.html
Packager: Robert Roebling <roebling@ruf.uni-freiburg.de>
BuildRoot: /tmp/wxmotif_root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxqt..."
Provides: wxwin

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%prep
%setup -n wxMotif
./configure --prefix=%{pref} --disable-threads --disable-std_iostreams

%build
make

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
%attr(755, -, -) %{pref}/lib/libwx_motif*
%attr(755, -, -) %{pref}/bin/wx-config

