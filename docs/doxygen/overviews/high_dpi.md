High DPI Support in wxWidgets       {#overview_high_dpi}
=============================
[TOC]

Introduction                        {#high_dpi_intro}
============

Many modern displays have way more pixels on the same surface than used to be
the norm, resulting in much higher values of DPI (dots, i.e. pixels, per inch)
than the traditionally used values. This allows to render texts, or geometric
shapes in general much more smoothly.

As an illustration here are two scaled up views of the same text in 11 pt
Helvetica using up the same space on screen. First on an original Mac display
at 72 dpi, then on a High DPI Display, called "Retina" by Apple with twice as
many pixels in both dimensions (144 dpi), thus 4 times the number of pixels on
the same surface. Using these the contours are much more detailed.

![11 pt Helvetica at 72 DPI](overview_highdpi_text_72.png)

![11 pt Helvetica at 144 DPI](overview_highdpi_text_144.png)

To the user the DPI is typically expressed using a scaling factor, by which the
baseline DPI value is multiplied. For example, MSW systems may use 125% or 150%
scaling, meaning that they use DPI of 120 or 144 respectively, as baseline DPI
value is 96. Similarly, Linux systems may use "2x" scaling, resulting in DPI
value of 192. Macs are slightly different, as even they also may use "2x"
scaling, as in the example above, the effective DPI corresponding to it is 144,
as the baseline value on this platform is 72.


The Problem with High DPI Displays
----------------------------------

If high DPI displays were treated in the same way as normal ones, existing
applications would look tiny of them. For example, a square window 500 pixels
in size would take half of a standard 1920×1080 ("Full HD") display vertically,
but only a quarter on a 3840×2160 ("4K UHD") display. To prevent this from
happening, most platforms automatically scale the windows by the scaling
factor, defined above, when displaying them on high DPI displays. In this
example, scaling factor is 2 and so the actual size of the window on screen
would become 1000 when automatic scaling is in effect.

Automatic scaling is convenient, but doesn't really allow the application to
use the extra pixels available on the display. Visually, this means that the
scaled application appears blurry, in contrast to sharper applications using
the full display resolution, so a better solution for interpreting pixel values
on high DPI displays is needed: one which allows to scale some pixel values
(e.g. the total window size), but not some other ones (e.g. those used for
drawing, which should remain unscaled to use the full available resolution).


Pixel Values in wxWidgets           {#high_dpi_pixel_types}
=========================

Logical and Device-Independent Pixels {#high_dpi_lp_and_dip}
-------------------------------------

Some systems like eg Apple's OSes automatically scale all the coordinates by
the DPI scaling factor, however not all systems supported by wxWidgets do it --
notably, MSW does not. This means that **logical pixels**, in which all
coordinates and sizes are expressed in wxWidgets API, do _not_ have the same
meaning on all platforms when using high DPI displays. So while on macOS you
can always pass in a size of (500,500) to create the window from the previous
paragraph, whatever the resolution of the display is, you would have to
increase this to (1000,1000) on MSW when working on a 200% display. To hide
this difference from the application, wxWidgets provides **device-independent
pixels**, abbreviated as "DIP", that are always of the same size on all
displays and all platforms.

Thus, the first thing do when preparing your application for high DPI support
is to stop using raw pixel values, as they mean different things under
different platforms when DPI scaling is used. Actually, using any pixel values
is not recommended and replacing them with the values based on the text
metrics, i.e. obtained using wxWindow::GetTextExtent(), or expressing them in
dialog units (see wxWindow::ConvertDialogToPixels()) is preferable. However
the simplest change is to just replace the pixel values with the values in
DIP: for this, just use wxWindow::FromDIP() to convert from one to the other.

For example, if you have the existing code:
~~~{cpp}
myFrame->SetClientSize(wxSize(400, 300));
~~~
you can just replace it with
~~~{cpp}
myFrame->SetClientSize(myFrame->FromDIP(wxSize(400, 300)));
~~~
although replacing it with something like
~~~{cpp}
const wxSize sizeM = myFrame->GetTextExtent("M");
myFrame->SetClientSize(100*sizeM.x, 40*sizeM.y));
~~~
might be even better.

In any case, remember that window and wxDC or wxGraphicsContext coordinates
must be in logical pixels that can depend on the current DPI scaling, and so
should never be fixed at compilation time.


Physical Pixels                     {#high_dpi_pp}
---------------

In addition to (logical) pixels and DIPs discussed above, you may also need to
work in physical pixel coordinates, corresponding to the actual display pixels.
Physical pixels are never scaled, on any platform, and are used for the real
bitmap sizes. They are also used for drawing operations on wxGLCanvas, which
is _different_ from wxDC and wxGraphicsContext that use logical pixels.

Under MSW physical pixels are same as logical ones, but when writing portable
code you need to convert between logical and physical pixels using
wxWindow::GetContentScaleFactor(): this function returns a value greater than
or equal to 1 (and always just 1 under MSW), so a value in logical pixels
needs to be multiplied by it in order to obtain the value in physical pixels.

For example, in a wxGLCanvas created with the size of 100 (logical) pixels, the
rightmost physical pixel coordinate will be `100*GetContentScaleFactor()`
under all platforms.

You can convert from DIPs to physical pixels by converting DIPs to the logical
pixels first, but you can also do it directly, by using
wxWindow::GetDPIScaleFactor(). This function can return a value different from
1 even under MSW, i.e. it returns DPI scaling for physical display pixels.

Summary of Different Pixel Kinds    {#high_dpi_pixel_conversions}
--------------------------------

Under MSW, logical pixels are always the same as physical pixels, but are
different from DIPs, while under all the other platforms with DPI scaling
support (currently only GTK 3 and macOS), logical pixels are the same as DIP,
but different from physical pixels.

However under all platforms the following functions can be used to convert
between different kinds of pixels:

* From DIP to logical pixels: use wxWindow::FromDIP() or wxWindow::ToDIP().
* Logical pixels and physical pixels: multiply or divide by wxWindow::GetContentScaleFactor().
* From DIP to physical pixels: multiply or divide by wxWindow::GetDPIScaleFactor().

Or, in the diagram form:

@dot
digraph Pixels
{
    node [shape = hexagon, style = filled, fontname = Helvetica];

    LP  [fillcolor = lightblue, label = "Logical\nPixels"];
    DIP [fillcolor = yellow, label = "DI\nPixels"];
    PP  [fillcolor = green, label = "Physical\nPixels"];

    LP  -> PP [fontname = Helvetica, labeldistance = 5, labelangle = 30, dir = both, weight = 2, minlen = 3, label = "GetContentScaleFactor()", headlabel = "multiply by", taillabel = "divide by"];
    LP -> DIP [fontname = Helvetica, labeldistance = 6, dir = both, weight = 2, minlen = 3, headlabel = "ToDIP()", taillabel = "FromDIP()"];
    DIP -> PP [fontname = Helvetica, dir = both, minlen = 10, label = "GetDPIScaleFactor()" headlabel = "multiply by", taillabel = "divide by", constraint = false] ;
}
@enddot


High-Resolution Images and Artwork  {#high_dpi_artwork}
==================================

In order to benefit from the increased detail on High DPI devices you might want
to provide the images or artwork your application uses in higher resolutions as
well. Note that it is not recommended to just provide a high-resolution version
and let the system scale that down on 1x displays. Apart from performance
consideration also the quality might suffer, contours become more blurry.

You can use vector based graphics like SVG or you can add the same image at different
sizes / resolutions.

[comment]: # (TODO: API and Use Cases)

Platform-Specific Build Issues      {#high_dpi_platform_specific}
==============================

Generally speaking, all systems handle applications not specifically marked as
being "DPI-aware" by emulating low-resolution display for them and scaling them
up, resulting in blurry graphics and fonts, but globally preserving the
application appearance. For the best results, the application needs to be
explicitly marked as DPI-aware in a platform-dependent way.

MSW                                 {#high_dpi_platform_msw}
---

The behaviour of the application when running on a high-DPI display depends on
the values in its [manifest][1]. You may either use your own manifest, in which
case you need to define the `dpiAware` (for compatibility with older OS
versions) and `dpiAwareness` (for proper per-monitor DPI support) in it, or
simply include `wx/msw/wx.rc` from your resource file to use the manifest
provided by wxWidgets and predefine `wxUSE_DPI_AWARE_MANIFEST` to opt-in into
[high DPI support][2]: define it as `1` for minimal DPI awareness and `2` for
full, per-monitor DPI awareness supported by Windows 10 version 1703 or later.

[1]: https://docs.microsoft.com/en-us/windows/win32/sbscs/application-manifests
[2]: https://docs.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows

macOS                               {#high_dpi_platform_mac}
-----

DPI-aware applications must set their `NSPrincipalClass` to `wxNSApplication`
(or at least `NSApplication`) in their `Info.plist` file. Also see Apple [high
resolution guidelines][3] for more information.

[3]: https://developer.apple.com/library/archive/documentation/GraphicsAnimation/Conceptual/HighResolutionOSX/Explained/Explained.html
