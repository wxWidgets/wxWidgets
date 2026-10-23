///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/richtooltipg.cpp
// Purpose:     Implementation of wxRichToolTip.
// Author:      Vadim Zeitlin
// Created:     2011-10-07
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_RICHTOOLTIP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/icon.h"
    #include "wx/region.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/timer.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/generic/private/richtooltip.h"

#include "wx/artprov.h"
#include "wx/custombgwin.h"
#include "wx/display.h"
#include "wx/graphics.h"
#include "wx/image.h"
#include "wx/popupwin.h"
#include "wx/textwrapper.h"

#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
    #include "wx/generic/hyperlink.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/uxtheme.h"
#endif

#include <memory>
#include <string.h>

namespace
{

// The image is scaled down to fit in this box (in DIPs).
constexpr int MAX_IMAGE_SIZE = 128;

#if wxUSE_HYPERLINKCTRL
// Size (in DIPs) of the help icon.
constexpr int HELP_ICON_SIZE = 16;
#endif

// How often a tooltip with a help link checks if the mouse is still over it
// once its timeout has expired.
constexpr int HELP_LINK_POLL_MS = 250;

#ifdef __WXMSW__
// Returns true if the themed MSW tooltip colors can be used. They are the
// light mode ones, so they can't be used in dark mode.
bool UseThemedColours()
{
    return wxUxThemeIsActive() && !wxSystemSettings::GetAppearance().IsDark();
}
#endif // __WXMSW__

void SetColours(wxWindow* win, const wxColour& fg, const wxColour& bg)
{
    if ( fg.IsOk() )
        win->SetForegroundColour(fg);
    if ( bg.IsOk() )
        win->SetBackgroundColour(bg);
}

// Creates the static texts used for the lines of the message.
class wxRichToolTipTextWrapper : public wxTextSizerWrapper
{
public:
    wxRichToolTipTextWrapper(wxWindow* win,
                             const wxColour& fg,
                             const wxColour& bg)
        : wxTextSizerWrapper(win),
          m_fg(fg),
          m_bg(bg)
    {
    }

protected:
    virtual wxWindow* OnCreateLine(const wxString& line) override
    {
        wxWindow* const win = wxTextSizerWrapper::OnCreateLine(line);
        SetColours(win, m_fg, m_bg);
        return win;
    }

private:
    const wxColour m_fg,
                   m_bg;
};

// Returns the image scaled down to fit in MAX_IMAGE_SIZE.
wxBitmapBundle GetScaledImage(wxWindow* win, const wxBitmapBundle& image)
{
    wxSize size = image.GetDefaultSize();
    if ( size.x <= 0 || size.y <= 0 )
        return image;

    if ( size.x > MAX_IMAGE_SIZE || size.y > MAX_IMAGE_SIZE )
    {
        const double scale = wxMin(double(MAX_IMAGE_SIZE) / size.x,
                                   double(MAX_IMAGE_SIZE) / size.y);
        size.x = wxMax(1, wxRound(size.x * scale));
        size.y = wxMax(1, wxRound(size.y * scale));
    }

    // Get the bitmap at the physical size for the window's DPI, and tell it
    // its scale factor.
    const double scaleFactor = win->GetDPIScaleFactor();
    wxBitmap bitmap = image.GetBitmap(wxSize(wxRound(size.x * scaleFactor),
                                             wxRound(size.y * scaleFactor)));
    if ( !bitmap.IsOk() )
        return image;
    bitmap.SetScaleFactor(scaleFactor);
    return wxBitmapBundle::FromBitmap(bitmap);
}

#if wxUSE_HYPERLINKCTRL && wxUSE_GRAPHICS_CONTEXT

// Returns the help icon: a question mark in a circle, drawn with our own
// colors, as the stock icon may be hard to see on the background.
wxBitmapBundle MakeHelpIcon(wxWindow* win,
                            const wxColour& circleColour,
                            const wxColour& markColour)
{
    const double scaleFactor = win->GetDPIScaleFactor();
    const int size = wxRound(HELP_ICON_SIZE * scaleFactor);

    wxImage image(size, size);
    image.InitAlpha();
    memset(image.GetAlpha(), 0, size * size);

    {
        std::unique_ptr<wxGraphicsContext>
            gc(wxGraphicsContext::Create(image));
        if ( gc == nullptr )
            return wxBitmapBundle{};

        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->SetBrush(circleColour);
        gc->DrawEllipse(0, 0, size, size);

        wxFont font = win->GetFont().Bold();
        font.SetPixelSize(wxSize{ 0, wxRound(size * 0.8) });
        gc->SetFont(font, markColour);

        wxDouble width, height;
        gc->GetTextExtent("?", &width, &height);
        gc->DrawText("?", (size - width) / 2, (size - height) / 2);
    }

    wxBitmap bitmap{ image };
    bitmap.SetScaleFactor(scaleFactor);
    return wxBitmapBundle::FromBitmap(bitmap);
}

#endif // wxUSE_HYPERLINKCTRL && wxUSE_GRAPHICS_CONTEXT

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxRichToolTipContents
// ----------------------------------------------------------------------------

wxSizer* wxRichToolTipContents::Create(wxWindow* parent,
                                       const wxRichToolTipInfo& info,
                                       bool disabled) const
{
    wxBoxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

    const wxString& title = info.GetTitle();
    if ( !title.empty() || m_icon.IsOk() )
    {
        wxBoxSizer* const sizerTitle = new wxBoxSizer(wxHORIZONTAL);
        if ( m_icon.IsOk() )
        {
            wxStaticBitmap* const icon =
                new wxStaticBitmap(parent, wxID_ANY, m_icon);
            SetColours(icon, wxColour(), m_backgroundColour);
            sizerTitle->Add(icon, wxSizerFlags().Centre().Border(wxRIGHT));
        }
        //else: Simply don't show any icon.

        wxStaticText* const labelTitle =
            new wxStaticText(parent, wxID_ANY, wxString());
        labelTitle->SetLabelText(title);
        SetColours(labelTitle, m_textColour, m_backgroundColour);

        wxFont titleFont(m_titleFont);
        if ( !titleFont.IsOk() )
        {
            // Determine the appropriate title font for the current platform.
            titleFont = labelTitle->GetFont();

#ifdef __WXMSW__
            // When using themes MSW tooltips use larger bluish version of the
            // normal font.
            if ( UseThemedColours() )
            {
                titleFont.MakeLarger();

                if ( !m_textColour.IsOk() )
                {
                    wxUxThemeHandle theme(parent, L"TOOLTIP");
                    wxColour c = theme.GetColour(TTP_BALLOONTITLE,
                                                 TMT_TEXTCOLOR);
                    if ( !c.IsOk() )
                    {
                        // Use the standard value of this colour as fallback.
                        c.Set(0x00, 0x33, 0x99);
                    }

                    labelTitle->SetForegroundColour(c);
                }
            }
            else
#endif // __WXMSW__
            {
                // Everything else, including "classic" MSW look uses just the
                // bold version of the base font.
                titleFont.MakeBold();
            }
        }

        labelTitle->SetFont(titleFont);
        sizerTitle->Add(labelTitle, wxSizerFlags().Centre());

        sizerTop->Add(sizerTitle);
    }

    const wxString& message = info.GetMessageFor(disabled);
    const wxBitmapBundle& image = info.GetImage();
    if ( !message.empty() || image.IsOk() )
    {
        wxBoxSizer* const sizerMessage = new wxBoxSizer(wxHORIZONTAL);
        if ( image.IsOk() )
        {
            wxStaticBitmap* const bitmap =
                new wxStaticBitmap(parent, wxID_ANY,
                                   GetScaledImage(parent, image));
            SetColours(bitmap, wxColour(), m_backgroundColour);
            sizerMessage->Add(bitmap, wxSizerFlags().Top());
        }
#ifdef __WXMSW__
        else if ( m_icon.IsOk() && UseThemedColours() )
        {
            // Themed tooltips under MSW align the text with the title, not
            // with the icon.
            sizerMessage->AddSpacer(m_icon.GetPreferredLogicalSizeFor(parent).x);
        }
#endif // __WXMSW__

        if ( !message.empty() )
        {
            wxRichToolTipTextWrapper wrapper(parent,
                                             m_textColour,
                                             m_backgroundColour);
            wxSizer* const sizerText =
                wrapper.CreateSizer(message,
                                    m_wrapWidth == -1
                                        ? -1
                                        : parent->FromDIP(m_wrapWidth));
            wxSizerFlags flagsText = wxSizerFlags().Top();
            if ( !sizerMessage->IsEmpty() )
                flagsText.Border(wxLEFT);
            sizerMessage->Add(sizerText, flagsText);
        }

        // Use a simple border between the elements, not a double one.
        wxSizerFlags flags;
        if ( !sizerTop->IsEmpty() )
            flags.Border(wxTOP);
        if ( m_centreMessage )
            flags.Centre();
        sizerTop->Add(sizerMessage, flags);
    }

#if wxUSE_HYPERLINKCTRL
    if ( info.HasHelpLink() )
    {
        const wxColour linkColour =
            wxSystemSettings::SelectLightDark(wxColour(0x00, 0x33, 0x99),
                                              wxColour(0x8a, 0xb4, 0xf8));
        const wxColour lineColour = m_textColour.IsOk()
                                        ? m_textColour
                                        : parent->GetForegroundColour();

        // Use vanilla window as the divider (not wxStaticLine)
        // so that we can manage its color.
        wxWindow* const divider = new wxWindow(parent, wxID_ANY,
                                               wxDefaultPosition,
                                               wxSize(-1, parent->FromDIP(1)));
        divider->SetBackgroundColour(lineColour);
        sizerTop->Add(divider, wxSizerFlags().Expand().DoubleBorder(wxTOP));

        wxBoxSizer* const sizerLink = new wxBoxSizer(wxHORIZONTAL);

#if wxUSE_GRAPHICS_CONTEXT
        const wxBitmapBundle helpIcon =
            MakeHelpIcon(parent, linkColour,
                         wxSystemSettings::SelectLightDark(*wxWHITE, *wxBLACK));
#else
        const wxBitmapBundle helpIcon =
            wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_MENU);
#endif
        if ( helpIcon.IsOk() )
        {
            wxStaticBitmap* const bitmap =
                new wxStaticBitmap(parent, wxID_ANY, helpIcon);
            SetColours(bitmap, wxColour(), m_backgroundColour);
            sizerLink->Add(bitmap,
                           wxSizerFlags().CentreVertical().Border(wxRIGHT));
        }

        // The URL isn't used (the parent handles the click), but it
        // should not be empty.
        wxGenericHyperlinkCtrl* const link =
            new wxGenericHyperlinkCtrl(parent, wxID_ANY,
                                       info.GetHelpLinkText(), "help",
                                       wxDefaultPosition, wxDefaultSize,
                                       wxHL_ALIGN_LEFT | wxNO_BORDER);
        link->SetFont(link->GetFont().Bold());
        link->SetNormalColour(linkColour);
        link->SetHoverColour(linkColour);
        link->SetVisitedColour(linkColour);
        SetColours(link, wxColour(), m_backgroundColour);
        sizerLink->Add(link, wxSizerFlags().CentreVertical());

        sizerTop->Add(sizerLink, wxSizerFlags().Border(wxTOP));
    }
#endif // wxUSE_HYPERLINKCTRL

    return sizerTop;
}

// ----------------------------------------------------------------------------
// wxRichToolTipPopup: the popup window used by wxRichToolTip.
// ----------------------------------------------------------------------------

class wxRichToolTipPopup;

static wxRichToolTipPopup* gs_currentRichToolTipPopup = nullptr;

class wxRichToolTipPopup :
    public wxCustomBackgroundWindow<wxPopupTransientWindow>
{
public:
    wxRichToolTipPopup(wxWindow* parent,
                       const wxRichToolTipInfo& info,
                       const wxBitmapBundle& icon,
                       wxTipKind tipKind,
                       const wxFont& titleFont) :
        m_timer(this)
    {
        Create(parent, wxFRAME_SHAPED);

        Move(GetTipPoint(), wxSIZE_ALLOW_MINUS_ONE);

        wxRichToolTipContents contents;
        contents.m_icon = icon;
        contents.m_titleFont = titleFont;
        contents.m_centreMessage = true;

        wxBoxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);
        sizerTop->Add(contents.Create(this, info, !parent->IsEnabled()),
                      wxSizerFlags().DoubleBorder());

        SetSizer(sizerTop);

#if wxUSE_HYPERLINKCTRL
        m_hasHelpLink = info.HasHelpLink();
        Bind(wxEVT_HYPERLINK, &wxRichToolTipPopup::OnHyperlink, this);
#else
        m_hasHelpLink = false;
#endif
        Bind(wxEVT_PAINT, &wxRichToolTipPopup::OnPaint, this);

        const int offsetY = SetTipShapeAndSize(tipKind, GetBestSize());
        if ( offsetY > 0 )
        {
            // Offset our contents by the tip height to make it appear in the
            // main rectangle.
            sizerTop->PrependSpacer(offsetY);
        }

        Layout();
    }

    ~wxRichToolTipPopup()
    {
        if ( gs_currentRichToolTipPopup == this )
            gs_currentRichToolTipPopup = nullptr;
    }

    void SetBackgroundColours(wxColour colStart, wxColour colEnd)
    {
        if ( !colStart.IsOk() )
        {
            // Determine the best colour(s) to use on our own.
#ifdef __WXMSW__
            if ( UseThemedColours() )
            {
                wxUxThemeHandle hTheme(GetParent(), L"TOOLTIP");

                colStart = hTheme.GetColour(TTP_BALLOONTITLE, TMT_GRADIENTCOLOR1);
                if ( !colStart.IsOk() )
                    colStart = wxSystemSettings::SelectLightDark(*wxWHITE, *wxBLACK);

                colEnd = hTheme.GetColour(TTP_BALLOONTITLE, TMT_GRADIENTCOLOR2);
                if ( !colEnd.IsOk() )
                    colEnd = wxSystemSettings::SelectLightDark({0xe4, 0xe5, 0xf0},
                                                               {0x40, 0x40, 0x20});
            }
            else
#endif // __WXMSW__
            {
                // In wxGTK wxSYS_COLOUR_INFOBK typically uses alpha channel
                // and this doesn't work as wxPopupWindow background currently,
                // so prefer not to set any colour at all to at least get
                // something visible on the screen instead of using black
                // bacgkround.
#ifndef __WXGTK__
                colStart = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK);
#endif
            }
        }

        if ( colEnd.IsOk() )
        {
            // Use gradient-filled background bitmap.
            const wxSize size = GetClientSize();
            wxBitmap bmp(size);
            {
                wxMemoryDC dc(bmp);
                dc.Clear();
                dc.GradientFillLinear(size, colStart, colEnd, wxDOWN);
            }

            SetBackgroundBitmap(bmp);
        }
        else if ( colStart.IsOk() ) // Use solid colour.
        {
            SetBackgroundColour(colStart);
        }
    }

    void SetPosition(const wxRect* rect)
    {
        wxPoint pos;

        if ( !rect || rect->IsEmpty() )
            pos = GetTipPoint();
        else
            pos = GetParent()->ClientToScreen( wxPoint( rect->x + rect->width / 2, rect->y + rect->height / 2 ) );

        // We want our anchor point to coincide with this position so offset
        // the position of the top left corner passed to Move() accordingly.
        pos -= m_anchorPos;

        Move(pos, wxSIZE_NO_ADJUSTMENTS);
    }

    void DoShow()
    {
        Popup();
    }

    void DismissRichToolTip()
    {
        DismissAndNotify();
    }

    void SetTimeoutAndShow(unsigned timeout, unsigned delay)
    {
        if ( !timeout && !delay )
        {
            DoShow();
            return;
        }

        Bind(wxEVT_TIMER, &wxRichToolTipPopup::OnTimer, this);

        m_timeout = timeout; // set for use in OnTimer if we have a delay
        m_delayShow = delay != 0;

        if ( !m_delayShow )
            DoShow();

        m_timer.Start((delay ? delay : timeout), true /* one shot */);
    }

protected:
    virtual void OnDismiss() override
    {
        if ( gs_currentRichToolTipPopup == this )
            gs_currentRichToolTipPopup = nullptr;

        if ( wxTheApp->IsScheduledForDestruction(this) )
            return;

        Destroy();
    }

private:
    // For now we just hard code the tip height, would be nice to do something
    // smarter in the future.
    static int GetTipHeight()
    {
#ifdef __WXMSW__
        if ( wxUxThemeIsActive() )
            return 20;
#endif // __WXMSW__

        return 15;
    }

    // Get the point to which our tip should point.
    wxPoint GetTipPoint() const
    {
        // Currently we always use the middle of the window. It seems that MSW
        // native tooltips use a different point but it's not really clear how
        // do they determine it nor whether it's worth the trouble to emulate
        // their behaviour.
        const wxRect r = GetParent()->GetScreenRect();
        return wxPoint(r.x + r.width/2, r.y + r.height/2);
    }

    // Choose the correct orientation depending on the window position.
    //
    // Also use the tip kind appropriate for the current environment. For MSW
    // the right triangles are used and for Mac the equilateral ones as this is
    // the prevailing kind under these systems. For everything else we go with
    // right triangles as well but without any real rationale so this could be
    // tweaked in the future.
    wxTipKind GetBestTipKind() const
    {
        const wxPoint pos = GetTipPoint();

        // Use GetFromWindow() and not GetFromPoint() here to try to get the
        // correct display even if the tip point itself is not visible.
        const wxRect rectDpy = wxDisplay(GetParent()).GetClientArea();

#ifdef __WXMAC__
        return pos.y > rectDpy.height/2 ? wxTipKind_Bottom : wxTipKind_Top;
#else // !__WXMAC__
        return pos.y > rectDpy.height/2
                    ? pos.x > rectDpy.width/2
                        ? wxTipKind_BottomRight
                        : wxTipKind_BottomLeft
                    : pos.x > rectDpy.width/2
                        ? wxTipKind_TopRight
                        : wxTipKind_TopLeft;
#endif // __WXMAC__/!__WXMAC__
    }

    // Set the size and shape of the tip window and returns the offset of its
    // content area from the top (horizontal offset is always 0 currently).
    int SetTipShapeAndSize(wxTipKind tipKind, const wxSize& contentSize)
    {
#if wxUSE_GRAPHICS_CONTEXT
        wxSize size = contentSize;

        // The size is the vertical size and the offset is the distance from
        // edge for asymmetric tips, currently hard-coded to be the same as the
        // size.
        const int tipSize = FromDIP(GetTipHeight());
        const int tipOffset = tipSize;

        // The horizontal position of the tip.
        int x = -1;

        // The vertical coordinates of the tip base and apex.
        int yBase = -1,
            yApex = -1;

        // The offset of the content part of the window.
        int dy = -1;

        // Define symbolic names for the rectangle corners and mid-way points
        // that we use below in an attempt to make the code more clear. Notice
        // that these values must be consecutive as we iterate over them.
        enum RectPoint
        {
            RectPoint_TopLeft,
            RectPoint_Top,
            RectPoint_TopRight,
            RectPoint_Right,
            RectPoint_BotRight,
            RectPoint_Bot,
            RectPoint_BotLeft,
            RectPoint_Left,
            RectPoint_Max
        };

        // The starting point for AddArcToPoint() calls below, we iterate over
        // all RectPoints from it.
        RectPoint pointStart = RectPoint_Max;


        // Hard-coded radius of the round main rectangle corners.
        const double RADIUS = 5;

        // Create a path defining the shape of the tooltip window.
        wxGraphicsPath
            path = wxGraphicsRenderer::GetDefaultRenderer()->CreatePath();

        if ( tipKind == wxTipKind_Auto )
            tipKind = GetBestTipKind();

        // Points defining the tip shape (in clockwise order as we must end at
        // tipPoints[0] after drawing the rectangle outline in this order).
        wxPoint2DDouble tipPoints[3];

        switch ( tipKind )
        {
            case wxTipKind_Auto:
                wxFAIL_MSG( "Impossible kind value" );
                break;

            case wxTipKind_TopLeft:
                x = tipOffset;
                yApex = 0;
                yBase = tipSize;
                dy = tipSize;

                tipPoints[0] = wxPoint2DDouble(x, yBase);
                tipPoints[1] = wxPoint2DDouble(x, yApex);
                tipPoints[2] = wxPoint2DDouble(x + tipSize, yBase);

                pointStart = RectPoint_TopRight;
                break;

            case wxTipKind_TopRight:
                x = size.x - tipOffset;
                yApex = 0;
                yBase = tipSize;
                dy = tipSize;

                tipPoints[0] = wxPoint2DDouble(x - tipSize, yBase);
                tipPoints[1] = wxPoint2DDouble(x, yApex);
                tipPoints[2] = wxPoint2DDouble(x, yBase);

                pointStart = RectPoint_TopRight;
                break;

            case wxTipKind_BottomLeft:
                x = tipOffset;
                yApex = size.y + tipSize;
                yBase = size.y;
                dy = 0;

                tipPoints[0] = wxPoint2DDouble(x + tipSize, yBase);
                tipPoints[1] = wxPoint2DDouble(x, yApex);
                tipPoints[2] = wxPoint2DDouble(x, yBase);

                pointStart = RectPoint_BotLeft;
                break;

            case wxTipKind_BottomRight:
                x = size.x - tipOffset;
                yApex = size.y + tipSize;
                yBase = size.y;
                dy = 0;

                tipPoints[0] = wxPoint2DDouble(x, yBase);
                tipPoints[1] = wxPoint2DDouble(x, yApex);
                tipPoints[2] = wxPoint2DDouble(x - tipSize, yBase);

                pointStart = RectPoint_BotLeft;
                break;

            case wxTipKind_Top:
                x = size.x/2;
                yApex = 0;
                yBase = tipSize;
                dy = tipSize;

                {
                    // A half-side of an equilateral triangle is its altitude
                    // divided by sqrt(3) ~= 1.73.
                    const double halfside = tipSize/1.73;

                    tipPoints[0] = wxPoint2DDouble(x - halfside, yBase);
                    tipPoints[1] = wxPoint2DDouble(x, yApex);
                    tipPoints[2] = wxPoint2DDouble(x + halfside, yBase);
                }

                pointStart = RectPoint_TopRight;
                break;

            case wxTipKind_Bottom:
                x = size.x/2;
                yApex = size.y + tipSize;
                yBase = size.y;
                dy = 0;

                {
                    const double halfside = tipSize/1.73;

                    tipPoints[0] = wxPoint2DDouble(x + halfside, yBase);
                    tipPoints[1] = wxPoint2DDouble(x, yApex);
                    tipPoints[2] = wxPoint2DDouble(x - halfside, yBase);
                }

                pointStart = RectPoint_BotLeft;
                break;

            case wxTipKind_None:
                x = size.x/2;
                dy = 0;

                path.AddRoundedRectangle(0, 0, size.x, size.y, RADIUS);
                break;
        }

        wxASSERT_MSG( dy != -1, wxS("Unknown tip kind?") );

        size.y += tipSize;
        SetSize(size);

        if ( tipKind != wxTipKind_None )
        {
            path.MoveToPoint(tipPoints[0]);
            path.AddLineToPoint(tipPoints[1]);
            path.AddLineToPoint(tipPoints[2]);

            const double xLeft = 0.;
            const double xMid = size.x/2.;
            const double xRight = size.x;

            const double yTop = dy;
            const double yMid = (dy + size.y)/2.;
            const double yBot = dy + contentSize.y;

            wxPoint2DDouble rectPoints[RectPoint_Max];
            rectPoints[RectPoint_TopLeft]  = wxPoint2DDouble(xLeft,  yTop);
            rectPoints[RectPoint_Top]      = wxPoint2DDouble(xMid,   yTop);
            rectPoints[RectPoint_TopRight] = wxPoint2DDouble(xRight, yTop);
            rectPoints[RectPoint_Right]    = wxPoint2DDouble(xRight, yMid);
            rectPoints[RectPoint_BotRight] = wxPoint2DDouble(xRight, yBot);
            rectPoints[RectPoint_Bot]      = wxPoint2DDouble(xMid,   yBot);
            rectPoints[RectPoint_BotLeft]  = wxPoint2DDouble(xLeft,  yBot);
            rectPoints[RectPoint_Left]     = wxPoint2DDouble(xLeft,  yMid);

            // Iterate over all rectangle rectPoints for the first 3 corners.
            unsigned n = pointStart;
            for ( unsigned corner = 0; corner < 3; corner++ )
            {
                const wxPoint2DDouble& pt1 = rectPoints[n];

                n = (n + 1) % RectPoint_Max;

                const wxPoint2DDouble& pt2 = rectPoints[n];

                path.AddArcToPoint(pt1.m_x, pt1.m_y, pt2.m_x, pt2.m_y, RADIUS);

                n = (n + 1) % RectPoint_Max;
            }

            // Last one wraps to the first point of the tip.
            const wxPoint2DDouble& pt1 = rectPoints[n];
            const wxPoint2DDouble& pt2 = tipPoints[0];

            path.AddArcToPoint(pt1.m_x, pt1.m_y, pt2.m_x, pt2.m_y, RADIUS);

            path.CloseSubpath();
        }

        SetShape(path);
        m_path = path;
#else // !wxUSE_GRAPHICS_CONTEXT
        wxUnusedVar(tipKind);

        int x = contentSize.x/2,
            yApex = 0,
            dy = 0;

        SetSize(contentSize);
#endif // wxUSE_GRAPHICS_CONTEXT/!wxUSE_GRAPHICS_CONTEXT

        m_anchorPos.x = x;
        m_anchorPos.y = yApex;

        return dy;
    }

#if wxUSE_HYPERLINKCTRL
    void OnHyperlink(wxHyperlinkEvent& WXUNUSED(event))
    {
        // Don't skip the event, so that the URL is not opened.
        wxWindow* const win = GetParent();
        wxCommandEvent* const
            event = new wxCommandEvent(wxEVT_RICHTOOLTIP_HELP_CLICKED,
                                       win->GetId());
        event->SetEventObject(win);
        wxQueueEvent(win->GetEventHandler(), event);

        // Don't destroy ourselves while still inside the link event handler.
        CallAfter(&wxRichToolTipPopup::DismissRichToolTip);
    }
#endif // wxUSE_HYPERLINKCTRL

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        // Without an outline, the tooltip would be hard to see against
        // the dark windows below it.
        if ( !wxSystemSettings::GetAppearance().IsDark() )
            return;

        const wxColour outlineColour(0x80, 0x80, 0x80);
#if wxUSE_GRAPHICS_CONTEXT
        std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
        if ( gc != nullptr )
        {
            // Half of the pen is outside of the window shape.
            gc->SetPen(wxPen(outlineColour, FromDIP(2)));
            gc->StrokePath(m_path);
        }
#else // !wxUSE_GRAPHICS_CONTEXT
        dc.SetPen(outlineColour);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(GetClientRect());
#endif // wxUSE_GRAPHICS_CONTEXT/!wxUSE_GRAPHICS_CONTEXT
    }

    // Timer event handler hides the tooltip when the timeout expires.
    void OnTimer(wxTimerEvent& WXUNUSED(event))
    {
        if ( !m_delayShow )
        {
            // Keep the tooltip with a link shown while the mouse is over it
            // so that you can click on it.
            if ( m_hasHelpLink &&
                    GetScreenRect().Contains(wxGetMousePosition()) )
            {
                m_timer.Start(HELP_LINK_POLL_MS, true);
                return;
            }

            // Doing "Notify" here ensures that our OnDismiss() is called and so we
            // also Destroy() ourselves. We could use Dismiss() and call Destroy()
            // explicitly from here as well.
            DismissAndNotify();

            return;
        }

        m_delayShow = false;

        if ( m_timeout )
            m_timer.Start(m_timeout, true);

        DoShow();
    }


    // The anchor point offset if we show a tip or the middle of the top side
    // otherwise.
    wxPoint m_anchorPos;

    // The timer counting down the time until we're hidden.
    wxTimer m_timer;

    // We will need to accesss the timeout period when delaying showing tooltip.
    int m_timeout;

    // If true, delay showing the tooltip.
    bool m_delayShow;

    // If true, the tooltip shows a help link.
    bool m_hasHelpLink;

#if wxUSE_GRAPHICS_CONTEXT
    // The shape of the window, used to draw its outline.
    wxGraphicsPath m_path;
#endif

    wxDECLARE_NO_COPY_CLASS(wxRichToolTipPopup);
};

static void DismissCurrentRichToolTipPopup()
{
    if ( !gs_currentRichToolTipPopup )
        return;

    wxRichToolTipPopup* const popup = gs_currentRichToolTipPopup;
    gs_currentRichToolTipPopup = nullptr;
    popup->DismissRichToolTip();
}

// ----------------------------------------------------------------------------
// wxRichToolTipGenericImpl: generic implementation of wxRichToolTip.
// ----------------------------------------------------------------------------

void
wxRichToolTipGenericImpl::SetBackgroundColour(const wxColour& col,
                                              const wxColour& colEnd)
{
    m_colStart = col;
    m_colEnd = colEnd;
}

void wxRichToolTipGenericImpl::SetCustomIcon(const wxBitmapBundle& icon)
{
    m_icon = icon;
}

void wxRichToolTipGenericImpl::SetStandardIcon(int icon)
{
    switch ( icon & wxICON_MASK )
    {
        case wxICON_WARNING:
        case wxICON_ERROR:
        case wxICON_INFORMATION:
            // Although we don't use this icon in a list, we need a smallish
            // icon here and not an icon of a typical message box size so use
            // wxART_LIST to get it.
            m_icon = wxArtProvider::GetBitmapBundle
                     (
                        wxArtProvider::GetMessageBoxIconId(icon),
                        wxART_LIST
                     );
            break;

        case wxICON_QUESTION:
            wxFAIL_MSG("Question icon doesn't make sense for a tooltip");
            break;

        case wxICON_NONE:
            m_icon = wxBitmapBundle();
            break;
    }
}

void wxRichToolTipGenericImpl::SetTimeout(unsigned millisecondsTimeout,
                                          unsigned millisecondsDelay)
{
    m_delay = millisecondsDelay;
    m_timeout = millisecondsTimeout;
}

void wxRichToolTipGenericImpl::SetTipKind(wxTipKind tipKind)
{
    m_tipKind = tipKind;
}

void wxRichToolTipGenericImpl::SetTitleFont(const wxFont& font)
{
    m_titleFont = font;
}

void wxRichToolTipGenericImpl::ShowFor(wxWindow* win, const wxRect* rect)
{
    DismissCurrentRichToolTipPopup();

    wxRichToolTipPopup* const popup = new wxRichToolTipPopup
                                          (
                                            win,
                                            m_info,
                                            m_icon,
                                            m_tipKind,
                                            m_titleFont
                                          );
    gs_currentRichToolTipPopup = popup;

    popup->SetBackgroundColours(m_colStart, m_colEnd);

    popup->SetPosition(rect);
    // show or start the timer to delay showing the popup
    popup->SetTimeoutAndShow( m_timeout, m_delay );
}

// Currently only wxMSW provides a native implementation.
#ifndef __WXMSW__

/* static */
wxRichToolTipImpl*
wxRichToolTipImpl::Create(const wxRichToolTipInfo& info)
{
    return new wxRichToolTipGenericImpl(info);
}

#endif // !__WXMSW__

#endif // wxUSE_RICHTOOLTIP
