Welcome to wxWidgets, a free and open source cross-platform C++ framework for writing advanced GUI applications using native controls.

wxWidgets 3.1.0 is the first release in the new development 3.1.0 branch. This release is a "development" one as it makes (very few) incompatible API changes compared to 3.0 and does not guarantee the ABI stability, unlike the 3.0.x series. It is not inherently more buggy or less stable than the "stable" releases and you're encouraged to use it. If you're already using 3.0, upgrading shouldn't require any special effort, so please try it out.

Please notice that we provide a single source distribution containing files for all platforms, so you only need to download either the archive with the `.zip` (or `.7z` for much smaller size) extension for Microsoft Windows systems or the one with `.tar.bz2` extenstion for Unix ones, including Mac OS X.

See [README](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.0/docs/readme.txt) for more information about this release and the [change log](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.0/docs/changes.txt) for details of the changes in it.

## Errata

A few problems have unfortunately sneaked into this release:

* Creating `wxComboCtrl` without `wxTE_PROCESS_ENTER` style results in an assert. Solution: specify this style when creating it or apply this [one line fix](https://github.com/wxWidgets/wxWidgets/commit/cee3188c1abaa5b222c57b87cc94064e56921db8).
* Building under OS X in C++11 mode for i386 architecture (but not amd64) results in an error about narrowing conversion. Solution: apply this [two character fix](https://github.com/wxWidgets/wxWidgets/commit/ee486dba32d02c744ae4007940f41a5b24b8c574).
* Building under OS X in C++11 results in several `-Winconsistent-missing-override` warnings. Solution: we have a [fix for this](https://github.com/wxWidgets/wxWidgets/commit/173ecd77c4280e48541c33bdfe499985852935ba) too.

## Reporting Problems

Please report bugs to the [issue tracker](http://trac.wxwidgets.org/newticket) or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following SHA-1 checksums:

    18963f168805a175a7b542020d6c354ea88e4407  wxMSW-3.1.0-Setup.exe
    984c8831d78a45345b510cabcd0162d3c88297c5  wxWidgets-3.1.0-docs-chm.zip
    9f3f918e3be1d571ac870fe45ab692ad41256cf6  wxWidgets-3.1.0-docs-html.tar.bz2
    2f38fb5166c66136b72b2956b6f76c20350fb5b7  wxWidgets-3.1.0-docs-html.zip
    c1b33a1e9c245b76031461b8cdc1fea8f6c68904  wxWidgets-3.1.0-headers.7z
    daf03ed0006e41334f10ceeb3aa2d20c63aacd42  wxWidgets-3.1.0.7z
    2170839cfa9d9322e8ee8368b21a15a2497b4f11  wxWidgets-3.1.0.tar.bz2
    49ca4625b943cc0ec8addd7583a22db742fa716c  wxWidgets-3.1.0.zip

## Binaries

We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0 and 14.0
  (corresponding to marketing product names of Microsoft Visual Studio 2008,
   2010, 2012, 2013 and 2015 respectively).
* MinGW-TDM versions 4.9 and 5.1 (with the default SJLJ exceptions propagation
  method, using C++11).

### For Developers

For developing applications with wxWidgets you need to download the compiler-independent `wxWidgets-3.1.0_Headers.7z` file and one of `wxMSW-3.1.0-vcXXX_Dev.7z` or `wxMSW-3.1.0_gccXXX_Dev.7z` files depending on your compiler, its version and the target architecture (x86 if not specified or x64).

Unpack both files into the same directory so that `include` and `lib` directories are at the same level after unpacking. You should be able to compile and link applications using wxWidgets in both debug and release modes but the debug symbols are provided only for debug libraries in this archive, see below for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.1.0_vcXXX_ReleaseDLL.7z` or `wxMSW-3.1.0_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.1.0_vcXXX_ReleasePDB.7z` contain
  the debug symbols for the release build of the DLLs. Download them if you want
  to debug your own applications in release build or if you want to get
  meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release
  build of the DLLs (only the debug versions of the DLLs contains the debug
  symbols).

### Binary File Download Verification

To verify your download please use the following SHA-1 checksums:

    c64df074dd50490eeee77bcfc83aa3bbf942d3fa wxMSW-3.1.0_gcc492TDM_Dev.7z*
    43c5d694c8edf16bf32f8df843c9a7f53f85f2b1 wxMSW-3.1.0_gcc492TDM_ReleaseDLL.7z*
    c7e320b4acf0e7e75fddc822aba34ba8f597b967 wxMSW-3.1.0_gcc492TDM_x64_Dev.7z*
    d2d3d030fd8b6e50dc1425acc9a5bf408025ee46 wxMSW-3.1.0_gcc492TDM_x64_ReleaseDLL.7z*
    4e02601d8f08b6309d292eac1d9dc9abfb091289 wxMSW-3.1.0_gcc510TDM_Dev.7z*
    ed005763c51a8d60f2eaa5bf64a8defafbf5a528 wxMSW-3.1.0_gcc510TDM_ReleaseDLL.7z*
    5d79a0f0d1066c081fd3cf1cf7d40b03e70fd6c5 wxMSW-3.1.0_gcc510TDM_x64_Dev.7z*
    b2dbd576187022d2264cfa58bc19da376a9db060 wxMSW-3.1.0_gcc510TDM_x64_ReleaseDLL.7z*
    98181a39a900936f43f6463292226e0220b35c38 wxMSW-3.1.0_vc90_Dev.7z
    e8c6a9f38aa7e916fe6b0e3a5e52f149b97160b0 wxMSW-3.1.0_vc90_ReleaseDLL.7z
    94a0681aa23f5c52154c4e982a095b8333128ac3 wxMSW-3.1.0_vc90_ReleasePDB.7z
    4b8bf833735568bf19a120d86ffb88861ab9c806 wxMSW-3.1.0_vc90_x64_Dev.7z
    89dcf670a1329a1a0aa277f6cb683dfe7b321d5d wxMSW-3.1.0_vc90_x64_ReleaseDLL.7z
    10b22186fc38baa75795cd075b70a5888fe79774 wxMSW-3.1.0_vc90_x64_ReleasePDB.7z
    f8400a2b4d3fb23576dd8139073abdd1519ccf0a wxMSW-3.1.0_vc100_Dev.7z
    02c5b459b0618a6cf43a651cdf9d8f725c9350b6 wxMSW-3.1.0_vc100_ReleaseDLL.7z
    ac401f050a6f89277af2d7097f141db5c66a3007 wxMSW-3.1.0_vc100_ReleasePDB.7z
    3365875f016008a50fe91ac3d9e729db4b5b268c wxMSW-3.1.0_vc100_x64_Dev.7z
    938591b7e9145d6accbea4577a3ea568f828b5a0 wxMSW-3.1.0_vc100_x64_ReleaseDLL.7z
    ea97b44fd965c9053bfa0c82dda896053b2aa3be wxMSW-3.1.0_vc100_x64_ReleasePDB.7z
    2874ec7d17aebeae52667a95421c153dd0dc88b0 wxMSW-3.1.0_vc110_Dev.7z
    0cf63a1351dcfa4e3503da502b76d20ecdab2494 wxMSW-3.1.0_vc110_ReleaseDLL.7z
    dbadcf9bad27f80badd550cceed0c1510ae18d56 wxMSW-3.1.0_vc110_ReleasePDB.7z
    786e10cd425b793c894a3758cd60363c4d4cda8b wxMSW-3.1.0_vc110_x64_Dev.7z
    92ad06fc2ae5e33c861e5b4ebc936e8bedd61b81 wxMSW-3.1.0_vc110_x64_ReleaseDLL.7z
    a27d88b620619e48f175a3c4bd20c4966e000cc6 wxMSW-3.1.0_vc110_x64_ReleasePDB.7z
    7e0e7f7270b7db4a118cb01060be8b53d1c1c116 wxMSW-3.1.0_vc120_Dev.7z
    ec7f38a8aec8b07910cff379107ddbb116d7cf9e wxMSW-3.1.0_vc120_ReleaseDLL.7z
    c72582fd793aef3396a44658eb590937b624181c wxMSW-3.1.0_vc120_ReleasePDB.7z
    4e2c5bfd4b007318f320317e2d359ea62f48e8e8 wxMSW-3.1.0_vc120_x64_Dev.7z
    b4468fcee7e899de22b5076737e7f37142e8004a wxMSW-3.1.0_vc120_x64_ReleaseDLL.7z
    0640855c05a762b4505e5fe8eff6326802516b3e wxMSW-3.1.0_vc120_x64_ReleasePDB.7z
    5499214d5f37ecfe70725be4bbf1612ba235bb66 wxMSW-3.1.0_vc140_Dev.7z
    30394ed5b8fad0307c6dc1c12c47b8bd53214e87 wxMSW-3.1.0_vc140_ReleaseDLL.7z
    89912076136882dcec79eba8e27a111cbdaf20be wxMSW-3.1.0_vc140_ReleasePDB.7z
    a25b9e0e51552a5f48a7da723d0f19f23beb37d1 wxMSW-3.1.0_vc140_x64_Dev.7z
    4f2f6a6468cafb01d6048bbca4df6b9c829429d1 wxMSW-3.1.0_vc140_x64_ReleaseDLL.7z
    f21516e6d970b6874e335ccd6de3a1f57cb56329 wxMSW-3.1.0_vc140_x64_ReleasePDB.7z
