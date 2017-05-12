// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWidgets
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STC

#ifndef WX_PRECOMP
    #include "wx/math.h"
    #include "wx/menu.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include <ctype.h>

#if wxUSE_DISPLAY
#include "wx/display.h"
#endif

#include "wx/encconv.h"
#include "wx/listctrl.h"
#include "wx/mstream.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/tokenzr.h"
#include "wx/dynlib.h"

#ifdef wxHAS_RAW_BITMAP
#include "wx/rawbmp.h"
#endif
#if wxUSE_GRAPHICS_CONTEXT
#include "wx/dcgraph.h"
#endif

#include "Platform.h"
#include "PlatWX.h"
#include "wx/stc/stc.h"
#include "wx/stc/private.h"


Point Point::FromLong(long lpoint) {
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

wxRect wxRectFromPRectangle(PRectangle prc) {
    wxRect r(wxRound(prc.left), wxRound(prc.top),
             wxRound(prc.Width()), wxRound(prc.Height()));
    return r;
}

PRectangle PRectangleFromwxRect(wxRect rc) {
    return PRectangle(rc.GetLeft(), rc.GetTop(),
                      rc.GetRight()+1, rc.GetBottom()+1);
}

wxColour wxColourFromCD(ColourDesired& cd) {
    return wxColour((unsigned char)cd.GetRed(),
                    (unsigned char)cd.GetGreen(),
                    (unsigned char)cd.GetBlue());
}

wxColour wxColourFromCDandAlpha(ColourDesired& cd, int alpha) {
    return wxColour((unsigned char)cd.GetRed(),
                    (unsigned char)cd.GetGreen(),
                    (unsigned char)cd.GetBlue(),
                    (unsigned char)alpha);
}

//----------------------------------------------------------------------

namespace
{

// wxFont with ascent cached, a pointer to this type is stored in Font::fid.
class wxFontWithAscent : public wxFont
{
public:
    explicit wxFontWithAscent(const wxFont &font)
        : wxFont(font),
          m_ascent(0)
    {
    }

    static wxFontWithAscent* FromFID(FontID fid)
    {
        return static_cast<wxFontWithAscent*>(fid);
    }

    void SetAscent(int ascent) { m_ascent = ascent; }
    int GetAscent() const { return m_ascent; }

private:
    int m_ascent;
};

void SetAscent(Font& f, int ascent)
{
    wxFontWithAscent::FromFID(f.GetID())->SetAscent(ascent);
}

int GetAscent(Font& f)
{
    return wxFontWithAscent::FromFID(f.GetID())->GetAscent();
}

} // anonymous namespace

Font::Font() {
    fid = 0;
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
    fid = new wxFontWithAscent(font);
}


void Font::Release() {
    if (fid)
        delete wxFontWithAscent::FromFID(fid);
    fid = 0;
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

    virtual void Init(WindowID wid) wxOVERRIDE;
    virtual void Init(SurfaceID sid, WindowID wid) wxOVERRIDE;
    virtual void InitPixMap(int width, int height, Surface *surface_, WindowID wid) wxOVERRIDE;

    virtual void Release() wxOVERRIDE;
    virtual bool Initialised() wxOVERRIDE;
    virtual void PenColour(ColourDesired fore) wxOVERRIDE;
    virtual int LogPixelsY() wxOVERRIDE;
    virtual int DeviceHeightFont(int points) wxOVERRIDE;
    virtual void MoveTo(int x_, int y_) wxOVERRIDE;
    virtual void LineTo(int x_, int y_) wxOVERRIDE;
    virtual void Polygon(Point *pts, int npts, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void FillRectangle(PRectangle rc, ColourDesired back) wxOVERRIDE;
    virtual void FillRectangle(PRectangle rc, Surface &surfacePattern) wxOVERRIDE;
    virtual void RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void AlphaRectangle(PRectangle rc, int cornerSize, ColourDesired fill, int alphaFill,
                                ColourDesired outline, int alphaOutline, int flags) wxOVERRIDE;
    virtual void DrawRGBAImage(PRectangle rc, int width, int height,
                               const unsigned char *pixelsImage) wxOVERRIDE;
    virtual void Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void Copy(PRectangle rc, Point from, Surface &surfaceSource) wxOVERRIDE;

    virtual void DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back) wxOVERRIDE;
    virtual void DrawTextTransparent(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore) wxOVERRIDE;
    virtual void MeasureWidths(Font &font_, const char *s, int len, XYPOSITION *positions) wxOVERRIDE;
    virtual XYPOSITION WidthText(Font &font_, const char *s, int len) wxOVERRIDE;
    virtual XYPOSITION WidthChar(Font &font_, char ch) wxOVERRIDE;
    virtual XYPOSITION Ascent(Font &font_) wxOVERRIDE;
    virtual XYPOSITION Descent(Font &font_) wxOVERRIDE;
    virtual XYPOSITION InternalLeading(Font &font_) wxOVERRIDE;
    virtual XYPOSITION ExternalLeading(Font &font_) wxOVERRIDE;
    virtual XYPOSITION Height(Font &font_) wxOVERRIDE;
    virtual XYPOSITION AverageCharWidth(Font &font_) wxOVERRIDE;

    virtual void SetClip(PRectangle rc) wxOVERRIDE;
    virtual void FlushCachedState() wxOVERRIDE;

    virtual void SetUnicodeMode(bool unicodeMode_) wxOVERRIDE;
    virtual void SetDBCSMode(int codePage) wxOVERRIDE;

    void BrushColour(ColourDesired back);
    void SetFont(Font &font_);
};



SurfaceImpl::SurfaceImpl() :
    hdc(0), hdcOwned(0), bitmap(0),
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
    InitPixMap(1,1,NULL,wid);
#endif
}

void SurfaceImpl::Init(SurfaceID hdc_, WindowID) {
    Release();
    hdc = (wxDC*)hdc_;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface *surface, WindowID winid) {
    Release();
    if (surface)
        hdc = new wxMemoryDC(static_cast<SurfaceImpl*>(surface)->hdc);
    else
        hdc = new wxMemoryDC();
    hdcOwned = true;
    if (width < 1) width = 1;
    if (height < 1) height = 1;
#ifdef __WXMSW__
    bitmap = new wxBitmap(width, height);
    wxUnusedVar(winid);
#else
    bitmap = new wxBitmap();
    bitmap->CreateScaled(width, height,wxBITMAP_SCREEN_DEPTH,((wxWindow*)winid)->GetContentScaleFactor());
#endif
    ((wxMemoryDC*)hdc)->SelectObject(*bitmap);
}


void SurfaceImpl::Release() {
    if (bitmap) {
        ((wxMemoryDC*)hdc)->SelectObject(wxNullBitmap);
        delete bitmap;
        bitmap = 0;
    }
    if (hdcOwned) {
        delete hdc;
        hdc = 0;
        hdcOwned = false;
    }
}


bool SurfaceImpl::Initialised() {
    return hdc != 0;
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

void SurfaceImpl::Polygon(Point *pts, int npts, ColourDesired fore, ColourDesired back) {
    PenColour(fore);
    BrushColour(back);
    wxPoint *p = new wxPoint[npts];

    for (int i=0; i<npts; i++) {
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
        ColourDesired cdf(fill.AsLong());
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
        ColourDesired cdo(outline.AsLong());
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
#endif


void SurfaceImpl::DrawRGBAImage(PRectangle rc, int width, int height,
                                const unsigned char *pixelsImage)
{
#ifdef wxHAS_RAW_BITMAP
    wxRect r = wxRectFromPRectangle(rc);
    wxBitmap bmp = BitmapFromRGBAImage(width, height, pixelsImage);
    hdc->DrawBitmap(bmp, r.x, r.y, true);
#endif
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

#if wxUSE_UNICODE
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
#else // !wxUSE_UNICODE
    // If not unicode then just use the widths we have
    for (int i = 0; i < len; i++) {
        positions[i] = tpos[i];
    }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}


XYPOSITION SurfaceImpl::WidthText(Font &font, const char *s, int len) {
    SetFont(font);
    int w;
    int h;

    hdc->GetTextExtent(stc2wx(s, len), &w, &h);
    return w;
}


XYPOSITION SurfaceImpl::WidthChar(Font &font, char ch) {
    SetFont(font);
    int w;
    int h;
    char s[2] = { ch, 0 };

    hdc->GetTextExtent(stc2wx(s, 1), &w, &h);
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

XYPOSITION SurfaceImpl::ExternalLeading(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return e;
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


Surface *Surface::Allocate(int WXUNUSED(technology)) {
    return new SurfaceImpl;
}


//----------------------------------------------------------------------


inline wxWindow* GETWIN(WindowID id) { return (wxWindow*)id; }

Window::~Window() {
}

void Window::Destroy() {
    if (wid) {
        Show(false);
        GETWIN(wid)->Destroy();
    }
    wid = 0;
}

bool Window::HasFocus() {
    return wxWindow::FindFocus() == GETWIN(wid);
}

PRectangle Window::GetPosition() {
    if (! wid) return PRectangle();
    wxRect rc(GETWIN(wid)->GetPosition(), GETWIN(wid)->GetSize());
    return PRectangleFromwxRect(rc);
}

void Window::SetPosition(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    GETWIN(wid)->SetSize(r);
}

void Window::SetPositionRelative(PRectangle rc, Window) {
    SetPosition(rc);  // ????
}

PRectangle Window::GetClientPosition() {
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

    wxCursor wc = wxCursor(cursorId);
    if(curs != cursorLast)
    {
        GETWIN(wid)->SetCursor(wc);
        cursorLast = curs;
    }
}


void Window::SetTitle(const char *s) {
    GETWIN(wid)->SetLabel(stc2wx(s));
}


// Returns rectangle of monitor pt is on
PRectangle Window::GetMonitorRect(Point pt) {
    wxRect rect;
    if (! wid) return PRectangle();
#if wxUSE_DISPLAY
    // Get the display the point is found on
    int n = wxDisplay::GetFromPoint(wxPoint(wxRound(pt.x), wxRound(pt.y)));
    wxDisplay dpy(n == wxNOT_FOUND ? 0 : n);
    rect = dpy.GetGeometry();
#else
    wxUnusedVar(pt);
#endif
    return PRectangleFromwxRect(rect);
}

//----------------------------------------------------------------------
// Helper classes for ListBox


// This is a simple subclass of wxListView that just resets focus to the
// parent when it gets it.
class wxSTCListBox : public wxListView {
public:
    wxSTCListBox(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos, const wxSize& size,
                 long style)
        : wxListView()
    {
#ifdef __WXMSW__
        Hide(); // don't flicker as we move it around...
#endif
        Create(parent, id, pos, size, style);
    }


    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnKillFocus(wxFocusEvent& WXUNUSED(event)) {
        // Do nothing.  Prevents base class from resetting the colors...
    }

#ifdef __WXMAC__
    // For some reason I don't understand yet the focus doesn't really leave
    // the listbox like it should, so if we get any events feed them back to
    // the wxSTC
    void OnKeyDown(wxKeyEvent& event) {
        GetGrandParent()->GetEventHandler()->ProcessEvent(event);
    }
    void OnChar(wxKeyEvent& event) {
        GetGrandParent()->GetEventHandler()->ProcessEvent(event);
    }

    // And we need to force the focus back when being destroyed
    ~wxSTCListBox() {
        GetGrandParent()->SetFocus();
    }
#endif

private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSTCListBox, wxListView)
    EVT_SET_FOCUS( wxSTCListBox::OnFocus)
    EVT_KILL_FOCUS(wxSTCListBox::OnKillFocus)
#ifdef __WXMAC__
    EVT_KEY_DOWN(  wxSTCListBox::OnKeyDown)
    EVT_CHAR(      wxSTCListBox::OnChar)
#endif
wxEND_EVENT_TABLE()



#if wxUSE_POPUPWIN //-----------------------------------
#include "wx/popupwin.h"

// A popup window to place the wxSTCListBox upon
class wxSTCListBoxWin : public wxPopupWindow
{
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id, Point WXUNUSED(location)) :
        wxPopupWindow(parent, wxBORDER_SIMPLE)
    {

        lv = new wxSTCListBox(parent, id, wxPoint(-50,-50), wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxBORDER_NONE);
        lv->SetCursor(wxCursor(wxCURSOR_ARROW));
        lv->InsertColumn(0, wxEmptyString);
        lv->InsertColumn(1, wxEmptyString);

        // NOTE: We need to fool the wxListView into thinking that it has the
        // focus so it will use the normal selection colour and will look
        // "right" to the user.  But since the wxPopupWindow or its children
        // can't receive focus then we have to pull a fast one and temporarily
        // parent the listctrl on the STC window and then call SetFocus and
        // then reparent it back to the popup.
        lv->SetFocus();
        lv->Reparent(this);
#ifdef __WXMSW__
        lv->Show();
#endif
#if defined(__WXOSX_COCOA__) || defined(__WXGTK__)
        // This color will end up being our border
        SetBackgroundColour(wxColour(0xC0, 0xC0, 0xC0));
#endif
    }


    // Set position in client coords
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE {
        if (x != wxDefaultCoord) {
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxPopupWindow::DoSetSize(x, y, width, height, sizeFlags);
    }

    // return position as if it were in client coords
    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE {
        int sx, sy;
        wxPopupWindow::DoGetPosition(&sx, &sy);
        GetParent()->ScreenToClient(&sx, &sy);
        if (x) *x = sx;
        if (y) *y = sy;
    }


    bool Destroy() wxOVERRIDE {
        if ( !wxPendingDelete.Member(this) )
            wxPendingDelete.Append(this);
        return true;
    }


    int IconWidth() {
        wxImageList* il = lv->GetImageList(wxIMAGE_LIST_SMALL);
        if (il != NULL) {
            int w, h;
            il->GetSize(0, w, h);
            return w;
        }
        return 0;
    }


    void SetDoubleClickAction(CallBackAction action, void *data) {
        doubleClickAction = action;
        doubleClickActionData = data;
    }


    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnSize(wxSizeEvent& event) {
        // resize the child to fill the popup
        wxSize sz = GetClientSize();
        int x, y, w, h;
        x = y = 0;
        w = sz.x;
        h = sz.y;
#if defined(__WXOSX_COCOA__) || defined(__WXGTK__)
        // make room for the parent's bg color to show, to act as a border
        x = y = 1;
        w -= 2;
        h -= 2;
#endif
        lv->SetSize(x, y, w, h);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, w - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void OnActivate(wxListEvent& WXUNUSED(event)) {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    wxDECLARE_EVENT_TABLE();

};

wxBEGIN_EVENT_TABLE(wxSTCListBoxWin, wxPopupWindow)
    EVT_SET_FOCUS          (          wxSTCListBoxWin::OnFocus)
    EVT_SIZE               (          wxSTCListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSTCListBoxWin::OnActivate)
wxEND_EVENT_TABLE()



#else // !wxUSE_POPUPWIN -----------------------------------
#include "wx/frame.h"

// A normal window to place the wxSTCListBox upon, but make it behave as much
// like a wxPopupWindow as possible
class wxSTCListBoxWin : public wxFrame {
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id, Point location) :
        wxFrame(parent, id, wxEmptyString, wxPoint(location.x, location.y), wxSize(0,0),
                wxFRAME_NO_TASKBAR
                | wxFRAME_FLOAT_ON_PARENT
#ifdef __WXMAC__
                | wxPOPUP_WINDOW
                | wxNO_BORDER
#else
                | wxSIMPLE_BORDER
#endif
            )
    {

        lv = new wxSTCListBox(this, id, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxNO_BORDER);
        lv->SetCursor(wxCursor(wxCURSOR_ARROW));
        lv->InsertColumn(0, wxEmptyString);
        lv->InsertColumn(1, wxEmptyString);

        // Eventhough we immediately reset the focus to the parent, this helps
        // things to look right...
        lv->SetFocus();

        Hide();
    }


    // On OSX and (possibly others) there can still be pending
    // messages/events for the list control when Scintilla wants to
    // close it, so do a pending delete of it instead of destroying
    // immediately.
    bool Destroy()
    {
#ifdef __WXMAC__
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


    int IconWidth()
    {
        wxImageList* il = lv->GetImageList(wxIMAGE_LIST_SMALL);
        if (il != NULL) {
            int w, h;
            il->GetSize(0, w, h);
            return w;
        }
        return 0;
    }


    void SetDoubleClickAction(CallBackAction action, void *data)
    {
        doubleClickAction = action;
        doubleClickActionData = data;
    }


    void OnFocus(wxFocusEvent& event)
    {
        ActivateParent();
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnSize(wxSizeEvent& event)
    {
        // resize the child
        wxSize sz = GetClientSize();
        lv->SetSize(sz);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, sz.x - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void ActivateParent()
    {
        // Although we're a frame, we always want the parent to be active, so
        // raise it whenever we get shown, focused, etc.
        wxTopLevelWindow *frame = wxDynamicCast(
            wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
        if (frame)
            frame->Raise();
    }


    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO)
    {
        // convert coords to screen coords since we're a top-level window
        if (x != wxDefaultCoord) {
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxFrame::DoSetSize(x, y, width, height, sizeFlags);
    }

    virtual bool Show(bool show = true)
    {
        bool rv = wxFrame::Show(show);
        if (rv && show)
            ActivateParent();
#ifdef __WXMAC__
        GetParent()->Refresh(false);
#endif
        return rv;
    }

    void OnActivate(wxListEvent& WXUNUSED(event))
    {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    wxDECLARE_EVENT_TABLE();
};


wxBEGIN_EVENT_TABLE(wxSTCListBoxWin, wxWindow)
    EVT_SET_FOCUS          (          wxSTCListBoxWin::OnFocus)
    EVT_SIZE               (          wxSTCListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSTCListBoxWin::OnActivate)
wxEND_EVENT_TABLE()

#endif // wxUSE_POPUPWIN -----------------------------------


inline wxSTCListBoxWin* GETLBW(WindowID win) {
    return ((wxSTCListBoxWin*)win);
}

inline wxListView* GETLB(WindowID win) {
    return GETLBW(win)->GetLB();
}

//----------------------------------------------------------------------

class ListBoxImpl : public ListBox {
private:
    int                 lineHeight;
    bool                unicodeMode;
    int                 desiredVisibleRows;
    int                 aveCharWidth;
    size_t              maxStrWidth;
    Point               location;       // Caret location at which the list is opened
    wxImageList*        imgList;
    wxArrayInt*         imgTypeMap;

public:
    ListBoxImpl();
    ~ListBoxImpl();
    static ListBox *Allocate();

    virtual void SetFont(Font &font) wxOVERRIDE;
    virtual void Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_, int technology_) wxOVERRIDE;
    virtual void SetAverageCharWidth(int width) wxOVERRIDE;
    virtual void SetVisibleRows(int rows) wxOVERRIDE;
    virtual int GetVisibleRows() const wxOVERRIDE;
    virtual PRectangle GetDesiredRect() wxOVERRIDE;
    virtual int CaretFromEdge() wxOVERRIDE;
    virtual void Clear() wxOVERRIDE;
    virtual void Append(char *s, int type = -1) wxOVERRIDE;
            void Append(const wxString& text, int type);
    virtual int Length() wxOVERRIDE;
    virtual void Select(int n) wxOVERRIDE;
    virtual int GetSelection() wxOVERRIDE;
    virtual int Find(const char *prefix) wxOVERRIDE;
    virtual void GetValue(int n, char *value, int len) wxOVERRIDE;
    virtual void RegisterImage(int type, const char *xpm_data) wxOVERRIDE;
            void RegisterImageHelper(int type, wxBitmap& bmp);
    virtual void RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage) wxOVERRIDE;
    virtual void ClearRegisteredImages() wxOVERRIDE;
    virtual void SetDoubleClickAction(CallBackAction, void *) wxOVERRIDE;
    virtual void SetList(const char* list, char separator, char typesep) wxOVERRIDE;
};


ListBoxImpl::ListBoxImpl()
    : lineHeight(10), unicodeMode(false),
      desiredVisibleRows(5), aveCharWidth(8), maxStrWidth(0),
      imgList(NULL), imgTypeMap(NULL)
{
}

ListBoxImpl::~ListBoxImpl() {
    wxDELETE(imgList);
    wxDELETE(imgTypeMap);
}


void ListBoxImpl::SetFont(Font &font) {
    GETLB(wid)->SetFont(*((wxFont*)font.GetID()));
}


void ListBoxImpl::Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_, int WXUNUSED(technology_)) {
    location = location_;
    lineHeight =  lineHeight_;
    unicodeMode = unicodeMode_;
    maxStrWidth = 0;
    wid = new wxSTCListBoxWin(GETWIN(parent.GetID()), ctrlID, location);
    if (imgList != NULL)
        GETLB(wid)->SetImageList(imgList, wxIMAGE_LIST_SMALL);
}


void ListBoxImpl::SetAverageCharWidth(int width) {
    aveCharWidth = width;
}


void ListBoxImpl::SetVisibleRows(int rows) {
    desiredVisibleRows = rows;
}


int ListBoxImpl::GetVisibleRows() const {
    return desiredVisibleRows;
}

PRectangle ListBoxImpl::GetDesiredRect() {
    // wxListCtrl doesn't have a DoGetBestSize, so instead we kept track of
    // the max size in Append and calculate it here...
    int maxw = maxStrWidth * aveCharWidth;
    int maxh ;

    // give it a default if there are no lines, and/or add a bit more
    if (maxw == 0) maxw = 100;
    maxw += aveCharWidth * 3 +
            GETLBW(wid)->IconWidth() + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    if (maxw > 350)
        maxw = 350;

    // estimate a desired height
    int count = GETLB(wid)->GetItemCount();
    if (count) {
        wxRect rect;
        GETLB(wid)->GetItemRect(0, rect);
        maxh = count * rect.GetHeight();
        if (maxh > 140)  // TODO:  Use desiredVisibleRows??
            maxh = 140;

        // Try to make the size an exact multiple of some number of lines
        int lines = maxh / rect.GetHeight();
        maxh = (lines + 1) * rect.GetHeight() + 2;
    }
    else
        maxh = 100;

    PRectangle rc;
    rc.top = 0;
    rc.left = 0;
    rc.right = maxw;
    rc.bottom = maxh;
    return rc;
}


int ListBoxImpl::CaretFromEdge() {
    return 4 + GETLBW(wid)->IconWidth();
}


void ListBoxImpl::Clear() {
    GETLB(wid)->DeleteAllItems();
}


void ListBoxImpl::Append(char *s, int type) {
    Append(stc2wx(s), type);
}

void ListBoxImpl::Append(const wxString& text, int type) {
    long count  = GETLB(wid)->GetItemCount();
    long itemID  = GETLB(wid)->InsertItem(count, wxEmptyString);
    long idx = -1;
    GETLB(wid)->SetItem(itemID, 1, text);
    maxStrWidth = wxMax(maxStrWidth, text.length());
    if (type != -1) {
        wxCHECK_RET(imgTypeMap, wxT("Unexpected NULL imgTypeMap"));
        idx = imgTypeMap->Item(type);
    }
    GETLB(wid)->SetItemImage(itemID, idx, idx);
}

void ListBoxImpl::SetList(const char* list, char separator, char typesep) {
    GETLB(wid)->Freeze();
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
        Append(token, (int)type);
    }
    GETLB(wid)->Thaw();
}


int ListBoxImpl::Length() {
    return GETLB(wid)->GetItemCount();
}


void ListBoxImpl::Select(int n) {
    bool select = true;
    if (n == -1) {
        n = 0;
        select = false;
    }
    GETLB(wid)->EnsureVisible(n);
    GETLB(wid)->Select(n, select);
}


int ListBoxImpl::GetSelection() {
    return GETLB(wid)->GetFirstSelected();
}


int ListBoxImpl::Find(const char *WXUNUSED(prefix)) {
    // No longer used
    return wxNOT_FOUND;
}


void ListBoxImpl::GetValue(int n, char *value, int len) {
    wxListItem item;
    item.SetId(n);
    item.SetColumn(1);
    item.SetMask(wxLIST_MASK_TEXT);
    GETLB(wid)->GetItem(item);
    strncpy(value, wx2stc(item.GetText()), len);
    value[len-1] = '\0';
}

void ListBoxImpl::RegisterImageHelper(int type, wxBitmap& bmp)
{
    if (! imgList) {
        // assumes all images are the same size
        imgList = new wxImageList(bmp.GetWidth(), bmp.GetHeight(), true);
        imgTypeMap = new wxArrayInt;
    }

    int idx = imgList->Add(bmp);

    // do we need to extend the mapping array?
    wxArrayInt& itm = *imgTypeMap;
    if ( itm.GetCount() < (size_t)type+1)
        itm.Add(-1, type - itm.GetCount() + 1);

    // Add an item that maps type to the image index
    itm[type] = idx;
}

void ListBoxImpl::RegisterImage(int type, const char *xpm_data) {
    wxMemoryInputStream stream(xpm_data, strlen(xpm_data)+1);
    wxImage img(stream, wxBITMAP_TYPE_XPM);
    wxBitmap bmp(img);
    RegisterImageHelper(type, bmp);
}


void ListBoxImpl::RegisterRGBAImage(int type, int width, int height,
                                    const unsigned char *pixelsImage)
{
#ifdef wxHAS_RAW_BITMAP
    wxBitmap bmp = BitmapFromRGBAImage(width, height, pixelsImage);
    RegisterImageHelper(type, bmp);
#endif
}


void ListBoxImpl::ClearRegisteredImages() {
    wxDELETE(imgList);
    wxDELETE(imgTypeMap);
    if (wid)
        GETLB(wid)->SetImageList(NULL, wxIMAGE_LIST_SMALL);
}


void ListBoxImpl::SetDoubleClickAction(CallBackAction action, void *data) {
    GETLBW(wid)->SetDoubleClickAction(action, data);
}


ListBox::ListBox() {
}

ListBox::~ListBox() {
}

ListBox *ListBox::Allocate() {
    return new ListBoxImpl();
}

//----------------------------------------------------------------------

Menu::Menu() : mid(0) {
}

void Menu::CreatePopUp() {
    Destroy();
    mid = new wxMenu();
}

void Menu::Destroy() {
    if (mid)
        delete (wxMenu*)mid;
    mid = 0;
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
    virtual Function FindFunction(const char *name) wxOVERRIDE {
        if (m_dynlib.IsLoaded()) {
            bool status;
            void* fn_address = m_dynlib.GetSymbol(wxString::FromUTF8(name),
                                                  &status);
            if(status)
                return fn_address;
            else
                return NULL;
        }
        else
            return NULL;
    }

    virtual bool IsValid() wxOVERRIDE {
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

bool Platform::MouseButtonBounce() {
    return false;
}

bool Platform::IsKeyDown(int WXUNUSED(key)) {
    return false;  // I don't think we'll need this.
}

long Platform::SendScintilla(WindowID w,
                             unsigned int msg,
                             unsigned long wParam,
                             long lParam) {

    wxStyledTextCtrl* stc = (wxStyledTextCtrl*)w;
    return stc->SendMsg(msg, wParam, lParam);
}

long Platform::SendScintillaPointer(WindowID w,
                                    unsigned int msg,
                                    unsigned long wParam,
                                    void *lParam) {

    wxStyledTextCtrl* stc = (wxStyledTextCtrl*)w;
    return stc->SendMsg(msg, wParam, (wxIntPtr)lParam);
}


// These are utility functions not really tied to a platform

int Platform::Minimum(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

int Platform::Maximum(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
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


int Platform::Clamp(int val, int minVal, int maxVal) {
    if (val > maxVal)
        val = maxVal;
    if (val < minVal)
        val = minVal;
    return val;
}


bool Platform::IsDBCSLeadByte(int WXUNUSED(codePage), char WXUNUSED(ch)) {
    return false;
}

int Platform::DBCSCharLength(int WXUNUSED(codePage), const char *WXUNUSED(s)) {
    return 1;
}

int Platform::DBCSCharMaxLength() {
    return 1;
}


//----------------------------------------------------------------------

ElapsedTime::ElapsedTime() {
    wxLongLong localTime = wxGetLocalTimeMillis();
    littleBit = localTime.GetLo();
    bigBit = localTime.GetHi();
}

double ElapsedTime::Duration(bool reset) {
    wxLongLong prevTime(bigBit, littleBit);
    wxLongLong localTime = wxGetLocalTimeMillis();
    if(reset) {
        littleBit = localTime.GetLo();
        bigBit = localTime.GetHi();
    }
    wxLongLong duration = localTime - prevTime;
    double result = duration.ToDouble();
    result /= 1000.0;
    return result;
}


//----------------------------------------------------------------------

#if wxUSE_UNICODE

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

#endif

#endif // wxUSE_STC
