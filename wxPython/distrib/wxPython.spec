%define pref /usr
%define ver 2.1.15
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
Requires: python >= 1.5.1
##BuildRoot: /tmp/wxPython-build
##Prefix: /usr/lib/python1.5/site-packages

Prefix: %{pref}

%description
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWindows C++ GUI library.  wxPython provides a large variety of
window types and controls, all imlemented with a native look and
feel (and runtime speed) on the platforms it is supported on.

%prep

%setup

%build
cd src
python ../distrib/build.py -b WXPSRCDIR=$PWD
cd ../modules/html
python ../../distrib/build.py -b WXPSRCDIR=../../src
cd ../utils
python ../../distrib/build.py -b WXPSRCDIR=../../src
cd ../ogl
python ../../distrib/build.py -b WXPSRCDIR=../../src
cd ../stc
python ../../distrib/build.py -b WXPSRCDIR=../../src
if [ ! -z $NOGLCANVAS ]; then
    cd ../glcanvas
    python ../../distrib/build.py -b WXPSRCDIR=../../src
fi

%install
cd src
python ../distrib/build.py -i
cd ../modules/html
python ../../distrib/build.py -i
cd ../utils
python ../../distrib/build.py -i
cd ../ogl
python ../../distrib/build.py -i
cd ../stc
python ../../distrib/build.py -i
if [ ! -z $NOGLCANVAS ]; then
    cd ../glcanvas
    python ../../distrib/build.py -i
fi


%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%doc BUILD.txt CHANGES.txt README.txt gpl.txt lgpl.txt licence.txt licendoc.txt preamble.txt
%{pref}/lib/libwxPyHelpers.so
%{pref}/lib/python1.5/site-packages/wxPython



