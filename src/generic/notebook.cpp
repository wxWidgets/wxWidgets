///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include  <wx/string.h>
#include  <wx/log.h>
#include  <wx/settings.h>
#include  <wx/generic/imaglist.h>
#include  <wx/generic/notebook.h>
#include  <wx/dcclient.h>

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)
    EVT_SIZE(wxNotebook::OnSize)
    EVT_PAINT(wxNotebook::OnPaint)
    EVT_MOUSE_EVENTS(wxNotebook::OnMouseEvent)
    EVT_SET_FOCUS(wxNotebook::OnSetFocus)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
//    EVT_IDLE(wxNotebook::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxNotebook::Init()
{
    m_tabView = (wxNotebookTabView*) NULL;
    m_pImageList = NULL;
    m_nSelection = -1;
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

    m_windowId = id == -1 ? NewControlId() : id;

    // It's like a normal window...
    if (!wxWindow::Create(parent, id, pos, size, style|wxNO_BORDER, name))
        return FALSE;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

    SetTabView(new wxNotebookTabView(this));

    return TRUE;
}

// dtor
wxNotebook::~wxNotebook()
{
    delete m_tabView;
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------
int wxNotebook::GetPageCount() const
{
    return m_aPages.Count();
}

int wxNotebook::GetRowCount() const
{
    // TODO
    return 0;
}

int wxNotebook::SetSelection(int nPage)
{
    if (nPage == -1)
      return 0;

    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* pPage = GetPage(nPage);

    m_tabView->SetTabSelection((int) (long) pPage);

    // TODO
    return 0;
}

void wxNotebook::AdvanceSelection(bool bForward)
{
    int nSel = GetSelection();
    int nMax = GetPageCount() - 1;
    if ( bForward )
        SetSelection(nSel == nMax ? 0 : nSel + 1);
    else
        SetSelection(nSel == 0 ? nMax : nSel - 1);
}

bool wxNotebook::SetPageText(int nPage, const wxString& strText)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* page = GetPage(nPage);
    if (page)
    {
        m_tabView->SetTabText((int) (long) page, strText);
        Refresh();
        return TRUE;
    }

    return FALSE;
}

wxString wxNotebook::GetPageText(int nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage* page = ((wxNotebook*)this)->GetPage(nPage);
    if (page)
        return m_tabView->GetTabText((int) (long) page);
    else
        return wxEmptyString;
}

int wxNotebook::GetPageImage(int nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return 0;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return FALSE;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{ 
    m_pImageList = imageList;
    // TODO
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook and delete it
bool wxNotebook::DeletePage(int nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), FALSE );

    if (m_nSelection != -1)
    {
        m_aPages[m_nSelection]->Show(FALSE);
        m_aPages[m_nSelection]->Lower();
    }

    wxNotebookPage* pPage = GetPage(nPage);
    m_tabView->RemoveTab((int) (long) pPage);

    delete m_aPages[nPage];
    m_aPages.Remove(nPage);

    if (m_aPages.GetCount() == 0)
    {
      m_nSelection = -1;
      m_tabView->SetTabSelection(-1, FALSE);
    }
    else if (m_nSelection > -1)
    {
      m_nSelection = -1;
      m_tabView->SetTabSelection((int) (long) GetPage(0), FALSE);
      if (m_nSelection != 0)
        ChangePage(-1, 0);
    }

    RefreshLayout(FALSE);

    return TRUE;
}

bool wxNotebook::DeletePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return DeletePage(pagePos);
    else
        return FALSE;
}

// remove one page from the notebook
bool wxNotebook::RemovePage(int nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), FALSE );

    m_aPages[nPage]->Show(FALSE);
    //    m_aPages[nPage]->Lower();

    wxNotebookPage* pPage = GetPage(nPage);
    m_tabView->RemoveTab((int) (long) pPage);

    m_aPages.Remove(nPage);

    if (m_aPages.GetCount() == 0)
    {
      m_nSelection = -1;
      m_tabView->SetTabSelection(-1, TRUE);
    }
    else if (m_nSelection > -1)
    {
      // Only change the selection if the page we
      // deleted was the selection.
      if (nPage == m_nSelection)
      {
         m_nSelection = -1;
         // Select the first tab. Generates a ChangePage.
         m_tabView->SetTabSelection((int) (long) GetPage(0), TRUE);
      }
      else
      {
	// We must adjust which tab we think is selected.
        // If greater than the page we deleted, it must be moved down
        // a notch.
        if (m_nSelection > nPage)
          m_nSelection -- ;
      }
    }

    RefreshLayout(FALSE);

    return TRUE;
}

bool wxNotebook::RemovePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return RemovePage(pagePos);
    else
        return FALSE;
}

// Find the position of the wxNotebookPage, -1 if not found.
int wxNotebook::FindPagePosition(wxNotebookPage* page) const
{
    int nPageCount = GetPageCount();
    int nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        if (m_aPages[nPage] == page)
            return nPage;
    return -1;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    m_tabView->ClearTabs(TRUE);

    int nPageCount = GetPageCount();
    int nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        delete m_aPages[nPage];

    m_aPages.Clear();

    return TRUE;
}

// add a page to the notebook
bool wxNotebook::AddPage(wxNotebookPage *pPage,
                         const wxString& strText,
                         bool bSelect,
                         int imageId)
{
    return InsertPage(GetPageCount(), pPage, strText, bSelect, imageId);
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxASSERT( pPage != NULL );
    wxCHECK( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), FALSE );

    m_tabView->AddTab((int) (long) pPage, strText);
    if (!bSelect)
      pPage->Show(FALSE);

    // save the pointer to the page
    m_aPages.Insert(pPage, nPage);

    if (bSelect)
    {
        // This will cause ChangePage to be called, via OnSelPage
        m_tabView->SetTabSelection((int) (long) pPage, TRUE);
    }

    // some page must be selected: either this one or the first one if there is
    // still no selection
    if ( m_nSelection == -1 )
      ChangePage(-1, 0);

    RefreshLayout(FALSE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxNotebook::OnSize(wxSizeEvent& event)
{
    static bool s_bFirstTime = TRUE;
    if ( s_bFirstTime ) {
        // TODO: any first-time-size processing.
        s_bFirstTime = FALSE;
    }

    RefreshLayout();

    // Processing continues to next OnSize
    event.Skip();
}

// This was supposed to cure the non-display of the notebook
// until the user resizes the window.
// What's going on?
void wxNotebook::OnIdle(wxIdleEvent& event)
{
    static bool s_bFirstTime = TRUE;
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
      s_bFirstTime = FALSE;
    }
    event.Skip();
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

        m_tabView->Layout();

        // Need to do it a 2nd time to get the tab height with
        // the new view width, since changing the view width changes the
        // tab layout.
        tabHeight = m_tabView->GetTotalTabHeight();
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y ;
  
        m_tabView->SetViewRect(rect);

        m_tabView->Layout();

        if (!force && (rect == oldRect))
          return FALSE;

        // fit the notebook page to the tab control's display area

        unsigned int nCount = m_aPages.Count();
        for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
            wxNotebookPage *pPage = m_aPages[nPage];
            if (pPage->IsShown())
            {
                wxRect clientRect = GetAvailableClientSize();
                pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);
                if ( pPage->GetAutoLayout() )
                   pPage->Layout();
            }
        }
        Refresh();
    }
    return TRUE;
}

void wxNotebook::OnSelChange(wxNotebookEvent& event)
{
    // is it our tab control?
    if ( event.GetEventObject() == this )
    {
        if (event.GetSelection() != m_nSelection)
          ChangePage(event.GetOldSelection(), event.GetSelection());
    }

    // we want to give others a chance to process this message as well
    event.Skip();
}

void wxNotebook::OnSetFocus(wxFocusEvent& event)
{
    // set focus to the currently selected page if any
    if ( m_nSelection != -1 )
        m_aPages[m_nSelection]->SetFocus();

    event.Skip();
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        AdvanceSelection(event.GetDirection());
    }
    else {
        // pass to the parent
        if ( GetParent() ) {
            event.SetCurrentFocus(this);
            GetParent()->ProcessEvent(event);
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
    wxControl::SetConstraintSizes(FALSE);
}

bool wxNotebook::DoPhase(int /* nPhase */)
{
    return TRUE;
}

void wxNotebook::Command(wxCommandEvent& event)
{
    wxFAIL_MSG("wxNotebook::Command not implemented");
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
        m_aPages[nOldSel]->Show(FALSE);
        m_aPages[nOldSel]->Lower();
    }

    wxNotebookPage *pPage = m_aPages[nSel];

    wxRect clientRect = GetAvailableClientSize();
    pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);

    pPage->Show(TRUE);
    pPage->Raise();
    pPage->SetFocus();

    Refresh();

    m_nSelection = nSel;
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
 
IMPLEMENT_CLASS(wxNotebookTabView, wxTabView)

wxNotebookTabView::wxNotebookTabView(wxNotebook *notebook, long style): wxTabView(style)
{
  m_notebook = notebook;

  m_notebook->SetTabView(this);

  SetWindow(m_notebook);
}

wxNotebookTabView::~wxNotebookTabView(void)
{
}

// Called when a tab is activated
void wxNotebookTabView::OnTabActivate(int activateId, int deactivateId)
{
  if (!m_notebook)
    return;

// Because of name truncation!
#if defined(__BORLANDC__) && defined(__WIN16__)
  wxNotebookEvent event(wxEVT_COMMAND_NB_PAGE_CHANGED, m_notebook->GetId());
#else
  wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_notebook->GetId());
#endif

  // Translate from wxTabView's ids (which aren't position-dependent)
  // to wxNotebook's (which are).
  wxNotebookPage* pActive = (wxNotebookPage*) activateId;
  wxNotebookPage* pDeactive = (wxNotebookPage*) deactivateId;

  int activatePos = m_notebook->FindPagePosition(pActive);
  int deactivatePos = m_notebook->FindPagePosition(pDeactive);

  event.SetEventObject(m_notebook);
  event.SetSelection(activatePos);
  event.SetOldSelection(deactivatePos);
  m_notebook->GetEventHandler()->ProcessEvent(event);
}


