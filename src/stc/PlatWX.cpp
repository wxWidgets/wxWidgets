// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWindows
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <ctype.h>

#include "Platform.h"
#include "wx/stc/stc.h"

Point Point::FromLong(long lpoint) {
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

wxRect wxRectFromPRectangle(PRectangle prc) {
    wxRect rc(prc.left, prc.top,
              prc.right-prc.left, prc.bottom-prc.top);
    return rc;
}

PRectangle PRectangleFromwxRect(wxRect rc) {
    return PRectangle(rc.GetLeft(), rc.GetTop(), rc.GetRight(), rc.GetBottom());
}

Colour::Colour(long lcol) {
    co.Set(lcol & 0xff, (lcol >> 8) & 0xff, (lcol >> 16) & 0xff);
}

Colour::Colour(unsigned int red, unsigned int green, unsigned int blue) {
    co.Set(red, green, blue);
}

bool Colour::operator==(const Colour &other) const {
    return co == other.co;
}

long Colour::AsLong() const {
    return (((long)co.Blue()  << 16) |
            ((long)co.Green() <<  8) |
            ((long)co.Red()));
}

unsigned int Colour::GetRed() {
    return co.Red();
}

unsigned int Colour::GetGreen() {
    return co.Green();
}

unsigned int Colour::GetBlue() {
    return co.Blue();
}

Palette::Palette() {
    used = 0;
    allowRealization = false;
}

Palette::~Palette() {
    Release();
}

void Palette::Release() {
    used = 0;
}

// This method either adds a colour to the list of wanted colours (want==true)
// or retrieves the allocated colour back to the ColourPair.
// This is one method to make it easier to keep the code for wanting and retrieving in sync.
void Palette::WantFind(ColourPair &cp, bool want) {
    if (want) {
        for (int i=0; i < used; i++) {
            if (entries[i].desired == cp.desired)
                return;
        }

        if (used < numEntries) {
            entries[used].desired = cp.desired;
            entries[used].allocated = cp.desired;
            used++;
        }
    } else {
        for (int i=0; i < used; i++) {
            if (entries[i].desired == cp.desired) {
                cp.allocated = entries[i].allocated;
                return;
            }
        }
        cp.allocated = cp.desired;
    }
}

void Palette::Allocate(Window &) {
    if (allowRealization) {
    }
}


Font::Font() {
    id = 0;
    ascent = 0;
}

Font::~Font() {
}

void Font::Create(const char *faceName, int characterSet, int size, bool bold, bool italic) {
    Release();
    id = new wxFont(size,
                    wxDEFAULT,
                    italic ? wxITALIC :  wxNORMAL,
                    bold ? wxBOLD : wxNORMAL,
                    false,
                    faceName,
                    wxFONTENCODING_DEFAULT);
}


void Font::Release() {
    if (id)
        delete id;
    id = 0;
}


Surface::Surface() :
    hdc(0), hdcOwned(0), bitmap(0),
    x(0), y(0) {
}

Surface::~Surface() {
    Release();
}

void Surface::Release() {
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


bool Surface::Initialised() {
    return hdc != 0;
}

void Surface::Init() {
    Release();
    hdc = new wxMemoryDC();
    hdcOwned = true;
    // **** ::SetTextAlign(hdc, TA_BASELINE);
}

void Surface::Init(SurfaceID hdc_) {
    Release();
    hdc = hdc_;
    // **** ::SetTextAlign(hdc, TA_BASELINE);
}

void Surface::InitPixMap(int width, int height, Surface *surface_) {
    Release();
    hdc = new wxMemoryDC(surface_->hdc);
    hdcOwned = true;
    bitmap = new wxBitmap(width, height);
    ((wxMemoryDC*)hdc)->SelectObject(*bitmap);
    // **** ::SetTextAlign(hdc, TA_BASELINE);
}

void Surface::PenColour(Colour fore) {
    hdc->SetPen(wxPen(fore.co, 1, wxSOLID));
}

void Surface::BrushColor(Colour back) {
    hdc->SetBrush(wxBrush(back.co, wxSOLID));
}

void Surface::SetFont(Font &font_) {
  if (font_.GetID()) {
      hdc->SetFont(*font_.GetID());
    }
}

int Surface::LogPixelsY() {
    return hdc->GetPPI().y;
}


int Surface::DeviceHeightFont(int points) {
    return points * LogPixelsY() / 72;
}


void Surface::MoveTo(int x_, int y_) {
    x = x_;
    y = y_;
}

void Surface::LineTo(int x_, int y_) {
    hdc->DrawLine(x,y, x_,y_);
    x = x_;
    y = y_;
}

void Surface::Polygon(Point *pts, int npts, Colour fore,
                      Colour back) {
    PenColour(fore);
    BrushColor(back);
    hdc->DrawPolygon(npts, (wxPoint*)pts);
}

void Surface::RectangleDraw(PRectangle rc, Colour fore, Colour back) {
    PenColour(fore);
    BrushColor(back);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void Surface::FillRectangle(PRectangle rc, Colour back) {
    BrushColor(back);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void Surface::FillRectangle(PRectangle rc, Surface &surfacePattern) {
    wxBrush br;
    if (surfacePattern.bitmap)
        br = wxBrush(*surfacePattern.bitmap);
    else    // Something is wrong so display in red
        br = wxBrush(*wxRED, wxSOLID);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->SetBrush(br);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void Surface::RoundedRectangle(PRectangle rc, Colour fore, Colour back) {
    PenColour(fore);
    BrushColor(back);
    hdc->DrawRoundedRectangle(wxRectFromPRectangle(rc), 4);
}

void Surface::Ellipse(PRectangle rc, Colour fore, Colour back) {
    PenColour(fore);
    BrushColor(back);
    hdc->DrawEllipse(wxRectFromPRectangle(rc));
}

void Surface::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
    wxRect r = wxRectFromPRectangle(rc);
    hdc->Blit(r.x, r.y, r.width, r.height,
              surfaceSource.hdc, from.x, from.y, wxCOPY);
}

void Surface::DrawText(PRectangle rc, Font &font, int ybase,
                       const char *s, int len, Colour fore, Colour back) {
    SetFont(font);
    hdc->SetTextForeground(fore.co);
    hdc->SetTextBackground(back.co);
    FillRectangle(rc, back);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hdc->DrawText(wxString(s, len), rc.left, ybase - font.ascent);
}

void Surface::DrawTextClipped(PRectangle rc, Font &font, int ybase, const char *s, int len, Colour fore, Colour back) {
    SetFont(font);
    hdc->SetTextForeground(fore.co);
    hdc->SetTextBackground(back.co);
    FillRectangle(rc, back);
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));

    // see comments above
    hdc->DrawText(wxString(s, len), rc.left, ybase - font.ascent);
    hdc->DestroyClippingRegion();
}

int Surface::WidthText(Font &font, const char *s, int len) {
    SetFont(font);
    int w;
    int h;
    hdc->GetTextExtent(wxString(s, len), &w, &h);
    return w;
}

void Surface::MeasureWidths(Font &font, const char *s, int len, int *positions) {
    SetFont(font);
    int totalWidth = 0;
    for (int i=0; i<len; i++) {
        int w;
        int h;
        hdc->GetTextExtent(s[i], &w, &h);
        totalWidth += w;
        positions[i] = totalWidth;
    }
}

int Surface::WidthChar(Font &font, char ch) {
    SetFont(font);
    int w;
    int h;
    hdc->GetTextExtent(ch, &w, &h);
    return w;
}

#define EXTENT_TEST " `~!@#$%^&*()-_=+\\|[]{};:\"\'<,>.?/1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

int Surface::Ascent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    font.ascent = h - d;
    return font.ascent;
}

int Surface::Descent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return d;
}

int Surface::InternalLeading(Font &font) {
    return 0;
}

int Surface::ExternalLeading(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return e;
}

int Surface::Height(Font &font) {
    SetFont(font);
    return hdc->GetCharHeight();
}

int Surface::AverageCharWidth(Font &font) {
    SetFont(font);
    return hdc->GetCharWidth();
}

int Surface::SetPalette(Palette *pal, bool inBackGround) {
    return 0;  // **** figure out what to do with palettes...
}

void Surface::SetClip(PRectangle rc) {
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));
}

void Surface::FlushCachedState() {
}

Window::~Window() {
}

void Window::Destroy() {
    if (id)
        id->Destroy();
    id = 0;
}

bool Window::HasFocus() {
    return wxWindow::FindFocus() == id;
}

PRectangle Window::GetPosition() {
    wxRect rc(id->GetPosition(), id->GetSize());
    return PRectangleFromwxRect(rc);
}

void Window::SetPosition(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    id->SetSize(r);
}

void Window::SetPositionRelative(PRectangle rc, Window) {
    SetPosition(rc);  // ????
}

PRectangle Window::GetClientPosition() {
    wxSize sz = id->GetClientSize();
    return  PRectangle(0, 0, sz.x, sz.y);
}

void Window::Show(bool show) {
    id->Show(show);
}

void Window::InvalidateAll() {
    id->Refresh(false);
}

void Window::InvalidateRectangle(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    id->Refresh(false, &r);
}

void Window::SetFont(Font &font) {
    id->SetFont(*font.GetID());
}

void Window::SetCursor(Cursor curs) {
    int cursorId;

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
        cursorId = wxCURSOR_POINT_RIGHT;
        break;
    default:
        cursorId = wxCURSOR_ARROW;
        break;
    }

    id->SetCursor(wxCursor(cursorId));
}


void Window::SetTitle(const char *s) {
    id->SetTitle(s);
}



ListBox::ListBox() {
}

ListBox::~ListBox() {
}

void ListBox::Create(Window &parent, int ctrlID) {
    id = new wxListBox(parent.id, ctrlID, wxDefaultPosition, wxDefaultSize,
                       0, NULL, wxLB_SINGLE | wxLB_SORT);
    ((wxListBox*)id)->Show(FALSE);
}

void ListBox::Clear() {
    ((wxListBox*)id)->Clear();
}

void ListBox::Append(char *s) {
    ((wxListBox*)id)->Append(s);
}

int ListBox::Length() {
    return ((wxListBox*)id)->Number();
}

void ListBox::Select(int n) {
    ((wxListBox*)id)->SetSelection(n);
}

int ListBox::GetSelection() {
    return ((wxListBox*)id)->GetSelection();
}

int ListBox::Find(const char *prefix) {
    if (prefix) {
        for (int x=0; x < ((wxListBox*)id)->Number(); x++) {
            wxString text = ((wxListBox*)id)->GetString(x);
            if (text.StartsWith(prefix))
                return x;
        }
    }
    return -1;
}

void ListBox::GetValue(int n, char *value, int len) {
    wxString text = ((wxListBox*)id)->GetString(n);
    strncpy(value, text.c_str(), len);
    value[len-1] = '\0';
}

void ListBox::Sort() {
    // wxWindows keeps sorted so no need to sort
}


Menu::Menu() : id(0) {
}

void Menu::CreatePopUp() {
    Destroy();
    id = new wxMenu();
}

void Menu::Destroy() {
    if (id)
        delete id;
    id = 0;
}

void Menu::Show(Point pt, Window &w) {
    w.GetID()->PopupMenu(id, pt.x - 4, pt.y);
    Destroy();
}


Colour Platform::Chrome() {
    wxColour c;
    c = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    return Colour(c.Red(), c.Green(), c.Blue());
}

Colour Platform::ChromeHighlight() {
    wxColour c;
    c = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHIGHLIGHT);
    return Colour(c.Red(), c.Green(), c.Blue());
}

const char *Platform::DefaultFont() {
    return wxNORMAL_FONT->GetFaceName();
}

int Platform::DefaultFontSize() {
    return 8;
}

unsigned int Platform::DoubleClickTime() {
    return 500;   // **** ::GetDoubleClickTime();
}

void Platform::DebugDisplay(const char *s) {
    wxLogDebug(s);
}

bool Platform::IsKeyDown(int key) {
    return false;  // I don't think we'll need this.
}

long Platform::SendScintilla(WindowID w,
                             unsigned int msg,
                             unsigned long wParam,
                             long lParam) {

    wxStyledTextCtrl* stc = (wxStyledTextCtrl*)w;
    return stc->SendMsg(msg, wParam, lParam);
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

#define TRACE

void Platform::DebugPrintf(const char *format, ...) {
#ifdef TRACE
    char buffer[2000];
    va_list pArguments;
    va_start(pArguments, format);
    vsprintf(buffer,format,pArguments);
    va_end(pArguments);
    Platform::DebugDisplay(buffer);
#endif
}

int Platform::Clamp(int val, int minVal, int maxVal) {
    if (val > maxVal)
        val = maxVal;
    if (val < minVal)
        val = minVal;
    return val;
}






