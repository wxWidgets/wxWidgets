%define pref /usr
%define ver 2.5.3
%define ver2 2.5
%define rel 1

%define portname    motif
%define name        wx-%{portname}
%define wxbasename  wx-base
%define wxconfigname wx%{portname}-%{ver2}-config

Summary: The Motif/Lesstif port of the wxWindows library
Name: %{name}
Version: %{ver}
Release: %{rel}
License: wxWindows Licence
Group: X11/Libraries
Source: wxMOTIF-%{ver}.tar.bz2
URL: http://www.wxwidgets.org
Packager: wxWindows developers <wx-dev@lists.wxwidgets.org>
BuildRoot: /tmp/wxmotif_root

# all packages providing an implementation of wxWindows library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxuniv..."
Provides: wxwin

Provides: wxMotif

Requires:      %{wxbasename} = %{ver}

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code.

%package devel
Summary: The Motif/Lesstif port of the wxWindows library
Group: X11/Libraries
Requires: %{name} = %{ver}
Requires: %{wxbasename}-devel = %{ver}

%description devel
Header files for wxMotif, the Motif/Lesstif port of the wxWindows library.

%package gl
Summary: The Motif/Lesstif port of the wxWindows library, OpenGL add-on.
Group: X11/Libraries
Requires: %{name} = %{ver}

%description gl
OpenGL add-on library for wxMotif, the Motif/Lesstif port of the wxWindows library.

%package static
Summary: wxMotif static libraries
Group: Development/Libraries

%description static
Static libraries for wxMotif. You need them if you want to link statically against wxMotif.

%prep
%setup -q -n wxMOTIF-%{ver}

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi

mkdir obj-shared
cd obj-shared
../configure --prefix=%{pref} --with-odbc --with-opengl --with-motif
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

# Remove headers that are part of wx-base-devel:

# --- wxBase headers list begins here ---
cat <<EOF >wxbase-headers-list
wx/afterstd.h
wx/app.h
wx/apptrait.h
wx/arrimpl.cpp
wx/arrstr.h
wx/beforestd.h
wx/buffer.h
wx/build.h
wx/chkconf.h
wx/clntdata.h
wx/cmdline.h
wx/confbase.h
wx/config.h
wx/containr.h
wx/datetime.h
wx/datetime.inl
wx/datstrm.h
wx/dde.h
wx/debug.h
wx/defs.h
wx/dir.h
wx/dlimpexp.h
wx/dynarray.h
wx/dynlib.h
wx/dynload.h
wx/encconv.h
wx/event.h
wx/except.h
wx/features.h
wx/ffile.h
wx/file.h
wx/fileconf.h
wx/filefn.h
wx/filename.h
wx/filesys.h
wx/fontenc.h
wx/fontmap.h
wx/fs_mem.h
wx/fs_zip.h
wx/hash.h
wx/hashmap.h
wx/hashset.h
wx/iconloc.h
wx/init.h
wx/intl.h
wx/iosfwrap.h
wx/ioswrap.h
wx/ipc.h
wx/ipcbase.h
wx/isql.h
wx/isqlext.h
wx/list.h
wx/listimpl.cpp
wx/log.h
wx/longlong.h
wx/math.h
wx/memconf.h
wx/memory.h
wx/memtext.h
wx/mimetype.h
wx/module.h
wx/msgout.h
wx/msgout.h
wx/mstream.h
wx/object.h
wx/platform.h
wx/process.h
wx/ptr_scpd.h
wx/regex.h
wx/scopeguard.h
wx/snglinst.h
wx/sstream.h
wx/stack.h
wx/stockitem.h
wx/stopwatch.h
wx/strconv.h
wx/stream.h
wx/string.h
wx/sysopt.h
wx/textbuf.h
wx/textfile.h
wx/thread.h
wx/thrimpl.cpp
wx/timer.h
wx/tokenzr.h
wx/txtstrm.h
wx/types.h
wx/utils.h
wx/variant.h
wx/vector.h
wx/version.h
wx/volume.h
wx/wfstream.h
wx/wx.h
wx/wxchar.h
wx/wxprec.h
wx/xti.h
wx/xtistrm.h
wx/zipstrm.h
wx/zstream.h
wx/msw/apptrait.h
wx/msw/apptbase.h
wx/msw/chkconf.h
wx/msw/crashrpt.h
wx/msw/dde.h
wx/msw/gccpriv.h
wx/msw/mimetype.h
wx/msw/winundef.h
wx/msw/wrapcctl.h
wx/msw/wrapwin.h
wx/fs_inet.h
wx/gsocket.h
wx/protocol/file.h
wx/protocol/ftp.h
wx/protocol/http.h
wx/protocol/protocol.h
wx/sckaddr.h
wx/sckipc.h
wx/sckstrm.h
wx/socket.h
wx/url.h
wx/msw/gsockmsw.h
wx/xml/xml.h
wx/xtixml.h
wx/db.h
wx/dbkeyg.h
wx/dbtable.h
EOF
# --- wxBase headers list ends here ---
for f in `cat wxbase-headers-list` ; do
  rm -f $RPM_BUILD_ROOT%{_includedir}/wx-*/$f
done

# list of all core headers:
find $RPM_BUILD_ROOT/usr/include/wx -type f | sed -e "s,$RPM_BUILD_ROOT,,g" >core-headers.files

# remove wxBase files so that RPM doesn't complain about unpackaged files:
rm -f $RPM_BUILD_ROOT%{_libdir}/libwx_base*
rm -f $RPM_BUILD_ROOT%{_datadir}/aclocal/*
rm -f $RPM_BUILD_ROOT%{_datadir}/locale/*/*/*


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


%files
%defattr(-,root,root)
%doc COPYING.LIB *.txt
%dir %{_datadir}/wx
%{_datadir}/wx/*
%{_libdir}/libwx_%{portname}*_core*.so.*
%{_libdir}/libwx_%{portname}*_html*.so.*
%{_libdir}/libwx_%{portname}*_adv*.so.*
%{_libdir}/libwx_%{portname}*_dbgrid*.so.*

%files devel -f core-headers.files
%defattr(-,root,root)
%{_libdir}/libwx_%{portname}*_core*.so
%{_libdir}/libwx_%{portname}*_html*.so
%{_libdir}/libwx_%{portname}*_adv*.so
%{_libdir}/libwx_%{portname}*_dbgrid*.so
%{_libdir}/libwx_%{portname}*_gl*.so
%dir %{_libdir}/wx
%{_libdir}/wx/*
%{_bindir}/%{wxconfigname}

%files gl
%defattr(-,root,root)
%{_libdir}/libwx_%{portname}*_gl*.so.*

%files static
%defattr (-,root,root)
%{_libdir}/libwx_%{portname}*.a

