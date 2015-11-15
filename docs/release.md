Welcome to wxWidgets, a free and open source cross-platform C++ framework for
writing advanced GUI applications using native controls.

Please notice that since the 2.9 series, only a single source distribution is
provided instead of the multiple distributions containing only the files for
each port as was done before. So you only need to download wxWidgets-3.0.0.zip
(or .7z for much smaller archive size) for Microsoft Windows systems or
wxWidgets-3.0.0.tar.bz2 for Unix ones, including Mac OS X. The only difference
between these files is that the ZIP (and 7z) archives contain files with DOS
line endings while the tar balls contain files with Unix line endings.

## Reporting Problems

Please report any bugs to the [issue tracker](http://trac.wxwidgets.org/report)
or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following sha1sums:

```
ceac35caeab6a0d3bdc6715b23dde82812ed8f7f  wxMSW-3.0.2-Setup.exe
8cc75cc76aca18cdfbf66283015af620b5457014  wxWidgets-3.0.2.7z
6461eab4428c0a8b9e41781b8787510484dea800  wxWidgets-3.0.2.tar.bz2
e9e5aeb0cabd998db737ffb0af165e7a8e02d68f  wxWidgets-3.0.2.zip
9db3068925a7c928062e3db59ab1aca5b0e654a9  wxWidgets-3.0.2-docs-html.tar.bz2
a58c1625679d78ec20cea4542dc1382b6a7a01e8  wxWidgets-3.0.2-docs-html.zip
591e706da0d2358ff392adaff706c9df2bd42a10  wxWidgets-3.0.2-docs-chm.zip
7670804d478a5b7008f5d7a82c30f0c6bfd358bf  wxWidgets-3.0.2_headers.7z
```

## Binaries

We have provided pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0 and 12.0 corresponding
  to marketing product names of Microsoft Visual Studio 2008, 2010, 2012 and
  2013 respectively.
* MinGW-TDM versions 4.7.1 and 4.8.1 (with the default SJLJ exceptions
  propagation method).

### For Developers

For developing applications with wxWidgets you need to download the
compiler-independent `wxWidgets-3.0.2_Headers.7z` file and one of
`wxMSW-3.0.2-vcXXX_Dev.7z` or `wxMSW-3.0.2_gccXXX_Dev.7z` files depending on
your compiler, its version and the target architecture (x86 if not specified or
x64).

Unpack both files into the same directory so that `include` and `lib`
directories are at the same level after unpacking. You should be able to compile
and link applications using wxWidgets in both debug and release modes but the
debug symbols are provided only for debug libraries in this archive, see below
for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.0.2_vcXXX_ReleaseDLL.7z` or
`wxMSW-3.0.2_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for
running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.0.2_vcXXX_ReleasePDB.7z` contain
  the debug symbols for the release build of the DLLs. Download them if you want
  to debug your own applications in release build or if you want to get
  meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release
  build of the DLLs (only the debug versions of the DLLs contains the debug
  symbols).

### Binary File Download Verification

To verify your download please use the following sha1sums:

```
d4de907d5cbbc6feabec3ae06b7cb875f2f552f6  wxMSW-3.0.2_gcc471TDM_Dev.7z
55ff0cd21df57ad6d25895964c13e421f86c0781  wxMSW-3.0.2_gcc471TDM_ReleaseDLL.7z
5856429ba46f29689e95a86b97a741ed42ebad98  wxMSW-3.0.2_gcc471TDM_x64_Dev.7z
6c4289652aa06b1ff2e28e72250c3ecd61b36acd  wxMSW-3.0.2_gcc471TDM_x64_ReleaseDLL.7z
e9e09cfe9c64c528a4e5b825f1d4eb93d41da490  wxMSW-3.0.2_gcc481TDM_Dev.7z
1f5e03d3e07812270fdd15f7ea47ccd43cb41f01  wxMSW-3.0.2_gcc481TDM_ReleaseDLL.7z
ff2a74d4298f58f7a1f9412ee632c8c695ca7efe  wxMSW-3.0.2_gcc481TDM_x64_Dev.7z
713860af5d001e3affdfae4003c3479fcceae72d  wxMSW-3.0.2_gcc481TDM_x64_ReleaseDLL.7z
c675da612bc702c66cc985ac02262a0529df34b0  wxMSW-3.0.2_vc90_Dev.7z
2ee0d4c15bf4453ad5f3c3656a5a76bb489ed267  wxMSW-3.0.2_vc90_ReleaseDLL.7z
1006789fdff272058803c28ec77aff328dab2de2  wxMSW-3.0.2_vc90_ReleasePDB.7z
1c815bafd9e8506549800354e44b22af60631941  wxMSW-3.0.2_vc90_x64_Dev.7z
2830484c43193e72f994adc3601438fde354fb4c  wxMSW-3.0.2_vc90_x64_ReleaseDLL.7z
9a1e87141345c818f58e523766c5e42ab9ecfb02  wxMSW-3.0.2_vc90_x64_ReleaseDLL.msi
1c9094ab4fa93875477ceb99d72db2a904c48136  wxMSW-3.0.2_vc90_x64_ReleasePDB.7z
5d1c2ad76c85bba32cc6329446bc70632af245ae  wxMSW-3.0.2_vc90_x86_ReleaseDLL.msi
d51026d681916f86cd34885cccc4408e8b0b3962  wxMSW-3.0.2_vc100_Dev.7z
f1fd09ab3ced110afd0ecaa191117533981928ed  wxMSW-3.0.2_vc100_ReleaseDLL.7z
da515bdf35a2aa8972614865ab4687da4727fae7  wxMSW-3.0.2_vc100_ReleasePDB.7z
deec659658076e46e794d73b2b4f4cf812a27278  wxMSW-3.0.2_vc100_x64_Dev.7z
7c9c0a710112f993655a0b51a9dc63206897d553  wxMSW-3.0.2_vc100_x64_ReleaseDLL.7z
ea22edf6983a282513571ee2901ed18035aadf80  wxMSW-3.0.2_vc100_x64_ReleaseDLL.msi
1f2eeecdf2355f89c70fa94c364afd3267b1f51f  wxMSW-3.0.2_vc100_x64_ReleasePDB.7z
00b47fc629868d3634e13eefc4adc08d6e822853  wxMSW-3.0.2_vc100_x86_ReleaseDLL.msi
33e9e9216b1c0fafc44fd681520a746465eb30d6  wxMSW-3.0.2_vc110_Dev.7z
e38f4f7c00ba58bf90afa53474ca0fdafa8ea791  wxMSW-3.0.2_vc110_ReleaseDLL.7z
d5ce2cf60829b4fda83439975ff4f7d857dda92d  wxMSW-3.0.2_vc110_ReleasePDB.7z
46e5a66a65db34107a089d1aef326c25cafbe7af  wxMSW-3.0.2_vc110_x64_Dev.7z
fff956c21826e5268b1533fc0b4fad6016963be0  wxMSW-3.0.2_vc110_x64_ReleaseDLL.7z
2c3b6f6e63a40027bca9601d7668999dec596013  wxMSW-3.0.2_vc110_x64_ReleaseDLL.msi
8a0c6e9bd844e204e26955573049fb17ce704d60  wxMSW-3.0.2_vc110_x64_ReleasePDB.7z
98ac8e4163d972610267fa8e070adb9b5b4e1f13  wxMSW-3.0.2_vc110_x86_ReleaseDLL.msi
87bee27e5b15852cb81862bb5245c751a70ee9ac  wxMSW-3.0.2_vc120_Dev.7z
f1cc74b0a558af80ceac350a5777a4a54d226315  wxMSW-3.0.2_vc120_ReleaseDLL.7z
cb4e92c6bbd4a25b7d92f8490dd85482e1d32c7d  wxMSW-3.0.2_vc120_ReleasePDB.7z
f7c8dad92deed936faecc8b59e13a6ac1c4fd4e4  wxMSW-3.0.2_vc120_x64_Dev.7z
77f2ac56edf5739ed7af261698069776cfef86d5  wxMSW-3.0.2_vc120_x64_ReleaseDLL.7z
4a33022c59f484d4da6979581e0ba409c804764f  wxMSW-3.0.2_vc120_x64_ReleaseDLL.msi
51e047f1576a61d5b5e019b6579b12ab4bdfb941  wxMSW-3.0.2_vc120_x64_ReleasePDB.7z
a31f3af0de6dd331dd0984ebaeef643f3f856aba  wxMSW-3.0.2_vc120_x86_ReleaseDLL.msi
```
