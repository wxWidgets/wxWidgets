// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWindows
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <ctype.h>

#include <wx/wx.h>
#include <wx/encconv.h>


#include "Platform.h"
#include "PlatWX.h"
#include "wx/stc/stc.h"


#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif


Point Point::FromLong(long lpoint) {
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

wxRect wxRectFromPRectangle(PRectangle prc) {
    wxRect rc(prc.left, prc.top,
              prc.right-prc.left, prc.bottom-prc.top);
    return rc;
}

PRectangle PRectangleFromwxRect(wxRect rc) {
    return PRectangle(rc.GetLeft(), rc.GetTop(),
                      rc.GetRight()+1, rc.GetBottom()+1);
}

wxColour wxColourFromCA(const ColourAllocated& ca) {
    ColourDesired cd(ca.AsLong());
    return wxColour(cd.GetRed(), cd.GetGreen(), cd.GetBlue());
}

//----------------------------------------------------------------------

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
            entries[used].allocated.Set(cp.desired.AsLong());
            used++;
        }
    } else {
        for (int i=0; i < used; i++) {
            if (entries[i].desired == cp.desired) {
                cp.allocated = entries[i].allocated;
                return;
            }
        }
        cp.allocated.Set(cp.desired.AsLong());
    }
}

void Palette::Allocate(Window &) {
    if (allowRealization) {
    }
}


//----------------------------------------------------------------------

Font::Font() {
    id = 0;
    ascent = 0;
}

Font::~Font() {
}

void Font::Create(const char *faceName, int characterSet, int size, bool bold, bool italic) {
    wxFontEncoding encoding;

    Release();

    switch (characterSet) {
        default:
        case wxSTC_CHARSET_ANSI:
        case wxSTC_CHARSET_DEFAULT:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_BALTIC:
            encoding = wxFONTENCODING_ISO8859_13;
            break;

        case wxSTC_CHARSET_CHINESEBIG5:
            encoding = wxFONTENCODING_CP950;
            break;

        case wxSTC_CHARSET_EASTEUROPE:
            encoding = wxFONTENCODING_ISO8859_2;
            break;

        case wxSTC_CHARSET_GB2312:
            encoding = wxFONTENCODING_CP936;
            break;

        case wxSTC_CHARSET_GREEK:
            encoding = wxFONTENCODING_ISO8859_7;
            break;

        case wxSTC_CHARSET_HANGUL:
            encoding = wxFONTENCODING_CP949;
            break;

        case wxSTC_CHARSET_MAC:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_OEM:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_RUSSIAN:
            encoding = wxFONTENCODING_KOI8;
            break;

        case wxSTC_CHARSET_SHIFTJIS:
            encoding = wxFONTENCODING_CP932;
            break;

        case wxSTC_CHARSET_SYMBOL:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_TURKISH:
            encoding = wxFONTENCODING_ISO8859_9;
            break;

        case wxSTC_CHARSET_JOHAB:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_HEBREW:
            encoding = wxFONTENCODING_ISO8859_8;
            break;

        case wxSTC_CHARSET_ARABIC:
            encoding = wxFONTENCODING_ISO8859_6;
            break;

        case wxSTC_CHARSET_VIETNAMESE:
            encoding = wxFONTENCODING_DEFAULT;
            break;

        case wxSTC_CHARSET_THAI:
            encoding = wxFONTENCODING_ISO8859_11;
            break;
    }

    wxFontEncodingArray ea = wxEncodingConverter::GetPlatformEquivalents(encoding);
    if (ea.GetCount())
        encoding = ea[0];

    id = new wxFont(size,
                    wxDEFAULT,
                    italic ? wxITALIC :  wxNORMAL,
                    bold ? wxBOLD : wxNORMAL,
                    false,
                    stc2wx(faceName),
                    encoding);
}


void Font::Release() {
    if (id)
        delete (wxFont*)id;
    id = 0;
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

    void Init();
    void Init(SurfaceID sid);
    void InitPixMap(int width, int height, Surface *surface_);

    void Release();
    bool Initialised();
    void PenColour(ColourAllocated fore);
    int LogPixelsY();
    int DeviceHeightFont(int points);
    void MoveTo(int x_, int y_);
    void LineTo(int x_, int y_);
    void Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back);
    void RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    void FillRectangle(PRectangle rc, ColourAllocated back);
    void FillRectangle(PRectangle rc, Surface &surfacePattern);
    void RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    void Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    void Copy(PRectangle rc, Point from, Surface &surfaceSource);

    void DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
    void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
    void MeasureWidths(Font &font_, const char *s, int len, int *positions);
    int WidthText(Font &font_, const char *s, int len);
    int WidthChar(Font &font_, char ch);
    int Ascent(Font &font_);
    int Descent(Font &font_);
    int InternalLeading(Font &font_);
    int ExternalLeading(Font &font_);
    int Height(Font &font_);
    int AverageCharWidth(Font &font_);

    int SetPalette(Palette *pal, bool inBackGround);
    void SetClip(PRectangle rc);
    void FlushCachedState();

    void SetUnicodeMode(bool unicodeMode_);

    void BrushColour(ColourAllocated back);
    void SetFont(Font &font_);
};



SurfaceImpl::SurfaceImpl() :
    hdc(0), hdcOwned(0), bitmap(0),
    x(0), y(0), unicodeMode(0)
{}

SurfaceImpl::~SurfaceImpl() {
    Release();
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

void SurfaceImpl::Init() {
#if 0
    Release();
    hdc = new wxMemoryDC();
    hdcOwned = true;
#else
    // On Mac and GTK the DC is not really valid until it has a bitmap
    // selected into it.  So instead of just creating the DC with no bitmap,
    // go ahead and give it one.
    InitPixMap(1,1,NULL);
#endif
}

void SurfaceImpl::Init(SurfaceID hdc_) {
    Release();
    hdc = (wxDC*)hdc_;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface *surface_) {
    Release();
    hdc = new wxMemoryDC();
    hdcOwned = true;
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    bitmap = new wxBitmap(width, height);
    ((wxMemoryDC*)hdc)->SelectObject(*bitmap);
}

void SurfaceImpl::PenColour(ColourAllocated fore) {
    hdc->SetPen(wxPen(wxColourFromCA(fore), 1, wxSOLID));
}

void SurfaceImpl::BrushColour(ColourAllocated back) {
    hdc->SetBrush(wxBrush(wxColourFromCA(back), wxSOLID));
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

void SurfaceImpl::Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawPolygon(npts, (wxPoint*)pts);
}

void SurfaceImpl::RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourAllocated back) {
    BrushColour(back);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern) {
    wxBrush br;
    if (((SurfaceImpl&)surfacePattern).bitmap)
        br = wxBrush(*((SurfaceImpl&)surfacePattern).bitmap);
    else    // Something is wrong so display in red
        br = wxBrush(*wxRED, wxSOLID);
    hdc->SetPen(*wxTRANSPARENT_PEN);
    hdc->SetBrush(br);
    hdc->DrawRectangle(wxRectFromPRectangle(rc));
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawRoundedRectangle(wxRectFromPRectangle(rc), 4);
}

void SurfaceImpl::Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
    PenColour(fore);
    BrushColour(back);
    hdc->DrawEllipse(wxRectFromPRectangle(rc));
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
    wxRect r = wxRectFromPRectangle(rc);
    hdc->Blit(r.x, r.y, r.width, r.height,
              ((SurfaceImpl&)surfaceSource).hdc,
              from.x, from.y, wxCOPY);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font, int ybase,
                                 const char *s, int len,
                                 ColourAllocated fore, ColourAllocated back) {
    SetFont(font);
    hdc->SetTextForeground(wxColourFromCA(fore));
    hdc->SetTextBackground(wxColourFromCA(back));
    FillRectangle(rc, back);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hdc->DrawText(stc2wx(s, len), rc.left, ybase - font.ascent);
}

void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font, int ybase,
                                  const char *s, int len,
                                  ColourAllocated fore, ColourAllocated back) {
    SetFont(font);
    hdc->SetTextForeground(wxColourFromCA(fore));
    hdc->SetTextBackground(wxColourFromCA(back));
    FillRectangle(rc, back);
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));

    // see comments above
    hdc->DrawText(stc2wx(s, len), rc.left, ybase - font.ascent);
    hdc->DestroyClippingRegion();
}

int SurfaceImpl::WidthText(Font &font, const char *s, int len) {
    SetFont(font);
    int w;
    int h;

    hdc->GetTextExtent(stc2wx(s, len), &w, &h);
    return w;
}


void SurfaceImpl::MeasureWidths(Font &font, const char *s, int len, int *positions) {
    wxString str = stc2wx(s, len);
    SetFont(font);

    // Calculate the position of each character based on the widths of
    // the previous characters
    int* tpos = new int[len];
    int totalWidth = 0;
    size_t i;
    for (i=0; i<str.Length(); i++) {
        int w, h;
        hdc->GetTextExtent(str[i], &w, &h);
        totalWidth += w;
        tpos[i] = totalWidth;
    }

#if wxUSE_UNICODE
    // Map the widths for UCS-2 characters back to the UTF-8 input string
    i = 0;
    size_t ui = 0;
    while (i < len) {
        unsigned char uch = (unsigned char)s[i];
        positions[i++] = tpos[ui];
        if (uch >= 0x80) {
            if (uch < (0x80 + 0x40 + 0x20)) {
                positions[i++] = tpos[ui];
            } else {
                positions[i++] = tpos[ui];
                positions[i++] = tpos[ui];
            }
        }
        ui++;
    }
#else

    // If not unicode then just use the widths we have
    memcpy(positions, tpos, len * sizeof(*tpos));
#endif

    delete [] tpos;
}


int SurfaceImpl::WidthChar(Font &font, char ch) {
    SetFont(font);
    int w;
    int h;
    char s[2] = { ch, 0 };

    hdc->GetTextExtent(stc2wx(s, 1), &w, &h);
    return w;
}

#define EXTENT_TEST wxT(" `~!@#$%^&*()-_=+\\|[]{};:\"\'<,>.?/1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

int SurfaceImpl::Ascent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    font.ascent = h - d;
    return font.ascent;
}

int SurfaceImpl::Descent(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return d;
}

int SurfaceImpl::InternalLeading(Font &font) {
    return 0;
}

int SurfaceImpl::ExternalLeading(Font &font) {
    SetFont(font);
    int w, h, d, e;
    hdc->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return e;
}

int SurfaceImpl::Height(Font &font) {
    SetFont(font);
    return hdc->GetCharHeight();
}

int SurfaceImpl::AverageCharWidth(Font &font) {
    SetFont(font);
    return hdc->GetCharWidth();
}

int SurfaceImpl::SetPalette(Palette *pal, bool inBackGround) {
    return 0;
}

void SurfaceImpl::SetClip(PRectangle rc) {
    hdc->SetClippingRegion(wxRectFromPRectangle(rc));
}

void SurfaceImpl::FlushCachedState() {
}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) {
    unicodeMode=unicodeMode_;
#if wxUSE_UNICODE
    wxASSERT_MSG(unicodeMode == wxUSE_UNICODE,
                 wxT("Only unicode may be used when wxUSE_UNICODE is on."));
#else
    wxASSERT_MSG(unicodeMode == wxUSE_UNICODE,
                 wxT("Only non-unicode may be used when wxUSE_UNICODE is off."));
#endif
}

Surface *Surface::Allocate() {
    return new SurfaceImpl;
}


//----------------------------------------------------------------------


inline wxWindow* GETWIN(WindowID id) { return (wxWindow*)id; }

Window::~Window() {
}

void Window::Destroy() {
    if (id)
        GETWIN(id)->Destroy();
    id = 0;
}

bool Window::HasFocus() {
    return wxWindow::FindFocus() == GETWIN(id);
}

PRectangle Window::GetPosition() {
    wxRect rc(GETWIN(id)->GetPosition(), GETWIN(id)->GetSize());
    return PRectangleFromwxRect(rc);
}

void Window::SetPosition(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    GETWIN(id)->SetSize(r);
}

void Window::SetPositionRelative(PRectangle rc, Window) {
    SetPosition(rc);  // ????
}

PRectangle Window::GetClientPosition() {
    wxSize sz = GETWIN(id)->GetClientSize();
    return  PRectangle(0, 0, sz.x, sz.y);
}

void Window::Show(bool show) {
    GETWIN(id)->Show(show);
}

void Window::InvalidateAll() {
    GETWIN(id)->Refresh(false);
    wxWakeUpIdle();
}

void Window::InvalidateRectangle(PRectangle rc) {
    wxRect r = wxRectFromPRectangle(rc);
    GETWIN(id)->Refresh(false, &r);
    wxWakeUpIdle();
}

void Window::SetFont(Font &font) {
    GETWIN(id)->SetFont(*((wxFont*)font.GetID()));
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
        cursorId = wxCURSOR_RIGHT_ARROW;
        break;
    default:
        cursorId = wxCURSOR_ARROW;
        break;
    }
#ifdef __WXMOTIF__
       wxCursor wc = wxStockCursor(cursorId) ;
#else
       wxCursor wc = wxCursor(cursorId) ;
#endif
       GETWIN(id)->SetCursor(wc);
}


void Window::SetTitle(const char *s) {
    GETWIN(id)->SetTitle(stc2wx(s));
}


//----------------------------------------------------------------------
// Helper classes for ListBox


#if 1 // defined(__WXMAC__)
class wxSTCListBoxWin : public wxListBox {
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id)
        : wxListBox(parent, id, wxDefaultPosition, wxSize(0,0),
                    0, NULL, wxLB_SINGLE | wxSIMPLE_BORDER) {
        SetCursor(wxCursor(wxCURSOR_ARROW));
        Hide();
    }

    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    wxListBox* GetLB() { return this; }

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxSTCListBoxWin, wxListBox)
    EVT_SET_FOCUS(wxSTCListBoxWin::OnFocus)
END_EVENT_TABLE()



#else


class wxSTCListBox : public wxListBox {
public:
    wxSTCListBox(wxWindow* parent, wxWindowID id)
        : wxListBox(parent, id, wxDefaultPosition, wxDefaultSize,
                    0, NULL, wxLB_SINGLE | wxSIMPLE_BORDER | wxWANTS_CHARS)
        {}

    void OnKeyDown(wxKeyEvent& event) {
        // Give the key events to the STC.  It will then update
        // the listbox as needed.
        GetGrandParent()->GetEventHandler()->ProcessEvent(event);
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCListBox, wxListBox)
    EVT_KEY_DOWN(wxSTCListBox::OnKeyDown)
    EVT_CHAR(wxSTCListBox::OnKeyDown)
END_EVENT_TABLE()



#undef  wxSTC_USE_POPUP
#define wxSTC_USE_POPUP 0  // wxPopupWindow just doesn't work well in this case...

// A window to place the listbox upon.  If wxPopupWindow is supported then
// that will be used so the listbox can extend beyond the client area of the
// wxSTC if needed.
#if wxUSE_POPUPWIN && wxSTC_USE_POPUP
#include <wx/popupwin.h>
#define wxSTCListBoxWinBase wxPopupWindow
#define param2  wxBORDER_NONE  // popup's 2nd param is flags
#else
#define wxSTCListBoxWinBase wxWindow
#define param2 -1 // wxWindow's 2nd param is ID
#endif

class wxSTCListBoxWin : public wxSTCListBoxWinBase {
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id)
        : wxSTCListBoxWinBase(parent, param2) {
        lb = new wxSTCListBox(this, id);
        lb->SetCursor(wxCursor(wxCURSOR_ARROW));
        lb->SetFocus();
   }

    void OnSize(wxSizeEvent& event) {
        lb->SetSize(GetSize());
    }

    wxListBox* GetLB() { return lb; }

#if wxUSE_POPUPWIN && wxSTC_USE_POPUP
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {
        if (x != -1)
            GetParent()->ClientToScreen(&x, NULL);
        if (y != -1)
            GetParent()->ClientToScreen(NULL, &y);
        wxSTCListBoxWinBase::DoSetSize(x, y, width, height, sizeFlags);
    }
#endif

private:
    wxSTCListBox* lb;
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCListBoxWin, wxSTCListBoxWinBase)
    EVT_SIZE(wxSTCListBoxWin::OnSize)
END_EVENT_TABLE()
#endif

inline wxListBox* GETLB(WindowID win) {
    return (((wxSTCListBoxWin*)win)->GetLB());
}

//----------------------------------------------------------------------

ListBox::ListBox() {
}

ListBox::~ListBox() {
}

void ListBox::Create(Window &parent, int ctrlID) {
    id = new wxSTCListBoxWin(GETWIN(parent.GetID()), ctrlID);
}

void ListBox::SetVisibleRows(int rows) {
    desiredVisibleRows = rows;
}

PRectangle ListBox::GetDesiredRect() {
    wxSize sz = GETLB(id)->GetBestSize();
    PRectangle rc;
    rc.top = 0;
    rc.left = 0;
    if (sz.x > 400)
        sz.x = 400;
    if (sz.y > 140)  // TODO:  Use desiredVisibleRows??
        sz.y = 140;
    rc.right = sz.x;
    rc.bottom = sz.y;
    return rc;
}

void ListBox::SetAverageCharWidth(int width) {
    aveCharWidth = width;
}

void ListBox::SetFont(Font &font) {
    GETLB(id)->SetFont(*((wxFont*)font.GetID()));
}

void ListBox::Clear() {
    GETLB(id)->Clear();
}

void ListBox::Append(char *s) {
    GETLB(id)->Append(stc2wx(s));
}

int ListBox::Length() {
    return GETLB(id)->GetCount();
}

void ListBox::Select(int n) {
    GETLB(id)->SetSelection(n);
#ifdef __WXGTK__
    if (n > 4)
        n = n - 4;
    else
        n = 1;
    GETLB(id)->SetFirstItem(n);
#endif
}

int ListBox::GetSelection() {
    return GETLB(id)->GetSelection();
}

int ListBox::Find(const char *prefix) {
    // No longer used
    return -1;
}

void ListBox::GetValue(int n, char *value, int len) {
    wxString text = GETLB(id)->GetString(n);
    strncpy(value, wx2stc(text), len);
    value[len-1] = '\0';
}

void ListBox::Sort() {
}

//----------------------------------------------------------------------

Menu::Menu() : id(0) {
}

void Menu::CreatePopUp() {
    Destroy();
    id = new wxMenu();
}

void Menu::Destroy() {
    if (id)
        delete (wxMenu*)id;
    id = 0;
}

void Menu::Show(Point pt, Window &w) {
    GETWIN(w.GetID())->PopupMenu((wxMenu*)id, pt.x - 4, pt.y);
    Destroy();
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
    strcpy(buf, wxNORMAL_FONT->GetFaceName().mbc_str());
    return buf;
}

int Platform::DefaultFontSize() {
    return 8;
}

unsigned int Platform::DoubleClickTime() {
    return 500;   // **** ::GetDoubleClickTime();
}

void Platform::DebugDisplay(const char *s) {
    wxLogDebug(stc2wx(s));
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

long Platform::SendScintillaPointer(WindowID w,
                                    unsigned int msg,
                                    unsigned long wParam,
                                    void *lParam) {

    wxStyledTextCtrl* stc = (wxStyledTextCtrl*)w;
    return stc->SendMsg(msg, wParam, (long)lParam);
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


static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) {
	bool ret = assertionPopUps;
	assertionPopUps = assertionPopUps_;
	return ret;
}

void Platform::Assert(const char *c, const char *file, int line) {
	char buffer[2000];
	sprintf(buffer, "Assertion [%s] failed at %s %d", c, file, line);
	if (assertionPopUps) {
            /*int idButton = */
            wxMessageBox(stc2wx(buffer),
                         wxT("Assertion failure"),
                         wxICON_HAND | wxOK);
//  		if (idButton == IDRETRY) {
//  			::DebugBreak();
//  		} else if (idButton == IDIGNORE) {
//  			// all OK
//  		} else {
//  			abort();
//  		}
	} else {
		strcat(buffer, "\r\n");
		Platform::DebugDisplay(buffer);
		abort();
	}
}


int Platform::Clamp(int val, int minVal, int maxVal) {
    if (val > maxVal)
        val = maxVal;
    if (val < minVal)
        val = minVal;
    return val;
}


bool Platform::IsDBCSLeadByte(int codePage, char ch) {
    return false;
}



//----------------------------------------------------------------------

ElapsedTime::ElapsedTime() {
    wxStartTimer();
}

double ElapsedTime::Duration(bool reset) {
    double result = wxGetElapsedTime(reset);
    result /= 1000.0;
    return result;
}


//----------------------------------------------------------------------




