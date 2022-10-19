/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_font wxFont Overview

@tableofcontents

A font is an object which determines the appearance of text, primarily when
drawing text to a window or device context. A font is determined by the
following parameters (not all of them have to be specified, of course):

@beginDefList
@itemdef{Point size, This is the standard way of referring to text size.}
@itemdef{Family,
        Supported families are:
        @b wxDEFAULT, @b wxDECORATIVE, @b wxROMAN, @b wxSCRIPT, @b wxSWISS, @b wxMODERN.
        @b wxMODERN is a fixed pitch font; the others are either fixed or variable pitch.}
@itemdef{Style, The value can be @b wxNORMAL, @b wxSLANT or @b wxITALIC.}
@itemdef{Weight, The value can be @b wxNORMAL, @b wxLIGHT or @b wxBOLD.}
@itemdef{Underlining, The value can be @true or @false.}
@itemdef{Face name,
        An optional string specifying the actual typeface to be used. If @NULL,
        a default typeface will chosen based on the family.}
@itemdef{Encoding,
        The font encoding (see @b wxFONTENCODING_XXX
        constants and the @ref overview_fontencoding for more details)}
@endDefList

Specifying a family, rather than a specific typeface name, ensures a degree of
portability across platforms because a suitable font will be chosen for the
given font family, however it doesn't allow to choose a font precisely as the
parameters above don't suffice, in general, to identify all the available fonts
and this is where using the native font descriptions may be helpful - see
below.

Under Windows, the face name can be one of the installed fonts on the user's
system. Since the choice of fonts differs from system to system, either choose
standard Windows fonts, or if allowing the user to specify a face name, store
the family name with any file that might be transported to a different Windows
machine or other platform.

@see wxFont, wxFontDialog

@note There is currently a difference between the appearance of fonts on the
      two platforms, if the mapping mode is anything other than wxMM_TEXT.
      Under X, font size is always specified in points. Under MS Windows, the
      unit for text is points but the text is scaled according to the current
      mapping mode. However, user scaling on a device context will also scale
      fonts under both environments.



@section overview_font_nativeinfo Native Font Information

An alternative way of choosing fonts is to use the native font description.
This is the only acceptable solution if the user is allowed to choose the font
using the wxFontDialog because the selected font cannot
be described using only the family name and so, if only family name is stored
permanently, the user would almost surely see a different font in the program
later.

Instead, you should store the value returned by wxFont::GetNativeFontInfoDesc and pass
it to wxFont::SetNativeFontInfo later to recreate exactly the same font.

Note that the contents of this string depends on the platform and shouldn't be
used for any other purpose (in particular, it is not meant to be shown to the
user). Also please note that although the native font information is currently
implemented for Windows and Unix (GTK and X11) ports only, all the methods
are available for all the ports and should be used to make your program work
correctly when they are implemented later.

@section overview_font_privateinfo Private font information

Sometimes an application needs fonts that are not globally installed on the
system. On Macintosh/OSX this can be arranged by placing the desired fonts
within the Application Bundle in Contents/Resources/Fonts and using
the ATSApplicationFontsPath key to point there. The full details of the
procedure there can be found as OSX developer resources. For the GTK+ and
Windows ports it is possible to add TrueType fonts from arbitrary locations at
run-time using wxFont::AddPrivateFont(). Notice that under MSW this function
should be called before creating the first wxGraphicsContext object if you want
the private font to be usable from it.

*/
