%define pref /usr
%define ver 2.1.16
%define rpmver 1

Summary: Cross platform GUI toolkit for use with the Python language.
Name: wxPython
Version: %{ver}
Release: %{rpmver}
Copyright: wxWindows License
Group: Development/Languages/Python
Source: wxPython-%{ver}.tar.gz
Packager: Robin Dunn <robin@alldunn.com>
URL:http://alldunn.com/wxPython/index.html
Vendor: Total Control Software
Requires: python >= 1.5.2
##BuildRoot: /tmp/wxPython-build
##Prefix: /usr/lib/python1.5/site-packages

Prefix: %{pref}

%description
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWindows C++ GUI library.  wxPython provides a large variety of
window types and controls, all imlemented with a native look and
feel (and runtime speed) on the platforms it is supported on.


%package gl
Summary: Cross platform GUI toolkit for use with the Python language, OpenGL addon.
Group: Development/Languages/Python
Requires: wxPython

%description gl
OpenGL add on for wxPython.

#----------------------------------------------------------------------

%prep

%setup

%build
cd src
python ../distrib/build.py -b WXPSRCDIR=$PWD
cd ../ogl
python ../../distrib/build.py -b WXPSRCDIR=../../src
cd ../stc
python ../../distrib/build.py -b WXPSRCDIR=../../src
if [ ! -z $NOGLCANVAS ]; then
    cd ../glcanvas
    python ../../distrib/build.py -b WXPSRCDIR=../../src
fi

#----------------------------------------------------------------------

%install
cd src
python ../distrib/build.py -i
cd ../ogl
python ../../distrib/build.py -i
cd ../stc
python ../../distrib/build.py -i
if [ ! -z $NOGLCANVAS ]; then
    cd ../glcanvas
    python ../../distrib/build.py -i
fi

#----------------------------------------------------------------------

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

#----------------------------------------------------------------------

%files
%doc BUILD.txt CHANGES.txt README.txt gpl.txt lgpl.txt licence.txt licendoc.txt preamble.txt
%{pref}/lib/libwxPyHelpers.so
%{pref}/lib/python1.5/site-packages/wxPython/*.py
%{pref}/lib/python1.5/site-packages/wxPython/*.py?
%{pref}/lib/python1.5/site-packages/wxPython/lib
%{pref}/lib/python1.5/site-packages/wxPython/wxcmodule.so
%{pref}/lib/python1.5/site-packages/wxPython/oglcmodule.so
%{pref}/lib/python1.5/site-packages/wxPython/stc_cmodule.so

%files gl
%{pref}/lib/python1.5/site-packages/wxPython/glcanvascmodule.so







