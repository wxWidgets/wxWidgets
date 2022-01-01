Installing wxWidgets for Windows UWP      {#plat_uwp_install}
------------------------------------

This is wxWidgets for the Universal Windows Platform.
Supported OS are Windows 10+ including both 32 bit and 64 bit versions.
**This platform is currently in development**

[TOC]

Installation                           {#uwp_install}
============

If you are using one of the supported compilers, you can use
[pre-built binaries](@ref plat_uwp_binaries).

In this case, just uncompress the binaries archive under any directory.
Otherwise, or if you want to build a configuration of the library
different from the default one, you need to build the library from
sources before using it.

Only Cmake can be used to build the UWP platform for now.

The first step, which you may have already performed, unless you are
reading this file online, is to download the source archive and
uncompress it in any directory. It is strongly advised to avoid using
spaces in the name of this directory, i.e. notably do *not* choose a
location under "C:\Program Files", as this risks creating problems
with makefiles and other command-line tools.

After choosing the directory location, please define WXWIN environment
variable containing the full path to this directory. While this is not
actually required, this makes using the library more convenient and
this environment variable is used in the examples below.


Building wxWidgets                     {#uwp_build}
==================

The following steps explain how to compile wxWidgets using Visual Studio.


1) Make sure you have installed the development workloads required to use C++/WinRT. 
https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt#visual-studio-support-for-cwinrt-xaml-the-vsix-extension-and-the-nuget-package  

2) Generate the Visual Studio solution using Cmake: 
**cmake -B<ouptut_folder> -G <cmake_generator> -A x64 -DCMAKE_SYSTEM_NAME=<system_name> -DCMAKE_SYSTEM_VERSION="<system_version>" -DwxBUILD_TOOLKIT="uwp"**  

Powershell example for Visual Studio 2022 under Windows 10:  

**cmake -BMyOutputFolder -G "Visual Studio 17 2022" -A x64 -DCMAKE_SYSTEM_NAME="WindowsStore" -DCMAKE_SYSTEM_VERSION="10.0" -DwxBUILD_TOOLKIT="uwp"**  
  

3) Build the generated solution.
