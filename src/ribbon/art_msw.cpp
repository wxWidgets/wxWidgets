///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_msw.cpp
// Purpose:     MSW style art provider for ribbon interface
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/ribbon/art_internal.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

static const char* const gallery_up_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "     ",
  "  x  ",
  " xxx ",
  "xxxxx",
  "     "};

static const char* const gallery_down_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "     ",
  "xxxxx",
  " xxx ",
  "  x  ",
  "     "};

static const char* const gallery_left_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "   x ",
  "  xx ",
  " xxx ",
  "  xx ",
  "   x "};

static const char* const gallery_right_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  " x   ",
  " xx  ",
  " xxx ",
  " xx  ",
  " x   "};

static const char* const gallery_extension_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "xxxxx",
  "     ",
  "xxxxx",
  " xxx ",
  "  x  "};

static const char* const panel_extension_xpm[] = {
  "7 7 2 1",
  "  c None",
  "x c #FF00FF",
  "xxxxxx ",
  "x      ",
  "x      ",
  "x  x  x",
  "x   xxx",
  "x   xxx",
  "   xxxx"};

static const char* const panel_toggle_down_xpm[] = {
  "7 9 2 1",
  "  c None",
  "x c #FF00FF",
  "       ",
  "x     x",
  "xx   xx",
  " xx xx ",
  "x xxx x",
  "xx x xx",
  " xx xx ",
  "  xxx  ",
  "   x   ",};

static const char* const panel_toggle_up_xpm[] = {
  "7 9 2 1",
  "  c None",
  "x c #FF00FF",
  "   x   ",
  "  xxx  ",
  " xx xx ",
  "xx x xx",
  "x xxx x",
  " xx xx ",
  "xx   xx",
  "x     x",
  "       ",};

static const char* const ribbon_toggle_pin_xpm[] = {
  "12 9 3 1",
  "  c None",
  "x c #FF00FF",
  ". c #FF00FF",
  "   xx       ",
  "   x.x   xxx",
  "   x..xxx..x",
  "xxxx.......x",
  "x..........x",
  "xxxx.......x",
  "   x..xxx..x",
  "   x.x   xxx",
  "   xx       "
};

static const char * const ribbon_help_button_xpm[] = {
"12 12 112 2",
"   c #163B95",
".  c none",
"X  c #1B3F98",
"o  c #1B4097",
"O  c #1D4198",
"+  c #1E4298",
"@  c #1E439B",
"#  c #1A419F",
"$  c #1E439D",
"%  c #204398",
"&  c #204399",
"*  c #25479B",
"=  c #25489A",
"-  c #284A9D",
";  c #2A4C9D",
":  c #30519E",
">  c #3B589A",
",  c #3D599B",
"<  c #1840A2",
"1  c #1E45A1",
"2  c #1E4AB4",
"3  c #2D4FA0",
"4  c #224AAC",
"5  c #254DAC",
"6  c #294FA9",
"7  c #2B52AE",
"8  c #3051A0",
"9  c #3354A0",
"0  c #3354A2",
"q  c #3454A3",
"w  c #3456A4",
"e  c #3556A4",
"r  c #3C5BA3",
"t  c #395AA6",
"y  c #3E5CA6",
"u  c #3E5DA7",
"i  c #3F5EA6",
"p  c #2A51B0",
"a  c #2E55B5",
"s  c #2752BA",
"d  c #3058B8",
"f  c #3F61B2",
"g  c #415FA7",
"h  c #4562A7",
"j  c #4864A7",
"k  c #4D67A5",
"l  c #4361A8",
"z  c #4361A9",
"x  c #4663A8",
"c  c #4563AA",
"v  c #4764AA",
"b  c #4B68AE",
"n  c #506AA8",
"m  c #516DAD",
"M  c #546EAC",
"N  c #5F75AB",
"B  c #5A72AC",
"V  c #5C77B6",
"C  c #6C7DA7",
"Z  c #6077AD",
"A  c #687DAF",
"S  c #637BB4",
"D  c #687FB7",
"F  c #2D59C1",
"G  c #2E5AC2",
"H  c #2F5ECE",
"J  c #3763CC",
"K  c #4169CB",
"L  c #7787AC",
"P  c #7E8CAE",
"I  c #7A8BB5",
"U  c #7B8CB4",
"Y  c #7C8FBD",
"T  c #758FCA",
"R  c #808CA8",
"E  c #969DAF",
"W  c #8291B4",
"Q  c #8A95B0",
"!  c #8B96B1",
"~  c #8F9AB3",
"^  c #8D98B5",
"/  c #8E9AB7",
"(  c #8997B8",
")  c #949EB9",
"_  c #99A1B4",
"`  c #ADAFB7",
"'  c #A5ABB8",
"]  c #A6ABB8",
"[  c #AAAFBE",
"{  c #AFB2BE",
"}  c #B0B1B6",
"|  c #BAB8B6",
" . c #B4B5BC",
".. c #B6B9BF",
"X. c #BBB9B8",
"o. c #8C9DC3",
"O. c #8EA3D4",
"+. c #97AAD4",
"@. c #ACB5C9",
"#. c #B3B7C0",
"$. c #A1B1D5",
"%. c #BAC3D7",
"&. c #BEC6D6",
"*. c #D7D2C7",
"=. c #C2C8D6",
"-. c #D2D6DF",
";. c #E8E4DA",
":. c #CED5E4",
">. c #FFF9EC",
",. c #F3F4F5",
"<. c #F6F8FB",
"1. c None",
/* pixels */
"1.1.1.1.#./ W ~ } 1.1.1.",
"1.1.1.U r c b t h Q 1.1.",
"1.1.A 3 $.<.,.&.m w ^ 1.",
"1.( 0 z :.%.=.;.) e x ` ",
"1.n u v M * B *.R O @ P ",
"' i z l - 9 { | > $ # Z ",
"_ y l ; & [ X., 1 6 4 D ",
"] g 8 o :  .C < 7 a s o.",
"1.k X % = I S 5 d G K ..",
"1.! .   j >.-.p F H +.1.",
"1.1.L X + Y V 2 J O.1.1.",
"1.1.1.E N q f T @.1.1.1."
};

wxRibbonMSWArtProvider::wxRibbonMSWArtProvider(bool setColourScheme)
#if defined( __WXOSX__ )
    : m_tabLabelFont(*wxSMALL_FONT)
#else
    : m_tabLabelFont(*wxNORMAL_FONT)
#endif
{
    m_flags = 0;
    m_buttonBarLabelFont = m_tabLabelFont;
    m_panelLabelFont = m_tabLabelFont;

    if ( setColourScheme )
    {
        if ( wxSystemSettings::GetAppearance().IsDark() )
        {
            SetColourScheme(
                wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),
                wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
                wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        }
        else
        {
            SetColourScheme(
                wxColour(194, 216, 241),
                wxColour(255, 223, 114),
                wxColour(0, 0, 0));
        }
    }

    m_cachedTabSeparatorVisibility = -10.0; // valid visibilities are in range [0, 1]
    m_tabSeparationSize = 3;
    m_pageBorderLeft = 2;
    m_pageBorderTop = 1;
    m_pageBorderRight = 2;
    m_pageBorderBottom = 3;
    m_panelXSeparationSize = 1;
    m_panelYSeparationSize = 1;
    m_toolGroupSeparationSize = 3;
    m_galleryBitmapPaddingLeftSize = 4;
    m_galleryBitmapPaddingRightSize = 4;
    m_galleryBitmapPaddingTopSize = 4;
    m_galleryBitmapPaddingBottomSize = 4;
    m_toggleButtonOffset = 22;
    m_helpButtonOffset = 22;
}

wxRibbonMSWArtProvider::~wxRibbonMSWArtProvider()
{
}

void wxRibbonMSWArtProvider::GetColourScheme(
                         wxColour* primary,
                         wxColour* secondary,
                         wxColour* tertiary) const
{
    if ( primary != nullptr )
        *primary = m_primarySchemeColour;
    if ( secondary != nullptr )
        *secondary = m_secondarySchemeColour;
    if ( tertiary != nullptr )
        *tertiary = m_tertiarySchemeColour;
}

void wxRibbonMSWArtProvider::SetColourScheme(
                         const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary)
{
    m_primarySchemeColour = primary;
    m_secondarySchemeColour = secondary;
    m_tertiarySchemeColour = tertiary;

    wxRibbonHSLColour primaryHsl(primary);
    wxRibbonHSLColour secondaryHsl(secondary);
    // tertiary not used for anything

    // Map primary saturation from [0, 1] to [.25, .75]
    bool primaryIsGray = false;
    static const float graySaturationThreshold = 0.01f;
    if ( primaryHsl.m_saturation <= graySaturationThreshold )
        primaryIsGray = true;
    else
    {
        primaryHsl.m_saturation = std::cos(primaryHsl.m_saturation * float(M_PI))
            * -0.25f + 0.5f;
    }

    // Map primary luminance from [0, 1] to [.23, .83]
    primaryHsl.m_luminance = std::cos(primaryHsl.m_luminance * float(M_PI)) * -0.3f + 0.53f;

    // Map secondary saturation from [0, 1] to [0.16, 0.84]
    bool secondaryIsGray = false;
    if ( secondaryHsl.m_saturation <= graySaturationThreshold )
        secondaryIsGray = true;
    else
    {
        secondaryHsl.m_saturation = std::cos(secondaryHsl.m_saturation * float(M_PI))
            * -0.34f + 0.5f;
    }

    // Map secondary luminance from [0, 1] to [0.1, 0.9]
    secondaryHsl.m_luminance = std::cos(secondaryHsl.m_luminance * float(M_PI)) * -0.4f + 0.5f;

    const auto LikePrimary = [primaryHsl, primaryIsGray]
        (double h, double s, double l)
        {
            return primaryHsl.ShiftHue(h).Saturated(primaryIsGray ? 0.0 : s)
                .AdjustLuminance(l).ToRGB();
        };
    const auto LikeSecondary = [secondaryHsl, secondaryIsGray]
        (double h, double s, double l)
        {
            return secondaryHsl.ShiftHue(h).Saturated(secondaryIsGray ? 0.0 : s)
                .AdjustLuminance(l).ToRGB();
        };

    m_pageBorderPen = LikePrimary(1.4, 0.00, -0.08);

    m_pageBackgroundTopColour = LikePrimary(-0.1, -0.03, 0.12);
    m_pageHoverBackgroundTopColour = LikePrimary(-2.8, 0.27, 0.17);
    m_pageBackgroundTopGradientColour = LikePrimary(0.1, -0.10, 0.08);
    m_pageHoverBackgroundTopGradientColour = LikePrimary(3.2, 0.16, 0.13);
    m_pageBackgroundColour = LikePrimary(0.4, -0.09, 0.05);
    m_pageHoverBackgroundColour = LikePrimary(0.1, 0.19, 0.10);
    m_pageBackgroundGradientColour = LikePrimary(-3.2, 0.27, 0.10);
    m_pageHoverBackgroundGradientColour = LikePrimary(1.8, 0.01, 0.15);

    m_tabActiveBackgroundColour = LikePrimary(-0.1, -0.31, 0.16);
    m_tabActiveBackgroundGradientColour = LikePrimary(-0.1, -0.03, 0.12);
    m_tabSeparatorColour = LikePrimary(0.9, 0.24, 0.05);
    m_tabCtrlBackgroundBrush = LikePrimary(1.0, 0.39, 0.07);
    m_tabHoverBackgroundColour = LikePrimary(1.3, 0.15, 0.10);
    m_tabHoverBackgroundTopColour = LikePrimary(1.4, 0.36, 0.08);
    m_tabBorderPen = LikePrimary(1.4, 0.03, -0.05);
    m_tabSeparatorGradientColour = LikePrimary(1.7, -0.15, -0.18);
    m_tabHoverBackgroundTopGradientColour = LikePrimary(1.8, 0.34, 0.13);
    m_tabLabelColour = LikePrimary(4.3, 0.13, -0.49);
    m_tabActiveLabelColour = m_tabLabelColour;
    m_tabHoverLabelColour = m_tabLabelColour;
    m_tabHoverBackgroundGradientColour = LikeSecondary(-1.5, -0.34, 0.01);

    m_panelMinimisedBorderGradientPen = LikePrimary(-6.9, -0.17, -0.09);
    m_panelMinimisedBorderPen = LikePrimary(-5.3, -0.24, -0.06);
    m_panelHoverBorderGradientPen = m_panelBorderGradientPen = LikePrimary(-5.2, -0.15, -0.06);
    m_panelHoverBorderPen = m_panelBorderPen = LikePrimary(-2.8, -0.32, 0.02);
    m_panelLabelBackgroundBrush = LikePrimary(-1.5, 0.03, 0.05);
    m_panelActiveBackgroundGradientColour = LikePrimary(0.5, 0.34, 0.05);
    m_panelHoverLabelBackgroundBrush = LikePrimary(1.0, 0.30, 0.09);
    m_panelActiveBackgroundTopGradientColour = LikePrimary(1.4, -0.17, -0.13);
    m_panelActiveBackgroundColour = LikePrimary(1.6, -0.18, -0.18);
    m_panelActiveBackgroundTopColour = LikePrimary(1.7, -0.20, -0.03);
    m_panelLabelColour = LikePrimary(2.8, -0.14, -0.35);
    m_panelHoverLabelColour = m_panelLabelColour;
    m_panelMinimisedLabelColour = m_tabLabelColour;
    m_panelHoverButtonBackgroundBrush = LikeSecondary(-0.9, 0.16, -0.07);
    m_panelHoverButtonBorderPen = LikeSecondary(-3.9, -0.16, -0.14);
    SetColour(wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR, LikePrimary(1.4, -0.21, -0.23));
    SetColour(wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));

    m_ribbonToggleBrush = LikeSecondary(-0.9, 0.16, -0.07);
    m_ribbonTogglePen = LikeSecondary(-3.9, -0.16, -0.14);
    SetColour(wxRIBBON_ART_PAGE_TOGGLE_FACE_COLOUR, LikePrimary(1.7, -0.20, -0.15));
    SetColour(wxRIBBON_ART_PAGE_TOGGLE_HOVER_FACE_COLOUR, LikePrimary(1.8, -0.23, -0.21));

    m_galleryButtonDisabledBackgroundColour = LikePrimary(-2.8, -0.46, 0.09);
    m_galleryButtonDisabledBackgroundTopBrush = LikePrimary(-2.8, -0.36, 0.15);
    m_galleryHoverBackgroundBrush = LikePrimary(-0.8, 0.05, 0.15);
    m_galleryBorderPen = LikePrimary(0.7, -0.02, 0.03);
    m_galleryButtonBackgroundTopBrush = LikePrimary(0.8, 0.34, 0.13);
    m_galleryButtonBackgroundColour = LikePrimary(1.3, 0.10, 0.08);
    // SetColour used so that the relevant bitmaps are generated
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR, LikePrimary(1.4, -0.21, -0.23));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR, LikePrimary(0.0, -1.0, 0.0));
    m_galleryButtonDisabledBackgroundGradientColour = LikePrimary(1.5, -0.43, 0.12);
    m_galleryButtonBackgroundGradientColour = LikePrimary(1.7, 0.11, 0.09);
    m_galleryItemBorderPen = LikeSecondary(-3.9, -0.16, -0.14);
    m_galleryButtonHoverBackgroundColour = LikeSecondary(-0.9, 0.16, -0.07);
    m_galleryButtonHoverBackgroundGradientColour = LikeSecondary(0.1, 0.12, 0.03);
    m_galleryButtonHoverBackgroundTopBrush = LikeSecondary(4.3, 0.16, 0.17);

    m_galleryButtonActiveBackgroundColour = LikeSecondary(-9.9, 0.03, -0.22);
    m_galleryButtonActiveBackgroundGradientColour = LikeSecondary(-9.5, 0.14, -0.11);
    m_galleryButtonActiveBackgroundTopBrush = LikeSecondary(-9.0, 0.15, -0.08);

    m_buttonBarLabelColour = m_tabLabelColour;
    m_buttonBarLabelDisabledColour = m_tabLabelColour;

    m_buttonBarHoverBorderPen = LikeSecondary(-6.2, -0.47, -0.14);
    m_buttonBarHoverBackgroundGradientColour = LikeSecondary(-0.6, 0.16, 0.04);
    m_buttonBarHoverBackgroundColour = LikeSecondary(-0.2, 0.16, -0.10);
    m_buttonBarHoverBackgroundTopGradientColour = LikeSecondary(0.2, 0.16, 0.03);
    m_buttonBarHoverBackgroundTopColour = LikeSecondary(8.8, 0.16, 0.17);
    m_buttonBarActiveBorderPen = LikeSecondary(-6.2, -0.47, -0.25);
    m_buttonBarActiveBackgroundTopColour = LikeSecondary(-8.4, 0.08, 0.06);
    m_buttonBarActiveBackgroundTopGradientColour = LikeSecondary(-9.7, 0.13, -0.07);
    m_buttonBarActiveBackgroundColour = LikeSecondary(-9.9, 0.14, -0.14);
    m_buttonBarActiveBackgroundGradientColour = LikeSecondary(-8.7, 0.17, -0.03);

    m_toolbarBorderPen = LikePrimary(1.4, -0.21, -0.16);
    SetColour(wxRIBBON_ART_TOOLBAR_FACE_COLOUR, LikePrimary(1.4, -0.17, -0.22));
    m_toolBackgroundTopColour = LikePrimary(-1.9, -0.07, 0.06);
    m_toolBackgroundTopGradientColour = LikePrimary(1.4, 0.12, 0.08);
    m_toolBackgroundColour = LikePrimary(1.4, -0.09, 0.03);
    m_toolBackgroundGradientColour = LikePrimary(1.9, 0.11, 0.09);
    m_toolHoverBackgroundTopColour = LikeSecondary(3.4, 0.11, 0.16);
    m_toolHoverBackgroundTopGradientColour = LikeSecondary(-1.4, 0.04, 0.08);
    m_toolHoverBackgroundColour = LikeSecondary(-1.8, 0.16, -0.12);
    m_toolHoverBackgroundGradientColour = LikeSecondary(-2.6, 0.16, 0.05);
    m_toolActiveBackgroundTopColour = LikeSecondary(-9.9, -0.12, -0.09);
    m_toolActiveBackgroundTopGradientColour = LikeSecondary(-8.5, 0.16, -0.12);
    m_toolActiveBackgroundColour = LikeSecondary(-7.9, 0.16, -0.20);
    m_toolActiveBackgroundGradientColour = LikeSecondary(-6.6, 0.16, -0.10);

    // For highlight pages we show a colour between the active page and for a hovered page:
    wxColour topColour1((m_tabActiveBackgroundColour.Red()   + m_tabHoverBackgroundTopColour.Red())/2,
                         (m_tabActiveBackgroundColour.Green() + m_tabHoverBackgroundTopColour.Green())/2,
                         (m_tabActiveBackgroundColour.Blue()  + m_tabHoverBackgroundTopColour.Blue())/2);

    wxColour bottomColour1((m_tabActiveBackgroundGradientColour.Red()   + m_tabHoverBackgroundTopGradientColour.Red())/2,
                            (m_tabActiveBackgroundGradientColour.Green() + m_tabHoverBackgroundTopGradientColour.Green())/2,
                            (m_tabActiveBackgroundGradientColour.Blue()  + m_tabHoverBackgroundTopGradientColour.Blue())/2);

    m_tabHighlightTopColour = topColour1;
    m_tabHighlightTopGradientColour = bottomColour1;

    wxColour topColour2((m_tabActiveBackgroundColour.Red()   + m_tabHoverBackgroundColour.Red())/2,
                         (m_tabActiveBackgroundColour.Green() + m_tabHoverBackgroundColour.Green())/2,
                         (m_tabActiveBackgroundColour.Blue()  + m_tabHoverBackgroundColour.Blue())/2);

    wxColour bottomColour2((m_tabActiveBackgroundGradientColour.Red()   + m_tabHoverBackgroundGradientColour.Red())/2,
                            (m_tabActiveBackgroundGradientColour.Green() + m_tabHoverBackgroundGradientColour.Green())/2,
                            (m_tabActiveBackgroundGradientColour.Blue()  + m_tabHoverBackgroundGradientColour.Blue())/2);

    m_tabHighlightColour = topColour2;
    m_tabHighlightGradientColour = bottomColour2;

    // Invalidate cached tab separator
    m_cachedTabSeparatorVisibility = -1.0;
}

wxRibbonArtProvider* wxRibbonMSWArtProvider::Clone() const
{
    wxRibbonMSWArtProvider *copy = new wxRibbonMSWArtProvider;
    CloneTo(copy);
    return copy;
}

void wxRibbonMSWArtProvider::CloneTo(wxRibbonMSWArtProvider* copy) const
{
    int i;
    for ( i = 0; i < 4; ++i )
    {
        copy->m_galleryUpBundle[i] = m_galleryUpBundle[i];
        copy->m_galleryDownBundle[i] = m_galleryDownBundle[i];
        copy->m_galleryExtensionBundle[i] = m_galleryExtensionBundle[i];
    }
    for ( i = 0; i < 2; ++i )
    {
        copy->m_panelExtensionBundle[i] = m_panelExtensionBundle[i];
        copy->m_ribbonToggleUpBundle[i] = m_ribbonToggleUpBundle[i];
        copy->m_ribbonToggleDownBundle[i] = m_ribbonToggleDownBundle[i];
        copy->m_ribbonTogglePinBundle[i] = m_ribbonTogglePinBundle[i];
        copy->m_ribbonBarHelpButtonBundle[i] = m_ribbonBarHelpButtonBundle[i];
    }
    copy->m_toolbarDropBundle = m_toolbarDropBundle;

    copy->m_primarySchemeColour = m_primarySchemeColour;
    copy->m_secondarySchemeColour = m_secondarySchemeColour;
    copy->m_tertiarySchemeColour = m_tertiarySchemeColour;

    copy->m_pageToggleFaceColour = m_pageToggleFaceColour;
    copy->m_pageToggleHoverFaceColour = m_pageToggleHoverFaceColour;

    copy->m_buttonBarLabelColour = m_buttonBarLabelColour;
    copy->m_buttonBarLabelDisabledColour = m_buttonBarLabelDisabledColour;
    copy->m_tabLabelColour = m_tabLabelColour;
    copy->m_tabActiveLabelColour = m_tabActiveLabelColour;
    copy->m_tabHoverLabelColour = m_tabHoverLabelColour;
    copy->m_tabSeparatorColour = m_tabSeparatorColour;
    copy->m_tabSeparatorGradientColour = m_tabSeparatorGradientColour;
    copy->m_tabActiveBackgroundColour = m_tabActiveBackgroundColour;
    copy->m_tabActiveBackgroundGradientColour = m_tabActiveBackgroundGradientColour;
    copy->m_tabHoverBackgroundColour = m_tabHoverBackgroundColour;
    copy->m_tabHoverBackgroundGradientColour = m_tabHoverBackgroundGradientColour;
    copy->m_tabHoverBackgroundTopColour = m_tabHoverBackgroundTopColour;
    copy->m_tabHoverBackgroundTopGradientColour = m_tabHoverBackgroundTopGradientColour;
    copy->m_panelLabelColour = m_panelLabelColour;
    copy->m_panelHoverLabelColour = m_panelHoverLabelColour;
    copy->m_panelMinimisedLabelColour = m_panelMinimisedLabelColour;
    copy->m_panelButtonFaceColour = m_panelButtonFaceColour;
    copy->m_panelButtonHoverFaceColour = m_panelButtonHoverFaceColour;
    copy->m_panelActiveBackgroundColour = m_panelActiveBackgroundColour;
    copy->m_panelActiveBackgroundGradientColour = m_panelActiveBackgroundGradientColour;
    copy->m_panelActiveBackgroundTopColour = m_panelActiveBackgroundTopColour;
    copy->m_panelActiveBackgroundTopGradientColour = m_panelActiveBackgroundTopGradientColour;
    copy->m_pageBackgroundColour = m_pageBackgroundColour;
    copy->m_pageBackgroundGradientColour = m_pageBackgroundGradientColour;
    copy->m_pageBackgroundTopColour = m_pageBackgroundTopColour;
    copy->m_pageBackgroundTopGradientColour = m_pageBackgroundTopGradientColour;
    copy->m_pageHoverBackgroundColour = m_pageHoverBackgroundColour;
    copy->m_pageHoverBackgroundGradientColour = m_pageHoverBackgroundGradientColour;
    copy->m_pageHoverBackgroundTopColour = m_pageHoverBackgroundTopColour;
    copy->m_pageHoverBackgroundTopGradientColour = m_pageHoverBackgroundTopGradientColour;
    copy->m_buttonBarHoverBackgroundColour = m_buttonBarHoverBackgroundColour;
    copy->m_buttonBarHoverBackgroundGradientColour = m_buttonBarHoverBackgroundGradientColour;
    copy->m_buttonBarHoverBackgroundTopColour = m_buttonBarHoverBackgroundTopColour;
    copy->m_buttonBarHoverBackgroundTopGradientColour = m_buttonBarHoverBackgroundTopGradientColour;
    copy->m_buttonBarActiveBackgroundColour = m_buttonBarActiveBackgroundColour;
    copy->m_buttonBarActiveBackgroundGradientColour = m_buttonBarActiveBackgroundGradientColour;
    copy->m_buttonBarActiveBackgroundTopColour = m_buttonBarActiveBackgroundTopColour;
    copy->m_buttonBarActiveBackgroundTopGradientColour = m_buttonBarActiveBackgroundTopGradientColour;
    copy->m_galleryButtonBackgroundColour = m_galleryButtonBackgroundColour;
    copy->m_galleryButtonBackgroundGradientColour = m_galleryButtonBackgroundGradientColour;
    copy->m_galleryButtonHoverBackgroundColour = m_galleryButtonHoverBackgroundColour;
    copy->m_galleryButtonHoverBackgroundGradientColour = m_galleryButtonHoverBackgroundGradientColour;
    copy->m_galleryButtonActiveBackgroundColour = m_galleryButtonActiveBackgroundColour;
    copy->m_galleryButtonActiveBackgroundGradientColour = m_galleryButtonActiveBackgroundGradientColour;
    copy->m_galleryButtonDisabledBackgroundColour = m_galleryButtonDisabledBackgroundColour;
    copy->m_galleryButtonDisabledBackgroundGradientColour = m_galleryButtonDisabledBackgroundGradientColour;
    copy->m_galleryButtonFaceColour = m_galleryButtonFaceColour;
    copy->m_galleryButtonHoverFaceColour = m_galleryButtonHoverFaceColour;
    copy->m_galleryButtonActiveFaceColour = m_galleryButtonActiveFaceColour;
    copy->m_galleryButtonDisabledFaceColour = m_galleryButtonDisabledFaceColour;

    copy->m_tabHighlightTopColour = m_tabHighlightTopColour;
    copy->m_tabHighlightTopGradientColour = m_tabHighlightTopGradientColour;
    copy->m_tabHighlightColour = m_tabHighlightColour;
    copy->m_tabHighlightGradientColour = m_tabHighlightGradientColour;

    copy->m_tabCtrlBackgroundBrush = m_tabCtrlBackgroundBrush;
    copy->m_panelLabelBackgroundBrush = m_panelLabelBackgroundBrush;
    copy->m_panelHoverLabelBackgroundBrush = m_panelHoverLabelBackgroundBrush;
    copy->m_panelHoverButtonBackgroundBrush = m_panelHoverButtonBackgroundBrush;
    copy->m_galleryHoverBackgroundBrush = m_galleryHoverBackgroundBrush;
    copy->m_galleryButtonBackgroundTopBrush = m_galleryButtonBackgroundTopBrush;
    copy->m_galleryButtonHoverBackgroundTopBrush = m_galleryButtonHoverBackgroundTopBrush;
    copy->m_galleryButtonActiveBackgroundTopBrush = m_galleryButtonActiveBackgroundTopBrush;
    copy->m_galleryButtonDisabledBackgroundTopBrush = m_galleryButtonDisabledBackgroundTopBrush;
    copy->m_ribbonToggleBrush = m_ribbonToggleBrush;

    copy->m_tabLabelFont = m_tabLabelFont;
    copy->m_buttonBarLabelFont = m_buttonBarLabelFont;
    copy->m_panelLabelFont = m_panelLabelFont;

    copy->m_pageBorderPen = m_pageBorderPen;
    copy->m_panelBorderPen = m_panelBorderPen;
    copy->m_panelBorderGradientPen = m_panelBorderGradientPen;
    copy->m_panelHoverBorderPen = m_panelHoverBorderPen;
    copy->m_panelHoverBorderGradientPen = m_panelHoverBorderGradientPen;
    copy->m_panelMinimisedBorderPen = m_panelMinimisedBorderPen;
    copy->m_panelMinimisedBorderGradientPen = m_panelMinimisedBorderGradientPen;
    copy->m_panelHoverButtonBorderPen = m_panelHoverButtonBorderPen;
    copy->m_tabBorderPen = m_tabBorderPen;
    copy->m_galleryBorderPen = m_galleryBorderPen;
    copy->m_buttonBarHoverBorderPen = m_buttonBarHoverBorderPen;
    copy->m_buttonBarActiveBorderPen = m_buttonBarActiveBorderPen;
    copy->m_galleryItemBorderPen = m_galleryItemBorderPen;
    copy->m_toolbarBorderPen = m_toolbarBorderPen;
    copy->m_ribbonTogglePen = m_ribbonTogglePen;

    copy->m_flags = m_flags;
    copy->m_tabSeparationSize = m_tabSeparationSize;
    copy->m_pageBorderLeft = m_pageBorderLeft;
    copy->m_pageBorderTop = m_pageBorderTop;
    copy->m_pageBorderRight = m_pageBorderRight;
    copy->m_pageBorderBottom = m_pageBorderBottom;
    copy->m_panelXSeparationSize = m_panelXSeparationSize;
    copy->m_panelYSeparationSize = m_panelYSeparationSize;
    copy->m_galleryBitmapPaddingLeftSize = m_galleryBitmapPaddingLeftSize;
    copy->m_galleryBitmapPaddingRightSize = m_galleryBitmapPaddingRightSize;
    copy->m_galleryBitmapPaddingTopSize = m_galleryBitmapPaddingTopSize;
    copy->m_galleryBitmapPaddingBottomSize = m_galleryBitmapPaddingBottomSize;
}

long wxRibbonMSWArtProvider::GetFlags() const
{
    return m_flags;
}

void wxRibbonMSWArtProvider::SetFlags(long flags)
{
    if ( (flags ^ m_flags) & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        if ( flags & wxRIBBON_BAR_FLOW_VERTICAL )
        {
            m_pageBorderLeft++;
            m_pageBorderRight++;
            m_pageBorderTop--;
            m_pageBorderBottom--;
        }
        else
        {
            m_pageBorderLeft--;
            m_pageBorderRight--;
            m_pageBorderTop++;
            m_pageBorderBottom++;
        }
    }
    m_flags = flags;

    // Need to reload some bitmaps when flags change
#define Reload(setting) SetColour(setting, GetColour(setting))
    Reload(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR);
    Reload(wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR);
    Reload(wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR);
#undef Reload
}

int wxRibbonMSWArtProvider::GetMetric(int id) const
{
    switch ( id )
    {
        case wxRIBBON_ART_TAB_SEPARATION_SIZE:
            return m_tabSeparationSize;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE:
            return m_pageBorderLeft;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE:
            return m_pageBorderTop;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE:
            return m_pageBorderRight;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE:
            return m_pageBorderBottom;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE:
            return m_panelXSeparationSize;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE:
            return m_panelYSeparationSize;
        case wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE:
            return m_toolGroupSeparationSize;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE:
            return m_galleryBitmapPaddingLeftSize;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE:
            return m_galleryBitmapPaddingRightSize;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE:
            return m_galleryBitmapPaddingTopSize;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE:
            return m_galleryBitmapPaddingBottomSize;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }

    return 0;
}

void wxRibbonMSWArtProvider::SetMetric(int id, int newVal)
{
    switch ( id )
    {
        case wxRIBBON_ART_TAB_SEPARATION_SIZE:
            m_tabSeparationSize = newVal;
            break;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE:
            m_pageBorderLeft = newVal;
            break;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE:
            m_pageBorderTop = newVal;
            break;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE:
            m_pageBorderRight = newVal;
            break;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE:
            m_pageBorderBottom = newVal;
            break;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE:
            m_panelXSeparationSize = newVal;
            break;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE:
            m_panelYSeparationSize = newVal;
            break;
        case wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE:
            m_toolGroupSeparationSize = newVal;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE:
            m_galleryBitmapPaddingLeftSize = newVal;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE:
            m_galleryBitmapPaddingRightSize = newVal;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE:
            m_galleryBitmapPaddingTopSize = newVal;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE:
            m_galleryBitmapPaddingBottomSize = newVal;
            break;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }
}

void wxRibbonMSWArtProvider::SetFont(int id, const wxFont& font)
{
    switch ( id )
    {
        case wxRIBBON_ART_TAB_LABEL_FONT:
            m_tabLabelFont = font;
            break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_FONT:
            m_buttonBarLabelFont = font;
            break;
        case wxRIBBON_ART_PANEL_LABEL_FONT:
            m_panelLabelFont = font;
            break;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }
}

wxFont wxRibbonMSWArtProvider::GetFont(int id) const
{
    switch ( id )
    {
        case wxRIBBON_ART_TAB_LABEL_FONT:
            return m_tabLabelFont;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_FONT:
            return m_buttonBarLabelFont;
        case wxRIBBON_ART_PANEL_LABEL_FONT:
            return m_panelLabelFont;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }

    return wxNullFont;
}

wxColour wxRibbonMSWArtProvider::GetColour(int id) const
{
    switch ( id )
    {
        case wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR:
            return m_buttonBarLabelColour;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR:
            return m_buttonBarLabelDisabledColour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR:
            return m_buttonBarHoverBorderPen.GetColour();
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR:
            return m_buttonBarHoverBackgroundTopColour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_buttonBarHoverBackgroundTopGradientColour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
            return m_buttonBarHoverBackgroundColour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_buttonBarHoverBackgroundGradientColour;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BORDER_COLOUR:
            return m_buttonBarActiveBorderPen.GetColour();
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_buttonBarActiveBackgroundTopColour;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_buttonBarActiveBackgroundTopGradientColour;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_COLOUR:
            return m_buttonBarActiveBackgroundColour;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_buttonBarActiveBackgroundGradientColour;
        case wxRIBBON_ART_GALLERY_BORDER_COLOUR:
            return m_galleryBorderPen.GetColour();
        case wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR:
            return m_galleryHoverBackgroundBrush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_COLOUR:
            return m_galleryButtonBackgroundColour;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_GRADIENT_COLOUR:
            return m_galleryButtonBackgroundGradientColour;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_TOP_COLOUR:
            return m_galleryButtonBackgroundTopBrush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR:
            return m_galleryButtonFaceColour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
            return m_galleryButtonHoverBackgroundColour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_galleryButtonHoverBackgroundGradientColour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_TOP_COLOUR:
            return m_galleryButtonHoverBackgroundTopBrush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR:
            return m_galleryButtonHoverFaceColour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
            return m_galleryButtonActiveBackgroundColour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_galleryButtonActiveBackgroundGradientColour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_galleryButtonBackgroundTopBrush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR:
            return m_galleryButtonActiveFaceColour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
            return m_galleryButtonDisabledBackgroundColour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
            return m_galleryButtonDisabledBackgroundGradientColour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_TOP_COLOUR:
            return m_galleryButtonDisabledBackgroundTopBrush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR:
            return m_galleryButtonDisabledFaceColour;
        case wxRIBBON_ART_GALLERY_ITEM_BORDER_COLOUR:
            return m_galleryItemBorderPen.GetColour();
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_GRADIENT_COLOUR:
            return m_tabCtrlBackgroundBrush.GetColour();
        case wxRIBBON_ART_TAB_LABEL_COLOUR:
            return m_tabLabelColour;
        case wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR:
            return m_tabActiveLabelColour;
        case wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR:
            return m_tabHoverLabelColour;
        case wxRIBBON_ART_TAB_SEPARATOR_COLOUR:
            return m_tabSeparatorColour;
        case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR:
            return m_tabSeparatorGradientColour;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_COLOUR:
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return wxColour(0, 0, 0);
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR:
            return m_tabActiveBackgroundColour;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_tabActiveBackgroundGradientColour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR:
            return m_tabHoverBackgroundTopColour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_tabHoverBackgroundTopGradientColour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
            return m_tabHoverBackgroundColour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_tabHoverBackgroundGradientColour;
        case wxRIBBON_ART_TAB_BORDER_COLOUR:
            return m_tabBorderPen.GetColour();
        case wxRIBBON_ART_PANEL_BORDER_COLOUR:
            return m_panelBorderPen.GetColour();
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR:
            return m_panelBorderGradientPen.GetColour();
        case wxRIBBON_ART_PANEL_HOVER_BORDER_COLOUR:
            return m_panelHoverBorderPen.GetColour();
        case wxRIBBON_ART_PANEL_HOVER_BORDER_GRADIENT_COLOUR:
            return m_panelHoverBorderGradientPen.GetColour();
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR:
            return m_panelMinimisedBorderPen.GetColour();
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR:
            return m_panelMinimisedBorderGradientPen.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_GRADIENT_COLOUR:
            return m_panelLabelBackgroundBrush.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_COLOUR:
            return m_panelLabelColour;
        case wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR:
            return m_panelMinimisedLabelColour;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_GRADIENT_COLOUR:
            return m_panelHoverLabelBackgroundBrush.GetColour();
        case wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR:
            return m_panelHoverLabelColour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_panelActiveBackgroundTopColour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_panelActiveBackgroundTopGradientColour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR:
            return m_panelActiveBackgroundColour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_panelActiveBackgroundGradientColour;
        case wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR:
            return m_panelButtonFaceColour;
        case wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR:
            return m_panelButtonHoverFaceColour;
        case wxRIBBON_ART_PAGE_BORDER_COLOUR:
            return m_pageBorderPen.GetColour();
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR:
            return m_pageBackgroundTopColour;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_pageBackgroundTopGradientColour;
        case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
            return m_pageBackgroundColour;
        case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
            return m_pageBackgroundGradientColour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR:
            return m_pageHoverBackgroundTopColour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_pageHoverBackgroundTopGradientColour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR:
            return m_pageHoverBackgroundColour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_pageHoverBackgroundGradientColour;
        case wxRIBBON_ART_TOOLBAR_BORDER_COLOUR:
        case wxRIBBON_ART_TOOLBAR_HOVER_BORDER_COLOUR:
            return m_toolbarBorderPen.GetColour();
        case wxRIBBON_ART_TOOLBAR_FACE_COLOUR:
            return m_toolFaceColour;
        case wxRIBBON_ART_PAGE_TOGGLE_FACE_COLOUR:
            return m_pageToggleFaceColour;
        case wxRIBBON_ART_PAGE_TOGGLE_HOVER_FACE_COLOUR:
            return m_pageToggleHoverFaceColour;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR:
           return m_tabHighlightTopColour;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR:
           return m_tabHighlightTopGradientColour;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR:
           return m_tabHighlightColour;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR:
           return m_tabHighlightGradientColour;
        case wxRIBBON_ART_TOOL_BACKGROUND_TOP_COLOUR:
            return m_toolBackgroundTopColour;
        case wxRIBBON_ART_TOOL_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_toolBackgroundTopGradientColour;
        case wxRIBBON_ART_TOOL_BACKGROUND_COLOUR:
            return m_toolBackgroundColour;
        case wxRIBBON_ART_TOOL_BACKGROUND_GRADIENT_COLOUR:
            return m_toolBackgroundGradientColour;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_COLOUR:
            return m_toolHoverBackgroundTopColour;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_toolHoverBackgroundTopGradientColour;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_COLOUR:
            return m_toolHoverBackgroundColour;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_toolHoverBackgroundGradientColour;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_toolActiveBackgroundTopColour;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_toolActiveBackgroundTopGradientColour;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_COLOUR:
            return m_toolActiveBackgroundColour;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_toolActiveBackgroundGradientColour;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }

    return wxColour();
}

void wxRibbonMSWArtProvider::SetColour(int id, const wxColor& colour)
{
    switch ( id )
    {
        case wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR:
            m_buttonBarLabelColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR:
            m_buttonBarLabelDisabledColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR:
            m_buttonBarHoverBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR:
            m_buttonBarHoverBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_buttonBarHoverBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
            m_buttonBarHoverBackgroundColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_buttonBarHoverBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BORDER_COLOUR:
            m_buttonBarActiveBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_buttonBarActiveBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_buttonBarActiveBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_COLOUR:
            m_buttonBarActiveBackgroundColour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_buttonBarActiveBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BORDER_COLOUR:
            m_galleryBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR:
            m_galleryHoverBackgroundBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_COLOUR:
            m_galleryButtonBackgroundColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_GRADIENT_COLOUR:
            m_galleryButtonBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_TOP_COLOUR:
            m_galleryButtonBackgroundTopBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR:
            m_galleryButtonFaceColour = colour;
            if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                m_galleryUpBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_left_xpm, colour));
                m_galleryDownBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_right_xpm, colour));
            }
            else
            {
                m_galleryUpBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_up_xpm, colour));
                m_galleryDownBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_down_xpm, colour));
            }
            m_galleryExtensionBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_extension_xpm, colour));
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
            m_galleryButtonHoverBackgroundColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_galleryButtonHoverBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_TOP_COLOUR:
            m_galleryButtonHoverBackgroundTopBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR:
            m_galleryButtonHoverFaceColour = colour;
            if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                m_galleryUpBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_left_xpm, colour));
                m_galleryDownBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_right_xpm, colour));
            }
            else
            {
                m_galleryUpBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_up_xpm, colour));
                m_galleryDownBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_down_xpm, colour));
            }
            m_galleryExtensionBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_extension_xpm, colour));
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
            m_galleryButtonActiveBackgroundColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_galleryButtonActiveBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_galleryButtonActiveBackgroundTopBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR:
            m_galleryButtonActiveFaceColour = colour;
            if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                m_galleryUpBundle[2] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_left_xpm, colour));
                m_galleryDownBundle[2] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_right_xpm, colour));
            }
            else
            {
                m_galleryUpBundle[2] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_up_xpm, colour));
                m_galleryDownBundle[2] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_down_xpm, colour));
            }
            m_galleryExtensionBundle[2] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_extension_xpm, colour));
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
            m_galleryButtonDisabledBackgroundColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
            m_galleryButtonDisabledBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_TOP_COLOUR:
            m_galleryButtonDisabledBackgroundTopBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR:
            m_galleryButtonDisabledFaceColour = colour;
            if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                m_galleryUpBundle[3] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_left_xpm, colour));
                m_galleryDownBundle[3] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_right_xpm, colour));
            }
            else
            {
                m_galleryUpBundle[3] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_up_xpm, colour));
                m_galleryDownBundle[3] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_down_xpm, colour));
            }
            m_galleryExtensionBundle[3] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_extension_xpm, colour));
            break;
        case wxRIBBON_ART_GALLERY_ITEM_BORDER_COLOUR:
            m_galleryItemBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_GRADIENT_COLOUR:
            m_tabCtrlBackgroundBrush.SetColour(colour);
            m_cachedTabSeparatorVisibility = -1.0;
            break;
        case wxRIBBON_ART_TAB_LABEL_COLOUR:
            m_tabLabelColour = colour;
            break;
        case wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR:
            m_tabActiveLabelColour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR:
            m_tabHoverLabelColour = colour;
            break;
        case wxRIBBON_ART_TAB_SEPARATOR_COLOUR:
            m_tabSeparatorColour = colour;
            m_cachedTabSeparatorVisibility = -1.0;
            break;
        case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR:
            m_tabSeparatorGradientColour = colour;
            m_cachedTabSeparatorVisibility = -1.0;
            break;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_COLOUR:
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            break;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR:
            m_tabActiveBackgroundColour = colour;
            break;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_tabActiveBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR:
            m_tabHoverBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_tabHoverBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
            m_tabHoverBackgroundColour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_tabHoverBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_TAB_BORDER_COLOUR:
            m_tabBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_BORDER_COLOUR:
            m_panelBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR:
            m_panelBorderGradientPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_HOVER_BORDER_COLOUR:
            m_panelHoverBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_HOVER_BORDER_GRADIENT_COLOUR:
            m_panelHoverBorderGradientPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR:
            m_panelMinimisedBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR:
            m_panelMinimisedBorderGradientPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_GRADIENT_COLOUR:
            m_panelLabelBackgroundBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_LABEL_COLOUR:
            m_panelLabelColour = colour;
            break;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR:
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_GRADIENT_COLOUR:
            m_panelHoverLabelBackgroundBrush.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR:
            m_panelHoverLabelColour = colour;
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR:
            m_panelMinimisedLabelColour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_panelActiveBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_panelActiveBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR:
            m_panelActiveBackgroundColour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_panelActiveBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR:
            m_panelButtonFaceColour = colour;
            m_panelExtensionBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_extension_xpm, colour));
            break;
        case wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR:
            m_panelButtonHoverFaceColour = colour;
            m_panelExtensionBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_extension_xpm, colour));
            break;
        case wxRIBBON_ART_PAGE_BORDER_COLOUR:
            m_pageBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR:
            m_pageBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_pageBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
            m_pageBackgroundColour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
            m_pageBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR:
            m_pageHoverBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_pageHoverBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR:
            m_pageHoverBackgroundColour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_pageHoverBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOLBAR_BORDER_COLOUR:
        case wxRIBBON_ART_TOOLBAR_HOVER_BORDER_COLOUR:
            m_toolbarBorderPen.SetColour(colour);
            break;
        case wxRIBBON_ART_TOOLBAR_FACE_COLOUR:
            m_toolFaceColour = colour;
            m_toolbarDropBundle = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(gallery_down_xpm, colour));
            break;
        case wxRIBBON_ART_PAGE_TOGGLE_FACE_COLOUR:
            m_pageToggleFaceColour = colour;
            m_ribbonToggleDownBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_toggle_down_xpm, colour));
            m_ribbonToggleUpBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_toggle_up_xpm, colour));
            m_ribbonTogglePinBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(ribbon_toggle_pin_xpm, colour));
            m_ribbonBarHelpButtonBundle[0] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(ribbon_help_button_xpm, colour));
            break;
        case wxRIBBON_ART_PAGE_TOGGLE_HOVER_FACE_COLOUR:
            m_pageToggleHoverFaceColour = colour;
            m_ribbonToggleDownBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_toggle_down_xpm, colour));
            m_ribbonToggleUpBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(panel_toggle_up_xpm, colour));
            m_ribbonTogglePinBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(ribbon_toggle_pin_xpm, colour));
            m_ribbonBarHelpButtonBundle[1] = wxBitmapBundle::FromBitmap(wxRibbonLoadPixmap(ribbon_help_button_xpm, colour));
            break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR:
           m_tabHighlightTopColour = colour;
           break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR:
           m_tabHighlightTopGradientColour = colour;
           break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR:
           m_tabHighlightColour = colour;
           break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR:
           m_tabHighlightGradientColour = colour;
           break;
        case wxRIBBON_ART_TOOL_BACKGROUND_TOP_COLOUR:
            m_toolBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_TOOL_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_toolBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOL_BACKGROUND_COLOUR:
            m_toolBackgroundColour = colour;
            break;
        case wxRIBBON_ART_TOOL_BACKGROUND_GRADIENT_COLOUR:
            m_toolBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_COLOUR:
            m_toolHoverBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_toolHoverBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_COLOUR:
            m_toolHoverBackgroundColour = colour;
            break;
        case wxRIBBON_ART_TOOL_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_toolHoverBackgroundGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_toolActiveBackgroundTopColour = colour;
            break;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_toolActiveBackgroundTopGradientColour = colour;
            break;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_COLOUR:
            m_toolActiveBackgroundColour = colour;
            break;
        case wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_toolActiveBackgroundGradientColour = colour;
            break;
        default:
            wxFAIL_MSG("Invalid Metric Ordinal");
            break;
    }
}

void wxRibbonMSWArtProvider::DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_tabCtrlBackgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    dc.SetPen(m_pageBorderPen);
    if ( rect.width > 6 )
    {
        dc.DrawLine(rect.x + 3, rect.y + rect.height - 1, rect.x + rect.width - 3, rect.y + rect.height - 1);
    }
    else
    {
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
    }
}

void wxRibbonMSWArtProvider::DrawTab(
                 wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
                 const wxRibbonPageTabInfo& tab)
{
    if ( tab.m_rect.height <= 2 )
        return;

    if ( tab.m_active || tab.m_hovered || tab.m_highlight )
    {
        if ( tab.m_active )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 2;

            dc.GradientFillLinear(background, m_tabActiveBackgroundColour,
                m_tabActiveBackgroundGradientColour, wxSOUTH);

            // TODO: active and hovered
        }
        else if ( tab.m_hovered )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;
            int h = background.height;
            background.height /= 2;
            dc.GradientFillLinear(background,
                m_tabHoverBackgroundTopColour,
                m_tabHoverBackgroundTopGradientColour, wxSOUTH);

            background.y += background.height;
            background.height = h - background.height;
            dc.GradientFillLinear(background, m_tabHoverBackgroundColour,
                m_tabHoverBackgroundGradientColour, wxSOUTH);
        }
        else if ( tab.m_highlight )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;
            int h = background.height;
            background.height /= 2;

            dc.GradientFillLinear(background, m_tabHighlightTopColour, m_tabHighlightTopGradientColour, wxSOUTH);

            background.y += background.height;
            background.height = h - background.height;

            dc.GradientFillLinear(background, m_tabHighlightColour, m_tabHighlightGradientColour, wxSOUTH);
        }

        wxPoint borderPoints[6];
        borderPoints[0] = wxPoint(1, tab.m_rect.height - 2);
        borderPoints[1] = wxPoint(1, 3);
        borderPoints[2] = wxPoint(3, 1);
        borderPoints[3] = wxPoint(tab.m_rect.width - 4, 1);
        borderPoints[4] = wxPoint(tab.m_rect.width - 2, 3);
        borderPoints[5] = wxPoint(tab.m_rect.width - 2, tab.m_rect.height - 1);

        dc.SetPen(m_tabBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints, tab.m_rect.x, tab.m_rect.y);

        if ( tab.m_active )
        {
            // Give the tab a curved outward border at the bottom
            dc.DrawPoint(tab.m_rect.x, tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 1, tab.m_rect.y + tab.m_rect.height - 2);

            wxPen p(m_tabActiveBackgroundGradientColour);
            dc.SetPen(p);

            // Technically the first two points are the wrong colour, but they're near enough
            dc.DrawPoint(tab.m_rect.x + 1, tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 2, tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + 1, tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x, tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 2, tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 1, tab.m_rect.y + tab.m_rect.height - 1);
        }
    }

    if ( tab.m_page == nullptr )
        return;

    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        wxBitmap icon = tab.m_page->GetIcon();
        if ( icon.IsOk() )
        {
        int x = tab.m_rect.x + 4;
        if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0 )
            x = tab.m_rect.x + (tab.m_rect.width - icon.GetLogicalWidth()) / 2;
        dc.DrawBitmap(icon, x, tab.m_rect.y + 1 + (tab.m_rect.height - 1 -
            icon.GetLogicalHeight()) / 2, true);
        }
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        wxString label = tab.m_page->GetLabel();
        if ( !label.empty() )
        {
            dc.SetFont(m_tabLabelFont);

            if ( tab.m_active )
            {
                dc.SetTextForeground(m_tabActiveLabelColour);
            }
            else if ( tab.m_hovered )
            {
                dc.SetTextForeground(m_tabHoverLabelColour);
            }
            else
            {
                dc.SetTextForeground(m_tabLabelColour);
            }

            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

            int textHeight;
            int textWidth;
            dc.GetTextExtent(label, &textWidth, &textHeight);
            int width = tab.m_rect.width - 5;
            int x = tab.m_rect.x + 3;
            if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
            {
                const wxBitmap& icon = tab.m_page->GetIcon();
                if ( icon.IsOk() )
                {
                    const int iconWidth = icon.GetLogicalWidth();
                    x += 3 + iconWidth;
                    width -= 3 + iconWidth;
                }
            }
            int y = tab.m_rect.y + (tab.m_rect.height - textHeight) / 2;

            if ( width <= textWidth )
            {
                wxDCClipper clip(dc, x, tab.m_rect.y, width, tab.m_rect.height);
                dc.DrawText(label, x, y);
            }
            else
            {
                dc.DrawText(label, x + (width - textWidth) / 2 + 1, y);
            }
        }
    }
}

void wxRibbonMSWArtProvider::DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility)
{
    if ( visibility <= 0.0 )
    {
        return;
    }
    if ( visibility > 1.0 )
    {
        visibility = 1.0;
    }

    // The tab separator is relatively expensive to draw (for its size), and is
    // usually drawn multiple times sequentially (in different positions), so it
    // makes sense to draw it once and cache it.
    if ( !m_cachedTabSeparator.IsOk() || m_cachedTabSeparator.GetLogicalSize() != rect.GetSize() || visibility != m_cachedTabSeparatorVisibility )
    {
        wxRect size(rect.GetSize());
        ReallyDrawTabSeparator(wnd, size, visibility);
    }
    dc.DrawBitmap(m_cachedTabSeparator, rect.x, rect.y, false);
}

void wxRibbonMSWArtProvider::ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect, double visibility)
{
    if ( !m_cachedTabSeparator.IsOk() || m_cachedTabSeparator.GetLogicalSize() != rect.GetSize() )
    {
        m_cachedTabSeparator = wxBitmap(rect.GetSize());
    }

    wxMemoryDC dc(m_cachedTabSeparator);
    DrawTabCtrlBackground(dc, wnd, rect);

    wxCoord x = rect.x + rect.width / 2;
    double h = (double)(rect.height - 1);

    double r1 = m_tabCtrlBackgroundBrush.GetColour().Red() * (1.0 - visibility) + 0.5;
    double g1 = m_tabCtrlBackgroundBrush.GetColour().Green() * (1.0 - visibility) + 0.5;
    double b1 = m_tabCtrlBackgroundBrush.GetColour().Blue() * (1.0 - visibility) + 0.5;
    double r2 = m_tabSeparatorColour.Red();
    double g2 = m_tabSeparatorColour.Green();
    double b2 = m_tabSeparatorColour.Blue();
    double r3 = m_tabSeparatorGradientColour.Red();
    double g3 = m_tabSeparatorGradientColour.Green();
    double b3 = m_tabSeparatorGradientColour.Blue();

    for ( int i = 0; i < rect.height - 1; ++i )
    {
        double p = ((double)i)/h;

        double r = (p * r3 + (1.0 - p) * r2) * visibility + r1;
        double g = (p * g3 + (1.0 - p) * g2) * visibility + g1;
        double b = (p * b3 + (1.0 - p) * b2) * visibility + b1;

        wxPen P(wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b));
        dc.SetPen(P);
        dc.DrawPoint(x, rect.y + i);
    }

    m_cachedTabSeparatorVisibility = visibility;
}

void wxRibbonMSWArtProvider::DrawPartialPageBackground(wxDC& dc,
        wxWindow* wnd, const wxRect& r, wxRibbonPage* page,
        wxPoint offset, bool hovered)
{
    wxRect background;
    // Expanded panels need a background - the expanded panel at
    // best size may have a greater Y dimension higher than when
    // on the bar if it has a sizer. AUI art provider does not need this
    // because it paints the panel without reference to its parent's size.
    // Expanded panels use a wxFrame as parent (not a wxRibbonPage).

    if ( wnd->GetSizer() && wnd->GetParent() != page )
    {
        background = wnd->GetParent()->GetSize();
        offset = wxPoint(0,0);
    }
    else
    {
        background = page->GetSize();
        page->AdjustRectToIncludeScrollButtons(&background);
        background.height -= 2;
    }
    // Page background isn't dependent upon the width of the page
    // (at least not the part of it intended to be painted by this
    // function). Set to wider than the page itself for when externally
    // expanded panels need a background - the expanded panel can be wider
    // than the bar.
    background.x = 0;
    background.width = INT_MAX;

    // upperRect, lowerRect, paintRect are all in page co-ordinates
    wxRect upperRect(background);
    upperRect.height /= 5;

    wxRect lowerRect(background);
    lowerRect.y += upperRect.height;
    lowerRect.height -= upperRect.height;

    wxRect paintRect(r);
    paintRect.x += offset.x;
    paintRect.y += offset.y;

    wxColour bgTop, bgTopGrad, bgBtm, bgBtmGrad;
    if ( hovered )
    {
        bgTop = m_pageHoverBackgroundTopColour;
        bgTopGrad = m_pageHoverBackgroundTopGradientColour;
        bgBtm = m_pageHoverBackgroundColour;
        bgBtmGrad = m_pageHoverBackgroundGradientColour;
    }
    else
    {
        bgTop = m_pageBackgroundTopColour;
        bgTopGrad = m_pageBackgroundTopGradientColour;
        bgBtm = m_pageBackgroundColour;
        bgBtmGrad = m_pageBackgroundGradientColour;
    }

    if ( paintRect.Intersects(upperRect) )
    {
        wxRect rect(upperRect);
        rect.Intersect(paintRect);
        rect.x -= offset.x;
        rect.y -= offset.y;
        wxColour startingColour(wxRibbonInterpolateColour(bgTop, bgTopGrad,
            paintRect.y, upperRect.y, upperRect.y + upperRect.height));
        wxColour endingColour(wxRibbonInterpolateColour(bgTop, bgTopGrad,
            paintRect.y + paintRect.height, upperRect.y,
            upperRect.y + upperRect.height));
        dc.GradientFillLinear(rect, startingColour, endingColour, wxSOUTH);
    }

    if ( paintRect.Intersects(lowerRect) )
    {
        wxRect rect(lowerRect);
        rect.Intersect(paintRect);
        rect.x -= offset.x;
        rect.y -= offset.y;
        wxColour startingColour(wxRibbonInterpolateColour(bgBtm, bgBtmGrad,
            paintRect.y, lowerRect.y, lowerRect.y + lowerRect.height));
        wxColour endingColour(wxRibbonInterpolateColour(bgBtm, bgBtmGrad,
            paintRect.y + paintRect.height,
            lowerRect.y, lowerRect.y + lowerRect.height));
        dc.GradientFillLinear(rect, startingColour, endingColour, wxSOUTH);
    }
}

void wxRibbonMSWArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_tabCtrlBackgroundBrush);

    {
        wxRect edge(rect);

        edge.width = 2;
        dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);

        edge.x += rect.width - 2;
        dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);

        edge = rect;
        edge.height = 2;
        edge.y += (rect.height - edge.height);
        dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);
    }

    {
        wxRect background(rect);
        background.x += 2;
        background.width -= 4;
        background.height -= 2;

        background.height /= 5;
        dc.GradientFillLinear(background, m_pageBackgroundTopColour,
            m_pageBackgroundTopGradientColour, wxSOUTH);

        background.y += background.height;
        background.height = rect.height - 2 - background.height;
        dc.GradientFillLinear(background, m_pageBackgroundColour,
            m_pageBackgroundGradientColour, wxSOUTH);
    }

    {
        wxPoint borderPoints[8];
        borderPoints[0] = wxPoint(2, 0);
        borderPoints[1] = wxPoint(1, 1);
        borderPoints[2] = wxPoint(1, rect.height - 4);
        borderPoints[3] = wxPoint(3, rect.height - 2);
        borderPoints[4] = wxPoint(rect.width - 4, rect.height - 2);
        borderPoints[5] = wxPoint(rect.width - 2, rect.height - 4);
        borderPoints[6] = wxPoint(rect.width - 2, 1);
        borderPoints[7] = wxPoint(rect.width - 4, -1);

        dc.SetPen(m_pageBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints, rect.x, rect.y);
    }
}

void wxRibbonMSWArtProvider::DrawScrollButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect_,
                        long style)
{
    wxRect rect(rect_);

    if ( (style & wxRIBBON_SCROLL_BTN_FOR_MASK) == wxRIBBON_SCROLL_BTN_FOR_PAGE )
    {
        // Page scroll buttons do not have the luxury of rendering on top of anything
        // else, and their size includes some padding, hence the background painting
        // and size adjustment.
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_tabCtrlBackgroundBrush);
        dc.DrawRectangle(rect);
        dc.SetClippingRegion(rect);
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            rect.x++;
            wxFALLTHROUGH;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            rect.y--;
            rect.width--;
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            rect.x++;
            rect.y--;
            rect.width -= 2;
            rect.height++;
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            rect.x++;
            rect.width -= 2;
            rect.height--;
            break;
        }
    }

    {
        wxRect background(rect);
        background.x++;
        background.y++;
        background.width -= 2;
        background.height -= 2;

        if ( style & wxRIBBON_SCROLL_BTN_UP )
            background.height /= 2;
        else
            background.height /= 5;
        dc.GradientFillLinear(background, m_pageBackgroundTopColour,
            m_pageBackgroundTopGradientColour, wxSOUTH);

        background.y += background.height;
        background.height = rect.height - 2 - background.height;
        dc.GradientFillLinear(background, m_pageBackgroundColour,
            m_pageBackgroundGradientColour, wxSOUTH);
    }

    {
        wxPoint borderPoints[7];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            borderPoints[0] = wxPoint(2, 0);
            borderPoints[1] = wxPoint(rect.width - 1, 0);
            borderPoints[2] = wxPoint(rect.width - 1, rect.height - 1);
            borderPoints[3] = wxPoint(2, rect.height - 1);
            borderPoints[4] = wxPoint(0, rect.height - 3);
            borderPoints[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            borderPoints[0] = wxPoint(0, 0);
            borderPoints[1] = wxPoint(rect.width - 3, 0);
            borderPoints[2] = wxPoint(rect.width - 1, 2);
            borderPoints[3] = wxPoint(rect.width - 1, rect.height - 3);
            borderPoints[4] = wxPoint(rect.width - 3, rect.height - 1);
            borderPoints[5] = wxPoint(0, rect.height - 1);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            borderPoints[0] = wxPoint(2, 0);
            borderPoints[1] = wxPoint(rect.width - 3, 0);
            borderPoints[2] = wxPoint(rect.width - 1, 2);
            borderPoints[3] = wxPoint(rect.width - 1, rect.height - 1);
            borderPoints[4] = wxPoint(0, rect.height - 1);
            borderPoints[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            borderPoints[0] = wxPoint(0, 0);
            borderPoints[1] = wxPoint(rect.width - 1, 0);
            borderPoints[2] = wxPoint(rect.width - 1, rect.height - 3);
            borderPoints[3] = wxPoint(rect.width - 3, rect.height - 1);
            borderPoints[4] = wxPoint(2, rect.height - 1);
            borderPoints[5] = wxPoint(0, rect.height - 3);
            break;
        }
        borderPoints[6] = borderPoints[0];

        dc.SetPen(m_pageBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints, rect.x, rect.y);
    }

    {
        // NB: Code for handling hovered/active state is temporary
        wxPoint arrowPoints[3];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            arrowPoints[0] = wxPoint(rect.width / 2 - 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] + wxPoint(3, -3);
            arrowPoints[2] = arrowPoints[0] + wxPoint(3,  3);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            arrowPoints[0] = wxPoint(rect.width / 2 + 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] - wxPoint(3,  3);
            arrowPoints[2] = arrowPoints[0] - wxPoint(3, -3);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 - 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] + wxPoint( 3, 3);
            arrowPoints[2] = arrowPoints[0] + wxPoint(-3, 3);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 + 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] - wxPoint( 3, 3);
            arrowPoints[2] = arrowPoints[0] - wxPoint(-3, 3);
            break;
        }

        dc.SetPen(*wxTRANSPARENT_PEN);
        wxBrush B(style & wxRIBBON_SCROLL_BTN_HOVERED ? m_tabActiveBackgroundColour : m_tabLabelColour);
        dc.SetBrush(B);
        dc.DrawPolygon(sizeof(arrowPoints)/sizeof(wxPoint), arrowPoints, rect.x, rect.y);
    }
}

void wxRibbonMSWArtProvider::DrawDropdownArrow(wxDC& dc, int x, int y, const wxColour& colour)
{
    wxPoint arrowPoints[3];
    wxBrush brush(colour);
    arrowPoints[0] = wxPoint(1, 2);
    arrowPoints[1] = arrowPoints[0] + wxPoint(-3, -3);
    arrowPoints[2] = arrowPoints[0] + wxPoint( 3, -3);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(brush);
    dc.DrawPolygon(sizeof(arrowPoints)/sizeof(wxPoint), arrowPoints, x, y);
}

void wxRibbonMSWArtProvider::RemovePanelPadding(wxRect* rect)
{
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        rect->y += 1;
        rect->height -= 2;
    }
    else
    {
        rect->x += 1;
        rect->width -= 2;
    }
}

void wxRibbonMSWArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);
    bool hasExtButton = wnd->HasExtButton();

    int labelHeight;
    {
        dc.SetFont(m_panelLabelFont);
        dc.SetPen(*wxTRANSPARENT_PEN);
        if ( wnd->IsHovered() )
        {
            dc.SetBrush(m_panelHoverLabelBackgroundBrush);
            dc.SetTextForeground(m_panelHoverLabelColour);
        }
        else
        {
            dc.SetBrush(m_panelLabelBackgroundBrush);
            dc.SetTextForeground(m_panelLabelColour);
        }

        wxRect labelRect(trueRect);
        wxString label = wnd->GetLabel();
        bool clipLabel = false;
        wxSize labelSize(dc.GetTextExtent(label));

        labelRect.SetX(labelRect.GetX() + 1);
        labelRect.SetWidth(labelRect.GetWidth() - 2);
        labelRect.SetHeight(labelSize.GetHeight() + 2);
        labelRect.SetY(trueRect.GetBottom() - labelRect.GetHeight());
        labelHeight = labelRect.GetHeight();

        wxRect labelBgRect = labelRect;

        if ( hasExtButton )
            labelRect.SetWidth(labelRect.GetWidth() - 13);

        if ( labelSize.GetWidth() > labelRect.GetWidth() )
        {
            // Test if there is enough length for 3 letters and ...
            wxString newLabel = label.Mid(0, 3) + "...";
            labelSize = dc.GetTextExtent(newLabel);
            if ( labelSize.GetWidth() > labelRect.GetWidth() )
            {
                // Not enough room for three characters and ...
                // Display the entire label and just crop it
                clipLabel = true;
            }
            else
            {
                // Room for some characters and ...
                // Display as many characters as possible and append ...
                for ( int len = wxSsize(label) - 1; len >= 3; --len )
                {
                    newLabel = label.Mid(0, len) + "...";
                    labelSize = dc.GetTextExtent(newLabel);
                    if ( labelSize.GetWidth() <= labelRect.GetWidth() )
                    {
                        label = newLabel;
                        break;
                    }
                }
            }
        }

        dc.DrawRectangle(labelBgRect);
        if ( clipLabel )
        {
            wxDCClipper clip(dc, labelRect);
            dc.DrawText(label, labelRect.x, labelRect.y +
                (labelRect.GetHeight() - labelSize.GetHeight()) / 2);
        }
        else
        {
            dc.DrawText(label, labelRect.x +
                (labelRect.GetWidth() - labelSize.GetWidth()) / 2,
                labelRect.y +
                (labelRect.GetHeight() - labelSize.GetHeight()) / 2);
        }

        if ( hasExtButton )
        {
            if ( wnd->IsExtButtonHovered() )
            {
                dc.SetPen(m_panelHoverButtonBorderPen);
                dc.SetBrush(m_panelHoverButtonBackgroundBrush);
                dc.DrawRoundedRectangle(labelRect.GetRight(), labelRect.GetBottom() - 13, 13, 13, 1.0);
                dc.DrawBitmap(m_panelExtensionBundle[1].GetBitmapFor(wnd), labelRect.GetRight() + 3, labelRect.GetBottom() - 10, true);
            }
            else
                dc.DrawBitmap(m_panelExtensionBundle[0].GetBitmapFor(wnd), labelRect.GetRight() + 3, labelRect.GetBottom() - 10, true);
        }
    }

    if ( wnd->IsHovered() )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height -= 2 + labelHeight;
        DrawPartialPageBackground(dc, wnd, clientRect, true);
    }

    if ( !wnd->IsHovered() )
        DrawPanelBorder(dc, trueRect, m_panelBorderPen, m_panelBorderGradientPen);
    else
        DrawPanelBorder(dc, trueRect, m_panelHoverBorderPen, m_panelHoverBorderGradientPen);
}

wxRect wxRibbonMSWArtProvider::GetPanelExtButtonArea(wxReadOnlyDC& WXUNUSED(dc),
                        const wxRibbonPanel* WXUNUSED(wnd),
                        wxRect rect)
{
    RemovePanelPadding(&rect);
    rect = wxRect(rect.GetRight()-13, rect.GetBottom()-13, 13, 13);
    return rect;
}

void wxRibbonMSWArtProvider::DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);

    if ( wnd->IsHovered() )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_galleryHoverBackgroundBrush);
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 2,
                rect.height - 16);
        }
        else
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 16,
                rect.height - 2);
        }
    }

    dc.SetPen(m_galleryBorderPen);
    // Outline
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    DrawGalleryBackgroundCommon(dc, wnd, rect);
}

void wxRibbonMSWArtProvider::DrawGalleryBackgroundCommon(wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    wxRect upBtn, downBtn, extBtn;

    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        // Divider between items and buttons
        dc.DrawLine(rect.x, rect.y + rect.height - 15, rect.x + rect.width,
            rect.y + rect.height - 15);

        upBtn = wxRect(rect.x, rect.y + rect.height - 15, rect.width / 3, 15);

        downBtn = wxRect(upBtn.GetRight() + 1, upBtn.GetTop(),
            upBtn.GetWidth(), upBtn.GetHeight());
        dc.DrawLine(downBtn.GetLeft(), downBtn.GetTop(), downBtn.GetLeft(),
            downBtn.GetBottom());

        extBtn = wxRect(downBtn.GetRight() + 1, upBtn.GetTop(), rect.width -
            upBtn.GetWidth() - downBtn.GetWidth() - 1, upBtn.GetHeight());
        dc.DrawLine(extBtn.GetLeft(), extBtn.GetTop(), extBtn.GetLeft(),
            extBtn.GetBottom());
    }
    else
    {
        // Divider between items and buttons
        dc.DrawLine(rect.x + rect.width - 15, rect.y, rect.x + rect.width - 15,
            rect.y + rect.height);

        upBtn = wxRect(rect.x + rect.width - 15, rect.y, 15, rect.height / 3);

        downBtn = wxRect(upBtn.GetLeft(), upBtn.GetBottom() + 1,
            upBtn.GetWidth(), upBtn.GetHeight());
        dc.DrawLine(downBtn.GetLeft(), downBtn.GetTop(), downBtn.GetRight(),
            downBtn.GetTop());

        extBtn = wxRect(upBtn.GetLeft(), downBtn.GetBottom() + 1, upBtn.GetWidth(),
            rect.height - upBtn.GetHeight() - downBtn.GetHeight() - 1);
        dc.DrawLine(extBtn.GetLeft(), extBtn.GetTop(), extBtn.GetRight(),
            extBtn.GetTop());
    }

    DrawGalleryButton(dc, upBtn, wnd->GetUpButtonState(),
        m_galleryUpBundle, wnd);
    DrawGalleryButton(dc, downBtn, wnd->GetDownButtonState(),
        m_galleryDownBundle, wnd);
    DrawGalleryButton(dc, extBtn, wnd->GetExtensionButtonState(),
        m_galleryExtensionBundle, wnd);
}

void wxRibbonMSWArtProvider::DrawGalleryButton(wxDC& dc,
                                            wxRect rect,
                                            wxRibbonGalleryButtonState state,
                                            wxBitmapBundle* bundles,
                                            wxWindow* wnd)
{
    wxBitmap btnBitmap;
    wxBrush btnTopBrush;
    wxColour btnColour;
    wxColour btnGradColour;
    switch ( state )
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        btnTopBrush = m_galleryButtonBackgroundTopBrush;
        btnColour = m_galleryButtonBackgroundColour;
        btnGradColour = m_galleryButtonBackgroundGradientColour;
        btnBitmap = bundles[0].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        btnTopBrush = m_galleryButtonHoverBackgroundTopBrush;
        btnColour = m_galleryButtonHoverBackgroundColour;
        btnGradColour = m_galleryButtonHoverBackgroundGradientColour;
        btnBitmap = bundles[1].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        btnTopBrush = m_galleryButtonActiveBackgroundTopBrush;
        btnColour = m_galleryButtonActiveBackgroundColour;
        btnGradColour = m_galleryButtonActiveBackgroundGradientColour;
        btnBitmap = bundles[2].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        btnTopBrush = m_galleryButtonDisabledBackgroundTopBrush;
        btnColour = m_galleryButtonDisabledBackgroundColour;
        btnGradColour = m_galleryButtonDisabledBackgroundGradientColour;
        btnBitmap = bundles[3].GetBitmapFor(wnd);
        break;
    }

    rect.x++;
    rect.y++;
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        rect.width--;
        rect.height -= 2;
    }
    else
    {
        rect.width -= 2;
        rect.height--;
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(btnTopBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height / 2);

    wxRect lower(rect);
    lower.height = (lower.height + 1) / 2;
    lower.y += rect.height - lower.height;
    dc.GradientFillLinear(lower, btnColour, btnGradColour, wxSOUTH);

    dc.DrawBitmap(btnBitmap, rect.x + rect.width / 2 - 2, lower.y - 2, true);
}

void wxRibbonMSWArtProvider::DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item)
{
    if ( wnd->GetHoveredItem() != item && wnd->GetActiveItem() != item &&
        wnd->GetSelection() != item )
        return;

    dc.SetPen(m_galleryItemBorderPen);
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    wxBrush topBrush;
    wxColour bgColour;
    wxColour bgGradientColour;

    if ( wnd->GetActiveItem() == item || wnd->GetSelection() == item )
    {
        topBrush = m_galleryButtonActiveBackgroundTopBrush;
        bgColour = m_galleryButtonActiveBackgroundColour;
        bgGradientColour = m_galleryButtonActiveBackgroundGradientColour;
    }
    else
    {
        topBrush = m_galleryButtonHoverBackgroundTopBrush;
        bgColour = m_galleryButtonHoverBackgroundColour;
        bgGradientColour = m_galleryButtonHoverBackgroundGradientColour;
    }

    wxRect upper(rect);
    upper.x += 1;
    upper.width -= 2;
    upper.y += 1;
    upper.height /= 3;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(topBrush);
    dc.DrawRectangle(upper.x, upper.y, upper.width, upper.height);

    wxRect lower(upper);
    lower.y += lower.height;
    lower.height = rect.height - 2 - lower.height;
    dc.GradientFillLinear(lower, bgColour, bgGradientColour, wxSOUTH);
}

void wxRibbonMSWArtProvider::DrawPanelBorder(wxDC& dc, const wxRect& rect,
                                             wxPen& primaryColour,
                                             wxPen& secondaryColour)
{
    wxPoint borderPoints[9];
    borderPoints[0] = wxPoint(2, 0);
    borderPoints[1] = wxPoint(rect.width - 3, 0);
    borderPoints[2] = wxPoint(rect.width - 1, 2);
    borderPoints[3] = wxPoint(rect.width - 1, rect.height - 3);
    borderPoints[4] = wxPoint(rect.width - 3, rect.height - 1);
    borderPoints[5] = wxPoint(2, rect.height - 1);
    borderPoints[6] = wxPoint(0, rect.height - 3);
    borderPoints[7] = wxPoint(0, 2);

    if ( primaryColour.GetColour() == secondaryColour.GetColour() )
    {
        borderPoints[8] = borderPoints[0];
        dc.SetPen(primaryColour);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints, rect.x, rect.y);
    }
    else
    {
        dc.SetPen(primaryColour);
        dc.DrawLines(3, borderPoints, rect.x, rect.y);

#define SingleLine(start, finish) \
        dc.DrawLine(start.x + rect.x, start.y + rect.y, finish.x + rect.x, finish.y + rect.y)

        SingleLine(borderPoints[0], borderPoints[7]);
        dc.SetPen(secondaryColour);
        dc.DrawLines(3, borderPoints + 4, rect.x, rect.y);
        SingleLine(borderPoints[4], borderPoints[3]);

#undef SingleLine

        borderPoints[6] = borderPoints[2];
        wxRibbonDrawParallelGradientLines(dc, 2, borderPoints + 6, 0, 1,
            borderPoints[3].y - borderPoints[2].y + 1, rect.x, rect.y,
            primaryColour.GetColour(), secondaryColour.GetColour());
    }
}

void wxRibbonMSWArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);

    if ( wnd->GetExpandedPanel() != nullptr )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height = (rect.y + rect.height / 5) - clientRect.x;
        dc.GradientFillLinear(clientRect,
            m_panelActiveBackgroundTopColour,
            m_panelActiveBackgroundTopGradientColour, wxSOUTH);

        clientRect.y += clientRect.height;
        clientRect.height = (trueRect.y + trueRect.height) - clientRect.y;
        dc.GradientFillLinear(clientRect,
            m_panelActiveBackgroundColour,
            m_panelActiveBackgroundGradientColour, wxSOUTH);
    }
    else if ( wnd->IsHovered() )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height -= 2;
        DrawPartialPageBackground(dc, wnd, clientRect, true);
    }

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, trueRect, &preview);

    dc.SetBrush(m_panelHoverLabelBackgroundBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(preview.x + 1, preview.y + preview.height - 8,
        preview.width - 2, 7);

    int midPos = rect.y + rect.height / 5 - preview.y;
    if ( midPos < 0 || midPos >= preview.height )
    {
        wxRect fullRect(preview);
        fullRect.x += 1;
        fullRect.y += 1;
        fullRect.width -= 2;
        fullRect.height -= 9;
        if ( midPos < 0 )
        {
            dc.GradientFillLinear(fullRect,
                m_pageHoverBackgroundColour,
                m_pageHoverBackgroundGradientColour, wxSOUTH);
        }
        else
        {
            dc.GradientFillLinear(fullRect,
                m_pageHoverBackgroundTopColour,
                m_pageHoverBackgroundTopGradientColour, wxSOUTH);
        }
    }
    else
    {
        wxRect topRect(preview);
        topRect.x += 1;
        topRect.y += 1;
        topRect.width -= 2;
        topRect.height = midPos;
        dc.GradientFillLinear(topRect,
            m_pageHoverBackgroundTopColour,
            m_pageHoverBackgroundTopGradientColour, wxSOUTH);

        wxRect btmRect(topRect);
        btmRect.y = preview.y + midPos;
        btmRect.height = preview.y + preview.height - 7 - btmRect.y;
        dc.GradientFillLinear(btmRect,
            m_pageHoverBackgroundColour,
            m_pageHoverBackgroundGradientColour, wxSOUTH);
    }

    if ( bitmap.IsOk() )
    {
        dc.DrawBitmap(bitmap, preview.x + (preview.width - bitmap.GetLogicalWidth()) / 2,
            preview.y + (preview.height - 7 - bitmap.GetLogicalHeight()) / 2, true);
    }

    if ( !wnd->IsHovered() )
        DrawPanelBorder(dc, preview, m_panelBorderPen, m_panelBorderGradientPen);
    else
        DrawPanelBorder(dc, preview, m_panelHoverBorderPen, m_panelHoverBorderGradientPen);

    DrawPanelBorder(dc, trueRect, m_panelMinimisedBorderPen,
        m_panelMinimisedBorderGradientPen);
}

void wxRibbonMSWArtProvider::DrawMinimisedPanelCommon(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& trueRect,
                        wxRect* previewRect)
{
    wxRect preview(0, 0, 32, 32);
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        preview.x = trueRect.x + 4;
        preview.y = trueRect.y + (trueRect.height - preview.height) / 2;
    }
    else
    {
        preview.x = trueRect.x + (trueRect.width - preview.width) / 2;
        preview.y = trueRect.y + 4;
    }
    if ( previewRect )
        *previewRect = preview;

    wxCoord label_width, labelHeight;
    dc.SetFont(m_panelLabelFont);
    dc.GetTextExtent(wnd->GetLabel(), &label_width, &labelHeight);

    int xpos = trueRect.x + (trueRect.width - label_width + 1) / 2;
    int ypos = preview.y + preview.height + 5;

    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        xpos = preview.x + preview.width + 5;
        ypos = trueRect.y + (trueRect.height - labelHeight) / 2;
    }

    dc.SetTextForeground(m_panelMinimisedLabelColour);
    dc.DrawText(wnd->GetLabel(), xpos, ypos);


    wxPoint arrowPoints[3];
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        xpos += label_width;
        arrowPoints[0] = wxPoint(xpos + 5, ypos + labelHeight / 2);
        arrowPoints[1] = arrowPoints[0] + wxPoint(-3,  3);
        arrowPoints[2] = arrowPoints[0] + wxPoint(-3, -3);
    }
    else
    {
        ypos += labelHeight;
        arrowPoints[0] = wxPoint(trueRect.width / 2, ypos + 5);
        arrowPoints[1] = arrowPoints[0] + wxPoint(-3, -3);
        arrowPoints[2] = arrowPoints[0] + wxPoint( 3, -3);
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    wxBrush B(m_panelMinimisedLabelColour);
    dc.SetBrush(B);
    dc.DrawPolygon(sizeof(arrowPoints)/sizeof(wxPoint), arrowPoints,
        trueRect.x, trueRect.y);
}

void wxRibbonMSWArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, true);
}

void wxRibbonMSWArtProvider::DrawPartialPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        bool allowHovered)
{
    // Assume the window is a child of a ribbon page, and also check for a
    // hovered panel somewhere between the window and the page, as it causes
    // the background to change.
    wxPoint offset(wnd->GetPosition());
    wxRibbonPage* page = nullptr;
    wxWindow* parent = wnd->GetParent();
    wxRibbonPanel* panel = wxDynamicCast(wnd, wxRibbonPanel);
    bool hovered = false;

    if ( panel != nullptr )
    {
        hovered = allowHovered && panel->IsHovered();
        if ( panel->GetExpandedDummy() != nullptr )
        {
            offset = panel->GetExpandedDummy()->GetPosition();
            parent = panel->GetExpandedDummy()->GetParent();
        }
    }
    for ( ; parent; parent = parent->GetParent() )
    {
        if ( panel == nullptr )
        {
            panel = wxDynamicCast(parent, wxRibbonPanel);
            if ( panel != nullptr )
            {
                hovered = allowHovered && panel->IsHovered();
                if ( panel->GetExpandedDummy() != nullptr )
                {
                    parent = panel->GetExpandedDummy();
                }
            }
        }
        page = wxDynamicCast(parent, wxRibbonPage);
        if ( page != nullptr )
        {
            break;
        }
        offset += parent->GetPosition();
    }
    if ( page != nullptr )
    {
        DrawPartialPageBackground(dc, wnd, rect, page, offset, hovered);
        return;
    }

    // No page found - fallback to painting with a stock brush
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonMSWArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        kind = wxRIBBON_BUTTON_NORMAL;
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED )
            state ^= wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
    }

    if ( state & (wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK |
        wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK) )
    {
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            dc.SetPen(m_buttonBarActiveBorderPen);
        else
            dc.SetPen(m_buttonBarHoverBorderPen);

        wxRect bgRect(rect);
        bgRect.x++;
        bgRect.y++;
        bgRect.width -= 2;
        bgRect.height -= 2;

        wxRect bgRectTop(bgRect);
        bgRectTop.height /= 3;
        bgRect.y += bgRectTop.height;
        bgRect.height -= bgRectTop.height;

        if ( kind == wxRIBBON_BUTTON_HYBRID )
        {
            switch ( state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmapLarge.GetLogicalHeight() + 4;
                    wxRect partialBg(rect);
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        partialBg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partialBg.height -= (iYBorder - partialBg.y + 1);
                        partialBg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width, iYBorder);
                    bgRect.Intersect(partialBg);
                    bgRectTop.Intersect(partialBg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                {
                    int iArrowWidth = 9;
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        bgRect.width -= iArrowWidth;
                        bgRectTop.width -= iArrowWidth;
                        dc.DrawLine(bgRectTop.x + bgRectTop.width,
                            rect.y, bgRectTop.x + bgRectTop.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bgRect.x += bgRect.width - iArrowWidth;
                        bgRectTop.x += bgRectTop.width - iArrowWidth;
                        bgRect.width = iArrowWidth;
                        bgRectTop.width = iArrowWidth;
                        dc.DrawLine(bgRectTop.x - 1, rect.y,
                            bgRectTop.x - 1, rect.y + rect.height);
                    }
                }
                break;
            }
        }

        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
        {
            dc.GradientFillLinear(bgRectTop,
                m_buttonBarActiveBackgroundTopColour,
                m_buttonBarActiveBackgroundTopGradientColour, wxSOUTH);
            dc.GradientFillLinear(bgRect,
                m_buttonBarActiveBackgroundColour,
                m_buttonBarActiveBackgroundGradientColour, wxSOUTH);
        }
        else
        {
            dc.GradientFillLinear(bgRectTop,
                m_buttonBarHoverBackgroundTopColour,
                m_buttonBarHoverBackgroundTopGradientColour, wxSOUTH);
            dc.GradientFillLinear(bgRect,
                m_buttonBarHoverBackgroundColour,
                m_buttonBarHoverBackgroundGradientColour, wxSOUTH);
        }

        wxPoint borderPoints[9];
        borderPoints[0] = wxPoint(2, 0);
        borderPoints[1] = wxPoint(rect.width - 3, 0);
        borderPoints[2] = wxPoint(rect.width - 1, 2);
        borderPoints[3] = wxPoint(rect.width - 1, rect.height - 3);
        borderPoints[4] = wxPoint(rect.width - 3, rect.height - 1);
        borderPoints[5] = wxPoint(2, rect.height - 1);
        borderPoints[6] = wxPoint(0, rect.height - 3);
        borderPoints[7] = wxPoint(0, 2);
        borderPoints[8] = borderPoints[0];

        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
            rect.x, rect.y);
    }

    dc.SetFont(m_buttonBarLabelFont);
    dc.SetTextForeground(state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED
                            ? m_buttonBarLabelDisabledColour
                            : m_buttonBarLabelColour);
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmapLarge,
        bitmapSmall);
}

void wxRibbonMSWArtProvider::DrawButtonBarButtonForeground(
                        wxDC& dc,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall)
{
    const wxColour
        arrowColour(state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED
                        ? m_buttonBarLabelDisabledColour
                        : m_buttonBarLabelColour);

    switch ( state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
    {
    case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
        {
            const int padding = 2;
            dc.DrawBitmap(bitmapLarge,
                rect.x + (rect.width - bitmapLarge.GetLogicalWidth()) / 2,
                rect.y + padding, true);
            int ypos = rect.y + padding + bitmapLarge.GetLogicalHeight() + padding;
            int arrowWidth = kind == wxRIBBON_BUTTON_NORMAL ? 0 : 8;
            wxCoord labelW, labelH;
            dc.GetTextExtent(label, &labelW, &labelH);
            if ( labelW + 2 * padding <= rect.width )
            {
                dc.DrawText(label, rect.x + (rect.width - labelW) / 2, ypos);
                if ( arrowWidth != 0 )
                {
                    DrawDropdownArrow(dc, rect.x + rect.width / 2,
                        ypos + (labelH * 3) / 2,
                        arrowColour);
                }
            }
            else
            {
                size_t breakI = label.Len();
                do
                {
                    --breakI;
                    if ( wxRibbonCanLabelBreakAtPosition(label, breakI) )
                    {
                        wxString labelTop = label.Mid(0, breakI);
                        dc.GetTextExtent(labelTop, &labelW, &labelH);
                        if ( labelW + 2 * padding <= rect.width )
                        {
                            dc.DrawText(labelTop,
                                rect.x + (rect.width - labelW) / 2, ypos);
                            ypos += labelH;
                            wxString labelBottom = label.Mid(breakI + 1);
                            dc.GetTextExtent(labelBottom, &labelW, &labelH);
                            labelW += arrowWidth;
                            int iX = rect.x + (rect.width - labelW) / 2;
                            dc.DrawText(labelBottom, iX, ypos);
                            if ( arrowWidth != 0 )
                            {
                                DrawDropdownArrow(dc,
                                    iX + 2 +labelW - arrowWidth,
                                    ypos + labelH / 2 + 1,
                                    arrowColour);
                            }
                            break;
                        }
                    }
                } while ( breakI > 0 );
            }
        }
        break;
    case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
        {
            int xCursor = rect.x + 2;
            dc.DrawBitmap(bitmapSmall, xCursor,
                    rect.y + (rect.height - bitmapSmall.GetLogicalHeight())/2, true);
            xCursor += bitmapSmall.GetLogicalWidth() + 2;
            wxCoord labelW, labelH;
            dc.GetTextExtent(label, &labelW, &labelH);
            dc.DrawText(label, xCursor,
                rect.y + (rect.height - labelH) / 2);
            xCursor += labelW + 3;
            if ( kind != wxRIBBON_BUTTON_NORMAL )
            {
                DrawDropdownArrow(dc, xCursor, rect.y + rect.height / 2, arrowColour);
            }
            break;
        }
    case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
        {
            int availWidth = rect.width;
            if ( kind != wxRIBBON_BUTTON_NORMAL )
            {
                availWidth -= 8;
                DrawDropdownArrow(dc, rect.x + availWidth + 4, rect.y + rect.height / 2, arrowColour);
            }
            int xCursor = rect.x + (availWidth - bitmapSmall.GetLogicalWidth()) / 2;
            dc.DrawBitmap(bitmapSmall, xCursor,
                    rect.y + (rect.height - bitmapSmall.GetLogicalHeight()) / 2, true);
            break;
        }
    default:
        break;
    }
}

void wxRibbonMSWArtProvider::DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);
}

void wxRibbonMSWArtProvider::DrawToolGroupBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(m_toolbarBorderPen);
    wxPoint outline[9];
    outline[0] = wxPoint(2, 0);
    outline[1] = wxPoint(rect.width - 3, 0);
    outline[2] = wxPoint(rect.width - 1, 2);
    outline[3] = wxPoint(rect.width - 1, rect.height - 3);
    outline[4] = wxPoint(rect.width - 3, rect.height - 1);
    outline[5] = wxPoint(2, rect.height - 1);
    outline[6] = wxPoint(0, rect.height - 3);
    outline[7] = wxPoint(0, 2);
    outline[8] = outline[0];

    dc.DrawLines(sizeof(outline)/sizeof(wxPoint), outline, rect.x, rect.y);
}

void wxRibbonMSWArtProvider::DrawTool(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        if ( state & wxRIBBON_TOOLBAR_TOOL_TOGGLED )
            state ^= wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
    }

    wxRect bgRect(rect);
    bgRect.Deflate(1);
    if ( (state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0 )
        bgRect.width++;
    bool isSplitHybrid = (kind == wxRIBBON_BUTTON_HYBRID && (state &
        (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK | wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)));

    // Background
    wxRect bgRectTop(bgRect);
    bgRectTop.height = (bgRectTop.height * 2) / 5;
    wxRect bgRectBtm(bgRect);
    bgRectBtm.y += bgRectTop.height;
    bgRectBtm.height -= bgRectTop.height;
    wxColour bgTopColour = m_toolBackgroundTopColour;
    wxColour bgTopGradColour = m_toolBackgroundTopGradientColour;
    wxColour bgColour = m_toolBackgroundColour;
    wxColour bgGradColour = m_toolBackgroundGradientColour;
    if ( state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK )
    {
        bgTopColour = m_toolActiveBackgroundTopColour;
        bgTopGradColour = m_toolActiveBackgroundTopGradientColour;
        bgColour = m_toolActiveBackgroundColour;
        bgGradColour = m_toolActiveBackgroundGradientColour;
    }
    else if ( state & wxRIBBON_TOOLBAR_TOOL_HOVER_MASK )
    {
        bgTopColour = m_toolHoverBackgroundTopColour;
        bgTopGradColour = m_toolHoverBackgroundTopGradientColour;
        bgColour = m_toolHoverBackgroundColour;
        bgGradColour = m_toolHoverBackgroundGradientColour;
    }
    dc.GradientFillLinear(bgRectTop, bgTopColour, bgTopGradColour, wxSOUTH);
    dc.GradientFillLinear(bgRectBtm, bgColour, bgGradColour, wxSOUTH);
    if ( isSplitHybrid )
    {
        wxRect nonrect(bgRect);
        if ( state & (wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED |
            wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE) )
        {
            nonrect.width -= 8;
        }
        else
        {
            nonrect.x += nonrect.width - 8;
            nonrect.width = 8;
        }
        wxBrush B(m_toolHoverBackgroundTopColour);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(B);
        dc.DrawRectangle(nonrect.x, nonrect.y, nonrect.width, nonrect.height);
    }

    // Border
    dc.SetPen(m_toolbarBorderPen);
    if ( state & wxRIBBON_TOOLBAR_TOOL_FIRST )
    {
        dc.DrawPoint(rect.x + 1, rect.y + 1);
        dc.DrawPoint(rect.x + 1, rect.y + rect.height - 2);
    }
    else
        dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);

    if ( state & wxRIBBON_TOOLBAR_TOOL_LAST )
    {
        dc.DrawPoint(rect.x + rect.width - 2, rect.y + 1);
        dc.DrawPoint(rect.x + rect.width - 2, rect.y + rect.height - 2);
    }

    // Foreground
    int availWidth = bgRect.GetWidth();
    if ( kind & wxRIBBON_BUTTON_DROPDOWN )
    {
        availWidth -= 8;
        if ( isSplitHybrid )
        {
            dc.DrawLine(rect.x + availWidth + 1, rect.y,
                rect.x + availWidth + 1, rect.y + rect.height);
        }
        dc.DrawBitmap(m_toolbarDropBundle.GetBitmapFor(wnd), bgRect.x + availWidth + 2,
            bgRect.y + (bgRect.height / 2) - 2, true);
    }
    dc.DrawBitmap(bitmap, bgRect.x + (availWidth - bitmap.GetLogicalWidth()) / 2,
        bgRect.y + (bgRect.height - bitmap.GetLogicalHeight()) / 2, true);
}

void
wxRibbonMSWArtProvider::DrawToggleButton(wxDC& dc,
                                         wxRibbonBar* wnd,
                                         const wxRect& rect,
                                         wxRibbonDisplayMode mode)
{
    int bindex = 0;
    DrawPartialPageBackground(dc, wnd, rect, false);

    dc.DestroyClippingRegion();
    dc.SetClippingRegion(rect);

    if ( wnd->IsToggleButtonHovered() )
    {
        dc.SetPen(m_ribbonTogglePen);
        dc.SetBrush(m_ribbonToggleBrush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        bindex = 1;
    }
    switch ( mode )
    {
        case wxRIBBON_BAR_PINNED:
            dc.DrawBitmap(m_ribbonToggleUpBundle[bindex].GetBitmapFor(wnd), rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_MINIMIZED:
            dc.DrawBitmap(m_ribbonToggleDownBundle[bindex].GetBitmapFor(wnd), rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_EXPANDED:
            dc.DrawBitmap(m_ribbonTogglePinBundle[bindex].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
            break;
    }
}

void wxRibbonMSWArtProvider::DrawHelpButton(wxDC& dc,
                                       wxRibbonBar* wnd,
                                       const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    dc.DestroyClippingRegion();
    dc.SetClippingRegion(rect);

    if ( wnd->IsHelpButtonHovered() )
    {
        dc.SetPen(m_ribbonTogglePen);
        dc.SetBrush(m_ribbonToggleBrush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        dc.DrawBitmap(m_ribbonBarHelpButtonBundle[1].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
    }
    else
    {
        dc.DrawBitmap(m_ribbonBarHelpButtonBundle[0].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
    }

}

void wxRibbonMSWArtProvider::GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* smallBeginNeedSeparator,
                        int* smallMustHaveSeparator,
                        int* minimum)
{
    int width = 0;
    int min = 0;
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) && !label.empty() )
    {
        dc.SetFont(m_tabLabelFont);
        width += dc.GetTextExtent(label).GetWidth();
        min += wxMin(25, width); // enough for a few chars
        if ( bitmap.IsOk() )
        {
            // gap between label and bitmap
            width += 4;
            min += 2;
        }
    }
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS) && bitmap.IsOk() )
    {
        width += bitmap.GetLogicalWidth();
        min += bitmap.GetLogicalWidth();
    }

    if ( ideal != nullptr )
    {
        *ideal = width + 30;
    }
    if ( smallBeginNeedSeparator != nullptr )
    {
        *smallBeginNeedSeparator = width + 20;
    }
    if ( smallMustHaveSeparator != nullptr )
    {
        *smallMustHaveSeparator = width + 10;
    }
    if ( minimum != nullptr )
    {
        *minimum = min;
    }
}

int wxRibbonMSWArtProvider::GetTabCtrlHeight(
                        wxReadOnlyDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRibbonPageTabInfoArray& pages)
{
    int textHeight = 0;
    int iconHeight = 0;

    if ( pages.GetCount() <= 1 && (m_flags & wxRIBBON_BAR_ALWAYS_SHOW_TABS) == 0 )
    {
        // To preserve space, a single tab need not be displayed. We still need
        // two pixels of border / padding though.
        return 2;
    }

    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        dc.SetFont(m_tabLabelFont);
        textHeight = dc.GetTextExtent("ABCDEFXj").GetHeight() + 10;
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        size_t numPages = pages.GetCount();
        for ( size_t i = 0; i < numPages; ++i )
        {
            const wxRibbonPageTabInfo& info = pages.Item(i);
            if ( info.m_page->GetIcon().IsOk() )
            {
                iconHeight = wxMax(iconHeight, info.m_page->GetIcon().GetLogicalHeight() + 4);
            }
        }
    }

    return wxMax(textHeight, iconHeight);
}

wxSize wxRibbonMSWArtProvider::GetScrollButtonMinimumSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        wxWindow* WXUNUSED(wnd),
                        long WXUNUSED(style))
{
    return wxSize(12, 12);
}

wxSize wxRibbonMSWArtProvider::GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize clientSize,
                        wxPoint* clientOffset)
{
    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());

    clientSize.IncBy(0, labelSize.GetHeight());
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
        clientSize.IncBy(4, 8);
    else
        clientSize.IncBy(6, 6);

    if ( clientOffset != nullptr )
    {
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            *clientOffset = wxPoint(2, 3);
        else
            *clientOffset = wxPoint(3, 2);
    }

    return clientSize;
}

wxSize wxRibbonMSWArtProvider::GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* clientOffset)
{
    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());

    size.DecBy(0, labelSize.GetHeight());
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
        size.DecBy(4, 8);
    else
        size.DecBy(6, 6);

    if ( clientOffset != nullptr )
    {
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            *clientOffset = wxPoint(2, 3);
        else
            *clientOffset = wxPoint(3, 2);
    }
    if ( size.x < 0 ) size.x = 0;
    if ( size.y < 0 ) size.y = 0;

    return size;
}

wxSize wxRibbonMSWArtProvider::GetGallerySize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        const wxRibbonGallery* WXUNUSED(wnd),
                        wxSize clientSize)
{
    clientSize.IncBy( 2, 1); // Left / top padding
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
        clientSize.IncBy(1, 16); // Right / bottom padding
    else
        clientSize.IncBy(16, 1); // Right / bottom padding
    return clientSize;
}

wxSize wxRibbonMSWArtProvider::GetGalleryClientSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        const wxRibbonGallery* WXUNUSED(wnd),
                        wxSize size,
                        wxPoint* clientOffset,
                        wxRect* scrollUpButton,
                        wxRect* scrollDownButton,
                        wxRect* extensionButton)
{
    wxRect scrollUp;
    wxRect scrollDown;
    wxRect extension;
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        // Flow is vertical - put buttons on bottom
        scrollUp.y = size.GetHeight() - 15;
        scrollUp.height = 15;
        scrollUp.x = 0;
        scrollUp.width = (size.GetWidth() + 2) / 3;
        scrollDown.y = scrollUp.y;
        scrollDown.height = scrollUp.height;
        scrollDown.x = scrollUp.x + scrollUp.width;
        scrollDown.width = scrollUp.width;
        extension.y = scrollDown.y;
        extension.height = scrollDown.height;
        extension.x = scrollDown.x + scrollDown.width;
        extension.width = size.GetWidth() - scrollUp.width - scrollDown.width;
        size.DecBy(1, 16);
        size.DecBy( 2, 1);
    }
    else
    {
        // Flow is horizontal - put buttons on right
        scrollUp.x = size.GetWidth() - 15;
        scrollUp.width = 15;
        scrollUp.y = 0;
        scrollUp.height = (size.GetHeight() + 2) / 3;
        scrollDown.x = scrollUp.x;
        scrollDown.width = scrollUp.width;
        scrollDown.y = scrollUp.y + scrollUp.height;
        scrollDown.height = scrollUp.height;
        extension.x = scrollDown.x;
        extension.width = scrollDown.width;
        extension.y = scrollDown.y + scrollDown.height;
        extension.height = size.GetHeight() - scrollUp.height - scrollDown.height;
        size.DecBy(16, 1);
        size.DecBy( 2, 1);
    }

    if ( clientOffset != nullptr )
        *clientOffset = wxPoint(2, 1);
    if ( scrollUpButton != nullptr )
        *scrollUpButton = scrollUp;
    if ( scrollDownButton != nullptr )
        *scrollDownButton = scrollDown;
    if ( extensionButton != nullptr )
        *extensionButton = extension;

    return size;
}

wxRect wxRibbonMSWArtProvider::GetPageBackgroundRedrawArea(
                        wxReadOnlyDC& WXUNUSED(dc),
                        const wxRibbonPage* WXUNUSED(wnd),
                        wxSize pageOldSize,
                        wxSize pageNewSize)
{
    wxRect newRect, oldRect;

    if ( pageNewSize.GetWidth() != pageOldSize.GetWidth() )
    {
        if ( pageNewSize.GetHeight() != pageOldSize.GetHeight() )
        {
            // Width and height both changed - redraw everything
            return wxRect(pageNewSize);
        }
        else
        {
            // Only width changed - redraw right hand side
            const int right_edge_width = 4;

            newRect = wxRect(pageNewSize.GetWidth() - right_edge_width, 0, right_edge_width, pageNewSize.GetHeight());
            oldRect = wxRect(pageOldSize.GetWidth() - right_edge_width, 0, right_edge_width, pageOldSize.GetHeight());
        }
    }
    else
    {
        if ( pageNewSize.GetHeight() == pageOldSize.GetHeight() )
        {
            // Nothing changed (should never happen) - redraw nothing
            return wxRect(0, 0, 0, 0);
        }
        else
        {
            // Height changed - need to redraw everything (as the background
            // gradient is done vertically).
            return pageNewSize;
        }
    }

    newRect.Union(oldRect);
    newRect.Intersect(wxRect(pageNewSize));
    return newRect;
}

bool wxRibbonMSWArtProvider::GetButtonBarButtonSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxCoord textMinWidth,
                        wxSize bitmapSizeLarge,
                        wxSize bitmapSizeSmall,
                        wxSize* buttonSize,
                        wxRect* normalRegion,
                        wxRect* dropdownRegion)
{
    const int drop_button_width = 8;

    dc.SetFont(m_buttonBarLabelFont);
    switch ( size & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
    {
    case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
        // Small bitmap, no label
        *buttonSize = bitmapSizeSmall + wxSize(6, 4);
        switch ( kind )
        {
        case wxRIBBON_BUTTON_NORMAL:
        case wxRIBBON_BUTTON_TOGGLE:
            *normalRegion = wxRect(*buttonSize);
            *dropdownRegion = wxRect(0, 0, 0, 0);
            break;
        case wxRIBBON_BUTTON_DROPDOWN:
            *buttonSize += wxSize(drop_button_width, 0);
            *dropdownRegion = wxRect(*buttonSize);
            *normalRegion = wxRect(0, 0, 0, 0);
            break;
        case wxRIBBON_BUTTON_HYBRID:
            *normalRegion = wxRect(*buttonSize);
            *dropdownRegion = wxRect(buttonSize->GetWidth(), 0,
                drop_button_width, buttonSize->GetHeight());
            *buttonSize += wxSize(drop_button_width, 0);
            break;
        }
        break;
    case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
        // Small bitmap, with label to the right
        {
            GetButtonBarButtonSize(dc, wnd, kind, wxRIBBON_BUTTONBAR_BUTTON_SMALL,
                label, textMinWidth, bitmapSizeLarge, bitmapSizeSmall,
                buttonSize, normalRegion, dropdownRegion);
            int textSize = dc.GetTextExtent(label).GetWidth();
            if ( textSize < textMinWidth )
                textSize = textMinWidth;
            buttonSize->SetWidth(buttonSize->GetWidth() + textSize);
            switch ( kind )
            {
            case wxRIBBON_BUTTON_DROPDOWN:
                dropdownRegion->SetWidth(dropdownRegion->GetWidth() + textSize);
                break;
            case wxRIBBON_BUTTON_HYBRID:
                dropdownRegion->SetX(dropdownRegion->GetX() + textSize);
                wxFALLTHROUGH;// no break
            case wxRIBBON_BUTTON_NORMAL:
            case wxRIBBON_BUTTON_TOGGLE:
                normalRegion->SetWidth(normalRegion->GetWidth() + textSize);
                break;
            }
            break;
        }
    case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
        // Large bitmap, with label below (possibly split over 2 lines)
        {
            wxSize iconSize(bitmapSizeLarge);
            iconSize += wxSize(4, 4);
            wxCoord labelHeight;
            wxCoord bestWidth;
            dc.GetTextExtent(label, &bestWidth, &labelHeight);
            if ( bestWidth < textMinWidth )
                bestWidth = textMinWidth;
            int lastLineExtraWidth = 0;
            if ( kind != wxRIBBON_BUTTON_NORMAL && kind != wxRIBBON_BUTTON_TOGGLE )
            {
                lastLineExtraWidth += 8;
            }
            size_t i;
            for ( i = 0; i < label.Len(); ++i )
            {
                if ( wxRibbonCanLabelBreakAtPosition(label, i) )
                {
                    int width = wxMax(
                        dc.GetTextExtent(label.Left(i)).GetWidth(),
                        dc.GetTextExtent(label.Mid(i + 1)).GetWidth() + lastLineExtraWidth);
                    if ( bestWidth < textMinWidth )
                        bestWidth = textMinWidth;
                    if ( width < bestWidth )
                    {
                        bestWidth = width;
                    }
                }
            }
            labelHeight *= 2; // Assume two lines even when only one is used
                               // (to give all buttons a consistent height)
            iconSize.SetWidth(wxMax(iconSize.GetWidth(), bestWidth) + 6);
            iconSize.SetHeight(iconSize.GetHeight() + labelHeight);
            *buttonSize = iconSize;
            switch ( kind )
            {
            case wxRIBBON_BUTTON_DROPDOWN:
                *dropdownRegion = wxRect(iconSize);
                break;
            case wxRIBBON_BUTTON_HYBRID:
                *normalRegion = wxRect(iconSize);
                normalRegion->height -= 2 + labelHeight;
                dropdownRegion->x = 0;
                dropdownRegion->y = normalRegion->height;
                dropdownRegion->width = iconSize.GetWidth();
                dropdownRegion->height = iconSize.GetHeight() - normalRegion->height;
                break;
            case wxRIBBON_BUTTON_NORMAL:
            case wxRIBBON_BUTTON_TOGGLE:
                *normalRegion = wxRect(iconSize);
                break;
            }
            break;
        }
    }
    return true;
}

wxCoord wxRibbonMSWArtProvider::GetButtonBarButtonTextWidth(
                        wxReadOnlyDC& dc, const wxString& label,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size)
{
    wxCoord bestWidth = 0;
    dc.SetFont(m_buttonBarLabelFont);

    if ( (size & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
       == wxRIBBON_BUTTONBAR_BUTTON_LARGE )
    {
        bestWidth = dc.GetTextExtent(label).GetWidth();
        int lastLineExtraWidth = 0;
        if ( kind != wxRIBBON_BUTTON_NORMAL && kind != wxRIBBON_BUTTON_TOGGLE )
        {
            lastLineExtraWidth += 8;
        }
        size_t i;
        for ( i = 0; i < label.Len(); ++i )
        {
            if ( wxRibbonCanLabelBreakAtPosition(label, i) )
            {
                int width = wxMax(
                    dc.GetTextExtent(label.Left(i)).GetWidth(),
                    dc.GetTextExtent(label.Mid(i + 1)).GetWidth() + lastLineExtraWidth);
                if ( width < bestWidth )
                {
                    bestWidth = width;
                }
            }
        }
    }
    else if ( (size & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
       == wxRIBBON_BUTTONBAR_BUTTON_MEDIUM )
    {
        bestWidth = dc.GetTextExtent(label).GetWidth();
    }

    return bestWidth;
}

wxSize wxRibbonMSWArtProvider::GetMinimisedPanelMinimumSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desiredBitmapSize,
                        wxDirection* expandedPanelDirection)
{
    if ( desiredBitmapSize != nullptr )
    {
        *desiredBitmapSize = wxSize(16, 16);
    }
    if ( expandedPanelDirection != nullptr )
    {
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
            *expandedPanelDirection = wxEAST;
        else
            *expandedPanelDirection = wxSOUTH;
    }
    wxSize base_size(42, 42);

    dc.SetFont(m_panelLabelFont);
    wxSize labelSize(dc.GetTextExtent(wnd->GetLabel()));
    labelSize.IncBy(2, 2); // Allow for differences between this DC and a paint DC
    labelSize.IncBy(6, 0); // Padding
    labelSize.y *= 2; // Second line for dropdown button

    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        // Label alongside icon
        return wxSize(base_size.x + labelSize.x,
            wxMax(base_size.y, labelSize.y));
    }
    else
    {
        // Label beneath icon
        return wxSize(wxMax(base_size.x, labelSize.x),
            base_size.y + labelSize.y);
    }
}

wxSize wxRibbonMSWArtProvider::GetToolSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        wxWindow* WXUNUSED(wnd),
                        wxSize bitmapSize,
                        wxRibbonButtonKind kind,
                        bool WXUNUSED(isFirst),
                        bool isLast,
                        wxRect* dropdownRegion)
{
    wxSize size(bitmapSize);
    size.IncBy(7, 6);
    if ( isLast )
        size.IncBy(1, 0);
    if ( kind & wxRIBBON_BUTTON_DROPDOWN )
    {
        size.IncBy(8, 0);
        if ( dropdownRegion )
        {
            if ( kind == wxRIBBON_BUTTON_DROPDOWN )
                *dropdownRegion = size;
            else
                *dropdownRegion = wxRect(size.GetWidth() - 8, 0, 8, size.GetHeight());
        }
    }
    else
    {
        if ( dropdownRegion )
            *dropdownRegion = wxRect(0, 0, 0, 0);
    }
    return size;
}

wxRect
wxRibbonMSWArtProvider::GetBarToggleButtonArea(const wxRect& rect)
{
    wxRect rectOut = wxRect(rect.GetWidth()-m_toggleButtonOffset, 2, 20, 20);
    if ( (m_toggleButtonOffset==22) && (m_helpButtonOffset==22) )
        m_helpButtonOffset += 22;
    return rectOut;
}

wxRect
wxRibbonMSWArtProvider::GetRibbonHelpButtonArea(const wxRect& rect)
{
    wxRect rectOut = wxRect(rect.GetWidth()-m_helpButtonOffset, 2, 20, 20);
    if ( (m_toggleButtonOffset==22) && (m_helpButtonOffset==22) )
        m_toggleButtonOffset += 22;
    return rectOut;
}

#endif // wxUSE_RIBBON
