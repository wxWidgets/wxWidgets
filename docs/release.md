Welcome to wxWidgets, a free and open source cross-platform C++ framework for writing advanced GUI applications using native controls.

Please notice that since the 2.9 series, only a single source distribution is provided instead of the multiple distributions containing only the files for each port as was done before. So you only need to download wxWidgets-3.0.0.zip (or .7z for much smaller archive size) for Microsoft Windows systems or wxWidgets-3.0.0.tar.bz2 for Unix ones, including Mac OS X. The only difference between these files is that the ZIP (and 7z) archives contain files with DOS line endings while the tar balls contain files with Unix line endings.

See [README](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.0.3/docs/readme.txt) for more information about this release and the [change log](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.0.3/docs/changes.txt) for details of the changes in it.

## Reporting Problems

Please report bugs to the [issue tracker](http://trac.wxwidgets.org/newticket) or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following sha1sums:

```
b7a65ea3d3df02173fa9973f2f696b4e8110d14f  wxMSW-3.0.3-Setup.exe
9c818ae6a08a4094d323abd5ff8afc9f52b64e95  wxWidgets-3.0.3-docs-chm.zip
4b77c04f2a9d811e3d43dae8e8640998ca04b647  wxWidgets-3.0.3-docs-html.tar.bz2
f175080da62390510034df55fdb7b13c8010b228  wxWidgets-3.0.3-docs-html.zip
12adc4545f4ec90b026d956b40d05a6f9c0a59b8  wxWidgets-3.0.3-headers.7z
7836c1b5f22871080de538446aaa26044e387e9c  wxWidgets-3.0.3.7z
3525306c926e208d9b0272aaa9c091b8c7264e5b  wxWidgets-3.0.3.tar.bz2
6b5ef065e6a8b6b010a9d92f344a423ecd9fb711  wxWidgets-3.0.3.zip
```

## Binaries

We have provided pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0 and 14.1, corresponding to marketing product names of Microsoft Visual Studio 2008, 2010, 2012, 2013, 2015 and 2017 respectively.
* MinGW-TDM versions 4.9.2 and 5.1.0 (with the default SJLJ exceptions propagation method).

### For Developers

For developing applications with wxWidgets you need to download the compiler-independent `wxWidgets-3.0.3_Headers.7z` file and one of `wxMSW-3.0.3-vcXXX_Dev.7z` or `wxMSW-3.0.3_gccXXX_Dev.7z` files depending on your compiler, its version and the target architecture (x86 if not specified or x64).

Unpack both files into the same directory so that `include` and `lib` directories are at the same level after unpacking. You should be able to compile and link applications using wxWidgets in both debug and release modes but the debug symbols are provided only for debug libraries in this archive, see below for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.0.3_vcXXX_ReleaseDLL.7z` or `wxMSW-3.0.3_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.0.3_vcXXX_ReleasePDB.7z` contain the debug symbols for the release build of the DLLs. Download them if you want to debug your own applications in release build or if you want to get meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release build of the DLLs (only the debug versions of the DLLs contains the debug symbols).

### Binary File Download Verification

To verify your download please use the following sha1sums:

```
5bc5f4cdb5e54daf5b17f57a9581c812119f8f0c  wxMSW-3.0.3_gcc492TDM_Dev.7z
46d8cbe1c8e0aedfb842e86a786ccca58cd4adb3  wxMSW-3.0.3_gcc492TDM_ReleaseDLL.7z
bd06cd7779b46b30e463c8b32660fc486cf3bcd4  wxMSW-3.0.3_gcc492TDM_x64_Dev.7z
c63a4bae088900b07fb41a07ed30e8c1f0ae1348  wxMSW-3.0.3_gcc492TDM_x64_ReleaseDLL.7z
eca596c6f2dabd0b04a5011905f3bfb0f2ae4f35  wxMSW-3.0.3_gcc510TDM_Dev.7z
5dc49aff237a87a2f34a2246963f08482c0ff0fd  wxMSW-3.0.3_gcc510TDM_ReleaseDLL.7z
4bbdddbcd10121100407969d1e07367b1cbedd5c  wxMSW-3.0.3_gcc510TDM_x64_Dev.7z
52180553267525bacdde154098a1b1b44c4938fe  wxMSW-3.0.3_gcc510TDM_x64_ReleaseDLL.7z
69d5d8d93d2409856137602863c2766f96553093  wxMSW-3.0.3_vc90_Dev.7z
71178e069594b7c194a6f1506bc8aad7165bf627  wxMSW-3.0.3_vc90_ReleaseDLL.7z
b04916df6e2aeb6ab0f0ef49ecac5d5927f0159e  wxMSW-3.0.3_vc90_ReleasePDB.7z
13ef53b4647607fb04e8e70ab52a4189247e4efe  wxMSW-3.0.3_vc90_x64_Dev.7z
55c3778d2312740391cd4081a608c0d3261905e4  wxMSW-3.0.3_vc90_x64_ReleaseDLL.7z
a7264b71662c36c8a7167e23c8724f9d608359af  wxMSW-3.0.3_vc90_x64_ReleasePDB.7z
5fb26a0935e5c5786bec26aaad4156388fc72b85  wxMSW-3.0.3_vc100_Dev.7z
544926ea1546d03e091cad6ce19483d99d2e0b04  wxMSW-3.0.3_vc100_ReleaseDLL.7z
2697fa5d1f79ab9ee6dce3ec5f870f0ebb74a539  wxMSW-3.0.3_vc100_ReleasePDB.7z
96f7aa82d3bf862fb4f7e3959a90d27093bba3d8  wxMSW-3.0.3_vc100_x64_Dev.7z
745d3c0a8de3c446de35d755f9f79b515524efeb  wxMSW-3.0.3_vc100_x64_ReleaseDLL.7z
56b29284868448494b5b02cf64a87ec180fa4eb7  wxMSW-3.0.3_vc100_x64_ReleasePDB.7z
cf613014fc50f3ea2bf1b2813a8b243f1c9a7db2  wxMSW-3.0.3_vc110_Dev.7z
5c65de8530d696ffb70b026dbe49a81d824eb4de  wxMSW-3.0.3_vc110_ReleaseDLL.7z
c1da975be11c89ff1d43d87c0b77613e922b2acd  wxMSW-3.0.3_vc110_ReleasePDB.7z
0ce2c02d6238693228438b2e8a990f7399e52970  wxMSW-3.0.3_vc110_x64_Dev.7z
e8a5c432fc574735c6311c7522fbf79faa3f9d53  wxMSW-3.0.3_vc110_x64_ReleaseDLL.7z
1b1b0224449cfac4427be3e528912e7100075ea3  wxMSW-3.0.3_vc110_x64_ReleasePDB.7z
92f282a3f6dfa9eacdd207e58f35253dc77c274c  wxMSW-3.0.3_vc120_Dev.7z
994ae7ada1d7a923ed929fe0595e23fed140ab98  wxMSW-3.0.3_vc120_ReleaseDLL.7z
357bbe8d534ad0e5ed3c73b1e34241c7f15f3fe6  wxMSW-3.0.3_vc120_ReleasePDB.7z
4b286f8b79267187248b0ff8568c8e43438b3e0d  wxMSW-3.0.3_vc120_x64_Dev.7z
519126b3c529a60f0654b51bec2910a7984006ff  wxMSW-3.0.3_vc120_x64_ReleaseDLL.7z
81f562db75645f27137f51a493aa521de3085dc7  wxMSW-3.0.3_vc120_x64_ReleasePDB.7z
30cc206c9a9b1f5ea38dca0e9af31ed38cd1e1f5  wxMSW-3.0.3_vc140_Dev.7z
f7cca6f35df05c62bc0ad4f0ed6c6e326df1f05e  wxMSW-3.0.3_vc140_ReleaseDLL.7z
09d251e1f4ed46f95bc6e3eb5d94d4984080d33a  wxMSW-3.0.3_vc140_ReleasePDB.7z
bf30f907ba51a54ac0719adff3486bb22ab3673d  wxMSW-3.0.3_vc140_x64_Dev.7z
6c892db45ec3b0f8e8722b5e60d46dfcd878249c  wxMSW-3.0.3_vc140_x64_ReleaseDLL.7z
e512c449bcc1df195b871b59ae682b396a2a66e7  wxMSW-3.0.3_vc140_x64_ReleasePDB.7z
1e44739a47bf0a7b56bf1e31c75aae680402f2ec  wxMSW-3.0.3_vc141_Dev.7z
af2f7d3423ed78ea2526bf8a386170c7a4de5d52  wxMSW-3.0.3_vc141_ReleaseDLL.7z
f803d6e5d53a1ad52ce07dab5f7869bd2c9a3f0a  wxMSW-3.0.3_vc141_ReleasePDB.7z
87ed0a9c0dc7c5924d2d02afc622988c02d3e45d  wxMSW-3.0.3_vc141_x64_Dev.7z
cd5f2be45ec881ff1651aca6aa9333266978ff47  wxMSW-3.0.3_vc141_x64_ReleaseDLL.7z
d66dbfb990481910350547fbc3da01e910eab7b1  wxMSW-3.0.3_vc141_x64_ReleasePDB.7z
```
