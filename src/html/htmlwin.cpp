/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwin.cpp
// Purpose:     wxHtmlWindow class for parsing & displaying HTML (implementation)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmlwin.h"
#include "wx/html/forcelnk.h"
#include "wx/log.h"


//-----------------------------------------------------------------------------
// wxHtmlWindow
//-----------------------------------------------------------------------------



#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(HtmlHistoryArray)


wxHtmlWindow::wxHtmlWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                long style, const wxString& name) : wxScrolledWindow(parent, id, pos, size, wxVSCROLL | wxHSCROLL, name)
{
    m_tmpMouseMoved = FALSE;
    m_tmpLastLink = NULL;
    m_tmpCanDrawLocks = 0;
    m_FS = new wxFileSystem();
    m_RelatedStatusBar = -1;
    m_RelatedFrame = NULL;
    m_TitleFormat = "%s";
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Cell = NULL;
    m_Parser = new wxHtmlWinParser(this);
    m_Parser -> SetFS(m_FS);
    SetBorders(10);
    m_HistoryPos = -1;
    m_HistoryOn = TRUE;
    m_Style = style;
    SetPage("<html><body></body></html>");
}



wxHtmlWindow::~wxHtmlWindow()
{
    HistoryClear();

    if (m_Cell) delete m_Cell;

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



void wxHtmlWindow::SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
{
    wxString op = m_OpenedPage;

    m_Parser -> SetFonts(normal_face, fixed_face, sizes);
    SetPage(wxT("<html><body></body></html>")); // fonts changed => contents invalid
    if (!op.IsEmpty()) LoadPage(op);
}



bool wxHtmlWindow::SetPage(const wxString& source)
{
    wxClientDC *dc = new wxClientDC(this);

    dc -> SetMapMode(wxMM_TEXT);
    SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Parser -> SetDC(dc);
    if (m_Cell) {
        delete m_Cell;
        m_Cell = NULL;
    }
    m_Cell = (wxHtmlContainerCell*) m_Parser -> Parse(source);
    delete dc;
    m_Cell -> SetIndent(m_Borders, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cell -> SetAlignHor(wxHTML_ALIGN_CENTER);
    CreateLayout();
    if (m_tmpCanDrawLocks == 0) Refresh();
    return TRUE;
}


bool wxHtmlWindow::LoadPage(const wxString& location)
{
    wxFSFile *f;
    bool rt_val;
    bool needs_refresh = FALSE;
    
    SetCursor(*wxHOURGLASS_CURSOR);
    wxYield(); Refresh(FALSE);

    m_tmpCanDrawLocks++;
    if (m_HistoryOn && (m_HistoryPos != -1)) { // store scroll position into history item
        int x, y;
        ViewStart(&x, &y);
        m_History[m_HistoryPos].SetPos(y);
    }

    if (location[0] == wxT('#')) { // local anchor
        wxString anch = location.Mid(1) /*1 to end*/;
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND && location.BeforeFirst(wxT('#')) == m_OpenedPage) {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND && 
             (m_FS -> GetPath() + location.BeforeFirst(wxT('#'))) == m_OpenedPage) {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }

    else {
        needs_refresh = TRUE;
        // load&display it:
        if (m_RelatedStatusBar != -1) {
            m_RelatedFrame -> SetStatusText(_("Connecting..."), m_RelatedStatusBar);
            Refresh(FALSE);
        }

        f = m_FS -> OpenFile(location);
        
        if (f == NULL) {
            wxString err;

            wxLogError(_("Unable to open requested HTML document: %s"), location.mb_str());
            m_tmpCanDrawLocks--;

            SetCursor(*wxSTANDARD_CURSOR);
            return FALSE;
        }

        else {
            wxNode *node;
            wxString src = wxEmptyString;

            if (m_RelatedStatusBar != -1) {
                wxString msg = _("Loading : ") + location;
                m_RelatedFrame -> SetStatusText(msg, m_RelatedStatusBar);
                Refresh(FALSE);
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
                wxYield();
                ScrollToAnchor(f -> GetAnchor());
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

    if (m_OpenedPageTitle == wxEmptyString)
        OnSetTitle(wxFileNameFromPath(m_OpenedPage));
    SetCursor(*wxSTANDARD_CURSOR);

    if (needs_refresh) {    
        wxYield();
        m_tmpCanDrawLocks--;
        Refresh();
    }
    else
        m_tmpCanDrawLocks--;

    return rt_val;
}



bool wxHtmlWindow::ScrollToAnchor(const wxString& anchor)
{
    const wxHtmlCell *c = m_Cell -> Find(wxHTML_COND_ISANCHOR, &anchor);
    if (!c)
    {
        wxLogWarning(_("HTML anchor %s does not exist."), anchor.mb_str());
        return FALSE;
    }
    else {
        int y;

        for (y = 0; c != NULL; c = c -> GetParent()) y += c -> GetPosY();
        Scroll(-1, y / wxHTML_SCROLL_STEP);
        m_OpenedAnchor = anchor;
        return TRUE;
    }
}


void wxHtmlWindow::OnSetTitle(const wxString& title)
{
    if (m_RelatedFrame) {
        wxString tit;
        tit.Printf(m_TitleFormat, title.c_str());
        m_RelatedFrame -> SetTitle(tit);
    }
    m_OpenedPageTitle = title;
}





void wxHtmlWindow::CreateLayout()
{
    int ClientWidth, ClientHeight;

    if (!m_Cell) return;

    if (m_Style == wxHW_SCROLLBAR_NEVER) {
        SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell -> GetWidth() / wxHTML_SCROLL_STEP, 0); // always off
        GetClientSize(&ClientWidth, &ClientHeight);
        m_Cell -> Layout(ClientWidth);
    }

    else {
        GetClientSize(&ClientWidth, &ClientHeight);
        m_Cell -> Layout(ClientWidth);
        if (ClientHeight < m_Cell -> GetHeight() + GetCharHeight()) {
            SetScrollbars(
                  wxHTML_SCROLL_STEP, wxHTML_SCROLL_STEP,
                  m_Cell -> GetWidth() / wxHTML_SCROLL_STEP,
                  (m_Cell -> GetHeight() + GetCharHeight()) / wxHTML_SCROLL_STEP
                  /*cheat: top-level frag is always container*/);
        }
        else { /* we fit into window, no need for scrollbars */
            SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell -> GetWidth() / wxHTML_SCROLL_STEP, 0); // disable...
            GetClientSize(&ClientWidth, &ClientHeight);
            m_Cell -> Layout(ClientWidth); // ...and relayout
        }
    }
}



void wxHtmlWindow::ReadCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;
    int p_fontsizes[7];
    wxString p_fff, p_ffn;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    m_Borders = cfg -> Read("wxHtmlWindow/Borders", m_Borders);
    p_fff = cfg -> Read("wxHtmlWindow/FontFaceFixed", m_Parser -> m_FontFaceFixed);
    p_ffn = cfg -> Read("wxHtmlWindow/FontFaceNormal", m_Parser -> m_FontFaceNormal);
    for (int i = 0; i < 7; i++) {
        tmp.Printf(wxT("wxHtmlWindow/FontsSize%i"), i);
        p_fontsizes[i] = cfg -> Read(tmp, m_Parser -> m_FontsSizes[i]);
    }
    SetFonts(p_ffn, p_fff, p_fontsizes);

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
    for (int i = 0; i < 7; i++) {
        tmp.Printf(wxT("wxHtmlWindow/FontsSize%i"), i);
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
    m_tmpCanDrawLocks++;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = TRUE;
    wxYield();
    m_tmpCanDrawLocks--;
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
    m_tmpCanDrawLocks++;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = TRUE;
    wxYield();
    m_tmpCanDrawLocks--;
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
wxCursor *wxHtmlWindow::s_cur_hand = NULL;
wxCursor *wxHtmlWindow::s_cur_arrow = NULL;

void wxHtmlWindow::CleanUpStatics()
{
    if (m_DefaultFilter) delete m_DefaultFilter;
    m_DefaultFilter = NULL;
    m_Filters.DeleteContents(TRUE);
    m_Filters.Clear();
    if (s_cur_hand) delete s_cur_hand;
    if (s_cur_arrow) delete s_cur_arrow;
}



void wxHtmlWindow::AddFilter(wxHtmlFilter *filter)
{
    m_Filters.Append(filter);
}




void wxHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    LoadPage(link.GetHref());
}



void wxHtmlWindow::OnDraw(wxDC& dc)
{
    int x, y;
    wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
    int v_y, v_h;

    if (m_tmpCanDrawLocks > 0) return;
    
    dc.SetMapMode(wxMM_TEXT);
#if 0
/* VS - I don't think this is neccessary any longer 
        MSC_VER 1200 means MSVC 6.0 and it works fine */
#if defined(_MSC_VER) && (_MSC_VER == 1200)
    ::SetMapMode((HDC)dc.GetHDC(), MM_TEXT);
#endif
#endif
    dc.SetBackgroundMode(wxTRANSPARENT);
    ViewStart(&x, &y);

    while (upd) {
        v_y = upd.GetY();
        v_h = upd.GetH();
        if (m_Cell) m_Cell -> Draw(dc, 0, 0, y * wxHTML_SCROLL_STEP + v_y, y * wxHTML_SCROLL_STEP + v_h + v_y);
        upd++;
    }
}




void wxHtmlWindow::OnSize(wxSizeEvent& event)
{
    wxScrolledWindow::OnSize(event);
    CreateLayout();
}


void wxHtmlWindow::OnMouseEvent(wxMouseEvent& event)
{
    m_tmpMouseMoved = TRUE;

    if (event.ButtonDown()) {
        int sx, sy;
        wxPoint pos;
        wxString lnk;

        ViewStart(&sx, &sy); sx *= wxHTML_SCROLL_STEP; sy *= wxHTML_SCROLL_STEP;
        pos = event.GetPosition();

        if (m_Cell)
            m_Cell -> OnMouseClick(this, sx + pos.x, sy + pos.y, event);
    }
}



void wxHtmlWindow::OnIdle(wxIdleEvent& event)
{
    if (s_cur_hand == NULL) 
    {
        s_cur_hand = new wxCursor(wxCURSOR_HAND);
        s_cur_arrow = new wxCursor(wxCURSOR_ARROW);
    }

    if (m_tmpMouseMoved && (m_Cell != NULL)) {
        int sx, sy;
        int x, y;
        wxHtmlLinkInfo *lnk;

        ViewStart(&sx, &sy); sx *= wxHTML_SCROLL_STEP; sy *= wxHTML_SCROLL_STEP;
        wxGetMousePosition(&x, &y);
        ScreenToClient(&x, &y);
        lnk = m_Cell -> GetLink(sx + x, sy + y);

        if (lnk != m_tmpLastLink) {
            if (lnk == NULL) {
                SetCursor(*s_cur_arrow);
                if (m_RelatedStatusBar != -1) m_RelatedFrame -> SetStatusText(wxEmptyString, m_RelatedStatusBar);
            }
            else {
                SetCursor(*s_cur_hand);
                if (m_RelatedStatusBar != -1) 
                    m_RelatedFrame -> SetStatusText(lnk -> GetHref(), m_RelatedStatusBar);
            }
            m_tmpLastLink = lnk;
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

FORCE_LINK(m_layout)
FORCE_LINK(m_fonts)
FORCE_LINK(m_image)
FORCE_LINK(m_list)
FORCE_LINK(m_dflist)
FORCE_LINK(m_pre)
FORCE_LINK(m_hline)
FORCE_LINK(m_links)
FORCE_LINK(m_tables)
FORCE_LINK(m_meta)


#endif
