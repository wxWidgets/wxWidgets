# wxWebViewChromium

wxWebViewChromium is a Chromium backend for wxWebView using the Chromium
Embedded Framework(CEF3).

## Requirements

* [Chromium Embedded Framework][1]: 3.1650.1562.

## Building Instructions 

Currently wxWebViewChromium only supports on Windows/Linux platforms.

1. Clone the repository by running `git clone https://github.com/hokein/wxWidgets.git` 

2. Download CEF3 from [1], and put the directory in wxWidgets src directory(`wxWidgets_src/cef`).

3. Running `cd wxWidgets_src/cef/` and build `libcef_dll_wrapper` library.


* Windows: Build `build/msw/wx_vc9.sln` and `samples/webview_chromium/webview_chromium_vc9.sln` solution.

* Linux: Running `./configure --enable-webview --enable-webviewwebkit` and `make` commands under wxWidgets src directory.

Notes: wxWidgets will set `wxWidgets_src/cef` directory as default CEF path.
If you want to specify your custom CEF path, please follow the steps:

1. Specify your CEF path to `CEF_INCLUDE_DIR` variable in `build/bakefiles/config.bkl`.

2. Regenerate the build files by running `bakefile_gen -f msvs2008prj`/`bakefile_gen -f autoconf` command under `build/bakefiles` directory.


[1]: http://code.google.com/p/chromiumembedded/
