/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwin.cpp
// Purpose:     wxHtmlWindow class for parsing & displaying HTML (implementation)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include <wx/html/htmlwin.h>

#include <wx/html/forcelink.h>

///// This is my own wxBusyCursor. It works only with one window.

#if (defined __WXGTK__) && (wxVERSION_NUMBER < 2100)
class wxLocalBusyCursor
#else
class wxLocalBusyCursor : public wxBusyCursor
#endif
{
    private:
        wxWindow *m_Wnd;
    public:
#if (defined __WXGTK__) && (wxVERSION_NUMBER < 2100)
        wxLocalBusyCursor(wxWindow *w) {m_Wnd = w; m_Wnd -> SetCursor(*wxHOURGLASS_CURSOR);}
        ~wxLocalBusyCursor() {m_Wnd -> SetCursor(*wxSTANDARD_CURSOR);}
#else
        wxLocalBusyCursor(wxWindow *w) : wxBusyCursor() {}
#endif
};




//-----------------------------------------------------------------------------
// wxHtmlWindow
//-----------------------------------------------------------------------------



#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(HtmlHistoryArray)


wxHtmlWindow::wxHtmlWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
                const wxString& name, bool scrollable) : wxScrolledWindow(parent, id, pos, size, wxVSCROLL, name)
{
    m_tmpMouseMoved = FALSE;
    m_tmpCanDraw = TRUE;
    m_FS = new wxFileSystem();
    m_RelatedStatusBar = -1;
    m_RelatedFrame = NULL;
    m_TitleFormat = "%s";
    m_OpenedPage = m_OpenedAnchor = wxEmptyString;
    m_Cell = NULL;
    m_Parser = new wxHtmlWinParser(this);
    m_Parser -> SetFS(m_FS);
    SetBorders(10);
    m_HistoryPos = -1;
    m_HistoryOn = TRUE;
    m_Scrollable = scrollable;
    SetPage("<html><body></body></html>");
}



wxHtmlWindow::~wxHtmlWindow()
{
    HistoryClear();

    if (m_Cell) delete m_Cell;

    wxList *parser_data = m_Parser -> GetTempData();
    if (parser_data) delete parser_data;

    delete m_Parser;
    delete m_FS;
}



void wxHtmlWindow::SetRelatedFrame(wxFrame* frame, const wxString& format)
{
    m_RelatedFrame = frame;
    m_TitleFormat = format;
}



void wxHtmlWindow::SetRelatedStatusBar(int bar)
{
    m_RelatedStatusBar = bar;
}
        
        
        
void wxHtmlWindow::SetFonts(wxString normal_face, int normal_italic_mode, wxString fixed_face, int fixed_italic_mode, int *sizes)
{
    m_Parser -> SetFonts(normal_face, normal_italic_mode, fixed_face, fixed_italic_mode, sizes);
    if (!m_OpenedPage.IsEmpty()) LoadPage(m_OpenedPage);
}



bool wxHtmlWindow::SetPage(const wxString& source)
{
    wxClientDC *dc = new wxClientDC(this);

    dc -> SetMapMode(wxMM_TEXT);
    SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    m_OpenedPage = m_OpenedAnchor = wxEmptyString;
    m_Parser -> SetDC(dc);
    if (m_Cell) {
      delete m_Cell;
      m_Cell = NULL;
    }
    m_Cell = (wxHtmlContainerCell*) m_Parser -> Parse(source);
    delete dc;
    m_Cell -> SetIndent(m_Borders, HTML_INDENT_ALL, HTML_UNITS_PIXELS);
    m_Cell -> SetAlignHor(HTML_ALIGN_CENTER);
    CreateLayout();
    Refresh();
    return TRUE;
}


bool wxHtmlWindow::LoadPage(const wxString& location)
{
    wxFSFile *f;
    bool rt_val;
    wxLocalBusyCursor b(this);

    m_tmpCanDraw = FALSE;
    if (m_HistoryOn && (m_HistoryPos != -1)) { // store scroll position into history item
        int x, y;
        ViewStart(&x, &y);
        m_History[m_HistoryPos].SetPos(y);
    }

    if (location[0] == '#') { // local anchor
        wxString anch = location.Mid(1) /*1 to end*/;
        m_tmpCanDraw = TRUE;
        rt_val = ScrollToAnchor(anch);
    }

    else {
        // load&display it:
        if (m_RelatedStatusBar != -1) {
            m_RelatedFrame -> SetStatusText(_("Connecting..."), m_RelatedStatusBar);
            Refresh();
        }

        f = m_FS -> OpenFile(location);
        if (f == NULL) {
            wxString err;

            err.Printf(_("The browser is unable to open requested location :\n\n%s"), WXSTRINGCAST location);
            m_tmpCanDraw = TRUE;
            Refresh();
            wxMessageBox(err, "Error");
            return FALSE;
        }

        else {
            wxNode *node;
            wxString src = wxEmptyString;

            if (m_RelatedStatusBar != -1) {
                wxString msg = _("Loading : ") + location;
                m_RelatedFrame -> SetStatusText(msg, m_RelatedStatusBar);
                Refresh();
            }

            node = m_Filters.GetFirst();
            while (node){
                wxHtmlFilter *h = (wxHtmlFilter*) node -> GetData();
                if (h -> CanRead(*f)) {
                    src = h -> ReadFile(*f);
                    break;
                }
                node = node -> GetNext();
            }
            if (src == wxEmptyString) {
	        if (m_DefaultFilter == NULL) m_DefaultFilter = GetDefaultFilter();
	        src = m_DefaultFilter -> ReadFile(*f);
	    }

            m_FS -> ChangePathTo(f -> GetLocation());
            rt_val = SetPage(src);
            m_OpenedPage = f -> GetLocation();
            if (f -> GetAnchor() != wxEmptyString) {
                m_tmpCanDraw = TRUE;
                ScrollToAnchor(f -> GetAnchor());
                m_tmpCanDraw = FALSE;
            }

            delete f;

            if (m_RelatedStatusBar != -1) m_RelatedFrame -> SetStatusText(_("Done"), m_RelatedStatusBar);
        }
    }

    if (m_HistoryOn) { // add this page to history there:
        int c = m_History.GetCount() - (m_HistoryPos + 1);

        m_HistoryPos++;
        for (int i = 0; i < c; i++)
            m_History.Remove(m_HistoryPos);
        m_History.Add(new HtmlHistoryItem(m_OpenedPage, m_OpenedAnchor));
    }

    m_tmpCanDraw = TRUE;
    Refresh();
    return rt_val;
}



bool wxHtmlWindow::ScrollToAnchor(const wxString& anchor)
{
    const wxHtmlCell *c = m_Cell -> Find(HTML_COND_ISANCHOR, &anchor);
    if (!c) return FALSE;
    else {
        int y;
    
        for (y = 0; c != NULL; c = c -> GetParent()) y += c -> GetPosY();
        Scroll(-1, y / HTML_SCROLL_STEP);
        m_OpenedAnchor = anchor;
        return TRUE;
    }
}


void wxHtmlWindow::SetTitle(const wxString& title)
{
    if (m_RelatedFrame) {
        wxString tit;
        tit.Printf(m_TitleFormat, title.c_str());
        m_RelatedFrame -> SetTitle(tit);
    }
}





void wxHtmlWindow::CreateLayout()
{
    int ClientWidth, ClientHeight;

    if (!m_Cell) return;
    GetClientSize(&ClientWidth, &ClientHeight);
    m_Cell -> Layout(ClientWidth);
    if (m_Scrollable)
        SetScrollbars(HTML_SCROLL_STEP, HTML_SCROLL_STEP,
                      m_Cell -> GetWidth() / HTML_SCROLL_STEP,
                      m_Cell -> GetHeight() / HTML_SCROLL_STEP
                      /*cheat: top-level frag is always container*/ );
}

        

void wxHtmlWindow::ReadCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    m_Borders = cfg -> Read("wxHtmlWindow/Borders", m_Borders);
    m_Parser -> m_FontFaceFixed = cfg -> Read("wxHtmlWindow/FontFaceFixed", m_Parser -> m_FontFaceFixed);
    m_Parser -> m_FontFaceNormal = cfg -> Read("wxHtmlWindow/FontFaceNormal", m_Parser -> m_FontFaceNormal);
    m_Parser -> m_ItalicModeFixed = cfg -> Read("wxHtmlWindow/ItalicModeFixed", m_Parser -> m_ItalicModeFixed);
    m_Parser -> m_ItalicModeNormal = cfg -> Read("wxHtmlWindow/ItalicModeNormal", m_Parser -> m_ItalicModeNormal);
    for (int i = 0; i < 7; i++) {
        tmp.Printf("wxHtmlWindow/FontsSize%i", i);
        m_Parser -> m_FontsSizes[i] = cfg -> Read(tmp, m_Parser -> m_FontsSizes[i]);
    }

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}



void wxHtmlWindow::WriteCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    cfg -> Write("wxHtmlWindow/Borders", (long) m_Borders);
    cfg -> Write("wxHtmlWindow/FontFaceFixed", m_Parser -> m_FontFaceFixed);
    cfg -> Write("wxHtmlWindow/FontFaceNormal", m_Parser -> m_FontFaceNormal);
    cfg -> Write("wxHtmlWindow/ItalicModeFixed", (long) m_Parser -> m_ItalicModeFixed);
    cfg -> Write("wxHtmlWindow/ItalicModeNormal", (long) m_Parser -> m_ItalicModeNormal);
    for (int i = 0; i < 7; i++) {
        tmp.Printf("wxHtmlWindow/FontsSize%i", i);
        cfg -> Write(tmp, (long) m_Parser -> m_FontsSizes[i]);
    }

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}



bool wxHtmlWindow::HistoryBack()
{
    wxString a, l;

    if (m_HistoryPos < 1) return FALSE;

    m_HistoryPos--;

    l = m_History[m_HistoryPos].GetPage();
    a = m_History[m_HistoryPos].GetAnchor();
    m_HistoryOn = FALSE;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + "#" + a);
    m_HistoryOn = TRUE;
    Scroll(0, m_History[m_HistoryPos].GetPos());
    Refresh();
    return TRUE;
}



bool wxHtmlWindow::HistoryForward()
{
    wxString a, l;

    if (m_HistoryPos == -1) return FALSE;
    if (m_HistoryPos >= (int)m_History.GetCount() - 1)return FALSE;

    m_OpenedPage = wxEmptyString; // this will disable adding new entry into history in LoadPage()

    m_HistoryPos++;
    l = m_History[m_HistoryPos].GetPage();
    a = m_History[m_HistoryPos].GetAnchor();
    m_HistoryOn = FALSE;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + "#" + a);
    m_HistoryOn = TRUE;
    Scroll(0, m_History[m_HistoryPos].GetPos());
    Refresh();
    return TRUE;
}



void wxHtmlWindow::HistoryClear()
{
    m_History.Empty();
    m_HistoryPos = -1;
}



wxList wxHtmlWindow::m_Filters;
wxHtmlFilter *wxHtmlWindow::m_DefaultFilter = NULL;

void wxHtmlWindow::CleanUpStatics()
{
    if (m_DefaultFilter) delete m_DefaultFilter;
    m_DefaultFilter = NULL;
}



void wxHtmlWindow::AddFilter(wxHtmlFilter *filter)
{
    m_Filters.DeleteContents(TRUE);
    m_Filters.Append(filter);
}




void wxHtmlWindow::OnLinkClicked(const wxString& link)
{
    LoadPage(link);
}



void wxHtmlWindow::OnDraw(wxDC& dc)
{
    int x, y;
    wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
    int v_y, v_h;

    if (!m_tmpCanDraw) return;
    dc.SetMapMode(wxMM_TEXT);
#if defined(_MSC_VER) && (_MSC_VER == 1200)
    ::SetMapMode((HDC)dc.GetHDC(), MM_TEXT);
#endif
    dc.SetBackgroundMode(wxTRANSPARENT);
    ViewStart(&x, &y);

    while (upd) {
        v_y = upd.GetY();
        v_h = upd.GetH();
        if (m_Cell) m_Cell -> Draw(dc, 0, 0, y * HTML_SCROLL_STEP + v_y, y * HTML_SCROLL_STEP + v_h + v_y);
        upd++;
    }
}




void wxHtmlWindow::OnSize(wxSizeEvent& event)
{
    wxScrolledWindow::OnSize(event);
    CreateLayout();
}



void wxHtmlWindow::OnKeyDown(wxKeyEvent& event)
{
    int dummy;
    int sty, szy, cliy;

    ViewStart(&dummy, &sty);
    GetClientSize(&dummy, &cliy); cliy /= HTML_SCROLL_STEP;
    GetVirtualSize(&dummy, &szy); szy /= HTML_SCROLL_STEP;

    switch (event.KeyCode()) {
        case WXK_PAGEUP :
    case WXK_PRIOR :
                Scroll(-1, sty - cliy);
                break;
        case WXK_PAGEDOWN :
    case WXK_NEXT :
                Scroll(-1, sty + cliy);
                break;
        case WXK_HOME :
                Scroll(-1, 0);
                break;
        case WXK_END :
                Scroll(-1, szy - cliy);
                break;
        case WXK_UP :
                Scroll(-1, sty - 1);
                break;
        case WXK_DOWN :
                Scroll(-1, sty + 1);
                break;
    }
}



void wxHtmlWindow::OnMouseEvent(wxMouseEvent& event)
{
    m_tmpMouseMoved = TRUE;

    if (event.ButtonDown()) {
        int sx, sy;
        wxPoint pos;
        wxString lnk;

        ViewStart(&sx, &sy); sx *= HTML_SCROLL_STEP; sy *= HTML_SCROLL_STEP;
        pos = event.GetPosition();

        if (m_Cell)
            m_Cell -> OnMouseClick(this, sx + pos.x, sy + pos.y, event.ButtonDown(1), event.ButtonDown(2), event.ButtonDown(3));
    }
}



void wxHtmlWindow::OnIdle(wxIdleEvent& event)
{
    static wxCursor cur_hand(wxCURSOR_HAND), cur_arrow(wxCURSOR_ARROW);

    if (m_tmpMouseMoved && (m_Cell != NULL)) {
        int sx, sy;
        int x, y;
        wxString lnk;

        ViewStart(&sx, &sy); sx *= HTML_SCROLL_STEP; sy *= HTML_SCROLL_STEP;
        wxGetMousePosition(&x, &y);
        ScreenToClient(&x, &y);
        lnk = m_Cell -> GetLink(sx + x, sy + y);

        if (lnk == wxEmptyString) {
            SetCursor(cur_arrow);
            if (m_RelatedStatusBar != -1) m_RelatedFrame -> SetStatusText(wxEmptyString, m_RelatedStatusBar);
        }
        else {
            SetCursor(cur_hand);
            if (m_RelatedStatusBar != -1) m_RelatedFrame -> SetStatusText(lnk, m_RelatedStatusBar);
        }
        m_tmpMouseMoved = FALSE;
    }
}




IMPLEMENT_DYNAMIC_CLASS(wxHtmlWindow,wxScrolledWindow)

BEGIN_EVENT_TABLE(wxHtmlWindow, wxScrolledWindow)
    EVT_SIZE(wxHtmlWindow::OnSize)
    EVT_LEFT_DOWN(wxHtmlWindow::OnMouseEvent)
    EVT_MOTION(wxHtmlWindow::OnMouseEvent)
    EVT_IDLE(wxHtmlWindow::OnIdle)
    EVT_KEY_DOWN(wxHtmlWindow::OnKeyDown)
END_EVENT_TABLE()





// A module to allow initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxHtmlWinModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxHtmlWinModule)
public:
    wxHtmlWinModule() : wxModule() {}
    bool OnInit() { return TRUE; }
    void OnExit() { wxHtmlWindow::CleanUpStatics(); }
};

IMPLEMENT_DYNAMIC_CLASS(wxHtmlWinModule, wxModule)




///// default mod handlers are forced there:

FORCE_LINK(mod_layout)
FORCE_LINK(mod_fonts)
FORCE_LINK(mod_image)
FORCE_LINK(mod_list)
FORCE_LINK(mod_pre)
FORCE_LINK(mod_hline)
FORCE_LINK(mod_links)
FORCE_LINK(mod_tables)


#endif
