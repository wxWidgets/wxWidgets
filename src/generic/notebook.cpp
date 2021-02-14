///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/notebook.cpp
// Purpose:     generic implementation of wxNotebook
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_NOTEBOOK

#include  "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include  "wx/dcclient.h"
    #include  "wx/settings.h"
#endif

#include  "wx/imaglist.h"
#include  "wx/generic/tabg.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxNotebook, wxBookCtrlBase)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, wxNotebook::OnSelChange)
    EVT_SIZE(wxNotebook::OnSize)
    EVT_PAINT(wxNotebook::OnPaint)
    EVT_MOUSE_EVENTS(wxNotebook::OnMouseEvent)
    EVT_SET_FOCUS(wxNotebook::OnSetFocus)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ============================================================================
// Private class
// ============================================================================

WX_DECLARE_HASH_MAP(int, wxNotebookPage*, wxIntegerHash, wxIntegerEqual,
                    wxIntToNotebookPageHashMap);

WX_DECLARE_HASH_MAP(wxNotebookPage*, int, wxPointerHash, wxPointerEqual,
                    wxNotebookPageToIntHashMap);

// This reuses wxTabView to draw the tabs.
class WXDLLEXPORT wxNotebookTabView: public wxTabView
{
    wxDECLARE_DYNAMIC_CLASS(wxNotebookTabView);
public:
    wxNotebookTabView(wxNotebook* notebook, long style = wxTAB_STYLE_DRAW_BOX | wxTAB_STYLE_COLOUR_INTERIOR);
    virtual ~wxNotebookTabView(void);

    // Called when a tab is activated
    virtual void OnTabActivate(int activateId, int deactivateId);
    // Allows vetoing
    virtual bool OnTabPreActivate(int activateId, int deactivateId);

    // map integer ids used by wxTabView to wxNotebookPage pointers
    int GetId(wxNotebookPage *page);
    wxNotebookPage *GetPage(int id) { return m_idToPage[id]; }

protected:
    wxNotebook* m_notebook;

private:
    wxIntToNotebookPageHashMap m_idToPage;
    wxNotebookPageToIntHashMap m_pageToId;
    int m_nextid;
};

static int GetPageId(wxTabView *tabview, wxNotebookPage *page)
{
    return static_cast<wxNotebookTabView*>(tabview)->GetId(page);
}

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxNotebook::Init()
{
    m_tabView = NULL;
    m_selection = -1;
}

// default for dynamic class
wxNotebook::wxNotebook()
{
    Init();
}

// the same arguments as for wxControl
wxNotebook::wxNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();

    Create(parent, id, pos, size, style, name);
}

// Create() function
bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    // base init
    SetName(name);

    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    m_windowId = id == wxID_ANY ? NewControlId() : id;

    if (!wxControl::Create(parent, id, pos, size, style|wxNO_BORDER, wxDefaultValidator, name))
        return false;

    SetTabView(new wxNotebookTabView(this));

    return true;
}

// dtor
wxNotebook::~wxNotebook()
{
    delete m_tabView;
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------
int wxNotebook::GetRowCount() const
{
    // TODO
    return 0;
}

int wxNotebook::SetSelection(size_t nPage)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* pPage = GetPage(nPage);

    m_tabView->SetTabSelection(GetPageId(m_tabView, pPage));

    // TODO
    return 0;
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    // FIXME: currently it does generate events too
    return SetSelection(nPage);
}

#if 0
void wxNotebook::AdvanceSelection(bool bForward)
{
    int nSel = GetSelection();
    int nMax = GetPageCount() - 1;
    if ( bForward )
        SetSelection(nSel == nMax ? 0 : nSel + 1);
    else
        SetSelection(nSel == 0 ? nMax : nSel - 1);
}
#endif

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* page = GetPage(nPage);
    if (page)
    {
        m_tabView->SetTabText(GetPageId(m_tabView, page), strText);
        Refresh();
        return true;
    }

    return false;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* page = ((wxNotebook*)this)->GetPage(nPage);
    if (page)
        return m_tabView->GetTabText(GetPageId(m_tabView, page));
    else
        return wxEmptyString;
}

int wxNotebook::GetPageImage(size_t WXUNUSED_UNLESS_DEBUG(nPage)) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return 0;
}

bool wxNotebook::SetPageImage(size_t WXUNUSED_UNLESS_DEBUG(nPage),
                              int WXUNUSED(nImage))
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return false;
}

// set the size (the same for all pages)
void wxNotebook::SetPageSize(const wxSize& WXUNUSED(size))
{
    // TODO
}

// set the padding between tabs (in pixels)
void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
    // TODO
}

// set the size of the tabs for wxNB_FIXEDWIDTH controls
void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
    // TODO
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook and delete it
bool wxNotebook::DeletePage(size_t nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), false );

    if (m_selection != -1)
    {
        m_pages[m_selection]->Show(false);
        m_pages[m_selection]->Lower();
    }

    wxNotebookPage* pPage = GetPage(nPage);

    m_tabView->RemoveTab(GetPageId(m_tabView, pPage));

    m_pages.erase(m_pages.begin() + nPage);
    delete pPage;

    if (m_pages.empty())
    {
        m_selection = -1;
        m_tabView->SetTabSelection(-1, false);
    }
    else if (m_selection > -1)
    {
        m_selection = -1;

        m_tabView->SetTabSelection(GetPageId(m_tabView, GetPage(0)), false);

        if (m_selection != 0)
            ChangePage(-1, 0);
    }

    RefreshLayout(false);

    return true;
}

bool wxNotebook::DeletePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return DeletePage(pagePos);
    else
        return false;
}

bool wxNotebook::RemovePage(size_t nPage)
{
    return DoRemovePage(nPage) != NULL;
}

// remove one page from the notebook
wxWindow* wxNotebook::DoRemovePage(size_t nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), NULL );

    m_pages[nPage]->Show(false);
    //    m_pages[nPage]->Lower();

    wxNotebookPage* pPage = GetPage(nPage);

    m_tabView->RemoveTab(GetPageId(m_tabView, pPage));

    m_pages.erase(m_pages.begin() + nPage);

    if (m_pages.empty())
    {
      m_selection = -1;
      m_tabView->SetTabSelection(-1, true);
    }
    else if (m_selection > -1)
    {
      // Only change the selection if the page we
      // deleted was the selection.
      if (nPage == (size_t)m_selection)
      {
         m_selection = -1;
         // Select the first tab. Generates a ChangePage.
         m_tabView->SetTabSelection(0, true);
      }
      else
      {
        // We must adjust which tab we think is selected.
        // If greater than the page we deleted, it must be moved down
        // a notch.
        if (size_t(m_selection) > nPage)
          m_selection -- ;
      }
    }

    RefreshLayout(false);

    return pPage;
}

bool wxNotebook::RemovePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return RemovePage(pagePos);
    else
        return false;
}

// Find the position of the wxNotebookPage, -1 if not found.
int wxNotebook::FindPagePosition(wxNotebookPage* page) const
{
    size_t nPageCount = GetPageCount();
    size_t nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        if (m_pages[nPage] == page)
            return nPage;
    return -1;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    m_tabView->ClearTabs(true);

    size_t nPageCount = GetPageCount();
    size_t nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        delete m_pages[nPage];

    m_pages.clear();

    return true;
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int WXUNUSED(imageId))
{
    wxASSERT( pPage != NULL );
    wxCHECK( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), false );

    m_tabView->AddTab(GetPageId(m_tabView, pPage), strText);

    if (!bSelect)
      pPage->Show(false);

    // save the pointer to the page
    m_pages.insert(m_pages.begin() + nPage, pPage);

    if (bSelect)
    {
        // This will cause ChangePage to be called, via OnSelPage

        m_tabView->SetTabSelection(GetPageId(m_tabView, pPage), true);
    }

    // some page must be selected: either this one or the first one if there is
    // still no selection
    if ( m_selection == -1 )
      ChangePage(-1, 0);

    RefreshLayout(false);

    return true;
}

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxNotebook::OnSize(wxSizeEvent& event)
{
    static bool s_bFirstTime = true;
    if ( s_bFirstTime ) {
        // TODO: any first-time-size processing.
        s_bFirstTime = false;
    }

    RefreshLayout();

    // Processing continues to next OnSize
    event.Skip();
}

// This was supposed to cure the non-display of the notebook
// until the user resizes the window.
// What's going on?
void wxNotebook::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

#if 0
    static bool s_bFirstTime = true;
    if ( s_bFirstTime ) {
      /*
      wxSize sz(GetSize());
      sz.x ++;
      SetSize(sz);
      sz.x --;
      SetSize(sz);
      */

      /*
      wxSize sz(GetSize());
      wxSizeEvent sizeEvent(sz, GetId());
      sizeEvent.SetEventObject(this);
      GetEventHandler()->ProcessEvent(sizeEvent);
      Refresh();
      */
      s_bFirstTime = false;
    }
#endif
}

// Implementation: calculate the layout of the view rect
// and resize the children if required
bool wxNotebook::RefreshLayout(bool force)
{
    if (m_tabView)
    {
        wxRect oldRect = m_tabView->GetViewRect();

        int cw, ch;
        GetClientSize(& cw, & ch);

        int tabHeight = m_tabView->GetTotalTabHeight();
        wxRect rect;
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y ;

        m_tabView->SetViewRect(rect);

        m_tabView->LayoutTabs();

        // Need to do it a 2nd time to get the tab height with
        // the new view width, since changing the view width changes the
        // tab layout.
        tabHeight = m_tabView->GetTotalTabHeight();
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y ;

        m_tabView->SetViewRect(rect);

        m_tabView->LayoutTabs();

        if (!force && (rect == oldRect))
          return false;

        // fit the notebook page to the tab control's display area

        size_t nCount = m_pages.size();
        for ( size_t nPage = 0; nPage < nCount; nPage++ ) {
            wxNotebookPage *pPage = m_pages[nPage];
            wxRect clientRect = GetAvailableClientSize();
            if (pPage->IsShown())
            {
                pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);
                if ( pPage->GetAutoLayout() )
                   pPage->Layout();
            }
        }
        Refresh();
    }
    return true;
}

void wxNotebook::OnSelChange(wxBookCtrlEvent& event)
{
    // is it our tab control?
    if ( event.GetEventObject() == this )
    {
        if (event.GetSelection() != m_selection)
          ChangePage(event.GetOldSelection(), event.GetSelection());
    }

    // we want to give others a chance to process this message as well
    event.Skip();
}

void wxNotebook::OnSetFocus(wxFocusEvent& event)
{
    // set focus to the currently selected page if any
    if ( m_selection != -1 )
        m_pages[m_selection]->SetFocus();

    event.Skip();
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() )
    {
        // change pages
        AdvanceSelection(event.GetDirection());
    }
    else {
        // pass to the parent
        if ( GetParent() )
        {
            event.SetCurrentFocus(this);
            GetParent()->ProcessWindowEvent(event);
        }
    }
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

// override these 2 functions to do nothing: everything is done in OnSize

void wxNotebook::SetConstraintSizes(bool /* recurse */)
{
    // don't set the sizes of the pages - their correct size is not yet known
    wxControl::SetConstraintSizes(false);
}

bool wxNotebook::DoPhase(int /* nPhase */)
{
    return true;
}

void wxNotebook::Command(wxCommandEvent& WXUNUSED(event))
{
    wxFAIL_MSG(wxT("wxNotebook::Command not implemented"));
}

// ----------------------------------------------------------------------------
// wxNotebook helper functions
// ----------------------------------------------------------------------------

// hide the currently active panel and show the new one
void wxNotebook::ChangePage(int nOldSel, int nSel)
{
  //  cout << "ChangePage: " << nOldSel << ", " << nSel << "\n";
    wxASSERT( nOldSel != nSel ); // impossible

    if ( nOldSel != -1 ) {
        m_pages[nOldSel]->Show(false);
        m_pages[nOldSel]->Lower();
    }

    wxNotebookPage *pPage = m_pages[nSel];

    wxRect clientRect = GetAvailableClientSize();
    pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);

    Refresh();

    pPage->Show(true);
    pPage->Raise();
    pPage->SetFocus();

    m_selection = nSel;
}

void wxNotebook::OnMouseEvent(wxMouseEvent& event)
{
  if (m_tabView)
    m_tabView->OnEvent(event);
}

void wxNotebook::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if (m_tabView)
        m_tabView->Draw(dc);
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // MBN: since the total tab height is really a function of the
    // width, this should really call
    // GetTotalTabHeightPretendingWidthIs(), but the current
    // implementation will suffice, provided the wxNotebook has been
    // created with a sensible initial width.
    return wxSize( sizePage.x + 12,
                   sizePage.y + m_tabView->GetTotalTabHeight() + 6 + 4 );
}

wxRect wxNotebook::GetAvailableClientSize()
{
    int cw, ch;
    GetClientSize(& cw, & ch);

    int tabHeight = m_tabView->GetTotalTabHeight();

    // TODO: these margins should be configurable.
    wxRect rect;
    rect.x = 6;
    rect.y = tabHeight + 6;
    rect.width = cw - 12;
    rect.height = ch - 4 - rect.y ;

    return rect;
}

/*
 * wxNotebookTabView
 */

wxIMPLEMENT_CLASS(wxNotebookTabView, wxTabView);

wxNotebookTabView::wxNotebookTabView(wxNotebook *notebook, long style)
    : wxTabView(style), m_nextid(1)
{
  m_notebook = notebook;

  m_notebook->SetTabView(this);

  SetWindow(m_notebook);
}

wxNotebookTabView::~wxNotebookTabView(void)
{
}

int wxNotebookTabView::GetId(wxNotebookPage *page)
{
    int& id = m_pageToId[page];

    if (!id)
    {
        id = m_nextid++;
        m_idToPage[id] = page;
    }

    return id;
}

// Called when a tab is activated
void wxNotebookTabView::OnTabActivate(int activateId, int deactivateId)
{
  if (!m_notebook)
    return;

  wxBookCtrlEvent event(wxEVT_NOTEBOOK_PAGE_CHANGED, m_notebook->GetId());

  // Translate from wxTabView's ids (which aren't position-dependent)
  // to wxNotebook's (which are).
  wxNotebookPage* pActive = GetPage(activateId);
  wxNotebookPage* pDeactive = GetPage(deactivateId);

  int activatePos = m_notebook->FindPagePosition(pActive);
  int deactivatePos = m_notebook->FindPagePosition(pDeactive);

  event.SetEventObject(m_notebook);
  event.SetSelection(activatePos);
  event.SetOldSelection(deactivatePos);
  m_notebook->GetEventHandler()->ProcessEvent(event);
}

// Allows Vetoing
bool wxNotebookTabView::OnTabPreActivate(int activateId, int deactivateId)
{
  bool retval = true;

  if (m_notebook)
  {
    wxBookCtrlEvent event(wxEVT_NOTEBOOK_PAGE_CHANGING, m_notebook->GetId());

    // Translate from wxTabView's ids (which aren't position-dependent)
    // to wxNotebook's (which are).
    wxNotebookPage* pActive = GetPage(activateId);
    wxNotebookPage* pDeactive = GetPage(deactivateId);

    int activatePos = m_notebook->FindPagePosition(pActive);
    int deactivatePos = m_notebook->FindPagePosition(pDeactive);

    event.SetEventObject(m_notebook);
    event.SetSelection(activatePos);
    event.SetOldSelection(deactivatePos);
    if (m_notebook->GetEventHandler()->ProcessEvent(event))
    {
      retval = event.IsAllowed();
    }
  }
  return retval;
}

#endif // wxUSE_NOTEBOOK
