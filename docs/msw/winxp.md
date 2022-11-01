Microsoft Windows Theme Support from wxWidgets {#plat_msw_winxp}
-------------------------------------------

Windows XP introduced the themes (called "visual styles" in the Microsoft
documentation) which have been used since then for Win32 controls.
As wxWidgets uses the standard Windows controls for most of its
classes, it can take advantage of it without (almost) any effort from your part.
The only thing you need to do if you want your program to honour the visual style is to
add the manifest file to your program (this is not at all specific to
wxWidgets programs but is required for all Windows applications).

wxWidgets now includes manifest resources in wx.rc, so it should be enough to
include "wx/msw/wx.rc" in your application's resource file and you get
proper look automatically. Notice that MSVS automatically generates the manifest
and embeds it in the executables it produces. Therefore, wxWidgets by default doesn't
use its own manifest when using MSVC (i.e., wxUSE_RC_MANIFEST is not defined as 1).
If you don't want to use wxWidgets manifest with any compiler you may define wxUSE_NO_MANIFEST
as 1 prior to including wx/msw/wx.rc.

wxWidgets offers three manifests, differing only in which DPI-awareness mode they use.
Which of the three is used depends on the value of wxUSE_DPI_AWARE_MANIFEST define.
See more in @ref high_dpi_platform_msw "MSW Platform-Specific Build Issues" the High DPI guide.

More information about application manifests is available at
https://learn.microsoft.com/en-us/windows/win32/controls/cookbook-overview#using-manifests-or-directives-to-ensure-that-visual-styles-can-be-applied-to-applications
