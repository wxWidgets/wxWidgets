/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwin.cpp
// Purpose:     wxHtmlWindow class for parsing & displaying HTML (implementation)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "htmlwin.h"
#pragma implementation "htmlproc.h"
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/frame.h"
#endif

#include "wx/html/htmlwin.h"
#include "wx/html/htmlproc.h"
#include "wx/list.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/timer.h"
#include "wx/dcmemory.h"

#include "wx/arrimpl.cpp"
#include "wx/listimpl.cpp"



#if wxUSE_CLIPBOARD
// ----------------------------------------------------------------------------
// wxHtmlWinAutoScrollTimer: the timer used to generate a stream of scroll
// events when a captured mouse is held outside the window
// ----------------------------------------------------------------------------

class wxHtmlWinAutoScrollTimer : public wxTimer
{
public:
    wxHtmlWinAutoScrollTimer(wxScrolledWindow *win,
                      wxEventType eventTypeToSend,
                      int pos, int orient)
    {
        m_win = win;
        m_eventType = eventTypeToSend;
        m_pos = pos;
        m_orient = orient;
    }

    virtual void Notify();

private:
    wxScrolledWindow *m_win;
    wxEventType m_eventType;
    int m_pos,
        m_orient;

    DECLARE_NO_COPY_CLASS(wxHtmlWinAutoScrollTimer)
};

void wxHtmlWinAutoScrollTimer::Notify()
{
    // only do all this as long as the window is capturing the mouse
    if ( wxWindow::GetCapture() != m_win )
    {
        Stop();
    }
    else // we still capture the mouse, continue generating events
    {
        // first scroll the window if we are allowed to do it
        wxScrollWinEvent event1(m_eventType, m_pos, m_orient);
        event1.SetEventObject(m_win);
        if ( m_win->GetEventHandler()->ProcessEvent(event1) )
        {
            // and then send a pseudo mouse-move event to refresh the selection
            wxMouseEvent event2(wxEVT_MOTION);
            wxGetMousePosition(&event2.m_x, &event2.m_y);

            // the mouse event coordinates should be client, not screen as
            // returned by wxGetMousePosition
            wxWindow *parentTop = m_win;
            while ( parentTop->GetParent() )
                parentTop = parentTop->GetParent();
            wxPoint ptOrig = parentTop->GetPosition();
            event2.m_x -= ptOrig.x;
            event2.m_y -= ptOrig.y;

            event2.SetEventObject(m_win);

            // FIXME: we don't fill in the other members - ok?
            m_win->GetEventHandler()->ProcessEvent(event2);
        }
        else // can't scroll further, stop
        {
            Stop();
        }
    }
}

#endif // wxUSE_CLIPBOARD



//-----------------------------------------------------------------------------
// wxHtmlHistoryItem
//-----------------------------------------------------------------------------

// item of history list
class WXDLLIMPEXP_HTML wxHtmlHistoryItem
{
public:
    wxHtmlHistoryItem(const wxString& p, const wxString& a) {m_Page = p, m_Anchor = a, m_Pos = 0;}
    int GetPos() const {return m_Pos;}
    void SetPos(int p) {m_Pos = p;}
    const wxString& GetPage() const {return m_Page;}
    const wxString& GetAnchor() const {return m_Anchor;}

private:
    wxString m_Page;
    wxString m_Anchor;
    int m_Pos;
};


//-----------------------------------------------------------------------------
// our private arrays:
//-----------------------------------------------------------------------------

WX_DECLARE_OBJARRAY(wxHtmlHistoryItem, wxHtmlHistoryArray);
WX_DEFINE_OBJARRAY(wxHtmlHistoryArray);

WX_DECLARE_LIST(wxHtmlProcessor, wxHtmlProcessorList);
WX_DEFINE_LIST(wxHtmlProcessorList);

//-----------------------------------------------------------------------------
// wxHtmlWindow
//-----------------------------------------------------------------------------


void wxHtmlWindow::Init()
{
    m_tmpMouseMoved = FALSE;
    m_tmpLastLink = NULL;
    m_tmpLastCell = NULL;
    m_tmpCanDrawLocks = 0;
    m_FS = new wxFileSystem();
    m_RelatedStatusBar = -1;
    m_RelatedFrame = NULL;
    m_TitleFormat = wxT("%s");
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Cell = NULL;
    m_Parser = new wxHtmlWinParser(this);
    m_Parser->SetFS(m_FS);
    m_HistoryPos = -1;
    m_HistoryOn = TRUE;
    m_History = new wxHtmlHistoryArray;
    m_Processors = NULL;
    m_Style = 0;
    SetBorders(10);
    m_selection = NULL;
    m_makingSelection = false;
#if wxUSE_CLIPBOARD
    m_timerAutoScroll = NULL;
    m_lastDoubleClick = 0;
#endif // wxUSE_CLIPBOARD
    m_backBuffer = NULL;
}

bool wxHtmlWindow::Create(wxWindow *parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    if (!wxScrolledWindow::Create(parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL, name))
        return FALSE;

    m_Style = style;
    SetPage(wxT("<html><body></body></html>"));
    return TRUE;
}


wxHtmlWindow::~wxHtmlWindow()
{
#if wxUSE_CLIPBOARD
    StopAutoScrolling();
#endif // wxUSE_CLIPBOARD
    HistoryClear();

    if (m_Cell) delete m_Cell;

    delete m_Parser;
    delete m_FS;
    delete m_History;
    delete m_Processors;
    delete m_backBuffer;
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

    m_Parser->SetFonts(normal_face, fixed_face, sizes);
    // fonts changed => contents invalid, so reload the page:
    SetPage(wxT("<html><body></body></html>"));
    if (!op.IsEmpty()) LoadPage(op);
}



bool wxHtmlWindow::SetPage(const wxString& source)
{
    wxString newsrc(source);

    wxDELETE(m_selection);

    // pass HTML through registered processors:
    if (m_Processors || m_GlobalProcessors)
    {
        wxHtmlProcessorList::Node *nodeL, *nodeG;
        int prL, prG;

        nodeL = (m_Processors) ? m_Processors->GetFirst() : NULL;
        nodeG = (m_GlobalProcessors) ? m_GlobalProcessors->GetFirst() : NULL;

        // VS: there are two lists, global and local, both of them sorted by
        //     priority. Since we have to go through _both_ lists with
        //     decreasing priority, we "merge-sort" the lists on-line by
        //     processing that one of the two heads that has higher priority
        //     in every iteration
        while (nodeL || nodeG)
        {
            prL = (nodeL) ? nodeL->GetData()->GetPriority() : -1;
            prG = (nodeG) ? nodeG->GetData()->GetPriority() : -1;
            if (prL > prG)
            {
                if (nodeL->GetData()->IsEnabled())
                    newsrc = nodeL->GetData()->Process(newsrc);
                nodeL = nodeL->GetNext();
            }
            else // prL <= prG
            {
                if (nodeG->GetData()->IsEnabled())
                    newsrc = nodeG->GetData()->Process(newsrc);
                nodeG = nodeG->GetNext();
            }
        }
    }

    // ...and run the parser on it:
    wxClientDC *dc = new wxClientDC(this);
    dc->SetMapMode(wxMM_TEXT);
    SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Parser->SetDC(dc);
    if (m_Cell)
    {
        delete m_Cell;
        m_Cell = NULL;
    }
    m_Cell = (wxHtmlContainerCell*) m_Parser->Parse(newsrc);
    delete dc;
    m_Cell->SetIndent(m_Borders, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cell->SetAlignHor(wxHTML_ALIGN_CENTER);
    CreateLayout();
    if (m_tmpCanDrawLocks == 0)
        Refresh();
    return TRUE;
}

bool wxHtmlWindow::AppendToPage(const wxString& source)
{
    return SetPage(*(GetParser()->GetSource()) + source);
}

bool wxHtmlWindow::LoadPage(const wxString& location)
{
    wxBusyCursor busyCursor;

    wxFSFile *f;
    bool rt_val;
    bool needs_refresh = FALSE;

    m_tmpCanDrawLocks++;
    if (m_HistoryOn && (m_HistoryPos != -1))
    {
        // store scroll position into history item:
        int x, y;
        GetViewStart(&x, &y);
        (*m_History)[m_HistoryPos].SetPos(y);
    }

    if (location[0] == wxT('#'))
    {
        // local anchor:
        wxString anch = location.Mid(1) /*1 to end*/;
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND && location.BeforeFirst(wxT('#')) == m_OpenedPage)
    {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND &&
             (m_FS->GetPath() + location.BeforeFirst(wxT('#'))) == m_OpenedPage)
    {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }

    else
    {
        needs_refresh = TRUE;
        // load&display it:
        if (m_RelatedStatusBar != -1)
        {
            m_RelatedFrame->SetStatusText(_("Connecting..."), m_RelatedStatusBar);
            Refresh(FALSE);
        }

        f = m_Parser->OpenURL(wxHTML_URL_PAGE, location);

        // try to interpret 'location' as filename instead of URL:
        if (f == NULL)
        {
            wxFileName fn(location);
            wxString location2 = wxFileSystem::FileNameToURL(fn);
            f = m_Parser->OpenURL(wxHTML_URL_PAGE, location2);
        }

        if (f == NULL)
        {
            wxLogError(_("Unable to open requested HTML document: %s"), location.c_str());
            m_tmpCanDrawLocks--;
            return FALSE;
        }

        else
        {
            wxNode *node;
            wxString src = wxEmptyString;

            if (m_RelatedStatusBar != -1)
            {
                wxString msg = _("Loading : ") + location;
                m_RelatedFrame->SetStatusText(msg, m_RelatedStatusBar);
                Refresh(FALSE);
            }

            node = m_Filters.GetFirst();
            while (node)
            {
                wxHtmlFilter *h = (wxHtmlFilter*) node->GetData();
                if (h->CanRead(*f))
                {
                    src = h->ReadFile(*f);
                    break;
                }
                node = node->GetNext();
            }
            if (src == wxEmptyString)
            {
                if (m_DefaultFilter == NULL) m_DefaultFilter = GetDefaultFilter();
                src = m_DefaultFilter->ReadFile(*f);
            }

            m_FS->ChangePathTo(f->GetLocation());
            rt_val = SetPage(src);
            m_OpenedPage = f->GetLocation();
            if (f->GetAnchor() != wxEmptyString)
            {
                ScrollToAnchor(f->GetAnchor());
            }

            delete f;

            if (m_RelatedStatusBar != -1) m_RelatedFrame->SetStatusText(_("Done"), m_RelatedStatusBar);
        }
    }

    if (m_HistoryOn) // add this page to history there:
    {
        int c = m_History->GetCount() - (m_HistoryPos + 1);

        if (m_HistoryPos < 0 ||
            (*m_History)[m_HistoryPos].GetPage() != m_OpenedPage ||
            (*m_History)[m_HistoryPos].GetAnchor() != m_OpenedAnchor)
        {
            m_HistoryPos++;
            for (int i = 0; i < c; i++)
                m_History->RemoveAt(m_HistoryPos);
            m_History->Add(new wxHtmlHistoryItem(m_OpenedPage, m_OpenedAnchor));
        }
    }

    if (m_OpenedPageTitle == wxEmptyString)
        OnSetTitle(wxFileNameFromPath(m_OpenedPage));

    if (needs_refresh)
    {
        m_tmpCanDrawLocks--;
        Refresh();
    }
    else
        m_tmpCanDrawLocks--;

    return rt_val;
}


bool wxHtmlWindow::LoadFile(const wxFileName& filename)
{
    wxString url = wxFileSystem::FileNameToURL(filename);
    return LoadPage(url);
}


bool wxHtmlWindow::ScrollToAnchor(const wxString& anchor)
{
    const wxHtmlCell *c = m_Cell->Find(wxHTML_COND_ISANCHOR, &anchor);
    if (!c)
    {
        wxLogWarning(_("HTML anchor %s does not exist."), anchor.c_str());
        return FALSE;
    }
    else
    {
        int y;

        for (y = 0; c != NULL; c = c->GetParent()) y += c->GetPosY();
        Scroll(-1, y / wxHTML_SCROLL_STEP);
        m_OpenedAnchor = anchor;
        return TRUE;
    }
}


void wxHtmlWindow::OnSetTitle(const wxString& title)
{
    if (m_RelatedFrame)
    {
        wxString tit;
        tit.Printf(m_TitleFormat, title.c_str());
        m_RelatedFrame->SetTitle(tit);
    }
    m_OpenedPageTitle = title;
}





void wxHtmlWindow::CreateLayout()
{
    int ClientWidth, ClientHeight;

    if (!m_Cell) return;

    if (m_Style & wxHW_SCROLLBAR_NEVER)
    {
        SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0); // always off
        GetClientSize(&ClientWidth, &ClientHeight);
        m_Cell->Layout(ClientWidth);
    }

    else {
        GetClientSize(&ClientWidth, &ClientHeight);
        m_Cell->Layout(ClientWidth);
        if (ClientHeight < m_Cell->GetHeight() + GetCharHeight())
        {
            SetScrollbars(
                  wxHTML_SCROLL_STEP, wxHTML_SCROLL_STEP,
                  m_Cell->GetWidth() / wxHTML_SCROLL_STEP,
                  (m_Cell->GetHeight() + GetCharHeight()) / wxHTML_SCROLL_STEP
                  /*cheat: top-level frag is always container*/);
        }
        else /* we fit into window, no need for scrollbars */
        {
            SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0); // disable...
            GetClientSize(&ClientWidth, &ClientHeight);
            m_Cell->Layout(ClientWidth); // ...and relayout
        }
    }
}



void wxHtmlWindow::ReadCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;
    int p_fontsizes[7];
    wxString p_fff, p_ffn;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(path);
    }

    m_Borders = cfg->Read(wxT("wxHtmlWindow/Borders"), m_Borders);
    p_fff = cfg->Read(wxT("wxHtmlWindow/FontFaceFixed"), m_Parser->m_FontFaceFixed);
    p_ffn = cfg->Read(wxT("wxHtmlWindow/FontFaceNormal"), m_Parser->m_FontFaceNormal);
    for (int i = 0; i < 7; i++)
    {
        tmp.Printf(wxT("wxHtmlWindow/FontsSize%i"), i);
        p_fontsizes[i] = cfg->Read(tmp, m_Parser->m_FontsSizes[i]);
    }
    SetFonts(p_ffn, p_fff, p_fontsizes);

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}



void wxHtmlWindow::WriteCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(path);
    }

    cfg->Write(wxT("wxHtmlWindow/Borders"), (long) m_Borders);
    cfg->Write(wxT("wxHtmlWindow/FontFaceFixed"), m_Parser->m_FontFaceFixed);
    cfg->Write(wxT("wxHtmlWindow/FontFaceNormal"), m_Parser->m_FontFaceNormal);
    for (int i = 0; i < 7; i++)
    {
        tmp.Printf(wxT("wxHtmlWindow/FontsSize%i"), i);
        cfg->Write(tmp, (long) m_Parser->m_FontsSizes[i]);
    }

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}



bool wxHtmlWindow::HistoryBack()
{
    wxString a, l;

    if (m_HistoryPos < 1) return FALSE;

    // store scroll position into history item:
    int x, y;
    GetViewStart(&x, &y);
    (*m_History)[m_HistoryPos].SetPos(y);

    // go to previous position:
    m_HistoryPos--;

    l = (*m_History)[m_HistoryPos].GetPage();
    a = (*m_History)[m_HistoryPos].GetAnchor();
    m_HistoryOn = FALSE;
    m_tmpCanDrawLocks++;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = TRUE;
    m_tmpCanDrawLocks--;
    Scroll(0, (*m_History)[m_HistoryPos].GetPos());
    Refresh();
    return TRUE;
}

bool wxHtmlWindow::HistoryCanBack()
{
    if (m_HistoryPos < 1) return FALSE;
    return TRUE ;
}


bool wxHtmlWindow::HistoryForward()
{
    wxString a, l;

    if (m_HistoryPos == -1) return FALSE;
    if (m_HistoryPos >= (int)m_History->GetCount() - 1)return FALSE;

    m_OpenedPage = wxEmptyString; // this will disable adding new entry into history in LoadPage()

    m_HistoryPos++;
    l = (*m_History)[m_HistoryPos].GetPage();
    a = (*m_History)[m_HistoryPos].GetAnchor();
    m_HistoryOn = FALSE;
    m_tmpCanDrawLocks++;
    if (a == wxEmptyString) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = TRUE;
    m_tmpCanDrawLocks--;
    Scroll(0, (*m_History)[m_HistoryPos].GetPos());
    Refresh();
    return TRUE;
}

bool wxHtmlWindow::HistoryCanForward()
{
    if (m_HistoryPos == -1) return FALSE;
    if (m_HistoryPos >= (int)m_History->GetCount() - 1)return FALSE;
    return TRUE ;
}


void wxHtmlWindow::HistoryClear()
{
    m_History->Empty();
    m_HistoryPos = -1;
}

void wxHtmlWindow::AddProcessor(wxHtmlProcessor *processor)
{
    if (!m_Processors)
    {
        m_Processors = new wxHtmlProcessorList;
        m_Processors->DeleteContents(TRUE);
    }
    wxHtmlProcessorList::Node *node;

    for (node = m_Processors->GetFirst(); node; node = node->GetNext())
    {
        if (processor->GetPriority() > node->GetData()->GetPriority())
        {
            m_Processors->Insert(node, processor);
            return;
        }
    }
    m_Processors->Append(processor);
}

/*static */ void wxHtmlWindow::AddGlobalProcessor(wxHtmlProcessor *processor)
{
    if (!m_GlobalProcessors)
    {
        m_GlobalProcessors = new wxHtmlProcessorList;
        m_GlobalProcessors->DeleteContents(TRUE);
    }
    wxHtmlProcessorList::Node *node;

    for (node = m_GlobalProcessors->GetFirst(); node; node = node->GetNext())
    {
        if (processor->GetPriority() > node->GetData()->GetPriority())
        {
            m_GlobalProcessors->Insert(node, processor);
            return;
        }
    }
    m_GlobalProcessors->Append(processor);
}



wxList wxHtmlWindow::m_Filters;
wxHtmlFilter *wxHtmlWindow::m_DefaultFilter = NULL;
wxHtmlProcessorList *wxHtmlWindow::m_GlobalProcessors = NULL;

void wxHtmlWindow::CleanUpStatics()
{
    wxDELETE(m_DefaultFilter);
    m_Filters.DeleteContents(TRUE);
    m_Filters.Clear();
    wxDELETE(m_GlobalProcessors);
}



void wxHtmlWindow::AddFilter(wxHtmlFilter *filter)
{
    m_Filters.Append(filter);
}


bool wxHtmlWindow::IsSelectionEnabled() const
{
#if wxUSE_CLIPBOARD
    return !(m_Style & wxHW_NO_SELECTION);
#else
    return false;
#endif
}


#if wxUSE_CLIPBOARD
wxString wxHtmlWindow::SelectionToText()
{
    if ( !m_selection )
        return wxEmptyString;

    wxClientDC dc(this);

    const wxHtmlCell *end = m_selection->GetToCell();
    wxString text;
    wxHtmlTerminalCellsInterator i(m_selection->GetFromCell(), end);
    if ( i )
    {
        text << i->ConvertToText(m_selection);
        ++i;
    }
    const wxHtmlCell *prev = *i;
    while ( i )
    {
        if ( prev->GetParent() != i->GetParent() )
            text << _T('\n');
        text << i->ConvertToText(*i == end ? m_selection : NULL);
        prev = *i;
        ++i;
    }
    return text;
}

#endif // wxUSE_CLIPBOARD

void wxHtmlWindow::CopySelection(ClipboardType t)
{
#if wxUSE_CLIPBOARD
    if ( m_selection )
    {
#ifdef __UNIX__
        wxTheClipboard->UsePrimarySelection(t == Primary);
#else // !__UNIX__
        // Primary selection exists only under X11, so don't do anything under
        // the other platforms when we try to access it
        //
        // TODO: this should be abstracted at wxClipboard level!
        if ( t == Primary )
            return;
#endif // __UNIX__/!__UNIX__

        if ( wxTheClipboard->Open() )
        {
            const wxString txt(SelectionToText());
            wxTheClipboard->SetData(new wxTextDataObject(txt));
            wxTheClipboard->Close();
            wxLogTrace(_T("wxhtmlselection"),
                       _("Copied to clipboard:\"%s\""), txt.c_str());
        }
    }
#endif // wxUSE_CLIPBOARD
}


void wxHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    const wxMouseEvent *e = link.GetEvent();
    if (e == NULL || e->LeftUp())
        LoadPage(link.GetHref());
}

void wxHtmlWindow::OnCellClicked(wxHtmlCell *cell,
                                 wxCoord x, wxCoord y,
                                 const wxMouseEvent& event)
{
    wxCHECK_RET( cell, _T("can't be called with NULL cell") );

    cell->OnMouseClick(this, x, y, event);
}

void wxHtmlWindow::OnCellMouseHover(wxHtmlCell * WXUNUSED(cell),
                                    wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    // do nothing here
}

void wxHtmlWindow::OnEraseBackground(wxEraseEvent& event)
{
}

void wxHtmlWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if (m_tmpCanDrawLocks > 0 || m_Cell == NULL) return;

    int x, y;
    GetViewStart(&x, &y);
    wxRect rect = GetUpdateRegion().GetBox();
    wxSize sz = GetSize();

    wxMemoryDC dcm;
    if ( !m_backBuffer )
        m_backBuffer = new wxBitmap(sz.x, sz.y);
    dcm.SelectObject(*m_backBuffer);
    dcm.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
    dcm.Clear();
    PrepareDC(dcm);
    dcm.SetMapMode(wxMM_TEXT);
    dcm.SetBackgroundMode(wxTRANSPARENT);

    wxHtmlRenderingInfo rinfo;
    wxDefaultHtmlRenderingStyle rstyle;
    rinfo.SetSelection(m_selection);
    rinfo.SetStyle(&rstyle);
    m_Cell->Draw(dcm, 0, 0,
                 y * wxHTML_SCROLL_STEP + rect.GetTop(),
                 y * wxHTML_SCROLL_STEP + rect.GetBottom(),
                 rinfo);
 
    dcm.SetDeviceOrigin(0,0);
    dc.Blit(0, rect.GetTop(),
            sz.x, rect.GetBottom() - rect.GetTop() + 1,
            &dcm,
            0, rect.GetTop());
}




void wxHtmlWindow::OnSize(wxSizeEvent& event)
{
    wxDELETE(m_backBuffer);

    wxScrolledWindow::OnSize(event);
    CreateLayout();

    // Recompute selection if necessary:
    if ( m_selection )
    {
        m_selection->Set(m_selection->GetFromCell(),
                         m_selection->GetToCell());
        m_selection->ClearPrivPos();
    }

    Refresh();
}


void wxHtmlWindow::OnMouseMove(wxMouseEvent& event)
{
    m_tmpMouseMoved = true;
}

void wxHtmlWindow::OnMouseDown(wxMouseEvent& event)
{
#if wxUSE_CLIPBOARD
    if ( event.LeftDown() && IsSelectionEnabled() )
    {
        const long TRIPLECLICK_LEN = 200; // 0.2 sec after doubleclick
        if ( wxGetLocalTimeMillis() - m_lastDoubleClick <= TRIPLECLICK_LEN )
        {
            SelectLine(CalcUnscrolledPosition(event.GetPosition()));

            CopySelection();
        }
        else
        {
            m_makingSelection = true;

            if ( m_selection )
            {
                wxDELETE(m_selection);
                Refresh();
            }
            m_tmpSelFromPos = CalcUnscrolledPosition(event.GetPosition());
            m_tmpSelFromCell = NULL;

            CaptureMouse();
        }
    }
#endif // wxUSE_CLIPBOARD
}

void wxHtmlWindow::OnMouseUp(wxMouseEvent& event)
{
#if wxUSE_CLIPBOARD
    if ( m_makingSelection )
    {
        ReleaseMouse();
        m_makingSelection = false;

        // did the user move the mouse far enough from starting point?
        if ( m_selection )
        {
            CopySelection(Primary);

            // we don't want mouse up event that ended selecting to be
            // handled as mouse click and e.g. follow hyperlink:
            return;
        }
    }
#endif // wxUSE_CLIPBOARD

    SetFocus();
    if ( m_Cell )
    {
        wxPoint pos = CalcUnscrolledPosition(event.GetPosition());
        wxHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);

        // check is needed because FindCellByPos returns terminal cell and
        // containers may have empty borders -- in this case NULL will be
        // returned
        if ( cell )
            OnCellClicked(cell, pos.x, pos.y, event);
    }
}



void wxHtmlWindow::OnIdle(wxIdleEvent& WXUNUSED(event))
{    
    if (m_tmpMouseMoved && (m_Cell != NULL))
    {
        int xc, yc, x, y;
        wxGetMousePosition(&xc, &yc);
        ScreenToClient(&xc, &yc);
        CalcUnscrolledPosition(xc, yc, &x, &y);

        wxHtmlCell *cell = m_Cell->FindCellByPos(x, y);

        // handle selection update:
        if ( m_makingSelection )
        {
            bool goingDown = m_tmpSelFromPos.y < y ||
                             m_tmpSelFromPos.y == y && m_tmpSelFromPos.x < x;

            if ( !m_tmpSelFromCell )
                m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y);
            if ( /*still*/ !m_tmpSelFromCell )
            {
                if (goingDown)
                {
                    m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y,
                                         wxHTML_FIND_NEAREST_AFTER);
                    if (!m_tmpSelFromCell)
                        m_tmpSelFromCell = m_Cell->GetFirstTerminal();
                }
                else
                {
                    m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y,
                                         wxHTML_FIND_NEAREST_BEFORE);
                    if (!m_tmpSelFromCell)
                        m_tmpSelFromCell = m_Cell->GetLastTerminal();
                }
            }

            wxHtmlCell *selcell = cell;
            if (!selcell)
            {
                if (goingDown)
                {
                    selcell = m_Cell->FindCellByPos(x, y,
                                                 wxHTML_FIND_NEAREST_AFTER);
                    if (!selcell)
                        selcell = m_Cell->GetLastTerminal();
                }
                else
                {
                    selcell = m_Cell->FindCellByPos(x, y,
                                                 wxHTML_FIND_NEAREST_BEFORE);
                    if (!selcell)
                        selcell = m_Cell->GetFirstTerminal();
                }
            }

            // NB: it may *rarely* happen that the code above didn't find one
            //     of the cells, e.g. if wxHtmlWindow doesn't contain any
            //     visible cells.
            if ( selcell && m_tmpSelFromCell )
            {
                if ( !m_selection )
                {
                    // start selecting only if mouse movement was big enough
                    // (otherwise it was meant as mouse click, not selection):
                    const int PRECISION = 2;
                    wxPoint diff = m_tmpSelFromPos - wxPoint(x,y);
                    if (abs(diff.x) > PRECISION || abs(diff.y) > PRECISION)
                    {
                        m_selection = new wxHtmlSelection();
                    }
                }
                if ( m_selection )
                {
                    if ( m_tmpSelFromCell->IsBefore(selcell) )
                    {
                        m_selection->Set(m_tmpSelFromPos, m_tmpSelFromCell,
                                         wxPoint(x,y), selcell);                                    }
                    else
                    {
                        m_selection->Set(wxPoint(x,y), selcell,
                                         m_tmpSelFromPos, m_tmpSelFromCell);
                    }
                    m_selection->ClearPrivPos();
                    Refresh();
                }
            }
        }

        // handle cursor and status bar text changes:
        if ( cell != m_tmpLastCell )
        {
            wxHtmlLinkInfo *lnk = cell ? cell->GetLink(x, y) : NULL;
            wxCursor cur;
            if (cell)
                cur = cell->GetCursor();
            else
                cur = *wxSTANDARD_CURSOR;
            SetCursor(cur);

            if (lnk != m_tmpLastLink)
            {
                if (lnk == NULL)
                {
                    if (m_RelatedStatusBar != -1)
                        m_RelatedFrame->SetStatusText(wxEmptyString,
                                                      m_RelatedStatusBar);
                }
                else
                {
                    if (m_RelatedStatusBar != -1)
                        m_RelatedFrame->SetStatusText(lnk->GetHref(),
                                                      m_RelatedStatusBar);
                }
                m_tmpLastLink = lnk;
            }

            m_tmpLastCell = cell;
        }
        else // mouse moved but stayed in the same cell
        {
            if ( cell )
                OnCellMouseHover(cell, x, y);
        }

        m_tmpMouseMoved = FALSE;
    }
}

#if wxUSE_CLIPBOARD
void wxHtmlWindow::StopAutoScrolling()
{
    if ( m_timerAutoScroll )
    {
        wxDELETE(m_timerAutoScroll);
    }
}

void wxHtmlWindow::OnMouseEnter(wxMouseEvent& event)
{
    StopAutoScrolling();
    event.Skip();
}

void wxHtmlWindow::OnMouseLeave(wxMouseEvent& event)
{
    // don't prevent the usual processing of the event from taking place
    event.Skip();

    // when a captured mouse leave a scrolled window we start generate
    // scrolling events to allow, for example, extending selection beyond the
    // visible area in some controls
    if ( wxWindow::GetCapture() == this )
    {
        // where is the mouse leaving?
        int pos, orient;
        wxPoint pt = event.GetPosition();
        if ( pt.x < 0 )
        {
            orient = wxHORIZONTAL;
            pos = 0;
        }
        else if ( pt.y < 0 )
        {
            orient = wxVERTICAL;
            pos = 0;
        }
        else // we're lower or to the right of the window
        {
            wxSize size = GetClientSize();
            if ( pt.x > size.x )
            {
                orient = wxHORIZONTAL;
                pos = GetVirtualSize().x / wxHTML_SCROLL_STEP;
            }
            else if ( pt.y > size.y )
            {
                orient = wxVERTICAL;
                pos = GetVirtualSize().y / wxHTML_SCROLL_STEP;
            }
            else // this should be impossible
            {
                // but seems to happen sometimes under wxMSW - maybe it's a bug
                // there but for now just ignore it

                //wxFAIL_MSG( _T("can't understand where has mouse gone") );

                return;
            }
        }

        // only start the auto scroll timer if the window can be scrolled in
        // this direction
        if ( !HasScrollbar(orient) )
            return;

        delete m_timerAutoScroll;
        m_timerAutoScroll = new wxHtmlWinAutoScrollTimer
                                (
                                    this,
                                    pos == 0 ? wxEVT_SCROLLWIN_LINEUP
                                             : wxEVT_SCROLLWIN_LINEDOWN,
                                    pos,
                                    orient
                                );
        m_timerAutoScroll->Start(50); // FIXME: make configurable
    }
}

void wxHtmlWindow::OnKeyUp(wxKeyEvent& event)
{
    if ( IsSelectionEnabled() &&
         event.GetKeyCode() == 'C' && event.ControlDown() )
    {
        if ( m_selection )
            CopySelection();
    }
}

void wxHtmlWindow::OnCopy(wxCommandEvent& event)
{
    if ( m_selection )
        CopySelection();
}

void wxHtmlWindow::OnDoubleClick(wxMouseEvent& event)
{
    // select word under cursor:
    if ( IsSelectionEnabled() )
    {
        SelectWord(CalcUnscrolledPosition(event.GetPosition()));

        CopySelection(Primary);

        m_lastDoubleClick = wxGetLocalTimeMillis();
    }
    else
        event.Skip();
}

void wxHtmlWindow::SelectWord(const wxPoint& pos)
{
    wxHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);
    if ( cell )
    {
        delete m_selection;
        m_selection = new wxHtmlSelection();
        m_selection->Set(cell, cell);
        RefreshRect(wxRect(CalcScrolledPosition(cell->GetAbsPos()),
                           wxSize(cell->GetWidth(), cell->GetHeight())));
    }
}

void wxHtmlWindow::SelectLine(const wxPoint& pos)
{
    wxHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);
    if ( cell )
    {
        // We use following heuristic to find a "line": let the line be all
        // cells in same container as the cell under mouse cursor that are
        // neither completely above nor completely bellow the clicked cell
        // (i.e. are likely to be words positioned on same line of text).

        int y1 = cell->GetAbsPos().y;
        int y2 = y1 + cell->GetHeight();
        int y;
        const wxHtmlCell *c;
        const wxHtmlCell *before = NULL;
        const wxHtmlCell *after = NULL;

        // find last cell of line:
        for ( c = cell->GetNext(); c; c = c->GetNext())
        {
            y = c->GetAbsPos().y;
            if ( y + c->GetHeight() > y1 && y < y2 )
                after = c;
            else
                break;
        }
        if ( !after )
            after = cell;

        // find first cell of line:
        for ( c = cell->GetParent()->GetFirstChild();
                c && c != cell; c = c->GetNext())
        {
            y = c->GetAbsPos().y;
            if ( y + c->GetHeight() > y1 && y < y2 )
            {
                if ( ! before )
                    before = c;
            }
            else
                before = NULL;
        }
        if ( !before )
            before = cell;

        delete m_selection;
        m_selection = new wxHtmlSelection();
        m_selection->Set(before, after);

        Refresh();
    }
}
#endif // wxUSE_CLIPBOARD



IMPLEMENT_ABSTRACT_CLASS(wxHtmlProcessor,wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxHtmlWindow,wxScrolledWindow)

BEGIN_EVENT_TABLE(wxHtmlWindow, wxScrolledWindow)
    EVT_SIZE(wxHtmlWindow::OnSize)
    EVT_LEFT_DOWN(wxHtmlWindow::OnMouseDown)
    EVT_LEFT_UP(wxHtmlWindow::OnMouseUp)
    EVT_RIGHT_UP(wxHtmlWindow::OnMouseUp)
    EVT_MOTION(wxHtmlWindow::OnMouseMove)
    EVT_IDLE(wxHtmlWindow::OnIdle)
    EVT_ERASE_BACKGROUND(wxHtmlWindow::OnEraseBackground)
    EVT_PAINT(wxHtmlWindow::OnPaint)
#if wxUSE_CLIPBOARD
    EVT_LEFT_DCLICK(wxHtmlWindow::OnDoubleClick)
    EVT_ENTER_WINDOW(wxHtmlWindow::OnMouseEnter)
    EVT_LEAVE_WINDOW(wxHtmlWindow::OnMouseLeave)
    EVT_KEY_UP(wxHtmlWindow::OnKeyUp)
    EVT_MENU(wxID_COPY, wxHtmlWindow::OnCopy)
#endif // wxUSE_CLIPBOARD
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


// This hack forces the linker to always link in m_* files
// (wxHTML doesn't work without handlers from these files)
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

#endif // wxUSE_HTML

