%define _prefix /usr
%define ver  3.0.0
%define ver2 3.0
%define rel  1
# version for package name according to OpenSuse policy:
# http://en.opensuse.org/openSUSE:Shared_library_packaging_policy
%define sver 2_9-5

# Configurable settings (use --with(out) {unicode,gtk2} on rpmbuild cmd line):
%define unicode 1
%{?_with_unicode: %{expand: %%define unicode 1}}
%{?_without_unicode: %{expand: %%define unicode 0}}

%define gtk2 1
%{?_with_gtk2: %{expand: %%define gtk2 1}}
%{?_without_gtk2: %{expand: %%define gtk2 0}}

# "buildname" needs to be e.g. gtk2ud for debug builds
%if %{gtk2}
    %define gtkver 2
    %define portname gtk2
%if %{unicode}
    %define buildname gtk2u
%else
    %define buildname gtk2
%endif
%else
    %define gtkver 1.2
    %define portname gtk
    %define buildname gtk
%endif

%if %{unicode}
    %define name		wx-%{portname}-unicode
    %define wxconfig		%{portname}-unicode-%{ver2}
    %define wxconfigstatic	%{portname}-unicode-static-%{ver2}
    %define wxconfiglink	wx%{portname}u-%{ver2}-config
%else
    %define name		wx-%{portname}-ansi
    %define wxconfig		%{portname}-ansi-%{ver2}
    %define wxconfigstatic	%{portname}-ansi-static-%{ver2}
    %define wxconfiglink	wx%{portname}-%{ver2}-config
%endif

%if %{unicode}
    %define wxbasename		wx-base-unicode
    %define wxbaseconfig	base-unicode-%{ver2}
    %define wxbaseconfiglink	wxbaseu-%{ver2}-config
%else
    %define wxbasename		wx-base-ansi
    %define wxbaseconfig	base-ansi-%{ver2}
    %define wxbaseconfiglink	wxbase-%{ver2}-config
%endif

%if 0%{?suse_version}
# avoiding shlib-policy-name-error: Your package contains a single shared
# library but is not named after its SONAME.
    %define glname  libwx_%{buildname}_gl-%{sver}
%else
    %define glname  %{name}-gl
%endif

Summary: The GTK+ %{gtkver} port of the wxWidgets library
Name: %{name}
Version: %{ver}
Release: %{rel}
License: wxWindows Licence
Group: System/Libraries
Source: wxWidgets-%{ver}.tar.bz2
URL: http://www.wxwidgets.org
Prefix: %{_prefix}
BuildRoot: %{_tmppath}/%{name}-root
Requires: %{wxbasename} = %{ver}
%if %{portname} == gtk2
BuildRequires: gtk2-devel
%else
BuildRequires: gtk+-devel >= 1.2.0
%endif

BuildRequires: zlib-devel, libjpeg-devel, libpng-devel, libtiff-devel
# on RedHat 5 default GCC 4.1.2 crashes during compilation
%if 0%{?rhel} == 5
BuildRequires: gcc44-c++
%else
BuildRequires: gcc-c++
%endif

%if 0%{?suse_version}
BuildRequires: libexpat-devel, Mesa-devel, xorg-x11-libSM-devel
BuildRequires: gstreamer-0_10-devel, gstreamer-0_10-plugins-base-devel
%else
%if 0%{?mandriva_version}
BuildRequires: libexpat-devel, libmesaglu-devel, libsm-devel
# For now disabling mediactrl
# FIXME: How to resolve OBS Mandriva dependecies for these?
#BuildRequires: libgstreamer-devel, libgstreamer-plugins-base-devel
%else
BuildRequires: expat-devel, libGLU-devel, libSM-devel
BuildRequires: gstreamer-devel, gstreamer-plugins-base-devel
%endif
%endif

# all packages providing an implementation of wxWidgets library (regardless of
# the toolkit used) should provide the (virtual) wxwin package, this makes it
# possible to require wxwin instead of requiring "wxgtk or wxmotif or wxuniv..."
Provides: wxwin = %{version}
Provides: wxGTK = %{version}

%description
wxWidgets is a free C++ library for cross-platform GUI development.
With wxWidgets, you can create applications for different GUIs (GTK+,
Motif, MS Windows, MacOS X, Windows CE, GPE) from the same source code.

%package -n wx-i18n
Summary: The translations for the wxWidgets library
Group: System/Libraries

%description -n wx-i18n
The translations files for the wxWidgets library.

%package devel
Summary: The GTK+ %{gtkver} port of the wxWidgets library
Group: Development/Libraries
Requires: %{name} = %{ver}
Requires: %{glname} = %{ver}
Requires: %{wxbasename}-devel = %{ver}
Provides: wxGTK-devel = %{version}

%description devel
The GTK+ %{gtkver} port of the wxWidgets library, header files.

%package -n %{glname}
Summary: The GTK+ %{gtkver} port of the wxWidgets library, OpenGL add-on
Group: System/Libraries
Requires: %{name} = %{ver}
Provides: wxGTK-gl = %{version}

%description -n %{glname}
OpenGL add-on library for wxGTK, the GTK+ %{gtkver} port of the wxWidgets library.

%package -n %{wxbasename}
Summary: wxBase library - non-GUI support classes of the wxWidgets toolkit
Group: Development/Libraries
Provides: wxBase = %{version}

%description -n %{wxbasename}
wxBase is a collection of C++ classes providing basic data structures (strings,
lists, arrays), portable wrappers around many OS-specific funstions (file
operations, time/date manipulations, threads, processes, sockets, shared
library loading) as well as other utility classes (streams, archive and
compression). wxBase currently supports Win32, most Unix variants (Linux,
FreeBSD, Solaris, HP-UX) and MacOS X (Carbon and Mach-0).

%package -n %{wxbasename}-devel
Summary: wxBase library, header files
Group: Development/Libraries
Provides: wxBase-devel = %{version}

%description -n %{wxbasename}-devel
wxBase library - non-GUI support classes of the wxWidgets toolkit,
header files.

%prep
%setup -q -n wxWidgets-%{ver}

%build

%if 0%{?rhel} == 5
export CC=gcc44
export CXX=g++44
%endif
# if it's not set OpenSuse warns: "compiled without RPM_OPT_FLAGS"
export CFLAGS="$RPM_OPT_FLAGS -fno-strict-aliasing"
export CXXFLAGS="$RPM_OPT_FLAGS -fno-strict-aliasing"

mkdir obj-shared
cd obj-shared
../configure --prefix=%{_prefix} --libdir=%{_libdir} --disable-rpath \
%if ! %{gtk2}
			      --with-gtk=1 \
%else
			      --with-gtk=2 \
%endif
%if %{unicode}
			      --enable-unicode \
%else
			      --disable-unicode \
%endif
%if ! 0%{?mandriva_version}
			      --enable-mediactrl \
%endif
			      --with-opengl
make %{?_smp_mflags}

cd ..

mkdir obj-static
cd obj-static
../configure --prefix=%{_prefix} --libdir=%{_libdir} --disable-rpath \
%if ! %{gtk2}
			      --with-gtk=1 \
%else
			      --with-gtk=2 \
%endif
			      --disable-shared \
%if %{unicode}
			      --enable-unicode \
%else
			      --disable-unicode \
%endif
%if ! 0%{?mandriva_version}
			      --enable-mediactrl \
%endif
			      --with-opengl
make %{?_smp_mflags}

cd ..

make -C locale allmo

%install
rm -rf $RPM_BUILD_ROOT
(cd obj-static; make DESTDIR=$RPM_BUILD_ROOT prefix=%{_prefix} install)
(cd obj-shared; make DESTDIR=$RPM_BUILD_ROOT prefix=%{_prefix} install)

# --- wxBase headers list begins here ---
cat <<EOF >wxbase-headers.files
wx/afterstd.h
wx/any.h
wx/anystr.h
wx/app.h
wx/apptrait.h
wx/archive.h
wx/arrimpl.cpp
wx/arrstr.h
wx/atomic.h
wx/base64.h
wx/beforestd.h
wx/buffer.h
wx/build.h
wx/chartype.h
wx/checkeddelete.h
wx/chkconf.h
wx/clntdata.h
wx/cmdargs.h
wx/cmdline.h
wx/compiler.h
wx/confbase.h
wx/config.h
wx/convauto.h
wx/containr.h
wx/cpp.h
wx/crt.h
wx/datetime.h
wx/datstrm.h
wx/dde.h
wx/debug.h
wx/defs.h
wx/dir.h
wx/dlimpexp.h
wx/dlist.h
wx/dynarray.h
wx/dynlib.h
wx/dynload.h
wx/encconv.h
wx/event.h
wx/eventfilter.h
wx/evtloop.h
wx/except.h
wx/features.h
wx/flags.h
wx/ffile.h
wx/file.h
wx/fileconf.h
wx/filefn.h
wx/filename.h
wx/filesys.h
wx/fontenc.h
wx/fontmap.h
wx/fs_arc.h
wx/fs_filter.h
wx/fs_mem.h
wx/fs_zip.h
wx/hash.h
wx/hashmap.h
wx/hashset.h
wx/html/forcelnk.h
wx/iconloc.h
wx/init.h
wx/intl.h
wx/iosfwrap.h
wx/ioswrap.h
wx/ipc.h
wx/ipcbase.h
wx/kbdstate.h
wx/language.h
wx/link.h
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
wx/mousestate.h
wx/msgout.h
wx/msgqueue.h
wx/mstream.h
wx/numformatter.h
wx/object.h
wx/platform.h
wx/platinfo.h
wx/power.h
wx/process.h
wx/ptr_scpd.h
wx/ptr_shrd.h
wx/recguard.h
wx/regex.h
wx/rtti.h
wx/scopedarray.h
wx/scopedptr.h
wx/scopeguard.h
wx/sharedptr.h
wx/snglinst.h
wx/sstream.h
wx/stack.h
wx/stackwalk.h
wx/stdpaths.h
wx/stdstream.h
wx/stockitem.h
wx/stopwatch.h
wx/strconv.h
wx/stream.h
wx/string.h
wx/stringimpl.h
wx/stringops.h
wx/strvararg.h
wx/sysopt.h
wx/tarstrm.h
wx/textbuf.h
wx/textfile.h
wx/thread.h
wx/thrimpl.cpp
wx/time.h
wx/timer.h
wx/tls.h
wx/tokenzr.h
wx/tracker.h
wx/translation.h
wx/txtstrm.h
wx/typeinfo.h
wx/types.h
wx/unichar.h
wx/uri.h
wx/ustring.h
wx/utils.h
wx/variant.h
wx/vector.h
wx/version.h
wx/versioninfo.h
wx/volume.h
wx/weakref.h
wx/wfstream.h
wx/wx.h
wx/wxchar.h
wx/wxcrt.h
wx/wxcrtbase.h
wx/wxcrtvararg.h
wx/wxprec.h
wx/xlocale.h
wx/xti.h
wx/xti2.h
wx/xtistrm.h
wx/xtictor.h
wx/xtihandler.h
wx/xtiprop.h
wx/xtitypes.h
wx/zipstrm.h
wx/zstream.h
wx/meta/convertible.h
wx/meta/if.h
wx/meta/implicitconversion.h
wx/meta/int2type.h
wx/meta/movable.h
wx/meta/pod.h
wx/meta/removeref.h
wx/fswatcher.h
wx/generic/fswatcher.h
wx/unix/app.h
wx/unix/apptbase.h
wx/unix/apptrait.h
wx/unix/chkconf.h
wx/unix/evtloop.h
wx/unix/evtloopsrc.h
wx/unix/pipe.h
wx/unix/stdpaths.h
wx/unix/stackwalk.h
wx/unix/tls.h
wx/unix/fswatcher_kqueue.h
wx/unix/execute.h
wx/unix/mimetype.h
wx/unix/fswatcher_inotify.h
wx/fs_inet.h
wx/protocol/file.h
wx/protocol/ftp.h
wx/protocol/http.h
wx/protocol/log.h
wx/protocol/protocol.h
wx/sckaddr.h
wx/sckipc.h
wx/sckstrm.h
wx/socket.h
wx/url.h
wx/xml/xml.h
wx/xtixml.h
EOF
# --- wxBase headers list ends here ---

# --- wxBase headers list with paths ---
sed -e 's@^@%{_includedir}/wx-%{ver2}/@' wxbase-headers.files > wxbase-headers.paths


# temporarily remove base headers
mkdir $RPM_BUILD_ROOT/_save_dir
cp -r $RPM_BUILD_ROOT%{_includedir}/wx-%{ver2} $RPM_BUILD_ROOT/_save_dir
for f in `cat wxbase-headers.files` ; do
    rm -f $RPM_BUILD_ROOT%{_includedir}/wx-%{ver2}/$f
done
# list of all core headers:
find $RPM_BUILD_ROOT%{_includedir}/wx-%{ver2} -type f | sed -e "s,$RPM_BUILD_ROOT,,g" >core-headers.files
# move base headers (actually all headers) back again
cp -f -r $RPM_BUILD_ROOT/_save_dir/* $RPM_BUILD_ROOT%{_includedir}
rm -rf $RPM_BUILD_ROOT/_save_dir

# utils:
(cd obj-shared/utils/wxrc; make DESTDIR=$RPM_BUILD_ROOT prefix=%{_prefix} install)

# wx-config link is created during package installation, remove it for now
rm -f $RPM_BUILD_ROOT%{_bindir}/wx-config

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%post devel
# link wx-config when you install RPM.
%if %{unicode}
    ln -sf %{_libdir}/wx/config/%{wxconfig} %{_bindir}/wx-config
%endif
# link wx-config with explicit name.
ln -sf %{_libdir}/wx/config/%{wxconfig} %{_bindir}/%{wxconfiglink}
/sbin/ldconfig

%postun devel
/sbin/ldconfig

%preun devel
%if %{unicode}
    rm -f %{_bindir}/wx-config
%endif
rm -f %{_bindir}/%{wxconfiglink}

%post -n %{wxbasename}
/sbin/ldconfig

%postun -n %{wxbasename}
/sbin/ldconfig

%post -n %{wxbasename}-devel
# link wx-config with explicit name.
ln -sf %{_libdir}/wx/config/%{wxbaseconfig} %{_bindir}/%{wxbaseconfiglink}
/sbin/ldconfig

%postun  -n %{wxbasename}-devel
/sbin/ldconfig

%preun  -n %{wxbasename}-devel
rm -f %{_bindir}/%{wxbaseconfiglink}

%post -n %{glname}
/sbin/ldconfig

%postun -n %{glname}
/sbin/ldconfig

%files
%defattr(-,root,root)
#doc COPYING.LIB *.txt
%{_libdir}/libwx_%{buildname}_adv-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_aui-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_core-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_html-%{ver2}.so.*
%if ! 0%{?mandriva_version}
%{_libdir}/libwx_%{buildname}_media-%{ver2}.so.*
%endif
%{_libdir}/libwx_%{buildname}_propgrid-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_qa-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_ribbon-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_richtext-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_stc-%{ver2}.so.*
%{_libdir}/libwx_%{buildname}_xrc-%{ver2}.so.*

%files -n wx-i18n
%defattr(-,root,root)
%{_datadir}/locale/*/LC_MESSAGES/*.mo

%files devel -f core-headers.files
%defattr(-,root,root)
%dir %{_includedir}/wx-%{ver2}/wx/aui
%dir %{_includedir}/wx-%{ver2}/wx/gtk
%dir %{_includedir}/wx-%{ver2}/wx/gtk/gnome
%dir %{_includedir}/wx-%{ver2}/wx/gtk/hildon
%dir %{_includedir}/wx-%{ver2}/wx/persist
%dir %{_includedir}/wx-%{ver2}/wx/propgrid
%dir %{_includedir}/wx-%{ver2}/wx/ribbon
%dir %{_includedir}/wx-%{ver2}/wx/richtext
%dir %{_includedir}/wx-%{ver2}/wx/stc
%dir %{_includedir}/wx-%{ver2}/wx/unix
%dir %{_includedir}/wx-%{ver2}/wx/xrc
# shared libs
%{_libdir}/libwx_%{buildname}_adv-%{ver2}.so
%{_libdir}/libwx_%{buildname}_aui-%{ver2}.so
%{_libdir}/libwx_%{buildname}_core-%{ver2}.so
%{_libdir}/libwx_%{buildname}_gl-%{ver2}.so
%{_libdir}/libwx_%{buildname}_html-%{ver2}.so
%if ! 0%{?mandriva_version}
%{_libdir}/libwx_%{buildname}_media-%{ver2}.so
%endif
%{_libdir}/libwx_%{buildname}_propgrid-%{ver2}.so
%{_libdir}/libwx_%{buildname}_qa-%{ver2}.so
%{_libdir}/libwx_%{buildname}_ribbon-%{ver2}.so
%{_libdir}/libwx_%{buildname}_richtext-%{ver2}.so
%{_libdir}/libwx_%{buildname}_stc-%{ver2}.so
%{_libdir}/libwx_%{buildname}_xrc-%{ver2}.so
# static libs
%{_libdir}/libwx_%{buildname}_adv-%{ver2}.a
%{_libdir}/libwx_%{buildname}_aui-%{ver2}.a
%{_libdir}/libwx_%{buildname}_core-%{ver2}.a
%{_libdir}/libwx_%{buildname}_gl-%{ver2}.a
%{_libdir}/libwx_%{buildname}_html-%{ver2}.a
%if ! 0%{?mandriva_version}
%{_libdir}/libwx_%{buildname}_media-%{ver2}.a
%endif
%{_libdir}/libwx_%{buildname}_propgrid-%{ver2}.a
%{_libdir}/libwx_%{buildname}_qa-%{ver2}.a
%{_libdir}/libwx_%{buildname}_ribbon-%{ver2}.a
%{_libdir}/libwx_%{buildname}_richtext-%{ver2}.a
%{_libdir}/libwx_%{buildname}_stc-%{ver2}.a
%{_libdir}/libwx_%{buildname}_xrc-%{ver2}.a
%dir %{_libdir}/wx
%dir %{_libdir}/wx/config
%dir %{_libdir}/wx/include
%dir %{_libdir}/wx/include/%{wxconfig}
%dir %{_libdir}/wx/include/%{wxconfig}/wx
%dir %{_libdir}/wx/include/%{wxconfigstatic}
%dir %{_libdir}/wx/include/%{wxconfigstatic}/wx
%{_libdir}/wx/config/%{wxconfig}
%{_libdir}/wx/include/%{wxconfig}/wx/setup.h
%{_libdir}/wx/config/%{wxconfigstatic}
%{_libdir}/wx/include/%{wxconfigstatic}/wx/setup.h
%{_bindir}/wxrc*

%files -n %{wxbasename}
%defattr(-,root,root)
%{_libdir}/libwx_base*-%{ver2}.so.*

%files -n %{wxbasename}-devel -f wxbase-headers.paths
%defattr (-,root,root)
%dir %{_includedir}/wx-%{ver2}
%dir %{_includedir}/wx-%{ver2}/wx
%dir %{_includedir}/wx-%{ver2}/wx/generic
%dir %{_includedir}/wx-%{ver2}/wx/html
%dir %{_includedir}/wx-%{ver2}/wx/meta
%dir %{_includedir}/wx-%{ver2}/wx/protocol
%dir %{_includedir}/wx-%{ver2}/wx/unix
%dir %{_includedir}/wx-%{ver2}/wx/xml
%dir %{_datadir}/bakefile/presets
%{_libdir}/libwx_base*-%{ver2}.so
%{_libdir}/libwx_base*-%{ver2}.a
%if %{unicode}
    %{_libdir}/libwxregexu-%{ver2}.a
%endif
%{_libdir}/libwxscintilla-%{ver2}.a
%dir %{_libdir}/wx
%dir %{_datadir}/bakefile
%dir %{_datadir}/bakefile/presets
%{_datadir}/aclocal/*.m4
%{_datadir}/bakefile/presets/*

%files -n %{glname}
%defattr(-,root,root)
%{_libdir}/libwx_%{buildname}_gl-%{ver2}.so.*
