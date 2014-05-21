# wxWebViewChromium

wxWebViewChromium is a Chromium backend for wxWebView using the Chromium
Embedded Framework(CEF3).

## Requirements

* [Chromium Embedded Framework][1]: 3.1650.1562.

## Building Instructions 

Currently wxWebViewChromium only supports on Windows.

1. Clone the repository by running `git clone https://github.com/hokein/wxWidgets.git` 

2. Specify your CEF path to `CEF_INCLUDE_DIR` variable in `build/bakefiles/files.bkl` and `samples/webview_chromium/webview_chromium.bkl`

3. Regenerate the MSVC project files by running `bakefile_gen` command under `build/bakefiles` directory.

4. Since step3 only generates vc6~vc9 project files, for using VS2010 or upper, you need to upgrade the project.
(Use VS2010 open the project files and it will upgrade automatically.)

5. Build `build/msw/wx_vc9.sln` and `samples/webview_chromium/webview_chromium_vc9.sln` solution.

[1]: http://code.google.com/p/chromiumembedded/
