// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWidgets
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include <ctype.h>

#include "wx/wx.h"
#include "wx/encconv.h"
#include "wx/listctrl.h"
#include "wx/mstream.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/tokenzr.h"

#include "Platform.h"
#include "PlatWX.h"
#include "wx/stc/stc.h"



Point Point::FromLong(long lpoint) {
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

wxRect wxRectFromPRectangle(PRectangle prc) {
    wxRect r(prc.left, prc.top,
             prc.Width(), prc.Height());
    return r;
}

PRectangle PRectangleFromwxRect(wxRect rc) {
    return PRectangle(rc.GetLeft(), rc.GetTop(),
                      rc.GetRight()+1, rc.GetBottom()+1);
}

wxColour wxColourFromCA(const ColourAllocated& ca) {
    ColourDesired cd(ca.AsLong());
    return wxColour((unsigned char)cd.GetRed(),
                    (unsigned char)cd.GetGreen(),
                    (unsigned char)cd.GetBlue());
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

void Font::Create(const char *faceName, int characterSet, int size, bool bold, bool italic, bool extraFontFlag) {

    Release();

    // The minus one is done because since Scintilla uses SC_SHARSET_DEFAULT
    // internally and we need to have wxFONENCODING_DEFAULT == SC_SHARSET_DEFAULT
    // so we adjust the encoding before passing it to Scintilla.  See also
    // wxStyledTextCtrl::StyleSetCharacterSet
    wxFontEncoding encoding = (wxFontEncoding)(characterSet-1);

    wxFontEncodingArray ea = wxEncodingConverter::GetPlatformEquivalents(encoding);
    if (ea.GetCount())
        encoding = ea[0];

    wxFont* font = new wxFont(size,
                    wxDEFAULT,
                    italic ? wxITALIC :  wxNORMAL,
                    bold ? wxBOLD : wxNORMAL,
                    false,
                    stc2wx(faceName),
                    encoding);
    font->SetNoAntiAliasing(!extraFontFlag);
    id = font;
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

    wxString   str = stc2wx(s, len);
    wxArrayInt tpos;

    SetFont(font);

    hdc->GetPartialTextExtents(str, tpos);

#if wxUSE_UNICODE
    // Map the widths for UCS-2 characters back to the UTF-8 input string
    // NOTE:  I don't think this is right for when sizeof(wxChar) > 2, ie wxGTK2
    // so figure it out and fix it!
    size_t i = 0;
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
#if wxUSE_STL
    std::copy(tpos.begin(), tpos.end(), positions);
#else
    memcpy(positions, tpos.begin(), len * sizeof(int));
#endif
#endif
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
    return hdc->GetCharHeight() + 1;
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
        Show(false);
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
       if(curs != cursorLast)
       {
           GETWIN(id)->SetCursor(wc);
           cursorLast = curs;
       }
}


void Window::SetTitle(const char *s) {
    GETWIN(id)->SetLabel(stc2wx(s));
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
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCListBox, wxListView)
    EVT_SET_FOCUS( wxSTCListBox::OnFocus)
    EVT_KILL_FOCUS(wxSTCListBox::OnKillFocus)
#ifdef __WXMAC__
    EVT_KEY_DOWN(  wxSTCListBox::OnKeyDown)
    EVT_CHAR(      wxSTCListBox::OnChar)
#endif
END_EVENT_TABLE()



#if wxUSE_POPUPWIN //-----------------------------------
#include <wx/popupwin.h>


//
// TODO: Refactor these two classes to have a common base (or a mix-in) to get
// rid of the code duplication.  (Either that or convince somebody to
// implement wxPopupWindow for the Mac!!)
//
// In the meantime, be careful to duplicate any changes as needed...
//

// A popup window to place the wxSTCListBox upon
class wxSTCListBoxWin : public wxPopupWindow
{
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id, Point WXUNUSED(location)) :
        wxPopupWindow(parent, wxBORDER_NONE)
    {
        SetBackgroundColour(*wxBLACK);  // for our simple border

        lv = new wxSTCListBox(parent, id, wxDefaultPosition, wxDefaultSize,
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
    }


    // Set position in client coords
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {
        if (x != wxDefaultCoord) {
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxPopupWindow::DoSetSize(x, y, width, height, sizeFlags);
    }

    // return position as if it were in client coords
    virtual void DoGetPosition( int *x, int *y ) const {
        int sx, sy;
        wxPopupWindow::DoGetPosition(&sx, &sy);
        GetParent()->ScreenToClient(&sx, &sy);
        if (x) *x = sx;
        if (y) *y = sy;
    }


    bool Destroy() {
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
        // resize the child
        wxSize sz = GetSize();
        sz.x -= 2;
        sz.y -= 2;
        lv->SetSize(1, 1, sz.x, sz.y);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, sz.x - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void OnActivate(wxListEvent& WXUNUSED(event)) {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(wxSTCListBoxWin, wxPopupWindow)
    EVT_SET_FOCUS          (          wxSTCListBoxWin::OnFocus)
    EVT_SIZE               (          wxSTCListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSTCListBoxWin::OnActivate)
END_EVENT_TABLE()



#else // wxUSE_POPUPWIN -----------------------------------

// A normal window to place the wxSTCListBox upon.
class wxSTCListBoxWin : public wxWindow {
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSTCListBoxWin(wxWindow* parent, wxWindowID id, Point location) :
        wxWindow(parent, id, wxPoint(location.x, location.y), wxSize(0,0), wxSIMPLE_BORDER )
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
    bool Destroy() {
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
        // resize the child
        wxSize sz = GetClientSize();
        lv->SetSize(sz);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, sz.x - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

#ifdef __WXMAC__
    virtual bool Show(bool show = true) {
        bool rv = wxWindow::Show(show);
        GetParent()->Refresh(false);
        return rv;
    }
#endif

    void OnActivate(wxListEvent& WXUNUSED(event)) {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxSTCListBoxWin, wxWindow)
    EVT_SET_FOCUS          (          wxSTCListBoxWin::OnFocus)
    EVT_SIZE               (          wxSTCListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSTCListBoxWin::OnActivate)
END_EVENT_TABLE()

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

    virtual void SetFont(Font &font);
    virtual void Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_);
    virtual void SetAverageCharWidth(int width);
    virtual void SetVisibleRows(int rows);
    virtual int GetVisibleRows() const;
    virtual PRectangle GetDesiredRect();
    virtual int CaretFromEdge();
    virtual void Clear();
    virtual void Append(char *s, int type = -1);
            void Append(const wxString& text, int type);
    virtual int Length();
    virtual void Select(int n);
    virtual int GetSelection();
    virtual int Find(const char *prefix);
    virtual void GetValue(int n, char *value, int len);
    virtual void RegisterImage(int type, const char *xpm_data);
    virtual void ClearRegisteredImages();
    virtual void SetDoubleClickAction(CallBackAction, void *);
    virtual void SetList(const char* list, char separator, char typesep);
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


void ListBoxImpl::Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_) {
    location = location_;
    lineHeight =  lineHeight_;
    unicodeMode = unicodeMode_;
    maxStrWidth = 0;
    id = new wxSTCListBoxWin(GETWIN(parent.GetID()), ctrlID, location);
    if (imgList != NULL)
        GETLB(id)->SetImageList(imgList, wxIMAGE_LIST_SMALL);
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
    Append(stc2wx(s), type);
}

void ListBoxImpl::Append(const wxString& text, int type) {
    long count  = GETLB(id)->GetItemCount();
    long itemID  = GETLB(id)->InsertItem(count, wxEmptyString);
    GETLB(id)->SetItem(itemID, 1, text);
    maxStrWidth = wxMax(maxStrWidth, text.length());
    if (type != -1) {
        wxCHECK_RET(imgTypeMap, wxT("Unexpected NULL imgTypeMap"));
        long idx = imgTypeMap->Item(type);
        GETLB(id)->SetItemImage(itemID, idx, idx);
    }
}

void ListBoxImpl::SetList(const char* list, char separator, char typesep) {
    GETLB(id)->Freeze();
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
    GETLB(id)->Thaw();
}


int ListBoxImpl::Length() {
    return GETLB(id)->GetItemCount();
}


void ListBoxImpl::Select(int n) {
    bool select = true;
    if (n == -1) {
        n = 0;
        select = false;
    }
    GETLB(id)->Focus(n);
    GETLB(id)->Select(n, select);
}


int ListBoxImpl::GetSelection() {
    return GETLB(id)->GetFirstSelected();
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
    GETLB(id)->GetItem(item);
    strncpy(value, wx2stc(item.GetText()), len);
    value[len-1] = '\0';
}


void ListBoxImpl::RegisterImage(int type, const char *xpm_data) {
    wxMemoryInputStream stream(xpm_data, strlen(xpm_data)+1);
    wxImage img(stream, wxBITMAP_TYPE_XPM);
    wxBitmap bmp(img);

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
    return false;
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
//      if (idButton == IDRETRY) {
//          ::DebugBreak();
//      } else if (idButton == IDIGNORE) {
//          // all OK
//      } else {
//          abort();
//      }
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

#include "UniConversion.h"

// Convert using Scintilla's functions instead of wx's, Scintilla's are more
// forgiving and won't assert...

wxString stc2wx(const char* str, size_t len)
{
    if (!len)
        return wxEmptyString;

    size_t wclen = UCS2Length(str, len);
    wxWCharBuffer buffer(wclen+1);

    size_t actualLen = UCS2FromUTF8(str, len, buffer.data(), wclen+1);
    return wxString(buffer.data(), actualLen);
}



wxString stc2wx(const char* str)
{
    return stc2wx(str, strlen(str));
}


const wxWX2MBbuf wx2stc(const wxString& str)
{
    const wchar_t* wcstr = str.c_str();
    size_t wclen         = str.length();
    size_t len           = UTF8Length(wcstr, wclen);

    wxCharBuffer buffer(len+1);
    UTF8FromUCS2(wcstr, wclen, buffer.data(), len);

    // TODO check NULL termination!!

    return buffer;
}

#endif
