// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWindows
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <ctype.h>

#include <wx/wx.h>
#include <wx/encconv.h>
#include <wx/listctrl.h>
#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/imaglist.h>

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

    virtual void Init(WindowID wid);
    virtual void Init(SurfaceID sid, WindowID wid);
    virtual void InitPixMap(int width, int height, Surface *surface_, WindowID wid);

    virtual void Release();
    virtual bool Initialised();
    virtual void PenColour(ColourAllocated fore);
    virtual int LogPixelsY();
    virtual int DeviceHeightFont(int points);
    virtual void MoveTo(int x_, int y_);
    virtual void LineTo(int x_, int y_);
    virtual void Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back);
    virtual void RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    virtual void FillRectangle(PRectangle rc, ColourAllocated back);
    virtual void FillRectangle(PRectangle rc, Surface &surfacePattern);
    virtual void RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    virtual void Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back);
    virtual void Copy(PRectangle rc, Point from, Surface &surfaceSource);

    virtual void DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
    virtual void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
    virtual void DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore);
    virtual void MeasureWidths(Font &font_, const char *s, int len, int *positions);
    virtual int WidthText(Font &font_, const char *s, int len);
    virtual int WidthChar(Font &font_, char ch);
    virtual int Ascent(Font &font_);
    virtual int Descent(Font &font_);
    virtual int InternalLeading(Font &font_);
    virtual int ExternalLeading(Font &font_);
    virtual int Height(Font &font_);
    virtual int AverageCharWidth(Font &font_);

    virtual int SetPalette(Palette *pal, bool inBackGround);
    virtual void SetClip(PRectangle rc);
    virtual void FlushCachedState();

    virtual void SetUnicodeMode(bool unicodeMode_);
    virtual void SetDBCSMode(int codePage);

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

void SurfaceImpl::InitPixMap(int width, int height, Surface *WXUNUSED(surface_), WindowID) {
    Release();
    hdc = new wxMemoryDC();
    hdcOwned = true;
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    bitmap = new wxBitmap(width, height);
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


void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font, int ybase,
                                      const char *s, int len,
                                      ColourAllocated fore) {

    SetFont(font);
    hdc->SetTextForeground(wxColourFromCA(fore));
    hdc->SetBackgroundMode(wxTRANSPARENT);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hdc->DrawText(stc2wx(s, len), rc.left, ybase - font.ascent);

    hdc->SetBackgroundMode(wxSOLID);
}


void SurfaceImpl::MeasureWidths(Font &font, const char *s, int len, int *positions) {

    wxString str = stc2wx(s, len);
    SetFont(font);

#ifndef __WXMAC__
    // Calculate the position of each character based on the widths of
    // the previous characters
    int* tpos = new int[len+1];
    int totalWidth = 0;
    size_t i;
    for (i=0; i<str.Length(); i++) {
        int w, h;
        hdc->GetTextExtent(str[i], &w, &h);
        totalWidth += w;
        tpos[i] = totalWidth;
    }
#else
    // Instead of a running total, remeasure from the begining of the
    // text for each character's position.  This is because with AA fonts
    // on OS X widths can be fractions of pixels wide when more than one
    // are drawn together, so the sum of all character widths is not necessarily
    // (and probably not) the same as the whole string width.
    int* tpos = new int[len+1];
    size_t i;
    for (i=0; i<str.Length(); i++) {
        int w, h;
        hdc->GetTextExtent(str.Left(i+1), &w, &h);
        tpos[i] = w;
    }
#endif


#if wxUSE_UNICODE
    // Map the widths for UCS-2 characters back to the UTF-8 input string
    // NOTE:  I don't think this is right for when sizeof(wxChar) > 2, ie wxGTK2
    // so figure it out and fix it!
    i = 0;
    size_t ui = 0;
    while ((int)i < len) {
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


int SurfaceImpl::WidthText(Font &font, const char *s, int len) {
    SetFont(font);
    int w;
    int h;

    hdc->GetTextExtent(stc2wx(s, len), &w, &h);
    return w;
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

int SurfaceImpl::InternalLeading(Font &WXUNUSED(font)) {
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

int SurfaceImpl::SetPalette(Palette *WXUNUSED(pal), bool WXUNUSED(inBackGround)) {
    return 0;
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


Surface *Surface::Allocate() {
    return new SurfaceImpl;
}


//----------------------------------------------------------------------


inline wxWindow* GETWIN(WindowID id) { return (wxWindow*)id; }

Window::~Window() {
}

void Window::Destroy() {
    if (id) {
        Show(FALSE);
        GETWIN(id)->Destroy();
    }
    id = 0;
}

bool Window::HasFocus() {
    return wxWindow::FindFocus() == GETWIN(id);
}

PRectangle Window::GetPosition() {
    if (! id) return PRectangle();
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
    if (! id) return PRectangle();
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
    case cursorHand:
        cursorId = wxCURSOR_HAND;
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


// This is a simple subclass of wxLIstView that just resets focus to the
// parent when it gets it.
class wxSTCListBox : public wxListView {
public:
    wxSTCListBox(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos, const wxSize& size,
                 long style)
        : wxListView(parent, id, pos, size, style)
    {}

    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCListBox, wxListView)
    EVT_SET_FOCUS( wxSTCListBox::OnFocus)
END_EVENT_TABLE()




// A window to place the wxSTCListBox upon
class wxSTCListBoxWin : public wxWindow {
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id) :
        wxWindow(parent, id, wxDefaultPosition, wxSize(0,0), wxNO_BORDER )
    {

        SetBackgroundColour(*wxBLACK);
        lv = new wxSTCListBox(this, id, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxNO_BORDER);
        lv->SetCursor(wxCursor(wxCURSOR_ARROW));
        lv->InsertColumn(0, wxEmptyString);
        lv->InsertColumn(1, wxEmptyString);
        Hide();
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
        // resize the child, leaving a 1 pixel border
        wxSize sz = GetClientSize();
        lv->SetSize(1, 1, sz.x-2, sz.y-2);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, sz.x - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void OnActivate(wxListEvent& event) {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxSTCListBoxWin, wxWindow)
    EVT_SET_FOCUS          (    wxSTCListBoxWin::OnFocus)
    EVT_SIZE               (    wxSTCListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(-1, wxSTCListBoxWin::OnActivate)
END_EVENT_TABLE()



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
    int                 maxStrWidth;
    wxImageList*        imgList;
    wxArrayInt*         imgTypeMap;

public:
    ListBoxImpl();
    ~ListBoxImpl();

    virtual void SetFont(Font &font);
    virtual void Create(Window &parent, int ctrlID, int lineHeight_, bool unicodeMode_);
    virtual void SetAverageCharWidth(int width);
    virtual void SetVisibleRows(int rows);
    virtual PRectangle GetDesiredRect();
    virtual int CaretFromEdge();
    virtual void Clear();
    virtual void Append(char *s, int type = -1);
    virtual int Length();
    virtual void Select(int n);
    virtual int GetSelection();
    virtual int Find(const char *prefix);
    virtual void GetValue(int n, char *value, int len);
    virtual void Sort();
    virtual void RegisterImage(int type, const char *xpm_data);
    virtual void ClearRegisteredImages();
    virtual void SetDoubleClickAction(CallBackAction, void *);

};


ListBoxImpl::ListBoxImpl()
    : lineHeight(10), unicodeMode(false),
      desiredVisibleRows(5), aveCharWidth(8), maxStrWidth(0),
      imgList(NULL), imgTypeMap(NULL)
{
}

ListBoxImpl::~ListBoxImpl() {
    if (imgList) {
        delete imgList;
        imgList = NULL;
    }
    if (imgTypeMap) {
        delete imgTypeMap;
        imgTypeMap = NULL;
    }
}


void ListBoxImpl::SetFont(Font &font) {
    GETLB(id)->SetFont(*((wxFont*)font.GetID()));
}


void ListBoxImpl::Create(Window &parent, int ctrlID, int lineHeight_, bool unicodeMode_) {
    lineHeight =  lineHeight_;
    unicodeMode = unicodeMode_;
    maxStrWidth = 0;
    id = new wxSTCListBoxWin(GETWIN(parent.GetID()), ctrlID);
    if (imgList != NULL)
        GETLB(id)->SetImageList(imgList, wxIMAGE_LIST_SMALL);
}


void ListBoxImpl::SetAverageCharWidth(int width) {
    aveCharWidth = width;
}


void ListBoxImpl::SetVisibleRows(int rows) {
    desiredVisibleRows = rows;
}


PRectangle ListBoxImpl::GetDesiredRect() {
    // wxListCtrl doesn't have a DoGetBestSize, so instead we kept track of
    // the max size in Append and calculate it here...
    int maxw = maxStrWidth;
    int maxh ;

    // give it a default if there are no lines, and/or add a bit more
    if (maxw == 0) maxw = 100;
    maxw += aveCharWidth * 3 +
            GETLBW(id)->IconWidth() + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    if (maxw > 350)
        maxw = 350;

    // estimate a desired height
    int count = GETLB(id)->GetItemCount();
    if (count) {
        wxRect rect;
        GETLB(id)->GetItemRect(0, rect);
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
    return 4 + GETLBW(id)->IconWidth();
}


void ListBoxImpl::Clear() {
    GETLB(id)->DeleteAllItems();
}


void ListBoxImpl::Append(char *s, int type) {
    wxString text = stc2wx(s);
    long count  = GETLB(id)->GetItemCount();
    long itemID  = GETLB(id)->InsertItem(count, wxEmptyString);
    GETLB(id)->SetItem(itemID, 1, text);
    int itemWidth = 0;
    GETLB(id)->GetTextExtent(text, &itemWidth, NULL);
    maxStrWidth = wxMax(maxStrWidth, itemWidth);
    if (type != -1) {
        wxCHECK_RET(imgTypeMap, wxT("Unexpected NULL imgTypeMap"));
        long idx = imgTypeMap->Item(type);
        GETLB(id)->SetItemImage(itemID, idx, idx);
    }
}


int ListBoxImpl::Length() {
    return GETLB(id)->GetItemCount();
}


void ListBoxImpl::Select(int n) {
    bool select = TRUE;
    if (n == -1) {
        n = 0;
        select = FALSE;
    }
    GETLB(id)->Focus(n);
    GETLB(id)->Select(n, select);
}


int ListBoxImpl::GetSelection() {
    return GETLB(id)->GetFirstSelected();
}


int ListBoxImpl::Find(const char *WXUNUSED(prefix)) {
    // No longer used
    return -1;
}


void ListBoxImpl::GetValue(int n, char *value, int len) {
    wxListItem item;
    item.SetId(n);
    item.SetColumn(1);
    item.SetMask(wxLIST_MASK_TEXT);
    GETLB(id)->GetItem(item);
    strncpy(value, wx2stc(item.GetText()), len);
    value[len-1] = '\0';
}

void ListBoxImpl::Sort() {
}


void ListBoxImpl::RegisterImage(int type, const char *xpm_data) {
    wxMemoryInputStream stream(xpm_data, strlen(xpm_data)+1);
    wxBitmap bmp(wxImage(stream, wxBITMAP_TYPE_XPM));

    if (! imgList) {
        // assumes all images are the same size
        imgList = new wxImageList(bmp.GetWidth(), bmp.GetHeight(), TRUE);
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

void ListBoxImpl::ClearRegisteredImages() {
    if (imgList) {
        delete imgList;
        imgList = NULL;
    }
    if (imgTypeMap) {
        delete imgTypeMap;
        imgTypeMap = NULL;
    }
    if (id)
        GETLB(id)->SetImageList(NULL, wxIMAGE_LIST_SMALL);
}


void ListBoxImpl::SetDoubleClickAction(CallBackAction action, void *data) {
    GETLBW(id)->SetDoubleClickAction(action, data);
}



ListBox::ListBox() {
}

ListBox::~ListBox() {
}

ListBox *ListBox::Allocate() {
    return new ListBoxImpl();
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

DynamicLibrary *DynamicLibrary::Load(const char *WXUNUSED(modulePath)) {
    wxFAIL_MSG(wxT("Dynamic lexer loading not implemented yet"));
    return NULL;
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
    return wxNORMAL_FONT->GetPointSize();
}

unsigned int Platform::DoubleClickTime() {
    return 500;   // **** ::GetDoubleClickTime();
}

bool Platform::MouseButtonBounce() {
	return FALSE;
}
void Platform::DebugDisplay(const char *s) {
    wxLogDebug(stc2wx(s));
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
    wxStartTimer();
}

double ElapsedTime::Duration(bool reset) {
    double result = wxGetElapsedTime(reset);
    result /= 1000.0;
    return result;
}


//----------------------------------------------------------------------

#if wxUSE_UNICODE
wxString stc2wx(const char* str, size_t len)
{
    char *buffer=new char[len+1];
    strncpy(buffer, str, len);
    buffer[len]=0;

    wxString cstr(buffer, wxConvUTF8);

    delete[] buffer;
    return cstr;
}
#endif






