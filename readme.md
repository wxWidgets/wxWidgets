# wxWebViewChromium

wxWebViewChromium is a Chromium backend for wxWebView using the Chromium
Embedded Framework(CEF3).

## Requirements

* [Chromium Embedded Framework][1]: 3.1650.1562.

1. Download CEF3 from [1], and build `libcef_dll_wrapper` static library.

## Building Instructions 

Currently wxWebViewChromium only supports on Windows/Linux platforms.

###Common Steps

1. Clone the repository by running `git clone https://github.com/hokein/wxWidgets.git` 

2. Copy the CEF3 directory to wxWidgets src directory(wxWidgets sets `<wxWidgets_src>/cef` as a default library).

3. Regenerate the build files by running `bakefile_gen -f msvs2008prj`/`bakefile_gen -f autoconf` command under `build/bakefiles` directory.

###Windows Platform

4. Build `build/msw/wx_vc9.sln` and `samples/webview_chromium/webview_chromium_vc9.sln` solution.

5. Copy CEF3 related resources(libcef.dll, libffmpegsumo.so, locales/*, cef.apk, devtools_resources.pak) to webview_chromium binary directory.

###Linux Platform

4. Run `./autogen.sh` under `<wxWidgets_src>`.

5. Run `./configure --enable-webview --enable-webviewwebkit` and `make` commands.

6. Copy CEF3 related resources as above step5 in windows Platform.

7. Add `libcef.so` directory path to `LD_LIBRARY_PATH`(`export LD_LIBRARY_PATH=<libcef_directory>`),
otherwise webview_chromium will not find `libcef.so`.

Notes: wxWidgets will set `wxWidgets_src/cef` directory as default CEF path.
If you want to specify your custom CEF path, please follow the steps:

1. Specify your CEF path to `CEF_INCLUDE_DIR` variable in `build/bakefiles/config.bkl`.

2. Regenerate the build files as above step2.

[1]: http://code.google.com/p/chromiumembedded/
