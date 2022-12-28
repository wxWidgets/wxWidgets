// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWidgets
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STC

#ifndef WX_PRECOMP
    #include "wx/math.h"
    #include "wx/menu.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/display.h"
#include "wx/encconv.h"
#include "wx/listctrl.h"
#include "wx/mstream.h"
#include "wx/xpmdecod.h"
#include "wx/image.h"
#include "wx/tokenzr.h"
#include "wx/dynlib.h"
#include "wx/scopedarray.h"
#include "wx/toplevel.h"
#include "wx/vlbox.h"
#include "wx/sizer.h"
#include "wx/renderer.h"
#include "wx/hashset.h"
#include "wx/dcclient.h"
#include "wx/wupdlock.h"

#ifdef wxHAS_RAW_BITMAP
#include "wx/rawbmp.h"
#endif
#if wxUSE_GRAPHICS_CONTEXT
#include "wx/dcgraph.h"
#include "wx/graphics.h"
#endif

#include "PlatWX.h"
#include "wx/stc/stc.h"
#include "wx/stc/private.h"

#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
#define HAVE_DIRECTWRITE_TECHNOLOGY
#endif

#ifdef HAVE_DIRECTWRITE_TECHNOLOGY
#include "ScintillaWX.h"
#include <float.h>
#endif

#if defined(__WXGTK__) && wxSTC_POPUP_IS_FRAME
    #include "wx/gtk/private/wrapgtk.h"
#elif defined(__WXMSW__)
    #include "wx/msw/wrapwin.h"
#elif defined(__WXOSX_COCOA__)
    #include "PlatWXcocoa.h"
#endif

wxRect wxRectFromPRectangle(PRectangle prc) {
    wxRect r(wxRound(prc.left), wxRound(prc.top),
             wxRound(prc.Width()), wxRound(prc.Height()));
    return r;
}

PRectangle PRectangleFromwxRect(wxRect rc) {
    return PRectangle(rc.GetLeft(), rc.GetTop(),
                      rc.GetRight()+1, rc.GetBottom()+1);
}

wxColour wxColourFromCD(ColourDesired const& cd) {
    return wxColour(cd.GetRed(), cd.GetGreen(), cd.GetBlue());
}

wxColour wxColourFromCDandAlpha(ColourDesired const& cd, int alpha) {
    return wxColour(cd.GetRed(), cd.GetGreen(), cd.GetBlue(), alpha);
}

wxColour wxColourFromCA(ColourAlpha const& cd) {
    return wxColour(cd.GetRed(), cd.GetGreen(), cd.GetBlue(), cd.GetAlpha());
}

//----------------------------------------------------------------------

namespace Scintilla
{

inline wxWindow* GETWIN(WindowID id) { return (wxWindow*)id; }

// wxFont with ascent cached, a pointer to this type is stored in Font::fid.
class wxFontWithAscent : public wxFont
{
public:
    explicit wxFontWithAscent(const wxFont &font)
        : wxFont(font),
          m_ascent(0),
          m_surfaceFontData(nullptr)
    {
    }

    virtual ~wxFontWithAscent()
    {
        delete m_surfaceFontData;
    }

    static wxFontWithAscent* FromFID(FontID fid)
    {
        return static_cast<wxFontWithAscent*>(fid);
    }

    void SetAscent(int ascent) { m_ascent = ascent; }
    int GetAscent() const { return m_ascent; }

    SurfaceData* GetSurfaceFontData() const { return m_surfaceFontData; }
    void SetSurfaceFontData(SurfaceData* data) { m_surfaceFontData=data; }

private:
    int m_ascent;
    SurfaceData* m_surfaceFontData;
};

void SetAscent(Font& f, int ascent)
{
    wxFontWithAscent::FromFID(f.GetID())->SetAscent(ascent);
}

int GetAscent(Font& f)
{
    return wxFontWithAscent::FromFID(f.GetID())->GetAscent();
}


Font::Font() noexcept {
    fid = nullptr;
}

Font::~Font() {
}

void Font::Create(const FontParameters &fp) {
    Release();

    // The minus one is done because since Scintilla uses SC_CHARSET_DEFAULT
    // internally and we need to have wxFONENCODING_DEFAULT == SC_SHARSET_DEFAULT
    // so we adjust the encoding before passing it to Scintilla.  See also
    // wxStyledTextCtrl::StyleSetCharacterSet
    wxFontEncoding encoding = (wxFontEncoding)(fp.characterSet-1);

    wxFontEncodingArray ea = wxEncodingConverter::GetPlatformEquivalents(encoding);
    if (ea.GetCount())
        encoding = ea[0];

    wxFontWeight weight;
    if (fp.weight <= 300)
        weight = wxFONTWEIGHT_LIGHT;
    else if (fp.weight >= 700)
        weight = wxFONTWEIGHT_BOLD;
    else
        weight = wxFONTWEIGHT_NORMAL;

    wxFont font(wxRound(fp.size),
        wxFONTFAMILY_DEFAULT,
        fp.italic ? wxFONTSTYLE_ITALIC :  wxFONTSTYLE_NORMAL,
        weight,
        false,
        stc2wx(fp.faceName),
        encoding);
    wxFontWithAscent* newFont = new wxFontWithAscent(font);
    fid = newFont;

#ifdef HAVE_DIRECTWRITE_TECHNOLOGY
    if ( fp.technology == wxSTC_TECHNOLOGY_DIRECTWRITE ) {
        newFont->SetSurfaceFontData(new SurfaceFontDataD2D(fp));
    }
#endif // HAVE_DIRECTWRITE_TECHNOLOGY
}


void Font::Release() {
    if (fid)
        delete wxFontWithAscent::FromFID(fid);
    fid = nullptr;
}

//----------------------------------------------------------------------

class SurfaceImpl : public Surface {
private:
    wxDC*       hdc;
    bool        hdcOwned;
    wxBitmap*   bitmap;
    int         x;
    int         y;
    bool        unicodeMode;

public:
    SurfaceImpl();
    ~SurfaceImpl();

    virtual void Init(WindowID wid) override;
    virtual void Init(SurfaceID sid, WindowID wid) override;
    virtual void InitPixMap(int width, int height, Surface *surface_, WindowID wid) override;

    virtual void Release() override;
    virtual bool Initialised() override;
    virtual void PenColour(ColourDesired fore) override;
    virtual int LogPixelsY() override;
    virtual int DeviceHeightFont(int points) override;
    virtual void MoveTo(int x_, int y_) override;
    virtual void LineTo(int x_, int y_) override;
    virtual void Polygon(Point *pts, size_t npts, ColourDesired fore, ColourDesired back) override;
    virtual void RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back) override;
    virtual void FillRectangle(PRectangle rc, ColourDesired back) override;
    virtual void FillRectangle(PRectangle rc, Surface &surfacePattern) override;
    virtual void RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back) override;
    virtual void AlphaRectangle(PRectangle rc, int cornerSize, ColourDesired fill, int alphaFill,
                                ColourDesired outline, int alphaOutline, int flags) override;
    virtual void GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops,
                                   GradientOptions options) override;
    virtual void DrawRGBAImage(PRectangle rc, int width, int height,
                               const unsigned char *pixelsImage) override;
    virtual void Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back) override;
    virtual void Copy(PRectangle rc, Point from, Surface &surfaceSource) override;

    virtual void DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back) override;
    virtual void DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back) override;
    virtual void DrawTextTransparent(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore) override;
    virtual void MeasureWidths(Font &font_, const char *s, int len, XYPOSITION *positions) override;
    virtual XYPOSITION WidthText(Font &font_, const char *s, int len) override;
    virtual XYPOSITION Ascent(Font &font_) override;
    virtual XYPOSITION Descent(Font &font_) override;
    virtual XYPOSITION InternalLeading(Font &font_) override;
    virtual XYPOSITION Height(Font &font_) override;
    virtual XYPOSITION AverageCharWidth(Font &font_) override;

    virtual void SetClip(PRectangle rc) override;
    virtual void FlushCachedState() override;

    virtual void SetUnicodeMode(bool unicodeMode_) override;
    virtual void SetDBCSMode(int codePage) override;

    void BrushColour(ColourDesired back);
    void SetFont(Font &font_);
};



SurfaceImpl::SurfaceImpl() :
    hdc(nullptr), hdcOwned(0), bitmap(nullptr),
    x(0), y(0), unicodeMode(0)
{}

SurfaceImpl::~SurfaceImpl() {
    Release();
}

void SurfaceImpl::Init(WindowID wid) {
#if 0
    Release();
    hdc = new wxMemoryDC();
    hdcOwned = true;
#else
    // On Mac and GTK the DC is not really valid until it has a bitmap
    // selected into it.  So instead of just creating the DC with no bitmap,
    // go ahead and give it one.
    InitPixMap(1,1,nullptr,wid);
#endif
}

void SurfaceImpl::Init(SurfaceID hdc_, WindowID) {
    Release();
    hdc = (wxDC*)hdc_;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface *surface, WindowID winid) {
    Release();
    wxMemoryDC* mdc = surface
        ? new wxMemoryDC(static_cast<SurfaceImpl*>(surface)->hdc)
        : new wxMemoryDC();
    hdc = mdc;
    hdcOwned = true;
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    bitmap = new wxBitmap(GETWIN(winid)->ToPhys(wxSize(width, height)));
    bitmap->SetScaleFactor(GETWIN(winid)->GetDPIScaleFactor());
    mdc->SelectObject(*bitmap);
}


void SurfaceImpl::Release() {
    if (bitmap) {
        ((wxMemoryDC*)hdc)->SelectObject(wxNullBitmap);
        delete bitmap;
        bitmap = nullptr;
    }
    if (hdcOwned) {
        delete hdc;
        hdc = nullptr;
        hdcOwned = false;
    }
}


bool SurfaceImpl::Initialised() {
    return hdc != nullptr;
}


void SurfaceImpl::PenColour(ColourDesired fore) {
    hdc->SetPen(wxPen(wxColourFromCD(fore)));
}

void SurfaceImpl::BrushColour(ColourDesired back) {
    hdc->SetBrush(wxBrush(wxColourFromCD(back)));
}

void SurfaceImpl::SetFont(Font &font_) {
  if (font_.GetID()) {
      hdc->SetFont(*((wxFont*)font_.GetID()));
    }
}

int SurfaceImpl::LogPixelsY() {
    return hdc->GetPPI().y;
}

int SurfaceImpl::DeviceHeightFont(int points) {
    return points;
}

void SurfaceImpl::MoveTo(int x_, int y_) {
    x = x_;
    y = y_;
}

void SurfaceImpl::LineTo(int x_, int y_) {
    hdc->DrawLine(x,y, x_,y_);
    x = x_;
    y = y_;
}

void SurfaceImpl::Polygon(Point *pts, size_t npts, ColourDesired fore, ColourDesired back) {
    PenColour(fore);
    BrushColour(back);
    wxPoint *p = new wxPoint[npts];

    for (size_t i=0; i<npts; i++) {
        p[i].x = wxRound(pts[i].x);
        p[i].y = wxRound(pts[i].y);
    }
    hdc->DrawPolygon(npts, p);
    delete [] p;
}

void SurfaceImpl::RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourDesired back) {
    BrushColour(back);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern) {
    wxBrush br;
    if (((SurfaceImpl&)surfacePattern).bitmap)
        br = wxBrush(*((SurfaceImpl&)surfacePattern).bitmap);
    else    // Something is wrong so display in red
        br = wxBrush(*wxRED);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->SetBrush(br);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawRoundedRectangle(wxRectFromPRectangle(rc), 4);
}

#if defined(__WXMSW__) || defined(__WXMAC__)
#define wxPy_premultiply(p, a)   ((p) * (a) / 0xff)
#else
#define wxPy_premultiply(p, a)   (p)
#endif

void SurfaceImpl::AlphaRectangle(PRectangle rc, int cornerSize,
                                 ColourDesired fill, int alphaFill,
                                 ColourDesired outline, int alphaOutline,
                                 int /*flags*/) {
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC dc(*(wxMemoryDC*)hdc);
    wxColour penColour(wxColourFromCDandAlpha(outline, alphaOutline));
    wxColour brushColour(wxColourFromCDandAlpha(fill, alphaFill));
    dc.SetPen(wxPen(penColour));
    dc.SetBrush(wxBrush(brushColour));
    dc.DrawRoundedRectangle(wxRectFromPRectangle(rc), cornerSize);
    return;
#else

#ifdef wxHAS_RAW_BITMAP

    // TODO:  do something with cornerSize
    wxUnusedVar(cornerSize);

    wxRect r = wxRectFromPRectangle(rc);
    wxBitmap bmp(r.width, r.height, 32);

    // This block is needed to ensure that the changes done to the bitmap via
    // pixel data object are committed before the bitmap is drawn.
    {
        int px, py;
        wxAlphaPixelData pixData(bmp);

        // Set the fill pixels
        ColourDesired cdf(fill.AsInteger());
        int red   = cdf.GetRed();
        int green = cdf.GetGreen();
        int blue  = cdf.GetBlue();

        wxAlphaPixelData::Iterator p(pixData);
        for (py=0; py<r.height; py++) {
            p.MoveTo(pixData, 0, py);
            for (px=0; px<r.width; px++) {
                p.Red()   = wxPy_premultiply(red,   alphaFill);
                p.Green() = wxPy_premultiply(green, alphaFill);
                p.Blue()  = wxPy_premultiply(blue,  alphaFill);
                p.Alpha() = alphaFill;
                ++p;
            }
        }

        // Set the outline pixels
        ColourDesired cdo(outline.AsInteger());
        red   = cdo.GetRed();
        green = cdo.GetGreen();
        blue  = cdo.GetBlue();
        for (px=0; px<r.width; px++) {
            p.MoveTo(pixData, px, 0);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
            p.MoveTo(pixData, px, r.height-1);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
        }

        for (py=0; py<r.height; py++) {
            p.MoveTo(pixData, 0, py);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
            p.MoveTo(pixData, r.width-1, py);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
        }
    }

    // Draw the bitmap
    hdc->DrawBitmap(bmp, r.x, r.y, true);

#else
    wxUnusedVar(cornerSize);
    wxUnusedVar(alphaFill);
    wxUnusedVar(alphaOutline);
    RectangleDraw(rc, outline, fill);
#endif
#endif
}

void SurfaceImpl::GradientRectangle(PRectangle rc,
                                    const std::vector<ColourStop>& stops,
                                    GradientOptions options)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsGradientStops gradientStops;
    for ( size_t i = 0; i < stops.size(); ++i )
    {
        if ( i == 0 )
            gradientStops.SetStartColour(wxColourFromCA(stops[i].colour));
        else if ( i == stops.size() - 1 )
            gradientStops.SetEndColour(wxColourFromCA(stops[i].colour));
        else
            gradientStops.Add(wxColourFromCA(stops[i].colour), stops[i].position);
    }

    wxPoint ep;
    switch ( options )
    {
        case GradientOptions::leftToRight:
            ep = wxPoint(rc.right, rc.top);
            break;
        case GradientOptions::topToBottom:
        default:
            ep = wxPoint(rc.left, rc.bottom);
            break;
    }

    wxGCDC dc(*(wxMemoryDC*)hdc);
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    gc->SetBrush(gc->CreateLinearGradientBrush(rc.left, rc.top, ep.x, ep.y, gradientStops));
    gc->DrawRectangle(rc.left, rc.top, rc.Width(), rc.Height());
#else
    // limited implementation that only uses the first and last stop
    wxDirection dir;
    switch ( options )
    {
        case GradientOptions::leftToRight:
            dir = wxEAST;
            break;
        case GradientOptions::topToBottom:
        default:
            dir = wxSOUTH;
            break;
    }

    hdc->GradientFillLinear(wxRectFromPRectangle(rc),
                            wxColourFromCA(stops[0].colour),
                            wxColourFromCA(stops[stops.size() - 1].colour),
                            dir);
#endif
}

#ifdef wxHAS_RAW_BITMAP
wxBitmap BitmapFromRGBAImage(int width, int height, const unsigned char *pixelsImage)
{
    int x, y;
    wxBitmap bmp(width, height, 32);
    wxAlphaPixelData pixData(bmp);

    wxAlphaPixelData::Iterator p(pixData);
    for (y=0; y<height; y++) {
        p.MoveTo(pixData, 0, y);
        for (x=0; x<width; x++) {
            unsigned char red   = *pixelsImage++;
            unsigned char green = *pixelsImage++;
            unsigned char blue  = *pixelsImage++;
            unsigned char alpha = *pixelsImage++;

            p.Red()   = wxPy_premultiply(red,   alpha);
            p.Green() = wxPy_premultiply(green, alpha);
            p.Blue()  = wxPy_premultiply(blue,  alpha);
            p.Alpha() = alpha;
            ++p;
        }
    }
    return bmp;
}
#else
wxBitmap BitmapFromRGBAImage(int width, int height, const unsigned char *pixelsImage)
{
    const int totalPixels = width * height;
    wxScopedArray<unsigned char> data(3*totalPixels);
    wxScopedArray<unsigned char> alpha(totalPixels);
    int curDataLocation = 0, curAlphaLocation = 0, curPixelsImageLocation = 0;

    for ( int i = 0; i < totalPixels; ++i )
    {
        data[curDataLocation++] = pixelsImage[curPixelsImageLocation++];
        data[curDataLocation++] = pixelsImage[curPixelsImageLocation++];
        data[curDataLocation++] = pixelsImage[curPixelsImageLocation++];
        alpha[curAlphaLocation++] = pixelsImage[curPixelsImageLocation++];
    }

    wxImage img(width, height, data.get(), alpha.get(), true);
    wxBitmap bmp(img);

    return bmp;
}
#endif


void SurfaceImpl::DrawRGBAImage(PRectangle rc, int width, int height,
                                const unsigned char *pixelsImage)
{
    wxRect r = wxRectFromPRectangle(rc);
    wxBitmap bmp = BitmapFromRGBAImage(width, height, pixelsImage);
    hdc->DrawBitmap(bmp, r.x, r.y, true);
}


void SurfaceImpl::Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawEllipse(wxRectFromPRectangle(rc));
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
    wxRect r = wxRectFromPRectangle(rc);
    hdc->Blit(r.x, r.y, r.width, r.height,
              ((SurfaceImpl&)surfaceSource).hdc,
              wxRound(from.x), wxRound(from.y), wxCOPY);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font, XYPOSITION ybase,
                                 const char *s, int len,
                                 ColourDesired fore, ColourDesired back) {
    SetFont(font);
    hdc->SetTextForeground(wxColourFromCD(fore));
    hdc->SetTextBackground(wxColourFromCD(back));
    FillRectangle(rc, back);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hdc->DrawText(stc2wx(s, len), wxRound(rc.left), wxRound(ybase - GetAscent(font)));
}

void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font, XYPOSITION ybase,
                                  const char *s, int len,
                                  ColourDesired fore, ColourDesired back) {
    SetFont(font);
    hdc->SetTextForeground(wxColourFromCD(fore));
    hdc->SetTextBackground(wxColourFromCD(back));
    FillRectangle(rc, back);
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));

    // see comments above
    hdc->DrawText(stc2wx(s, len), wxRound(rc.left), wxRound(ybase - GetAscent(font)));
    hdc->DestroyClippingRegion();
}


void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font, XYPOSITION ybase,
                                      const char *s, int len,
                                      ColourDesired fore) {

    SetFont(font);
    hdc->SetTextForeground(wxColourFromCD(fore));
    hdc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hdc->DrawText(stc2wx(s, len), wxRound(rc.left), wxRound(ybase - GetAscent(font)));

    hdc->SetBackgroundMode(wxBRUSHSTYLE_SOLID);
}


void SurfaceImpl::MeasureWidths(Font &font, const char *s, int len, XYPOSITION *positions) {

    wxString   str = stc2wx(s, len);
    wxArrayInt tpos;

    SetFont(font);

    hdc->GetPartialTextExtents(str, tpos);

    // Map the widths back to the UTF-8 input string
    size_t utf8i = 0;
    for (size_t wxi = 0; wxi < str.size(); ++wxi) {
        wxUniChar c = str[wxi];

#if SIZEOF_WCHAR_T == 2
        // For surrogate pairs, the position for the lead surrogate is garbage
        // and we need to use the position of the trail surrogate for all four bytes
        if (c >= 0xD800 && c < 0xE000 && wxi + 1 < str.size()) {
            ++wxi;
            positions[utf8i++] = tpos[wxi];
            positions[utf8i++] = tpos[wxi];
            positions[utf8i++] = tpos[wxi];
            positions[utf8i++] = tpos[wxi];
            continue;
        }
#endif

        positions[utf8i++] = tpos[wxi];
        if (c >= 0x80)
            positions[utf8i++] = tpos[wxi];
        if (c >= 0x800)
            positions[utf8i++] = tpos[wxi];
        if (c >= 0x10000)
            positions[utf8i++] = tpos[wxi];
    }
}


XYPOSITION SurfaceImpl::WidthText(Font &font, const char *s, int len) {
    SetFont(font);
    int w;
    int h;

    hdc->GetTextExtent(stc2wx(s, len), &w, &h);
    return w;
}

#define EXTENT_TEST wxT(" `~!@#$%^&*()-_=+\\|[]{};:\"\'<,>.?/1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

XYPOSITION SurfaceImpl::Ascent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    const int ascent = h - d;
    SetAscent(font, ascent);
    return ascent;
}

XYPOSITION SurfaceImpl::Descent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return d;
}

XYPOSITION SurfaceImpl::InternalLeading(Font &WXUNUSED(font)) {
    return 0;
}

XYPOSITION SurfaceImpl::Height(Font &font) {
    SetFont(font);
    return hdc->GetCharHeight() + 1;
}

XYPOSITION SurfaceImpl::AverageCharWidth(Font &font) {
    SetFont(font);
    return hdc->GetCharWidth();
}

void SurfaceImpl::SetClip(PRectangle rc) {
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FlushCachedState() {
}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) {
    unicodeMode=unicodeMode_;
}

void SurfaceImpl::SetDBCSMode(int WXUNUSED(codePage)) {
    // dbcsMode = codePage == SC_CP_DBCS;
}

} // namespace scintilla


#ifdef HAVE_DIRECTWRITE_TECHNOLOGY

//----------------------------------------------------------------------
// SurfaceFontDataD2D

SurfaceFontDataD2D::SurfaceFontDataD2D(const FontParameters& fp)
{
    wxCOMPtr<IDWriteFactory> pDWriteFactory(::wxDWriteFactory());

    if ( pDWriteFactory.get() )
    {
        wxCOMPtr<IDWriteTextLayout> pTextLayout;
        wxString faceName = fp.faceName;
        wxString extentTest(EXTENT_TEST);
        DWRITE_FONT_STYLE fontStyle;
        DWRITE_TEXT_METRICS textmetrics = {0,0,0,0,0,0,0,0,0};
        const int maxLines = 2;
        DWRITE_LINE_METRICS lineMetrics[maxLines];
        UINT32 lineCount = 0;
        FLOAT emHeight = 0;
        HRESULT hr;

        fontStyle = fp.italic?DWRITE_FONT_STYLE_ITALIC:DWRITE_FONT_STYLE_NORMAL;

        hr = pDWriteFactory->CreateTextFormat(
            faceName.wc_str(),
            nullptr,
            static_cast<DWRITE_FONT_WEIGHT>(fp.weight),
            fontStyle,
            DWRITE_FONT_STRETCH_NORMAL,
            fp.size,
            L"en-us", //locale
            &m_pTextFormat
            );

        if ( SUCCEEDED(hr) )
        {
            m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

            hr = pDWriteFactory->CreateTextLayout(extentTest.wc_str(),
                extentTest.length(), m_pTextFormat, FLT_MAX, FLT_MAX,
                &pTextLayout);
        }

        if ( SUCCEEDED(hr) )
        {
            hr = pTextLayout->GetMetrics(&textmetrics);
        }

        if ( SUCCEEDED(hr) )
        {
            hr = pTextLayout->GetLineMetrics(lineMetrics, maxLines, &lineCount);
        }

        if ( SUCCEEDED(hr) )
        {
            hr = pTextLayout->GetFontSize(0, &emHeight);
        }

        if ( SUCCEEDED(hr) )
        {
            m_ascent = lineMetrics[0].baseline;
            m_descent = lineMetrics[0].height - lineMetrics[0].baseline;
            m_internalLeading = lineMetrics[0].height - emHeight;
            m_averageCharWidth = textmetrics.width/extentTest.length();

            switch ( fp.extraFontFlag & wxSTC_EFF_QUALITY_MASK )
            {
                case wxSTC_EFF_QUALITY_NON_ANTIALIASED:
                    m_aaMode = D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
                    break;
                case wxSTC_EFF_QUALITY_ANTIALIASED:
                    m_aaMode = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
                    break;
                case wxSTC_EFF_QUALITY_LCD_OPTIMIZED:
                    m_aaMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
                    break;
                default:
                    m_aaMode = D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
                    break;
            }

            m_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,
                lineMetrics[0].height, m_ascent);
        }
        else
        {
            m_pTextFormat.reset();
        }
    }
}

bool SurfaceFontDataD2D::Initialised() const
{
    return (m_pTextFormat.get() != nullptr);
}

//----------------------------------------------------------------------
// SurfaceDataD2D

SurfaceDataD2D::SurfaceDataD2D(ScintillaWX* editor)
    : m_pD2DFactory(::wxD2D1Factory()),
      m_pDWriteFactory(::wxDWriteFactory()),
      m_editor(editor)
{
    if ( Initialised() )
    {
        HRESULT hr =
            m_pDWriteFactory->CreateRenderingParams(&m_defaultRenderingParams);

        if ( SUCCEEDED(hr) )
        {
            unsigned int clearTypeContrast;
            if ( ::SystemParametersInfo(SPI_GETFONTSMOOTHINGCONTRAST, 0,
                    &clearTypeContrast, 0) )
            {
                FLOAT gamma;
                if (clearTypeContrast >= 1000 && clearTypeContrast <= 2200)
                    gamma = static_cast<FLOAT>(clearTypeContrast) / 1000.0f;
                else
                    gamma = m_defaultRenderingParams->GetGamma();

                m_pDWriteFactory->CreateCustomRenderingParams(
                    gamma,
                    m_defaultRenderingParams->GetEnhancedContrast(),
                    m_defaultRenderingParams->GetClearTypeLevel(),
                    m_defaultRenderingParams->GetPixelGeometry(),
                    m_defaultRenderingParams->GetRenderingMode(),
                    &m_customClearTypeRenderingParams);
            }
        }
    }
}

bool SurfaceDataD2D::Initialised() const
{
    return (m_pD2DFactory.get() != nullptr && m_pDWriteFactory.get() != nullptr);
}

void SurfaceDataD2D::DiscardGraphicsResources()
{
    m_pRenderTarget.reset();
    m_pSolidBrush.reset();
    m_pPatternBrush.reset();
}

HRESULT SurfaceDataD2D::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if ( m_pRenderTarget.get() == nullptr )
    {
        D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_PREMULTIPLIED));

        hr = m_pD2DFactory->CreateDCRenderTarget(
            &renderTargetProperties,
            &m_pRenderTarget);

        if ( SUCCEEDED(hr) )
        {
            const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
            hr = m_pRenderTarget->CreateSolidColorBrush(color, &m_pSolidBrush);
        }

        if ( SUCCEEDED(hr) )
        {
            D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
                D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP,
                    D2D1_EXTEND_MODE_WRAP,
                    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

            wxCOMPtr<ID2D1Bitmap> bitmap;

            D2D1_PIXEL_FORMAT pixel_format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED);


            m_pRenderTarget->CreateBitmap(D2D1::SizeU(1,1),
                                          D2D1::BitmapProperties(pixel_format),
                                          &bitmap);

            hr = m_pRenderTarget->CreateBitmapBrush(bitmap, brushProperties,
                                                    &m_pPatternBrush);
        }
    }
    return hr;
}

void SurfaceDataD2D::SetEditorPaintAbandoned()
{
    m_editor->SetPaintAbandoned();
}

//----------------------------------------------------------------------
// SurfaceD2D - This is basically SurfaceD2D from the Scintilla source
// code adapted to draw in the wxMSW environment.

class SurfaceD2D : public Surface
{
public:
    SurfaceD2D();
    virtual ~SurfaceD2D();

    // base class virtuals
    virtual void Init(WindowID wid) override;
    virtual void Init(SurfaceID sid, WindowID wid) override;
    virtual void InitPixMap(int width, int height, Surface *surface_,
                            WindowID wid) override;

    virtual void Release() override;
    virtual bool Initialised() override;
    virtual void PenColour(ColourDesired fore) override;
    virtual int LogPixelsY() override;
    virtual int DeviceHeightFont(int points) override;
    virtual void MoveTo(int x_, int y_) override;
    virtual void LineTo(int x_, int y_) override;
    virtual void Polygon(Point *pts, size_t npts, ColourDesired fore,
                         ColourDesired back) override;
    virtual void RectangleDraw(PRectangle rc, ColourDesired fore,
                               ColourDesired back) override;
    virtual void FillRectangle(PRectangle rc, ColourDesired back) override;
    virtual void FillRectangle(PRectangle rc, Surface &surfacePattern) override;
    virtual void RoundedRectangle(PRectangle rc, ColourDesired fore,
                                  ColourDesired back) override;
    virtual void AlphaRectangle(PRectangle rc, int cornerSize,
                                ColourDesired fill, int alphaFill,
                                ColourDesired outline, int alphaOutline,
                                int flags) override;
    virtual void GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops,
                                   GradientOptions options) override;
    virtual void DrawRGBAImage(PRectangle rc, int width, int height,
                               const unsigned char *pixelsImage) override;
    virtual void Ellipse(PRectangle rc, ColourDesired fore,
                         ColourDesired back) override;
    virtual void Copy(PRectangle rc, Point from, Surface &surfaceSource) override;

    virtual void DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase,
                                const char *s, int len, ColourDesired fore,
                                ColourDesired back) override;
    virtual void DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase,
                                 const char *s, int len, ColourDesired fore,
                                 ColourDesired back) override;
    virtual void DrawTextTransparent(PRectangle rc, Font &font_,
                                     XYPOSITION ybase, const char *s, int len,
                                     ColourDesired fore) override;
    virtual void MeasureWidths(Font &font_, const char *s, int len,
                               XYPOSITION *positions) override;
    virtual XYPOSITION WidthText(Font &font_, const char *s, int len) override;
    virtual XYPOSITION Ascent(Font &font_) override;
    virtual XYPOSITION Descent(Font &font_) override;
    virtual XYPOSITION InternalLeading(Font &font_) override;
    virtual XYPOSITION Height(Font &font_) override;
    virtual XYPOSITION AverageCharWidth(Font &font_) override;

    virtual void SetClip(PRectangle rc) override;
    virtual void FlushCachedState() override;

    virtual void SetUnicodeMode(bool unicodeMode_) override;
    virtual void SetDBCSMode(int codePage) override;

    // helpers
    void SetFont(Font &font_);
    HRESULT FlushDrawing();
    void D2DPenColour(ColourDesired fore, int alpha=255);
    void DrawTextCommon(PRectangle rc, Font &font_, XYPOSITION ybase,
                        const char *s, int len, UINT fuOptions);

private:
    // Private so SurfaceD2D objects can not be copied
    SurfaceD2D(const SurfaceD2D &);
    SurfaceD2D &operator=(const SurfaceD2D &);

    bool m_unicodeMode;
    int m_x, m_y;
    int m_logPixelsY;

    bool m_ownRenderTarget;
    int m_clipsActive;

    XYPOSITION m_yAscent;
    XYPOSITION m_yDescent;
    XYPOSITION m_yInternalLeading;
    XYPOSITION m_averageCharWidth;

    wxCOMPtr<IDWriteFactory> m_pDWriteFactory;
    wxCOMPtr<ID2D1RenderTarget> m_pRenderTarget;
    wxCOMPtr<ID2D1SolidColorBrush> m_pSolidBrush;
    wxCOMPtr<ID2D1BitmapBrush> m_pPatternBrush;
    wxCOMPtr<IDWriteTextFormat> m_pTextFormat;

    SurfaceDataD2D* m_surfaceData;
    FontID m_curFontID;
};

SurfaceD2D::SurfaceD2D()
    : m_pDWriteFactory(::wxDWriteFactory())
{
    m_unicodeMode = false;
    m_x = 0;
    m_y = 0;
    m_logPixelsY = 72;

    m_ownRenderTarget = false;
    m_clipsActive = 0;

    m_yAscent = 2;
    m_yDescent = 1;
    m_yInternalLeading = 0;
    m_averageCharWidth = 1;

    m_surfaceData = nullptr;
    m_curFontID = nullptr;
}

SurfaceD2D::~SurfaceD2D()
{
    Release();
}

void SurfaceD2D::Init(WindowID wid)
{
    Release();

    m_logPixelsY = GETWIN(wid)->GetDPI().GetY();
}

void SurfaceD2D::Init(SurfaceID sid, WindowID wid)
{
    Release();

    wxWindow* win = GETWIN(wid);
    if ( win && win->GetName() == "wxSTCCallTip" )
        win = win->GetParent();

    wxStyledTextCtrl* const stc = wxDynamicCast(win, wxStyledTextCtrl);
    if ( stc )
    {
        wxDC* const dc = static_cast<wxDC*>(sid);
        const wxSize sz = dc->GetSize();
        m_logPixelsY = win->GetDPI().GetY();
        ScintillaWX* const
            sciwx = reinterpret_cast<ScintillaWX*>(stc->GetDirectPointer());
        m_surfaceData = static_cast<SurfaceDataD2D*>(sciwx->GetSurfaceData());
        HRESULT hr = m_surfaceData->CreateGraphicsResources();

        if ( SUCCEEDED(hr) )
        {
            RECT rc;
            ::SetRect(&rc,0,0,sz.GetWidth(),sz.GetHeight());
            hr = m_surfaceData->GetRenderTarget()
                ->BindDC(reinterpret_cast<HDC>(dc->GetHandle()),&rc);
        }

        if ( SUCCEEDED(hr) )
        {
            m_pRenderTarget.reset(m_surfaceData->GetRenderTarget());
            m_pSolidBrush.reset(m_surfaceData->GetSolidBrush());
            m_pPatternBrush.reset(m_surfaceData->GetPatternBrush());
            m_pRenderTarget->BeginDraw();
        }
    }
}

void SurfaceD2D::InitPixMap(int width, int height, Surface *surface_, WindowID)
{
    Release();

    SurfaceD2D *psurfOther = static_cast<SurfaceD2D *>(surface_);
    wxCOMPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;
    D2D1_SIZE_F desiredSize = D2D1::SizeF(static_cast<float>(width),
        static_cast<float>(height));
    D2D1_PIXEL_FORMAT desiredFormat;

#ifdef __MINGW32__
    desiredFormat.format = DXGI_FORMAT_UNKNOWN;
#else
    desiredFormat = psurfOther->m_pRenderTarget->GetPixelFormat();
#endif

    desiredFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    HRESULT hr = psurfOther->m_pRenderTarget->CreateCompatibleRenderTarget(
        &desiredSize, nullptr, &desiredFormat,
        D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &pCompatibleRenderTarget);
    if ( SUCCEEDED(hr) )
    {
        m_pRenderTarget = pCompatibleRenderTarget;
        m_pRenderTarget->BeginDraw();
        m_ownRenderTarget = true;
    }
    SetUnicodeMode(psurfOther->m_unicodeMode);
    m_logPixelsY = psurfOther->LogPixelsY();
    m_surfaceData = psurfOther->m_surfaceData;
}

void SurfaceD2D::Release()
{
    if ( m_pRenderTarget.get() )
    {
        HRESULT hr;
        while ( m_clipsActive )
        {
            m_pRenderTarget->PopAxisAlignedClip();
            m_clipsActive--;
        }
        hr = m_pRenderTarget->EndDraw();
        if ( hr == (HRESULT)D2DERR_RECREATE_TARGET && m_surfaceData && !m_ownRenderTarget )
        {
            m_surfaceData->DiscardGraphicsResources();
            m_surfaceData->SetEditorPaintAbandoned();
        }
    }

    m_pRenderTarget.reset();
    m_pSolidBrush.reset();
    m_pPatternBrush.reset();
    m_pTextFormat.reset();
    m_curFontID = nullptr;
}

bool SurfaceD2D::Initialised()
{
    return m_pRenderTarget.get() != nullptr;
}

void SurfaceD2D::PenColour(ColourDesired fore)
{
    D2DPenColour(fore);
}

int SurfaceD2D::LogPixelsY()
{
    return m_logPixelsY;
}

int SurfaceD2D::DeviceHeightFont(int points)
{
    return ::MulDiv(points, LogPixelsY(), 72);
}

void SurfaceD2D::MoveTo(int x_, int y_)
{
    m_x = x_;
    m_y = y_;
}

static int Delta(int difference)
{
    if ( difference < 0 )
        return -1;
    else if ( difference > 0 )
        return 1;
    else
        return 0;
}

static float RoundFloat(float f)
{
    return float(int(f+0.5f));
}

void SurfaceD2D::LineTo(int x_, int y_)
{
    wxCHECK( Initialised(), void() );

    int xDiff = x_ - m_x;
    int xDelta = Delta(xDiff);
    int yDiff = y_ - m_y;
    int yDelta = Delta(yDiff);
    if ( (xDiff == 0) || (yDiff == 0) )
    {
        // Horizontal or vertical lines can be more precisely drawn as a
        // filled rectangle
        int xEnd = x_ - xDelta;
        int left = wxMin(m_x, xEnd);
        int width = abs(m_x - xEnd) + 1;
        int yEnd = y_ - yDelta;
        int top = wxMin(m_y, yEnd);
        int height = abs(m_y - yEnd) + 1;
        D2D1_RECT_F rectangle1 = D2D1::RectF(static_cast<float>(left),
            static_cast<float>(top), static_cast<float>(left + width),
            static_cast<float>(top + height));
        m_pRenderTarget->FillRectangle(&rectangle1, m_pSolidBrush);
    }
    else if ( (abs(xDiff) == abs(yDiff)) )
    {
        // 45 degree slope
        m_pRenderTarget->DrawLine(D2D1::Point2F(m_x + 0.5f, m_y + 0.5f),
            D2D1::Point2F(x_ + 0.5f - xDelta, y_ + 0.5f - yDelta),
            m_pSolidBrush);
    }
    else
    {
        // Line has a different slope so difficult to avoid last pixel
        m_pRenderTarget->DrawLine(D2D1::Point2F(m_x + 0.5f, m_y + 0.5f),
            D2D1::Point2F(x_ + 0.5f, y_ + 0.5f), m_pSolidBrush);
    }
    m_x = x_;
    m_y = y_;
}

void SurfaceD2D::Polygon(Point *pts, size_t npts, ColourDesired fore,
                         ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    wxCOMPtr<ID2D1Factory> pFactory;
    wxCOMPtr<ID2D1PathGeometry> geometry;
    wxCOMPtr<ID2D1GeometrySink> sink;

    m_pRenderTarget->GetFactory(&pFactory);

    HRESULT hr = pFactory->CreatePathGeometry(&geometry);
    if ( SUCCEEDED(hr) )
    {
        hr = geometry->Open(&sink);
    }

    if ( SUCCEEDED(hr) )
    {
        sink->BeginFigure(D2D1::Point2F(pts[0].x + 0.5f, pts[0].y + 0.5f),
            D2D1_FIGURE_BEGIN_FILLED);
        for ( size_t i = 1; i < npts; i++ )
        {
            sink->AddLine(D2D1::Point2F(pts[i].x + 0.5f, pts[i].y + 0.5f));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        D2DPenColour(back);
        m_pRenderTarget->FillGeometry(geometry, m_pSolidBrush);
        D2DPenColour(fore);
        m_pRenderTarget->DrawGeometry(geometry, m_pSolidBrush);
    }
}

void SurfaceD2D::RectangleDraw(PRectangle rc, ColourDesired fore,
                               ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    D2D1_RECT_F rectangle1 = D2D1::RectF(RoundFloat(rc.left) + 0.5f,
        rc.top + 0.5f, RoundFloat(rc.right) - 0.5f, rc.bottom - 0.5f);
    D2DPenColour(back);
    m_pRenderTarget->FillRectangle(&rectangle1, m_pSolidBrush);
    D2DPenColour(fore);
    m_pRenderTarget->DrawRectangle(&rectangle1, m_pSolidBrush);
}

void SurfaceD2D::FillRectangle(PRectangle rc, ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    D2DPenColour(back);
    D2D1_RECT_F rectangle1 = D2D1::RectF(RoundFloat(rc.left), rc.top,
        RoundFloat(rc.right), rc.bottom);
    m_pRenderTarget->FillRectangle(&rectangle1, m_pSolidBrush);
}

void SurfaceD2D::FillRectangle(PRectangle rc, Surface &surfacePattern)
{
    wxCHECK( Initialised(), void() );

    SurfaceD2D &surfOther = static_cast<SurfaceD2D &>(surfacePattern);
    surfOther.FlushDrawing();

    wxCOMPtr<ID2D1Bitmap> pBitmap;
    wxCOMPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget(
        reinterpret_cast<ID2D1BitmapRenderTarget*>(
            surfOther.m_pRenderTarget.get()));

    HRESULT hr = pCompatibleRenderTarget->GetBitmap(&pBitmap);

    if ( SUCCEEDED(hr) )
    {
        if ( m_pPatternBrush.get() )
        {
            m_pPatternBrush->SetBitmap(pBitmap);
        }
        else
        {
            D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
                D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP,
                    D2D1_EXTEND_MODE_WRAP,
                    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

            hr = m_pRenderTarget->CreateBitmapBrush(pBitmap, brushProperties,
                &m_pPatternBrush);
        }
    }

    if ( SUCCEEDED(hr) )
    {
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),
            m_pPatternBrush);
    }
}

void SurfaceD2D::RoundedRectangle(PRectangle rc, ColourDesired fore,
                                  ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    D2D1_ROUNDED_RECT roundedRectFill = {
        D2D1::RectF(rc.left + 1.0f, rc.top + 1.0f, rc.right - 1.0f, rc.bottom - 1.0f),
        4, 4 };
    D2DPenColour(back);
    m_pRenderTarget->FillRoundedRectangle(roundedRectFill, m_pSolidBrush);

    D2D1_ROUNDED_RECT roundedRect = {
        D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right - 0.5f, rc.bottom - 0.5f),
        4, 4 };
    D2DPenColour(fore);
    m_pRenderTarget->DrawRoundedRectangle(roundedRect, m_pSolidBrush);
}

void SurfaceD2D::AlphaRectangle(PRectangle rc, int cornerSize,
                                ColourDesired fill, int alphaFill,
                                ColourDesired outline, int alphaOutline,
                                int /* flags*/ )
{
    wxCHECK( Initialised(), void() );

    if ( cornerSize == 0 )
    {
        // When corner size is zero, draw square rectangle to prevent
        // blurry pixels at corners
        D2D1_RECT_F rectFill = D2D1::RectF(RoundFloat(rc.left) + 1.0f,
            rc.top + 1.0f, RoundFloat(rc.right) - 1.0f, rc.bottom - 1.0f);
        D2DPenColour(fill, alphaFill);
        m_pRenderTarget->FillRectangle(rectFill, m_pSolidBrush);

        D2D1_RECT_F rectOutline = D2D1::RectF(RoundFloat(rc.left) + 0.5f,
            rc.top + 0.5f, RoundFloat(rc.right) - 0.5f, rc.bottom - 0.5f);
        D2DPenColour(outline, alphaOutline);
        m_pRenderTarget->DrawRectangle(rectOutline, m_pSolidBrush);
    }
    else
    {
        const float cornerSizeF = static_cast<float>(cornerSize);
        D2D1_ROUNDED_RECT roundedRectFill = {
            D2D1::RectF(RoundFloat(rc.left) + 1.0f, rc.top + 1.0f,
                RoundFloat(rc.right) - 1.0f, rc.bottom - 1.0f),
            cornerSizeF, cornerSizeF };
        D2DPenColour(fill, alphaFill);
        m_pRenderTarget->FillRoundedRectangle(roundedRectFill, m_pSolidBrush);

        D2D1_ROUNDED_RECT roundedRect = {
            D2D1::RectF(RoundFloat(rc.left) + 0.5f, rc.top + 0.5f,
                RoundFloat(rc.right) - 0.5f, rc.bottom - 0.5f),
            cornerSizeF, cornerSizeF };
        D2DPenColour(outline, alphaOutline);
        m_pRenderTarget->DrawRoundedRectangle(roundedRect, m_pSolidBrush);
    }
}

namespace {

D2D_COLOR_F ColorFromColourAlpha(ColourAlpha colour) noexcept {
    D2D_COLOR_F col;
    col.r = colour.GetRedComponent();
    col.g = colour.GetGreenComponent();
    col.b = colour.GetBlueComponent();
    col.a = colour.GetAlphaComponent();
    return col;
}

}

void SurfaceD2D::GradientRectangle(PRectangle rc,
                                   const std::vector<ColourStop>& stops,
                                   GradientOptions options)
{
    wxCHECK(Initialised(), void());

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lgbp;
    lgbp.startPoint = D2D1::Point2F(rc.left, rc.top);
    switch ( options )
    {
        case GradientOptions::leftToRight:
            lgbp.endPoint = D2D1::Point2F(rc.right, rc.top);
            break;
        case GradientOptions::topToBottom:
        default:
            lgbp.endPoint = D2D1::Point2F(rc.left, rc.bottom);
            break;
    }

    std::vector<D2D1_GRADIENT_STOP> gradientStops;
    for ( const ColourStop &stop : stops )
    {
        gradientStops.push_back({ stop.position, ColorFromColourAlpha(stop.colour) });
    }
    ID2D1GradientStopCollection *pGradientStops = nullptr;
    HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
        gradientStops.data(), static_cast<UINT32>(gradientStops.size()), &pGradientStops);
    if ( FAILED(hr) || !pGradientStops )
    {
        return;
    }
    ID2D1LinearGradientBrush *pBrushLinear = nullptr;
    hr = m_pRenderTarget->CreateLinearGradientBrush(
        lgbp, pGradientStops, &pBrushLinear);
    if ( SUCCEEDED(hr) && pBrushLinear )
    {
        const D2D1_RECT_F rectangle = D2D1::RectF(round(rc.left), rc.top, round(rc.right), rc.bottom);
        m_pRenderTarget->FillRectangle(&rectangle, pBrushLinear);
        pBrushLinear->Release();
    }
    pGradientStops->Release();
}

void SurfaceD2D::DrawRGBAImage(PRectangle rc, int width, int height,
                               const unsigned char *pixelsImage)
{
    wxCHECK( Initialised(), void() );

    if ( rc.Width() > width )
        rc.left += static_cast<int>((rc.Width() - width) / 2);
    rc.right = rc.left + width;
    if ( rc.Height() > height )
        rc.top += static_cast<int>((rc.Height() - height) / 2);
    rc.bottom = rc.top + height;

    wxVector<unsigned char> image(height * width * 4);
    for ( int yPixel = 0; yPixel < height; yPixel++ )
    {
        for ( int xPixel = 0; xPixel < width; xPixel++ )
        {
            unsigned char* pixel = &image[0] + (yPixel * width + xPixel) * 4;
            unsigned char alpha = pixelsImage[3];
            // Input is RGBA, output is BGRA with premultiplied alpha
            pixel[2] = (*pixelsImage++) * alpha / 255;
            pixel[1] = (*pixelsImage++) * alpha / 255;
            pixel[0] = (*pixelsImage++) * alpha / 255;
            pixel[3] = *pixelsImage++;
        }
    }

    wxCOMPtr<ID2D1Bitmap> bitmap;
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    D2D1_BITMAP_PROPERTIES props = { {DXGI_FORMAT_B8G8R8A8_UNORM,
        D2D1_ALPHA_MODE_PREMULTIPLIED}, 72.0, 72.0 };
    HRESULT hr = m_pRenderTarget->CreateBitmap(size, &image[0],
        width * 4, &props, &bitmap);

    if ( SUCCEEDED(hr) )
    {
        D2D1_RECT_F rcDestination = { rc.left, rc.top, rc.right, rc.bottom };
        m_pRenderTarget->DrawBitmap(bitmap, rcDestination);
    }
}

void SurfaceD2D::Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    FLOAT radius = rc.Width() / 2.0f;
    D2D1_ELLIPSE ellipse = {
        D2D1::Point2F((rc.left + rc.right) / 2.0f,
            (rc.top + rc.bottom) / 2.0f),
        radius, radius };

    PenColour(back);
    m_pRenderTarget->FillEllipse(ellipse, m_pSolidBrush);
    PenColour(fore);
    m_pRenderTarget->DrawEllipse(ellipse, m_pSolidBrush);
}

void SurfaceD2D::Copy(PRectangle rc, Point from, Surface& surfaceSource)
{
    wxCHECK( Initialised(), void() );

    SurfaceD2D& surfOther = static_cast<SurfaceD2D&>(surfaceSource);
    surfOther.FlushDrawing();

    wxCOMPtr<ID2D1Bitmap> pBitmap;
    wxCOMPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget(
        reinterpret_cast<ID2D1BitmapRenderTarget*>(surfOther.m_pRenderTarget.get()));

    HRESULT hr = pCompatibleRenderTarget->GetBitmap(&pBitmap);

    if ( SUCCEEDED(hr) )
    {
        D2D1_RECT_F rcDestination = { rc.left, rc.top, rc.right, rc.bottom };
        D2D1_RECT_F rcSource =
        { from.x, from.y, from.x + rc.Width(), from.y + rc.Height() };
        m_pRenderTarget->DrawBitmap(pBitmap, rcDestination, 1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, rcSource);

        hr = m_pRenderTarget->Flush();
        if ( FAILED(hr) )
        {
            Platform::DebugPrintf("Failed Flush 0x%x\n", hr);
        }
    }
}

void SurfaceD2D::DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase,
                                const char *s, int len, ColourDesired fore,
                                ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    FillRectangle(rc, back);
    D2DPenColour(fore);
    DrawTextCommon(rc, font_, ybase, s, len, ETO_OPAQUE);
}

void SurfaceD2D::DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase,
                                 const char *s, int len, ColourDesired fore,
                                 ColourDesired back)
{
    wxCHECK( Initialised(), void() );

    FillRectangle(rc, back);
    D2DPenColour(fore);
    DrawTextCommon(rc, font_, ybase, s, len, ETO_OPAQUE | ETO_CLIPPED);
}

void SurfaceD2D::DrawTextTransparent(PRectangle rc, Font &font_,
                                     XYPOSITION ybase, const char *s, int len,
                                     ColourDesired fore)
{
    wxCHECK( Initialised(), void() );

    // Avoid drawing spaces in transparent mode
    for ( int i=0; i<len; i++ )
    {
        if ( s[i] != ' ' )
        {
            D2DPenColour(fore);
            DrawTextCommon(rc, font_, ybase, s, len, 0);
            return;
        }
    }
}

XYPOSITION SurfaceD2D::WidthText(Font &font_, const char *s, int len)
{
    XYPOSITION width = 1.0;
    wxString tbuf = stc2wx(s,len);
    SetFont(font_);

    if ( m_pDWriteFactory.get() && m_pTextFormat.get() )
    {
        wxCOMPtr<IDWriteTextLayout> pTextLayout;
        DWRITE_TEXT_METRICS textMetrics = {0,0,0,0,0,0,0,0,0};

        HRESULT hr = m_pDWriteFactory->CreateTextLayout(tbuf.wc_str(),
            tbuf.length(), m_pTextFormat, FLT_MAX, FLT_MAX, &pTextLayout);

        if ( SUCCEEDED(hr) )
        {
            hr = pTextLayout->GetMetrics(&textMetrics);
        }

        if ( SUCCEEDED(hr) )
        {
            width = textMetrics.widthIncludingTrailingWhitespace;
        }
    }
    return width;
}

void SurfaceD2D::MeasureWidths(Font &font_, const char *s, int len,
                               XYPOSITION *positions)
{
    int fit = 0;
    wxString tbuf = stc2wx(s,len);
    wxVector<FLOAT> poses;
    poses.reserve(tbuf.length());
    poses.resize(tbuf.length());

    fit = tbuf.length();
    const int clusters = 1000;
    DWRITE_CLUSTER_METRICS clusterMetrics[clusters];
    UINT32 count = 0;
    SetFont(font_);

    if ( m_pDWriteFactory.get() && m_pTextFormat.get() )
    {
        wxCOMPtr<IDWriteTextLayout> pTextLayout;

        HRESULT hr = m_pDWriteFactory->CreateTextLayout(tbuf.wc_str(),
            tbuf.length(), m_pTextFormat, FLT_MAX, FLT_MAX, &pTextLayout);
        if ( !SUCCEEDED(hr) )
            return;

        hr = pTextLayout->GetClusterMetrics(clusterMetrics, clusters, &count);
        if ( !SUCCEEDED(hr) )
            return;

        // A cluster may be more than one WCHAR, such as for "ffi" which is a
        // ligature in the Candara font
        FLOAT position = 0.0f;
        size_t ti=0;
        for ( size_t ci=0; ci<count; ci++ )
        {
            for ( size_t inCluster=0; inCluster<clusterMetrics[ci].length;
                inCluster++)
            {
                poses[ti++] =
                    position + clusterMetrics[ci].width * (inCluster + 1) /
                        clusterMetrics[ci].length;
            }
            position += clusterMetrics[ci].width;
        }
        PLATFORM_ASSERT(ti == tbuf.length());
    }
    if (m_unicodeMode)
    {
        // Map the widths given for UTF-16 characters back onto the UTF-8 input
        // string
        int ui=0;
        const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
        int i=0;
        while ( ui<fit )
        {
            unsigned char uch = us[i];
            unsigned int lenChar = 1;
            if (uch >= (0x80 + 0x40 + 0x20 + 0x10))
            {
                lenChar = 4;
                ui++;
            }
            else if (uch >= (0x80 + 0x40 + 0x20))
            {
                lenChar = 3;
            }
            else if (uch >= (0x80))
            {
                lenChar = 2;
            }

            for ( unsigned int bytePos=0; (bytePos<lenChar) && (i<len); bytePos++ )
            {
                positions[i++] = poses[ui];
            }
            ui++;
        }
        XYPOSITION lastPos = 0.0f;
        if ( i > 0 )
            lastPos = positions[i-1];
        while ( i<len )
        {
            positions[i++] = lastPos;
        }
    }
    else
    {
        const size_t buflen = static_cast<size_t>(len);
        // One character per position
        PLATFORM_ASSERT(buflen == tbuf.length());
        for ( size_t kk=0; kk<buflen; kk++ )
        {
            positions[kk] = poses[kk];
        }
    }
}

XYPOSITION SurfaceD2D::Ascent(Font &font_)
{
    SetFont(font_);
    return ceil(m_yAscent);
}

XYPOSITION SurfaceD2D::Descent(Font &font_)
{
    SetFont(font_);
    return ceil(m_yDescent);
}

XYPOSITION SurfaceD2D::InternalLeading(Font &font_)
{
    SetFont(font_);
    return floor(m_yInternalLeading);
}

XYPOSITION SurfaceD2D::Height(Font &font_)
{
    return Ascent(font_) + Descent(font_);
}

XYPOSITION SurfaceD2D::AverageCharWidth(Font &font_)
{
    SetFont(font_);
    return m_averageCharWidth;
}

void SurfaceD2D::SetClip(PRectangle rc)
{
    wxCHECK( Initialised(), void() );

    D2D1_RECT_F rcClip = { rc.left, rc.top, rc.right, rc.bottom };
    m_pRenderTarget->PushAxisAlignedClip(rcClip, D2D1_ANTIALIAS_MODE_ALIASED);
    m_clipsActive++;
}

void SurfaceD2D::FlushCachedState()
{
}

void SurfaceD2D::SetUnicodeMode(bool unicodeMode_)
{
    m_unicodeMode=unicodeMode_;
}

void SurfaceD2D::SetDBCSMode(int WXUNUSED(codePage_))
{
}

void SurfaceD2D::SetFont(Font &font_)
{
    FontID id = font_.GetID();

    if (m_curFontID != id)
    {
        wxFontWithAscent *pfm = static_cast<wxFontWithAscent *>(id);
        SurfaceFontDataD2D* sfd =
            static_cast<SurfaceFontDataD2D*>(pfm->GetSurfaceFontData());

        if ( sfd->Initialised() )
        {
            m_pTextFormat.reset(sfd->GetFormat());
            m_yAscent = sfd->GetAscent();
            m_yDescent = sfd->GetDescent();
            m_yInternalLeading = sfd->GetInternalLeading();
            m_averageCharWidth = sfd->GetAverageCharWidth();

            if ( Initialised() )
            {
                D2D1_TEXT_ANTIALIAS_MODE aaMode = sfd->GetFontQuality();

                if ( m_surfaceData && m_surfaceData->Initialised() )
                {
                    if ( aaMode == D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE )
                    {
                        m_pRenderTarget->SetTextRenderingParams(
                            m_surfaceData->GetCustomClearTypeRenderingParams());
                    }
                    else
                    {
                        m_pRenderTarget->SetTextRenderingParams(
                            m_surfaceData->GetDefaultRenderingParams());
                    }
                }

                m_pRenderTarget->SetTextAntialiasMode(aaMode);
            }

            m_curFontID = id;
        }
    }
}

HRESULT SurfaceD2D::FlushDrawing()
{
    wxCHECK( Initialised(), E_POINTER );
    return m_pRenderTarget->Flush();
}

void SurfaceD2D::D2DPenColour(ColourDesired fore, int alpha)
{
    wxCHECK( Initialised(), void() );

    D2D_COLOR_F col;
    col.r = (fore.AsInteger() & 0xff) / 255.0f;
    col.g = ((fore.AsInteger() & 0xff00) >> 8) / 255.0f;
    col.b = (fore.AsInteger() >> 16) / 255.0f;
    col.a = alpha / 255.0f;
    if ( m_pSolidBrush.get() )
    {
        m_pSolidBrush->SetColor(col);
    }
    else
    {
        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(col, &m_pSolidBrush);
        if ( !SUCCEEDED(hr) )
        {
            m_pSolidBrush.reset();
        }
    }
}

void SurfaceD2D::DrawTextCommon(PRectangle rc, Font &font_, XYPOSITION ybase,
                                const char *s, int len, UINT fuOptions)
{
    wxCHECK( Initialised(), void() );

    SetFont(font_);

    if ( m_pSolidBrush.get() && m_pTextFormat.get() )
    {
        // Explicitly creating a text layout appears a little faster
        wxCOMPtr<IDWriteTextLayout> pTextLayout;
        wxString tbuf = stc2wx(s, len);
        HRESULT hr;

        if ( fuOptions & ETO_CLIPPED )
        {
            D2D1_RECT_F rcClip = {rc.left, rc.top, rc.right, rc.bottom};
            m_pRenderTarget->PushAxisAlignedClip(rcClip,
                D2D1_ANTIALIAS_MODE_ALIASED);
        }

        hr = m_pDWriteFactory->CreateTextLayout(tbuf.wc_str(), tbuf.length(),
            m_pTextFormat, rc.Width(), rc.Height(), &pTextLayout);

        if ( SUCCEEDED(hr) )
        {
            D2D1_POINT_2F origin = {rc.left, ybase-m_yAscent};
            m_pRenderTarget->DrawTextLayout(origin, pTextLayout, m_pSolidBrush,
                D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        if ( fuOptions & ETO_CLIPPED )
        {
            m_pRenderTarget->PopAxisAlignedClip();
        }
    }
}

#endif // HAVE_DIRECTWRITE_TECHNOLOGY

namespace Scintilla {

Surface *Surface::Allocate(int technology) {
    wxUnusedVar(technology);

#ifdef HAVE_DIRECTWRITE_TECHNOLOGY
    if ( technology == wxSTC_TECHNOLOGY_DIRECTWRITE ) {
        return new SurfaceD2D;
    }
#endif // HAVE_DIRECTWRITE_TECHNOLOGY
    return new SurfaceImpl;
}


//----------------------------------------------------------------------


Window::~Window() {
}

void Window::Destroy() {
    if (wid) {
        Show(false);
        GETWIN(wid)->Destroy();
    }
    wid = nullptr;
}


PRectangle Window::GetPosition() const {
    if (! wid) return PRectangle();
    wxRect rc(GETWIN(wid)->GetPosition(), GETWIN(wid)->GetSize());
    return PRectangleFromwxRect(rc);
}

void Window::SetPosition(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    GETWIN(wid)->SetSize(r);
}

void Window::SetPositionRelative(PRectangle rc, const Window *relativeTo) {
    wxWindow *relativeWin = GETWIN(relativeTo->wid);

    wxPoint position = relativeWin->GetScreenPosition();
    position.x = wxRound(position.x + rc.left);
    position.y = wxRound(position.y + rc.top);

    const wxRect displayRect = wxDisplay(relativeWin).GetClientArea();

    if (position.x < displayRect.GetLeft())
        position.x = displayRect.GetLeft();

    const int width = rc.Width();
    if (width > displayRect.GetWidth())
    {
        // We want to show at least the beginning of the window.
        position.x = displayRect.GetLeft();
    }
    else if (position.x + width > displayRect.GetRight())
        position.x = displayRect.GetRight() - width;

    const int height = rc.Height();
    if (position.y + height > displayRect.GetBottom())
        position.y = displayRect.GetBottom() - height;

    wxWindow *window = GETWIN(wid);
    window->SetSize(position.x, position.y, width, height);
}

PRectangle Window::GetClientPosition() const {
    if (! wid) return PRectangle();
    wxSize sz = GETWIN(wid)->GetClientSize();
    return  PRectangle(0, 0, sz.x, sz.y);
}

void Window::Show(bool show) {
    GETWIN(wid)->Show(show);
}

void Window::InvalidateAll() {
    GETWIN(wid)->Refresh(false);
}

void Window::InvalidateRectangle(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    GETWIN(wid)->Refresh(false, &r);
}

void Window::SetFont(Font &font) {
    GETWIN(wid)->SetFont(*((wxFont*)font.GetID()));
}

void Window::SetCursor(Cursor curs) {
    wxStockCursor cursorId;

    switch (curs) {
    case cursorText:
        cursorId = wxCURSOR_IBEAM;
        break;
    case cursorArrow:
        cursorId = wxCURSOR_ARROW;
        break;
    case cursorUp:
        cursorId = wxCURSOR_ARROW; // ** no up arrow...  wxCURSOR_UPARROW;
        break;
    case cursorWait:
        cursorId = wxCURSOR_WAIT;
        break;
    case cursorHoriz:
        cursorId = wxCURSOR_SIZEWE;
        break;
    case cursorVert:
        cursorId = wxCURSOR_SIZENS;
        break;
    case cursorReverseArrow:
        cursorId = wxCURSOR_RIGHT_ARROW;
        break;
    case cursorHand:
        cursorId = wxCURSOR_HAND;
        break;
    default:
        cursorId = wxCURSOR_ARROW;
        break;
    }

    if(curs != cursorLast)
    {
        wxCursor wc = wxCursor(cursorId);
        GETWIN(wid)->SetCursor(wc);
        cursorLast = curs;
    }
}


// Returns rectangle of monitor pt is on
PRectangle Window::GetMonitorRect(Point pt) {
    if (! wid) return PRectangle();
    // Get the display the point is found on
    int n = wxDisplay::GetFromPoint(wxPoint(wxRound(pt.x), wxRound(pt.y)));
    wxDisplay dpy(n == wxNOT_FOUND ? 0 : n);
    return PRectangleFromwxRect(dpy.GetGeometry());
}

} // namespace Scintilla

//----------------------------------------------------------------------
// wxSTCPopupBase and wxSTCPopupWindow

#ifdef __WXOSX_COCOA__

    wxSTCPopupBase::wxSTCPopupBase(wxWindow* parent):wxNonOwnedWindow()
    {
        m_nativeWin = CreateFloatingWindow(this);
        wxNonOwnedWindow::Create(parent, m_nativeWin);
        m_stc = wxDynamicCast(parent, wxStyledTextCtrl);
        m_isShown = false;
        m_cursorSetByPopup = false;
        m_prevCursor = wxSTC_CURSORNORMAL;

        Bind(wxEVT_ENTER_WINDOW, &wxSTCPopupBase::OnMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &wxSTCPopupBase::OnMouseLeave, this);

        if ( m_stc )
            m_stc->Bind(wxEVT_DESTROY, &wxSTCPopupBase::OnParentDestroy, this);
    }

    wxSTCPopupBase::~wxSTCPopupBase()
    {
        UnsubclassWin();
        CloseFloatingWindow(m_nativeWin);

        if ( m_stc )
        {
            m_stc->Unbind(wxEVT_DESTROY, &wxSTCPopupBase::OnParentDestroy,this);
            RestoreSTCCursor();
        }
    }

    bool wxSTCPopupBase::Show(bool show)
    {
        if ( !wxWindowBase::Show(show) )
            return false;

        if ( show )
        {
            ShowFloatingWindow(m_nativeWin);

            if ( GetRect().Contains(::wxMouseState().GetPosition()) )
                SetSTCCursor(wxSTC_CURSORARROW);
        }
        else
        {
            HideFloatingWindow(m_nativeWin);
            RestoreSTCCursor();
        }

        return true;
    }

    void wxSTCPopupBase::DoSetSize(int x, int y, int width, int ht, int flags)
    {
        wxSize oldSize = GetSize();
        wxNonOwnedWindow::DoSetSize(x, y, width, ht, flags);

        if ( oldSize != GetSize() )
            SendSizeEvent();
    }

    void wxSTCPopupBase::SetSTCCursor(int cursor)
    {
        if ( m_stc )
        {
            m_cursorSetByPopup = true;
            m_prevCursor = m_stc->GetSTCCursor();
            m_stc->SetSTCCursor(cursor);
        }
    }

    void wxSTCPopupBase::RestoreSTCCursor()
    {
        if ( m_stc != nullptr && m_cursorSetByPopup )
            m_stc->SetSTCCursor(m_prevCursor);

        m_cursorSetByPopup = false;
        m_prevCursor = wxSTC_CURSORNORMAL;
    }

    void wxSTCPopupBase::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
    {
        SetSTCCursor(wxSTC_CURSORARROW);
    }

    void wxSTCPopupBase::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
    {
        RestoreSTCCursor();
    }

    void wxSTCPopupBase::OnParentDestroy(wxWindowDestroyEvent& WXUNUSED(event))
    {
        m_stc = nullptr;
    }

#elif wxUSE_POPUPWIN

    wxSTCPopupBase::wxSTCPopupBase(wxWindow* parent)
                   :wxPopupWindow(parent, wxPU_CONTAINS_CONTROLS)
    {
    }

    #ifdef __WXGTK__

        wxSTCPopupBase::~wxSTCPopupBase()
        {
            wxRect rect = GetRect();
            GetParent()->ScreenToClient(&(rect.x), &(rect.y));
            GetParent()->Refresh(false, &rect);
        }

    #elif defined(__WXMSW__)

        // Do not activate the window when it is shown.
        bool wxSTCPopupBase::Show(bool show)
        {
            if ( show )
            {
                // Check if the window is changing from hidden to shown.
                bool changingVisibility = wxWindowBase::Show(true);

                if ( changingVisibility )
                {
                    HWND hWnd = reinterpret_cast<HWND>(GetHandle());
                    if ( GetName() == "wxSTCCallTip" )
                        ::AnimateWindow(hWnd, 25, AW_BLEND);
                    else
                        ::ShowWindow(hWnd, SW_SHOWNA );

                    ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                }

                return changingVisibility;
            }
            else
                return wxPopupWindow::Show(false);
        }

        // Do not activate in response to mouse clicks on this window.
        bool wxSTCPopupBase::MSWHandleMessage(WXLRESULT *res, WXUINT msg,
                                              WXWPARAM wParam, WXLPARAM lParam)
        {
            if ( msg == WM_MOUSEACTIVATE )
            {
                *res = MA_NOACTIVATE;
                return true;
            }
            else
                return wxPopupWindow::MSWHandleMessage(res, msg, wParam,lParam);
        }

    #endif // __WXGTK__

#else

    wxSTCPopupBase::wxSTCPopupBase(wxWindow* parent)
                   :wxFrame(parent, wxID_ANY, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize,
                            wxFRAME_FLOAT_ON_PARENT | wxBORDER_NONE)
    {
        #if defined(__WXGTK__)
            gtk_window_set_accept_focus(GTK_WINDOW(this->GetHandle()), FALSE);
        #endif
    }

    #ifdef __WXMSW__

        // Use ShowWithoutActivating instead of show.
        bool wxSTCPopupBase::Show(bool show)
        {
            if ( show )
            {
                if ( IsShown() )
                    return false;
                else
                {
                    ShowWithoutActivating();
                    return true;
                }
            }
            else
                return wxFrame::Show(false);
        }

        // Do not activate in response to mouse clicks on this window.
        bool wxSTCPopupBase::MSWHandleMessage(WXLRESULT *res, WXUINT msg,
                                              WXWPARAM wParam, WXLPARAM lParam)
        {
            if ( msg == WM_MOUSEACTIVATE )
            {
                *res = MA_NOACTIVATE;
                return true;
            }
            else
                return wxFrame::MSWHandleMessage(res, msg, wParam, lParam);
        }

    #elif !wxSTC_POPUP_IS_CUSTOM

        void wxSTCPopupBase::ActivateParent()
        {
            // Although we're a frame, we always want the parent to be active,
            // so raise it whenever we get shown, focused, etc.
            wxTopLevelWindow *frame = wxDynamicCast(
                wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
            if (frame)
                frame->Raise();
        }

        bool wxSTCPopupBase::Show(bool show)
        {
            bool rv = wxFrame::Show(show);
            if (rv && show)
                ActivateParent();

            #ifdef __WXMAC__
                GetParent()->Refresh(false);
            #endif
        }

    #endif

#endif // __WXOSX_COCOA__

wxSTCPopupWindow::wxSTCPopupWindow(wxWindow* parent)
    : wxSTCPopupBase(parent)
    , m_relPos(wxDefaultPosition)
    , m_absPos(wxDefaultPosition)
{
    #if !wxSTC_POPUP_IS_CUSTOM
        Bind(wxEVT_SET_FOCUS, &wxSTCPopupWindow::OnFocus, this);
    #endif

    m_tlw = wxDynamicCast(wxGetTopLevelParent(parent), wxTopLevelWindow);
    if ( m_tlw )
    {
        m_tlw->Bind(wxEVT_MOVE, &wxSTCPopupWindow::OnParentMove, this);
    #if defined(__WXOSX_COCOA__) || (defined(__WXGTK__)&&!wxSTC_POPUP_IS_FRAME)
        m_tlw->Bind(wxEVT_ICONIZE, &wxSTCPopupWindow::OnIconize, this);
    #endif
    }
}

wxSTCPopupWindow::~wxSTCPopupWindow()
{
    if ( m_tlw )
    {
        m_tlw->Unbind(wxEVT_MOVE, &wxSTCPopupWindow::OnParentMove, this);
    #if defined(__WXOSX_COCOA__) || (defined(__WXGTK__)&&!wxSTC_POPUP_IS_FRAME)
        m_tlw->Unbind(wxEVT_ICONIZE, &wxSTCPopupWindow::OnIconize, this);
    #endif
    }
}

bool wxSTCPopupWindow::Destroy()
{
    #if defined(__WXMAC__) && wxSTC_POPUP_IS_FRAME && !wxSTC_POPUP_IS_CUSTOM
        // The bottom edge of this window is not getting properly
        // refreshed upon deletion, so help it out...
        wxWindow* p = GetParent();
        wxRect r(GetPosition(), GetSize());
        r.SetHeight(r.GetHeight()+1);
        p->Refresh(false, &r);
    #endif

    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    return true;
}

bool wxSTCPopupWindow::AcceptsFocus() const
{
    return false;
}

void wxSTCPopupWindow::DoSetSize(int x, int y, int width, int height, int flags)
{
    wxPoint pos(x, y);
    if ( pos.IsFullySpecified() && !m_relPos.IsFullySpecified() )
    {
        m_relPos = GetParent()->ScreenToClient(pos);
    }

    m_absPos = GetParent()->ClientToScreen(m_relPos);

    wxSTCPopupBase::DoSetSize(m_absPos.x, m_absPos.y, width, height, flags);
}

void wxSTCPopupWindow::OnParentMove(wxMoveEvent& event)
{
    SetPosition(m_absPos);
    event.Skip();
}

#if defined(__WXOSX_COCOA__) || (defined(__WXGTK__) && !wxSTC_POPUP_IS_FRAME)

    void wxSTCPopupWindow::OnIconize(wxIconizeEvent& event)
    {
        Show(!event.IsIconized());
    }

#elif !wxSTC_POPUP_IS_CUSTOM

    void wxSTCPopupWindow::OnFocus(wxFocusEvent& event)
    {
        #if wxSTC_POPUP_IS_FRAME
            ActivateParent();
        #endif

        GetParent()->SetFocus();
        event.Skip();
    }

#endif // __WXOSX_COCOA__


//----------------------------------------------------------------------
// Helper classes for ListBox

// The class manages the colours, images, and other data needed for popup lists.
class wxSTCListBoxVisualData
{
public:
    wxSTCListBoxVisualData(int d);
    virtual ~wxSTCListBoxVisualData();

    // ListBoxImpl implementation
    void SetDesiredVisibleRows(int d);
    int  GetDesiredVisibleRows() const;
    void RegisterImage(int type, const wxBitmap& bmp);
    void RegisterImage(int, const char *);
    void RegisterRGBAImage(int, int, int,const unsigned char *);
    void ClearRegisteredImages();

    // Image data
    const wxBitmap* GetImage(int i) const;
    int GetImageAreaWidth() const;
    int GetImageAreaHeight() const;

    // Colour data
    void ComputeColours();
    const wxColour& GetBorderColour() const;
    void SetColours(const wxColour&, const wxColour&,
                    const wxColour&,const wxColour&);
    const wxColour& GetBgColour() const;
    const wxColour& GetTextColour() const;
    const wxColour& GetHighlightBgColour() const;
    const wxColour& GetHighlightTextColour() const;

    // ListCtrl Style
    void UseListCtrlStyle(bool, const wxColour&, const wxColour&);
    bool HasListCtrlAppearance() const;
    const wxColour& GetCurrentBgColour() const;
    const wxColour& GetCurrentTextColour() const;

private:
    WX_DECLARE_HASH_MAP(int, wxBitmap, wxIntegerHash, wxIntegerEqual, ImgList);

    int      m_desiredVisibleRows;
    ImgList  m_imgList;
    wxSize   m_imgAreaSize;

    wxColour m_borderColour;
    wxColour m_bgColour;
    wxColour m_textColour;
    wxColour m_highlightBgColour;
    wxColour m_highlightTextColour;
    bool     m_useDefaultBgColour;
    bool     m_useDefaultTextColour;
    bool     m_useDefaultHighlightBgColour;
    bool     m_useDefaultHighlightTextColour;

    bool     m_hasListCtrlAppearance;
    wxColour m_currentBgColour;
    wxColour m_currentTextColour;
    bool     m_useDefaultCurrentBgColour;
    bool     m_useDefaultCurrentTextColour;
};

wxSTCListBoxVisualData::wxSTCListBoxVisualData(int d):m_desiredVisibleRows(d),
                        m_useDefaultBgColour(true),
                        m_useDefaultTextColour(true),
                        m_useDefaultHighlightBgColour(true),
                        m_useDefaultHighlightTextColour(true),
                        m_hasListCtrlAppearance(true),
                        m_useDefaultCurrentBgColour(true),
                        m_useDefaultCurrentTextColour(true)
{
    ComputeColours();
}

wxSTCListBoxVisualData::~wxSTCListBoxVisualData()
{
    m_imgList.clear();
}

void wxSTCListBoxVisualData::SetDesiredVisibleRows(int d)
{
    m_desiredVisibleRows=d;
}

int wxSTCListBoxVisualData::GetDesiredVisibleRows() const
{
    return m_desiredVisibleRows;
}

void wxSTCListBoxVisualData::RegisterImage(int type, const wxBitmap& bmp)
{
    if ( !bmp.IsOk() )
        return;

    ImgList::iterator it=m_imgList.find(type);
    bool preExistingWithDifferentSize = false;
    if ( it != m_imgList.end() )
    {
        if ( it->second.GetSize() != bmp.GetSize() )
        {
            preExistingWithDifferentSize = true;
        }

        m_imgList.erase(it);
    }

    m_imgList[type] = bmp;

    if ( preExistingWithDifferentSize )
    {
        m_imgAreaSize.Set(0,0);

        for ( ImgList::iterator imgIt = m_imgList.begin() ;
              imgIt != m_imgList.end() ; ++imgIt )
        {
            m_imgAreaSize.IncTo(it->second.GetSize());
        }
    }
    else
    {
        m_imgAreaSize.IncTo(bmp.GetSize());
    }
}

void wxSTCListBoxVisualData::RegisterImage(int type, const char *xpm_data)
{
    wxXPMDecoder dec;
    wxImage img;

    // This check is borrowed from src/stc/scintilla/src/XPM.cpp.
    // Test done is two parts to avoid possibility of overstepping the memory
    // if memcmp implemented strangely. Must be 4 bytes at least at destination.
    if ( (0 == memcmp(xpm_data, "/* X", 4)) &&
         (0 == memcmp(xpm_data, "/* XPM */", 9)) )
    {
        wxMemoryInputStream stream(xpm_data, strlen(xpm_data)+1);
        img = dec.ReadFile(stream);
    }
    else
        img = dec.ReadData(reinterpret_cast<const char* const*>(xpm_data));

    wxBitmap bmp(img);
    RegisterImage(type, bmp);
}

void wxSTCListBoxVisualData::RegisterRGBAImage(int type, int width, int height,
                                    const unsigned char *pixelsImage)
{
    wxBitmap bmp = BitmapFromRGBAImage(width, height, pixelsImage);
    RegisterImage(type, bmp);
}

void wxSTCListBoxVisualData::ClearRegisteredImages()
{
    m_imgList.clear();
    m_imgAreaSize.Set(0,0);
}

const wxBitmap* wxSTCListBoxVisualData::GetImage(int i) const
{
    ImgList::const_iterator it = m_imgList.find(i);

    if ( it != m_imgList.end() )
        return &(it->second);
    else
        return nullptr;
}

int wxSTCListBoxVisualData::GetImageAreaWidth() const
{
    return m_imgAreaSize.GetWidth();
}

int wxSTCListBoxVisualData::GetImageAreaHeight() const
{
    return m_imgAreaSize.GetHeight();
}

void wxSTCListBoxVisualData::ComputeColours()
{
    // wxSYS_COLOUR_BTNSHADOW seems to be the closest match with most themes.
    m_borderColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW );

    if ( m_useDefaultBgColour )
        m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);

    if ( m_useDefaultTextColour )
        m_textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);

    if ( m_hasListCtrlAppearance )
    {
        // If m_highlightBgColour and/or m_currentBgColour are not
        // explicitly set, set them to wxNullColour to indicate that they
        // should be drawn with wxRendererNative.
        if ( m_useDefaultHighlightBgColour )
            m_highlightBgColour = wxNullColour;

        if ( m_useDefaultCurrentBgColour )
            m_currentBgColour = wxNullColour;

        #ifdef __WXMSW__
            if ( m_useDefaultHighlightTextColour )
                m_highlightTextColour =
                    wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
        #else
            if ( m_useDefaultHighlightTextColour )
                m_highlightTextColour = wxSystemSettings::GetColour(
                    wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
        #endif

        if ( m_useDefaultCurrentTextColour )
            m_currentTextColour = wxSystemSettings::GetColour(
                wxSYS_COLOUR_LISTBOXTEXT);
    }
    else
    {
    #ifdef __WXOSX_COCOA__
        if ( m_useDefaultHighlightBgColour )
            m_highlightBgColour = GetListHighlightColour();
    #else
        if ( m_useDefaultHighlightBgColour )
            m_highlightBgColour =
                wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    #endif

        if ( m_useDefaultHighlightTextColour )
            m_highlightTextColour =
                wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
    }
}

static void SetColourHelper(bool& isDefault, wxColour& itemColour,
                            const wxColour& newColour)
{
    isDefault = !newColour.IsOk();
    itemColour = newColour;
}

void wxSTCListBoxVisualData::SetColours(const wxColour& bg,
                                        const wxColour& txt,
                                        const wxColour& hlbg,
                                        const wxColour& hltext)
{
    SetColourHelper(m_useDefaultBgColour, m_bgColour, bg);
    SetColourHelper(m_useDefaultTextColour, m_textColour, txt);
    SetColourHelper(m_useDefaultHighlightBgColour, m_highlightBgColour, hlbg);
    SetColourHelper(m_useDefaultHighlightTextColour, m_highlightTextColour,
                    hltext);
    ComputeColours();
}

const wxColour& wxSTCListBoxVisualData::GetBorderColour() const
{
    return m_borderColour;
}

const wxColour& wxSTCListBoxVisualData::GetBgColour() const
{
    return m_bgColour;
}

const wxColour& wxSTCListBoxVisualData::GetTextColour() const
{
    return m_textColour;
}

const wxColour& wxSTCListBoxVisualData::GetHighlightBgColour() const
{
    return m_highlightBgColour;
}

const wxColour& wxSTCListBoxVisualData::GetHighlightTextColour() const
{
    return m_highlightTextColour;
}

void wxSTCListBoxVisualData::UseListCtrlStyle(bool useListCtrlStyle,
                                              const wxColour& curBg,
                                              const wxColour& curText)
{
    m_hasListCtrlAppearance = useListCtrlStyle;
    SetColourHelper(m_useDefaultCurrentBgColour, m_currentBgColour, curBg);
    SetColourHelper(m_useDefaultCurrentTextColour, m_currentTextColour,
                    curText);
    ComputeColours();
}

bool wxSTCListBoxVisualData::HasListCtrlAppearance() const
{
    return m_hasListCtrlAppearance;
}

const wxColour& wxSTCListBoxVisualData::GetCurrentBgColour() const
{
    return m_currentBgColour;
}

const wxColour& wxSTCListBoxVisualData::GetCurrentTextColour() const
{
    return m_currentTextColour;
}

// The class is intended to look like a standard listbox (with an optional
// icon). However, it needs to look like it has focus even when it doesn't.
class wxSTCListBox : public wxSystemThemedControl<wxVListBox>
{
public:
    wxSTCListBox(wxWindow*, wxSTCListBoxVisualData*, int);

    // wxWindow overrides
    virtual bool AcceptsFocus() const override;
    virtual void SetFocus() override;

    // Setters
    void SetContainerBorderSize(int);

    // ListBoxImpl implementation
    virtual void SetListBoxFont(Font &font);
    void SetAverageCharWidth(int width);
    PRectangle GetDesiredRect() const;
    int CaretFromEdge() const;
    void Clear();
    void Append(char *s, int type = -1);
    int Length() const;
    void Select(int n);
    void GetValue(int n, char *value, int len) const;
    void SetDelegate(IListBoxDelegate* lbDelegate);
    void SetList(const char* list, char separator, char typesep);

protected:
    // Helpers
    void AppendHelper(const wxString& text, int type);
    void RecalculateItemHeight();
    int TextBoxFromClientEdge() const;
    virtual void OnDrawItemText(wxDC&, const wxRect&,
                                const wxString&, const wxColour&) const;

    // Event handlers
    void OnSelection(wxCommandEvent&);
    void OnDClick(wxCommandEvent&);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnDPIChanged(wxDPIChangedEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeaveWindow(wxMouseEvent& event);

    // wxVListBox overrides
    virtual wxCoord OnMeasureItem(size_t) const override;
    virtual void OnDrawItem(wxDC& , const wxRect &, size_t) const override;
    virtual void OnDrawBackground(wxDC&, const wxRect&,size_t) const override;

    wxSTCListBoxVisualData* m_visualData;
    wxVector<wxString>      m_labels;
    wxVector<int>           m_imageNos;
    size_t                  m_maxStrWidth;
    int                     m_currentRow;

    IListBoxDelegate*       m_lbDelegate;
    int                     m_aveCharWidth;

    // These drawing parameters are computed or set externally.
    int m_borderSize;
    int m_textHeight;
    int m_itemHeight;
    int m_textTopGap;

    // These drawing parameters are set internally and can be changed if needed
    // to better match the appearance of a list box on a specific platform.
    int m_imagePadding;
    int m_textBoxToTextGap;
    int m_textExtraVerticalPadding;
};

wxSTCListBox::wxSTCListBox(wxWindow* parent, wxSTCListBoxVisualData* v, int ht)
             :wxSystemThemedControl<wxVListBox>(),
              m_visualData(v), m_maxStrWidth(0), m_currentRow(wxNOT_FOUND),
              m_lbDelegate(nullptr),
              m_aveCharWidth(8), m_textHeight(ht), m_itemHeight(ht),
              m_textTopGap(0)
{
    wxVListBox::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                       wxBORDER_NONE, "AutoCompListBox");

    m_imagePadding             = FromDIP(1);
    m_textBoxToTextGap         = FromDIP(3);
    m_textExtraVerticalPadding = FromDIP(1);

    SetBackgroundColour(m_visualData->GetBgColour());

    Bind(wxEVT_LISTBOX, &wxSTCListBox::OnSelection, this);
    Bind(wxEVT_LISTBOX_DCLICK, &wxSTCListBox::OnDClick, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED, &wxSTCListBox::OnSysColourChanged, this);
    Bind(wxEVT_DPI_CHANGED, &wxSTCListBox::OnDPIChanged, this);

    if ( m_visualData->HasListCtrlAppearance() )
    {
        EnableSystemTheme();
        Bind(wxEVT_MOTION, &wxSTCListBox::OnMouseMotion, this);
        Bind(wxEVT_LEAVE_WINDOW, &wxSTCListBox::OnMouseLeaveWindow, this);

        #ifdef __WXMSW__
            // On MSW when using wxRendererNative to draw items in list control
            // style, the colours used seem to be based on the parent's
            // background colour. So set the popup's background.
            parent->SetOwnBackgroundColour(m_visualData->GetBgColour());
        #endif
    }
}

bool wxSTCListBox::AcceptsFocus() const
{
    return false;
}

// Do nothing in response to an attempt to set focus.
void wxSTCListBox::SetFocus()
{
}

void wxSTCListBox::SetContainerBorderSize(int s)
{
    m_borderSize = s;
}

void wxSTCListBox::SetListBoxFont(Font &font)
{
    SetFont(*((wxFont*)font.GetID()));
    int w;
    GetTextExtent(EXTENT_TEST, &w, &m_textHeight);
    RecalculateItemHeight();
}

void wxSTCListBox::SetAverageCharWidth(int width)
{
    m_aveCharWidth = width;
}

PRectangle wxSTCListBox::GetDesiredRect() const
{
    int maxw = m_maxStrWidth * m_aveCharWidth;
    int maxh ;

    // give it a default if there are no lines, and/or add a bit more
    if ( maxw == 0 )
        maxw = 100;

    maxw += TextBoxFromClientEdge() + m_textBoxToTextGap + m_aveCharWidth * 3;

    // estimate a desired height
    const int count = Length();
    const int desiredVisibleRows = m_visualData->GetDesiredVisibleRows();
    if ( count )
    {
        if ( count <= desiredVisibleRows )
            maxh = count * m_itemHeight;
        else
            maxh = desiredVisibleRows * m_itemHeight;
    }
    else
        maxh = 100;

    // Add space for a scrollbar if needed.
    if ( count > desiredVisibleRows )
        maxw += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, this);

    // Add borders.
    maxw += 2 * m_borderSize;
    maxh += 2 * m_borderSize;

    PRectangle rc;
    rc.top = 0;
    rc.left = 0;
    rc.right = maxw;
    rc.bottom = maxh;
    return rc;
}

int wxSTCListBox::CaretFromEdge() const
{
    return m_borderSize + TextBoxFromClientEdge() + m_textBoxToTextGap;
}

void wxSTCListBox::Clear()
{
    m_labels.clear();
    m_imageNos.clear();
}

void wxSTCListBox::Append(char *s, int type)
{
    AppendHelper(stc2wx(s), type);
    RecalculateItemHeight();
}

int wxSTCListBox::Length() const
{
    return GetItemCount();
}

void wxSTCListBox::Select(int n)
{
    SetSelection(n);

    wxCommandEvent e;
    OnSelection(e);
}

void wxSTCListBox::GetValue(int n, char *value, int len) const
{
    strncpy(value, wx2stc(m_labels[n]), len);
    value[len-1] = '\0';
}

void wxSTCListBox::SetDelegate(IListBoxDelegate* lbDelegate)
{
    m_lbDelegate = lbDelegate;
}

void wxSTCListBox::SetList(const char* list, char separator, char typesep)
{
    wxWindowUpdateLocker noUpdates(this);
    Clear();
    wxStringTokenizer tkzr(stc2wx(list), (wxChar)separator);
    while ( tkzr.HasMoreTokens() ) {
        wxString token = tkzr.GetNextToken();
        long type = -1;
        int pos = token.Find(typesep);
        if (pos != -1) {
            token.Mid(pos+1).ToLong(&type);
            token.Truncate(pos);
        }
        AppendHelper(token, (int)type);
    }

    RecalculateItemHeight();
}

void wxSTCListBox::AppendHelper(const wxString& text, int type)
{
    m_maxStrWidth = wxMax(m_maxStrWidth, text.length());
    m_labels.push_back(text);
    m_imageNos.push_back(type);
    SetItemCount(m_labels.size());
}

void wxSTCListBox::RecalculateItemHeight()
{
    m_itemHeight = wxMax(m_textHeight + 2 * m_textExtraVerticalPadding,
                       m_visualData->GetImageAreaHeight() + 2 * m_imagePadding);
    m_textTopGap = (m_itemHeight - m_textHeight)/2;
}

int wxSTCListBox::TextBoxFromClientEdge() const
{
    const int width = m_visualData->GetImageAreaWidth();
    return (width == 0 ? 0 : width + 2 * m_imagePadding);
}

void wxSTCListBox::OnSelection(wxCommandEvent& WXUNUSED(event))
{
    if ( m_lbDelegate )
    {
        ListBoxEvent lbe(ListBoxEvent::EventType::selectionChange);
        m_lbDelegate->ListNotify(&lbe);
    }
}

void wxSTCListBox::OnDClick(wxCommandEvent& WXUNUSED(event))
{
    if ( m_lbDelegate )
    {
        ListBoxEvent lbe(ListBoxEvent::EventType::doubleClick);
        m_lbDelegate->ListNotify(&lbe);
    }
}

void wxSTCListBox::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    m_visualData->ComputeColours();
    GetParent()->SetOwnBackgroundColour(m_visualData->GetBgColour());
    SetBackgroundColour(m_visualData->GetBgColour());
    GetParent()->Refresh();
}

void wxSTCListBox::OnDPIChanged(wxDPIChangedEvent& event)
{
    m_imagePadding = FromDIP(1);
    m_textBoxToTextGap = FromDIP(3);
    m_textExtraVerticalPadding = FromDIP(1);

    int w;
    GetTextExtent(EXTENT_TEST, &w, &m_textHeight);

    RecalculateItemHeight();

    event.Skip();
}

void wxSTCListBox::OnMouseLeaveWindow(wxMouseEvent& event)
{
    const int old = m_currentRow;
    m_currentRow = wxNOT_FOUND;

    if ( old != wxNOT_FOUND )
        RefreshRow(old);

    event.Skip();
}

void wxSTCListBox::OnMouseMotion(wxMouseEvent& event)
{
    const int old = m_currentRow;
    m_currentRow = VirtualHitTest(event.GetY());

    if ( old != m_currentRow )
    {
        if( m_currentRow != wxNOT_FOUND )
            RefreshRow(m_currentRow);

        if( old != wxNOT_FOUND )
            RefreshRow(old);
    }

    event.Skip();
}

wxCoord wxSTCListBox::OnMeasureItem(size_t WXUNUSED(n)) const
{
    return static_cast<wxCoord>(m_itemHeight);
}

// This control will be drawn so that a typical row of pixels looks like:
//
//    +++++++++++++++++++++++++   =====ITEM TEXT================
//  |         |                 |    |
//  |       imageAreaWidth      |    |
//  |                           |    |
// m_imagePadding               |   m_textBoxToTextGap
//                              |
//                   m_imagePadding
//
//
// m_imagePadding            : Used to give a little extra space between the
//                             client edge and an item's bitmap.
// imageAreaWidth            : Computed as the width of the largest registered
//                             bitmap (part of wxSTCListBoxVisualData).
// m_textBoxToTextGap        : Used so that item text does not begin immediately
//                             at the edge of the highlight box.
//
// Images are drawn centered in the image area.
// If a selection rectangle is drawn, its left edge is at x=0 if there are
// no bitmaps. Otherwise
//       x = m_imagePadding + m_imageAreaWidth + m_imagePadding.
// Text is drawn at x + m_textBoxToTextGap and centered vertically.

void wxSTCListBox::OnDrawItemText(wxDC& dc, const wxRect& rect,
                                  const wxString& label,
                                  const wxColour& textCol) const
{
    wxDCTextColourChanger tcc(dc, textCol);

    wxString ellipsizedlabel = wxControl::Ellipsize(label, dc, wxELLIPSIZE_END,
                                                    rect.GetWidth());
    dc.DrawText(ellipsizedlabel, rect.GetLeft(), rect.GetTop());
}

void wxSTCListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    wxString label;
    int imageNo = -1;
    if ( n < m_labels.size() )
    {
        label   = m_labels[n];
        imageNo = m_imageNos[n];
    }

    int topGap = m_textTopGap;
    int leftGap = TextBoxFromClientEdge() + m_textBoxToTextGap;

    wxColour textCol;

    if ( IsSelected(n) )
        textCol = m_visualData->GetHighlightTextColour();
    else if ( static_cast<int>(n) == m_currentRow )
        textCol = m_visualData->GetCurrentTextColour();
    else
        textCol = m_visualData->GetTextColour();

    wxRect rect2(rect.GetLeft() + leftGap, rect.GetTop() + topGap,
                 rect.GetWidth() - leftGap, m_textHeight);

    OnDrawItemText(dc, rect2, label, textCol);

    const wxBitmap* b = m_visualData->GetImage(imageNo);
    if ( b )
    {
        const int width = m_visualData->GetImageAreaWidth();
        topGap = (m_itemHeight - b->GetHeight())/2;
        leftGap = m_imagePadding + (width - b->GetWidth())/2;
        dc.DrawBitmap(*b, rect.GetLeft()+leftGap, rect.GetTop()+topGap, true);
    }
}

void wxSTCListBox::OnDrawBackground(wxDC &dc, const wxRect &rect,size_t n) const
{
    if ( IsSelected(n) )
    {
        wxRect selectionRect(rect);
        const wxColour& highlightBgColour =m_visualData->GetHighlightBgColour();

        #ifdef __WXMSW__
            if ( !m_visualData->HasListCtrlAppearance() )
            {
                // On windows the selection rectangle in Scintilla's
                // autocompletion list only covers the text and not the icon.

                const int textBoxFromClientEdge = TextBoxFromClientEdge();
                selectionRect.SetLeft(rect.GetLeft() + textBoxFromClientEdge);
                selectionRect.SetWidth(rect.GetWidth() - textBoxFromClientEdge);
            }
        #endif // __WXMSW__

        if ( highlightBgColour.IsOk() )
        {
            wxDCBrushChanger bc(dc, highlightBgColour);
            wxDCPenChanger   pc(dc, highlightBgColour);
            dc.DrawRectangle(selectionRect);
        }
        else
        {
            wxRendererNative::GetDefault().DrawItemSelectionRect(
                const_cast<wxSTCListBox*>(this), dc, selectionRect,
                wxCONTROL_SELECTED | wxCONTROL_FOCUSED);
        }

        if ( !m_visualData->HasListCtrlAppearance() )
            wxRendererNative::GetDefault().DrawFocusRect(
                const_cast<wxSTCListBox*>(this), dc, selectionRect);
    }
    else if ( static_cast<int>(n) == m_currentRow )
    {
        const wxColour& currentBgColour = m_visualData->GetCurrentBgColour();

        if ( currentBgColour.IsOk() )
        {
            wxDCBrushChanger bc(dc, currentBgColour);
            wxDCPenChanger   pc(dc, currentBgColour);
            dc.DrawRectangle(rect);
        }
        else
        {
            wxRendererNative::GetDefault().DrawItemSelectionRect(
                const_cast<wxSTCListBox*>(this), dc, rect,
                wxCONTROL_CURRENT | wxCONTROL_FOCUSED);
        }
    }
}


#ifdef HAVE_DIRECTWRITE_TECHNOLOGY

// This class will use SurfaceD2D methods to measure and draw items in the popup
// listbox. This is needed to ensure that the text in the listbox matches the
// text in the editor window as closely as possible.

class wxSTCListBoxD2D : public wxSTCListBox
{
public:
    wxSTCListBoxD2D(wxWindow* parent, wxSTCListBoxVisualData* v, int ht)
        : wxSTCListBox(parent, v, ht)
        , m_surfaceFontData(nullptr)
    {
    }

    ~wxSTCListBoxD2D()
    {
        delete m_surfaceFontData;
    }

    void SetListBoxFont(Font& font) override
    {
        // Retrieve the SurfaceFontDataD2D from font and store a copy of it.
        wxFontWithAscent* fwa = wxFontWithAscent::FromFID(font.GetID());

        SurfaceData* data = fwa->GetSurfaceFontData();
        SurfaceFontDataD2D* d2dft = static_cast<SurfaceFontDataD2D*>(data);
        m_surfaceFontData = new SurfaceFontDataD2D(*d2dft);

        // Create a SurfaceD2D object to measure text height for the font.
        SurfaceD2D surface;
        wxClientDC dc(this);
        surface.Init(&dc, GetGrandParent());
        m_textHeight = surface.Height(font);
        surface.Release();

        RecalculateItemHeight();
    }

    void OnDrawItemText(wxDC& dc, const wxRect& rect, const wxString& label,
                        const wxColour& textCol) const override
    {
        // Create a font and a surface object.
        wxFontWithAscent* fontCopy = new wxFontWithAscent(wxFont());
        SurfaceFontDataD2D* sfd = new SurfaceFontDataD2D(*m_surfaceFontData);
        fontCopy->SetSurfaceFontData(sfd);
        Font tempFont;
        tempFont.SetID(fontCopy);

        SurfaceD2D surface;
        surface.Init(&dc, GetGrandParent());

        // Ellipsize the label if necessary. This is done by manually removing
        // characters from the end of the label until it's short enough.
        wxString ellipsizedLabel = label;

        wxCharBuffer buffer = wx2stc(ellipsizedLabel);
        int ellipsizedLen = wx2stclen(ellipsizedLabel, buffer);
        int curWidth = surface.WidthText(tempFont, buffer.data(),ellipsizedLen);

        for ( int i = label.length(); curWidth > rect.GetWidth() && i; --i )
        {
            ellipsizedLabel = label.Left(i);
            // Add the "Horizontal Ellipsis" character (U+2026).
            ellipsizedLabel << wxUniChar(0x2026);

            buffer = wx2stc(ellipsizedLabel);
            ellipsizedLen = wx2stclen(ellipsizedLabel, buffer);
            curWidth = surface.WidthText(tempFont, buffer.data(),ellipsizedLen);
        }

        // Construct the necessary Scintilla objects and then draw the label.
        PRectangle prect = PRectangleFromwxRect(rect);
        ColourDesired fore(textCol.Red(), textCol.Green(), textCol.Blue());

        XYPOSITION ybase = rect.GetTop() + m_surfaceFontData->GetAscent();

        surface.DrawTextTransparent(prect, tempFont, ybase, buffer.data(),
                                    ellipsizedLen, fore);

        // Clean up.
        tempFont.Release();
        surface.Release();
    }

private:
    SurfaceFontDataD2D* m_surfaceFontData;
};
#endif // HAVE_DIRECTWRITE_TECHNOLOGY


// A popup window to place the wxSTCListBox upon
class wxSTCListBoxWin : public wxSTCPopupWindow
{
public:
    wxSTCListBoxWin(wxWindow*, wxSTCListBox**, wxSTCListBoxVisualData*,
                    int, int);

protected:
    void OnPaint(wxPaintEvent&);

private:
    wxSTCListBoxVisualData* m_visualData;
};

wxSTCListBoxWin::wxSTCListBoxWin(wxWindow* parent, wxSTCListBox** lb,
                                 wxSTCListBoxVisualData* v, int h, int tech)
                :wxSTCPopupWindow(parent)
{
    switch ( tech )
    {
#ifdef HAVE_DIRECTWRITE_TECHNOLOGY
        case wxSTC_TECHNOLOGY_DIRECTWRITE:
            *lb = new wxSTCListBoxD2D(this, v, h);
            break;
#endif
        case wxSTC_TECHNOLOGY_DEFAULT:
            wxFALLTHROUGH;
        default:
            *lb = new wxSTCListBox(this, v, h);
    }

    // Use the background of this window to form a frame around the listbox
    // except on macos where the native Scintilla popup has no frame.
#ifdef __WXOSX_COCOA__
    const int borderThickness = 0;
#else
    const int borderThickness = FromDIP(1);
#endif
    wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
    bSizer->Add(*lb, 1, wxEXPAND|wxALL, borderThickness);
    SetSizer(bSizer);
    (*lb)->SetContainerBorderSize(borderThickness);

    // When drawing highlighting in listctrl style with wxRendererNative on MSW,
    // the colours used seem to be based on the background of the parent window.
    // So manually paint this window to give it the border colour instead of
    // setting the background colour.
    m_visualData = v;
    Bind(wxEVT_PAINT, &wxSTCListBoxWin::OnPaint, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void wxSTCListBoxWin::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.SetBackground(m_visualData->GetBorderColour());
    dc.Clear();
}


//----------------------------------------------------------------------

ListBoxImpl::ListBoxImpl()
            :m_listBox(nullptr), m_visualData(new wxSTCListBoxVisualData(5))
{
}

ListBoxImpl::~ListBoxImpl() {
    delete m_visualData;
}


void ListBoxImpl::SetFont(Font &font) {
    m_listBox->SetListBoxFont(font);
}


void ListBoxImpl::Create(Window &parent, int WXUNUSED(ctrlID),
                         Point WXUNUSED(location_), int lineHeight_,
                         bool WXUNUSED(unicodeMode_), int technology_) {
    wid = new wxSTCListBoxWin(GETWIN(parent.GetID()), &m_listBox, m_visualData,
                              lineHeight_, technology_);
}


void ListBoxImpl::SetAverageCharWidth(int width) {
    m_listBox->SetAverageCharWidth(width);
}


void ListBoxImpl::SetVisibleRows(int rows) {
    m_visualData->SetDesiredVisibleRows(rows);
}


int ListBoxImpl::GetVisibleRows() const {
    return m_visualData->GetDesiredVisibleRows();
}

PRectangle ListBoxImpl::GetDesiredRect() {
    return m_listBox->GetDesiredRect();
}


int ListBoxImpl::CaretFromEdge() {
    return m_listBox->CaretFromEdge();
}


void ListBoxImpl::Clear() {
    m_listBox->Clear();
}


void ListBoxImpl::Append(char *s, int type) {
    m_listBox->Append(s, type);
}


void ListBoxImpl::SetList(const char* list, char separator, char typesep) {
    m_listBox->SetList(list, separator, typesep);
}


int ListBoxImpl::Length() {
    return m_listBox->Length();
}


void ListBoxImpl::Select(int n) {
    m_listBox->Select(n);
}


int ListBoxImpl::GetSelection() {
    return m_listBox->GetSelection();
}


int ListBoxImpl::Find(const char *WXUNUSED(prefix)) {
    // No longer used
    return wxNOT_FOUND;
}


void ListBoxImpl::GetValue(int n, char *value, int len) {
    m_listBox->GetValue(n, value, len);
}

void ListBoxImpl::RegisterImageHelper(int type, const wxBitmap& bmp)
{
    m_visualData->RegisterImage(type, bmp);
}


void ListBoxImpl::RegisterImage(int type, const char *xpm_data) {
    m_visualData->RegisterImage(type, xpm_data);
}


void ListBoxImpl::RegisterRGBAImage(int type, int width, int height,
                                    const unsigned char *pixelsImage)
{
    m_visualData->RegisterRGBAImage(type, width, height, pixelsImage);
}


void ListBoxImpl::ClearRegisteredImages() {
    m_visualData->ClearRegisteredImages();
}


void ListBoxImpl::SetDelegate(IListBoxDelegate* lbDelegate) {
    m_listBox->SetDelegate(lbDelegate);
}


ListBox::ListBox() noexcept {
}

ListBox::~ListBox() {
}

ListBox *ListBox::Allocate() {
    return new ListBoxImpl();
}

//----------------------------------------------------------------------

Menu::Menu() noexcept : mid(nullptr) {
}

void Menu::CreatePopUp() {
    Destroy();
    mid = new wxMenu();
}

void Menu::Destroy() {
    if (mid)
        delete (wxMenu*)mid;
    mid = nullptr;
}

void Menu::Show(Point pt, Window &w) {
    GETWIN(w.GetID())->PopupMenu((wxMenu*)mid, wxRound(pt.x - 4), wxRound(pt.y));
    Destroy();
}

//----------------------------------------------------------------------

class DynamicLibraryImpl : public DynamicLibrary {
public:
    explicit DynamicLibraryImpl(const char *modulePath)
        : m_dynlib(wxString::FromUTF8(modulePath), wxDL_LAZY) {
    }

    // Use GetSymbol to get a pointer to the relevant function.
    virtual Function FindFunction(const char *name) override {
        if (m_dynlib.IsLoaded()) {
            bool status;
            void* fn_address = m_dynlib.GetSymbol(wxString::FromUTF8(name),
                                                  &status);
            if(status)
                return fn_address;
            else
                return nullptr;
        }
        else
            return nullptr;
    }

    virtual bool IsValid() override {
        return m_dynlib.IsLoaded();
    }

private:
    wxDynamicLibrary m_dynlib;
};

DynamicLibrary *DynamicLibrary::Load(const char *modulePath) {
    return static_cast<DynamicLibrary *>( new DynamicLibraryImpl(modulePath) );
}

//----------------------------------------------------------------------

ColourDesired Platform::Chrome() {
    wxColour c;
    c = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    return ColourDesired(c.Red(), c.Green(), c.Blue());
}

ColourDesired Platform::ChromeHighlight() {
    wxColour c;
    c = wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT);
    return ColourDesired(c.Red(), c.Green(), c.Blue());
}

const char *Platform::DefaultFont() {
    static char buf[128];
    wxStrlcpy(buf, wxNORMAL_FONT->GetFaceName().mbc_str(), WXSIZEOF(buf));
    return buf;
}

int Platform::DefaultFontSize() {
    return wxNORMAL_FONT->GetPointSize();
}

unsigned int Platform::DoubleClickTime() {
    return 500;   // **** ::GetDoubleClickTime();
}


//#define TRACE

void Platform::DebugDisplay(const char *s) {
#ifdef TRACE
    wxLogDebug(stc2wx(s));
#else
    wxUnusedVar(s);
#endif
}

void Platform::DebugPrintf(const char *format, ...) {
#ifdef TRACE
    char buffer[2000];
    va_list pArguments;
    va_start(pArguments, format);
    vsprintf(buffer,format,pArguments);
    va_end(pArguments);
    Platform::DebugDisplay(buffer);
#else
    wxUnusedVar(format);
#endif
}


static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) {
    bool ret = assertionPopUps;
    assertionPopUps = assertionPopUps_;
    return ret;
}

void Platform::Assert(const char *c, const char *file, int line) {
#ifdef TRACE
    char buffer[2000];
    sprintf(buffer, "Assertion [%s] failed at %s %d", c, file, line);
    if (assertionPopUps) {
        /*int idButton = */
        wxMessageBox(stc2wx(buffer),
                     wxT("Assertion failure"),
                     wxICON_HAND | wxOK);
    } else {
        strcat(buffer, "\r\n");
        Platform::DebugDisplay(buffer);
        abort();
    }
#else
    wxUnusedVar(c);
    wxUnusedVar(file);
    wxUnusedVar(line);
#endif
}


//----------------------------------------------------------------------

// For historical reasons, we use Scintilla-specific conversion functions, we
// should probably just call FromUTF8()/utf8_str() directly instead now.

wxString stc2wx(const char* str, size_t len)
{
    return wxString::FromUTF8(str, len);
}



wxString stc2wx(const char* str)
{
    return wxString::FromUTF8(str);
}


wxWX2MBbuf wx2stc(const wxString& str)
{
    return str.utf8_str();
}

#endif // wxUSE_STC
