
Name:          tex2rtf
Version:       2.01
Release:       1
Source:        %{name}-%{version}-source.tar.bz2

Prefix:        /usr
Summary:       Program to convert from LaTeX to RTF and HTML
Copyright:     wxWindows Library License
Group:         Applications/Editors
URL:           http://www.wxwindows.org/tex2rtf
Packager:      Vaclav Slavik <vaclav.slavik@matfyz.cz>

%define wxwin_version 2.3
BuildRequires: wxBase-devel >= 2.3.2
BuildRequires: wxBase-static >= 2.3.2
BuildRoot:     /var/tmp/%{name}-%{version}-root

%description
Tex2RTF is a program originally written by Julian Smart to convert 
from a LaTeX subset to four other formats:

* Linear RTF, for importing into a wide range of word processors.
* Windows Help hypertext RTF, for compilation with HC (Help Compiler).
* HTML (Hypertext Markup Language), with ordinary HTML mode and 
  Microsoft HTML Help/wxWindows HTML Help mode.
* XLP wxHelp format, the original wxWindows toolkit help format. 
  This format is now obsolete.

Tex2RTF parses a 'reasonable' subset of LaTeX, notable blind spots being 
the tabbing environment and maths. However, don't expect to translate 
your LaTeX files immediately with no alteration of your source files, since 
the objective is more to have a portable document source language than to 
convert old LaTeX files.

%prep
%setup -n %{name}-%{version}-source

%build

# build tex2rtf binary:
pushd `pwd`
cd src
make -f makefile.unx \
    "CXXFLAGS=$RPM_OPT_FLAGS `wxbase-%{wxwin_version}-config --static --cxxflags` -DNO_GUI" \
    "LDFLAGS=`wxbase-%{wxwin_version}-config --static --libs`"
popd

# and use it to generate documentation:
mkdir -p html
src/tex2rtf docs/tex2rtf.tex html/tex2rtf.html -html
ln -sf tex2rtf_contents.html html/index.html
cp -f docs/*.gif html
cp -f docs/*.txt .

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}%{_bindir}
cp -f src/tex2rtf ${RPM_BUILD_ROOT}%{_bindir}

%clean
rm -Rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%doc *.txt html/
%{_bindir}/*
