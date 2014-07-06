# wxWebViewChromium

wxWebViewChromium is a Chromium backend for wxWebView using the Chromium
Embedded Framework(CEF3).

## Requirements

[Chromium Embedded Framework][1]: 3.1750.1738

* Windows: 3.1750.1738 or 3.1650.1562
* Linux: 3.1750.1738 or 3.1650.1562
* Mac OS X: 3.1750.1738

On Windows platform, you will need to change the vistual stdio building
properties of `libcef_dll_wrapper` to consistent with wxWidgets, see following steps:

  * C/C++ - General - Treat Warngings as Error - No
  * C/C++ - Code Generation - Runtime Library - Multithreaded [Debug] DLL
  * C/C++ - Code Generation - Enable C++ Exceptions - Yes
  * C/C++ - Language - Enable Run-Time Type Information - Yes

For more details, you can refer to [wxWebViewChromium's wiki](https://github.com/wxWidgets/wxWebViewChromium/wiki/Windows-Build). 

## Building Instructions 

wxWebViewChromium supports Windows/Linux/Mac OS X platforms.

###Common Steps

1. Clone the repository by running `git clone https://github.com/hokein/wxWidgets.git` 

2. Copy the CEF3 directory to wxWidgets src directory(wxWidgets sets `<wxWidgets_src>/cef` as a default library).

###Windows Platform

By default, wxWidgets provides Visual Stdio 2010 project file to build wxWebViewChromium, and set Chromium Embedded Framework in `<wxWidgets_src>/cef` directory.

3. Open `build/msw/wc_vc10.sln` project file in Visual Stdio 2010, and build `wxWidgets` library. 

4. Open `samples/webview_chromium/webview_chromium_vc10.vcxproj` file in Visual Stdio 2010, and build it.

5. Copy CEF3 related resources(libcef.dll, libffmpegsumo.so, locales/\*, cef.apk, devtools_resources.pak) to webview_chromium binary directory.


###Linux Platform

3. Regenerate the building files by running `bakefile_gen -f autoconf` command under `build/bakefiles` directory.

4. Run `./autogen.sh` under `<wxWidgets_src>`.

5. Run `./configure --enable-webview --enable-webviewwebkit` and `make` commands.

6. Copy CEF3 related resources as above step5 in windows Platform.

7. Add `libcef.so` directory path to `LD_LIBRARY_PATH`(`export LD_LIBRARY_PATH=<libcef_directory>`),
otherwise webview_chromium will not find `libcef.so`.


###Mac OS X Platform

wxWidgets provides xcode building files to build wxWebViewChromium, supports OS X 10.8 or above.

1. Open `samples/webview_chromium/webview_chromium.xcodeproj`, and build `webview_chromium` sample target. 


##Notes

wxWidgets will set `wxWidgets_src/cef` directory as default CEF path.  If you want to specify your custom CEF path, please follow the steps:

1. Specify your CEF path to `CEF_INCLUDE_DIR` variable in `build/bakefiles/config.bkl` file.

2. Regenerate the building files by running `bakefile_gen -f <formats>`.

[1]: http://code.google.com/p/chromiumembedded/
