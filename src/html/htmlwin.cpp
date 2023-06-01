/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmlwin.cpp
// Purpose:     wxHtmlWindow class for parsing & displaying HTML (implementation)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/dcmemory.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/dataobj.h"
    #include "wx/statusbr.h"
#endif

#include "wx/html/htmlwin.h"
#include "wx/html/htmlproc.h"
#include "wx/clipbrd.h"
#include "wx/recguard.h"

#include <list>

// uncomment this line to visually show the extent of the selection
//#define DEBUG_HTML_SELECTION

// HTML events:
wxIMPLEMENT_DYNAMIC_CLASS(wxHtmlLinkEvent, wxCommandEvent);
wxIMPLEMENT_DYNAMIC_CLASS(wxHtmlCellEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_HTML_CELL_CLICKED, wxHtmlCellEvent );
wxDEFINE_EVENT( wxEVT_HTML_CELL_HOVER, wxHtmlCellEvent );
wxDEFINE_EVENT( wxEVT_HTML_LINK_CLICKED, wxHtmlLinkEvent );


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
        : m_eventType(eventTypeToSend)
    {
        m_win = win;
        m_pos = pos;
        m_orient = orient;
    }

    virtual void Notify() override;

private:
    wxScrolledWindow *m_win;
    wxEventType m_eventType;
    int m_pos,
        m_orient;

    wxDECLARE_NO_COPY_CLASS(wxHtmlWinAutoScrollTimer);
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
    wxHtmlHistoryItem(const wxString& p, const wxString& a) : m_Page(p), m_Anchor(a), m_Pos(0) { }
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
// our private containers: they have to be classes as they're forward-declared
//-----------------------------------------------------------------------------

class wxHtmlHistoryArray : public std::vector<wxHtmlHistoryItem>
{
public:
    wxHtmlHistoryArray() = default;
};

class wxHtmlProcessorList : public std::list<std::unique_ptr<wxHtmlProcessor>>
{
public:
    wxHtmlProcessorList() = default;
};

//-----------------------------------------------------------------------------
// wxHtmlWindowMouseHelper
//-----------------------------------------------------------------------------

wxHtmlWindowMouseHelper::wxHtmlWindowMouseHelper(wxHtmlWindowInterface *iface)
    : m_tmpMouseMoved(false),
      m_tmpLastLink(nullptr),
      m_tmpLastCell(nullptr),
      m_interface(iface)
{
}

void wxHtmlWindowMouseHelper::HandleMouseMoved()
{
    m_tmpMouseMoved = true;
}

bool wxHtmlWindowMouseHelper::HandleMouseClick(wxHtmlCell *rootCell,
                                               const wxPoint& pos,
                                               const wxMouseEvent& event)
{
    if (!rootCell)
        return false;

    wxHtmlCell *cell = rootCell->FindCellByPos(pos.x, pos.y);
    // this check is needed because FindCellByPos returns terminal cell and
    // containers may have empty borders -- in this case nullptr will be
    // returned
    if (!cell)
        return false;

    // adjust the coordinates to be relative to this cell:
    wxPoint relpos = pos - cell->GetAbsPos(rootCell);

    return OnCellClicked(cell, relpos.x, relpos.y, event);
}

void wxHtmlWindowMouseHelper::HandleIdle(wxHtmlCell *rootCell,
                                         const wxPoint& pos)
{
    wxHtmlCell *cell = rootCell ? rootCell->FindCellByPos(pos.x, pos.y) : nullptr;

    if (cell != m_tmpLastCell)
    {
        wxHtmlLinkInfo *lnk = nullptr;
        if (cell)
        {
            // adjust the coordinates to be relative to this cell:
            wxPoint relpos = pos - cell->GetAbsPos(rootCell);
            lnk = cell->GetLink(relpos.x, relpos.y);
        }

        wxCursor cur;
        if (cell)
            cur = cell->GetMouseCursorAt(m_interface, pos);
        else
            cur = m_interface->GetHTMLCursor(
                        wxHtmlWindowInterface::HTMLCursor_Default);

        m_interface->GetHTMLWindow()->SetCursor(cur);

        if (lnk != m_tmpLastLink)
        {
            if (lnk)
                m_interface->SetHTMLStatusText(lnk->GetHref());
            else
                m_interface->SetHTMLStatusText(wxEmptyString);

            m_tmpLastLink = lnk;
        }

        m_tmpLastCell = cell;
    }
    else // mouse moved but stayed in the same cell
    {
        if ( cell )
        {
            // A single cell can have different cursors for different positions,
            // so update cursor for this case as well.
            wxCursor cur = cell->GetMouseCursorAt(m_interface, pos);
            m_interface->GetHTMLWindow()->SetCursor(cur);

            OnCellMouseHover(cell, pos.x, pos.y);
        }
    }

    m_tmpMouseMoved = false;
}

bool wxHtmlWindowMouseHelper::OnCellClicked(wxHtmlCell *cell,
                                            wxCoord x, wxCoord y,
                                            const wxMouseEvent& event)
{
    wxHtmlCellEvent ev(wxEVT_HTML_CELL_CLICKED,
                       m_interface->GetHTMLWindow()->GetId(),
                       cell, wxPoint(x,y), event);

    if (!m_interface->GetHTMLWindow()->GetEventHandler()->ProcessEvent(ev))
    {
        // if the event wasn't handled, do the default processing here:

        wxASSERT_MSG( cell, wxT("can't be called with null cell") );

        // If we don't return true, HTML listboxes will always think that they should take
        // the focus
        if (cell->ProcessMouseClick(m_interface, ev.GetPoint(), ev.GetMouseEvent()))
            return true;
    }

    // true if a link was clicked, false otherwise
    return ev.GetLinkClicked();
}

void wxHtmlWindowMouseHelper::OnCellMouseHover(wxHtmlCell * cell,
                                               wxCoord x,
                                               wxCoord y)
{
    wxHtmlCellEvent ev(wxEVT_HTML_CELL_HOVER,
                       m_interface->GetHTMLWindow()->GetId(),
                       cell, wxPoint(x,y), wxMouseEvent());
    m_interface->GetHTMLWindow()->GetEventHandler()->ProcessEvent(ev);
}




//-----------------------------------------------------------------------------
// wxHtmlWindow
//-----------------------------------------------------------------------------

wxList wxHtmlWindow::m_Filters;
wxHtmlFilter *wxHtmlWindow::m_DefaultFilter = nullptr;
wxHtmlProcessorList *wxHtmlWindow::m_GlobalProcessors = nullptr;
wxCursor *wxHtmlWindow::ms_cursorLink = nullptr;
wxCursor *wxHtmlWindow::ms_cursorText = nullptr;
wxCursor *wxHtmlWindow::ms_cursorDefault = nullptr;

void wxHtmlWindow::CleanUpStatics()
{
    wxDELETE(m_DefaultFilter);
    WX_CLEAR_LIST(wxList, m_Filters);
    wxDELETE(m_GlobalProcessors);
    wxDELETE(ms_cursorLink);
    wxDELETE(ms_cursorText);
    wxDELETE(ms_cursorDefault);
}

void wxHtmlWindow::Init()
{
    m_tmpCanDrawLocks = 0;
    m_FS = new wxFileSystem();
#if wxUSE_STATUSBAR
    m_RelatedStatusBar = nullptr;
    m_RelatedStatusBarIndex = -1;
#endif // wxUSE_STATUSBAR
    m_RelatedFrame = nullptr;
    m_TitleFormat = wxT("%s");
    m_OpenedPage.clear();
    m_OpenedAnchor.clear();
    m_OpenedPageTitle.clear();
    m_Cell = nullptr;
    m_Parser = new wxHtmlWinParser(this);
    m_Parser->SetFS(m_FS);
    m_HistoryPos = -1;
    m_HistoryOn = true;
    m_History = new wxHtmlHistoryArray;
    m_Processors = nullptr;
    SetBorders(10);
    m_selection = nullptr;
    m_makingSelection = false;
#if wxUSE_CLIPBOARD
    m_timerAutoScroll = nullptr;
    m_lastDoubleClick = 0;
#endif // wxUSE_CLIPBOARD
    m_tmpSelFromCell = nullptr;
}

bool wxHtmlWindow::Create(wxWindow *parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    if (!wxScrolledWindow::Create(parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL,
                                  name))
        return false;

    // We can't erase our background in EVT_ERASE_BACKGROUND handler and use
    // double buffering in EVT_PAINT handler as this requires blitting back
    // something already drawn on the window to the backing store bitmap when
    // handling EVT_PAINT but blitting in this direction is simply not
    // supported by OS X.
    //
    // So instead we use a hack with artificial EVT_ERASE_BACKGROUND generation
    // from OnPaint() and this means that we never need the "real" erase event
    // at all so disable it to avoid executing any user-defined handlers twice
    // (and to avoid processing unnecessary event if no handlers are defined).
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetPage(wxT("<html><body></body></html>"));

    SetInitialSize(size);
    if ( !HasFlag(wxHW_SCROLLBAR_NEVER) )
        SetScrollRate(wxHTML_SCROLL_STEP, wxHTML_SCROLL_STEP);
    return true;
}


wxHtmlWindow::~wxHtmlWindow()
{
#if wxUSE_CLIPBOARD
    StopAutoScrolling();
#endif // wxUSE_CLIPBOARD
    HistoryClear();

    delete m_selection;

    delete m_Cell;

    delete m_Parser;
    delete m_FS;
    delete m_History;
    delete m_Processors;
}



void wxHtmlWindow::SetRelatedFrame(wxFrame* frame, const wxString& format)
{
    m_RelatedFrame = frame;
    m_TitleFormat = format;

    // Check if the format provided can actually be used: it's more
    // user-friendly to do it here and now rather than triggering the same
    // assert much later when it's really used.

    // If you get an assert here, it means that the title doesn't contain
    // exactly one "%s" format specifier, which is an error in the caller.
    wxString::Format(m_TitleFormat, wxString());
}



#if wxUSE_STATUSBAR
void wxHtmlWindow::SetRelatedStatusBar(int index)
{
    m_RelatedStatusBarIndex = index;
}

void wxHtmlWindow::SetRelatedStatusBar(wxStatusBar* statusbar, int index)
{
    m_RelatedStatusBar =  statusbar;
    m_RelatedStatusBarIndex = index;
}

#endif // wxUSE_STATUSBAR



void wxHtmlWindow::SetFonts(const wxString& normal_face, const wxString& fixed_face, const int *sizes)
{
    m_Parser->SetFonts(normal_face, fixed_face, sizes);

    // re-layout the page after changing fonts:
    DoSetPage(*(m_Parser->GetSource()));
}

void wxHtmlWindow::SetStandardFonts(int size,
                                    const wxString& normal_face,
                                    const wxString& fixed_face)
{
    m_Parser->SetStandardFonts(size, normal_face, fixed_face);

    // re-layout the page after changing fonts:
    DoSetPage(*(m_Parser->GetSource()));
}

bool wxHtmlWindow::SetPage(const wxString& source)
{
    m_OpenedPage.clear();
    m_OpenedAnchor.clear();
    m_OpenedPageTitle.clear();
    return DoSetPage(source);
}

bool wxHtmlWindow::DoSetPage(const wxString& source)
{
    wxString newsrc(source);

    wxDELETE(m_selection);

    // we will soon delete all the cells, so clear pointers to them:
    m_tmpSelFromCell = nullptr;

    // pass HTML through registered processors:
    if (m_Processors || m_GlobalProcessors)
    {
        wxHtmlProcessorList::iterator nodeL, nodeG;

        const auto isNodeLValid = [&nodeL, this]()
        {
            return m_Processors && nodeL != m_Processors->end();
        };

        const auto isNodeGValid = [&nodeG]()
        {
            return m_GlobalProcessors && nodeG != m_GlobalProcessors->end();
        };

        if ( m_Processors )
            nodeL = m_Processors->begin();
        if ( m_GlobalProcessors )
            nodeG = m_GlobalProcessors->begin();

        // VS: there are two lists, global and local, both of them sorted by
        //     priority. Since we have to go through _both_ lists with
        //     decreasing priority, we "merge-sort" the lists on-line by
        //     processing that one of the two heads that has higher priority
        //     in every iteration
        while (isNodeLValid() || isNodeGValid())
        {
            int prL, prG;
            prL = isNodeLValid() ? (*nodeL)->GetPriority() : -1;
            prG = isNodeGValid() ? (*nodeG)->GetPriority() : -1;
            if (prL > prG)
            {
                if ((*nodeL)->IsEnabled())
                    newsrc = (*nodeL)->Process(newsrc);
                ++nodeL;
            }
            else // prL <= prG
            {
                if ((*nodeG)->IsEnabled())
                    newsrc = (*nodeG)->Process(newsrc);
                ++nodeG;
            }
        }
    }

    // ...and run the parser on it:
    wxClientDC dc(this);
    dc.SetMapMode(wxMM_TEXT);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetBackgroundImage(wxNullBitmap);

    double pixelScale = 1.0;
#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
    pixelScale = GetDPIScaleFactor();
#endif

    m_Parser->SetDC(&dc, pixelScale, 1.0);

    // notice that it's important to set m_Cell to nullptr here before calling
    // Parse() below, even if it will be overwritten by its return value as
    // without this we may crash if it's used from inside Parse(), so use
    // wxDELETE() and not just delete here
    wxDELETE(m_Cell);

    m_Cell = (wxHtmlContainerCell*) m_Parser->Parse(newsrc);

    // The parser doesn't need the DC any more, so ensure it's not left with a
    // dangling pointer after the DC object goes out of scope.
    m_Parser->SetDC(nullptr);

    m_Cell->SetIndent(m_Borders, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cell->SetAlignHor(wxHTML_ALIGN_CENTER);
    CreateLayout();
    if (m_tmpCanDrawLocks == 0)
        Refresh();
    return true;
}

bool wxHtmlWindow::AppendToPage(const wxString& source)
{
    return DoSetPage(*(GetParser()->GetSource()) + source);
}

bool wxHtmlWindow::LoadPage(const wxString& location)
{
    wxCHECK_MSG( !location.empty(), false, "location must be non-empty" );

    wxBusyCursor busyCursor;

    bool rt_val;
    bool needs_refresh = false;

    m_tmpCanDrawLocks++;
    if (m_HistoryOn && (m_HistoryPos != -1))
    {
        // store scroll position into history item:
        int x, y;
        GetViewStart(&x, &y);
        (*m_History)[m_HistoryPos].SetPos(y);
    }

    // first check if we're moving to an anchor in the same page
    size_t posLocalAnchor = location.Find('#');
    if ( posLocalAnchor != wxString::npos && posLocalAnchor != 0 )
    {
        // check if the part before the anchor is the same as the (either
        // relative or absolute) URI of the current page
        const wxString beforeAnchor = location.substr(0, posLocalAnchor);
        if ( beforeAnchor != m_OpenedPage &&
                m_FS->GetPath() + beforeAnchor != m_OpenedPage )
        {
            // indicate that we're not moving to a local anchor
            posLocalAnchor = wxString::npos;
        }
    }

    if ( posLocalAnchor != wxString::npos )
    {
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(location.substr(posLocalAnchor + 1));
        m_tmpCanDrawLocks++;
    }
    else // moving to another page
    {
        needs_refresh = true;
#if wxUSE_STATUSBAR
        // load&display it:
        if (m_RelatedStatusBarIndex != -1)
        {
            SetHTMLStatusText(_("Connecting..."));
            Refresh(false);
        }
#endif // wxUSE_STATUSBAR

        wxFSFile *f = m_Parser->OpenURL(wxHTML_URL_PAGE, location);

        // try to interpret 'location' as filename instead of URL:
        if (f == nullptr)
        {
            wxFileName fn(location);
            wxString location2 = wxFileSystem::FileNameToURL(fn);
            f = m_Parser->OpenURL(wxHTML_URL_PAGE, location2);
        }

        if (f == nullptr)
        {
            wxLogError(_("Unable to open requested HTML document: %s"), location);
            m_tmpCanDrawLocks--;
            SetHTMLStatusText(wxEmptyString);
            return false;
        }

        else
        {
            wxList::compatibility_iterator node;
            wxString src;

#if wxUSE_STATUSBAR
            if (m_RelatedStatusBarIndex != -1)
            {
                wxString msg = _("Loading : ") + location;
                SetHTMLStatusText(msg);
                Refresh(false);
            }
#endif // wxUSE_STATUSBAR

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
            if (src.empty())
            {
                if (m_DefaultFilter == nullptr) m_DefaultFilter = GetDefaultFilter();
                src = m_DefaultFilter->ReadFile(*f);
            }

            m_FS->ChangePathTo(f->GetLocation());
            rt_val = SetPage(src);
            m_OpenedPage = f->GetLocation();
            if (!f->GetAnchor().empty())
            {
                ScrollToAnchor(f->GetAnchor());
            }
            else
            {
                // Without anchor, go to the top of the page, instead of
                // possibly remaining at non-zero scroll position and scrolling
                // the top out of view.
                Scroll(0, 0);
            }

            delete f;

#if wxUSE_STATUSBAR
            if (m_RelatedStatusBarIndex != -1)
            {
                SetHTMLStatusText(_("Done"));
            }
#endif // wxUSE_STATUSBAR
        }
    }

    if (m_HistoryOn) // add this page to history there:
    {
        int c = m_History->size() - (m_HistoryPos + 1);

        if (m_HistoryPos < 0 ||
            (*m_History)[m_HistoryPos].GetPage() != m_OpenedPage ||
            (*m_History)[m_HistoryPos].GetAnchor() != m_OpenedAnchor)
        {
            m_HistoryPos++;

            const auto first = m_History->begin() + m_HistoryPos;
            m_History->erase(first, first + c);

            m_History->emplace_back(m_OpenedPage, m_OpenedAnchor);
        }
    }

    if (m_OpenedPageTitle.empty())
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
        wxLogWarning(_("HTML anchor %s does not exist."), anchor);
        return false;
    }
    else
    {
        // Go to next visible cell in current container, if it exists. This
        // yields a bit better (even though still imperfect) results in that
        // there's better chance of using a suitable cell for upper Y
        // coordinate value. See bug #11406 for additional discussion.
        const wxHtmlCell *c_save = c;
        while ( c && c->IsFormattingCell() )
            c = c->GetNext();
        if ( !c )
            c = c_save;

        int y;

        for (y = 0; c != nullptr; c = c->GetParent()) y += c->GetPosY();
        Scroll(-1, y / wxHTML_SCROLL_STEP);
        m_OpenedAnchor = anchor;
        return true;
    }
}


void wxHtmlWindow::OnSetTitle(const wxString& title)
{
    if (m_RelatedFrame)
    {
        wxString tit;
        tit.Printf(m_TitleFormat, title);
        m_RelatedFrame->SetTitle(tit);
    }
    m_OpenedPageTitle = title;
}


void wxHtmlWindow::CreateLayout()
{
    // ShowScrollbars() results in size change events -- and thus a nested
    // CreateLayout() call -- on some platforms. Ignore nested calls, toplevel
    // CreateLayout() will do the right thing eventually.
    static wxRecursionGuardFlag s_flagReentrancy;
    wxRecursionGuard guard(s_flagReentrancy);
    if ( guard.IsInside() )
        return;

    if (!m_Cell)
        return;

    if ( HasFlag(wxHW_SCROLLBAR_NEVER) )
    {
        m_Cell->Layout(GetClientSize().GetWidth());
    }
    else // Do show scrollbars if necessary.
    {
        // Get client width if a vertical scrollbar is shown
        // (which is usually the case).
        ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS);
        const int widthWithVScrollbar = GetClientSize().GetWidth();

        // Let wxScrolledWindow decide whether it needs to show the vertical
        // scrollbar for the given contents size.
        ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);
        m_Cell->Layout(widthWithVScrollbar);
        SetVirtualSize(m_Cell->GetWidth(), m_Cell->GetHeight());

        // Check if the vertical scrollbar was hidden.
        const int newClientWidth = GetClientSize().GetWidth();
        if ( newClientWidth != widthWithVScrollbar )
        {
            m_Cell->Layout(newClientWidth);
            SetVirtualSize(m_Cell->GetWidth(), m_Cell->GetHeight());
        }
    }
}

#if wxUSE_CONFIG
void wxHtmlWindow::ReadCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;
    int p_fontsizes[7];
    wxString p_fff, p_ffn;

    if (!path.empty())
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

    if (!path.empty())
        cfg->SetPath(oldpath);
}



void wxHtmlWindow::WriteCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;

    if (!path.empty())
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

    if (!path.empty())
        cfg->SetPath(oldpath);
}
#endif // wxUSE_CONFIG

bool wxHtmlWindow::HistoryBack()
{
    wxString a, l;

    if (m_HistoryPos < 1) return false;

    // store scroll position into history item:
    int x, y;
    GetViewStart(&x, &y);
    (*m_History)[m_HistoryPos].SetPos(y);

    // go to previous position:
    m_HistoryPos--;

    l = (*m_History)[m_HistoryPos].GetPage();
    a = (*m_History)[m_HistoryPos].GetAnchor();
    m_HistoryOn = false;
    m_tmpCanDrawLocks++;
    if (a.empty()) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = true;
    m_tmpCanDrawLocks--;
    Scroll(0, (*m_History)[m_HistoryPos].GetPos());
    Refresh();
    return true;
}

bool wxHtmlWindow::HistoryCanBack()
{
    if (m_HistoryPos < 1) return false;
    return true ;
}


bool wxHtmlWindow::HistoryForward()
{
    wxString a, l;

    if (m_HistoryPos == -1) return false;
    if (m_HistoryPos >= (int)m_History->size() - 1)return false;

    m_OpenedPage.clear(); // this will disable adding new entry into history in LoadPage()

    m_HistoryPos++;
    l = (*m_History)[m_HistoryPos].GetPage();
    a = (*m_History)[m_HistoryPos].GetAnchor();
    m_HistoryOn = false;
    m_tmpCanDrawLocks++;
    if (a.empty()) LoadPage(l);
    else LoadPage(l + wxT("#") + a);
    m_HistoryOn = true;
    m_tmpCanDrawLocks--;
    Scroll(0, (*m_History)[m_HistoryPos].GetPos());
    Refresh();
    return true;
}

bool wxHtmlWindow::HistoryCanForward()
{
    if (m_HistoryPos == -1) return false;
    if (m_HistoryPos >= (int)m_History->size() - 1)return false;
    return true ;
}


void wxHtmlWindow::HistoryClear()
{
    m_History->clear();
    m_HistoryPos = -1;
}

void wxHtmlWindow::AddProcessor(wxHtmlProcessor *processor)
{
    if (!m_Processors)
    {
        m_Processors = new wxHtmlProcessorList;
    }

    std::unique_ptr<wxHtmlProcessor> processorPtr{processor};

    wxHtmlProcessorList::iterator node;

    for (node = m_Processors->begin(); node != m_Processors->end(); ++node)
    {
        if (processor->GetPriority() > (*node)->GetPriority())
        {
            m_Processors->insert(node, std::move(processorPtr));
            return;
        }
    }
    m_Processors->push_back(std::move(processorPtr));
}

/*static */ void wxHtmlWindow::AddGlobalProcessor(wxHtmlProcessor *processor)
{
    if (!m_GlobalProcessors)
    {
        m_GlobalProcessors = new wxHtmlProcessorList;
    }

    std::unique_ptr<wxHtmlProcessor> processorPtr{processor};

    wxHtmlProcessorList::iterator node;

    for (node = m_GlobalProcessors->begin(); node != m_GlobalProcessors->end(); ++node)
    {
        if (processor->GetPriority() > (*node)->GetPriority())
        {
            m_GlobalProcessors->insert(node, std::move(processorPtr));
            return;
        }
    }
    m_GlobalProcessors->push_back(std::move(processorPtr));
}



void wxHtmlWindow::AddFilter(wxHtmlFilter *filter)
{
    m_Filters.Append(filter);
}


bool wxHtmlWindow::IsSelectionEnabled() const
{
#if wxUSE_CLIPBOARD
    return !HasFlag(wxHW_NO_SELECTION);
#else
    return false;
#endif
}


#if wxUSE_CLIPBOARD
wxString wxHtmlWindow::DoSelectionToText(wxHtmlSelection *sel)
{
    if ( !sel )
        return wxEmptyString;

    wxClientDC dc(this);
    wxString text;

    wxHtmlTerminalCellsInterator i(sel->GetFromCell(), sel->GetToCell());
    const wxHtmlCell *prev = nullptr;

    while ( i )
    {
        // When converting HTML content to plain text, the entire paragraph
        // (container in wxHTML) goes on single line. A new paragraph (that
        // should go on its own line) has its own container. Therefore, the
        // simplest way of detecting where to insert newlines in plain text
        // is to check if the parent container changed -- if it did, we moved
        // to a new paragraph.
        if ( prev && prev->GetParent() != i->GetParent() )
            text << '\n';

        // NB: we don't need to pass the selection to ConvertToText() in the
        //     middle of the selected text; it's only useful when only part of
        //     a cell is selected
        text << i->ConvertToText(sel);

        prev = *i;
        ++i;
    }
    return text;
}

wxString wxHtmlWindow::ToText()
{
    if (m_Cell)
    {
        wxHtmlSelection sel;
        sel.Set(m_Cell->GetFirstTerminal(), m_Cell->GetLastTerminal());
        return DoSelectionToText(&sel);
    }
    else
        return wxEmptyString;
}

#endif // wxUSE_CLIPBOARD

bool wxHtmlWindow::CopySelection(ClipboardType t)
{
#if wxUSE_CLIPBOARD
    if ( m_selection )
    {
#if defined(__UNIX__) && !defined(__WXMAC__)
        wxTheClipboard->UsePrimarySelection(t == Primary);
#else // !__UNIX__
        // Primary selection exists only under X11, so don't do anything under
        // the other platforms when we try to access it
        //
        // TODO: this should be abstracted at wxClipboard level!
        if ( t == Primary )
            return false;
#endif // __UNIX__/!__UNIX__

        if ( wxTheClipboard->Open() )
        {
            const wxString txt(SelectionToText());
            wxTheClipboard->SetData(new wxTextDataObject(txt));
            wxTheClipboard->Close();
            wxLogTrace(wxT("wxhtmlselection"),
                       _("Copied to clipboard:\"%s\""), txt);

            return true;
        }
    }
#else
    wxUnusedVar(t);
#endif // wxUSE_CLIPBOARD

    return false;
}


void wxHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    wxHtmlLinkEvent event(GetId(), link);
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event))
    {
        // the default behaviour is to load the URL in this window
        const wxMouseEvent *e = event.GetLinkInfo().GetEvent();
        if (e == nullptr || e->LeftUp())
            LoadPage(event.GetLinkInfo().GetHref());
    }
}

void wxHtmlWindow::DoEraseBackground(wxDC& dc)
{
    wxBitmap bmp = m_bmpBg.GetBitmapFor(this);

    // if we don't have any background bitmap we just fill it with background
    // colour and we also must do it if the background bitmap is not fully
    // opaque as otherwise junk could be left there
    if ( !bmp.IsOk() || bmp.GetMask() )
    {
        dc.SetBackground(GetBackgroundColour());
        dc.Clear();
    }

    if ( bmp.IsOk() )
    {
        // draw the background bitmap tiling it over the entire window area
        const wxSize sz = GetVirtualSize();
        const wxSize sizeBmp(bmp.GetLogicalWidth(), bmp.GetLogicalHeight());
        for ( wxCoord x = 0; x < sz.x; x += sizeBmp.x )
        {
            for ( wxCoord y = 0; y < sz.y; y += sizeBmp.y )
            {
                dc.DrawBitmap(bmp, x, y, true /* use mask */);
            }
        }
    }
}

void wxHtmlWindow::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // We never get real erase background events as we changed our background
    // style to wxBG_STYLE_PAINT in our ctor so the only time when we get here
    // is when an artificial wxEraseEvent is generated by our own OnPaint()
    // below. This handler only exists to stop the event from propagating
    // downwards to wxWindow which may erase the background itself when it gets
    // it in some ports (currently this happens in wxUniv), so we simply stop
    // processing here and set a special flag allowing OnPaint() to see that
    // the event hadn't been really processed.
    m_isBgReallyErased = false;
}

void wxHtmlWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dcPaint(this);

    if (m_tmpCanDrawLocks > 0 || m_Cell == nullptr)
        return;

    int x, y;
    GetViewStart(&x, &y);
    const wxRect rect = GetUpdateRegion().GetBox();
    const wxSize sz = GetClientSize();

    // Don't bother drawing the empty window.
    if ( sz.x == 0 || sz.y == 0 )
        return;

    // set up the DC we're drawing on: if the window is already double buffered
    // we do it directly on wxPaintDC, otherwise we allocate a backing store
    // buffer and compose the drawing there and then blit it to screen all at
    // once
    wxDC *dc;
    wxMemoryDC dcm;
    if ( IsDoubleBuffered() )
    {
        dc = &dcPaint;
    }
    else // window is not double buffered by the system, do it ourselves
    {
        if ( !m_backBuffer.IsOk() )
            m_backBuffer.Create(sz.x, sz.y);
        dcm.SelectObject(m_backBuffer);
        dc = &dcm;
    }

    dc->GetImpl()->SetWindow(this);
    PrepareDC(*dc);

    // Erase the background: for compatibility, we must generate the event to
    // allow the user-defined handlers to do it, hence this hack with sending
    // an artificial wxEraseEvent to trigger the execution of such handlers.
    wxEraseEvent eraseEvent(GetId(), dc);
    eraseEvent.SetEventObject(this);

    // Hack inside a hack: the background wasn't really erased if our own
    // OnEraseBackground() was executed, so we need to check for the flag set
    // by it whenever it's called.
    m_isBgReallyErased = true; // Initially assume it wasn't.
    if ( !ProcessWindowEvent(eraseEvent) || !m_isBgReallyErased )
    {
        // erase background ourselves
        DoEraseBackground(*dc);
    }
    //else: background erased by the user-defined handler


    // draw the HTML window contents
    dc->SetMapMode(wxMM_TEXT);
    dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc->SetLayoutDirection(GetLayoutDirection());

    wxHtmlRenderingInfo rinfo;
    wxDefaultHtmlRenderingStyle rstyle(this);
    rinfo.SetSelection(m_selection);
    rinfo.SetStyle(&rstyle);
    m_Cell->Draw(*dc, 0, 0,
                 y * wxHTML_SCROLL_STEP + rect.GetTop(),
                 y * wxHTML_SCROLL_STEP + rect.GetBottom(),
                 rinfo);

#ifdef DEBUG_HTML_SELECTION
    {
    int xc, yc, x, y;
    wxGetMousePosition(&xc, &yc);
    ScreenToClient(&xc, &yc);
    CalcUnscrolledPosition(xc, yc, &x, &y);
    wxHtmlCell *at = m_Cell->FindCellByPos(x, y);
    wxHtmlCell *before =
        m_Cell->FindCellByPos(x, y, wxHTML_FIND_NEAREST_BEFORE);
    wxHtmlCell *after =
        m_Cell->FindCellByPos(x, y, wxHTML_FIND_NEAREST_AFTER);

    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    dc->SetPen(*wxBLACK_PEN);
    if (at)
        dc->DrawRectangle(at->GetAbsPos(),
                          wxSize(at->GetWidth(),at->GetHeight()));
    dc->SetPen(*wxGREEN_PEN);
    if (before)
        dc->DrawRectangle(before->GetAbsPos().x+1, before->GetAbsPos().y+1,
                          before->GetWidth()-2,before->GetHeight()-2);
    dc->SetPen(*wxRED_PEN);
    if (after)
        dc->DrawRectangle(after->GetAbsPos().x+2, after->GetAbsPos().y+2,
                          after->GetWidth()-4,after->GetHeight()-4);
    }
#endif // DEBUG_HTML_SELECTION

    if ( dc != &dcPaint )
    {
        dc->SetDeviceOrigin(0,0);
        dcPaint.Blit(0, rect.GetTop(),
                     sz.x, rect.GetBottom() - rect.GetTop() + 1,
                     dc,
                     0, rect.GetTop());
    }
}

namespace
{

// Returns true if leftCell is an ancestor of rightCell.
bool IsAncestor(const wxHtmlCell* leftCell, const wxHtmlCell* rightCell)
{
    for ( const wxHtmlCell* parent = rightCell->GetParent();
          parent; parent = parent->GetParent() )
    {
        if ( leftCell == parent )
            return true;
    }
    return false;
}

// Returns minimum bounding rectangle of all the cells between fromCell
// and toCell, inclusive.
wxRect GetBoundingRect(const wxHtmlCell* const fromCell,
                       const wxHtmlCell* const toCell)
{
    wxCHECK_MSG(fromCell || toCell, wxRect(), "At least one cell is required");

    // Check if we have only one cell or the cells are equal.
    if ( !fromCell )
        return toCell->GetRect();
    else if ( !toCell || fromCell == toCell )
        return fromCell->GetRect();

    // Check if one of the cells is an ancestor of the other.
    if ( IsAncestor(fromCell, toCell) )
        return fromCell->GetRect();
    else if ( IsAncestor(toCell, fromCell) )
        return toCell->GetRect();

    // Combine MBRs, starting with the fromCell.
    wxRect boundingRect = fromCell->GetRect();

    // For each subtree toward the lowest common ancestor,
    // combine MBRs until the (subtree of) toCell is reached.
    for ( const wxHtmlCell *startCell = fromCell,
                           *parent = fromCell->GetParent();
          parent;
          startCell = parent, parent = parent->GetParent() )
    {
        if ( IsAncestor(parent, toCell) )
        {
            // Combine all the cells up to the toCell or its subtree.
            for ( const wxHtmlCell* nextCell = startCell->GetNext();
                  nextCell;
                  nextCell = nextCell->GetNext() )
            {
                if ( nextCell == toCell )
                    return boundingRect.Union(toCell->GetRect());

                if ( IsAncestor(nextCell, toCell) )
                {
                    return boundingRect.Union(GetBoundingRect(
                        nextCell->GetFirstTerminal(), toCell));
                }

                boundingRect.Union(nextCell->GetRect());
            }

            wxFAIL_MSG("Unexpected: toCell is not reachable from the fromCell");
            return GetBoundingRect(toCell, fromCell);
        }
        else
        {
            // Combine rest of current container.
            for ( const wxHtmlCell* nextCell = startCell->GetNext();
                  nextCell;
                  nextCell = nextCell->GetNext() )
            {
                boundingRect.Union(nextCell->GetRect());
            }
        }
    }

    wxFAIL_MSG("The cells have no common ancestor");
    return wxRect();
}

} // namespace

void wxHtmlWindow::OnFocusEvent(wxFocusEvent& event)
{
    event.Skip();

    // Redraw selection, because its background colour depends on
    // whether the window has keyboard focus or not.

    if ( !m_selection || m_selection->IsEmpty() )
        return;

    const wxHtmlCell* fromCell = m_selection->GetFromCell();
    const wxHtmlCell* toCell = m_selection->GetToCell();
    wxCHECK_RET(fromCell || toCell,
                "Unexpected: selection is set but cells are not");

    wxRect boundingRect = GetBoundingRect(fromCell, toCell);

    boundingRect = wxRect
    (
        CalcScrolledPosition(boundingRect.GetTopLeft()),
        CalcScrolledPosition(boundingRect.GetBottomRight())
    );

    RefreshRect(boundingRect);
}


void wxHtmlWindow::OnSize(wxSizeEvent& event)
{
    event.Skip();

    m_backBuffer = wxNullBitmap;

    CreateLayout();

    // Recompute selection if necessary:
    if ( m_selection )
    {
        m_selection->Set(m_selection->GetFromCell(),
                         m_selection->GetToCell());
        m_selection->ClearFromToCharacterPos();
    }

    Refresh();
}

void wxHtmlWindow::OnDPIChanged(wxDPIChangedEvent& WXUNUSED(event))
{
    wxBitmapBundle bmpBg = m_bmpBg;
    DoSetPage(*(m_Parser->GetSource()));
    SetBackgroundImage(bmpBg);
}

void wxHtmlWindow::OnMouseMove(wxMouseEvent& WXUNUSED(event))
{
    wxHtmlWindowMouseHelper::HandleMouseMoved();
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

            (void) CopySelection();
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
            m_tmpSelFromCell = nullptr;

            CaptureMouse();
        }
    }
#endif // wxUSE_CLIPBOARD

    // in any case, let the default handler set focus to this window
    event.Skip();
}

void wxHtmlWindow::OnMouseUp(wxMouseEvent& event)
{
#if wxUSE_CLIPBOARD
    if ( m_makingSelection )
    {
        ReleaseMouse();
        m_makingSelection = false;

        // if m_selection=nullptr, the user didn't move the mouse far enough from
        // starting point and the mouse up event is part of a click, the user
        // is not selecting text:
        if ( m_selection )
        {
            CopySelection(Primary);

            // we don't want mouse up event that ended selecting to be
            // handled as mouse click and e.g. follow hyperlink:
            return;
        }
    }
#endif // wxUSE_CLIPBOARD

    wxPoint pos = CalcUnscrolledPosition(event.GetPosition());
    if ( !wxHtmlWindowMouseHelper::HandleMouseClick(m_Cell, pos, event) )
        event.Skip();
}

#if wxUSE_CLIPBOARD
void wxHtmlWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    if ( !m_makingSelection )
        return;

    // discard the selecting operation
    m_makingSelection = false;
    wxDELETE(m_selection);
    m_tmpSelFromCell = nullptr;
    Refresh();
}
#endif // wxUSE_CLIPBOARD


void wxHtmlWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_Cell != nullptr && DidMouseMove())
    {
#ifdef DEBUG_HTML_SELECTION
        Refresh();
#endif
        int xc, yc, x, y;
        wxGetMousePosition(&xc, &yc);
        ScreenToClient(&xc, &yc);
        CalcUnscrolledPosition(xc, yc, &x, &y);

        wxHtmlCell *cell = m_Cell->FindCellByPos(x, y);

        // handle selection update:
        if ( m_makingSelection )
        {
            if ( !m_tmpSelFromCell )
                m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y);

            // NB: a trick - we adjust selFromPos to be upper left or bottom
            //     right corner of the first cell of the selection depending
            //     on whether the mouse is moving to the right or to the left.
            //     This gives us more "natural" behaviour when selecting
            //     a line (specifically, first cell of the next line is not
            //     included if you drag selection from left to right over
            //     entire line):
            wxPoint dirFromPos;
            if ( !m_tmpSelFromCell )
            {
                dirFromPos = m_tmpSelFromPos;
            }
            else
            {
                dirFromPos = m_tmpSelFromCell->GetAbsPos();
                if ( x < m_tmpSelFromPos.x )
                {
                    dirFromPos.x += m_tmpSelFromCell->GetWidth();
                    dirFromPos.y += m_tmpSelFromCell->GetHeight();
                }
            }
            bool goingDown = dirFromPos.y < y ||
                             (dirFromPos.y == y && dirFromPos.x < x);

            // determine selection span:
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
                                                 wxHTML_FIND_NEAREST_BEFORE);
                    if (!selcell)
                        selcell = m_Cell->GetLastTerminal();
                }
                else
                {
                    selcell = m_Cell->FindCellByPos(x, y,
                                                 wxHTML_FIND_NEAREST_AFTER);
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
                                         wxPoint(x,y), selcell);
                    }
                    else
                    {
                        m_selection->Set(wxPoint(x,y), selcell,
                                         m_tmpSelFromPos, m_tmpSelFromCell);
                    }
                    m_selection->ClearFromToCharacterPos();
                    Refresh();
                }
            }
        }

        // handle cursor and status bar text changes:

        // NB: because we're passing in 'cell' and not 'm_Cell' (so that the
        //     leaf cell lookup isn't done twice), we need to adjust the
        //     position for the new root:
        wxPoint posInCell(x, y);
        if (cell)
            posInCell -= cell->GetAbsPos();
        wxHtmlWindowMouseHelper::HandleIdle(cell, posInCell);
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

                //wxFAIL_MSG( wxT("can't understand where has mouse gone") );

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
    if ( IsSelectionEnabled() && event.GetModifiers() == wxMOD_CONTROL &&
         (event.GetKeyCode() == 'C' || event.GetKeyCode() == WXK_INSERT) )
    {
        wxClipboardTextEvent evt(wxEVT_TEXT_COPY, GetId());
        evt.SetEventObject(this);
        ProcessWindowEvent(evt);
    }
    else
    {
        event.Skip();
    }
}

void wxHtmlWindow::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    (void) CopySelection();
}

void wxHtmlWindow::OnClipboardEvent(wxClipboardTextEvent& WXUNUSED(event))
{
    (void) CopySelection();
}

void wxHtmlWindow::OnDoubleClick(wxMouseEvent& event)
{
    // select word under cursor:
    if ( IsSelectionEnabled() )
    {
        SelectWord(CalcUnscrolledPosition(event.GetPosition()));

        (void) CopySelection(Primary);

        m_lastDoubleClick = wxGetLocalTimeMillis();
    }
    else
        event.Skip();
}

void wxHtmlWindow::SelectWord(const wxPoint& pos)
{
    if ( m_Cell )
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
}

void wxHtmlWindow::SelectLine(const wxPoint& pos)
{
    if ( m_Cell )
    {
        wxHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);
        if ( cell )
        {
            // We use following heuristic to find a "line": let the line be all
            // cells in same container as the cell under mouse cursor that are
            // neither completely above nor completely below the clicked cell
            // (i.e. are likely to be words positioned on same line of text).

            int y1 = cell->GetAbsPos().y;
            int y2 = y1 + cell->GetHeight();
            int y;
            const wxHtmlCell *c;
            const wxHtmlCell *before = nullptr;
            const wxHtmlCell *after = nullptr;

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
                    before = nullptr;
            }
            if ( !before )
                before = cell;

            delete m_selection;
            m_selection = new wxHtmlSelection();
            m_selection->Set(before, after);

            Refresh();
        }
    }
}

void wxHtmlWindow::SelectAll()
{
    if ( m_Cell )
    {
        delete m_selection;
        m_selection = new wxHtmlSelection();
        m_selection->Set(m_Cell->GetFirstTerminal(), m_Cell->GetLastTerminal());
        Refresh();
    }
}

#endif // wxUSE_CLIPBOARD



wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlProcessor, wxObject);

wxBEGIN_PROPERTIES_TABLE(wxHtmlWindow)
/*
    TODO PROPERTIES
        style , wxHW_SCROLLBAR_AUTO
        borders , (dimension)
        url , string
        htmlcode , string
*/
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxHtmlWindow)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxHtmlWindow , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size , long , WindowStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxHtmlWindow, wxScrolledWindow, "wx/html/htmlwin.h");

wxBEGIN_EVENT_TABLE(wxHtmlWindow, wxScrolledWindow)
    EVT_SIZE(wxHtmlWindow::OnSize)
    EVT_DPI_CHANGED(wxHtmlWindow::OnDPIChanged)
    EVT_LEFT_DOWN(wxHtmlWindow::OnMouseDown)
    EVT_LEFT_UP(wxHtmlWindow::OnMouseUp)
    EVT_RIGHT_UP(wxHtmlWindow::OnMouseUp)
    EVT_MOTION(wxHtmlWindow::OnMouseMove)
    EVT_PAINT(wxHtmlWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxHtmlWindow::OnEraseBackground)
    EVT_SET_FOCUS(wxHtmlWindow::OnFocusEvent)
    EVT_KILL_FOCUS(wxHtmlWindow::OnFocusEvent)
#if wxUSE_CLIPBOARD
    EVT_LEFT_DCLICK(wxHtmlWindow::OnDoubleClick)
    EVT_ENTER_WINDOW(wxHtmlWindow::OnMouseEnter)
    EVT_LEAVE_WINDOW(wxHtmlWindow::OnMouseLeave)
    EVT_MOUSE_CAPTURE_LOST(wxHtmlWindow::OnMouseCaptureLost)
    EVT_KEY_UP(wxHtmlWindow::OnKeyUp)
    EVT_MENU(wxID_COPY, wxHtmlWindow::OnCopy)
    EVT_TEXT_COPY(wxID_ANY, wxHtmlWindow::OnClipboardEvent)
#endif // wxUSE_CLIPBOARD
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// wxHtmlWindowInterface implementation in wxHtmlWindow
//-----------------------------------------------------------------------------

void wxHtmlWindow::SetHTMLWindowTitle(const wxString& title)
{
    OnSetTitle(title);
}

void wxHtmlWindow::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
    OnLinkClicked(link);
}

wxHtmlOpeningStatus wxHtmlWindow::OnHTMLOpeningURL(wxHtmlURLType type,
                                                   const wxString& url,
                                                   wxString *redirect) const
{
    return OnOpeningURL(type, url, redirect);
}

wxPoint wxHtmlWindow::HTMLCoordsToWindow(wxHtmlCell *WXUNUSED(cell),
                                         const wxPoint& pos) const
{
    return CalcScrolledPosition(pos);
}

wxWindow* wxHtmlWindow::GetHTMLWindow()
{
    return this;
}

wxColour wxHtmlWindow::GetHTMLBackgroundColour() const
{
    return GetBackgroundColour();
}

void wxHtmlWindow::SetHTMLBackgroundColour(const wxColour& clr)
{
    SetBackgroundColour(clr);
}

void wxHtmlWindow::SetHTMLBackgroundImage(const wxBitmapBundle& bmpBg)
{
    SetBackgroundImage(bmpBg);
}

void wxHtmlWindow::SetHTMLStatusText(const wxString& text)
{
#if wxUSE_STATUSBAR
    if (m_RelatedStatusBarIndex != -1)
    {
        if (m_RelatedStatusBar)
        {
            m_RelatedStatusBar->SetStatusText(text, m_RelatedStatusBarIndex);
        }
        else if (m_RelatedFrame)
        {
            m_RelatedFrame->SetStatusText(text, m_RelatedStatusBarIndex);
        }
    }
#else
    wxUnusedVar(text);
#endif // wxUSE_STATUSBAR
}

/*static*/
wxCursor wxHtmlWindow::GetDefaultHTMLCursor(HTMLCursor type)
{
    switch (type)
    {
        case HTMLCursor_Link:
            if ( !ms_cursorLink )
                ms_cursorLink = new wxCursor(wxCURSOR_HAND);
            return *ms_cursorLink;

        case HTMLCursor_Text:
            if ( !ms_cursorText )
                ms_cursorText = new wxCursor(wxCURSOR_IBEAM);
            return *ms_cursorText;

        case HTMLCursor_Default:
        default:
            if ( !ms_cursorDefault )
                ms_cursorDefault = new wxCursor(wxCURSOR_ARROW);
            return *ms_cursorDefault;
    }
}

wxCursor wxHtmlWindow::GetHTMLCursor(HTMLCursor type) const
{
    return GetDefaultHTMLCursor(type);
}

/*static*/
void wxHtmlWindow::SetDefaultHTMLCursor(HTMLCursor type, const wxCursor& cursor)
{
    switch (type)
    {
        case HTMLCursor_Link:
            delete ms_cursorLink;
            ms_cursorLink = new wxCursor(cursor);
            return;

        case HTMLCursor_Text:
            delete ms_cursorText;
            ms_cursorText = new wxCursor(cursor);
            return;

        case HTMLCursor_Default:
        default:
            delete ms_cursorText;
            ms_cursorDefault = new wxCursor(cursor);
    }
}

//-----------------------------------------------------------------------------
// wxHtmlWinModule
//-----------------------------------------------------------------------------

// A module to allow initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxHtmlWinModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxHtmlWinModule);
public:
    wxHtmlWinModule() : wxModule() {}
    bool OnInit() override { return true; }
    void OnExit() override { wxHtmlWindow::CleanUpStatics(); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxHtmlWinModule, wxModule);


// This hack forces the linker to always link in m_* files
// (wxHTML doesn't work without handlers from these files)
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

#endif // wxUSE_HTML
